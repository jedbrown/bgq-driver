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


#ifndef BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_V2_IMPL_H_
#define BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_V2_IMPL_H_


#include <bgsched/realtime/ClientEventListenerV2.h>

#include "AbstractDatabaseChange.h"
#include "AbstractMessage.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include <string>


namespace bgsched {
namespace realtime {


//------------------------------------------------------------
// ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl

class ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& location,
            Hardware::State state,
            SequenceId seq_id,
            Hardware::State prev_state,
            SequenceId prev_seq_id
        );

    const std::string& getLocation() const  { return _location; }
    Hardware::State getState() const  { return _state; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Hardware::State getPreviousState() const  { return _prev_state; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _location;
    Hardware::State _state;
    SequenceId _seq_id;
    Hardware::State _prev_state;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _location;
        ar & _state;
        ar & _seq_id;
        ar & _prev_state;
        ar & _prev_seq_id;
    }
};


//-----------------------------------------------------------
// ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl

class ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& location,
            Hardware::State state,
            SequenceId seq_id,
            Hardware::State prev_state,
            SequenceId prev_seq_id
        );

    const std::string& getLocation() const  { return _location; }
    Hardware::State getState() const  { return _state; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Hardware::State getPreviousState() const  { return _prev_state; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _location;
    Hardware::State _state;
    SequenceId _seq_id;
    Hardware::State _prev_state;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _location;
        ar & _state;
        ar & _seq_id;
        ar & _prev_state;
        ar & _prev_seq_id;
    }
};


} } // namespace bgsched::realtime


#endif
