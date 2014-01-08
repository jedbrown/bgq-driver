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


#include <bgsched/bgsched.h>

#include <bgsched/realtime/Client.h>
#include <bgsched/realtime/ClientEventListenerV2.h>
#include <bgsched/realtime/Filter.h>

#include <db/include/api/genblock.h>
#include <db/include/api/GenBlockParams.h>

#include <db/include/api/job/Operations.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Properties.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <string>
#include <sstream>

#include <unistd.h>


using boost::lexical_cast;

using std::cout;
using std::ostringstream;
using std::string;
using std::stringstream;


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE bgws
#include <boost/test/unit_test.hpp>


std::ostream& operator<<( std::ostream& os, const bgsched::Coordinates& coords )
{
    os << "(" << coords[0] << "," << coords[1] << "," << coords[2] << "," << coords[3] << ")";
    return os;
}

class EventListener : public bgsched::realtime::ClientEventListenerV2
{
public:

    string extractLog()
    {
        string ret(_log.str());
        _log.str( string() );
        return ret;
    }


    bool getRealtimeContinue() { return false; }

    void handleBlockAddedRealtimeEvent( const BlockAddedEventInfo& info )
    { _log << "blockAdded name=" << info.getBlockName() << " status=" << info.getStatus() << " seqId=" << info.getSequenceId() << "\n"; }

    void handleBlockStateChangedRealtimeEvent( const BlockStateChangedEventInfo& info )
    {
        _log << "blockState name=" << info.getBlockName() << " status=" << info.getStatus() << " seqId=" << info.getSequenceId()
             << " prevStatus=" << info.getPreviousStatus() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleBlockDeletedRealtimeEvent( const BlockDeletedEventInfo& info )
    { _log << "blockDeleted name=" << info.getBlockName() << " prevSeqId=" << info.getPreviousSequenceId() << "\n"; }

    virtual void handleJobAddedRealtimeEvent( const JobAddedEventInfo& info )
    { _log << "jobAdded id=" << info.getJobId() << " block=" << info.getComputeBlockName() << " status=" << info.getStatus() << " seqId=" << info.getSequenceId() << "\n"; }

    void handleJobStateChangedRealtimeEvent( const JobStateChangedEventInfo& info )
    {
        _log << "jobState id=" << info.getJobId() << " block=" << info.getComputeBlockName() << " status=" << info.getStatus() << " seqId=" << info.getSequenceId()
             << " prevStatus=" << info.getPreviousStatus() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleJobDeletedRealtimeEvent( const JobDeletedEventInfo& info )
    { _log << "jobDeleted id=" << info.getJobId() << " block=" << info.getComputeBlockName() << " prevSeqId=" << info.getPreviousSequenceId() << "\n"; }

    void handleMidplaneStateChangedRealtimeEvent( const MidplaneStateChangedEventInfo& info )
    {
        _log << "midplane location=" << info.getLocation() << " coords=" << info.getMidplaneCoordinates()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleNodeBoardStateChangedRealtimeEvent( const NodeBoardStateChangedEventInfo& info )
    {
        _log << "nodeBoard location=" << info.getLocation() << " midplaneCoords=" << info.getMidplaneCoordinates()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleNodeStateChangedRealtimeEvent( const NodeStateChangedEventInfo& info )
    {
        _log << "node location=" << info.getLocation() << " midplaneCoords=" << info.getMidplaneCoordinates()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleSwitchStateChangedRealtimeEvent( const SwitchStateChangedEventInfo& info )
    {
        _log << "switch midplaneLocation=" << info.getMidplaneLocation() << " midplaneCoords=" << info.getMidplaneCoordinates() << " dimension=" << info.getDimension()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleTorusCableStateChangedRealtimeEvent( const TorusCableStateChangedEventInfo& info )
    {
        _log << "cable from=" << info.getFromLocation() << " fromMp=" << info.getFromMidplaneLocation() << " fromMpCoords=" << info.getFromMidplaneCoordinates()
             << " to=" << info.getToLocation() << " toMp=" << info.getToMidplaneLocation() << " toMpCoords=" << info.getToMidplaneCoordinates()
             << " dimension=" << info.getDimension()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleIoCableStateChangedRealtimeEvent( const IoCableStateChangedEventInfo& info )
    {
        _log << "cable from=" << info.getFromLocation() << " fromMp=" << info.getFromMidplaneLocation() << " fromMpCoords=" << info.getFromMidplaneCoordinates()
             << " to=" << info.getToLocation()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleRasRealtimeEvent( const RasEventInfo& info )
    {
        _log << "ras recid=" << info.getRecordId() << " msgId=" << info.getMessageId() << " severity=" << info.getSeverity() << "\n";
    }

    void handleIoDrawerStateChangedEvent( const IoDrawerStateChangedEventInfo& info )
    {
        _log << "ioDrawer location=" << info.getLocation()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }

    void handleIoNodeStateChangedEvent( const IoNodeStateChangedEventInfo& info )
    {
        _log << "ioNode location=" << info.getLocation()
             << " state=" << info.getState() << " seqId=" << info.getSequenceId()
             << " prevState=" << info.getPreviousState() << " prevSeqId=" << info.getPreviousSequenceId() << "\n";
    }


private:

    stringstream _log;

};


class GlobalFixture
{
public:

    static GlobalFixture *p;


    static std::string waitMessageText()
    {
        p->_client_ptr->receiveMessages( NULL, NULL, NULL );

        return p->_event_listener_ptr->extractLog();
    }


    static BGQDB::job::Operations& getJobOps()  { return *(p->_job_ops_ptr); }

    static void setJobId( BGQDB::job::Id job_id )  { _s_job_id = job_id; }
    static BGQDB::job::Id getJobId()  { return _s_job_id; }


    GlobalFixture()
    {
        bgsched::init( "" /* properties_filename */ );

        _job_ops_ptr.reset( new BGQDB::job::Operations() );


        // Clean up.
        cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

        conn_ptr->executeUpdate( "DELETE FROM tbgqJob WHERE blockId='TEMP_TEST_REALTIME'" );
        conn_ptr->executeUpdate( "UPDATE tbgqBlock SET status='F' WHERE blockId='TEMP_TEST_REALTIME'" );
        conn_ptr->executeUpdate( "DELETE FROM tbgqBlock WHERE blockId='TEMP_TEST_REALTIME'" );
        conn_ptr->executeUpdate( "UPDATE tbgqMidplane SET status='A' WHERE posInMachine='R00-M0'" );
        conn_ptr->executeUpdate( "UPDATE tbgqNodeCard SET status='A' WHERE midplanePos='R00-M0' AND position='N00'" );
        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='A' WHERE fromLocation='R00-M0-N00-T00'" );

        _client_ptr.reset( new bgsched::realtime::Client() );

        _client_ptr->connect();

        _event_listener_ptr.reset( new EventListener() );

        _client_ptr->addListener( *_event_listener_ptr );

        bgsched::realtime::Filter::Id filter_id;
        _client_ptr->setFilter( bgsched::realtime::Filter::createAll(), &filter_id, NULL );

        _client_ptr->requestUpdates( NULL );

        _client_ptr->receiveMessages( NULL, NULL, NULL ); // expect rt started.


        p = this;
    }


    ~GlobalFixture()
    {
        _client_ptr.reset();
        _event_listener_ptr.reset();

        cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

        conn_ptr->executeUpdate( "DELETE FROM tbgqJob WHERE blockId='TEMP_TEST_REALTIME'" );
        conn_ptr->executeUpdate( "UPDATE tbgqBlock SET status='F' WHERE blockId='TEMP_TEST_REALTIME'" );
        conn_ptr->executeUpdate( "DELETE FROM tbgqBlock WHERE blockId='TEMP_TEST_REALTIME'" );
    }


private:

    static BGQDB::job::Id _s_job_id;

    boost::shared_ptr<BGQDB::job::Operations> _job_ops_ptr;
    boost::shared_ptr<bgsched::realtime::Client> _client_ptr;
    boost::shared_ptr<EventListener> _event_listener_ptr;
};


GlobalFixture *GlobalFixture::p = NULL;
BGQDB::job::Id GlobalFixture::_s_job_id = 0;


static bgsched::SequenceId getBlockSeqId( const std::string& block_id )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqBlock WHERE blockId='TEMP_TEST_REALTIME'" ));

    rs_ptr->fetch();

    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


static bgsched::SequenceId getJobSeqId( BGQDB::job::Id job_id )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqJob WHERE id=" + lexical_cast<string>(job_id) ));

    rs_ptr->fetch();

    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_GLOBAL_FIXTURE( GlobalFixture );


BOOST_AUTO_TEST_CASE( test_block_added )
{
    BGQDB::GenBlockParams gen_block_params;

    gen_block_params.setBlockId( "TEMP_TEST_REALTIME" );
    gen_block_params.setOwner( getlogin() );
    gen_block_params.setMidplane( "R00-M0" );
    gen_block_params.setNodeBoardAndCount( "N00", 1 );

    BGQDB::genBlockEx(
            gen_block_params
        );

    ostringstream exp_oss;
    exp_oss << "blockAdded name=TEMP_TEST_REALTIME status=2 seqId=" << getBlockSeqId( "TEMP_TEST_REALTIME" ) << "\n";

    BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
}


BOOST_AUTO_TEST_CASE( test_block_status_change_ABI )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    {
        prev_seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        BGQDB::setBlockStatus( "TEMP_TEST_REALTIME", BGQDB::ALLOCATED );

        seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        ostringstream exp_oss;
        exp_oss << "blockState name=TEMP_TEST_REALTIME status=0 seqId=" << seq_id << " prevStatus=2 prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        BGQDB::setBlockStatus( "TEMP_TEST_REALTIME", BGQDB::BOOTING );

        seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        ostringstream exp_oss;
        exp_oss << "blockState name=TEMP_TEST_REALTIME status=1 seqId=" << seq_id << " prevStatus=0 prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        BGQDB::setBlockStatus( "TEMP_TEST_REALTIME", BGQDB::INITIALIZED );

        seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        ostringstream exp_oss;
        exp_oss << "blockState name=TEMP_TEST_REALTIME status=3 seqId=" << seq_id << " prevStatus=1 prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }
}


BOOST_AUTO_TEST_CASE( test_job_added )
{
    BGQDB::job::InsertInfo insert_info;
    insert_info.setBlock( "TEMP_TEST_REALTIME" );

    BGQDB::job::Id job_id;

    GlobalFixture::getJobOps().insert(
            insert_info,
            &job_id
        );

    bgsched::SequenceId seq_id(getJobSeqId( job_id ));

    ostringstream exp_oss;
    exp_oss << "jobAdded id=" << job_id << " block=TEMP_TEST_REALTIME status=0 seqId=" << seq_id << "\n";

    BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );


    GlobalFixture::setJobId( job_id );

}


BOOST_AUTO_TEST_CASE( test_job_status_change )
{
    bgsched::SequenceId prev_seq_id, seq_id;

    {
        prev_seq_id = getJobSeqId( GlobalFixture::getJobId() );

        GlobalFixture::getJobOps().update(
                GlobalFixture::getJobId(),
                BGQDB::job::status::Loading
            );

        seq_id = getJobSeqId( GlobalFixture::getJobId() );

        ostringstream exp_oss;
        exp_oss << "jobState id=" << GlobalFixture::getJobId() << " block=TEMP_TEST_REALTIME status=1 seqId=" << seq_id
                << " prevStatus=0 prevSeqId=" << prev_seq_id << "\n";
        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        GlobalFixture::getJobOps().update(
                GlobalFixture::getJobId(),
                BGQDB::job::status::Debug
            );

        seq_id = getJobSeqId( GlobalFixture::getJobId() );

        ostringstream exp_oss;
        exp_oss << "jobState id=" << GlobalFixture::getJobId() << " block=TEMP_TEST_REALTIME status=4 seqId=" << seq_id
                << " prevStatus=1 prevSeqId=" << prev_seq_id << "\n";
        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        GlobalFixture::getJobOps().update(
                GlobalFixture::getJobId(),
                BGQDB::job::status::Starting
            );

        seq_id = getJobSeqId( GlobalFixture::getJobId() );

        ostringstream exp_oss;
        exp_oss << "jobState id=" << GlobalFixture::getJobId() << " block=TEMP_TEST_REALTIME status=2 seqId=" << seq_id
                << " prevStatus=4 prevSeqId=" << prev_seq_id << "\n";
        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        GlobalFixture::getJobOps().update(
                GlobalFixture::getJobId(),
                BGQDB::job::status::Running
            );

        seq_id = getJobSeqId( GlobalFixture::getJobId() );

        ostringstream exp_oss;
        exp_oss << "jobState id=" << GlobalFixture::getJobId() << " block=TEMP_TEST_REALTIME status=3 seqId=" << seq_id
                << " prevStatus=2 prevSeqId=" << prev_seq_id << "\n";
        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        GlobalFixture::getJobOps().update(
                GlobalFixture::getJobId(),
                BGQDB::job::status::Cleanup
            );

        seq_id = getJobSeqId( GlobalFixture::getJobId() );

        ostringstream exp_oss;
        exp_oss << "jobState id=" << GlobalFixture::getJobId() << " block=TEMP_TEST_REALTIME status=5 seqId=" << seq_id
                << " prevStatus=3 prevSeqId=" << prev_seq_id << "\n";
        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }
}


BOOST_AUTO_TEST_CASE( test_job_deleted )
{
    bgsched::SequenceId prev_seq_id(getJobSeqId( GlobalFixture::getJobId() ));

    GlobalFixture::getJobOps().remove(
            GlobalFixture::getJobId(),
            BGQDB::job::RemoveInfo()
        );

    ostringstream exp_oss;
    exp_oss << "jobDeleted id=" << GlobalFixture::getJobId() << " block=TEMP_TEST_REALTIME prevSeqId=" << prev_seq_id << "\n";
    BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
}


BOOST_AUTO_TEST_CASE( test_block_status_change_TF )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    {
        prev_seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        BGQDB::setBlockStatus( "TEMP_TEST_REALTIME", BGQDB::TERMINATING );

        seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        ostringstream exp_oss;
        exp_oss << "blockState name=TEMP_TEST_REALTIME status=4 seqId=" << seq_id << " prevStatus=3 prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        BGQDB::setBlockStatus( "TEMP_TEST_REALTIME", BGQDB::FREE );

        seq_id = getBlockSeqId( "TEMP_TEST_REALTIME" );

        ostringstream exp_oss;
        exp_oss << "blockState name=TEMP_TEST_REALTIME status=2 seqId=" << seq_id << " prevStatus=4 prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}


BOOST_AUTO_TEST_CASE( test_block_deleted )
{
    bgsched::SequenceId prev_seq_id(getBlockSeqId( "TEMP_TEST_REALTIME" ));

    if ( BGQDB::deleteBlock( "TEMP_TEST_REALTIME" ) != BGQDB::OK )  { throw std::runtime_error( "delete failed" ); }

    ostringstream exp_oss;
    exp_oss << "blockDeleted name=TEMP_TEST_REALTIME prevSeqId=" << prev_seq_id << "\n";

    BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
}


static bgsched::SequenceId getMidplaneSeqId( const std::string& location )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM tbgqMidplane where posInMachine='" + location + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_midplane )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    {
        prev_seq_id = getMidplaneSeqId( "R00-M0" );

        conn_ptr->executeUpdate( "UPDATE tbgqMidplane SET status='E' WHERE posInMachine='R00-M0'" );

        seq_id = getMidplaneSeqId( "R00-M0" );

        ostringstream exp_oss;
        exp_oss << "midplane location=R00-M0 coords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqMidplane SET status='M' WHERE posInMachine='R00-M0'" );

        seq_id = getMidplaneSeqId( "R00-M0" );

        ostringstream exp_oss;
        exp_oss << "midplane location=R00-M0 coords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqMidplane SET status='S' WHERE posInMachine='R00-M0'" );

        seq_id = getMidplaneSeqId( "R00-M0" );

        ostringstream exp_oss;
        exp_oss << "midplane location=R00-M0 coords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqMidplane SET status='F' WHERE posInMachine='R00-M0'" );

        seq_id = getMidplaneSeqId( "R00-M0" );

        ostringstream exp_oss;
        exp_oss << "midplane location=R00-M0 coords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::SoftwareFailure << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqMidplane SET status='A' WHERE posInMachine='R00-M0'" );

        seq_id = getMidplaneSeqId( "R00-M0" );

        ostringstream exp_oss;
        exp_oss << "midplane location=R00-M0 coords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::SoftwareFailure << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}


static bgsched::SequenceId getNodeBoardSeqId( const std::string& location )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqNodeCard where location='" + location + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_node_board )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    {
        prev_seq_id = getNodeBoardSeqId( "R00-M0-N00" );

        conn_ptr->executeUpdate( "UPDATE tbgqNodeCard SET status='E' WHERE midplanePos='R00-M0' AND position='N00'" );

        seq_id = getNodeBoardSeqId( "R00-M0-N00" );

        ostringstream exp_oss;
        exp_oss << "nodeBoard location=R00-M0-N00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNodeCard SET status='S' WHERE midplanePos='R00-M0' AND position='N00'" );

        seq_id = getNodeBoardSeqId( "R00-M0-N00" );

        ostringstream exp_oss;
        exp_oss << "nodeBoard location=R00-M0-N00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNodeCard SET status='M' WHERE midplanePos='R00-M0' AND position='N00'" );

        seq_id = getNodeBoardSeqId( "R00-M0-N00" );

        ostringstream exp_oss;
        exp_oss << "nodeBoard location=R00-M0-N00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNodeCard SET status='A' WHERE midplanePos='R00-M0' AND position='N00'" );

        seq_id = getNodeBoardSeqId( "R00-M0-N00" );

        ostringstream exp_oss;
        exp_oss << "nodeBoard location=R00-M0-N00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }
}


static bgsched::SequenceId getNodeSeqId( const std::string& location )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqNode where location='" + location + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_node )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    {
        prev_seq_id = getNodeSeqId( "R00-M0-N00-J00" );

        conn_ptr->executeUpdate( "UPDATE tbgqNode SET status='F' WHERE midplanePos='R00-M0' AND nodeCardPos='N00' AND position='J00'" );

        seq_id = getNodeSeqId( "R00-M0-N00-J00" );

        ostringstream exp_oss;
        exp_oss << "node location=R00-M0-N00-J00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::SoftwareFailure << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNode SET status='M' WHERE midplanePos='R00-M0' AND nodeCardPos='N00' AND position='J00'" );

        seq_id = getNodeSeqId( "R00-M0-N00-J00" );

        ostringstream exp_oss;
        exp_oss << "node location=R00-M0-N00-J00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::SoftwareFailure << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNode SET status='E' WHERE midplanePos='R00-M0' AND nodeCardPos='N00' AND position='J00'" );

        seq_id = getNodeSeqId( "R00-M0-N00-J00" );

        ostringstream exp_oss;
        exp_oss << "node location=R00-M0-N00-J00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNode SET status='S' WHERE midplanePos='R00-M0' AND nodeCardPos='N00' AND position='J00'" );

        seq_id = getNodeSeqId( "R00-M0-N00-J00" );

        ostringstream exp_oss;
        exp_oss << "node location=R00-M0-N00-J00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqNode SET status='A' WHERE midplanePos='R00-M0' AND nodeCardPos='N00' AND position='J00'" );

        seq_id = getNodeSeqId( "R00-M0-N00-J00" );

        ostringstream exp_oss;
        exp_oss << "node location=R00-M0-N00-J00 midplaneCoords=(0,0,0,0)"
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}


static bgsched::SequenceId getSwitchSeqId( const std::string& switch_id )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqSwitch where switchId='" + switch_id + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_switch )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    {
        prev_seq_id = getSwitchSeqId( "A_R00-M0" );

        conn_ptr->executeUpdate( "UPDATE tbgqSwitch SET status='M' WHERE switchId='A_R00-M0'" );

        seq_id = getSwitchSeqId( "A_R00-M0" );

        ostringstream exp_oss;
        exp_oss << "switch midplaneLocation=R00-M0 midplaneCoords=(0,0,0,0) dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqSwitch SET status='E' WHERE switchId='A_R00-M0'" );

        seq_id = getSwitchSeqId( "A_R00-M0" );

        ostringstream exp_oss;
        exp_oss << "switch midplaneLocation=R00-M0 midplaneCoords=(0,0,0,0) dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqSwitch SET status='S' WHERE switchId='A_R00-M0'" );

        seq_id = getSwitchSeqId( "A_R00-M0" );

        ostringstream exp_oss;
        exp_oss << "switch midplaneLocation=R00-M0 midplaneCoords=(0,0,0,0) dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqSwitch SET status='A' WHERE switchId='A_R00-M0'" );

        seq_id = getSwitchSeqId( "A_R00-M0" );

        ostringstream exp_oss;
        exp_oss << "switch midplaneLocation=R00-M0 midplaneCoords=(0,0,0,0) dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }
}


static bgsched::SequenceId getCableSeqId( const std::string& from_location, const std::string& to_location )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqCable where fromLocation='" + from_location + "' AND toLocation='" + to_location + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_torus_cable )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    string to_location;
    {
        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT toLocation FROM bgqCable where fromLocation='R00-M0-N00-T04'" ));
        rs_ptr->fetch();
        to_location = rs_ptr->columns()["toLocation"].getString();
    }

    {
        prev_seq_id = getCableSeqId( "R00-M0-N00-T04", to_location );

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='M' WHERE fromLocation='R00-M0-N00-T04' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T04", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T04 fromMp=R00-M0 fromMpCoords=(0,0,0,0)"
                << " to=" << to_location << " toMp=R02-M0 toMpCoords=(1,0,0,0)"
                << " dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='E' WHERE fromLocation='R00-M0-N00-T04' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T04", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T04 fromMp=R00-M0 fromMpCoords=(0,0,0,0)"
                << " to=" << to_location << " toMp=R02-M0 toMpCoords=(1,0,0,0)"
                << " dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='S' WHERE fromLocation='R00-M0-N00-T04' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T04", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T04 fromMp=R00-M0 fromMpCoords=(0,0,0,0)"
                << " to=" << to_location << " toMp=R02-M0 toMpCoords=(1,0,0,0)"
                << " dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='A' WHERE fromLocation='R00-M0-N00-T04' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T04", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T04 fromMp=R00-M0 fromMpCoords=(0,0,0,0)"
                << " to=" << to_location << " toMp=R02-M0 toMpCoords=(1,0,0,0)"
                << " dimension=" << bgsched::Dimension::A
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}


BOOST_AUTO_TEST_CASE( test_io_cable )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    string to_location;
    {
        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT toLocation FROM bgqCable where fromLocation='R00-M0-N00-T00'" ));
        rs_ptr->fetch();
        to_location = rs_ptr->columns()["toLocation"].getString();
    }

    {
        prev_seq_id = getCableSeqId( "R00-M0-N00-T00", to_location );

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='M' WHERE fromLocation='R00-M0-N00-T00' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T00", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T00 fromMp=R00-M0 fromMpCoords=(0,0,0,0) to=" << to_location
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='E' WHERE fromLocation='R00-M0-N00-T00' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T00", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T00 fromMp=R00-M0 fromMpCoords=(0,0,0,0) to=" << to_location
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='S' WHERE fromLocation='R00-M0-N00-T00' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T00", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T00 fromMp=R00-M0 fromMpCoords=(0,0,0,0) to=" << to_location
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqCable SET status='A' WHERE fromLocation='R00-M0-N00-T00' AND toLocation='" + to_location + "'" );

        seq_id = getCableSeqId( "R00-M0-N00-T00", to_location );

        ostringstream exp_oss;
        exp_oss << "cable from=R00-M0-N00-T00 fromMp=R00-M0 fromMpCoords=(0,0,0,0) to=" << to_location
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}


BOOST_AUTO_TEST_CASE( test_ras )
{
    bgsched::realtime::RasRecordId rec_id;

    {
        cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT recid FROM NEW TABLE ( INSERT INTO tbgqEventLog ( msg_id, severity ) VALUES ( 'RTTEST', 'INFO' ) )" ));
        rs_ptr->fetch();
        rec_id = rs_ptr->columns()["recid"].as<bgsched::realtime::RasRecordId>();
    }

    ostringstream exp_oss;
    exp_oss << "ras recid=" << rec_id << " msgId=RTTEST severity=" << bgsched::realtime::RasSeverity::INFO << "\n";

    BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
}


static bgsched::SequenceId getIoDrawerSeqId( const std::string& location )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqIoDrawer where location='" + location + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_io_drawer )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    {
        prev_seq_id = getIoDrawerSeqId( "R00-ID" );

        conn_ptr->executeUpdate( "UPDATE tbgqIoDrawer SET status='M' WHERE location='R00-ID'" );

        seq_id = getIoDrawerSeqId( "R00-ID" );

        ostringstream exp_oss;
        exp_oss << "ioDrawer location=R00-ID"
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoDrawer SET status='E' WHERE location='R00-ID'" );

        seq_id = getIoDrawerSeqId( "R00-ID" );

        ostringstream exp_oss;
        exp_oss << "ioDrawer location=R00-ID"
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoDrawer SET status='S' WHERE location='R00-ID'" );

        seq_id = getIoDrawerSeqId( "R00-ID" );

        ostringstream exp_oss;
        exp_oss << "ioDrawer location=R00-ID"
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoDrawer SET status='A' WHERE location='R00-ID'" );

        seq_id = getIoDrawerSeqId( "R00-ID" );

        ostringstream exp_oss;
        exp_oss << "ioDrawer location=R00-ID"
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}


static bgsched::SequenceId getIoNodeSeqId( const std::string& location )
{
    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());
    cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( "SELECT seqId FROM bgqIoNode where location='" + location + "'" ));
    rs_ptr->fetch();
    return rs_ptr->columns()["seqId"].as<bgsched::SequenceId>();
}


BOOST_AUTO_TEST_CASE( test_io_node )
{
    bgsched::SequenceId prev_seq_id;
    bgsched::SequenceId seq_id;

    cxxdb::ConnectionPtr conn_ptr(BGQDB::DBConnectionPool::instance().getConnection());

    {
        prev_seq_id = getIoNodeSeqId( "R00-ID-J00" );

        conn_ptr->executeUpdate( "UPDATE tbgqIoNode SET status='M' WHERE ioPos='R00-ID' AND position='J00'" );

        seq_id = getIoNodeSeqId( "R00-ID-J00" );

        ostringstream exp_oss;
        exp_oss << "ioNode location=R00-ID-J00"
                << " state=" << bgsched::Hardware::Missing << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Available << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoNode SET status='E' WHERE ioPos='R00-ID' AND position='J00'" );

        seq_id = getIoNodeSeqId( "R00-ID-J00" );

        ostringstream exp_oss;
        exp_oss << "ioNode location=R00-ID-J00"
                << " state=" << bgsched::Hardware::Error << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Missing << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoNode SET status='S' WHERE ioPos='R00-ID' AND position='J00'" );

        seq_id = getIoNodeSeqId( "R00-ID-J00" );

        ostringstream exp_oss;
        exp_oss << "ioNode location=R00-ID-J00"
                << " state=" << bgsched::Hardware::Service << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Error << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoNode SET status='F' WHERE ioPos='R00-ID' AND position='J00'" );

        seq_id = getIoNodeSeqId( "R00-ID-J00" );

        ostringstream exp_oss;
        exp_oss << "ioNode location=R00-ID-J00"
                << " state=" << bgsched::Hardware::SoftwareFailure << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::Service << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

    {
        prev_seq_id = seq_id;

        conn_ptr->executeUpdate( "UPDATE tbgqIoNode SET status='A' WHERE ioPos='R00-ID' AND position='J00'" );

        seq_id = getIoNodeSeqId( "R00-ID-J00" );

        ostringstream exp_oss;
        exp_oss << "ioNode location=R00-ID-J00"
                << " state=" << bgsched::Hardware::Available << " seqId=" << seq_id
                << " prevState=" << bgsched::Hardware::SoftwareFailure << " prevSeqId=" << prev_seq_id << "\n";

        BOOST_CHECK_EQUAL( GlobalFixture::waitMessageText(), exp_oss.str() );
    }

}
