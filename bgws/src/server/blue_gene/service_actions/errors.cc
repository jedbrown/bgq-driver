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


#include "errors.hpp"


using std::string;


namespace bgws {
namespace blue_gene {
namespace service_actions {


InvalidLocationError::InvalidLocationError()
    : std::runtime_error( "couldn't start prepare service action because the location is not valid for a service action" )
{
    // Nothing to do.
}


HardwareDoesntExistError::HardwareDoesntExistError()
    : std::runtime_error( "couldn't start prepare service action because hardware at the location doesn't exist" )
{
    // Nothing to do.
}


NoIdProvidedError::NoIdProvidedError( const std::string& error_msg )
    : std::runtime_error( string() + "starting the service action failed. Didn't find the service action ID in the output of the program. The error message is " + error_msg ),
      _error_msg(error_msg)
{
    // Nothing to do.
}


ConflictError::ConflictError()
    : std::runtime_error( "Could not start service action because a conflicting service action is already in progress" )
{
    // Nothing to do.
}


} } } // namespace bgws::blue_gene::service_actions
