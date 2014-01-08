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

#include "CloseServiceAction.hpp"

#include "../../blue_gene.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace blue_gene {
namespace service_actions {


CloseServiceAction::CloseServiceAction(
        const std::string& location,
        const std::string& username,
        const boost::filesystem::path& service_action_executable_path,
        const std::string& properties_filename,
        utility::ChildProcesses& child_processes
    ) :
        _location(location),
        _username(username)
{
    const string &hardware_type_name(hardware_type_name::fromLocation( bgq::util::Location( _location ) ));

    utility::ChildProcess::Args args = {
            hardware_type_name,
            _location,
            action::CLOSE,
            "--user",
            username,
            "--properties",
            properties_filename
        };

    _process_ptr = child_processes.createEasy(
            string() + "closeServiceAction@" + _location,
            service_action_executable_path,
            args
        );

    _process_ptr->setPreExecFn( &setCwdBaremetalBin );
}


void CloseServiceAction::start(
        CloseCb cb
    )
{
    _cb = cb;

    LOG_INFO_MSG( "Starting close service action on " << _location );

    _process_ptr->start(
            boost::bind( &CloseServiceAction::_handleProcessEnded, shared_from_this(), _1 ),
            boost::bind( &CloseServiceAction::_handleLine, shared_from_this(), _1, _2, _3 )
        );
}


void CloseServiceAction::_handleProcessEnded(
        const bgq::utility::ExitStatus& // exit_status
    )
{
    // Nothing to do.
}


void CloseServiceAction::_handleLine(
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

            // End of output or error, either way not going to get confirmation.
            if ( _cb ) {
                BOOST_THROW_EXCEPTION( std::runtime_error( "close service action process exited before it indicated that the close service action was started" ) );
            }

            return;
        }

        // Already got confirmation, so ignore this.
        if ( ! _cb )  return;

        static const string STARTED_STR("has been updated to 'CLOSED' state with status 'F' in the database.");

        if ( line.find( STARTED_STR ) != string::npos ) {
            // Got confirmation that end service action started.
            _cb( std::exception_ptr() );
            _cb = CloseCb();
        }

        // line didn't contain what we were looking for.
    } catch ( std::exception& e ) {
        _cb( std::current_exception() );
        _cb = CloseCb();
    }
}


} } } // namespace bgws::blue_gene::service_actions
