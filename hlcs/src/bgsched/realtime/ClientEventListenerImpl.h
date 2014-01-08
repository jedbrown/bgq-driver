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


#ifndef BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_IMPL_H_
#define BGSCHED_REALTIME_CLIENT_EVENT_LISTENER_IMPL_H_


#include <bgsched/realtime/ClientEventListener.h>

#include "AbstractDatabaseChange.h"
#include "AbstractMessage.h"

#include <bgsched/Block.h>
#include <bgsched/types.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

#include <string>


namespace bgsched {
namespace realtime {


class ClientEventListener::RealtimeStartedEventInfo::Impl : public AbstractMessage
{
public:
    Impl( Filter::Id filter_id );
    Impl() { /* Nothing to do */ };

    Filter::Id getFilterId() const  { return _filter_id; }

private:

    Filter::Id _filter_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractMessage>(*this);
        ar & _filter_id;
    }
};


class ClientEventListener::RealtimeEndedEventInfo::Impl : public AbstractMessage
{
public:
    Impl();

private:

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractMessage>(*this);
    }
};


class ClientEventListener::BlockAddedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& block_id,
            Block::Status status,
            SequenceId sequence_id
        );

    const std::string& getBlockName() const  { return _block_id; }
    Block::Status getStatus() const { return _status; }
    SequenceId getSequenceId() const  { return _seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _block_id;
    Block::Status _status;
    SequenceId _seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _block_id;
        ar & _status;
        ar & _seq_id;
    }
};


class ClientEventListener::BlockStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& block_id,
            Block::Status status,
            SequenceId seq_id,
            Block::Status prev_status,
            SequenceId prev_seq_id
        );

    const std::string& getBlockName() const  { return _block_id; }
    Block::Status getStatus() const  { return _status; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Block::Status getPreviousStatus() const  { return _prev_status; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _block_id;
    Block::Status _status;
    SequenceId _seq_id;
    Block::Status _prev_status;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _block_id;
        ar & _status;
        ar & _seq_id;
        ar & _prev_status;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::BlockDeletedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& block_id,
            SequenceId prev_seq_id
        );

    const std::string& getBlockName() const  { return _block_id; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _block_id;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _block_id;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::JobAddedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            Job::Id job_id,
            const std::string compute_block_id,
            Job::Status status,
            SequenceId seq_id
        );

    Job::Id getJobId() const  { return _job_id; }
    const std::string& getComputeBlockName() const  { return _compute_block_id; }
    Job::Status getStatus() const  { return _status; }
    SequenceId getSequenceId() const  { return _seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    Job::Id  _job_id;
    std::string _compute_block_id;
    Job::Status _status;
    SequenceId _seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _job_id;
        ar & _compute_block_id;
        ar & _status;
        ar & _seq_id;
    }
};


class ClientEventListener::JobStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            Job::Id job_id,
            const std::string& compute_block_id,
            Job::Status status,
            SequenceId seq_id,
            Job::Status prev_status,
            SequenceId prev_seq_id
        );

    Job::Id getJobId() const  { return _job_id; }
    const std::string& getComputeBlockName() const  { return _compute_block_id; }
    Job::Status getStatus() const  { return _status; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Job::Status getPreviousStatus() const  { return _prev_status; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    Job::Id _job_id;
    std::string _compute_block_id;
    Job::Status _status;
    SequenceId _seq_id;
    Job::Status _prev_status;
    SequenceId _prev_seq_id;

    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _job_id;
        ar & _compute_block_id;
        ar & _status;
        ar & _seq_id;
        ar & _prev_status;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::JobDeletedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            Job::Id job_id,
            const std::string& block_id,
            SequenceId seq_id
        );

    Job::Id getJobId() const  { return _job_id; }
    const std::string& getComputeBlockName() const  { return _block_id; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    Job::Id _job_id;
    std::string _block_id;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _job_id;
        ar & _block_id;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::MidplaneStateChangedEventInfo::Impl : public AbstractDatabaseChange
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


class ClientEventListener::NodeBoardStateChangedEventInfo::Impl : public AbstractDatabaseChange
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


class ClientEventListener::NodeStateChangedEventInfo::Impl : public AbstractDatabaseChange
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


class ClientEventListener::SwitchStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& midplane_location,
            Dimension::Value dim,
            Hardware::State state,
            SequenceId seq_id,
            Hardware::State prev_state,
            SequenceId prev_seq_id
        );

    const std::string& getMidplaneLocation() const  { return _midplane_location; }
    Dimension::Value getDimension() const  { return _dimension; }
    Hardware::State getState() const  { return _state; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Hardware::State getPreviousState() const  { return _prev_state; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _midplane_location;
    Dimension::Value _dimension;
    Hardware::State _state;
    SequenceId _seq_id;
    Hardware::State _prev_state;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _midplane_location;
        ar & _dimension;
        ar & _state;
        ar & _seq_id;
        ar & _prev_state;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::TorusCableStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& from_location,
            const std::string& to_location,
            Hardware::State state,
            SequenceId seq_id,
            Hardware::State prev_state,
            SequenceId prev_seq_id
        );

    const std::string& getFromLocation() const  { return _from_location; }
    const std::string& getToLocation() const  { return _to_location; }
    Hardware::State getState() const  { return _state; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Hardware::State getPreviousState() const  { return _prev_state; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _from_location;
    std::string _to_location;
    Hardware::State _state;
    SequenceId _seq_id;
    Hardware::State _prev_state;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _from_location;
        ar & _to_location;
        ar & _state;
        ar & _seq_id;
        ar & _prev_state;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::IoCableStateChangedEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            const std::string& from_location,
            const std::string& to_location,
            Hardware::State state,
            SequenceId seq_id,
            Hardware::State prev_state,
            SequenceId prev_seq_id
        );

    const std::string& getFromLocation() const  { return _from_location; }
    const std::string& getToLocation() const  { return _to_location; }
    Hardware::State getState() const  { return _state; }
    SequenceId getSequenceId() const  { return _seq_id; }
    Hardware::State getPreviousState() const  { return _prev_state; }
    SequenceId getPreviousSequenceId() const  { return _prev_seq_id; }

    // override
    bool merge( const AbstractDatabaseChange& other );

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    std::string _from_location;
    std::string _to_location;
    Hardware::State _state;
    SequenceId _seq_id;
    Hardware::State _prev_state;
    SequenceId _prev_seq_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _from_location;
        ar & _to_location;
        ar & _state;
        ar & _seq_id;
        ar & _prev_state;
        ar & _prev_seq_id;
    }
};


class ClientEventListener::RasEventInfo::Impl : public AbstractDatabaseChange
{
public:
    Impl() { /* Nothing to do */ };

    Impl(
            RasRecordId rec_id,
            const std::string& msg_id,
            RasSeverity::Value severity,
            const std::string& block_id,
            Job::Id job_id
        );

    RasRecordId getRecordId() const  { return _rec_id; }
    const std::string& getMessageId() const  { return _msg_id; }
    RasSeverity::Value getSeverity() const  { return _severity; }
    const std::string& getBlockId() const  { return _block_id; }
    Job::Id getJobId() const  { return _job_id; }

    // override
    void accept( AbstractDatabaseChangeVisitor& v );

private:
    RasRecordId _rec_id;
    std::string _msg_id;
    RasSeverity::Value _severity;
    std::string _block_id;
    Job::Id _job_id;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int /*version*/ )
    {
        ar & boost::serialization::base_object<AbstractDatabaseChange>(*this);
        ar & _rec_id;
        ar & _msg_id;
        ar & _severity;
        ar & _block_id;
        ar & _job_id;
    }
};


} } // namespace bgsched::realtime

#endif
