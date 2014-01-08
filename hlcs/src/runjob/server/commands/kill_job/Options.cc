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
 * \page kill_job
 *
 * deliver a signal to a BG/Q job asynchronously.
 *
 * \section SYNOPSIS
 *
 * kill_job id -SIGNAL [OPTIONS]
 *
 * \section DESCRIPTION
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * Delivers a signal to a job asynchronously. Signals other than SIGKILL can only be sent to a Running job.
 * Only one outstanding signal can be delivered to a job. In other words, all I/O nodes participating in the
 * job must acknowledge the signal was delivered before another signal can be sent. Attempting to send signals
 * in quick succession with kill_job may result in subsequent invocations failing with a descriptive error
 * indicating the first signal is still in flight.
 *
 * \section OPTIONS
 *
 * \subsection pid --pid.
 *
 * runjob process ID. Either this parameter
 * or --id must be given.
 *
 * \subsection id --id
 *
 * Numeric job ID, this may be specified as the first positional argument as well.
 *
 * \subsection signal --signal
 *
 * Signal to send. This may be specified as an integer or string. The --signal and -s switch is optional,
 * instead the signal number or string can be prefixed by a single - character similar to the kill command
 * in most shell environments. When specifying a string, it must be one of the well known signals, see
 * --list for more information. When specifying a number, it must be greater than zero.
 *
 * If no signal is provided, SIGKILL is sent by default.
 *
 * \subsection timeout --timeout
 *
 * A positive non-zero number of seconds to start a timeout when delivering SIGKILL. If the job has not finished
 * on its own after this value, it is forcefully terminated and the nodes in use will be unavailable for
 * future jobs until the block is rebooted. This value is ignored for signals other than SIGKILL. The time
 * remaining for this timeout can be queried with the job_status command. It can also be reduced by subsequent
 * invocations of kill_job with a smaller --timeout parameter.
 *
 * \subsection list --list
 *
 * Display a list of signal names and numbers.
 *
 * COMMON_ARGUMENTS_GO_HERE
 *
 * \section exit EXIT STATUS
 *
 * 0 on success, 1 otherwise.
 *
 * \section examples EXAMPLES
 *
 * kill_job -TERM 123
 *
 * sends a SIGTERM to job 123.
 *
 * kill_job 456
 *
 * sends a SIGKILL to job 456.
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
 */


#include "server/commands/kill_job/Options.h"

#include "common/defaults.h"
#include "common/logging.h"
#include "common/properties.h"

#include <utility/include/UserId.h>

#include <boost/asio/ip/host_name.hpp>
#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

#include <signal.h>


LOG_DECLARE_FILE( runjob::server::commands::log );

namespace runjob {
namespace server {
namespace commands {
namespace kill_job {

const Options::Signals Options::signals = boost::assign::list_of
(Options::Signal("HUP", SIGHUP))
(Options::Signal("INT", SIGINT))
(Options::Signal("QUIT", SIGQUIT))
(Options::Signal("ILL", SIGILL))
(Options::Signal("TRAP", SIGTRAP))
(Options::Signal("ABRT", SIGABRT))
(Options::Signal("BUS", SIGBUS))
(Options::Signal("FPE", SIGFPE))
(Options::Signal("KILL", SIGKILL))
(Options::Signal("USR1", SIGUSR1))
(Options::Signal("SEGV", SIGSEGV))
(Options::Signal("USR2", SIGUSR2))
(Options::Signal("PIPE", SIGPIPE))
(Options::Signal("ALRM", SIGALRM))
(Options::Signal("TERM", SIGTERM))
(Options::Signal("STKFLT", SIGSTKFLT))
(Options::Signal("CHLD", SIGCHLD))
(Options::Signal("CONT", SIGCONT))
(Options::Signal("STOP", SIGSTOP))
(Options::Signal("TSTP", SIGTSTP))
(Options::Signal("TTIN", SIGTTIN))
(Options::Signal("TTOU", SIGTTOU))
(Options::Signal("URG", SIGURG))
(Options::Signal("XCPU", SIGXCPU))
(Options::Signal("XFSZ", SIGXFSZ))
(Options::Signal("VTALRM", SIGVTALRM))
(Options::Signal("PROF", SIGPROF))
(Options::Signal("WINCH", SIGWINCH))
(Options::Signal("IO", SIGIO))
(Options::Signal("PWR", SIGPWR))
(Options::Signal("SYS", SIGSYS))
;

Options::Options(
        unsigned int argc,
        char** argv
        ) :
    runjob::commands::Options( defaults::ServerCommandService, runjob::server::commands::log, runjob::commands::Message::Tag::KillJob, argc, argv ),
    _pid( 0 ),
    _hostname(),
    _options( "Options" ),
    _signal(),
    _timeout( 0 )
{
    namespace po = boost::program_options;

    _options.add_options()
        ("pid", po::value(&_pid), "runjob process ID")
        ("id", po::value(&_job), "job ID to signal")
        ("signal,s", po::value(&_signal)->default_value("KILL"), "signal to deliver")
        ("list,l", po::bool_switch(), "list all valid signals")
        ("timeout", po::value(&_timeout)->default_value(defaults::ServerKillJobTimeout), "number of seconds to wait after\ndelivering a SIGKILL. Ignored for other\nsignals.")
        ;

    // --runjob-hostname is hidden
    boost::program_options::options_description hidden;
    hidden.add_options()
        ("runjob-hostname", po::value(&_hostname), "hostname where runjob was started")
        ;

    // job ID is positional
    _positionalArgs.add( "id", 1 );

    // extra parser for signal number
    AbstractOptions::ExtraParser extra =
        boost::bind(
                &Options::signalParser,
                this,
                _1
                );

    po::options_description both;
    both.add( _options );
    both.add( hidden );

    // add generic args
    Options::add( 
            runjob::server::commands::PropertiesSection,
            both,
            &extra
            );
}

void
Options::doHelp(
        std::ostream& os
        ) const
{
    os << _options << std::endl;
}

const char*
Options::description() const
{
    return
        "Send a signal to a running job.\n"
        "\n"
        "Requires Execute authority on job <id>.";
}

void
Options::doValidate() const
{
    if ( _job == 0 && _pid == 0 ) {
        BOOST_THROW_EXCEPTION( boost::program_options::error("missing required option 'id'") );
    }

    // convert signal to integer
    const int signal = this->convertSignal( _signal );
    if ( !signal ) {
        BOOST_THROW_EXCEPTION( boost::program_options::invalid_option_value("signal") );
    }

    const runjob::commands::request::KillJob::Ptr request(
            boost::static_pointer_cast<runjob::commands::request::KillJob>( this->getRequest() )
            );
    request->_job = _job;
    request->_pid = _pid;
    request->_hostname = _hostname;
    request->_signal = signal;
    request->_timeout = _timeout;
    request->_details = this->getDetails();
}
   
int
Options::convertSignal(
        const std::string& signal
        ) const
{
    int result = 0;

    if ( signal.empty() ) return result;

    BOOST_FOREACH( const Signal& i, signals ) {
        // name of signal is first tuple
        // number of signal is second tuple
        const std::string name = boost::get<0>(i);
        const int number = boost::get<1>(i);
        if ( name == signal ) {
            // we support both INT and...
            result = number;
            break; 
        } else if ( signal.size() > 3 && name == signal.substr(3) ) {
            // SIGINT and...
            result = number;
            break;
        } else if ( signal == boost::lexical_cast<std::string>(number) ) {
            // a number
            result = number;
            break;
        }
    }

    if ( !result ) {
        // we didn't match a signal name or number we know about so
        // check for valid positive numbers
        try {
            result = boost::lexical_cast<int>(signal);
            if ( result <= 0 ) return 0;
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_DEBUG_MSG( e.what() );
        }
    }

    return result;
}

std::pair<std::string,std::string>
Options::signalParser(
        const std::string& arg
        )
{
    std::pair<std::string,std::string> result;

    // only look at args that are prefixed with a single -
    if ( arg[0] != '-' ) return result;
    if ( arg.size() < 2) return result;
    if ( arg[1] == '-' ) return result;

    // strip off leading - char
    const int signal = this->convertSignal( arg.substr(1) );
    if ( signal ) {
        result = std::make_pair("signal", arg.substr(1) );
    }

    return result;
}

std::string
Options::getDetails() const
{
    std::ostringstream response;
    try {
        const bgq::utility::UserId uid;
        response << "Delivered by " << boost::filesystem::basename(_argv[0]);
        response << " user " << uid.getUser() << " on host " << boost::asio::ip::host_name();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    return response.str();
}

} // kill_job
} // commands
} // server
} // runjob
