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

#include "MMCSCommand_runjob.h"

#include "ConsoleController.h"
#include "Database.h"
#include "JobInfo.h"
#include "Job.h"
#include "MMCSCommand_lite.h"

#include "server/BCNodeInfo.h"
#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include "common/ConsoleController.h"

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <utility/include/Log.h>
#include <utility/include/ScopeGuard.h>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


using namespace mmcs::server;

using namespace std;


LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


extern "C" int
selectJobCallback(
        void* arg,
        int argc,
        char** argv,
        char**
        )
{
    // get job ID
    uint32_t id = 0;
    try {
        id = boost::lexical_cast<uint32_t>(argv[0]);
    } catch ( const boost::bad_lexical_cast& e ) {
        LOG_ERROR_MSG( e.what() );
        return -1;
    }

    // cast first arg to object
    lite::JobInfo* info = reinterpret_cast<lite::JobInfo*>(arg);
    info->setId(id);

    return 0;
}

void
MMCSCommand_runjob::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    reply << mmcs_client::OK << description();
    reply << ";";
    reply << ";Runs a job on a block generated with gen_io_block or gen_block, the block must be booted";
    reply << ";with boot_block prior to invoking runjob, only a single job can be run on a block.  For";
    reply << ";either a compute or I/O block, you must have an ioLink defined in your properties file";
    reply << ";connecting this block and another I/O block controlled by a different instance of mmcs_lite.";
    reply << ";";
    reply << ";When the job terminates, a message indicating the exit status will be displayed on stdout.";
    reply << ";within mmcs_lite, not the job's stdout file.  To prematurely kill a job, killjob can be";
    reply << ";used to deliver a signal to a running job.";
    reply << ";";
    reply << ";Stdout and stderr are sent to jobid.out and jobid.err in the cwd where you invoked";
    reply << ";mmcs_lite, unless you give --inline. There is no support for stdin. Note the total";
    reply << ";size of arguments, including null tokens between each arg, must be less than " << bgcios::jobctl::MaxArgumentSize << ". Similarly,";
    reply << ";the total size of environment variables must be less than " << bgcios::jobctl::MaxVariableSize << " characters.";

    reply
        << ";;   options:"
        << ";    --cwd <dir>                working directory. Default is your the mmcs_lite cwd."
        << ";    --env <key=value>          add a single environment variable to the job."
        << ";    --ranks-per-node <ranks>   1, 2, 4, 8, 16, 32, or 64 ranks per node. Default is 1."
        << ";    --np <ranks>               number of ranks to use. Default is the entire block."
        << ";    --timeout <seconds>        kill the job if it has not died naturally."
        << ";    --transition <seconds>     maximum delay to wait between status transitions before killing the job."
        << ";    --label                    prefix output with stdout/stderr and rank"
        << ";    --inline                   display stdout/stderr inline instead of to a file"
        << ";    --ip <address>             use this IP address rather than the connected I/O node."
        << mmcs_client::DONE;
}
MMCSCommand_runjob::MMCSCommand_runjob(
        const char* name,
        const char* description,
        const Attributes& attributes
        ) :
    AbstractCommand(name, description, attributes)
{
    // nothing to do
}

MMCSCommand_runjob*
MMCSCommand_runjob::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock( true );
    commandAttributes.requiresConnection( true );
    commandAttributes.requiresTarget( false );
    commandAttributes.mmcsLiteCommand( true );
    return new MMCSCommand_runjob("runjob", "runjob [options] exe [arg1 arg2 ... argn]", commandAttributes);
}

void
MMCSCommand_runjob::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        server::BlockControllerTarget* pTarget
        )
{
    // cast console controller to LiteConsoleController
    lite::ConsoleController* console = dynamic_cast<lite::ConsoleController*>( pController );
    BOOST_ASSERT( console );

    // get block controller
    server::BlockPtr block = console->getBlockHelper()->getBase();

    // ensure block is booted
    if ( !block->isStarted() ) {
        reply << mmcs_client::FAIL << "block is not booted;" << this->description() << mmcs_client::DONE;
        return;
    }

    // ensure a previous job is not still running
    if ( !console->getJob().expired() ) {
        reply << mmcs_client::FAIL << "Job already running;" << this->description() << mmcs_client::DONE;
        return;
    }

    // create JobInfo
    lite::JobInfo jinfo;

    // parse args
    for ( deque<string>::const_iterator i = args.begin(); i != args.end(); ++i ) {
        const std::string& arg = *i;
        if ( !arg.compare("--cwd") && ++i != args.end() ) {
            jinfo.setCwd( *i );
        } else if ( !arg.compare("--env") && ++i != args.end() ) {
            jinfo.addEnv( *i );
        } else if ( !arg.compare("--ranks-per-node") && ++i != args.end() ) {
            try {
                jinfo.setRanksPerNode( boost::lexical_cast<uint32_t>(*i) );
            } catch ( const boost::bad_lexical_cast& e ) {
                reply << mmcs_client::FAIL << "bad --ranks-per-node value: " << e.what() << mmcs_client::DONE;
                return;
            }

            // validate ranks per node
            switch ( jinfo.getRanksPerNode() ) {
                case 1:
                case 2:
                case 4:
                case 8:
                case 16:
                case 32:
                case 64:
                    // these are all valid
                    break;
                default:
                    reply << mmcs_client::FAIL;
                    reply << "invalid --ranks-per-node value: " << jinfo.getRanksPerNode() << ". ";
                    reply << "Please use 1, 2, 4, 8, 16, 32, or 64" << mmcs_client::DONE;
                    return;
            }
        } else if ( !arg.compare("--np") && ++i != args.end() ) {
            jinfo.setNp( boost::lexical_cast<uint32_t>(*i) );
        } else if ( !arg.compare("--timeout") && ++i != args.end() ) {
            try {
                jinfo.setTimeout( boost::lexical_cast<unsigned>( *i ) );
            } catch ( const boost::bad_lexical_cast& e ) {
                reply << mmcs_client::FAIL << "bad --timeout value: " << e.what() << mmcs_client::DONE;
                return;
            }

            // validate timeout
            if ( jinfo.getTimeout() == 0 ) {
                reply << mmcs_client::FAIL << "--timeout values must be > 0" << mmcs_client::DONE;
                return;
            }
        } else if ( !arg.compare("--transition") && ++i != args.end() ) {
            try {
                jinfo.setTransitionTimeout( boost::lexical_cast<unsigned>(*i) );
            } catch ( const boost::bad_lexical_cast& e ) {
                reply << mmcs_client::FAIL << "bad --transition value: " << e.what() << mmcs_client::DONE;
                return;
            }

            if ( jinfo.getTransitionTimeout() == 0 ) {
                reply << mmcs_client::FAIL << "--transition values must be > 0" << mmcs_client::DONE;
                return;
            }
        } else if ( !arg.compare("--label") ) {
            jinfo.setLabel();
        } else if ( !arg.compare("--inline") ) {
            jinfo.setInline();
        } else if ( !arg.compare("--ip") && ++i != args.end() ) {
            jinfo.setIp( *i );
        } else if (jinfo.getExe().empty()) {
            // assume executable
            jinfo.setExe(arg);
        } else {
            // assume arg
            jinfo.addArg(arg);
        }
    }

    // ensure we have an executable to run
    if ( jinfo.getExe().empty() ) {
        reply << mmcs_client::FAIL << "executable missing;" << this->description() << mmcs_client::DONE;
        return;
    }

    // validate arguments can fit into the message sent to the jobctld
    uint32_t arg_size = jinfo.getExe().size() + 1;
    BOOST_FOREACH( const std::string& arg, jinfo.getArgs() ) {
        arg_size += arg.size();
        arg_size += 1; // null terminator
    }
    if ( static_cast<int32_t>(arg_size) > bgcios::jobctl::MaxArgumentSize ) {
        reply << mmcs_client::FAIL << "argument size of " << arg_size << " bytes is larger than ";
        reply << "maximum size (" << bgcios::jobctl::MaxArgumentSize << ");";
        reply << this->description() << mmcs_client::DONE;
        return;
    }

    // validate environment variables can fit into required size,
    // this is needed for the same reasons as arguments
    uint32_t env_size = 0;
    BOOST_FOREACH( const std::string& env, jinfo.getEnvs() ) {
        env_size += env.size();
        env_size += 1; // null terminator
    }
    if ( static_cast<int32_t>(env_size) > bgcios::jobctl::MaxVariableSize ) {
        reply << mmcs_client::FAIL << "environment variable size of " << env_size << " bytes is larger than ";
        reply << "maximum size (" << bgcios::jobctl::MaxVariableSize << ");";
        reply << this->description() << mmcs_client::DONE;
        return;
    }

    // check if we are a compute or I/O block
    CNBlockPtr compute_block = boost::dynamic_pointer_cast<CNBlockController>( block );
    IOBlockPtr io_block = boost::dynamic_pointer_cast<IOBlockController>( block );
    if ( compute_block ) {
        this->getConnectedIoNode( compute_block, jinfo, reply );
    } else if ( io_block ) {
        this->getConnectedIoNode( io_block, jinfo, reply );
    } else {
        reply << mmcs_client::FAIL << "block is not compute or I/O;";
        reply << this->description() << mmcs_client::DONE;
        return;
    }

    // ensure we found a connected I/O node
    if ( reply.getStatus() != 0 ) {
        return;
    }
    BOOST_ASSERT( !jinfo.getIp().empty() );

    size_t job_size = 0;
    BOOST_FOREACH( const BCNodeInfo* node, block->getNodes() ) {
        // only count nodes that are included in this target
        // and I/O nodes that aren't used just for link training
        if ( node->_open && !node->_iopos.trainOnly() ) {
            ++job_size;
        }
    }

    // log target size
    LOG_TRACE_MSG( "job size " << job_size );

    // ensure we have cwd
    if ( jinfo.getCwd().empty() ) {
        boost::filesystem::path cwd = boost::filesystem::current_path();
        jinfo.setCwd( cwd.string() );
    }

    // set block name
    jinfo.setBlock( block->getBlockName() );

    // insert into database
    this->insert(reply, jinfo);
    if ( reply.getStatus() != 0 ) {
        return;
    }

    try {
        // create Job object
        lite::Job::Ptr job(
                lite::Job::create(
                    jinfo,
                    block
                    )
                );

        // remember job
        console->setJob( job );

        // start job
        job->start();
    } catch ( const std::runtime_error& e ) {
        reply << mmcs_client::FAIL << e.what() << ";" << this->description() << mmcs_client::DONE;
    }
}

void
MMCSCommand_runjob::insert(
        mmcs_client::CommandReply& reply,
        lite::JobInfo& info
        )
{
    // open database
    boost::scoped_ptr<lite::Database> db;
    try {
        db.reset(new lite::Database);
    } catch (const std::runtime_error& e) {
        reply << mmcs_client::FAIL << e.what() << ";" << this->description() << mmcs_client::DONE;
        return;
    }

    // get current time
    boost::posix_time::ptime local_time = boost::posix_time::second_clock::local_time();
    boost::gregorian::date d = local_time.date();

    // prepare SQL statement
    std::string sql(
            "insert into JOBS "
            "(blockid, executable, cwd, ranks, args, envs, starttime) "
            "values (?,?,?,?,?,?,datetime('now'))"
            );
    sqlite3_stmt* statement = NULL;
    bgq::utility::ScopeGuard statement_guard( boost::bind(&sqlite3_finalize, boost::ref(statement)) );
    int rc = sqlite3_prepare_v2(
            db->getHandle(),
            sql.c_str(),
            sql.size(),
            &statement,
            NULL
            );
    if ( rc != SQLITE_OK || statement == NULL) {
        reply << mmcs_client::FAIL << "could not prepare SQL statement: " << sql << mmcs_client::DONE;
        return;
    }

    // bind parameters
    rc = sqlite3_bind_text( statement, 1, info.getBlock().c_str(), info.getBlock().size(), SQLITE_STATIC );
    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "could not bind column 1" << mmcs_client::DONE;
        return;
    }
    rc = sqlite3_bind_text( statement, 2, info.getExe().c_str(), info.getExe().size(), SQLITE_STATIC );
    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "could not bind column 2" << mmcs_client::DONE;
        return;
    }
    rc = sqlite3_bind_text( statement, 3, info.getCwd().c_str(), info.getCwd().size(), SQLITE_STATIC );
    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "could not bind column 3" << mmcs_client::DONE;
        return;
    }
    rc = sqlite3_bind_int( statement, 4, info.getRanksPerNode() );
    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "could not bind column 4" << mmcs_client::DONE;
        return;
    }

    // add arguments
    int offset = 0;
    char arg_buf[bgcios::jobctl::MaxArgumentSize];
    LOG_TRACE_MSG( info.getArgs().size() << " arguments" );
    BOOST_FOREACH( const std::string& arg, info.getArgs() ) {
        offset += snprintf( arg_buf + offset, sizeof(arg_buf) - offset, "%s%c", arg.c_str(), '\0' );
        LOG_TRACE_MSG("adding arg " << arg);
    }
    LOG_TRACE_MSG( "arg size: " << offset );
    rc = sqlite3_bind_text( statement, 5, arg_buf, offset, SQLITE_STATIC );
    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "could not bind column 5" << mmcs_client::DONE;
        return;
    }

    // add environment variables
    char env_buf[bgcios::jobctl::MaxVariableSize];
    LOG_TRACE_MSG( info.getEnvs().size() << " environment variables" );
    offset = 0;
    BOOST_FOREACH( const std::string& env, info.getEnvs() ) {
        offset += snprintf( env_buf + offset, sizeof(env_buf) - offset, "%s%c", env.c_str(), '\0' );
        LOG_TRACE_MSG("adding env " << env);
    }
    LOG_TRACE_MSG( "env size: " << offset );
    rc = sqlite3_bind_text( statement, 6, env_buf, offset, SQLITE_STATIC );
    if ( rc != SQLITE_OK ) {
        reply << mmcs_client::FAIL << "could not bind column 6" << mmcs_client::DONE;
        return;
    }

    // do the insert
    rc = sqlite3_step( statement );
    if ( rc != SQLITE_DONE ) {
        reply << mmcs_client::FAIL << "could not insert into sqlite database." << mmcs_client::DONE;
        return;
    }

    // get job id
    sql = "select max(jobid) from jobs";
    rc = sqlite3_exec(
            db->getHandle(),
            sql.c_str(),
            selectJobCallback,
            &info,
            NULL
            );
    if (rc != SQLITE_OK) {
        reply << mmcs_client::FAIL << "could not select job ID" << mmcs_client::DONE;
        return;
    }

    reply << mmcs_client::OK;
}

void
MMCSCommand_runjob::getConnectedIoNode(
        CNBlockPtr block,
        lite::JobInfo& jobInfo,
        mmcs_client::CommandReply& reply
        )
{
    // no need to find a connected I/O node if user requested
    // we use a specific IP address
    if ( !jobInfo.getIp().empty() ) {
        reply << mmcs_client::OK;
        return;
    }

    // get block config
    BGQBlockNodeConfig* config = block->getBlockNodeConfig();
    BOOST_ASSERT( config );

    // remember I/O node location
    std::string io_node_location;

    // iterate through targets
    BOOST_FOREACH( BCNodeInfo* node, block->getNodes() ) {
        // ensure this node is present in our target info
        if ( !node->_open ) {
            LOG_TRACE_MSG( "discarding node " << node->location() << " it is not included in target info." );
            continue;
        } else if ( node->_iopos.trainOnly() ) {
            LOG_TRACE_MSG( "discarding node " << node->location() << " it is only used for link training" );
            continue;
        } else {
            LOG_TRACE_MSG( "using " << node->location() );
        }

        // get connected I/O node
        if ( io_node_location.empty() ) {
            io_node_location = config->connectedIONode( node->location() );
            LOG_TRACE_MSG( "I/O node location " << io_node_location );
        } else if ( io_node_location != config->connectedIONode( node->location() ) ) {
            LOG_ERROR_MSG( node->location() << " connects to " << config->connectedIONode( node->location() ) );
            LOG_ERROR_MSG( "which differs from " << io_node_location );
            reply << mmcs_client::FAIL << "blocks with multiple I/O connections are not supported" << mmcs_client::DONE;
            return;
        }
    }

    // ensure we found a connected I/O node
    if ( io_node_location.empty() ) {
        reply << mmcs_client::FAIL << "block " << block->getBlockName() << " has no connected I/O node" << mmcs_client::DONE;
        return;
    }

    // iterate through machine XML to find I/O node IP address
    BOOST_FOREACH( const BGQMachineIOBoard* io_board, block->getMachineXML()->_ioBoards ) {
        LOG_TRACE_MSG( io_board->_board );
        // iterate through I/O nodes
        BOOST_FOREACH( const BGQMachineNode* io_node, io_board->_nodes ) {
            // combine I/O board location and io_node location to create
            // a location string
            std::string location( io_board->_board );
            location.append( "-" );
            location.append( io_node->_location );
            LOG_TRACE_MSG( location );

            // if this location is the connected I/O node, remember its IP address
            if ( location == io_node_location ) {
                if ( io_node->_netConfig.empty() ) {
                    reply << mmcs_client::FAIL << "missing network configuration for I/O node " << location << mmcs_client::DONE;
                    return;
                }

                // assume ipv4 address on first interface in network config for now
                jobInfo.setIp( io_node->_netConfig[0]->_ipv4Address );
                LOG_TRACE_MSG( "using IP address " << jobInfo.getIp() );
                break;
            }
        }
    }

    if ( jobInfo.getIp().empty() ) {
        reply << mmcs_client::FAIL << "could not find IP address for connected I/O node " << io_node_location << mmcs_client::DONE;
    } else {
        reply << mmcs_client::OK;
    }
}

void
MMCSCommand_runjob::getConnectedIoNode(
        IOBlockPtr block,
        lite::JobInfo& jobInfo,
        mmcs_client::CommandReply& reply
        )
{
    // no need to find a connected I/O node if user requested
    // we use a specific IP address
    if ( !jobInfo.getIp().empty() ) {
        reply << mmcs_client::OK;
        return;
    }

    // get block config
    BGQBlockNodeConfig* config = block->getBlockNodeConfig();
    BOOST_ASSERT( config );

    // remember I/O node location
    std::string io_node_location;

    // iterate through nodes
    BOOST_FOREACH( BCNodeInfo* node, block->getNodes() ) {
        // ensure this node is present in our target info
        if ( !node->_open ) {
            LOG_TRACE_MSG( "discarding node " << node->location() << " it is not included in target info." );
            continue;
        } else if ( node->_iopos.trainOnly() ) {
            LOG_TRACE_MSG( "discarding node " << node->location() << " it is only used for link training" );
            continue;
        } else {
            LOG_TRACE_MSG( "using " << node->location() );
        }

        // get connected I/O node
        if ( io_node_location.empty() ) {
            io_node_location = config->connectedIONode( node->location() );
            LOG_TRACE_MSG( "I/O node location " << io_node_location );
        } else if ( io_node_location != config->connectedIONode( node->location() ) ) {
            LOG_ERROR_MSG( node->location() << " connects to " << config->connectedIONode( node->location() ) );
            LOG_ERROR_MSG( "which differs from " << io_node_location );
            reply << mmcs_client::FAIL << "blocks with multiple I/O connections are not supported" << mmcs_client::DONE;
            return;
        }
    }

    // ensure we found a connected I/O node
    if ( io_node_location.empty() ) {
        reply << mmcs_client::FAIL << "block " << block->getBlockName() << " has no connected I/O node" << mmcs_client::DONE;
        return;
    }

    // iterate through machine XML to find I/O node IP address
    BOOST_FOREACH( const BGQMachineIOBoard* io_board, block->getMachineXML()->_ioBoards ) {
        LOG_TRACE_MSG( io_board->_board );
        // iterate through I/O nodes
        BOOST_FOREACH( const BGQMachineNode* io_node, io_board->_nodes ) {
            // combine I/O board location and io_node location to create
            // a location string
            std::string location( io_board->_board );
            location.append( "-" );
            location.append( io_node->_location );
            LOG_TRACE_MSG( location );

            // if this location is the connected I/O node, remember its IP address
            if ( location == io_node_location ) {
                if ( io_node->_netConfig.empty() ) {
                    reply << mmcs_client::FAIL << "missing network configuration for I/O node " << location << mmcs_client::DONE;
                    return;
                }

                // assume ipv4 address on first interface in network config for now
                jobInfo.setIp( io_node->_netConfig[0]->_ipv4Address );
                LOG_TRACE_MSG( "using IP address " << jobInfo.getIp() );
                break;
            }
        }
    }

    if ( jobInfo.getIp().empty() ) {
        reply << mmcs_client::FAIL << "could not find IP address for connected I/O node " << io_node_location << mmcs_client::DONE;
    } else {
        reply << mmcs_client::OK;
    }
}


} } // namespace mmcs::lite
