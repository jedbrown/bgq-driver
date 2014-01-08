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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "EzTimer.hpp"

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace utility {


EzTimer::EzTimer(
        const std::string& name
    ) :
        _name(name),
        _start_time(boost::posix_time::microsec_clock::local_time())
{
    LOG_INFO_MSG( "Started " << _name << " timer at " << _start_time );
}


EzTimer::~EzTimer()
{
    boost::posix_time::ptime now(boost::posix_time::microsec_clock::local_time());
    LOG_INFO_MSG( _name + " timer took " << (now - _start_time) << " seconds." );
}


} } // namespace bgws::utility
