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
#include "StartRealtimeMessage.h"

#include <boost/serialization/export.hpp>

using namespace std;

BOOST_CLASS_EXPORT_GUID( bgsched::realtime::StartRealtimeMessage, "StartRealtime" )

namespace bgsched {
namespace realtime {

StartRealtimeMessage::StartRealtimeMessage(
        const bgsched::realtime::Filter::Impl& filter,
        bgsched::realtime::Filter::Id filter_id
    ) :
        _filter(filter),
        _filter_id(filter_id)
{
    // Nothing to do.
}

} } // namespace bgsched::realtime
