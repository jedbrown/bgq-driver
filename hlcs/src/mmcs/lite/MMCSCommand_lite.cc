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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file MMCSCommand_lite.cc
 * \brief Commands specific to mmcs_lite.
 */

#include "MMCSCommand_lite.h"

#include "BlockController.h"
#include "ConsoleController.h"

#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include "common/ConsoleController.h"
#include "common/Properties.h"

#include "lite/Database.h"
#include "lite/Job.h"

#include "libmmcs_client/CommandReply.h"

#include <control/include/bgqconfig/xml/BGQMachineXML.h>
#include <control/include/mc/cardsDefs.h>
#include <control/include/mcServer/MCServerAPIHelpers.h>
#include <control/include/mcServer/MCServerRef.h>

#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>

#include <fstream>


using namespace std;

using mmcs::common::Properties;

using mmcs::server::BlockControllerTarget;
using mmcs::server::BlockPtr;


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


FILE *ras;

static int
rs_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for(i=0; i<argc; i++){
        //   printf(" %s  ",  argv[i] ? argv[i] : "NULL");
        fprintf(ras," %s  ",  argv[i] ? argv[i] : "NULL");
        if (i < (argc - 1))
            fprintf(ras,",");
    }
    //    printf("\n");
    fprintf(ras,"\n");

    return 0;
}



MMCSCommand_gen_block*
MMCSCommand_gen_block::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_gen_block("gen_block", "gen_block <blockid> <midplane> <cnodes> <nodecard> [ <node> ] [ options ]", commandAttributes);
}

void
MMCSCommand_gen_block::execute(deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    char ncstr[16][4] =
        {"N00","N01","N02","N03","N04","N05","N06","N07","N08","N09","N10","N11","N12","N13","N14","N15"};
    char ndstr[32][4] =
        {"J00","J01","J02","J03","J04","J05","J06","J07","J08","J09","J10","J11","J12","J13","J14","J15",
         "J16","J17","J18","J19","J20","J21","J22","J23","J24","J25","J26","J27","J28","J29","J30","J31"};
    char e0nodes[16][4] =
        {"J00","J01","J02","J03","J12","J13","J14","J15","J16","J17","J18","J19","J28","J29","J30","J31"};
    char e1nodes[16][4] =
        {"J04","J05","J06","J07","J08","J09","J10","J11","J20","J21","J22","J23","J24","J25","J26","J27"};

    deque<string>::iterator arg;
    deque<string> blockArgs;

    if (args.size() < 4) {
        reply << mmcs_client::FAIL << "not enough args " << _usage << mmcs_client::DONE;
        return;
    }

    std::string blockName = args[0];
    if ( blockName.size() > 32 ) {
        reply << mmcs_client::FAIL << "block name must be 32 characters or less" << mmcs_client::DONE;
        return;
    }

    bool E0 = false, E1 = false;
    if ((args.size() == 5) && (args[4] == "E0")) {
        args.pop_back(); // take off the argument
        E0 = true;
    } else  if ((args.size() == 5) && (args[4] == "E1")) {
        args.pop_back(); // take off the argument
        E1 = true;
    }

    std::stringstream os;
    os << "<BGQBlock name='" << args[0] << "'   securityKey='FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF' ";

    if (!Properties::getProperty("boot_options").empty()) {
        os << "boot_options='" << Properties::getProperty("boot_options") << "' ";
    }

    os << ">" << endl ;


    os << "<BGQMidplane midplane='" << args[1] << "'>" << endl ;

    // get compute node count
    unsigned cNodes = 0;
    try {
        cNodes = boost::lexical_cast<unsigned>( args[2] );
    } catch ( const boost::bad_lexical_cast& e ) {
        reply << mmcs_client::FAIL << "Number of compute nodes must be a valid number;" << _usage << mmcs_client::DONE;
        return;
    }

    // validate compute node count
    if ((args.size() == 4) && (cNodes != 512) && (cNodes != 256) && (cNodes != 128) && (cNodes != 64) && (cNodes != 32)) {
        reply << mmcs_client::FAIL << "Number of compute nodes must be 512, 256, 128, 64, or 32;Unless compute node is provided;" << _usage << mmcs_client::DONE;
        return;
    }

    // validate compute node count
    if ((args.size() > 4) && (cNodes != 16) && (cNodes != 8) && (cNodes != 4) && (cNodes != 2) && (cNodes != 1)) {
        reply << mmcs_client::FAIL << "Number of compute nodes must be 1, 2, 4, 8, or 16 if compute node is provided;" << _usage << mmcs_client::DONE;
        return;
    }

    // get starting node board
    unsigned startNb = 0;
    if ( args[3].size() < 3 ) {
        reply << mmcs_client::FAIL << "Node Board must be one of N00 through N15;" << _usage << mmcs_client::DONE;
        return;
    }
    try {
        startNb = boost::lexical_cast<unsigned>( args[3].substr(1,2) );
    } catch ( const boost::bad_lexical_cast& e ) {
        reply << mmcs_client::FAIL << "Node Board must be one of N00 through N15;" << _usage << mmcs_client::DONE;
        return;
    }

    // generate block xml
    if (cNodes < 32) {
        unsigned startNode = 0;
        if ( args[4].size() < 3 ) {
            reply << mmcs_client::FAIL << "Node must be one of J00 through J31;" << _usage << mmcs_client::DONE;
            return;
        }
        try {
            startNode = boost::lexical_cast<unsigned>( args[4].substr(1,2) );
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << mmcs_client::FAIL << "Node must be one of J00 through J31;" << _usage << mmcs_client::DONE;
            return;
        }

        if ((startNode % cNodes) != 0) {
            reply << mmcs_client::FAIL << "Invalid starting node for block size;" << _usage << mmcs_client::DONE;
            return;
        }

        for ( unsigned node = startNode, cn = 0  ; cn < cNodes ; ++node, cn+=1) {
            os << "<BGQComputeNodes board='"  << ncstr[startNb] << "' card='" << ndstr[node] << "' />";
        }

        args.pop_front(); // take off the node
    } else {
        for ( unsigned nc = startNb, cn = 0  ; cn < cNodes ; ++nc, cn+=32) {
            if (E0) {
                for ( unsigned node = 0  ; node < 16 ; ++node) {
                    os << "<BGQComputeNodes board='"  << ncstr[nc] << "' card='" << ( (nc<8) ? e0nodes[node] : e1nodes[node]) << "' />";
                }
            } else  if (E1) {
                for ( unsigned node = 0  ; node < 16 ; ++node) {
                    os << "<BGQComputeNodes board='"  << ncstr[nc] << "' card='" << ( (nc<8) ? e1nodes[node] : e0nodes[node]) << "' />";
                }
            }  else {
                os << "<BGQComputeNodes board='"  << ncstr[nc] << "' />";
            }
        }
    }

    if ((cNodes >= 64) && (!Properties::getProperty("computeTorus").empty()))
        os << " <BGQSwitch axis='C' include='F' enablePorts='B'/> " << endl;
    if ((cNodes >= 128) && (!Properties::getProperty("computeTorus").empty()))
        os << " <BGQSwitch axis='D' include='F' enablePorts='B'/> " << endl;
    if ((cNodes >= 256) && (!Properties::getProperty("computeTorus").empty()))
        os << " <BGQSwitch axis='A' include='F' enablePorts='B'/> " << endl;
    if ((cNodes == 512) && (!Properties::getProperty("computeTorus").empty()))
        os << " <BGQSwitch axis='B' include='F' enablePorts='B'/> " << endl;


    os << "</BGQMidplane></BGQBlock>" << endl;
    LOG_TRACE_MSG( os.str() );

    args.pop_front(); // take off the block
    args.pop_front(); // take off the midplane
    args.pop_front(); // take off the cnodes
    args.pop_front(); // take off the nodecards

    if (cNodes < 32) {
        args.push_front("shared");  // mmcs_lite users can share the same node board
    }

    if(((lite::ConsoleController*)(pController))->blockInitialized()) {
        reply << mmcs_client::FAIL << "Only a single block allowed in mmcs_lite" << mmcs_client::DONE;
    }
    else ((lite::ConsoleController*)(pController))->genCNBlockController(args, blockName, os, reply);

    if(reply.str() == "args?")
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
}

void
MMCSCommand_gen_block::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";define a block of nodes to be controlled by mmcs"
        << ";Specify <midplane> by location, i.e. Rxx-Mx."
        << ";The <cnodes> is the total number of compute nodes and must be 32, 64, 128, 256, or 512."
        << ";The <cnodes> can be 1, 2, 4, 8, or 16 if <node> is provided."
        << ";The <nodecard> is the location of the compute nodes for the block, i.e. N00, N01, etc."
        << mmcs_client::DONE;
}


MMCSCommand_gen_io_block*
MMCSCommand_gen_io_block::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_gen_io_block("gen_io_block", "gen_io_block <blockid> <drawer> <ionodes> <node> [ options ]", commandAttributes);
}

void
MMCSCommand_gen_io_block::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    char nodestr[8][4] = {"J00","J01","J02","J03","J04","J05","J06","J07"};
    deque<string>::iterator arg;
    deque<string> blockArgs;

    if (args.size() < 4) {
        reply << mmcs_client::FAIL << "not enough args;" << _usage << mmcs_client::DONE;
        return;
    }

    // get node count
    unsigned ioNodes = 0;
    try {
        ioNodes = boost::lexical_cast<unsigned>( args[2] );
    } catch ( const boost::bad_lexical_cast& e ) {
        reply << mmcs_client::FAIL << "Number of io nodes must be from 1 to 8;" << _usage << mmcs_client::DONE;
        return;
    }

    // validate node count
    if ((ioNodes < 1) || (ioNodes > 8)) {
        reply << mmcs_client::FAIL << "Number of io nodes must be from 1 to 8;" << _usage << mmcs_client::DONE;
        return;
    }

    // get starting I/O node
    unsigned startNode = 0;
    if ( args[3].size() < 3 ) {
        reply << mmcs_client::FAIL << "I/O node must be between J00 and J07;" << _usage << mmcs_client::DONE;
        return;
    }
    try {
        startNode = boost::lexical_cast<unsigned>( args[3].substr(1,2) );
    } catch ( const boost::bad_lexical_cast& e ) {
        reply << mmcs_client::FAIL << "I/O node must be between J00 and J07;" << _usage << mmcs_client::DONE;
        return;
    }

    // validate starting I/O node
    if ( (startNode + ioNodes) > 8 ) {
        reply << mmcs_client::FAIL << "Number of io nodes and starting node goes beyond 8;" << _usage << mmcs_client::DONE;
        return;
    }

    // validate block name
    std::string blockName = args[0];
    if ( blockName.size() > 32 ) {
        reply << mmcs_client::FAIL << "block name must be 32 characters or less" << mmcs_client::DONE;
        return;
    }

    std::stringstream os;
    os << "<BGQBlock name='" << args[0] << "' numionodes='" << ioNodes << "' securityKey='FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF' ";

    if (!Properties::getProperty("boot_options").empty()) {
        os << "boot_options='" << Properties::getProperty("boot_options") << "' ";
    }

    os << ">" << endl ;

    for (unsigned nd = startNode, ion = 0 ; ion < ioNodes ; ++nd, ++ion) {
        os << "<BGQIONodes location='"  << args[1] << "-" << nodestr[nd] << "' />";
    }

    if ((ioNodes == 8) && (!Properties::getProperty("ioTorus").empty())) {
        os << " <BGQSwitch axis='A' include='F' enablePorts='B'/> " << endl;
        os << " <BGQSwitch axis='B' include='F' enablePorts='B'/> " << endl;
        os << " <BGQSwitch axis='C' include='F' enablePorts='B'/> " << endl;
    }

    os << "</BGQBlock>" << endl;
    LOG_TRACE_MSG( os.str() );

    args.pop_front(); // take off the block
    args.pop_front(); // take off the drawer
    args.pop_front(); // take off the ionodes
    args.pop_front(); // take off the node

    if (ioNodes < 8) {
        args.push_front("shared");  // mmcs_lite users can share the same IO or node board
    }

    if(((lite::ConsoleController*)(pController))->blockInitialized()) {
        reply << mmcs_client::FAIL << "Only a single block allowed in mmcs_lite" << mmcs_client::DONE;
    }
    else((lite::ConsoleController*)(pController))->genIOBlockController(args, blockName, os, reply);

    if(reply.str() == "args?")
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
}

void
MMCSCommand_gen_io_block::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
          << ";define a block of io nodes to be controlled by mmcs"
          << ";Specify <drawer> by location, i.e. Qxx-Ix."
          << ";The <ionodes> is the total number of io nodes and must be from 1 to 8."
          << ";The <node> is the location of the starting io node for the block, i.e. J00, J01, etc."
          << mmcs_client::DONE;
}

MMCSCommand_dump_ras*
MMCSCommand_dump_ras::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(false);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_dump_ras("dump_ras", "dump_ras <filename>", commandAttributes);
}


void
MMCSCommand_dump_ras::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{

    if (args.size() < 1) {
        reply << mmcs_client::FAIL << "not enough args: " << _usage << mmcs_client::DONE;
        return;
    }

    lite::Database db;
    char *zErrMsg = 0;
    char sqlstr[2048];
    int rc;

    ras = fopen(args[0].c_str(), "a");

    if (ras) {
        sprintf(sqlstr, "select *  from ras");
        rc = sqlite3_exec(db.getHandle(), sqlstr, rs_callback, 0 , &zErrMsg);
        fclose(ras);
    } else {
        reply << mmcs_client::FAIL << "could not open file " << args[0] << " for output" << mmcs_client::DONE;
        return;
    }

    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "no stored ras "  << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
MMCSCommand_dump_ras::help(deque<string> args,
        mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
        << ";dumps stored mmcs_lite ras to a file"
        << mmcs_client::DONE;
}


MMCSCommand_show_envs*
MMCSCommand_show_envs::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(false);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_show_envs("show_envs", "show_envs <io|service|bulk|node> <location>", commandAttributes);
}

void
MMCSCommand_show_envs::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    if (args.size() < 2) {
        reply << mmcs_client::FAIL << "not enough args: " << _usage << mmcs_client::DONE;
        return;
    }

    // connect to mcserver
    MCServerRef* temp = NULL;
    const BlockPtr block = pController->getBlockHelper()->getBase();
    block->mcserver_connect( temp, "mmcs_lite", reply );
    const boost::scoped_ptr<MCServerRef> server( temp );
    if ( reply.getStatus() ) return;

    // make the target set
    MCServerMessageSpec::MakeTargetSetRequest   mcMakeRequest("mmcs_lite","mmcs_lite",  true);
    mcMakeRequest._expression.push_back(args[1]);   // use the argument passed by the user for the location
    MCServerMessageSpec::MakeTargetSetReply     mcMakeReply;
    server->makeTargetSet(mcMakeRequest, mcMakeReply);

    // open the target set
    MCServerMessageSpec::OpenTargetRequest   mcOpenRequest( "mmcs_lite","mmcs_lite", MCServerMessageSpec::RAAW  , true);
    MCServerMessageSpec::OpenTargetReply     mcOpenReply;
    server->openTarget(mcOpenRequest, mcOpenReply);
    if (mcOpenReply._rc != 0) {
        reply << mmcs_client::FAIL << "unable to open target set, return code: " << mcOpenReply._rc << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::OK;

    bool haveData = false;

    if (args[0]  == "io") {  // io cards
        // read values from cards
        MCServerMessageSpec::ReadIoCardEnvRequest mcIORequest;
        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcIORequest );
        MCServerMessageSpec::ReadIoCardEnvReply mcIOReply;
        server->readIoCardEnv(mcIORequest, mcIOReply);

        for(unsigned io = 0 ; io < mcIOReply._ioCards.size() ; ++io) {
            haveData = true;
            if (mcIOReply._ioCards[io]._error != 0) {
                reply << mcIOReply._ioCards[io]._lctn << ": error" << endl;
            } else {
                reply << mcIOReply._ioCards[io]._lctn << " voltages: " << endl;
                reply << " 0.8V Rail: "      << mcIOReply._ioCards[io]._powerRailV08Voltage << endl;
                reply << " 1.4V Rail: "      << mcIOReply._ioCards[io]._powerRailV14Voltage << endl;
                reply << " 2.5V Rail: "      << mcIOReply._ioCards[io]._powerRailV25Voltage << endl;
                reply << " 3.3V Rail: "      << mcIOReply._ioCards[io]._powerRailV33Voltage << endl;
                reply << "12.0V Rail: "      << mcIOReply._ioCards[io]._powerRailV120Voltage << endl;
                reply << " 1.5V Rail: "      << mcIOReply._ioCards[io]._powerRailV15Voltage << endl;
                reply << " 0.9V Rail: "      << mcIOReply._ioCards[io]._powerRailV09Voltage << endl;
                reply << " 1.0V Rail: "      << mcIOReply._ioCards[io]._powerRailV10Voltage << endl;
                reply << "12.0V Pers Rail: " << mcIOReply._ioCards[io]._powerRailV120PVoltage << endl;
                reply << " 3.3V Pers Rail: " << mcIOReply._ioCards[io]._powerRailV33PVoltage << endl;
                reply << " 1.2V Rail: "      << mcIOReply._ioCards[io]._powerRailV12Voltage << endl;
                reply << " 1.8V Rail: "      << mcIOReply._ioCards[io]._powerRailV18Voltage << endl;
                reply << endl;
                reply <<  " Temperature (ext):       " << mcIOReply._ioCards[io]._onboardTemp << endl;
                reply <<  " Temperature (local):     " << mcIOReply._ioCards[io]._onboardTempLocal << endl;
                reply <<  " Link chip critical temp: " <<   (mcIOReply._ioCards[io]._tempCriticalBlink ? "T" : "F") << endl;
                reply <<  " Link chip warning  temp: " <<   (mcIOReply._ioCards[io]._tempWarningBlink ? "T" : "F") << endl;
                reply <<  " Clock Frequency: " << mcIOReply._ioCards[io]._clockFreq << endl;
                reply <<  " Compute alerts:  " << mcIOReply._ioCards[io]._alertsComputes << endl;
                reply <<  " Blink alerts:    " << mcIOReply._ioCards[io]._alertsBlinks << endl;
                reply <<  " Optics alerts:   " << mcIOReply._ioCards[io]._alertsOptics << endl;
                reply <<  " Compute pgood:   " << mcIOReply._ioCards[io]._pgoodComputes << endl;
                reply <<  " Blink pgood:     " << mcIOReply._ioCards[io]._pgoodBlinks << endl;
                reply <<  " Status of DCA:   " << mcIOReply._ioCards[io]._statusDca << endl;
                reply <<  " Status of VTMs:  " << mcIOReply._ioCards[io]._statusVtmDomains << endl;
                reply << endl;

                lite::Database db;
                char *zErrMsg = 0;

                // loop through fans
                reply << mcIOReply._ioCards[io]._lctn << " Fan speeds (in RPMs): " << endl;
                for(unsigned fan = 0 ; fan < mcIOReply._ioCards[io]._fanRpms.size() ; ++fan) {
                    reply << " fan " << fan << ": " << mcIOReply._ioCards[io]._fanRpms[fan]._rpmsFans << endl;

                    std::ostringstream insstr;
                    insstr << "insert into FAN_ENVS (location, fan, rpms) values('" <<
                        mcIOReply._ioCards[io]._lctn << "',"  << fan << " , " <<
                        mcIOReply._ioCards[io]._fanRpms[fan]._rpmsFans << ")";
                    (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                }
                reply << endl;

                // loop through computes
                reply << " Computes: " << endl;
                for(unsigned compute = 0 ; compute < mcIOReply._ioCards[io]._computes.size() ; ++compute) {
                    if ( mcIOReply._ioCards[io]._computes[compute]._error) {
                        reply << mcIOReply._ioCards[io]._computes[compute]._lctn << ": error" << endl;
                    } else {
                        reply << mcIOReply._ioCards[io]._computes[compute]._lctn << ":" << endl;
                        reply << "  Temperature (I2C): " << mcIOReply._ioCards[io]._computes[compute]._tempI2c << endl;
                        reply << "  Temperature (TVS): " << mcIOReply._ioCards[io]._computes[compute]._tempTvSense << endl;

                        std::ostringstream insstr;
                        insstr << "insert into COMPUTE_ENVS (location, tempi2c,temptvs) values('" <<
                            mcIOReply._ioCards[io]._computes[compute]._lctn << "',"  <<
                            mcIOReply._ioCards[io]._computes[compute]._tempI2c         << ", " <<
                            mcIOReply._ioCards[io]._computes[compute]._tempTvSense     << " ) ";

                        (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                    }
                }
                reply << endl;

                // loop through link chips
                reply << " Link Chips: " << endl;
                for(unsigned blink = 0 ; blink < mcIOReply._ioCards[io]._blinks.size() ; ++blink) {
                    if (mcIOReply._ioCards[io]._blinks[blink]._error) {
                        reply << mcIOReply._ioCards[io]._blinks[blink]._lctn << ": error" << endl;
                    } else {
                        reply << mcIOReply._ioCards[io]._blinks[blink]._lctn << ":" << endl;
                        reply << "  Temperature (I2C): " << mcIOReply._ioCards[io]._blinks[blink]._tempI2c << endl;
                        reply << "  Temperature (TVS): " << mcIOReply._ioCards[io]._blinks[blink]._tempTvSense << endl;

                        std::ostringstream insstr;
                        insstr << "insert into LINK_ENVS (location, tempi2c,temptvs) values('" <<
                            mcIOReply._ioCards[io]._blinks[blink]._lctn << "',"  <<
                            mcIOReply._ioCards[io]._blinks[blink]._tempI2c         << ", " <<
                            mcIOReply._ioCards[io]._blinks[blink]._tempTvSense     << " ) ";

                        (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                    }
                }
                reply << endl;

                // loop through optics
                reply << " Optics: " << endl;
                for(unsigned optic = 0 ; optic < mcIOReply._ioCards[io]._optics.size() ; ++optic) {
                    if (mcIOReply._ioCards[io]._optics[optic]._error) {
                        reply << mcIOReply._ioCards[io]._optics[optic]._lctn << ": error" << endl;
                    } else {
                        reply << mcIOReply._ioCards[io]._optics[optic]._lctn << ":" << endl;
                        reply << "  Status Flag:          " << (uint)mcIOReply._ioCards[io]._optics[optic]._status << endl;
                        reply << "  Loss of signal (los): " << mcIOReply._ioCards[io]._optics[optic]._los << endl;
                        reply << "  Mask for los:         " << mcIOReply._ioCards[io]._optics[optic]._losMask << endl;
                        reply << "  Faults:               " << mcIOReply._ioCards[io]._optics[optic]._faults << endl;
                        reply << "  Mask for Faults:      " << mcIOReply._ioCards[io]._optics[optic]._faultsMask << endl;
                        reply << "  Temp Alarms:          " << mcIOReply._ioCards[io]._optics[optic]._alarmsTemp << endl;
                        reply << "  Mask for Temp Alarms: " << mcIOReply._ioCards[io]._optics[optic]._alarmsTempMask << endl;
                        reply << "  Voltage Alarms:       " << mcIOReply._ioCards[io]._optics[optic]._alarmsVoltage << endl;
                        reply << "  Mask for Voltage Alarms:      " << mcIOReply._ioCards[io]._optics[optic]._alarmsVoltageMask << endl;
                        reply << "  Bias Current Alarms:  " << mcIOReply._ioCards[io]._optics[optic]._alarmsBiasCurrent << endl;
                        reply << "  Mask for Bias Current Alarms: " << mcIOReply._ioCards[io]._optics[optic]._alarmsBiasCurrentMask << endl;
                        reply << "  Power Alarms:         " << mcIOReply._ioCards[io]._optics[optic]._alarmsPower << endl;
                        reply << "  Mask for Power Alarms:" << mcIOReply._ioCards[io]._optics[optic]._alarmsPowerMask << endl;
                        reply << "  Temperature:          " << (uint)mcIOReply._ioCards[io]._optics[optic]._temp << endl;
                        reply << "  Voltage (3.3v):       " << mcIOReply._ioCards[io]._optics[optic]._voltageV33 << endl;
                        reply << "  Voltage (2.5v):       " << mcIOReply._ioCards[io]._optics[optic]._voltageV25 << endl;
                        reply << "  Optical Channels:     " << endl;

                        std::ostringstream insstr;
                        insstr << "insert into OPTIC_ENVS (location, status, los, losmask, faults , faultsmask, alarmstemp, alarmstempmask, alarmsvoltage, alarmsvoltagemask, alarmsbiascurrent, alarmsbiascurrentmask, alarmspower, alarmspowermask, temp , voltagev33 , voltagev25 ) values('" <<
                            mcIOReply._ioCards[io]._optics[optic]._lctn << "',"  <<
                            (uint)mcIOReply._ioCards[io]._optics[optic]._status << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._los << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._losMask << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._faults << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._faultsMask << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsTemp << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsTempMask << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsVoltage << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsVoltageMask << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsBiasCurrent << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsBiasCurrentMask << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsPower << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._alarmsPowerMask << "," <<
                            (uint)mcIOReply._ioCards[io]._optics[optic]._temp << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._voltageV33 << "," <<
                            mcIOReply._ioCards[io]._optics[optic]._voltageV25 << ")";

                        (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);

                        for(unsigned channel = 0 ; channel < mcIOReply._ioCards[io]._optics[optic]._OpticalEnvChannel.size() ; ++channel) {
                            reply << " Channel: " << channel << endl;
                            reply << "  Bias Current:         " << mcIOReply._ioCards[io]._optics[optic]._OpticalEnvChannel[channel]._biasCurrent << endl;
                            reply << "  Optical Power:        " << mcIOReply._ioCards[io]._optics[optic]._OpticalEnvChannel[channel]._opticalPower << endl;

                            std::ostringstream chstr;
                            chstr << "insert into OPT_CH_ENVS (location, channel, biascurrent, opticalpower) values('" <<
                                mcIOReply._ioCards[io]._optics[optic]._lctn << "',"  << channel << "," <<
                                mcIOReply._ioCards[io]._optics[optic]._OpticalEnvChannel[channel]._biasCurrent << "," <<
                                mcIOReply._ioCards[io]._optics[optic]._OpticalEnvChannel[channel]._opticalPower << ")";

                            (void)sqlite3_exec(db.getHandle(), chstr.str().c_str(), NULL, 0 , &zErrMsg);
                        }
                    }
                }
                reply << endl;

                // loop through DCAs
                reply << " DCAs: " << endl;
                for(unsigned dca = 0 ; dca < mcIOReply._ioCards[io]._dcas.size() ; ++dca) {
                    if (mcIOReply._ioCards[io]._dcas[dca]._error) {
                        reply << mcIOReply._ioCards[io]._dcas[dca]._lctn << ": error" << endl;
                    } else {
                        reply << mcIOReply._ioCards[io]._dcas[dca]._lctn << ":" << endl;
                        for(unsigned pd = 0 ; pd < mcIOReply._ioCards[io]._dcas[dca]._domains.size() ; ++pd) {
                            reply << "  Domain:      " << mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._domain << endl;
                            reply << "   Status:     " << (uint)mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._status << endl;
                            reply << "   Voltage:    " << mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._voltage << endl;
                            reply << "   Current:    " << mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._current << endl;

                            std::ostringstream insstr;
                            insstr << "insert into DCA_ENVS (location, domain, status, voltage, current) values('" <<
                                mcIOReply._ioCards[io]._dcas[dca]._lctn << "',"  <<
                                mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._domain <<  "," <<
                                (uint)mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._status <<  "," <<
                                mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._voltage <<  "," <<
                                mcIOReply._ioCards[io]._dcas[dca]._domains[pd]._current <<  ")";

                            (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                        }
                    }
                }
                reply << endl;

                std::ostringstream sqlstr;
                sqlstr
                    << "insert into IO_ENVS  (location, temp,tempcritical,tempwarning,  voltageV08,VoltageV14,VoltageV25,voltageV33,VoltageV120, "
                    << " voltageV15, VoltageV09, voltageV10, voltageV120P,voltageV33P, voltageV12,voltageV18, "
                    << "clockfreq,alertscompute,alertsblink,alertsoptic,pgoodcompute,pgoodblink,statusdca,statusvtm) values('" <<
                    mcIOReply._ioCards[io]._lctn << "',"  <<
                    mcIOReply._ioCards[io]._onboardTemp << "," <<
                    (mcIOReply._ioCards[io]._tempCriticalBlink ? "'T'," : "'F',") <<
                    (mcIOReply._ioCards[io]._tempWarningBlink  ? "'T'," : "'F',") <<
                    mcIOReply._ioCards[io]._powerRailV08Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV14Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV25Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV33Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV120Voltage  << "," <<
                    mcIOReply._ioCards[io]._powerRailV15Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV09Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV10Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV120PVoltage << "," <<
                    mcIOReply._ioCards[io]._powerRailV33PVoltage  << "," <<
                    mcIOReply._ioCards[io]._powerRailV12Voltage   << "," <<
                    mcIOReply._ioCards[io]._powerRailV18Voltage   << "," <<
                    mcIOReply._ioCards[io]._clockFreq             << "," <<
                    mcIOReply._ioCards[io]._alertsComputes        << "," <<
                    mcIOReply._ioCards[io]._alertsBlinks          << "," <<
                    mcIOReply._ioCards[io]._alertsOptics          << "," <<
                    mcIOReply._ioCards[io]._pgoodComputes         << "," <<
                    mcIOReply._ioCards[io]._pgoodBlinks           << "," <<
                    mcIOReply._ioCards[io]._statusDca             << "," <<
                    mcIOReply._ioCards[io]._statusVtmDomains      << ")";

                (void)sqlite3_exec(db.getHandle(), sqlstr.str().c_str(), NULL, 0 , &zErrMsg);

            }
        }
    }

    if (args[0]  == "service") {  // service card
        // read values from cards
        MCServerMessageSpec::ReadServiceCardEnvRequest mcSCRequest;
        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcSCRequest );
        MCServerMessageSpec::ReadServiceCardEnvReply mcSCReply;
        server->readServiceCardEnv(mcSCRequest, mcSCReply);

        for(unsigned sc = 0 ; sc < mcSCReply._serviceCards.size() ; ++sc) {
            haveData = true;
            if (mcSCReply._serviceCards[sc]._error == true) {
                reply << mcSCReply._serviceCards[sc]._location << ": error" << endl;
            } else {
                reply << mcSCReply._serviceCards[sc]._location << " voltages: " << endl;

                reply << " 1.2V Pers Rail: " << mcSCReply._serviceCards[sc]._powerRailV12PVoltage << endl;
                reply << " 1.2V R5 Rail: "   << mcSCReply._serviceCards[sc]._powerRailV12R5Voltage << endl;
                reply << " 1.5V Pers Rail: " << mcSCReply._serviceCards[sc]._powerRailV15PVoltage << endl;
                reply << " 2.5V Pers Rail: " <<	mcSCReply._serviceCards[sc]._powerRailV25PVoltage << endl;
                reply << " 3.3V Pers Rail: " << mcSCReply._serviceCards[sc]._powerRailV33PVoltage << endl;
                reply << " 5.0V Pers Rail: " << mcSCReply._serviceCards[sc]._powerRailV50PVoltage << endl;

                reply << mcSCReply._serviceCards[sc]._location << " external temperatures: " << endl;

                reply << " Temp sensor (port 05): " << mcSCReply._serviceCards[sc]._onboardTemp1 << endl;
                reply << " Temp sensor (port 10): " << mcSCReply._serviceCards[sc]._onboardTemp2 << endl;
                reply << " Temp sensor (port 11): " << mcSCReply._serviceCards[sc]._onboardTemp3 << endl;

                reply << endl;

                reply << mcSCReply._serviceCards[sc]._location << " local temperatures: " << endl;

                reply << " Temp sensor (port 05): " << mcSCReply._serviceCards[sc]._onboardTemp1Local << endl;
                reply << " Temp sensor (port 10): " << mcSCReply._serviceCards[sc]._onboardTemp2Local << endl;
                reply << " Temp sensor (port 11): " << mcSCReply._serviceCards[sc]._onboardTemp3Local << endl;

                reply << endl;

                reply << " Clock frequency:       " << mcSCReply._serviceCards[sc]._clockFreq << endl;
                reply << " Leak Detector 0 fault: " << (mcSCReply._serviceCards[sc]._leakDetectorFaultBpe0 ? "T" : "F") << endl;
                reply << " Leak Detector 1 fault: " << (mcSCReply._serviceCards[sc]._leakDetectorFaultBpe1 ? "T" : "F") << endl;

                lite::Database db;
                char *zErrMsg = 0;
                std::ostringstream sqlstr;
                sqlstr <<  "insert into SC_ENVS  (location, " <<
                    " VoltageV12P,VoltageV12r5,VoltageV15P,VoltageV25p,VoltageV33P,Voltagev50p,  " <<
                    " temp1,temp2,temp3,clockfreq, leakfault0, leakfault1)  values('" <<
                    mcSCReply._serviceCards[sc]._location              << "'," <<
                    mcSCReply._serviceCards[sc]._powerRailV12PVoltage  << ","  <<
                    mcSCReply._serviceCards[sc]._powerRailV12R5Voltage << ","  <<
                    mcSCReply._serviceCards[sc]._powerRailV15PVoltage  << ","  <<
                    mcSCReply._serviceCards[sc]._powerRailV25PVoltage  << ","  <<
                    mcSCReply._serviceCards[sc]._powerRailV33PVoltage  << ","  <<
                    mcSCReply._serviceCards[sc]._powerRailV50PVoltage  << ","  <<
                    mcSCReply._serviceCards[sc]._onboardTemp1          << ","  <<
                    mcSCReply._serviceCards[sc]._onboardTemp2          << ","  <<
                    mcSCReply._serviceCards[sc]._onboardTemp3          << ","  <<
                    mcSCReply._serviceCards[sc]._clockFreq             << ","  <<
                    (mcSCReply._serviceCards[sc]._leakDetectorFaultBpe0 ? "'T'" : "'F'")     << ","  <<
                    (mcSCReply._serviceCards[sc]._leakDetectorFaultBpe1 ? "'T'" : "'F'")     << ")";
                (void)sqlite3_exec(db.getHandle(), sqlstr.str().c_str(), NULL, 0 , &zErrMsg);
            }
        }
    }

    if (args[0]  == "bulk") {  // bulk power
        // read values from cards
        MCServerMessageSpec::ReadBulkPowerEnvRequest mcBPRequest;
        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcBPRequest );
        MCServerMessageSpec::ReadBulkPowerEnvReply   mcBPReply;
        server->readBulkPowerEnv(mcBPRequest, mcBPReply);

        for(unsigned bp = 0 ; bp < mcBPReply._bpms.size() ; ++bp) {
            haveData = true;
            if (mcBPReply._bpms[bp]._error == true) {
                reply << mcBPReply._bpms[bp]._location << ": error" << endl;
            } else {

                reply << mcBPReply._bpms[bp]._location << ": " << endl;
                for(unsigned bpm = 0 ; bpm < mcBPReply._bpms[bp]._bpms.size() ; ++bpm) {
                    reply << mcBPReply._bpms[bp]._bpms[bpm]._location << ": " << endl;

                    reply << " Input voltage:        " << mcBPReply._bpms[bp]._bpms[bpm]._inputVoltage << endl;
                    reply << " Input current:        " << mcBPReply._bpms[bp]._bpms[bpm]._inputCurrent << endl;
                    reply << " Output voltage (51V): " << mcBPReply._bpms[bp]._bpms[bpm]._outputVoltage51V << endl;
                    reply << " Output current (51V): " << mcBPReply._bpms[bp]._bpms[bpm]._outputCurrent51V << endl;
                    reply << " Output voltage (5V):  " << mcBPReply._bpms[bp]._bpms[bpm]._outputVoltage5V << endl;
                    reply << " Output current (5V):  " << mcBPReply._bpms[bp]._bpms[bpm]._outputCurrent5V << endl;
                    reply << " Temperature:          " << mcBPReply._bpms[bp]._bpms[bpm]._temp1 << endl;
                    reply << " Fan 1 RPMs:           " << mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan1 << endl;
                    reply << " Fan 2 RPMs:           " << mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan2 << endl;
                    reply << endl;

                    reply << "Status Flags: " << endl;

                    reply << " Summary:     " << mcBPReply._bpms[bp]._bpms[bpm]._statusWord << endl;
                    reply << " Vout:        " << mcBPReply._bpms[bp]._bpms[bpm]._statusVout << endl;
                    reply << " Iout:        " << mcBPReply._bpms[bp]._bpms[bpm]._statusIout << endl;
                    reply << " Input:       " << mcBPReply._bpms[bp]._bpms[bpm]._statusInput << endl;
                    reply << " Temp:        " << mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature << endl;
                    reply << " Cml:         " << mcBPReply._bpms[bp]._bpms[bpm]._statusCml << endl;
                    reply << " 5V:          " << mcBPReply._bpms[bp]._bpms[bpm]._status5V << endl;
                    reply << " Fans:        " << mcBPReply._bpms[bp]._bpms[bpm]._statusFans << endl;


                    lite::Database db;
                    char *zErrMsg = 0;
                    std::ostringstream sqlstr;
                    sqlstr <<  "insert into BULK_ENVS  " <<
                        " (cardlocation,location,voltagein,currentin,voltage51v,current51v,voltage5v,current5v,  " <<
                        "  temp, fan1rpm, fan2rpm, statusword, statusvout,statusiout,statusinput,statustemp,statuscml,status5v,statusfans) values('" <<
                        mcBPReply._bpms[bp]._location  << "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._location  << "'," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._inputVoltage <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._inputCurrent <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._outputVoltage51V <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._outputCurrent51V <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._outputVoltage5V <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._outputCurrent5V << "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._temp1 <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan1 <<  "," <<
                        mcBPReply._bpms[bp]._bpms[bpm]._rpmsFan2 <<  ",'" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusWord <<   "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusVout <<   "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusIout <<   "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusInput <<   "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusTemperature <<  "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusCml <<   "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._status5V <<   "','" <<
                        mcBPReply._bpms[bp]._bpms[bpm]._statusFans <<   "')";
                    (void)sqlite3_exec(db.getHandle(), sqlstr.str().c_str(), NULL, 0 , &zErrMsg);
                }
            }
        }
    }


    if (args[0]  == "node") {  // node card
        // read values from cards
        MCServerMessageSpec::ReadNodeCardEnvRequest mcNCRequest;
        MCServerAPIHelpers::copyTargetRequest ( mcOpenRequest, mcNCRequest );
        MCServerMessageSpec::ReadNodeCardEnvReply mcNCReply;
        server->readNodeCardEnv(mcNCRequest, mcNCReply);

        for(unsigned nc = 0 ; nc < mcNCReply._nodeCards.size() ; ++nc) {
            haveData = true;
            if (mcNCReply._nodeCards[nc]._error == true) {
                reply << mcNCReply._nodeCards[nc]._lctn << ": error" << endl;
            } else {
                reply << mcNCReply._nodeCards[nc]._lctn << " voltages: " << endl;

                reply << " 0.8V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV08Voltage   <<  endl;
                reply << " 1.4V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV14Voltage   <<  endl;
                reply << " 2.5V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV25Voltage   <<  endl;
                reply << " 3.3V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV33Voltage   <<  endl;
                reply << " 12.0V Pers Rail: " << mcNCReply._nodeCards[nc]._powerRailV120PVoltage <<  endl;
                reply << " 1.5V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV15Voltage   <<  endl;
                reply << " 0.9V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV09Voltage   <<  endl;
                reply << " 1.0V Rail: "      << mcNCReply._nodeCards[nc]._powerRailV10Voltage   <<  endl;
                reply << " 3.3V Pers Rail: " << mcNCReply._nodeCards[nc]._powerRailV33PVoltage  <<  endl;
                reply << " V12A Rail: " << mcNCReply._nodeCards[nc]._powerRailV12AVoltage  <<  endl;
                reply << " V12B Rail: " << mcNCReply._nodeCards[nc]._powerRailV12BVoltage  <<  endl;
                reply << " 1.8V Rail: " << mcNCReply._nodeCards[nc]._powerRailV18Voltage   <<  endl;
                reply << " 2.5V Pers Rail: " << mcNCReply._nodeCards[nc]._powerRailV25PVoltage  <<  endl;
                reply << " 1.2V Pers Rail: " << mcNCReply._nodeCards[nc]._powerRailV12PVoltage  <<  endl;
                reply << " 1.8V Pers Rail: " << mcNCReply._nodeCards[nc]._powerRailV18PVoltage  <<  endl;
                reply << endl;

                reply <<  " Temperature0 (ext):     " << mcNCReply._nodeCards[nc]._onboardTemp0 << endl;
                reply <<  " Temperature1 (ext):     " << mcNCReply._nodeCards[nc]._onboardTemp1 << endl;
                reply <<  " Temperature0 (local):     " << mcNCReply._nodeCards[nc]._onboardTemp0Local << endl;
                reply <<  " Temperature1 (local):     " << mcNCReply._nodeCards[nc]._onboardTemp1Local << endl;

                reply <<  " Clock Frequency: " << mcNCReply._nodeCards[nc]._clockFreq << endl;
                reply <<  " Compute alerts:  " << mcNCReply._nodeCards[nc]._alertsComputes << endl;
                reply <<  " Blink alerts:    " << mcNCReply._nodeCards[nc]._alertsBlinks << endl;
                reply <<  " Optics alerts:   " << mcNCReply._nodeCards[nc]._alertsOptics << endl;
                reply <<  " Compute pgood:   " << mcNCReply._nodeCards[nc]._pgoodComputes << endl;
                reply <<  " Blink pgood:     " << mcNCReply._nodeCards[nc]._pgoodBlinks << endl;
                reply <<  " Status of DCA:   " << mcNCReply._nodeCards[nc]._statusDca << endl;
                reply <<  " Status of VTMs (123):  " << mcNCReply._nodeCards[nc]._statusVtmDomains123 << endl;
                reply <<  " Status of VTMs (468):  " << mcNCReply._nodeCards[nc]._statusVtmDomains468 << endl;
                reply << endl;

                lite::Database db;
                char *zErrMsg = 0;

                // loop through computes
                reply << " Computes: " << endl;
                for(unsigned compute = 0 ; compute < mcNCReply._nodeCards[nc]._computes.size() ; ++compute) {
                    if (mcNCReply._nodeCards[nc]._computes[compute]._error) {
                        reply << mcNCReply._nodeCards[nc]._computes[compute]._lctn << ": error" << endl;
                    } else {
                        reply << mcNCReply._nodeCards[nc]._computes[compute]._lctn << ":" << endl;
                        reply << "  Temperature (I2C) external: " << mcNCReply._nodeCards[nc]._computes[compute]._tempI2c << endl;
                        reply << "  Temperature (I2C) local: " << mcNCReply._nodeCards[nc]._computes[compute]._tempI2cLocal << endl;
                        reply << "  Temperature (TVS): " << mcNCReply._nodeCards[nc]._computes[compute]._tempTvSense << endl;

                        std::ostringstream insstr;
                        insstr << "insert into COMPUTE_ENVS (location, tempi2c,temptvs) values('" <<
                            mcNCReply._nodeCards[nc]._computes[compute]._lctn            << "',"  <<
                            mcNCReply._nodeCards[nc]._computes[compute]._tempI2c         << ", " <<
                            mcNCReply._nodeCards[nc]._computes[compute]._tempTvSense     << " ) ";

                        (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                    }
                }
                reply << endl;


                // loop through link chips
                reply << " Link Chips: " << endl;
                for(unsigned blink = 0 ; blink < mcNCReply._nodeCards[nc]._blinks.size() ; ++blink) {
                    if (mcNCReply._nodeCards[nc]._blinks[blink]._error) {
                        reply << mcNCReply._nodeCards[nc]._blinks[blink]._lctn << ": error" << endl;
                    } else {
                        reply << mcNCReply._nodeCards[nc]._blinks[blink]._lctn << ":" << endl;
                        reply << "  Temperature (I2C) external: " << mcNCReply._nodeCards[nc]._blinks[blink]._tempI2c << endl;
                        reply << "  Temperature (I2C) local: " << mcNCReply._nodeCards[nc]._blinks[blink]._tempI2cLocal << endl;
                        reply << "  Temperature (TVS): " << mcNCReply._nodeCards[nc]._blinks[blink]._tempTvSense << endl;

                        std::ostringstream insstr;
                        insstr << "insert into LINK_ENVS (location, tempi2c,temptvs) values('" <<
                            mcNCReply._nodeCards[nc]._blinks[blink]._lctn            << "',"  <<
                            mcNCReply._nodeCards[nc]._blinks[blink]._tempI2c         << ", " <<
                            mcNCReply._nodeCards[nc]._blinks[blink]._tempTvSense     << " ) ";

                        (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                    }
                }
                reply << endl;

                // loop through optics
                reply << " Optics: " << endl;
                for(unsigned optic = 0 ; optic < mcNCReply._nodeCards[nc]._optics.size() ; ++optic) {
                    if (mcNCReply._nodeCards[nc]._optics[optic]._error) {
                        reply << mcNCReply._nodeCards[nc]._optics[optic]._lctn << ": error" << endl;
                    } else {
                        reply << mcNCReply._nodeCards[nc]._optics[optic]._lctn << ":" << endl;
                        reply << "  Status Flag:          " << (uint)mcNCReply._nodeCards[nc]._optics[optic]._status << endl;
                        reply << "  Loss of signal (los): " << mcNCReply._nodeCards[nc]._optics[optic]._los << endl;
                        reply << "  Mask for los:         " << mcNCReply._nodeCards[nc]._optics[optic]._losMask << endl;
                        reply << "  Faults:               " << mcNCReply._nodeCards[nc]._optics[optic]._faults << endl;
                        reply << "  Mask for Faults:      " << mcNCReply._nodeCards[nc]._optics[optic]._faultsMask << endl;
                        reply << "  Temp Alarms:          " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsTemp << endl;
                        reply << "  Mask for Temp Alarms: " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsTempMask << endl;
                        reply << "  Voltage Alarms:       " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsVoltage << endl;
                        reply << "  Mask for Voltage Alarms:      " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsVoltageMask << endl;
                        reply << "  Bias Current Alarms:  " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsBiasCurrent << endl;
                        reply << "  Mask for Bias Current Alarms: " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsBiasCurrentMask << endl;
                        reply << "  Power Alarms:         " << mcNCReply._nodeCards[nc]._optics[optic]._alarmsPower << endl;
                        reply << "  Mask for Power Alarms:" << mcNCReply._nodeCards[nc]._optics[optic]._alarmsPowerMask << endl;
                        reply << "  Temperature:          " << (uint)mcNCReply._nodeCards[nc]._optics[optic]._temp << endl;
                        reply << "  Voltage (3.3v):       " << mcNCReply._nodeCards[nc]._optics[optic]._voltageV33 << endl;
                        reply << "  Voltage (2.5v):       " << mcNCReply._nodeCards[nc]._optics[optic]._voltageV25 << endl;
                        reply << "  Optical Channels:     " << endl;

                        std::ostringstream insstr;
                        insstr << "insert into OPTIC_ENVS (location, status, los, losmask, faults , faultsmask, alarmstemp, alarmstempmask, alarmsvoltage, alarmsvoltagemask, alarmsbiascurrent, alarmsbiascurrentmask, alarmspower, alarmspowermask, temp , voltagev33 , voltagev25) values('" <<
                            mcNCReply._nodeCards[nc]._optics[optic]._lctn << "',"  <<
                            (uint)mcNCReply._nodeCards[nc]._optics[optic]._status << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._los << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._losMask << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._faults << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._faultsMask << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsTemp << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsTempMask << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsVoltage << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsVoltageMask << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsBiasCurrent << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsBiasCurrentMask << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsPower << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._alarmsPowerMask << "," <<
                            (uint)mcNCReply._nodeCards[nc]._optics[optic]._temp << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._voltageV33 << "," <<
                            mcNCReply._nodeCards[nc]._optics[optic]._voltageV25 << ")";

                        (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);

                        for(unsigned channel = 0 ; channel < mcNCReply._nodeCards[nc]._optics[optic]._OpticalEnvChannel.size() ; ++channel) {
                            reply << " Channel: " << channel << endl;
                            reply << "  Bias Current:         " << mcNCReply._nodeCards[nc]._optics[optic]._OpticalEnvChannel[channel]._biasCurrent << endl;
                            reply << "  Optical Power:        " << mcNCReply._nodeCards[nc]._optics[optic]._OpticalEnvChannel[channel]._opticalPower << endl;

                            std::ostringstream chstr;
                            chstr << "insert into OPT_CH_ENVS (location, channel, biascurrent, opticalpower) values('" <<
                                mcNCReply._nodeCards[nc]._optics[optic]._lctn << "',"  << channel << "," <<
                                mcNCReply._nodeCards[nc]._optics[optic]._OpticalEnvChannel[channel]._biasCurrent << "," <<
                                mcNCReply._nodeCards[nc]._optics[optic]._OpticalEnvChannel[channel]._opticalPower << ")";

                            (void)sqlite3_exec(db.getHandle(), chstr.str().c_str(), NULL, 0 , &zErrMsg);
                        }
                    }
                }
                reply << endl;

                // loop through DCAs
                reply << " DCAs: " << endl;
                for(unsigned dca = 0 ; dca < mcNCReply._nodeCards[nc]._dcas.size() ; ++dca) {
                    if (mcNCReply._nodeCards[nc]._dcas[dca]._error) {
                        reply << mcNCReply._nodeCards[nc]._dcas[dca]._lctn << ": error" << endl;
                    } else {
                        reply << mcNCReply._nodeCards[nc]._dcas[dca]._lctn << ":" << endl;
                        for(unsigned pd = 0 ; pd < mcNCReply._nodeCards[nc]._dcas[dca]._domains.size() ; ++pd) {
                            reply << "  Domain:      " << mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._domain << endl;
                            reply << "   Status:     " << (uint)mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._status << endl;
                            reply << "   Voltage:    " << mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._voltage << endl;
                            reply << "   Current:    " << mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._current << endl;

                            std::ostringstream insstr;
                            insstr << "insert into DCA_ENVS (location, domain, status, voltage, current) values('" <<
                                mcNCReply._nodeCards[nc]._dcas[dca]._lctn << "',"  <<
                                mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._domain <<  "," <<
                                (uint)mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._status <<  "," <<
                                mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._voltage <<  "," <<
                                mcNCReply._nodeCards[nc]._dcas[dca]._domains[pd]._current <<  ")";

                            (void)sqlite3_exec(db.getHandle(), insstr.str().c_str(), NULL, 0 , &zErrMsg);
                        }
                    }
                }
                reply << endl;

                std::ostringstream sqlstr;

                sqlstr << "insert into NC_ENVS  (location, temp0,temp1, "
                    << "clockfreq,alertscompute,alertsblink,alertsoptic,pgoodcompute,pgoodblink,statusdca,statusvtm123,statusvtm468) values('" <<
                    mcNCReply._nodeCards[nc]._lctn               << "'," <<
                    mcNCReply._nodeCards[nc]._onboardTemp0        << "," <<
                    mcNCReply._nodeCards[nc]._onboardTemp1        << "," <<
                    mcNCReply._nodeCards[nc]._clockFreq           << "," <<
                    mcNCReply._nodeCards[nc]._alertsComputes      << "," <<
                    mcNCReply._nodeCards[nc]._alertsBlinks        << "," <<
                    mcNCReply._nodeCards[nc]._alertsOptics        << "," <<
                    mcNCReply._nodeCards[nc]._pgoodComputes       << "," <<
                    mcNCReply._nodeCards[nc]._pgoodBlinks         << "," <<
                    mcNCReply._nodeCards[nc]._statusDca           << "," <<
                    mcNCReply._nodeCards[nc]._statusVtmDomains123 << "," <<
                    mcNCReply._nodeCards[nc]._statusVtmDomains468 << ")";
                (void)sqlite3_exec(db.getHandle(), sqlstr.str().c_str(), NULL, 0 , &zErrMsg);

                std::ostringstream pwrstr;
                pwrstr << "insert into NC_POWER_ENVS  (location,VoltageV08, VoltageV14 , VoltageV25 , VoltageV33 , VoltageV120P , VoltageV15 , VoltageV09 , VoltageV10 , VoltageV33P , VoltageV12A , VoltageV12B , VoltageV18 , VoltageV25P , VoltageV12P , VoltageV18P  )"
                       << "  values('" <<
                    mcNCReply._nodeCards[nc]._lctn                  << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV08Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV14Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV25Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV33Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV120PVoltage << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV15Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV09Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV10Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV33PVoltage  << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV12AVoltage  << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV12BVoltage  << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV18Voltage   << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV25PVoltage  << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV12PVoltage  << "'," <<
                    mcNCReply._nodeCards[nc]._powerRailV18PVoltage << ")";
                (void)sqlite3_exec(db.getHandle(), pwrstr.str().c_str(), NULL, 0 , &zErrMsg);
            }
        }
    }

    // close target set, we have our data
    MCServerMessageSpec::CloseTargetRequest mcCloseRequest = MCServerAPIHelpers::createCloseRequest( mcOpenRequest, mcOpenReply );
    MCServerMessageSpec::CloseTargetReply   mcCloseReply;
    server->closeTarget(mcCloseRequest, mcCloseReply);

    if (!haveData)  {
        reply << "no data found ";
    }
    reply << mmcs_client::DONE;
}

void
MMCSCommand_show_envs::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";displays the environmentals for a piece of hardware"
        << ";type can be service, bulk, io, node"
        << ";location is the location string, and supports regular expressions"
        << mmcs_client::DONE;
}

MMCSCommand_bringup*
MMCSCommand_bringup::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(false);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_bringup("bringup", "bringup [options]", commandAttributes);
}

void
MMCSCommand_bringup::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    // connect to mcserver
    const BlockPtr block = pController->getBlockHelper()->getBase();
    MCServerRef* temp = NULL;
    block->mcserver_connect( temp, "mmcs_lite", reply );
    const boost::scoped_ptr<MCServerRef> server( temp );
    if ( reply.getStatus() ) return;

    MCServerMessageSpec::BringupRequest buRequest;
    MCServerMessageSpec::BringupReply   buReply;
    server->bringup(buRequest, buReply);

    const int rows = boost::lexical_cast<int>(Properties::getProperty("rackRows"));
    const int cols = boost::lexical_cast<int>(Properties::getProperty("rackColumns"));

    int maxrow=0, maxcol=0, rowint, colint;

    // iterate through node cards
    for(unsigned nc = 0 ; nc < buReply._nodeCards.size() ; nc++) {
        // convert from 0-V row and column values
        std::istringstream row( buReply._nodeCards[nc]._cardLocation.substr(1,1) );
        row >> std::hex >> rowint;
        std::istringstream col( buReply._nodeCards[nc]._cardLocation.substr(2,1) );
        col >> std::hex >> colint;
        maxrow = (rowint > maxrow ? rowint : maxrow);
        maxcol = (colint > maxcol ? colint : maxcol);
    }

    // iterate through I/O cards
    for(unsigned nc = 0 ; nc < buReply._ioCards.size() ; nc++) {
        // convert from 0-V row and column values
        std::istringstream row( buReply._ioCards[nc]._cardLocation.substr(1,1) );
        row >> std::hex >> rowint;
        std::istringstream col( buReply._ioCards[nc]._cardLocation.substr(2,1) );
        col >> std::hex >> colint;
        maxrow = (rowint > maxrow ? rowint : maxrow);
        maxcol = (colint > maxcol ? colint : maxcol);
    }

    LOG_TRACE_MSG( "maxrow: " << maxrow );
    LOG_TRACE_MSG( "maxcol: " << maxcol );
    LOG_TRACE_MSG( "rows: " << rows );
    LOG_TRACE_MSG( "cols: " << cols );
    if ((++maxrow < rows) || (++maxcol < cols)) {
        reply << mmcs_client::FAIL << "hardware returned from bringup does not match computeRackRows and computeRackColumns entries in properties file "  << mmcs_client::DONE;
        return;
    }

    if ( !Properties::getProperty("ioDrawer").empty() ) {

        bool found = false;
        for(unsigned io = 0 ; io < buReply._ioCards.size() ; io++) {
            if (buReply._ioCards[io]._cardLocation.substr(0,6) ==  Properties::getProperty("ioDrawer").substr(0,6)) {
                found = true;
            }
        }

        if (!found) {
            reply << mmcs_client::FAIL << "hardware returned from bringup does not contain ioDrawer entry in properties file: " << Properties::getProperty("ioDrawer") << mmcs_client::DONE;
            return;
        }

    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
MMCSCommand_bringup::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
        << ";performs a bringup on the hardware"
        << mmcs_client::DONE;
}



MMCSCommand_wait_for_terminate*
MMCSCommand_wait_for_terminate::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);              // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(false);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_wait_for_terminate("wait_for_terminate", "wait_for_terminate [<seconds> [waitall]]", commandAttributes);
}

void
MMCSCommand_wait_for_terminate::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{
    // cast console controller to lite::ConsoleController
    lite::ConsoleController* console = dynamic_cast<lite::ConsoleController*>( pController );
    BOOST_ASSERT( console );

    // get block controller
    BlockPtr block = console->getBlockHelper()->getBase();

    // ensure block is booted
    if ( !block->isStarted() ) {
        reply << mmcs_client::FAIL << "block is not booted" << mmcs_client::DONE;
        return;
    }

    //    cout << " terminated: " << block->terminatedNodes() << endl;
    //    cout << " total: " << block->numNodesTotal() << endl;
    //    cout << " active: " << block->numNodesActive() << endl;
    //    cout << " started: " << block->numNodesStarted() << endl;

    unsigned timeout = 60;  // default timeout

    if (args.size() > 0) {
        try {
            timeout = boost::lexical_cast<unsigned>( args[0] );
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << mmcs_client::FAIL << "Invalid timeout value specified;" << _usage << mmcs_client::DONE;
            return;
        }
    }

    bool waitall = false;

    if ((args.size() > 1) && (args[1] == "waitall")) {
        waitall = true;
    }

    //sleep for <timeout> seconds, but wake up every 2 seconds to check if we can exit
    for (unsigned slept = 0 ;
         (abs(block->terminatedNodes()) != (int)block->numNodesStarted()) &&
             (waitall || (block->terminatedNodes() >= 0)) && (slept < timeout) ;
         slept+= 2)
        {
            sleep(2);
        }

    if (block->terminatedNodes() == (int)block->numNodesStarted())
        reply << mmcs_client::OK << mmcs_client::DONE;
    else
        if (block->terminatedNodes() < 0)
            reply << mmcs_client::FAIL << "Node returned FAIL message" << mmcs_client::DONE;
        else
            reply << mmcs_client::FAIL << "Timeout reached" << mmcs_client::DONE;

}

void
MMCSCommand_wait_for_terminate::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
          << ";wait for all nodes to send a terminate message"
          << ";timeout is 60 seconds, unless <seconds> is specified"
          << ";exits after first FAIL message, unless 'waitall' is specified"
          << mmcs_client::DONE;
}


MMCSCommand_d*
MMCSCommand_d::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);              // does require a BlockController object
    commandAttributes.requiresConnection(false);        // does require  mc_server connections
    commandAttributes.requiresTarget(false);            // does require a BlockControllerTarget object
    commandAttributes.mmcsLiteCommand(true);
    return new MMCSCommand_d("d", "d", commandAttributes);
}

void
MMCSCommand_d::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
        )
{

    BlockPtr pBlock = pController->getBlockHelper()->getBase();	// get selected block
    pBlock->disconnect(args, reply);
    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
MMCSCommand_d::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description()
          << ";alias for disconnect"
          << mmcs_client::DONE;
}

MMCSCommand_delete_block*
MMCSCommand_delete_block::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    Attributes::AuthPair blockdelete(hlcs::security::Object::Block, hlcs::security::Action::Delete);
    commandAttributes.addAuthPair(blockdelete);
    return new MMCSCommand_delete_block("delete_block", "delete_block", commandAttributes);
}


void
MMCSCommand_delete_block::execute(deque<string> args,
				  mmcs_client::CommandReply& reply,
				  common::ConsoleController* pController,
				  BlockControllerTarget* pTarget)
{
    // disconnect from previous block of nodes, if any
    //
    BlockPtr pBlock = pController->getBlockHelper()->getBase();	// get selected block
    pBlock->disconnect(args, reply);  // will this work?

    pBlock->delete_block(); // delete the block
    lite::ConsoleController* lite = dynamic_cast<lite::ConsoleController*>( pController );
    if ( lite ) {
        lite->setBlockInitialized(false);
    }
}

void
MMCSCommand_delete_block::help(deque<string> args,
			       mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
	  << ";Disconnect the block connections and delete the block"
	  << mmcs_client::DONE;
}

MMCSCommand_boot_block*
MMCSCommand_boot_block::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(true);       // does require  mc_server connections
    commandAttributes.requiresTarget(true);           // does require a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsLiteCommand(true);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new MMCSCommand_boot_block("boot_block", "boot_block [ options ]", commandAttributes);
}

void
MMCSCommand_boot_block::execute(std::deque<std::string> args,
				mmcs_client::CommandReply& reply,
				common::ConsoleController* pController,
				BlockControllerTarget* pTarget)
{
    BlockPtr pBlock = pController->getBlockHelper()->getBase();	// get selected block

    std::deque<std::string> boot_args, arg_uloader, arg_bootoptions, arg_domains, arg_steps;
    if (args.size() == 0);
    else {
        bool gotsteps = false;
        for (unsigned i = 0; i < args.size(); ++i)
            {
                // Need to support domain syntax instead
                // domain={cores=0-1 memory=0-3fff images=/path/to/image,/path/to/another/image id=0}:
                if (args[i].compare(0,8,"uloader=") == 0) { arg_uloader.push_back(args[i]); continue; }
                if (args[i].compare(0,6,"steps=") == 0) {
                    if(gotsteps) {
                        reply << mmcs_client::FAIL << "Only one comma separated list of steps permitted" << mmcs_client::DONE;
                        return;
                    }
                    gotsteps = true;
                    arg_steps.push_back(args[i]);
                    continue;
                }
                if (args[i].compare(0,7,"domain=")  == 0) {
                    std::string domainstr;

                    // Reconstitute the domain string.
                    // Loop through the args and append until we get to a '}'
                    unsigned int argindex = i;
                    for(std::string curstring = args[i];
                        argindex < args.size();
                        ++argindex) {
                        curstring = args[argindex];
                        if(curstring.find("cores=") != std::string::npos) {
                            curstring.replace(curstring.find('-'), 1, 1, '$');
                        }
                        if(curstring.find("memory=") != std::string::npos) {
                            curstring.replace(curstring.find('-'), 1, 1, '$');
                        }

                        curstring += " ";
                        domainstr += curstring;
                        if(curstring.find('}') != std::string::npos)
                            break;
                        ++i;
                    }

                    arg_domains.push_back(domainstr); continue;
                }
                else {
                    arg_bootoptions.push_back(args[i]);
                }
            }
    }

    boot_args.insert(boot_args.end(),arg_domains.begin(),arg_domains.end());
    boot_args.insert(boot_args.end(),arg_steps.begin(),arg_steps.end());
    boot_args.insert(boot_args.end(),arg_bootoptions.begin(),arg_bootoptions.end());
    boot_args.insert(boot_args.end(),arg_uloader.begin(),arg_uloader.end());

    pBlock->boot_block(boot_args, reply);
}

void
MMCSCommand_boot_block::help(deque<string> args,
			     mmcs_client::CommandReply& reply)
{
    // Needs to change for the domain syntax.  uloader option stays, ioload and cnload
    // go in favor of domain syntax.
    // domain={cores=0-1  memory=0-3fff  images=/path/to/image;/path/to/another/image}
    reply << mmcs_client::OK << description()
          << ";Initialize, load, and start the block's resources"
          << ";options:"
          << ";   uloader= <path>   - microloader"
          << ";   domain={cores=startcore-endcore memory=startaddr-endaddr images=imagename,imagename,..  id=domainid options=domainoptions custaddr=addr}"
          << ";   svchost_options=<svc_host_configuration_file>"
          << ";   tolerate_faults"
          << ";   boot options	       - any other options are passed to the machineController on the boot command"
          << ";   steps=                   - specify a comma separated collection of boot steps to perform"
          << ";     steps:"
          << ";       enableNodes"
          << ";       initDevbus"
          << ";       startTraining"
          << ";       enableLinkChips"
          << ";       trainLinkChips"
          << ";       trainTorus"
          << ";       installFirmware"
          << ";       installPersonalities"
          << ";       startCores"
          << ";       verifyMailbox"
          << ";       monitorMailbox"
          << ";       verifyMailboxReady"
          << ";       installKernelImages"
          << ";       configureDomains"
          << ";       launchKernels"
          << ";       verifyKernelReady"
          << ";       noop"
          << ";       cardNoop"
          << ";boot options        - any other options are passed to the machineController on the boot command"
          << ";If 'steps=' is specified, boot_block will execute the comma separated list of boot steps.  Subsequent calls to boot_block"
          << "; with additional steps defined, will execute those."
	  << mmcs_client::DONE;
}


} } // namespace mmcs::lite
