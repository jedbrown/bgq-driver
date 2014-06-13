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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include "api/BGQDBlib.h"
#include "api/genblock.h"
#include "api/ioUsage.h"

#include "api/job/Operations.h"
#include "api/job/SubBlockRas.h"

#include <bgq_util/include/Location.h>
#include <bgq_util/include/TempFile.h>

#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <boost/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using boost::lexical_cast;

using std::cout;
using std::runtime_error;
using std::string;

static boost::shared_ptr<bgq::utility::UserId> uid_ptr;
static bgq::utility::Properties::Ptr s_properties_ptr;

static void initializeBGQDB()
{
    static const unsigned connection_pool_size(1);
    BGQDB::init( s_properties_ptr, connection_pool_size );
}

class AbstractCommandHandler
{
public:
    typedef std::vector<std::string> Args;

    virtual std::string getName() const =0;
    virtual std::string getHelpText() const =0;

    virtual void execute( const Args& args ) =0;

    virtual ~AbstractCommandHandler()  { /* Nothing to do */ }
};

/*

class CommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return ""; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {
    }
};

*/

class GetMachineXMLCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getMachineXML"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {

        std::stringstream xmlStream;
        std::vector<string> invalid_memory_locations;
        TempFile tmpFile("__DBTester_tmp_machine.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getMachineXML(xmlStream, &invalid_memory_locations));
        if ( db_status != BGQDB::OK ) {
            cout << "getMachineXML failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getMachineXML completed successfully\n";
    }
};

class GetBPsCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getBPs"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {

        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_getBPs.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getBPs(xmlStream));
        if ( db_status != BGQDB::OK ) {
            cout << "getBPs failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getBPs completed successfully\n";
    }
};

class GetMidplanesCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getMidplanes"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {

        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_midplane.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getBPs(xmlStream));
        if ( db_status != BGQDB::OK ) {
            cout << "getBPs failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getMidplanes completed successfully\n";
    }
};

class GetMidplaneNodeBoardsCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getMidplaneNodeBoards"; }
    std::string getHelpText() const  { return "midplane"; }

    void execute( const Args& args ) {
        string midplane(args.at( 0 ));

        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_nodeboards.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getBPNodeCards(xmlStream, midplane.c_str()));
        if ( db_status != BGQDB::OK ) {
            cout << "getBPNodeCards failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getMidplaneNodeBoards completed successfully\n";
    }
};

class GetMidplaneIOCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getMidplaneIO"; }
    std::string getHelpText() const  { return "midplane"; }

    void execute( const Args& args ) {
        string midplane(args.at( 0 ));

        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_IO.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getIOWireList(xmlStream, midplane.c_str()));
        if ( db_status != BGQDB::OK ) {
            cout << "getIOWireList failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getMidplaneIO completed successfully\n";
    }
};

class CheckIOLinksCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "checkIOLinks"; }
    std::string getHelpText() const  { return "block"; }

    void execute( const Args& args ) {
        string block(args.at( 0 ));

        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_IOlinks.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::checkIOLinks(xmlStream, block.c_str()));
        if ( db_status != BGQDB::OK ) {
            cout << "checkIOLinks failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "checkIOLinks completed successfully\n";
    }
};

class GetSwitchesCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getSwitches"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {
        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_switches.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getSwitches(xmlStream));
        if ( db_status != BGQDB::OK ) {
            cout << "getSwitches failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getSwitches completed successfully\n";
    }
};

class GetMidplaneCablesCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getMidplaneCables"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {
        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_midplane_cables.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getBPWireList(xmlStream));
        if ( db_status != BGQDB::OK ) {
            cout << "getBPWireList failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getMidplaneCables completed successfully\n";
    }
};

class GetMachineBPSizeCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getMachineBPSize"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {
        initializeBGQDB();

        unsigned int a_size, b_size, c_size, d_size;

        BGQDB::STATUS db_status(BGQDB::getMachineBPSize( a_size, b_size, c_size, d_size ));

        if ( db_status != BGQDB::OK ) {
            cout << "getMachineBPSize failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "getMachineBPSize: a=" << a_size << " b=" << b_size << " c=" << c_size << " d=" << d_size << "\n";
    }
};


class GenBlockCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "genBlock"; }
    std::string getHelpText() const  { return "blockid midplane"; }

    void execute( const Args& args ) {
        string blockid(args.at( 0 ));
        string midplane(args.at( 1 ));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::genBlock( blockid.c_str(), midplane.c_str(), uid_ptr->getUser()) );

        if ( db_status != BGQDB::OK ) {
            cout << "genBlock failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "genBlock successful.\n";
    }
};


class GenBlocksCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "genBlocks"; }
    std::string getHelpText() const  { return "prefix"; }

    void execute( const Args& args ) {
        string prefix(args.at(0));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::genBlocks( prefix.c_str() ));

        if ( db_status != BGQDB::OK ) {
            cout << "genBlocks failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "genBlocks successful.\n";
    }
};


class GenFullBlockCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "genFullBlock"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        string blockid(args.at(0));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::genFullBlock( blockid.c_str(), uid_ptr->getUser() ));

        if ( db_status != BGQDB::OK ) {
            cout << "genFullBlock failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "genFullBlock successful.\n";
    }
};

class GenSmallBlockCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "genSmallBlock"; }
    std::string getHelpText() const  { return "blockid midplane cnodes nodecard"; }

    void execute( const Args& args ) {
        string blockid(args.at(0));
        string midplane(args.at(1));
        int cnodes(lexical_cast<int>( args.at(2) ));
        string nodecard(args.at(3));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::genSmallBlock( blockid.c_str(), midplane.c_str(), cnodes, nodecard.c_str(), uid_ptr->getUser() ));

        if ( db_status != BGQDB::OK ) {
            cout << "genSmallBlock failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "genSmallBlock successful.\n";
    }
};

class GetBlockXmlCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getBlockXML"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        string blockid(args.at(0));
        std::stringstream xmlStream;
        TempFile tmpFile("__DBTester_tmp_block.XXXXXX");
        tmpFile.keep = true;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getBlockXML( xmlStream, blockid ));

        if ( db_status != BGQDB::OK ) {
            cout << "getBlockXML failed with db_status=" << db_status << "\n";
            if ( db_status == BGQDB::XML_ERROR ) {
                cout << " -- check directory permissions\n";
            } else {
                //dump the XML stream to /tmp
                (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
                cout  << " -- XML file is " << tmpFile.fname << "\n";
            }
            throw runtime_error( "failed" );
        }
        //dump the XML stream to /tmp
        (void)write(tmpFile.fd, xmlStream.str().c_str(), xmlStream.str().size());
        cout  << " -- XML file is " << tmpFile.fname << "\n";
        cout << "getBlockXML completed successfully\n";
    }
};

class GetBlockStateCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getBlockStatus"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        const string blockid(args.at(0));

        initializeBGQDB();

        BGQDB::BLOCK_STATUS block_state;

        BGQDB::STATUS db_status(BGQDB::getBlockStatus( blockid, block_state ));

        if ( db_status != BGQDB::OK ) {
            cout << "getBlockStatus failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "Block state=" << block_state << "\n";
    }
};


std::istream& operator>>( std::istream& is, BGQDB::BLOCK_STATUS& block_state )
{
    string s;
    is >> s;
    block_state = BGQDB::blockCodeToStatus( s.c_str() );
    return is;
}


class SetBlockStateCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "setBlockStatus"; }
    std::string getHelpText() const  { return "blockid state [option... ('action')]"; }

    void execute( const Args& args ) {
        string blockid(args.at(0));
        string new_state_str(args.at(1));
        std::deque<std::string> options;
        for ( unsigned i(2) ; i < args.size() ; ++i ) {
            options.push_back( args[i] );
        }

        BGQDB::BLOCK_STATUS new_state(lexical_cast<BGQDB::BLOCK_STATUS>(new_state_str));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::setBlockStatus( blockid.c_str(), new_state, options ));

        if ( db_status != BGQDB::OK ) {
            cout << "setBlockStatus failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "setBlockStatus successful.\n";
    }
};

class GetBlockActionCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "getBlockAction"; }
    std::string getHelpText() const  { return ""; }

    void execute( const Args& args ) {
        string blockid;
        BGQDB::BLOCK_ACTION action;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::getBlockAction( blockid, action ));

        if ( db_status != BGQDB::OK ) {
            cout << "getBlockAction failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << blockid << "\n";
        cout << action << "\n";

        cout << "getBlockAction successful.\n";
    }
};

class SetBlockActionCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "setBlockAction"; }
    std::string getHelpText() const  { return "blockid [action]"; }


    void execute( const Args& args ) {
        BGQDB::BLOCK_ACTION action = BGQDB::NO_BLOCK_ACTION;
        string blockid(args.at(0));
        string action_str(args.size() > 1 ? args.at(1) : "");
        if (action_str == "B") {
            action = BGQDB::CONFIGURE_BLOCK;
        } else  if (action_str == "N") {
            action = BGQDB::CONFIGURE_BLOCK_NO_CHECK;
        } else if (action_str == "D") {
            action = BGQDB::DEALLOCATE_BLOCK;
        }

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::setBlockAction( blockid, action ));

        if ( db_status != BGQDB::OK ) {
            cout << "setBlockAction failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "setBlockAction successful.\n";
    }
};

class ClearBlockActionCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "clearBlockAction"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        string blockid(args.at(0));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::clearBlockAction( blockid ));

        if ( db_status != BGQDB::OK ) {
            cout << "clearBlockAction failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        cout << "clearBlockAction successful.\n";
    }
};

class KillMidplaneJobsCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "killMidplaneJobs"; }
    std::string getHelpText() const  { return "location [kill|list]"; }

    void execute( const Args& args ) {
        string location(args.at(0));
        std::vector<uint64_t> jobs_to_kill;

        bool listOnly = true;
        if (args.size() > 1 && args.at(1) == "kill")
            listOnly = false;

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::killMidplaneJobs( location, &jobs_to_kill, listOnly ));

        if ( db_status != BGQDB::OK ) {
            cout << "killMidplaneJobs failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }

        for(uint64_t jobs = 0; jobs < jobs_to_kill.size(); jobs++ ) {
            cout << "Job: " << jobs_to_kill[jobs] << "\n";
        }

        cout << "killMidplaneJobs successful.\n";
    }
};

class QueryMissingCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "queryMissing"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        string blockid(args.at(0));
        std::vector<string> missing;

        BGQDB::STATUS db_status(BGQDB::queryMissing(blockid, missing, BGQDB::NO_DIAGS ));

        if ( db_status != BGQDB::OK ) {
            cout << "queryMissing failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        cout << "missing hw : \n";

        for(unsigned int cn = 0 ; cn < missing.size(); cn++ ) {
            cout << missing[cn] << "\n";
        }

        cout << "queryMissing successful.\n";

    }
};

class CheckBlockConnectionCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "checkBlockConnection"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        string blockid(args.at(0));
        std::vector<string> unconnected;

        BGQDB::STATUS db_status(BGQDB::checkBlockConnection( blockid, &unconnected ));

        if ( db_status != BGQDB::OK ) {
            cout << "checkBlockConnection failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        cout << "unconnected I/O nodes: \n";

        for(unsigned int ion = 0 ; ion < unconnected.size(); ion++ ) {
            cout << unconnected[ion] << "\n";
        }

        cout << "checkBlockConnection successful.\n";
    }
};

class CheckBlockIOCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "checkBlockIO"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        string blockid(args.at(0));
        std::vector<string> unconnectedIONodes;
        std::vector<string> midplanesFailingIORules;
        std::vector<string> unconnectedAvailableIONodes;

        BGQDB::STATUS db_status(BGQDB::checkBlockIO( blockid, &unconnectedIONodes, &midplanesFailingIORules, &unconnectedAvailableIONodes ));

        if ( db_status != BGQDB::OK ) {
            cout << "checkBlockIO failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        cout << "unconnected I/O nodes: \n";

        for(unsigned int ion = 0 ; ion < unconnectedIONodes.size(); ion++ ) {
            cout << unconnectedIONodes[ion] << "\n";
        }

        cout << "midplanes failing I/O rules: \n";

        for(unsigned int mp = 0 ; mp < midplanesFailingIORules.size(); mp++ ) {
            cout << midplanesFailingIORules[mp] << "\n";
        }

        cout << "'Available' (connected) I/O nodes not booted: \n";

        for(unsigned int ion = 0 ; ion < unconnectedAvailableIONodes.size(); ion++ ) {
            cout << unconnectedAvailableIONodes[ion] << "\n";
        }

        cout << "checkBlockIO successful.\n";
    }
};


class CheckIOBlockConnectionCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "checkIOBlockConnection"; }
    std::string getHelpText() const  { return "blockid"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        string blockid(args.at(0));
        std::vector<string> connected;

        BGQDB::STATUS db_status(BGQDB::checkIOBlockConnection( blockid, &connected ));

        if ( db_status != BGQDB::OK ) {
            cout << "checkIOBlockConnection failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        cout << "connected CN blocks: \n";

        for(unsigned int cn = 0 ; cn < connected.size(); cn++ ) {
            cout << connected[cn] << "\n";
        }

        cout << "checkIOBlockConnection successful.\n";

    }
};

class CheckIONodeConnectionCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "checkIONodeConnection"; }
    std::string getHelpText() const  { return "ionode(s)"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        std::vector<string> nodes;
        std::vector<string> connected;

        for ( unsigned i = 0 ; i < args.size() ; ++i ) {
            nodes.push_back( args.at(i));
        }

        BGQDB::STATUS db_status(BGQDB::checkIONodeConnection(nodes, connected ));

        if ( db_status != BGQDB::OK ) {
            cout << "checkIONodeConnection failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        cout << "connected CN blocks: \n";

        for(unsigned int cn = 0 ; cn < connected.size(); cn++ ) {
            cout << connected[cn] << "\n";
        }

        cout << "checkIONodeConnection successful.\n";

    }
};


class InsertJobCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "insertJob"; }
    std::string getHelpText() const  { return "blockid [--shape AxBxCxDxE|MP|NB|node] [--status CODE]"; }

    void execute( const Args& args ) {
        namespace po = boost::program_options;

        po::options_description visible_desc( "Options" );

        string block_id;
        BGQDB::job::Shape shape;
        string status_code;

        visible_desc.add_options()
                ( "shape", po::value( &shape ), "shape" )
                ( "status", po::value( &status_code ), "status code" )
            ;

        po::options_description all_opts( "all" );

        all_opts.add_options()
                ( "block", po::value( &block_id ), "block id" )
            ;

        all_opts.add( visible_desc );

        po::positional_options_description p;
        p.add( "block", 1 );

        po::variables_map vm;

        po::store( po::command_line_parser( args ).options( all_opts ).positional( p ).run(), vm );
        po::notify( vm );

        if ( block_id.empty() ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "block must be specified" ) );
        }

        initializeBGQDB();

        BGQDB::job::InsertInfo insert_info;
        insert_info.setBlock( block_id );
        insert_info.setUserName( uid_ptr->getUser() );
        insert_info.setProcessesPerNode( 1 );
        insert_info.setShape( shape );
        insert_info.setHostname( "localhost" );
        insert_info.setPid( getpid() );
        insert_info.setExe( "sleeper" );

        BGQDB::job::Id job_id;

        BGQDB::job::Operations job_ops;
        job_ops.insert(
                insert_info,
                &job_id
            );

        if ( ! status_code.empty() ) {
            job_ops.update(
                    job_id,
                    BGQDB::job::codeToValue( status_code )
                );
        }

        cout << "Job ID: " << job_id << "\n";
    }
};


class UpdateJobCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "updateJob"; }
    std::string getHelpText() const  { return "jobid status"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        BGQDB::job::Id job_id(lexical_cast<BGQDB::job::Id>(args.at(0)));
        const std::string &status_code(args.at(1));

        BGQDB::job::Operations().update(
                job_id,
                BGQDB::job::codeToValue( status_code )
            );

        cout << "Updated job " << job_id << " to " << status_code << "\n";
    }
};


class RemoveJobCommandHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "removeJob"; }
    std::string getHelpText() const  { return "jobid [EXIT_STATUS | ERROR_MESSAGE]"; }

    void execute( const Args& args ) {
        namespace po = boost::program_options;

        BGQDB::job::Id job_id;
        string exit_status_or_error_msg;

        po::options_description all_opts( "all" );

        all_opts.add_options()
                ( "id", po::value( &job_id ), "job id" )
                ( "esem", po::value( &exit_status_or_error_msg ), "exit status or error message" )
            ;

        po::positional_options_description p;
        p.add( "id", 1 );
        p.add( "esem", 1 );

        po::variables_map vm;

        po::store( po::command_line_parser( args ).options( all_opts ).positional( p ).run(), vm );
        po::notify( vm );

        if ( vm.count( "id" ) == 0 ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "job ID must be specified" ) );
        }

        initializeBGQDB();

        BGQDB::job::RemoveInfo remove_info;

        if ( ! exit_status_or_error_msg.empty() ) {
            bool exit_status_set(false);

            try {
                int exit_status(boost::lexical_cast<int>( exit_status_or_error_msg ));
                remove_info = BGQDB::job::RemoveInfo( exit_status );
                exit_status_set = true;
            } catch ( std::exception& e ) {
            }

            if ( ! exit_status_set ) {
                remove_info = BGQDB::job::RemoveInfo( exit_status_or_error_msg );
            }
        }

        BGQDB::job::Operations().remove(
                job_id,
                remove_info
            );

        cout << "Removed job " << job_id << "\n";
    }

};

class SubBlockRasHandler : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "subBlockRas"; }
    std::string getHelpText() const  { return "block location"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        const std::string& block( args.at(0) );
        const bgq::util::Location location(args.at(1));

        const std::vector<BGQDB::job::Id> result(
                BGQDB::job::subBlockRas( block, location )
                );

        cout << result.size() << " jobs"  << "\n";
        cout << "job " << result[0] << "\n";
    }
};


class ioUsage : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "ioUsage"; }
    std::string getHelpText() const  { return "block cn:count..."; }

    void execute( const Args& args ) {
        initializeBGQDB();
        BGQDB::ioUsage::init();

        const std::string& block( args.at(0) );

        BGQMidplaneNodeConfig::ComputeCount cn_counts;

        for ( unsigned i(1) ; i < args.size() ; ++i ) {
            const string &cn_count(args[i]);

            string::size_type split_pos(cn_count.find(':'));
            string cn(cn_count.substr(0,split_pos));
            unsigned count(lexical_cast<unsigned>(cn_count.substr(split_pos+1)));

            cn_counts[cn] = count;
        }

        try {
            BGQDB::ioUsage::update( cn_counts, block );
        } catch ( BGQDB::ioUsage::UsageExceeded& ue ) {
            cout << "Caught BGQDB::ioUsage::UsageExceeded, ioNode=" << ue.getIoNode() << " count=" << ue.getCount() << "\n";
            throw;
        }

        cout << "Updated usage for " << block << ".\n";
    }
};

class QueryError : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "QueryError"; }
    std::string getHelpText() const  { return "block"; }

    void execute( const Args& args ) {
        initializeBGQDB();
        BGQDB::ioUsage::init();

        const std::string& block( args.at(0) );
        std::vector<string> error;

        BGQDB::STATUS db_status(BGQDB::queryError(block, error ));

        if ( db_status != BGQDB::OK ) {
            cout << "queryError failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        cout << "error hw : \n";

        for(unsigned int cn = 0 ; cn < error.size(); cn++ ) {
            cout << error[cn] << "\n";
        }

        cout << "queryError successful.\n";
    }
};


class GenIoBlock : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "genIOBlock"; }
    std::string getHelpText() const  { return "id location ionodesCount"; }

    void execute( const Args& args ) {
    	if ( args.size() != 3 ) {
    		throw runtime_error( string() + "arguments: " + getHelpText() );
    	}

    	const std::string &id(args[0]);
    	const std::string &location(args[1]);
    	unsigned ioNodesCount(lexical_cast<unsigned>(args[2]));

        initializeBGQDB();

        BGQDB::STATUS db_status(BGQDB::genIOBlock( id, location, ioNodesCount, "" ));

        if ( db_status != BGQDB::OK ) {
            cout << "genIOBlock failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
    }
};


class PutRas : public AbstractCommandHandler
{
public:
    std::string getName() const  { return "putRAS"; }
    std::string getHelpText() const  { return "msgid location"; }

    void execute( const Args& args ) {
    	if ( args.size() != 2 ) {
    		throw runtime_error( string() + "arguments: " + getHelpText() );
    	}

    	const std::string &msgid_string(args[0]);
    	const std::string &location(args[1]);

        initializeBGQDB();

        std::istringstream is( msgid_string );
        unsigned msgid;
        is >> std::hex >> msgid;

        RasEventImpl event(msgid);
        event.setDetail( RasEvent::LOCATION, location );
        RasEventHandlerChain::handle(event);

        const std::string block;
        const BGQDB::job::Id job( 0 );
        const uint32_t qualifier( 0 );
        std::vector<BGQDB::job::Id> jobs_to_kill;
        uint32_t recid;

        BGQDB::STATUS db_status(
                BGQDB::putRAS(
                    event,
                    block,
                    job,
                    qualifier,
                    &jobs_to_kill,
                    &recid
                    )
                );

        if ( db_status != BGQDB::OK ) {
            cout << "putRAS failed with db_status=" << db_status << "\n";
            throw runtime_error( "failed" );
        }
        std::cout << "inserted RAS event with recid " << recid << std::endl;
        BOOST_FOREACH( const BGQDB::job::Id i, jobs_to_kill ) {
            std::cout << "job " << i << " will be killed" << std::endl;
        }
    }
};


typedef std::vector<AbstractCommandHandler*> CommandHandlers;

static CommandHandlers initializeCommandHandlers()
{
    return boost::assign::list_of
            ( (AbstractCommandHandler*)( new GetMachineBPSizeCommandHandler ) )
            ( new GetMachineXMLCommandHandler )
            ( new GetBPsCommandHandler )
            ( new GetMidplanesCommandHandler )
            ( new GetMidplaneNodeBoardsCommandHandler )
            ( new GetMidplaneCablesCommandHandler )
            ( new GetMidplaneIOCommandHandler )
            ( new CheckIOLinksCommandHandler )
            ( new GetSwitchesCommandHandler )
            ( new GenBlockCommandHandler )
            ( new GenBlocksCommandHandler )
            ( new GenFullBlockCommandHandler )
            ( new GenSmallBlockCommandHandler )
            ( new GetBlockXmlCommandHandler )
            ( new GetBlockStateCommandHandler )
            ( new SetBlockStateCommandHandler )
            ( new GetBlockActionCommandHandler )
            ( new SetBlockActionCommandHandler )
            ( new ClearBlockActionCommandHandler )
            ( new KillMidplaneJobsCommandHandler )
            ( new QueryMissingCommandHandler )
            ( new CheckBlockConnectionCommandHandler )
            ( new CheckBlockIOCommandHandler )
            ( new CheckIOBlockConnectionCommandHandler )
            ( new CheckIONodeConnectionCommandHandler )
            ( new InsertJobCommandHandler )
            ( new UpdateJobCommandHandler )
            ( new RemoveJobCommandHandler )
            ( new SubBlockRasHandler )
            ( new ioUsage )
            ( new QueryError )
            ( new GenIoBlock )
            ( new PutRas )
        ;
}

static CommandHandlers commandHandlers(initializeCommandHandlers());


static void printHelp()
{
    cout << "Commands:\n";
    for ( CommandHandlers::const_iterator i(commandHandlers.begin()) ; i != commandHandlers.end() ; ++i ) {
        cout << "\t" << (*i)->getName() << " " << (*i)->getHelpText() << "\n";
    }

    cout << "\n";
}


int main( int argc, char *argv[] )
{
    if ( argc == 1 ) {
        // No command.
        printHelp();
        return 1;
    }

    s_properties_ptr = bgq::utility::Properties::create();
    bgq::utility::initializeLogging( *s_properties_ptr );

    uid_ptr.reset( new bgq::utility::UserId() );

    string command_name(argv[1]);

    if ( command_name == "help" ) {
        printHelp();
        return 0;
    }

    AbstractCommandHandler::Args args;

    for ( int i = 2 ; i < argc ; ++i ) {
        args.push_back( argv[i] );
    }

    for ( CommandHandlers::iterator i(commandHandlers.begin()) ; i != commandHandlers.end() ; ++i ) {
        if ( (*i)->getName() == command_name ) {
            (*i)->execute( args );
            return 0;
        }
    }

    // Didn't recognize command.
    cout << "Didn't recognize '" << command_name << "'\n";
    printHelp();

    return 1;
}
