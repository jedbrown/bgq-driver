/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2004, 2011                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include "HealthCheck.h"

#include "../BlockControllerBase.h"
#include "../HardwareBlockList.h"
#include "../RunJobConnection.h"

#include "common/Properties.h"

#include "libmmcs_client/CommandReply.h"

#include <control/include/mcServer/MCServerAPIHelpers.h>
#include <control/include/mcServer/MCServer_errno.h>
#include <control/include/mcServer/MCServerRef.h>

#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/gensrc/DBTBulkpowertemp.h>
#include <db/include/api/tableapi/gensrc/DBTCable.h>
#include <db/include/api/tableapi/gensrc/DBTComponentperf.h>
#include <db/include/api/tableapi/gensrc/DBTServicecardtemp.h>
#include <db/include/api/tableapi/TxObject.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/CableBadWires.h>
#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

using namespace std;

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

HealthCheck::HealthCheck() :
    seconds(ENVS_POLLING_PERIOD),
    _skipModules()
{

}

void*
HealthCheck::threadStart()
{
    unsigned pollingPeriod = NORMAL_POLLING_PERIOD;

    MCServerRef* server;

    // the following interval specification for health check will not be documented to customers, since they must run
    // a 5 minute (300 second) interval. This hidden option was added because early hardware has been generating too many
    // RAS events to deal with.
    unsigned int pollProperty = strtol(Properties::getProperty("health_check_seconds").c_str(), NULL, 10);
    if (pollProperty > 0) {
        seconds = pollProperty;
    }

    LOG_DEBUG_MSG("Health check Interval is " << seconds << " seconds.");

    mmcs_client::CommandReply rep;
    BlockControllerBase::mcserver_connect(server, "EnvMonHC", rep);
    if (!server) {
        LOG_ERROR_MSG("Failed to connect to mc_server.");
        return NULL;
    }

    BGQDB::DBTServicecardtemp sct;
    SQLRETURN rc;

    std::bitset<30> smap;
    smap.set();
    smap.reset(sct.TIME);
    sct._columns = smap.to_ulong();

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG("Unable to connect to database.");
        return NULL;
    }

    // Get list of midplanes from tbgqmidplane
    SQLHANDLE hMidplanes;
    SQLLEN index;
    SQLRETURN sqlrc, result;
    char mp[7];
    string sqlstr;

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQMidplane where location like 'R%-M0'");

    result = tx.execQuery( sqlstr.c_str(), &hMidplanes );

    SQLBindCol(hMidplanes, 1, SQL_C_CHAR, mp, 7, &index);

    // Turn SQL reply into vector of strings
    vector<string> midplaneLocs;
    sqlrc = SQLFetch(hMidplanes);
    while ( sqlrc == SQL_SUCCESS )   {
        midplaneLocs.push_back( mp );
        sqlrc = SQLFetch(hMidplanes);
    }

    SQLCloseCursor(hMidplanes);


    // make the target set for service cards
    MCServerMessageSpec::MakeTargetSetRequest   scMakeRequest("EnvMonSCHealth","EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     scMakeReply;
    scMakeRequest._expression.push_back("R..");
    server->makeTargetSet(scMakeRequest, scMakeReply);

    // make the target set for bulk power
    MCServerMessageSpec::MakeTargetSetRequest   bpMakeRequest("EnvMonBulkHealth","EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     bpMakeReply;
    bpMakeRequest._expression.push_back("R..-M.-S$");
    server->makeTargetSet(bpMakeRequest, bpMakeReply);

    // make the target set for coolant monitor
    MCServerMessageSpec::MakeTargetSetRequest   cmMakeRequest("EnvMonCMHealth","EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     cmMakeReply;
    cmMakeRequest._expression.push_back("R..-L$");
    server->makeTargetSet(cmMakeRequest, cmMakeReply);

    // Get list of IO drawers in IO racks
    SQLHANDLE ioDrawers;
    char ioDrawerLoc[7];

    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M' and location like 'Q%'  ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        bpMakeRequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);

     // make the target set for IO boards
    MCServerMessageSpec::MakeTargetSetRequest   ioMakeRequest( "EnvMonIOHealth", "EnvMonHC", true);
    MCServerMessageSpec::MakeTargetSetReply     ioMakeReply;

    //  Get either compute rack or IO rack locations
    sqlstr.clear();
    sqlstr.append("select LOCATION from BGQIODrawer where status <> 'M'  ");
    result = tx.execQuery( sqlstr.c_str(), &ioDrawers );
    SQLBindCol(ioDrawers, 1, SQL_C_CHAR, ioDrawerLoc, 7, &index);
    sqlrc = SQLFetch(ioDrawers);
    while ( sqlrc == SQL_SUCCESS )   {
        ioMakeRequest._expression.push_back(ioDrawerLoc);
        sqlrc = SQLFetch(ioDrawers);
    }
    SQLCloseCursor(ioDrawers);

    server->makeTargetSet(ioMakeRequest, ioMakeReply);

    // make the target sets for node boards, one target set for each rack
    vector<string> targetSetHandles;
    vector<string>::iterator handle;
    string charHandle = "EnvMonNC00Health";
    MCServerMessageSpec::MakeTargetSetRequest   ncMakeRequest(charHandle,"EnvMonHC",  true);
    MCServerMessageSpec::MakeTargetSetReply     ncMakeReply;
    unsigned mpindex;
    for( mpindex = 0; mpindex < midplaneLocs.size(); mpindex++ ) {

        // Add rack to request
        ncMakeRequest._expression.push_back(midplaneLocs[mpindex].substr(0,3).append("-M.-N..$"));

        // Handle is EnvMonNCxx where xx is rack row,column
        charHandle[8] = midplaneLocs[mpindex][1];
        charHandle[9] = midplaneLocs[mpindex][2];
        ncMakeRequest._set = charHandle;
        server->makeTargetSet(ncMakeRequest, ncMakeReply);
        if( ncMakeReply._rc == BGERR_MCSERVER_OK )
            targetSetHandles.push_back( charHandle );
        // start over for next rack
        ncMakeRequest._expression.clear();
    }


    // The following code is just in case we ever want to override the thresholds, we can
    // do it via undocumented keys in the properties file, rather than an efix
    unsigned int MINFREQ = 1520;
    if (!Properties::getProperty("MINFREQ").empty() ) {
        MINFREQ = strtol(Properties::getProperty("MINFREQ").c_str(), NULL, 10);
    }
    unsigned int MAXFREQ = 1680;
    if (!Properties::getProperty("MAXFREQ").empty() ) {
        MAXFREQ = strtol(Properties::getProperty("MAXFREQ").c_str(), NULL, 10);
    }
    unsigned int MAXTEMP = 50;
    if (!Properties::getProperty("MAXTEMP").empty() ) {
        MAXTEMP = strtol(Properties::getProperty("MAXTEMP").c_str(), NULL, 10);
    }
    double MINRPM = 0;
    if (!Properties::getProperty("MINRPM").empty() ) {
        MINRPM = strtod(Properties::getProperty("MINRPM").c_str(), NULL);
    }
    double MAXRPM = 25000;
    if (!Properties::getProperty("MAXRPM").empty() ) {
        MAXRPM = strtod(Properties::getProperty("MAXRPM").c_str(), NULL);
    }
    double MIN5VVOLTAGE = 4.0;
    if (!Properties::getProperty("MIN5VVOLTAGE").empty() ) {
        MIN5VVOLTAGE = strtod(Properties::getProperty("MIN5VVOLTAGE").c_str(), NULL);
    }
    double MAX5VVOLTAGE = 5.6;
    if (!Properties::getProperty("MAX5VVOLTAGE").empty() ) {
        MAX5VVOLTAGE = strtod(Properties::getProperty("MAX5VVOLTAGE").c_str(), NULL);
    }
    double MIN5VCURRENT = 0.0;
    if (!Properties::getProperty("MIN5VCURRENT").empty() ) {
        MIN5VCURRENT = strtod(Properties::getProperty("MIN5VCURRENT").c_str(), NULL);
    }
    double MAX5VCURRENT = 2.2;
    if (!Properties::getProperty("MAX5VCURRENT").empty() ) {
        MAX5VCURRENT = strtod(Properties::getProperty("MAX5VCURRENT").c_str(), NULL);
    }
    double MAXBPMTEMP = 80;
    if (!Properties::getProperty("MAXBPMTEMP").empty() ) {
        MAXBPMTEMP = strtod(Properties::getProperty("MAXBPMTEMP").c_str(), NULL);
    }
    double BPMTEMPRANGE = 10;
    if (!Properties::getProperty("BPMTEMPRANGE").empty() ) {
        BPMTEMPRANGE = strtod(Properties::getProperty("BPMTEMPRANGE").c_str(), NULL);
    }

    // these thresholds are in mV for voltage and mA for current - hence the large numbers

    double MINDCAVOLTAGE_N[] = {0.0, 700,   1200,  2250, 3300, 0.0,   1450, 700,  900,  11400};
    double MAXDCAVOLTAGE_N[] = {0.0, 1100,  1500,  2750, 3600, 0.0,   1550, 1200, 1100, 14400};
    //double MAXDCAVOLTAGE_N[] = {0.0, 1100,  1600,  2750, 3600, 0.0,   1550, 1200, 1100, 14400};  DD1 values
    double MINDCAVOLTAGE_I[] = {0.0, 700,   1200,  2250, 3300, 11000, 1450, 700,  900,  11000};
    double MAXDCAVOLTAGE_I[] = {0.0, 1100,  1500,  2750, 3600, 13000, 1550, 1200, 1100, 13000};
    //double MAXDCAVOLTAGE_I[] = {0.0, 1100,  1600,  2750, 3600, 13000, 1550, 1200, 1100, 13000};  DD1 values
    double MAXDCACURRENT_N[] = {0.0, 57280, 26080, 2115, 2203, 0.0,   4771, 4771, 1740, 0.0};
    double MAXDCACURRENT_I[] = {0.0, 13480, 6490,  0.0,  0.0,  6370,  0.0,  0.0,  0.0,  0.0};


    // set up for inserting performance counter info
    bgq::utility::performance::Mode::Value::Type mode = bgq::utility::performance::Mode::instance().getMode();

    BGQDB::DBTComponentperf perf;
    std::bitset<30> pmap;
    pmap.set();
    pmap.reset(perf.ENTRYDATE);
    pmap.reset(perf.DETAIL);
    pmap.reset(perf.QUALIFIER);

    perf._columns = pmap.to_ulong();
    strcpy(perf._id,"EnvMon");
    strcpy(perf._component,"mmcs");
    strcpy(perf._function,"EnvMon");
    strcpy(perf._subfunction,"full system health check");
    boost::posix_time::ptime  start;
    boost::posix_time::time_duration duration;

    // Loop until someone stops us
    while (!isThreadStopping()) {
        LOG_DEBUG_MSG("Performing full system health check." );

        // start the clock
        start = boost::posix_time::microsec_clock::local_time();

        try {
            this->getBadWireMasks();
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG("Error getting bad wire masks: " << e.what() );
        }

        // *********  HANDLE SERVICE CARDS   ***************

        // open target set for service cards
        MCServerMessageSpec::OpenTargetRequest   scOpenRequest( "EnvMonSCHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     scOpenReply;
        server->openTarget(scOpenRequest, scOpenReply);

        if (scOpenReply._rc != 0) {
            LOG_ERROR_MSG("Unable to open target set in mcServer, for service card health check: " << scOpenReply._rt);
        } else {
            // read values from cards
            MCServerMessageSpec::ReadServiceCardEnvRequest mcSCRequest;
            MCServerAPIHelpers::copyTargetRequest ( scOpenRequest, mcSCRequest );
            MCServerMessageSpec::ReadServiceCardEnvReply mcSCReply;
            mcSCRequest._shortForm = true;
            server->readServiceCardEnv(mcSCRequest, mcSCReply);

            // close target set, we have our data
            MCServerMessageSpec::CloseTargetRequest scCloseRequest = MCServerAPIHelpers::createCloseRequest( scOpenRequest, scOpenReply );
            MCServerMessageSpec::CloseTargetReply   scCloseReply;
            server->closeTarget(scCloseRequest, scCloseReply);

            for(unsigned sc = 0 ; sc < mcSCReply._serviceCardsShort.size() ; sc++) {
                if(isThreadStopping() == true) return 0;
                if (static_cast<int>(mcSCReply._serviceCardsShort[sc]._error) == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (static_cast<int>(mcSCReply._serviceCardsShort[sc]._error) == CARD_NOT_UP) { } else  // do nothing if the card is not up
                if (mcSCReply._serviceCardsShort[sc]._error != 0) {
                    LOG_ERROR_MSG("Error reading environmentals from: " << mcSCReply._serviceCardsShort[sc]._location);
                    RasEventImpl noContact(0x00061002);
                    noContact.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                    RasEventHandlerChain::handle(noContact);
                    BGQDB::putRAS(noContact);
                } else {
                    strcpy(sct._location,mcSCReply._serviceCardsShort[sc]._location.c_str());
                    sct._onboardtemp = mcSCReply._serviceCardsShort[sc]._onboardTemp;

                    // Note:  onboard temps for service cards are the only inserted value anywhere in the health check
                    // All the rest of the health check logic is for checking against expected values and issuing RAS events
                    rc = tx.insert(&sct);
                    if(rc != SQL_SUCCESS) {
                        LOG_ERROR_MSG("Error writing service card temps: location " << string(sct._location)  << " return code " << rc);
                    }

                    if (mcSCReply._serviceCardsShort[sc]._leakDetectorFaultBpe0 ||
                        mcSCReply._serviceCardsShort[sc]._leakDetectorFaultBpe1) {
                        // send RAS
                        RasEventImpl ras(0x00061006);
                        ras.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                    if (mcSCReply._serviceCardsShort[sc]._clockFreq*16 > MAXFREQ ||
                        mcSCReply._serviceCardsShort[sc]._clockFreq*16 < MINFREQ) {
                        // send RAS  (and fix the values that we're comparing against
                        RasEventImpl ras(0x00061007);
                        ras.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                        ras.setDetail("FREQ", boost::lexical_cast<string>(mcSCReply._serviceCardsShort[sc]._clockFreq*16));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                    if (mcSCReply._serviceCardsShort[sc]._onboardTemp > MAXTEMP) {
                        // send RAS  (and fix the value that we're comparing against
                        RasEventImpl ras(0x00061008);
                        ras.setDetail(RasEvent::LOCATION, mcSCReply._serviceCardsShort[sc]._location.c_str());
                        ras.setDetail("ACTUAL", boost::lexical_cast<string>(mcSCReply._serviceCardsShort[sc]._onboardTemp));
                        ras.setDetail("EXP", boost::lexical_cast<string>(MAXTEMP));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                }
            } // for loop that goes through each service card
        }

        // *********  HANDLE IO BOARDS   ***************

        // open target set for IO boards
        MCServerMessageSpec::OpenTargetRequest   ioOpenRequest( "EnvMonIOHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     ioOpenReply;
        server->openTarget(ioOpenRequest, ioOpenReply);

        if (ioOpenReply._rc != 0) {
            LOG_ERROR_MSG("Unable to open target set in mcServer, for I/O board health check: " << ioOpenReply._rt);
        } else {
            // process env data
            MCServerMessageSpec::ReadIoCardEnvRequest mcIORequest;
            MCServerAPIHelpers::copyTargetRequest ( ioOpenRequest, mcIORequest );
            MCServerMessageSpec::ReadIoCardEnvReply mcIOReply;
            mcIORequest._shortForm = true;
            server->readIoCardEnv(mcIORequest, mcIOReply);

            // close the target set, we have our data
            MCServerMessageSpec::CloseTargetRequest ioCloseRequest = MCServerAPIHelpers::createCloseRequest( ioOpenRequest, ioOpenReply );
            MCServerMessageSpec::CloseTargetReply   ioCloseReply;
            server->closeTarget(ioCloseRequest, ioCloseReply);

            for(unsigned io = 0 ; io < mcIOReply._ioCardsShort.size() ; io++) {
                if(isThreadStopping() == true) return 0;
                if (mcIOReply._ioCardsShort[io]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (mcIOReply._ioCardsShort[io]._error == CARD_NOT_UP) { } else       // do nothing if the card is not up
                if (mcIOReply._ioCardsShort[io]._error != 0) {
                    LOG_ERROR_MSG("Error reading environmentals from: " << mcIOReply._ioCardsShort[io]._lctn);
                    RasEventImpl noContact(0x00061004);
                    noContact.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn);
                    RasEventHandlerChain::handle(noContact);
                    BGQDB::putRAS(noContact);
                } else {

                    if (mcIOReply._ioCardsShort[io]._clockFreq*16 > MAXFREQ ||
                        mcIOReply._ioCardsShort[io]._clockFreq*16 < MINFREQ) {
                        // send RAS  (and fix the values that we're comparing against
                        RasEventImpl ras(0x00061009);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("FREQ", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._clockFreq*16));
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._tempCriticalBlink ||
                        mcIOReply._ioCardsShort[io]._tempWarningBlink ) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "TEMPERATURE");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    /* right now this is disabled because analysis by Don, Kahn, and others determined it was covered elsewhere
                       if (mcIOReply._ioCardsShort[io]._alertsComputes != 0xFF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "COMPUTE ALERTS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                    */

                    /*  right now this is disabled because unbooted computes are causing RAS
                        if (mcIOReply._ioCardsShort[io]._pgoodComputes != 0xFF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "COMPUTE PGOOD");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                        }
                    */

                    if (mcIOReply._ioCardsShort[io]._alertsBlinks != 0x3F) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "LINK CHIP ALERTS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                     if (mcIOReply._ioCardsShort[io]._pgoodBlinks != 0x3F) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "LINK CHIP PGOOD");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._alertsOptics != 0xFFFFFF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "OPTICS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._statusDca != 1) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "DCA");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    if (mcIOReply._ioCardsShort[io]._statusVtmDomains != 0x3FF) {
                        // put RAS
                        RasEventImpl ras(0x0006100A);
                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._lctn.c_str());
                        ras.setDetail("COMP", "VTM DOMAINS");
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }

                    for(unsigned iod = 0 ; iod < mcIOReply._ioCardsShort[io]._dcas.size() ; iod++) {
                        if(isThreadStopping() == true) return 0;
                        if (mcIOReply._ioCardsShort[io]._dcas[iod]._error != 0) {
                            LOG_ERROR_MSG("Error reading environmentals from: " <<  mcIOReply._ioCardsShort[io]._dcas[iod]._lctn);
                            // put RAS
                            RasEventImpl ras(0x0006100B);
                            ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                            ras.setDetail("COND", "CARD ERROR");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._error));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        } else {
                            for(unsigned iodp = 0 ; iodp < mcIOReply._ioCardsShort[io]._dcas[iod]._domains.size() ; iodp++) {
                                if(isThreadStopping() == true) return 0;
                                if ((mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0x90 &&
                                     mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0xB0) ||
                                    mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain > 9) {
                                    // put RAS
                                    RasEventImpl ras(0x0006100B);
                                    ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                                    if (mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0x90 &&
                                        mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status != 0xB0) {
                                        ras.setDetail("COND", string("INCORRECT STATUS, DOMAIN ") +  boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain));
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._status)));
                                    } else {
                                        ras.setDetail("COND", "INCORRECT POWER DOMAIN VALUE");
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain)));
                                    }
                                    RasEventHandlerChain::handle(ras);
                                    BGQDB::putRAS(ras);
                                } else {
                                    if (MINDCAVOLTAGE_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain] != 0.0 &&
                                        (mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._voltage < MINDCAVOLTAGE_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain]  ||
                                         mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._voltage > MAXDCAVOLTAGE_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain])) {
                                        // put RAS
                                        RasEventImpl ras(0x0006100B);
                                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                                        ras.setDetail("COND", string("INCORRECT VOLTAGE, DOMAIN ") +  boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain));
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._voltage));
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    }
                                    if (MAXDCACURRENT_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain] != 0.0 &&
                                        mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._current > MAXDCACURRENT_I[mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain]) {
                                        // put RAS
                                        RasEventImpl ras(0x0006100B);
                                        ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._dcas[iod]._lctn.c_str());
                                        ras.setDetail("COND", string("INCORRECT CURRENT, DOMAIN ") +  boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._domain));
                                        ras.setDetail("BADVAL", boost::lexical_cast<string>(mcIOReply._ioCardsShort[io]._dcas[iod]._domains[iodp]._current));
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    }
                                }

                            }

                        }
                    }

                    for(unsigned ioo = 0 ; ioo < mcIOReply._ioCardsShort[io]._optics.size() ; ioo++) {
                        const bool skip(
                                _skipModules.find( mcIOReply._ioCardsShort[io]._optics[ioo]._lctn ) != _skipModules.end()
                                );
                        if ( skip ) {
                            LOG_TRACE_MSG("Skipping health check for " << mcIOReply._ioCardsShort[io]._optics[ioo]._lctn);
                            continue;
                        }
                        if(isThreadStopping() == true) return 0;
                        if (mcIOReply._ioCardsShort[io]._optics[ioo]._error != 0) {
                            LOG_ERROR_MSG("Error reading environmentals from: " <<  mcIOReply._ioCardsShort[io]._optics[ioo]._lctn);
                            // put RAS
                            RasEventImpl ras(0x0006100D);
                            ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._optics[ioo]._lctn.c_str());
                            ras.setDetail("COND", "MODULE ERROR");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        } else {
                            if ((mcIOReply._ioCardsShort[io]._optics[ioo]._status & 0x02) != 0) {  // check interrupt bit
                                // put RAS
                                RasEventImpl ras(0x0006100D);
                                ras.setDetail(RasEvent::LOCATION, mcIOReply._ioCardsShort[io]._optics[ioo]._lctn.c_str());
                                string condition = "MODULE STATUS";
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._los & ~mcIOReply._ioCardsShort[io]._optics[ioo]._losMask) != 0) {
                                    condition.append(", CHANNEL ");
                                    // apply mask to get channel information
                                    const int channels = mcIOReply._ioCardsShort[io]._optics[ioo]._los  & ~mcIOReply._ioCardsShort[io]._optics[ioo]._losMask;

                                    // iterate through all 11 channels looking at which ones have loss of
                                    // signal alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0x1 << channel;
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                    condition.append("LOSS OF SIGNAL");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._faults & ~mcIOReply._ioCardsShort[io]._optics[ioo]._faultsMask) != 0) {
                                    condition.append(", CHANNEL ");
                                    // apply mask to get channel information
                                    const int channels = mcIOReply._ioCardsShort[io]._optics[ioo]._faults & ~mcIOReply._ioCardsShort[io]._optics[ioo]._faultsMask;

                                    // iterate through all 11 channels looking at which ones have fault
                                    // alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0x1 << channel;
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                    condition.append("FAULT");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsTemp & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsTempMask) != 0) {
                                    condition.append(", TEMPERATURE");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsVoltage & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsVoltageMask) != 0) {
                                    condition.append(", VOLTAGE");
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrent & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrentMask) != 0) {
                                    condition.append(", BIAS CURRENT ");
                                    // apply mask to get channel information
                                    const uint64_t channels = mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrent & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsBiasCurrentMask;

                                    // iterate through all 11 channels looking at which ones have power
                                    // alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0xFULL << (channel * 4);
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                }
                                if ((mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPower & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPowerMask) != 0) {
                                    condition.append(", POWER ");
                                    // apply mask to get channel information
                                    const uint64_t channels = mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPower & ~mcIOReply._ioCardsShort[io]._optics[ioo]._alarmsPowerMask;

                                    // iterate through all 11 channels looking at which ones have power
                                    // alarms, see mc.h for the format
                                    for ( unsigned channel = 0; channel < 12; ++channel ) {
                                        const uint64_t bit = 0xFULL << (channel * 4);
                                        if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                    }
                                }
                                // remove MODULE_STATUS if something else was found
                                if (condition != "MODULE STATUS") {
                                    condition = condition.substr(15);
                                }

                                ras.setDetail("COND", condition);
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                        }
                    } // end of loop thru optics
                }
            } // end of loop thru IO cards
        }

        // *********  HANDLE BULK POWER MODULES  *****************

        // open target set for bulk power modules
        MCServerMessageSpec::OpenTargetRequest   bpOpenRequest( "EnvMonBulkHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     bpOpenReply;
        server->openTarget(bpOpenRequest, bpOpenReply);

        if (bpOpenReply._rc != 0) {
            LOG_ERROR_MSG("Unable to open target set in mcServer, for bulk power health check: " << bpOpenReply._rt);
        } else {
            // read values from cards
            MCServerMessageSpec::ReadBulkPowerEnvRequest mcBPRequest;
            MCServerAPIHelpers::copyTargetRequest ( bpOpenRequest, mcBPRequest );
            MCServerMessageSpec::ReadBulkPowerEnvReply   mcBPReply;
            server->readBulkPowerEnv(mcBPRequest, mcBPReply);
            // Note that the above call is not a "short form".
            // The bulk power modules do not have a short form, since there was no difference between the data needed


            // close target set, we have our data
            MCServerMessageSpec::CloseTargetRequest bpCloseRequest = MCServerAPIHelpers::createCloseRequest( bpOpenRequest, bpOpenReply );
            MCServerMessageSpec::CloseTargetReply   bpCloseReply;
            server->closeTarget(bpCloseRequest, bpCloseReply);

            std::set<std::string> modulesToClear;

            for(unsigned bp = 0 ; bp < mcBPReply._bpms.size() ; ++bp) {
                if(isThreadStopping() == true) return 0;
                if (static_cast<int>(mcBPReply._bpms[bp]._error) == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (static_cast<int>(mcBPReply._bpms[bp]._error) == CARD_NOT_UP) { } else       // do nothing if the card is not up
                if (mcBPReply._bpms[bp]._error != 0) {
                    LOG_ERROR_MSG("Error reading environmentals from: " << mcBPReply._bpms[bp]._location);
                    RasEventImpl noContact(0x00061003);
                    noContact.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._location.c_str());
                    RasEventHandlerChain::handle(noContact);
                    BGQDB::putRAS(noContact);
                } else {
                    std::map<std::string, int> failedBpms;

                    for(unsigned bpm = 0 ; bpm < mcBPReply._bpms[bp]._bpms.size() ; ++bpm) {
                        if(isThreadStopping() == true) return 0;
                        // first check for failed BPM.  This has different criteria than sending RAS

                        if  ( ((mcBPReply._bpms[bp]._bpms[bpm]._statusWord & 0x0A78) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusVout & 0x0095) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusIout & 0x00CA) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusInput & 0x009C) != 0)||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature & 0x0090) != 0 ) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._status5V   & 0x009B) != 0) ||
                              ((mcBPReply._bpms[bp]._bpms[bpm]._statusFans & 0xC2) != 0) ) {
                            // the mask values are used to ignore warnings and non-critical faults
                            // see the spec from Delta, and issue 3007 for details
                            failedBpms[mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6)]++;

                            if (failedBpms[mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6)] > 1) {
                                // We have more than one BPM failure, time to start marking hardware in error

                                string B0cards[] = {"-M0-N00","-M0-N01","-M0-N02","-M0-N03","-M0-N04","-M0-N05","-M0-N06","-M0-N07"};
                                string B1cards[] = {"-M0-N08","-M0-N09","-M0-N10","-M0-N11","-M0-N12","-M0-N13","-M0-N14","-M0-N15"};
                                string B2cards[] = {"-M1-N00","-M1-N01","-M1-N02","-M1-N03","-M1-N04","-M1-N05","-M1-N06","-M1-N07"};
                                string B3cards[] = {"-M1-N08","-M1-N09","-M1-N10","-M1-N11","-M1-N12","-M1-N13","-M1-N14","-M1-N15"};

                                string IODrawersTH[] = {"-IE", "-ID", "-IC", "-IF"};
                                string IODrawersIOR[] = {"-I0","-I1","-I2","-I3","-I4","-I5","-I6","-I7","-I8","-I9","-IA","-IB"};

                                SQLHANDLE hIODrawer;
                                SQLLEN index;
                                SQLRETURN sqlrc, result;
                                char iod[7];
                                string sqlstr, iod_loc, nc_loc;

                                // clear the failure, so we don't get duplicate RAS events
                                failedBpms[mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6)] = 0;

                                LOG_ERROR_MSG("Multiple BPM failures from: " << mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,6) );

                                if(mcBPReply._bpms[bp]._bpms[bpm]._location.substr(0,1) == "R") {  //this is a compute rack
                                    for (int card = 0; card < 8; card++) {
                                        if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "0")
                                            nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B0cards[card];
                                        else
                                            if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "1")
                                                nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B1cards[card];
                                            else
                                                if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "2")
                                                    nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B2cards[card];
                                                else
                                                    nc_loc = mcBPReply._bpms[bp]._location.substr(0,3) + B3cards[card];

                                        RasEventImpl bpmFails(0x00061012);
                                        bpmFails.setDetail(RasEvent::LOCATION, nc_loc);
                                        RasEventHandlerChain::handle(bpmFails);

                                        vector<BGQDB::job::Id> jobs;
                                        uint32_t recid;
                                        const int signal = bgcios::jobctl::SIGHARDWAREFAILURE;
                                        const std::string block;
                                        const uint32_t qualifier = 0;
                                        const BGQDB::job::Id job = 0;
                                        BGQDB::putRAS(bpmFails, block, job, qualifier, &jobs, &recid);

                                        for (unsigned i = 0; i < jobs.size(); ++i) {
                                            RunJobConnection::instance().kill(jobs[i], signal, recid);
                                        }
                                    }

                                    // Now determine if an IO drawer also has to be disabled

                                    if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "0")
                                        iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[0];
                                    else
                                        if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "1")
                                            iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[1];
                                        else
                                            if (mcBPReply._bpms[bp]._bpms[bpm]._location.substr(5,1) == "2")
                                                iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[2];
                                            else
                                                iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersTH[3];

                                    sqlstr.clear();
                                    sqlstr = "select LOCATION from BGQIODrawer where status = 'A' and location = '" + iod_loc + string("'");
                                    result = tx.execQuery( sqlstr.c_str(), &hIODrawer );
                                    SQLBindCol(hMidplanes, 1, SQL_C_CHAR, iod, 7, &index);
                                    sqlrc = SQLFetch(hIODrawer);
                                    if ( sqlrc == SQL_SUCCESS )   {   //there is an IO drawer at this location
                                        RasEventImpl bpmFails(0x00061013);
                                        bpmFails.setDetail(RasEvent::LOCATION, iod_loc);
                                        RasEventHandlerChain::handle(bpmFails);
                                        BGQDB::putRAS(bpmFails);
                                    }
                                    SQLCloseCursor(hMidplanes);

                                } else {  //this is an IO rack

                                    RasEventImpl bpmFails(0x00061013);

                                    for (int io = 0; io < 12; io++) {  // loop thru 12 possible drawer locations
                                        iod_loc = mcBPReply._bpms[bp]._location.substr(0,3) + IODrawersIOR[io];
                                        sqlstr.clear();
                                        sqlstr = "select LOCATION from BGQIODrawer where status = 'A' and location = '" + iod_loc + string("'");
                                        result = tx.execQuery( sqlstr.c_str(), &hIODrawer );
                                        SQLBindCol(hMidplanes, 1, SQL_C_CHAR, iod, 7, &index);
                                        sqlrc = SQLFetch(hIODrawer);
                                        if ( sqlrc == SQL_SUCCESS )   {   //there is an IO drawer at this location
                                            RasEventImpl bpmFails(0x00061013);
                                            bpmFails.setDetail(RasEvent::LOCATION, iod_loc);
                                            RasEventHandlerChain::handle(bpmFails);
                                            BGQDB::putRAS(bpmFails);
                                        }
                                        SQLCloseCursor(hMidplanes);
                                    }
                                }
                            }
                        }

                        // now check for RAS conditions

                        if  (( mcBPReply._bpms[bp]._bpms[bpm]._statusWord != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusVout != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusIout != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusInput != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._statusCml != 0 ) ||
                             ( mcBPReply._bpms[bp]._bpms[bpm]._status5V != 0 ) ||
                             ( (mcBPReply._bpms[bp]._bpms[bpm]._statusFans & 0xF3) != 0 )) {

                            modulesToClear.insert( mcBPReply._bpms[bp]._bpms[bpm]._location );

                            stringstream badval;

                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusWord != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "MODULE STATUS");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusWord);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusVout != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "OUTPUT VOLTAGE");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusVout);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusIout != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "OUTPUT CURRENT");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusIout);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusInput != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "INPUT POWER");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusInput);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "TEMPERATURE");
                                badval << "0x" << std::hex <<  int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._statusCml != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "COMM, LOGIC, AND MEMORY");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusCml);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if ( mcBPReply._bpms[bp]._bpms[bpm]._status5V != 0 ) {
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "5V OUTPUT");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._status5V);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                            if (( mcBPReply._bpms[bp]._bpms[bpm]._statusFans & 0xF3) != 0 ) {  // masks off fan speed override bits
                                RasEventImpl ras(0x0006100F);
                                ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                                ras.setDetail("COND", "FANS");
                                badval << "0x" << std::hex << int64_t(mcBPReply._bpms[bp]._bpms[bpm]._statusFans);
                                ras.setDetail("BADVAL", badval.str());
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            }
                        }
                        double outVoltage5v, outCurrent5v, temp[3], rpmsFan1, rpmsFan2;
                        outVoltage5v = mcBPReply._bpms[bp]._bpms[bpm]._outputVoltage5V;
                        if ( outVoltage5v > MAX5VVOLTAGE ||
                             outVoltage5v < MIN5VVOLTAGE) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "5V OUTPUT VOLTAGE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(outVoltage5v));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        outCurrent5v = mcBPReply._bpms[bp]._bpms[bpm]._outputCurrent5V;
                        if ( outCurrent5v > MAX5VCURRENT ||
                             outCurrent5v < MIN5VCURRENT) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "5V OUTPUT CURRENT");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(outCurrent5v));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        temp[0] = mcBPReply._bpms[bp]._bpms[bpm]._temp1;
                        temp[1] = mcBPReply._bpms[bp]._bpms[bpm]._temp2;
                        temp[2] = mcBPReply._bpms[bp]._bpms[bpm]._temp3;


                        if (!Properties::getProperty("BPMTEMPS").empty() ) {
                            // test code to record temps for profiling.
                            // the purpose is to use actual temperatures to determine the MAXBPMTEMP value and
                            // the BPMTEMPRANGE based on accumulated data
                            BGQDB::DBTBulkpowertemp bpt;
                            std::bitset<30> map;
                            SQLRETURN rc;
                            map.set();
                            map.reset(bpt.TIME);
                            bpt._columns = map.to_ulong();
                            strcpy(bpt._location,mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            bpt._module1temp = mcBPReply._bpms[bp]._bpms[bpm]._temp1;
                            bpt._module2temp = mcBPReply._bpms[bp]._bpms[bpm]._temp2;
                            bpt._module3temp = mcBPReply._bpms[bp]._bpms[bpm]._temp3;

                            rc = tx.insert(&bpt);
                            if(rc != SQL_SUCCESS) {
                                LOG_ERROR_MSG("Error writing bulk power temperatures: location " << string(bpt._location)  << " return code " << rc);
                            }

                        }

                        if ( temp[0] > MAXBPMTEMP ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "DC/DC MODULE 1 TEMPERATURE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(temp[0]));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if ( temp[1] > MAXBPMTEMP ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "DC/DC MODULE 2 TEMPERATURE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(temp[1]));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if ( temp[2] > MAXBPMTEMP ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "DC/DC MODULE 3 TEMPERATURE");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(temp[2]));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        rpmsFan1 = mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan1;
                        rpmsFan2 = mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan2;
                        if ( rpmsFan1 > MAXRPM ||
                             rpmsFan1 < MINRPM ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "FAN 1 SPEED");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(rpmsFan1));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if ( rpmsFan2 > MAXRPM ||
                             rpmsFan2 < MINRPM ) {
                            // send RAS
                            RasEventImpl ras(0x0006100F);
                            ras.setDetail(RasEvent::LOCATION, mcBPReply._bpms[bp]._bpms[bpm]._location.c_str());
                            ras.setDetail("COND", "FAN 2 SPEED");
                            ras.setDetail("BADVAL", boost::lexical_cast<string>(rpmsFan2));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                    }
                }
            }

            if ( !modulesToClear.empty() ) {
                const std::string setName( "EnvMonBulkHealthFaults" );
                MCServerMessageSpec::MakeTargetSetRequest makeRequest( setName, "EnvMonHC",  true);
                MCServerMessageSpec::MakeTargetSetReply makeReply;
                LOG_DEBUG_MSG("Clearing BPM faults for " << modulesToClear.size() << " BPMs" );
                BOOST_FOREACH( const std::string& i, modulesToClear ) {
                    makeRequest._location.push_back( i );
                    LOG_DEBUG_MSG( i );
                }
                server->makeTargetSet( makeRequest, makeReply );

                const MCServerMessageSpec::OpenTargetRequest openRequest( setName, "EnvMonHC", MCServerMessageSpec::WUAR, true);
                MCServerMessageSpec::OpenTargetReply openReply;
                server->openTarget( openRequest, openReply );

                MCServerMessageSpec::BpmCommandRequest bpmRequest;
                bpmRequest._set = setName;
                bpmRequest._operation = MCServerMessageSpec::BPM_ClearFaults;
                MCServerMessageSpec::BpmCommandReply bpmReply;
                server->bpmCommand( bpmRequest, bpmReply );
                if ( bpmReply._rc ) {
                    LOG_WARN_MSG("Could not clear BPM faults: " << bpmReply._rt );
                } else {
                    LOG_DEBUG_MSG("Cleared BPM faults");
                }

                const MCServerMessageSpec::CloseTargetRequest closeRequest(
                        MCServerAPIHelpers::createCloseRequest( openRequest, openReply )
                        );
                MCServerMessageSpec::CloseTargetReply closeReply;
                server->closeTarget( closeRequest, closeReply );
                if ( closeReply._rc ) {
                    LOG_WARN_MSG("Could not close target set " << setName << ": " << closeReply._rt);
                }

                const MCServerMessageSpec::DeleteTargetSetRequest deleteRequest( setName, "EnvMonHC" );
                MCServerMessageSpec::DeleteTargetSetReply deleteReply;
                server->deleteTargetSet( deleteRequest, deleteReply );
                if ( deleteReply._rc ) {
                    LOG_WARN_MSG("Could not delete target set " << setName << ": " << deleteReply._rt);
                }
            }
        }

        // *********  HANDLE NODE BOARDS   ***************

        for( handle = targetSetHandles.begin(); handle != targetSetHandles.end(); ) {
            if(isThreadStopping() == true) return 0;
            // open target set in monitor mode
            MCServerMessageSpec::OpenTargetRequest   ncOpenRequest( *handle, "EnvMonHC", MCServerMessageSpec::RAAW  , true);
            MCServerMessageSpec::OpenTargetReply     ncOpenReply;
            server->openTarget(ncOpenRequest, ncOpenReply);

            if (ncOpenReply._rc != 0) {
                LOG_ERROR_MSG("Unable to open target set in mcServer: " << ncOpenReply._rt);
            } else {
                // read values from cards
                MCServerMessageSpec::ReadNodeCardEnvRequest mcNCRequest;
                MCServerAPIHelpers::copyTargetRequest( ncOpenRequest, mcNCRequest );
                MCServerMessageSpec::ReadNodeCardEnvReply   mcNCReply;
                mcNCRequest._shortForm = true;
                server->readNodeCardEnv(mcNCRequest, mcNCReply);

                // close target set, we have our data
                MCServerMessageSpec::CloseTargetRequest ncCloseRequest = MCServerAPIHelpers::createCloseRequest( ncOpenRequest, ncOpenReply);
                MCServerMessageSpec::CloseTargetReply   ncCloseReply;
                server->closeTarget(ncCloseRequest, ncCloseReply);

                for(unsigned nc = 0 ; nc < mcNCReply._nodeCardsShort.size() ; nc ++) {
                    if(isThreadStopping() == true) return 0;
                    if (mcNCReply._nodeCardsShort[nc]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                    if (mcNCReply._nodeCardsShort[nc]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
                    if (mcNCReply._nodeCardsShort[nc]._error != 0) {
                        LOG_ERROR_MSG("Error reading environmentals from: " << mcNCReply._nodeCardsShort[nc]._lctn);
                        RasEventImpl noContact(0x00061001);
                        noContact.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                        RasEventHandlerChain::handle(noContact);
                        BGQDB::putRAS(noContact);
                    } else {

                        if (mcNCReply._nodeCardsShort[nc]._clockFreq*16 > MAXFREQ ||
                            mcNCReply._nodeCardsShort[nc]._clockFreq*16 < MINFREQ) {
                            // send RAS  (and fix the values that we're comparing against
                            RasEventImpl ras(0x00061010);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("FREQ", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._clockFreq*16));
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        /* right now this is disabled because analysis by Don, Kahn, and others determined it was covered elsewhere
                        if (mcNCReply._nodeCardsShort[nc]._alertsComputes != 0x0F) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "COMPUTE ALERTS");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        */

                        /*  right now this is disabled because unbooted computes are causing RAS
                         if (mcNCReply._nodeCardsShort[nc]._pgoodComputes != 0xFFFFFFFF) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "COMPUTE PGOOD");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        */

                        if (mcNCReply._nodeCardsShort[nc]._alertsBlinks != 1) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "LINK CHIP ALERTS");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        if (mcNCReply._nodeCardsShort[nc]._pgoodBlinks != 1) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "LINK CHIP PGOOD");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        if (mcNCReply._nodeCardsShort[nc]._alertsOptics != 1) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "OPTICS");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        if (mcNCReply._nodeCardsShort[nc]._statusDca != 3) {
                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", "DCA");
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                        if (mcNCReply._nodeCardsShort[nc]._statusVtmDomains123 != 0x3FFFFFFF ||
                            (mcNCReply._nodeCardsShort[nc]._statusVtmDomains468 != 0x0FFF)
                             ) {
                            const std::map<int,std::string> domain123 = boost::assign::map_list_of
                                (0x20000000, "02 domain 3")
                                (0x10000000, "01 domain 3")
                                (0x08000000, "09 domain 2")
                                (0x04000000, "08 domain 2")
                                (0x02000000, "07 domain 2")
                                (0x01000000, "06 domain 2")
                                (0x00800000, "05 domain 2")
                                (0x00400000, "04 domain 2")
                                (0x00200000, "03 domain 2")
                                (0x00100000, "02 domain 2")
                                (0x00080000, "01 domain 2")
                                (0x00040000, "19 domain 1")
                                (0x00020000, "18 domain 1")
                                (0x00010000, "17 domain 1")
                                (0x00008000, "16 domain 1")
                                (0x00004000, "15 domain 1")
                                (0x00002000, "14 domain 1")
                                (0x00001000, "13 domain 1")
                                (0x00000800, "12 domain 1")
                                (0x00000400, "11 domain 1")
                                (0x00000200, "10 domain 1")
                                (0x00000100, "09 domain 1")
                                (0x00000080, "08 domain 1")
                                (0x00000040, "07 domain 1")
                                (0x00000020, "06 domain 1")
                                (0x00000010, "05 domain 1")
                                (0x00000008, "04 domain 1")
                                (0x00000004, "03 domain 1")
                                (0x00000002, "02 domain 1")
                                (0x00000001, "01 domain 1");
                            const std::map<int,std::string> domain468 = boost::assign::map_list_of
                                (0x00000800, "04 domain 7")
                                (0x00000400, "03 domain 7")
                                (0x00000200, "02 domain 7")
                                (0x00000100, "01 domain 7")
                                (0x00000080, "02 domain 8")
                                (0x00000040, "01 domain 8")
                                (0x00000020, "04 domain 6")
                                (0x00000010, "03 domain 6")
                                (0x00000008, "02 domain 6")
                                (0x00000004, "01 domain 6")
                                (0x00000002, "02 domain 4")
                                (0x00000001, "01 domain 4");

                            // add specific VTM location to event
                            std::ostringstream component;
                            component << "VTM DOMAINS: ";
                            for ( std::map<int,std::string>::const_iterator i = domain123.begin(); i != domain123.end(); ++i ) {
                                if (!(mcNCReply._nodeCardsShort[nc]._statusVtmDomains123 & i->first)) component << i->second << ", ";
                            }
                            for ( std::map<int,std::string>::const_iterator i = domain468.begin(); i != domain468.end(); ++i ) {
                                if (!(mcNCReply._nodeCardsShort[nc]._statusVtmDomains468 & i->first)) component << i->second << ", ";
                            }

                            // put RAS
                            RasEventImpl ras(0x00061011);
                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._lctn.c_str());
                            ras.setDetail("COMP", component.str());
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }

                        for(unsigned ncd = 0 ; ncd < mcNCReply._nodeCardsShort[nc]._dcas.size() ; ncd++) {
                            if(isThreadStopping() == true) return 0;
                            if (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._error != 0) {
                                LOG_ERROR_MSG("Error reading environmentals from: " <<  mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn);
                                // put RAS
                                RasEventImpl ras(0x0006100C);
                                ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                ras.setDetail("COND", "CARD ERROR");
                                ras.setDetail("BADVAL", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._error));
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            } else {
                                for(unsigned ncdp = 0 ; ncdp < mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains.size() ; ncdp++) {
                                    if(isThreadStopping() == true) return 0;
                                    if ((mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0x90 &&
                                         mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0xB0) ||
                                        mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain > 9) {
                                        // put RAS
                                        RasEventImpl ras(0x0006100C);
                                        ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                        if (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0x90 &&
                                            mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status != 0xB0) {
                                            ras.setDetail("COND", string("INCORRECT STATUS, DOMAIN ") + boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain));
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._status)));
                                        } else {
                                            ras.setDetail("COND", "INCORRECT POWER DOMAIN VALUE");
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(int64_t(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain)));
                                        }
                                        RasEventHandlerChain::handle(ras);
                                        BGQDB::putRAS(ras);
                                    } else {
                                        if (MINDCAVOLTAGE_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain] != 0.0 &&
                                            (mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._voltage < MINDCAVOLTAGE_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain]  ||
                                             mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._voltage > MAXDCAVOLTAGE_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain])) {
                                            // put RAS
                                            RasEventImpl ras(0x0006100C);
                                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                            ras.setDetail("COND", string("INCORRECT VOLTAGE, DOMAIN ") + boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain));
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._voltage));
                                            RasEventHandlerChain::handle(ras);
                                            BGQDB::putRAS(ras);
                                        }
                                        if (MAXDCACURRENT_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain] != 0.0 &&
                                            mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._current > MAXDCACURRENT_N[mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain]) {
                                            // put RAS
                                            RasEventImpl ras(0x0006100C);
                                            ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._dcas[ncd]._lctn.c_str());
                                            ras.setDetail("COND", string("INCORRECT CURRENT, DOMAIN ") + boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._domain));
                                            ras.setDetail("BADVAL", boost::lexical_cast<string>(mcNCReply._nodeCardsShort[nc]._dcas[ncd]._domains[ncdp]._current));
                                            RasEventHandlerChain::handle(ras);
                                            BGQDB::putRAS(ras);
                                        }
                                    }
                                }  // end for loop

                            }
                        }

                        for(unsigned nco = 0 ; nco < mcNCReply._nodeCardsShort[nc]._optics.size() ; nco++) {
                            const bool skip(
                                    _skipModules.find( mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn ) != _skipModules.end()
                                    );
                            if ( skip ) {
                                LOG_DEBUG_MSG("Skipping health check for " << mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn );
                                continue;
                            }
                            if(isThreadStopping() == true) return 0;
                            if (mcNCReply._nodeCardsShort[nc]._optics[nco]._error != 0) {
                                LOG_ERROR_MSG("Error reading environmentals from: " << mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn);
                                // put RAS
                                RasEventImpl ras(0x0006100E);
                                ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn.c_str());
                                ras.setDetail("COND", "MODULE ERROR");
                                RasEventHandlerChain::handle(ras);
                                BGQDB::putRAS(ras);
                            } else {
                                if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._status & 0x02) != 0) {  // check interrupt bit

                                    const std::map<int,std::string> channels = boost::assign::map_list_of
                                        (0x0800, "11")
                                        (0x0400, "10")
                                        (0x0200, "9")
                                        (0x0100, "8")
                                        (0x0080, "7")
                                        (0x0040, "6")
                                        (0x0020, "5")
                                        (0x0010, "4")
                                        (0x0008, "3")
                                        (0x0004, "2")
                                        (0x0002, "1")
                                        (0x0001, "0");

                                    // put RAS
                                    RasEventImpl ras(0x0006100E);
                                    ras.setDetail(RasEvent::LOCATION, mcNCReply._nodeCardsShort[nc]._optics[nco]._lctn.c_str());
                                    string condition = "MODULE STATUS";
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._los & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._losMask) != 0) {
                                        condition.append(", CHANNEL ");
                                        // apply mask to get channel information
                                        const int channels = mcNCReply._nodeCardsShort[nc]._optics[nco]._los  & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._losMask;

                                        // iterate through all 11 channels looking at which ones have loss of
                                        // signal alarms, see mc.h for the format
                                        for ( unsigned channel = 0; channel < 12; ++channel ) {
                                            const uint64_t bit = 0x1 << channel;
                                            if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                        }
                                        condition.append("LOSS OF SIGNAL");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._faults & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._faultsMask) != 0) {
                                        condition.append(", CHANNEL ");
                                        for ( std::map<int,std::string>::const_iterator i = channels.begin(); i != channels.end(); ++i ) {
                                            if (mcNCReply._nodeCardsShort[nc]._optics[nco]._faults & i->first) condition.append( i->second + " ");
                                        }
                                        condition.append("FAULT");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsTemp & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsTempMask) != 0) {
                                        condition.append(", TEMPERATURE");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsVoltage & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsVoltageMask) != 0) {
                                        condition.append(", VOLTAGE");
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrent & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrentMask) != 0) {
                                        condition.append(", BIAS CURRENT ");
                                        // apply mask to get channel information
                                        const uint64_t channels = mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrent & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsBiasCurrentMask;

                                        // iterate through all 11 channels looking at which ones have bias
                                        // current alarms, see mc.h for the format
                                        for ( unsigned channel = 0; channel < 12; ++channel ) {
                                            const uint64_t bit = 0xFULL << (channel * 4);
                                            if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                        }
                                    }
                                    if ((mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPower & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPowerMask) != 0) {
                                        condition.append(", POWER ");
                                        // apply mask to get channel information
                                        const uint64_t channels = mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPower & ~mcNCReply._nodeCardsShort[nc]._optics[nco]._alarmsPowerMask;

                                        // iterate through all 11 channels looking at which ones have power
                                        // alarms, see mc.h for the format
                                        for ( unsigned channel = 0; channel < 12; ++channel ) {
                                            const uint64_t bit = 0xFULL << (channel * 4);
                                            if (channels & bit) condition.append( boost::lexical_cast<std::string>(channel) + " ");
                                        }
                                    }
                                    // remove MODULE_STATUS if something else was found
                                    if (condition != "MODULE STATUS") {
                                        condition = condition.substr(15);
                                    }

                                    ras.setDetail("COND", condition);
                                    RasEventHandlerChain::handle(ras);
                                    BGQDB::putRAS(ras);
                                }
                            }
                        }  // end of loop through optical modules
                    }
                }

            }

            handle++;

        }

        // *********  HANDLE COOLANT MONITORS   ***************


        // open target set for coolant monitors (uses service card target set?)
        MCServerMessageSpec::OpenTargetRequest   cmOpenRequest( "EnvMonCMHealth","EnvMonHC", MCServerMessageSpec::RAAW  , true);
        MCServerMessageSpec::OpenTargetReply     cmOpenReply;
        server->openTarget(cmOpenRequest, cmOpenReply);

        if (cmOpenReply._rc != 0) {
            LOG_ERROR_MSG("Unable to open target set in mcServer, for coolant monitor health check: " << cmOpenReply._rt);
        } else {
            // read values from cards

            // read values from cards
            MCServerMessageSpec::ReadCoolantMonitorEnvRequest mcCMRequest;
            MCServerAPIHelpers::copyTargetRequest ( cmOpenRequest, mcCMRequest );
            MCServerMessageSpec::ReadCoolantMonitorEnvReply   mcCMReply;
            server->readCoolantMonitorEnv(mcCMRequest, mcCMReply);


            // close target set, we have our data
            MCServerMessageSpec::CloseTargetRequest cmCloseRequest = MCServerAPIHelpers::createCloseRequest( cmOpenRequest, cmOpenReply );
            MCServerMessageSpec::CloseTargetReply   cmCloseReply;
            server->closeTarget(cmCloseRequest, cmCloseReply);

            for(unsigned cmon = 0 ; cmon < mcCMReply._coolMons.size() ; cmon++) {
                if(isThreadStopping() == true) return 0;
                if (mcCMReply._coolMons[cmon]._error == CARD_NOT_PRESENT) { } else  // do nothing if the card is not present
                if (mcCMReply._coolMons[cmon]._error == CARD_NOT_UP) { } else  // do nothing if the card is not up
                if (mcCMReply._coolMons[cmon]._error != 0) {
                    LOG_ERROR_MSG("Error reading environmentals from: " << mcCMReply._coolMons[cmon]._lctn);
                    RasEventImpl ras(0x00061005);
                    ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                    RasEventHandlerChain::handle(ras);
                    BGQDB::putRAS(ras);
                } else {
                    if (mcCMReply._coolMons[cmon]._supplyFlowStatus != 0  ||
                        mcCMReply._coolMons[cmon]._returnFlowStatus != 0  ||
                        mcCMReply._coolMons[cmon]._leakStatus > 1   ||
                        mcCMReply._coolMons[cmon]._supplyTempStatus != 0  ||
                        mcCMReply._coolMons[cmon]._returnTempStatus != 0  ||
                        mcCMReply._coolMons[cmon]._supplyPressureStatus != 0  ||
                        mcCMReply._coolMons[cmon]._diffPressureStatus != 0  ||
                        mcCMReply._coolMons[cmon]._ambientTempStatus != 0  ||
                        mcCMReply._coolMons[cmon]._humidityStatus != 0 ||
                        mcCMReply._coolMons[cmon]._dewpointStatus != 0 ||
                        mcCMReply._coolMons[cmon]._powerConsumptionStatus != 0 ||
                        mcCMReply._coolMons[cmon]._impedanceStatus != 0) {
                        // put RAS
                        RasEventImpl ras(0x00061006);
                        ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                        string condition = "MONITOR STATUS";

                        switch (mcCMReply._coolMons[cmon]._supplyFlowStatus) {
                        case 1:   condition.append(", SUPPLY HIGH FLOW ALARM"); break;
                        case 2:   condition.append(", SUPPLY LOW FLOW ALARM"); break;
                        case 3:   condition.append(", SUPPLY HIGH FLOW FAULT"); break;
                        case 4:   condition.append(", SUPPLY LOW FLOW FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._returnFlowStatus) {
                        case 1:   condition.append(", RETURN HIGH FLOW ALARM"); break;
                        case 2:   condition.append(", RETURN LOW FLOW ALARM"); break;
                        case 3:   condition.append(", RETURN HIGH FLOW FAULT"); break;
                        case 4:   condition.append(", RETURN LOW FLOW FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._leakStatus) {
                        case 2:   condition.append(", SLOW LEAK DETECTED"); break;
                        case 3:   condition.append(", LEAK DETECTED"); break;
                        case 4:   condition.append(", CATASTROPHIC LEAK DETECTED"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._valveStatus) {
                        case 0:   condition.append(", SOLENOID VALVE CLOSED"); break;
                        case 1:   condition.append(", SOLENOID VALVE OPEN"); break;
                        default: break;
                        }

                        switch (mcCMReply._coolMons[cmon]._supplyTempStatus) {
                        case 1:   condition.append(", SUPPLY HIGH TEMP ALARM"); break;
                        case 2:   condition.append(", SUPPLY LOW TEMP ALARM"); break;
                        case 3:   condition.append(", SUPPLY HIGH TEMP FAULT"); break;
                        case 4:   condition.append(", SUPPLY LOW TEMP FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._returnTempStatus) {
                        case 1:   condition.append(", RETURN HIGH TEMP ALARM"); break;
                        case 2:   condition.append(", RETURN LOW TEMP ALARM"); break;
                        case 3:   condition.append(", RETURN HIGH TEMP FAULT"); break;
                        case 4:   condition.append(", RETURN LOW TEMP FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._supplyPressureStatus) {
                        case 1:   condition.append(", SUPPLY HIGH PRESSURE ALARM"); break;
                        case 2:   condition.append(", SUPPLY LOW PRESSURE ALARM"); break;
                        case 3:   condition.append(", SUPPLY HIGH PRESSURE FAULT"); break;
                        case 4:   condition.append(", SUPPLY LOW PRESSURE FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._diffPressureStatus) {
                        case 1:   condition.append(", DIFF HIGH PRESSURE ALARM"); break;
                        case 2:   condition.append(", DIFF LOW PRESSURE ALARM"); break;
                        case 3:   condition.append(", DIFF HIGH PRESSURE FAULT"); break;
                        case 4:   condition.append(", DIFF LOW PRESSURE FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._ambientTempStatus) {
                        case 1:   condition.append(", AMBIENT HIGH TEMP ALARM"); break;
                        case 2:   condition.append(", AMBIENT LOW TEMP ALARM"); break;
                        case 3:   condition.append(", AMBIENT HIGH TEMP FAULT"); break;
                        case 4:   condition.append(", AMBIENT LOW TEMP FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._humidityStatus) {
                        case 1:   condition.append(", HIGH HUMIDITY ALARM"); break;
                        case 2:   condition.append(", LOW HUMIDITY ALARM"); break;
                        case 3:   condition.append(", HIGH HUMIDITY FAULT"); break;
                        case 4:   condition.append(", LOW HUMIDITY FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._dewpointStatus) {
                        case 1:   condition.append(", HIGH DEW POINT ALARM"); break;
                        case 2:   condition.append(", LOW DEW POINT ALARM"); break;
                        case 3:   condition.append(", HIGH DEW POINT FAULT"); break;
                        case 4:   condition.append(", LOW DEW POINT FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._powerConsumptionStatus) {
                        case 1:   condition.append(", HIGH POWER CONSUMPTION ALARM"); break;
                        case 2:   condition.append(", LOW POWER CONSUMPTION ALARM"); break;
                        case 3:   condition.append(", HIGH POWER CONSUMPTION FAULT"); break;
                        case 4:   condition.append(", LOW POWER CONSUMPTION FAULT"); break;
                        default:;
                        }

                        switch (mcCMReply._coolMons[cmon]._impedanceStatus) {
                        case 1:   condition.append(", HIGH FLOW IMPEDANCE ALARM"); break;
                        case 2:   condition.append(", LOW FLOW IMPEDANCE ALARM"); break;
                        case 3:   condition.append(", HIGH FLOW IMPEDANCE FAULT"); break;
                        case 4:   condition.append(", LOW FLOW IMPEDANCE FAULT"); break;
                        default:;
                        }

                        // remove MODULE_STATUS if something else was found
                        if (condition != "MONITOR STATUS") {
                            condition = condition.substr(16);
                        }

                        // omit RAS event if solenoid open is the only condition
                        if ( condition != "SOLENOID VALVE OPEN" ) {
                            ras.setDetail("COND", condition);
                            RasEventHandlerChain::handle(ras);
                            BGQDB::putRAS(ras);
                        }
                    }

                    if (mcCMReply._coolMons[cmon]._shutoffCauseStatus) {
                        RasEventImpl ras(0x00061014);
                        ras.setDetail(RasEvent::LOCATION, mcCMReply._coolMons[cmon]._lctn.c_str());
                        string condition;
                        switch (mcCMReply._coolMons[cmon]._shutoffCauseStatus) {
                        case 0x1:   condition.append("SUPPLY FLOW LOW"); break;
                        case 0x2:   condition.append("SUPPLY TEMPERATURE LOW"); break;
                        case 0x4:   condition.append("SUPPLY PRESSURE HIGH"); break;
                        case 0x8:   condition.append("SLOW LEAK"); break;
                        case 0x10:  condition.append("CATASTROPHIC LEAK"); break;
                        default:;
                        }
                        ras.setDetail("COND", condition);
                        RasEventHandlerChain::handle(ras);
                        BGQDB::putRAS(ras);
                    }
                }
            }     //  end loop through coolant monitors
        }

        LOG_DEBUG_MSG("Full system health check completed.");
        // **********  END OF HEALTH CHECK LOOP ***************

        if (mode != bgq::utility::performance::Mode::Value::None) {
            duration = boost::posix_time::microsec_clock::local_time() - start;
            perf._duration = static_cast<double>(duration.total_microseconds()) / 1000000;
            rc = tx.insert(&perf);
        }

        //sleep for <seconds>, but wake up every <pollingPeriod> to check if we've been killed
        for (unsigned slept = 0 ; (isThreadStopping() == false) && (slept < seconds) ; slept+= pollingPeriod)
            {
                sleep(pollingPeriod);
            }

        // just in case the performance mode was changed
        if ( mode != bgq::utility::performance::Mode::instance().getMode() ) {
            mode = bgq::utility::performance::Mode::instance().getMode();
        }
    } //  while (isThreadStopping() == false)

    return NULL;
}

void
HealthCheck::getBadWireMasks()
{
    LOG_DEBUG_MSG( __FUNCTION__ );
    const cxxdb::ConnectionPtr database(
            BGQDB::DBConnectionPool::Instance().getConnection()
            );
    if ( !database ) {
        LOG_ERROR_MSG("Could not get database connection." );
        return;
    }

    _skipModules.clear();

    const cxxdb::ResultSetPtr cables(
            database->query(
                "SELECT " +
                BGQDB::DBTCable::FROMLOCATION_COL + ", " +
                BGQDB::DBTCable::TOLOCATION_COL + ", " +
                BGQDB::DBTCable::BADWIREMASK_COL + " " +
                "FROM " + BGQDB::DBTCable().getTableName() + " " +
                "WHERE " + BGQDB::DBTCable::BADWIREMASK_COL + " <> 0"
                )
            );
    if ( !cables ) {
        LOG_DEBUG_MSG("No cables with bad wire masks.");
        return;
    }

    while ( cables->fetch() ) {
        const cxxdb::Columns& columns( cables->columns() );
        const std::string fromPort( columns[ BGQDB::DBTCable::FROMLOCATION_COL ].getString() );
        const std::string toPort( columns[ BGQDB::DBTCable::TOLOCATION_COL ].getString() );
        const int64_t mask( columns[ BGQDB::DBTCable::BADWIREMASK_COL ].getInt64() );

        LOG_DEBUG_MSG(
                fromPort << " --> " << toPort << " mask: " <<
                std::setw(8) << std::setfill('0') << std::hex << "0x" << mask
                );

        const std::vector<std::string> badModules(
                bgq::utility::CableBadWires::getBadOpticalConnections( fromPort, toPort, mask )
                );
        BOOST_FOREACH( const std::string& i, badModules ) {
            if ( _skipModules.insert(i).second ) {
                LOG_DEBUG_MSG( __FUNCTION__ << "() skipping " << i );
            }
        }
    }
}

} } } // namespace mmcs::server::env
