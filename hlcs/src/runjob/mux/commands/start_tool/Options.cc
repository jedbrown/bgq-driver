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
/*!
 * \page start_tool
 * 
 * Start a tool daemon for a BG/Q job.
 *
 * \section SYNOPSIS
 *
 * start_tool id [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Start a tool daemon on a all, or a subset of the I/O nodes serviced by a job. This command is synchronous,
 * it returns when the tool has been successfully started on each I/O node. If the tool fails to start on any
 * node, the tool daemons that did start will be delivered a SIGTERM. A descriptive error will also be returned. 
 * Once the tool has started, a unique ID will be generated. The tool may later be ended by end_tool. Note 
 * that a maximum of four tools may be in use concurrently per job.
 *
 * \note Tools cannot be started for sub-node jobs.
 *
 * \section OPTIONS
 *
 * \subsection pid --pid.
 *
 * runjob process ID. Either this parameter
 * or --id must be given.
 *
 * \note start_tool must be invoked from the same host that runjob was when using this parameter.
 *
 * \subsection id --id.
 *
 * Job ID. If given this value overrides the pid. This parameter may be specified as the first 
 * positional argument. 
 *
 * \subsection tool --tool
 *
 * Fully qualified path to the tool daemon.
 *
 * \subsection args --args
 * 
 * Arguments to the tool daemon, split on spaces.
 *
 * \subsection subset --subset
 *
 * A subset specification is a single token of 4096 or less characters that consists of a space separated list. 
 * Each element in the list can have one of three formats:
 *
 * - rank1 : single rank
 * - rank1-rank2 : all ranks between rank1 and rank2
 * - rank1-rank2:stride : every strideth rank between rank1 and rank2
 *
 * A rank specification can either be a number or the special keywords $max or max. Both of them represent
 * the last rank participating in the job. The $ character is optional to prevent shell escaping. This subset
 * specification will restrict the I/O nodes used for launching the tool. Note each element in the 
 * specification must be in increasing order, and cannot have any overlapping ranks. The subset must specify 
 * at least one rank that is participating in the job. The --tool option must be given with this 
 * option. If not specified, the default value is 0-$max.
 *
 * \subsection interactive --interactive
 *
 * Display an interactive prompt for querying the proctable after the tool is started.
 *
 * COMMON_ARGUMENTS_GO_HERE
 *
 * \section debugger DEBUGGERS
 *
 * start_tool exposes the same MPIR_proctable symbol that runjob does. A tool can ptrace start_tool to
 * configure the tool path and arguments rather than passing them in as arguments to start_tool. Doing
 * this also allows the tool to programatically query each rank in the proctable. The proctable is defined
 * as
 *
\verbatim
typedef struct {
    const char* host_name;
    const char* executable_name;
    int pid;
} MPIR_PROCDESC;
\endverbatim
 *
 * To use this interface, a tool should
 *
 *  - set a breakpoint at MPIR_Breakpoint()
 *  - set MPIR_being_debugged = 1
 *  - set MPIR_executable_path to the path for the tool
 *  - optionally set MPIR_server_arguments to the arguments for the tool
 *
 * The breakpoint will be invoked when the tool has started and the proctable is filled in.
 *
 * \section exit EXIT STATUS
 *
 * \section AUTHOR
 *
 * IBM
 *
 * \section copyright COPYRIGHT
 *
 * © Copyright IBM Corp. 2010, 2011
 *
 * \section also SEE ALSO
 *
 * - \link runjob runjob \endlink
 * - \link runjob_server runjob_server \endlink
 * - \link end_tool \endlink
 * - \link tool_status \endlink
 * - \link dump_proctable \endlink
 */

#include "mux/commands/start_tool/Options.h"

#include "common/message/convert.h"
#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <iostream>

LOG_DECLARE_FILE( runjob::mux::commands::log );

namespace runjob {
namespace mux {
namespace commands {
namespace start_tool {

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    AbstractOptions(argc, argv, runjob::mux::log ),
    _pid( 0 ),
    _socket(),
    _options( "Options" ),
    _tool(),
    _interactive( false )
{
    namespace po = boost::program_options;
    _options.add_options()
        ("pid", po::value(&_pid), "runjob process ID")
        ("id", po::value(&_job), "job ID")
        (
         "tool",
         po::value<std::string>()
         ->default_value( std::string() )
         ->notifier( boost::bind(&tool::Daemon::setExecutable, boost::ref(_tool), _1) ),
         "path to tool daemon"
        )
        (
         "args",
         po::value<std::string>()
         ->default_value( std::string() )
         ->notifier( boost::bind(&tool::Daemon::splitArguments, boost::ref(_tool), _1) ),
         "tool arguments"
        )
        (
         "subset", po::value<tool::Subset>()
         ->default_value( tool::Subset() )
         ->notifier( boost::bind(&tool::Daemon::setSubset, boost::ref(_tool), _1) ),
         "rank subset"
        )
        (
         "interactive",
         po::value(&_interactive)->implicit_value(true)->default_value(false),
         "dump proctable after tool has been started"
        )
        (
         "socket",
         po::value(&_socket)->
         default_value(runjob::defaults::MuxLocalSocket),
         "runjob_mux listen socket"
        )
        ;
    
    // id is positional
    _positionalArgs.add( "id", 1 );

    // add generic args
    this->add(_options);

    // parse
    this->parse(_options);

    // notify
    this->notify();

    // get local socket
    if ( !_vm["socket"].defaulted() ) {
        // given specific arg, use it
    } else {
        // look in properties
        try {
            _socket = this->getProperties()->getValue(runjob::mux::PropertiesSection, "local_socket");
            LOG_DEBUG_MSG( "set local socket to " << _socket << " from properties file ");
        } catch (const std::invalid_argument& e) {
            // this isn't fatal, we'll use the default
            LOG_WARN_MSG( "missing local_socket key from " << PropertiesSection << " section in properties file" );
        }
    }
}

void
Options::validate() const
{
    // validate request
    if ( _job == 0 && _pid == 0 ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument("missing required options 'id' or 'pid'") );
    }

    if ( _debugger.attached() && _debugger.tool().getExecutable().empty() ) {;
        BOOST_THROW_EXCEPTION( std::invalid_argument("missing required option 'tool'") );
    } else if ( _tool.getExecutable().empty() ) {
        BOOST_THROW_EXCEPTION( std::invalid_argument("missing required option 'tool'") );
    }
}

void
Options::help(
        std::ostream& os
        ) const
{
    os <<
        "start a tool daemon.\n" <<
        "\n" <<
        "Requires Execute authority.\n" <<
        _options << std::endl
        ;
}

void
Options::handle(
        boost::asio::local::stream_protocol::socket& socket
        )
{
    const message::StartTool::Ptr start( new message::StartTool() );
    start->_description = _debugger.tool();
    start->_pid = _pid;
    start->setJobId( _job );
    start->_description = _debugger.attached() ? _debugger.tool(): _tool;

    boost::asio::streambuf buf;
    std::iostream os( &buf );
    start->serialize(os);

    // create header
    message::Header header;
    header._type = start->getType();
    header._length = static_cast<uint32_t>(buf.size());

    // use gather-write to send both header and message at the same time
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back( boost::asio::buffer( &header, sizeof(header) ) );
    buffers.push_back( buf.data() );

    size_t length = boost::asio::write( socket, buffers );
    buf.consume( length );
    LOG_TRACE_MSG( "wrote " << length << " bytes" );

    // wait for response
    length =  boost::asio::read(
            socket,
            boost::asio::buffer( &header, sizeof(header) )
            );
    LOG_TRACE_MSG( "read header " << length << " bytes" );

    length = boost::asio::read(
                socket,
                buf.prepare(header._length)
            );
    LOG_TRACE_MSG( "read body " << length << " bytes" );

    // commit buffer
    buf.commit( length );

    LOG_TRACE_MSG( 
            "converting " << 
            Message::toString( 
                static_cast<Message::Type>(header._type) 
                )
            << " message"
            );
    const Message::Ptr message = message::convert(header, buf);

    this->result( message );
}

void
Options::result(
        const Message::Ptr& message
        )
{
    if ( message->getType() == Message::Result ) {
        const message::Result::Ptr result = boost::static_pointer_cast<message::Result>( message );
        if ( result->getError() ) {
            BOOST_THROW_EXCEPTION(
                    std::runtime_error(error_code::toString(result->getError()) + std::string(": ") + result->getMessage() )
                    );
        } else {
            // should not get here
            BOOST_ASSERT( !"unhandled result with no error" );
        }
    } else if ( message->getType() == Message::Proctable ) {
        const message::Proctable::Ptr result = boost::static_pointer_cast<message::Proctable>( message );
        this->proctable( result );
    } else {
        BOOST_THROW_EXCEPTION(
                std::runtime_error( std::string("unknown response type: ") + Message::toString(message->getType()) )
                );
    }
}

void
Options::proctable(
        const message::Proctable::Ptr& result
        )
{
    // valid ranks in the proctable will have non-empty I/O node UCI
    const size_t ranks = std::count_if(
                result->_proctable.begin(),
                result->_proctable.end(),
                boost::bind(
                    std::not_equal_to<Uci>(),
                    boost::bind(
                        &runjob::tool::Rank::io,
                        _1
                        ),
                    Uci()
                    )
                );

    std::cout <<
        "tool " << result->_id << " started on " << 
        result->_io.size() << " I/O node" << (result->_io.size() == 1 ? "" : "s") << " for " <<
        ranks << " rank" << (ranks == 1 ? "" : "s") << "." <<
        std::endl;

    if ( _debugger.attached() ) {
        _debugger.fillProctable( 
                _debugger.tool().getExecutable(),
                result
                );
        return;
    }

    if ( !_interactive._value ) return;

    std::cout << "Enter a rank to see its associated I/O node's IP address, or press enter to quit: " << std::endl;

    while ( 1 ) {
        std::string line;
        std::getline( std::cin, line );
        LOG_TRACE_MSG( "line: " << line );

        if ( line.empty() ) return;

        // attempt to convert to rank
        try {
            const int rank = boost::lexical_cast<int>( line );
            if ( rank < 0 ) {
                std::cerr << "ranks must be positive." << std::endl;
                continue;
            }

            if ( static_cast<unsigned>(rank) >= result->_proctable.size() ) {
                std::cerr << "invalid rank, must be less than " << result->_proctable.size() << "." << std::endl;
                continue;
            }

            if ( result->_proctable[rank].io() == Uci() ) {
                std::cerr << "rank " << rank << " is not participating in the tool" << std::endl;
                continue;
            }

            const auto io = result->_io.find( result->_proctable[rank].io() );
            if ( io == result->_io.end() ) {
                std::cerr << "could not find I/O node associated with rank " << rank << "." << std::endl;
                continue;
            }

            std::cout <<
                "rank " << rank << 
                " has pid 0x" << std::hex << std::setfill('0') << std::setw(8) << result->_proctable[rank].pid() << std::dec <<
                " and uses I/O node " << io->first << 
                " at IP address " << io->second << 
                std::endl
                ;
        } catch ( const boost::bad_lexical_cast& e ) {
            std::cerr << "invalid rank, must be numeric." << std::endl;
        }
    }
}

} // start_tool
} // commands
} // mux
} // runjob
