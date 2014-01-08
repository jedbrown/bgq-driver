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
/* (C) Copyright IBM Corp.  2012, 2011                              */
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


namespace bgws {
namespace teal {
namespace errors {


NotFound::NotFound( const std::string& output )
    : std::runtime_error( "TEAL alert not found" ),
      _output(output)

{
    // Nothing to do.
}


InvalidState::InvalidState( const std::string& output )
    : std::runtime_error( "TEAL alert is not in the correct state for the action" ),
      _output(output)
{
    // Nothing to do.
}


Duplicate::Duplicate( const std::string& output )
    : std::runtime_error( "operation is not valid for the TEAL alert because it is a duplicate"),
      _output(output)
{
    // Nothing to do.
}


} } } // namespace bgws::teal::errors
