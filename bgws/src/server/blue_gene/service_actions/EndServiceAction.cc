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

#include "EndServiceAction.hpp"

#include "../../blue_gene.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

#include <boost/xpressive/xpressive.hpp>


using namespace boost::xpressive;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace blue_gene {
namespace service_actions {


namespace statics {


const string& calcHardwareType( const std::string& location_str )
{
    // Service action allows a location string like Q00-K, Q00-K0, Q00-K1 to service /either/ clock card on I/O rack.
	static const sregex CLOCK_CARD_ON_IO_RACK_REGEX = (s1= (as_xpr('Q') >> set[range('0','9') | range('A','V')] >> set[range('0','9') | range('A','V')])) >> "-K" >> !(as_xpr('0') | '1');

	smatch what;
	if ( regex_match( location_str, what, CLOCK_CARD_ON_IO_RACK_REGEX ) ) {
        return hardware_type_name::CLOCK_CARD;
	}

    return hardware_type_name::fromLocation( bgq::util::Location( location_str ) );
}


} // statics


EndServiceAction::EndServiceAction(
        const std::string& location,
        const std::string& username,
        const boost::filesystem::path& service_action_executable_path,
        const std::string& properties_filename,
        utility::ChildProcesses& child_processes
    ) :
        _location(location),
        _username(username)
{
    const string &hardware_type_name(statics::calcHardwareType( _location ));

    utility::ChildProcess::Args args = {
            hardware_type_name,
            _location,
            action::END,
            "--user",
            username,
            "--properties",
            properties_filename
        };

    _process_ptr = child_processes.createEasy(
            string() + "endServiceAction@" + _location,
            service_action_executable_path,
            args
        );

    _process_ptr->setPreExecFn( &setCwdBaremetalBin );
}


void EndServiceAction::start(
        EndCb cb
    )
{
    _cb = cb;

    LOG_INFO_MSG( "Starting end service action on " << _location );

    _process_ptr->start(
            boost::bind( &EndServiceAction::_handleProcessEnded, shared_from_this(), _1 ),
            boost::bind( &EndServiceAction::_handleLine, shared_from_this(), _1, _2, _3 )
        );
}


void EndServiceAction::_handleProcessEnded(
        const bgq::utility::ExitStatus& // exit_status
    )
{
    // Nothing to do.
}


void EndServiceAction::_handleLine(
        utility::EasyChildProcess::OutputType output_type,
        utility::EasyChildProcess::OutputIndicator output_ind,
        const std::string& line
    )
{
    // Only care about stdout.
    if ( output_type != utility::EasyChildProcess::OutputType::Out ) {
        return;
    }

    // Only care about normal output.
    if ( output_ind != utility::EasyChildProcess::OutputIndicator::Normal ) {

        // End of output or error, either way not going to get confirmation.
        if ( _cb ) {
            if ( _error_text == string() )  _error_text = "end service action process exited before it indicated that the end service action was started";
            _cb( _error_text );
            _cb = EndCb();
        }

        return;
    }

    // Already got confirmation, so ignore this.
    if ( ! _cb )  return;

    static const string ENDED_STR("has been updated to 'END' state with status 'A' in the database");

    if ( line.find( ENDED_STR ) != string::npos ) {
        // Got confirmation that end service action started.
        _cb( string() );
        _cb = EndCb();
    }

    // line didn't contain what we were looking for.
}


} } } // namespace bgws::blue_gene::service_actions
