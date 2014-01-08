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

#include "ConsoleController.h"
#include "Database.h"
#include "LiteControlEventListener.h"
#include "MMCSCommand_lite.h"
#include "Options.h"

#include "../MMCSCommandProcessor.h"

#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

#include <bgq_util/include/TempFile.h>
#include <bgq_util/include/string_tokenizer.h>

#include <control/include/bgqconfig/xml/BGQMachineXML.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/assign/list_of.hpp>

#include <iostream>
#include <fstream>

#include <editline/readline.h>

#include <sys/types.h>
#include <arpa/inet.h>

#include <signal.h>


using namespace std;

using mmcs::MMCSCommandProcessor;

using mmcs::common::Properties;


LOG_DECLARE_FILE( "mmcs.lite" );


void
signal_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        return;
    }
}

void
readHistoryFile()
{
    using_history();
    // use process name for unique history file
    std::string result = "." + Properties::getProperty(MMCS_PROCESS) + "_history";
    if ( getenv("HOME") ) {
        std::string home( getenv("HOME") );
        home.append( "/" );
        home.append( result );
        result = home;
    }

    LOG_TRACE_MSG( "using '" << result << "' for editline history" );

    const std::string file( result );
    if ( read_history( file.c_str() ) != 0 ) {
        if ( errno == ENOENT ) {
            // file does not exist, this is not a warning since it will not exists when first creating the
            // history file
        } else {
            LOG_WARN_MSG( "could not read history file '" << file << "' " << strerror(errno) );
        }
    }
}

char*
macOut(int *mac, char *buf)
{
    sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
    return buf;
}

void
macInc(int *mac)
{
    if (mac[5] == 255) {
        mac[5] = 0;
        mac[4]++;
    }
    else
    {
        mac[5]++;
    }
}

void addIoLinks( std::ostream& os );

int
main(int argc, char *argv[])
{
    MMCSCommandProcessor* commandProcessor  = NULL;         // executes mmcs commands
    mmcs::common::ConsoleController* console= NULL;                       // console interface to end user

    try {
        // parse options
        mmcs::lite::Options options( argc, argv );

        // check sqlite thread safety
        LOG_DEBUG_MSG( "sqlite thread safety: " << ( sqlite3_threadsafe() ? "yes" : "no" ) );

        // ensure sqlite key is available
        if ( Properties::getProperty("sqlite").empty() ) {
            cerr << "missing sqlite key in mmcs_lite section of properties file" << endl;
            exit(1);
        }

        // install handler for SIGUSR1
        struct sigaction sa;
        memset( &sa, 0, sizeof(sa) );
        sa.sa_handler = SIG_IGN;
        sa.sa_handler = &signal_handler;
        if ( sigaction( SIGUSR1, &sa, NULL ) != 0 ) {
            LOG_WARN_MSG( "could not install SIGUSR1 handler" );
        }

        // create tables
        LOG_TRACE_MSG( "creating tables" );
        {
            char *zErrMsg = 0;
            int rc = 0;
            mmcs::lite::Database db;
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE EVENTLOG     (recid integer  primary key autoincrement,event_time timestamp   NOT NULL  DEFAULT CURRENT_TIMESTAMP,msgid       CHAR(8),Category    CHAR(16) ,Component CHAR(16) ,Severity     char(8),location    char(64),serialnumber char(19),ecid         char(14) ,jobid      integer,block      char(32),count      integer,message     varchar(512),rawdata     varchar(2048)) ", NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE VIEW  RAS         as select recid,datetime(event_time,'localtime'),msgid,Category,Component ,Severity,location,serialnumber,ecid ,jobid ,block,count,message,rawdata  from EVENTLOG ", NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE JOBS         (jobid integer  primary key autoincrement,blockid      char(32), executable   varchar(256), cwd  varchar(256), ranks integer, args varchar(1024), envs varchar(2048), starttime timestamp, endtime timestamp, exitstatus integer, errortext varchar(1024))" , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE SC_ENVS      (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP,VoltageV12P float, VoltageV12R5 float, VoltageV18P float, VoltageV25phy float, VoltageV25sw float, VoltageV33p float, VoltageV50p float, temp1 integer, temp2 integer,temp3 integer,clockfreq integer, leakfault0 char(1), leakfault1 char(1))" , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE IO_ENVS      (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP,temp integer,tempcritical char(1), tempwarning char(1),voltageV08  float,VoltageV14 float,VoltageV25 float,voltageV33 float,VoltageV120 float, voltageV15 float, VoltageV09 float, voltageV10 float, voltageV120P float,voltageV33P float, voltageV12 float,voltageV18  float,clockfreq integer,alertscompute integer,alertsblink integer,alertsoptic integer,pgoodcompute integer,pgoodblink integer,statusdca integer,statusvtm integer)" , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE FAN_ENVS     (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP, fan integer,rpms integer)"   , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE BULK_ENVS    (cardlocation char(16), location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP,voltagein float,currentin float,voltage51v float,current51v float,voltage5v float,current5v float,temp float, fan1rpm float, fan2rpm float, statusword char(1), statusvout char(1),statusiout char(1),statusinput char(1),statustemp char(1),statuscml char(1),status5v char(1),statusfans char(1))"   , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE NC_ENVS      (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP,temp0 integer, temp1 integer, clockfreq integer,alertscompute integer, alertsblink integer, alertsoptic integer, pgoodcompute integer,pgoodblink integer,statusdca integer,statusvtm123 integer,statusvtm468 integer)", NULL , 0, &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE NC_POWER_ENVS (location char(16),time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP,VoltageV08 float, VoltageV14 float, VoltageV25 float, VoltageV33 float, VoltageV120P float, VoltageV15 float, VoltageV09 float, VoltageV10 float, VoltageV33P float, VoltageV12A float, VoltageV12B float, VoltageV18 float, VoltageV25P float, VoltageV12P float, VoltageV18P float, )", NULL , 0, &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE COMPUTE_ENVS (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP, tempi2c integer,temptvs integer)"   , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE LINK_ENVS    (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP, tempi2c integer,temptvs integer)"   , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE OPTIC_ENVS   (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP, status integer, los integer, losmask integer,faults integer, faultsmask integer, alarmstemp integer, alarmstempmask integer, alarmsvoltage integer, alarmsvoltagemask integer, alarmsbiascurrent bigint, alarmsbiascurrentmask bigint, alarmspower bigint, alarmspowermask bigint, temp integer, voltagev33 integer, voltagev25 integer  )"   , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE OPT_CH_ENVS  (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP, channel integer, biascurrent integer, opticalpower integer  )"   , NULL, 0 , &zErrMsg);
            rc = sqlite3_exec( db.getHandle(), "CREATE TABLE DCA_ENVS     (location char(16), time timestamp  NOT NULL  DEFAULT CURRENT_TIMESTAMP, domain integer, status integer, current integer, voltage integer)"   , NULL, 0 , &zErrMsg);
        }

        // build machine XML
        ostringstream os;
        os << "<BGQMidplaneMachine " ;
        os << " clockHz='" << Properties::getProperty("clockHz")  << "' ";
        os << " bgsysIpv4Address='" << Properties::getProperty("nfsIpAddress")  << "' ";
        os << " bgsysRemotePath='" << Properties::getProperty("nfsExportDir")  << "' ";
        os << " serviceNodeIpv4Address='" << Properties::getProperty("serviceNode")  << "' ";
        os << " computeNodeMemory='" << Properties::getProperty("computeNodeMemory")  << "' ";

        if (Properties::getProperty("computeNodeMemory") == "4096")
        {
            os << " computeDDRModuleSize='2' ";
        }
        else
        {
            os << " computeDDRModuleSize='1' ";
        }

        os << " ioNodeMemory='" << Properties::getProperty("ioNodeMemory")  << "' ";
        if (Properties::getProperty("ioNodeMemory") == "4096")
        {
            os << " ioDDRModuleSize='2' ";
        }
        else
        {
            os << " ioDDRModuleSize='1' ";
        }

        os << " mtu='" << Properties::getProperty("mtu")  << "' ";
        os << " gateway='" << Properties::getProperty("gateway")  << "' ";
        os << " broadcast='" << Properties::getProperty("broadcast")  << "' ";
        os << " ipv4Netmask='" << Properties::getProperty("netmask")  << "' ";
        os << "  >";

        int rows = atoi(Properties::getProperty("rackRows").c_str());
        int cols = atoi(Properties::getProperty("rackColumns").c_str());
        char rowChar[2], colChar[2];
        char ncChar[3];
        int ncNum;

        struct in_addr ip;
        ip.s_addr = inet_addr(Properties::getProperty("ionodeip").c_str());
        int mac[6];
        //char macAddr[18];
        if (6 != sscanf(Properties::getProperty("ionodemac").c_str(), "%x:%x:%x:%x:%x:%x", &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5])) {
            cerr << "mmcs_lite: ionodemac address not valid in property file." << endl;
            exit(1);
        }

        for (int rowNum = 0; rowNum < rows; rowNum++) {
            sprintf(rowChar,"%i",rowNum);
            for (int colNum = 0; colNum < cols; colNum++) {
                sprintf(colChar,"%i",colNum);
                os << "<BGQMidplane posInMachine='R" << string(rowChar) << string(colChar) << "-M0'>" << endl;

                for (ncNum = 0; ncNum < 16; ncNum++) {
                    sprintf(ncChar,"%02i",ncNum);
                    //ip.s_addr++;
                    //macInc(mac);
                    //os << "<BGQNodeCard location='R" << string(rowChar) << string(colChar) << "-M0-N" << string(ncChar) << "'  posInMidplane='N" << string(ncChar) << "'  ip0='" << inet_ntoa(ip) << "' mac0='" <<  string(macOut(mac,macAddr)) << "' ";
                    //ip.s_addr++;
                    //macInc(mac);
                    //os << " ip1='"  << inet_ntoa(ip) << "' mac1='" <<  string(macOut(mac,macAddr)) << "'/> " << endl;
                    os << "<BGQNodeCard location='R" << string(rowChar) << string(colChar) << "-M0-N" << string(ncChar) << "'  posInMidplane='N" << string(ncChar) << "' />  "<< endl;
                }

                os << "</BGQMidplane>" << endl;
                os << "<BGQMidplane posInMachine='R" << string(rowChar) << string(colChar) << "-M1'>" << endl;

                for (ncNum = 0; ncNum < 16; ncNum++) {
                    sprintf(ncChar,"%02i",ncNum);
                    //ip.s_addr++;
                    //macInc(mac);
                    //os << "<BGQNodeCard location='R" << string(rowChar) << string(colChar) << "-M1-N" << string(ncChar) << "'  posInMidplane='N" << string(ncChar) << "'  ip0='" << inet_ntoa(ip) << "' mac0='" <<  string(macOut(mac,macAddr)) << "' ";
                    //ip.s_addr++;
                    //macInc(mac);
                    //os << " ip1='"  << inet_ntoa(ip) << "' mac1='" <<  string(macOut(mac,macAddr)) << "'/> " << endl;
                    os << "<BGQNodeCard location='R" << string(rowChar) << string(colChar) << "-M1-N" << string(ncChar) << "'  posInMidplane='N" << string(ncChar) << "' /> "<< endl;
                 }

                os << "</BGQMidplane>" << endl;
            }
        }

        if ( !Properties::getProperty("ioDrawer").empty() ) {
            os << " <BGQIODrawer";
            os << " posInMachine='" << Properties::getProperty("ioDrawer") << "'>" << endl;
            for (int ionode = 0;ionode < 8; ionode++) {

                //                    macInc(mac);
                //                    os << "<BGQIONode location='" << Properties::getProperty("ioDrawer") << "-J0" << ionode << "' ip='" << inet_ntoa(ip) << "' mac='" << string(macOut(mac,macAddr)) << "' />" << endl;
                os << "<BGQIONode location='" << Properties::getProperty("ioDrawer") << "-J0" << ionode << "' >" << endl;
                os << "<BGQNetConfig interface='external1' ";
                if ( Properties::getProperty("ioInterfaceName").empty() ) {
                    os << " name='ib0' ";
                    LOG_WARN_MSG( "missing ioInterfaceName key in mmcs_lite section of properties file, using ib0 as default" );
                } else {
                    os << " name='" << Properties::getProperty("ioInterfaceName") << "' ";
                }
                os << " ipv4address='" << inet_ntoa(ip) << "' />" << endl;
                os << "</BGQIONode>" << endl;

                ip.s_addr++;

            }
            os << " </BGQIODrawer>" << endl;
        } else {
            LOG_WARN_MSG( "missing ioDrawer key in mmcs_lite section of properties file" );
        }

        addIoLinks( os );

        os << "</BGQMidplaneMachine>" << endl;

        TempFile machineFile("__mmcs_lite.xml.XXXXXX");
        ofstream temp(machineFile.fname);
        temp << os.str() << endl;
        LOG_DEBUG_MSG( "machine XML written to " << machineFile.fname );


        // create the BGQMachineXML object
        BGQMachineXML* machine = NULL;

        try
        {
            machine = BGQMachineXML::create(machineFile.fname);
        }
        catch (const XMLException& e)
        {
            cerr << "mmcs: can't load \"" << machineFile.fname << "\": " << e.what() << endl;
            exit(1);
        }
        if ( !machine)
        {
            cerr << "mmcs: can't load \"" << machineFile.fname << "\"" << endl;
            exit(1);
        }

        // create the list of mmcs_lite commands
        mmcs::common::AbstractCommand::Attributes attr;
        attr.mmcsLiteCommand(true);
        mmcs::common::AbstractCommand::Attributes mask;
        mask.mmcsLiteCommand(true);
        boost::scoped_ptr<mmcs::MMCSCommandMap> mmcsCommands( MMCSCommandProcessor::createCommandMap(attr, mask) );

        // create the mmcs_lite command processor
        commandProcessor = new MMCSCommandProcessor( mmcsCommands.get() );

        // Get the current username
        bgq::utility::UserId uid;

        readHistoryFile();
        console = new mmcs::lite::ConsoleController( commandProcessor, machine, uid );

        mmcs_client::CommandReply reply;

        if (options.getDefaultListener()) {
            // create a default listener
            mmcs::lite::LiteControlEventListener* liteListener = mmcs::lite::LiteControlEventListener::getLiteControlEventListener();

            // establish a socket connection to mcServer
            if (!liteListener->getBase()->isConnected()) {
                liteListener->getBase()->mcserver_connect(reply);
                if (reply.getStatus() != 0) {
                    cout << "default listener connect failed: " << reply.str() << endl;
                    delete liteListener;
                    liteListener = NULL;
                }  else {
                    // Start the generic RAS listener
                    if (!liteListener->getBase()->isMailboxStarted()) {
                        liteListener->getBase()->startMailbox(reply);
                        if (reply.getStatus() != 0) {
                            cout << "default listener start failed: " << reply.str() << endl;
                            delete liteListener;
                            liteListener = NULL;
                        }
                    }
                }
            }
        }

        if (options.doBringup()) {
            // do a bringup
            console->getCommandProcessor()->execute(boost::assign::list_of("bringup"), reply, console);
            if (reply.getStatus() != 0) {
                cout << reply.str() << endl;
                cout << "bringup failed, or hardware from bringup didn't match bg.properties file, exiting... " << endl;
                throw std::runtime_error( "bringup" );
            }
        }

        // execute interactive console
        //
        console->run();
    }
    catch(const exception &e)
    {
        cout << "catch(" << e.what() << ")"<< endl;
    }

    int rc = EXIT_SUCCESS;
    if(console)
        rc = console->quit();

    _exit( rc );
}

void
addIoLinks(
        std::ostream& os
        )
{
    // add a single or multiple links to the machine XML by extracting
    // information from the properties file

    if ( Properties::getProperty("ioLink").empty() ) {
        LOG_DEBUG_MSG( "missing ioLink key in mmcs_lite section of properties file" );

        // look for ioLink sub-section
        const std::string section( "mmcs_lite.ioLink" );
        try {
            const bgq::utility::Properties::ConstPtr properties = Properties::getProperties();
            const bgq::utility::Properties::Section& links = properties->getValues( section );
            BOOST_FOREACH( const bgq::utility::Properties::Pair& i, links ) {
                StringTokenizer tokens;
                tokens.tokenize( i.second, "," );
                // expect format compute=io,port,port
                if ( tokens.size() == 3 ) {
                    os << " <BGQIOLink computeNode='" << i.first
                        << "' ioNode='" << tokens[0]
                        << "' cnConnector='" << tokens[1]
                        << "' ioConnector='" << tokens[2]
                        << "' />" << endl;
                }
                if ( tokens.size() == 4 ) {
                    os << " <BGQIOLink computeNode='" << i.first
                        << "' ioNode='" << tokens[0]
                        << "' cnConnector='" << tokens[1]
                        << "' ioConnector='" << tokens[2] << " >" << endl;
                    os << "  <BGQBadWireMask  fromPort='" << tokens[1] << "' toPort='" <<  tokens[2] << "' mask='" << tokens[3] << "'  />" << endl;
                    os << " </BGQIOLink>" << endl;
                }
            }
        } catch ( const std::invalid_argument& e ) {
            LOG_WARN_MSG( "missing " << section << " section of properties file" );
        }

        return;
    }

    StringTokenizer nodes;
    nodes.tokenize(Properties::getProperty("ioLink"),",");
    if (nodes.size() == 4) {
        os <<
            " <BGQIOLink computeNode='" << nodes[0]
           << "' ioNode='" << nodes[1]
           << "' cnConnector='" << nodes[2]
           << "' ioConnector='" << nodes[3]
           << "' />" << endl;
    } else
        if (nodes.size() == 5) {
            os <<
                " <BGQIOLink computeNode='" << nodes[0]
               << "' ioNode='" << nodes[1]
               << "' cnConnector='" << nodes[2]
               << "' ioConnector='" << nodes[3] << " >" << endl;
            os << "  <BGQBadWireMask  fromPort='" << nodes[2] << "' toPort='" <<  nodes[3] << "' mask='" << nodes[4] << "'  />" << endl;
            os << " </BGQIOLink>" << endl;

        } else {
            LOG_WARN_MSG( "invalid number of tokens (" << nodes.size() << ") in ioLink key, expected 4 or 5" );
        }

}
