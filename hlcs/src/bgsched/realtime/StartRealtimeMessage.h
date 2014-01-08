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
#ifndef BGSCHED_REALTIME_SERVER_API_MSGS_H
#define BGSCHED_REALTIME_SERVER_API_MSGS_H

#include "AbstractMessage.h"

#include "FilterImpl.h"

namespace bgsched {
namespace realtime {

class StartRealtimeMessage : public AbstractMessage
{
public:

    StartRealtimeMessage(
            const bgsched::realtime::Filter::Impl& filter,
            bgsched::realtime::Filter::Id filter_id
        );

    StartRealtimeMessage() : _filter(bgsched::realtime::Filter::Impl::NONE) {}

    const bgsched::realtime::Filter::Impl& getFilter() const  { return _filter; }
    bgsched::realtime::Filter::Id getFilterId() const  { return _filter_id; }

private:

    bgsched::realtime::Filter::Impl _filter;
    bgsched::realtime::Filter::Id _filter_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractMessage>( *this );
        ar & _filter;
        ar & _filter_id;
    }
};

} } // namespace bgsched::realtime

#endif
