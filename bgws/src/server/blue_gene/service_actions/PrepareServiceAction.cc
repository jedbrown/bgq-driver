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

#include "PrepareServiceAction.hpp"

#include "errors.hpp"

#include "../../blue_gene.hpp"
#include "../../dbConnectionPool.hpp"

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/throw_exception.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/xpressive/xpressive.hpp>

#include <string>


using namespace boost::xpressive;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace blue_gene {
namespace service_actions {


namespace statics {

const string& checkLocationExists( const std::string& location_str )
{
    // Service action allows a location string like Q00-K, Q00-K0, Q00-K1 to service /either/ clock card on I/O rack.
	static const sregex CLOCK_CARD_ON_IO_RACK_REGEX = (s1= (as_xpr('Q') >> set[range('0','9') | range('A','V')] >> set[range('0','9') | range('A','V')])) >> "-K" >> !(as_xpr('0') | '1');

	smatch what;
	if ( regex_match( location_str, what, CLOCK_CARD_ON_IO_RACK_REGEX ) ) {
		const string &rack_location(what[1]);
        statics::checkLocationExists( rack_location );
        return hardware_type_name::CLOCK_CARD;
	}

    bgq::util::Location location( location_str );

    const string &hardware_type_name(hardware_type_name::fromLocation( location ));

    if ( ! BGQDB::checkLocationExists( location ) ) {
        BOOST_THROW_EXCEPTION( HardwareDoesntExistError() );
    }

    return hardware_type_name;
}

} // namespace statics


PrepareServiceAction::PrepareServiceAction(
        const std::string& location,
        const std::string& username,
        const boost::filesystem::path& service_action_executable_path,
        const std::string& properties_filename,
        utility::ChildProcesses& child_processes
    ) :
        _location(location),
        _username(username)
{
    try {

        const string& hardware_type_name(statics::checkLocationExists( location ));
            // throws if hardware doesn't exist.

        _process_ptr = child_processes.createEasy(
                string() + "prepareServiceAction@" + _location,
                service_action_executable_path,
                utility::ChildProcess::Args {
                        hardware_type_name,
                        _location,
                        action::PREPARE,
                        "--user",
                        username,
                        "--properties",
                        properties_filename
                    }
            );

        _process_ptr->setPreExecFn( &setCwdBaremetalBin );

    } catch ( bgq::util::LocationError& e ) {

        BOOST_THROW_EXCEPTION( InvalidLocationError() );

    } catch ( hardware_type_name::InvalidLocationError& ile ) {

        BOOST_THROW_EXCEPTION( InvalidLocationError() );

    }
}


void PrepareServiceAction::start(
        StartCb start_cb,
        NotifyAttentionMessagesCb notify_attention_messages_cb
    )
{
    _start_cb = start_cb;
    _notify_attention_messages_cb = notify_attention_messages_cb;

    LOG_INFO_MSG( "Starting service action on " << _location );

    _process_ptr->start(
            boost::bind( &PrepareServiceAction::_handleProcessEnded, shared_from_this(), _1 ),
            boost::bind( &PrepareServiceAction::_handleLine, shared_from_this(), _1, _2, _3 )
        );
}


void PrepareServiceAction::_handleProcessEnded(
        const bgq::utility::ExitStatus& //exit_status
    )
{

    if ( _notify_attention_messages_cb && (! _id.empty()) && (! _attention_text.empty()) ) {
        _notify_attention_messages_cb( _id, _attention_text );
        _notify_attention_messages_cb = NotifyAttentionMessagesCb();
    }

}


void PrepareServiceAction::_handleLine(
        utility::EasyChildProcess::OutputType output_type,
        utility::EasyChildProcess::OutputIndicator output_ind,
        const std::string& line
    )
{
    try {

        // Only care about stdout.
        if ( output_type != utility::EasyChildProcess::OutputType::Out ) {
            return;
        }

        // Only care about normal output.
        if ( output_ind != utility::EasyChildProcess::OutputIndicator::Normal ) {

            if ( ! _start_cb ) {
                // end of input and either reported service action ID or already reported error.
                return;
            }

            // End of output and haven't reported service action ID or an error.
            if ( _error_text == string() ) {
                // No service action ID and no error message, use normal error message.
                BOOST_THROW_EXCEPTION( NoIdProvidedError( "Service action exited before providing an ID." ) );
            }

            BOOST_THROW_EXCEPTION( NoIdProvidedError( _error_text ) );

            return;
        }

        _checkLineStartup( line );

        _checkLineAttention( line );

    } catch ( std::exception &e ) {

        if ( _start_cb ) {
            _start_cb( std::current_exception(), "" );
            _start_cb = StartCb();
        } else {
            // Nothing to do.
        }

    }
}


void PrepareServiceAction::_checkLineStartup(
        const std::string& line
    )
{
    // Already got the service action ID, so ignore any further program output.
    if ( ! _start_cb )  return;

    static const sregex ID_REGEX = as_xpr( ":   Service Action" ) >> +_s >> (s1=+~_s);
    static const sregex FAIL_START_REGEX = as_xpr( ": !" ) >> +_s >> (s1=*_);

    smatch what;

    if ( regex_search( line, what, ID_REGEX ) ) {
        // Found the service action ID.

        _id = what[1];

        LOG_INFO_MSG( _process_ptr << " service action ID is '" << _id << "'" );

        _start_cb( std::exception_ptr(), _id );
        _start_cb = StartCb();

        return;
    }

    if ( regex_search( line, what, FAIL_START_REGEX ) ) {

        string err_msg(what[1]);

        LOG_WARN_MSG( _process_ptr << " Error starting service_action, " << err_msg );

        // If output contains "return code -203" then indicates a ServiceActionConflict.
        if ( err_msg.find( "return code -203" ) != string::npos ) {
            BOOST_THROW_EXCEPTION( ConflictError() );
        }

        if ( _error_text == string() )  _error_text = err_msg;

        return;
    }
}


void PrepareServiceAction::_checkLineAttention(
        const std::string& line
    )
{
    // Look for lines that contain ATTENTION, ignore those that don't.

    static const sregex ATTENTION_REGEX = as_xpr( "ATTENTION" ) >> *_s >> (s1=*_);

    smatch what;

    if ( ! regex_search( line, what, ATTENTION_REGEX ) ) {
        return;
    }

    _attention_text += what[1] + "\n";
}


} } } // namespace bgws::blue_gene::service_actions
