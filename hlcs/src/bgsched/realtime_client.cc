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


#include <bgsched/bgsched.h>

#include <bgsched/realtime/Client.h>
#include <bgsched/realtime/ClientConfiguration.h>
#include <bgsched/realtime/ClientEventListenerV2.h>
#include <bgsched/realtime/Filter.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/bind.hpp>
#include <boost/program_options.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>

#include <unistd.h>


using boost::bind;

using std::auto_ptr;
using std::cout;
using std::endl;
using std::invalid_argument;
using std::istringstream;
using std::ostringstream;
using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "realtime.cli" );


class RealtimeClientConfiguration : public bgsched::realtime::ClientConfiguration
{
public:

    RealtimeClientConfiguration();

    void setHost( const Hosts& host )  { _host = host; }

    // override
    const Hosts& getHost() const { return _host; }

private:

    Hosts _host;

};

RealtimeClientConfiguration::RealtimeClientConfiguration()
    : _host(DEFAULT_HOST)
{
    // Nothing to do.
}


class FilterHolder
{
public:

    bgsched::realtime::Filter& get() {
        if ( ! _ptr.get() ) {
            _ptr.reset( new bgsched::realtime::Filter() );
        }
        return *_ptr.get();
    }

    bgsched::realtime::Filter& get() const {
        return *_ptr.get();
    }

    void setAll()  { _ptr.reset( new bgsched::realtime::Filter( bgsched::realtime::Filter::createAll() ) ); }
    void setNone()  { _ptr.reset( new bgsched::realtime::Filter( bgsched::realtime::Filter::createNone() ) ); }

    bool isSet() const  { return bool(_ptr.get()); }

private:

    auto_ptr<bgsched::realtime::Filter> _ptr;

};


static bgsched::realtime::Filter::JobIds parse_job_ids( const string& job_ids_str )
{
    bgsched::realtime::Filter::JobIds ret;

    istringstream iss( job_ids_str );

    while ( ! iss.eof() ) {
        bgsched::Job::Id job_id;
        iss >> job_id;
        if ( ! iss ) {
            ostringstream oss;
            oss << "The job ID argument '" << job_ids_str << "' is not valid.";
            throw invalid_argument( oss.str() );
        }

        ret.insert( job_id );
    }
    return ret;
}


static bgsched::Job::Status parse_job_status( const string& status_str )
{
    if ( (status_str == "Setup") || (status_str == "P") )  return bgsched::Job::Setup;
    if ( (status_str == "Loading") || (status_str == "L") )  return bgsched::Job::Loading;
    if ( (status_str == "Starting") || (status_str == "S") )  return bgsched::Job::Starting;
    if ( (status_str == "Running") || (status_str == "R") )  return bgsched::Job::Running;
    if ( (status_str == "Debug") || (status_str == "D") )  return bgsched::Job::Debug;
    if ( (status_str == "Cleanup") || (status_str == "N") )  return bgsched::Job::Cleanup;
    if ( (status_str == "Terminated") || (status_str == "T") )  return bgsched::Job::Terminated;
    if ( (status_str == "Error") || (status_str == "E") )  return bgsched::Job::Error;

    ostringstream oss;
    oss << "The job status '" << status_str << "' is not valid.";
    throw invalid_argument( oss.str() );
}


static bgsched::realtime::Filter::JobStatuses parse_job_statuses( const string& job_statuses )
{
    bgsched::realtime::Filter::JobStatuses ret;

    istringstream iss( job_statuses );

    while ( ! iss.eof() ) {
        string job_status_str;
        iss >> job_status_str;

        bgsched::Job::Status status(parse_job_status( job_status_str ));
        ret.insert( status );
    }

    return ret;
}


static bgsched::Block::Status parse_block_status( const string& status_str )
{
    if ( (status_str == "Free")        || (status_str == "F") )  return bgsched::Block::Free;
    if ( (status_str == "Allocated")   || (status_str == "A") )  return bgsched::Block::Allocated;
    if ( (status_str == "Booting")     || (status_str == "B") )  return bgsched::Block::Booting;
    if ( (status_str == "Initialized") || (status_str == "I") )  return bgsched::Block::Initialized;
    if ( (status_str == "Terminating") || (status_str == "T") )  return bgsched::Block::Terminating;

    ostringstream oss;
    oss << "The block status '" << status_str << "' is not valid.";
    throw invalid_argument( oss.str() );
}


static bgsched::realtime::Filter::BlockStatuses parse_block_statuses( const string& block_statuses )
{
    bgsched::realtime::Filter::BlockStatuses ret;

    istringstream iss( block_statuses );

    while ( ! iss.eof() ) {
        string block_status_str;
        iss >> block_status_str;

        bgsched::Block::Status status(parse_block_status( block_status_str ));
        ret.insert( status );
    }

    return ret;
}


static bgsched::realtime::RasSeverity::Value parse_ras_severity( const string& ras_severity_str )
{
    if ( (ras_severity_str == "INFO")    || (ras_severity_str == "I") )  return bgsched::realtime::RasSeverity::INFO;
    if ( (ras_severity_str == "WARN") || (ras_severity_str == "W") )  return bgsched::realtime::RasSeverity::WARN;
    if ( (ras_severity_str == "FATAL")   || (ras_severity_str == "F") )  return bgsched::realtime::RasSeverity::FATAL;
    if ( (ras_severity_str == "UNKNOWN")   || (ras_severity_str == "U") )  return bgsched::realtime::RasSeverity::UNKNOWN;

    ostringstream oss;
    oss << "The RAS severity '" << ras_severity_str << "' is not valid.";
    throw invalid_argument( oss.str() );
}


static bgsched::realtime::Filter::RasSeverities parse_ras_severities( const string& ras_severities_str )
{
    bgsched::realtime::Filter::RasSeverities ret;

    istringstream iss( ras_severities_str );

    while ( ! iss.eof() ) {
        string ras_severity_str;
        iss >> ras_severity_str;

        bgsched::realtime::RasSeverity::Value ras_severity(parse_ras_severity( ras_severity_str ));
        ret.insert( ras_severity );
    }
    return ret;
}


static std::ostream& operator<<( std::ostream& os, bgsched::Block::Status status )
{
    os << (status == bgsched::Block::Allocated ? "Allocated" :
           status == bgsched::Block::Booting ? "Booting" :
           status == bgsched::Block::Free ? "Free" :
           status == bgsched::Block::Initialized ? "Initialized" :
           status == bgsched::Block::Terminating ? "Terminating" :
           "Unexpected");
    return os;
}


static std::ostream& operator<<( std::ostream& os, bgsched::Job::Status status )
{
    os << (status == bgsched::Job::Setup ? "Setup" :
           status == bgsched::Job::Loading ? "Loading" :
           status == bgsched::Job::Starting ? "Starting" :
           status == bgsched::Job::Running ? "Running" :
           status == bgsched::Job::Debug ? "Debug" :
           status == bgsched::Job::Cleanup ? "Cleanup" :
           status == bgsched::Job::Terminated ? "Terminated" :
           status == bgsched::Job::Error ? "Error" :
           "Unexpected");
    return os;
}


static std::ostream& operator<<( std::ostream& os, bgsched::Hardware::State state )
{
    os << (state == bgsched::Hardware::Available ? "Available" :
           state == bgsched::Hardware::Missing ? "Missing" :
           state == bgsched::Hardware::Error ? "Error" :
           state == bgsched::Hardware::Service ? "Service" :
           state == bgsched::Hardware::SoftwareFailure ? "SoftwareFailure" :
           "Unexpected");
    return os;
}


static std::ostream& operator<<( std::ostream& os, bgsched::Dimension::Value dimension )
{
    os << (dimension == bgsched::Dimension::A ? "A" :
           dimension == bgsched::Dimension::B ? "B" :
           dimension == bgsched::Dimension::C ? "C" :
           dimension == bgsched::Dimension::D ? "D" :
           dimension == bgsched::Dimension::E ? "E" :
           "Unexpected");
    return os;
}


static std::ostream& operator<<( std::ostream& os, const bgsched::Coordinates& coords )
{
    os << "(" << coords[bgsched::Dimension::A] << "," << coords[bgsched::Dimension::B] << "," << coords[bgsched::Dimension::C] << "," << coords[bgsched::Dimension::D] << ")";
    return os;
}


static std::ostream& operator<<( std::ostream& os, bgsched::realtime::RasSeverity::Value severity )
{
    os << (severity == bgsched::realtime::RasSeverity::INFO ? "INFO" :
           severity == bgsched::realtime::RasSeverity::WARN ? "WARN" :
           severity == bgsched::realtime::RasSeverity::FATAL ? "FATAL" :
           severity == bgsched::realtime::RasSeverity::UNKNOWN ? "UNKNOWN" :
           "Unexpected"
          );
    return os;
}


class MyListener : public bgsched::realtime::ClientEventListenerV2
{
public:

    MyListener(
            unsigned max_events,
            bool fetch_ras_message
        ) :
            _max_events(max_events),
            _total_events(0),
            _fetch_ras_message(fetch_ras_message)
    { /* Nothing to do */ }

    bool getRealtimeContinue()  { return (_max_events == 0 || _total_events <= _max_events); }

    void handleRealtimeStartedRealtimeEvent( const RealtimeStartedEventInfo& info )
    { cout << "Real-time started. filterId=" << info.getFilterId() << "\n"; ++_total_events; }

    void handleRealtimeEndedRealtimeEvent( const RealtimeEndedEventInfo& /*info*/ )
    { cout << "Real-time ended.\n"; ++_total_events; }

    void handleBlockAddedRealtimeEvent( const BlockAddedEventInfo& info )
    { cout << "Block added. name=" << info.getBlockName() << " status=" << info.getStatus() << " sequenceId=" << info.getSequenceId() << "\n"; ++_total_events; }

    void handleBlockStateChangedRealtimeEvent( const BlockStateChangedEventInfo& info )
    { cout << "Block state changed. name=" << info.getBlockName() << " status=" << info.getStatus() << " sequenceId=" << info.getSequenceId() <<
        " previousStatus=" << info.getPreviousStatus() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n"; ++_total_events;
    }

    void handleBlockDeletedRealtimeEvent( const BlockDeletedEventInfo& info )
    { cout << "Block deleted. name=" << info.getBlockName() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n"; ++_total_events; }


    void handleJobAddedRealtimeEvent( const JobAddedEventInfo& info )
    { cout << "Job added. id=" << info.getJobId() << " blockName=" << info.getComputeBlockName() << " status=" << info.getStatus() << " sequenceId=" << info.getSequenceId() << "\n"; ++_total_events; }

    void handleJobStateChangedRealtimeEvent( const JobStateChangedEventInfo& info )
    { cout << "Job state changed. id=" << info.getJobId() << " blockName=" << info.getComputeBlockName() << " status=" << info.getStatus() << " sequenceId=" << info.getSequenceId() <<
        " previousStatus=" << info.getPreviousStatus() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
      ++_total_events;
    }

    void handleJobDeletedRealtimeEvent( const JobDeletedEventInfo& info )
    { cout << "Job deleted. id=" << info.getJobId() << " blockName=" << info.getComputeBlockName() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n"; ++_total_events; }

    void handleMidplaneStateChangedRealtimeEvent( const MidplaneStateChangedEventInfo& info )
    { cout << "Midplane. location=" << info.getLocation() << " @" << info.getMidplaneCoordinates() <<
                       " state=" << info.getState() << " sequenceId=" << info.getSequenceId() <<
                       " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
      ++_total_events;
    }

    void handleNodeBoardStateChangedRealtimeEvent( const NodeBoardStateChangedEventInfo& info )
    { cout << "Node board. location=" << info.getLocation() << " midplane@" << info.getMidplaneCoordinates() <<
                         " state=" << info.getState() << " sequenceId=" << info.getSequenceId() <<
                         " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
       ++_total_events;
    }

    void handleNodeStateChangedRealtimeEvent( const NodeStateChangedEventInfo& info )
    { cout << "Node. location=" << info.getLocation() << " midplane@" << info.getMidplaneCoordinates() <<
                   " state=" << info.getState() << " sequenceId=" << info.getSequenceId() <<
                   " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
       ++_total_events;
    }

    void handleSwitchStateChangedRealtimeEvent( const SwitchStateChangedEventInfo& info )
    { cout << "Switch. midplane=" << info.getMidplaneLocation() << " @" << info.getMidplaneCoordinates() << " dimension=" << info.getDimension()
         << " state=" << info.getState() << " sequenceId=" << info.getSequenceId() << " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
      ++_total_events;
    }

    void handleTorusCableStateChangedRealtimeEvent( const TorusCableStateChangedEventInfo& info )
    { cout << "Torus cable. from=" << info.getFromLocation() << " fromMidplane=" << info.getFromMidplaneLocation() << "@" << info.getFromMidplaneCoordinates() <<
                          " to=" << info.getToLocation() << " toMidplane=" << info.getToMidplaneLocation() << "@" << info.getFromMidplaneCoordinates() <<
                          " dim=" << info.getDimension()
            << " state=" << info.getState() << " sequenceId=" << info.getSequenceId() << " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
        ++_total_events;
    }

    void handleIoCableStateChangedRealtimeEvent( const IoCableStateChangedEventInfo& info )
    { cout << "I/O cable. from=" << info.getFromLocation() << " fromMidplane=" << info.getFromMidplaneLocation() << "@" << info.getFromMidplaneCoordinates() <<
                        " to=" << info.getToLocation()
            << " state=" << info.getState() << " sequenceId=" << info.getSequenceId() << " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
        ++_total_events;
    }

    void handleRasRealtimeEvent( const RasEventInfo& info )
    {
        cout << "Ras. recordId=" << info.getRecordId() << " messageId=" << info.getMessageId() << " severity=" << info.getSeverity();
        if ( _fetch_ras_message ) {
            string message(_fetchRasMessage( info.getRecordId() ));
            if ( ! message.empty() ) {
                cout << " message='" << message << "'";
            }
        }
        cout << "\n";
        ++_total_events;
    }

    void handleIoDrawerStateChangedEvent( const IoDrawerStateChangedEventInfo& info )
    {
        cout << "I/O drawer. location=" << info.getLocation() <<
                   " state=" << info.getState() << " sequenceId=" << info.getSequenceId() <<
                   " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
        ++_total_events;
    }

    void handleIoNodeStateChangedEvent( const IoNodeStateChangedEventInfo& info )
    {
        cout << "I/O node. location=" << info.getLocation() <<
                   " state=" << info.getState() << " sequenceId=" << info.getSequenceId() <<
                   " previousState=" << info.getPreviousState() << " previousSequenceId=" << info.getPreviousSequenceId() << "\n";
       ++_total_events;
    }


private:

    unsigned _max_events;
    unsigned _total_events;
    bool _fetch_ras_message;

    cxxdb::ConnectionPtr _conn_ptr;
    cxxdb::QueryStatementPtr _stmt_ptr;


    void _initStatementPtr()
    {
        if ( _stmt_ptr )  return;

        _conn_ptr = BGQDB::DBConnectionPool::Instance().getConnection();

        cxxdb::ParameterNames param_names;
        param_names.push_back( "recid" );

        _stmt_ptr = _conn_ptr->prepareQuery( "SELECT message FROM bgqEventLog WHERE recid=?", param_names );
    }


    string _fetchRasMessage( bgsched::realtime::RasRecordId record_id )
    {
        _initStatementPtr();

        _stmt_ptr->parameters()["recid"].cast( record_id );

        cxxdb::ResultSetPtr rs_ptr(_stmt_ptr->execute());

        if ( rs_ptr->fetch() ) {
            return rs_ptr->columns()["message"].getString();
        }

        return string();
    }
};


static void realtime_client(
        const bgsched::realtime::ClientConfiguration& client_configuration,
        const FilterHolder& filter_holder,
        unsigned max_events,
        bool fetch_ras_message,
        bool cont
    )
{
    LOG_DEBUG_MSG( "Creating real-time client..." );

    bgsched::realtime::Client c( client_configuration );

    MyListener my_listener(
            max_events,
            fetch_ras_message
        );

    c.addListener( my_listener );

    while ( true ) {

        try {

            LOG_DEBUG_MSG( "Connecting real-time client..." );

            c.connect();

            if ( filter_holder.isSet() ) {
                LOG_DEBUG_MSG( "Setting the filter on the real-time client..." );

                bgsched::realtime::Filter::Id filter_id;

                c.setFilter( filter_holder.get(), &filter_id, NULL );

                cout << "Filter ID is " << filter_id << endl;
            }

            LOG_DEBUG_MSG( "Requesting updates on the real-time client..." );

            c.requestUpdates( NULL );

            LOG_DEBUG_MSG( "Receiving messages on the real-time client..." );

            bool end(false);

            c.receiveMessages( NULL, NULL, &end );

            cout << "Client disconnected from realtime\n";

            LOG_INFO_MSG( "returned from receiveMessages. end=" << end );

        } catch ( std::exception& e ) {

            if ( ! cont ) {
                throw;
            }

            cout << "Caught exception, will continue. Exception is " << e.what() << "\n";
        }

        if ( cont ) {
            sleep( 1 );
        } else {
            break;
        }
    }
}


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {
        RealtimeClientConfiguration realtime_client_configuration;

        unsigned max_events(0);
        bool cont(false);

        FilterHolder filter_holder;

        string filter_str;

        bool help_ind(false);

        bool jobs_ind(false);
        string job_block;
        string job_statuses_str;
        bool job_deleted_ind(false);

        bool blocks_ind(false);
        string block_id;
        string block_statuses_str;
        bool block_deleted_ind(false);

        bool midplanes_ind(false);
        bool node_boards_ind(false);
        bool nodes_ind(false);
        bool switches_ind(false);
        bool torus_cables_ind(false);
        bool io_cables_ind(false);

        bool ras_ind(false);
        string ras_msg_id;
        string ras_severities_str;
        string ras_job_ids_str;
        string ras_block;

        bool fetch_ras_message_ind(false);

        bool io_drawers_ind(false);
        bool io_nodes_ind(false);


        po::options_description options( "Options" );

        bgq::utility::Properties::ProgramOptions properties_program_options;

        properties_program_options.addTo( options );

        bgq::utility::LoggingProgramOptions logging_program_options( "ibm.realtime" );

        logging_program_options.addTo( options );

        options.add_options()
                ( "host",
                  po::value<RealtimeClientConfiguration::Hosts>()->notifier( boost::bind( &RealtimeClientConfiguration::setHost, &realtime_client_configuration, _1 ) ),
                  "Real-time server host" )
                ( "help,h", po::bool_switch( &help_ind ), "print help text" )
                ( "max-events", po::value( &max_events ), "events to receive before exiting" )
                ( "continue", po::bool_switch( &cont ), "reconnect after disconnected" )
            ;

        po::options_description filter_options( "Filtering" );

        filter_options.add_options()
                ( "filter", po::value( &filter_str )->default_value( "default" ), "Initial filter, one of default, all, or none" )
                ( "jobs", po::bool_switch( &jobs_ind ), "Toggle job changes" )
                ( "job-block", po::value( &job_block ), "Job block pattern" )
                ( "job-status", po::value( &job_statuses_str ), "Job statuses" )
                ( "job-deleted", po::bool_switch( &job_deleted_ind ), "Toggle job deletions" )
                ( "blocks", po::bool_switch( &blocks_ind ), "Toggle block changes" )
                ( "block-id", po::value( &block_id ), "Block ID pattern" )
                ( "block-status", po::value( &block_statuses_str ), "Block statuses" )
                ( "block-deleted", po::bool_switch( &block_deleted_ind ), "Toggle block deletions" )
                ( "midplanes", po::bool_switch( &midplanes_ind ), "Toggle midplane changes" )
                ( "nodeboards", po::bool_switch( &node_boards_ind ), "Toggle node board changes" )
                ( "nodes", po::bool_switch( &nodes_ind ), "Toggle node changes" )
                ( "io-drawers", po::bool_switch( &io_drawers_ind ), "Toggle I/O drawer changes" )
                ( "io-nodes", po::bool_switch( &io_nodes_ind ), "Toggle I/O node changes" )
                ( "switches", po::bool_switch( &switches_ind ), "Toggle switch changes" )
                ( "torus-cables", po::bool_switch( &torus_cables_ind ), "Toggle torus cable changes" )
                ( "io-cables", po::bool_switch( &io_cables_ind ), "Toggle I/O cable changes" )
                ( "ras-events", po::bool_switch( &ras_ind ), "Toggle RAS events" )
                ( "ras-msg-id", po::value( &ras_msg_id ), "RAS message ID pattern" )
                ( "ras-severities", po::value( &ras_severities_str ), "RAS severities" )
                ( "ras-job-ids", po::value( &ras_job_ids_str ), "RAS job IDS" )
                ( "ras-block", po::value( &ras_block ), "RAS block ID pattern" )
                ( "fetch-ras-message", po::bool_switch( &fetch_ras_message_ind ), "Fetch RAS message" )
            ;

        options.add( filter_options );

        po::variables_map vm;

        try {
            po::store(po::parse_command_line( argc, argv, options ), vm);

            po::notify( vm );

            if ( help_ind ) {
                cout << "Usage: " << argv[0] << " [OPTION]...\n"
                        "Real-time client for Blue Gene/Q.\n"
                        "\n"
                     << options << "\n";
                return 0;
            }
        } catch ( std::exception& e ) {
            cout << argv[0] << ": failed parsing arguments, " << e.what() << ".\n"
                    "Try `" << argv[0] << " --help' for more information.\n";
            return 1;
        }

        bgsched::init( properties_program_options.getFilename() );
        logging_program_options.apply();

        if ( filter_str == "all" ) {
            filter_holder.setAll();
        } else if ( filter_str == "none" ) {
            filter_holder.setNone();
        }

        if ( jobs_ind )  filter_holder.get().setJobs( ! filter_holder.get().getJobs() );
        if ( job_block != string() ) {
            filter_holder.get().setJobs( true );
            filter_holder.get().setJobBlockIdPattern( &job_block );
        }

        if ( job_statuses_str != string() ) {
            filter_holder.get().setJobs( true );

            bgsched::realtime::Filter::JobStatuses job_statuses(parse_job_statuses( job_statuses_str ));
            filter_holder.get().setJobStatuses( &job_statuses );
        }

        if ( job_deleted_ind ) {
            filter_holder.get().setJobs( true );
            filter_holder.get().setJobDeleted( ! filter_holder.get().getJobDeleted() );
        }

        if ( blocks_ind )  filter_holder.get().setBlocks( ! filter_holder.get().getBlocks() );
        if ( block_id != string() ) {
            filter_holder.get().setBlocks( true );
            filter_holder.get().setBlockIdPattern( &block_id );
        }

        if ( block_statuses_str != string() ) {
            filter_holder.get().setBlocks( true );

            bgsched::realtime::Filter::BlockStatuses block_statuses(parse_block_statuses( block_statuses_str ));
            filter_holder.get().setBlockStatuses( &block_statuses );
        }

        if ( block_deleted_ind ) {
            filter_holder.get().setBlocks( true );
            filter_holder.get().setBlockDeleted( ! filter_holder.get().getBlockDeleted() );
        }

        if ( midplanes_ind ) {
            filter_holder.get().setMidplanes( ! filter_holder.get().getMidplanes() );
        }

        if ( node_boards_ind ) {
            filter_holder.get().setNodeBoards( ! filter_holder.get().getNodeBoards() );
        }

        if ( nodes_ind ) {
            filter_holder.get().setNodes( ! filter_holder.get().getNodes() );
        }

        if ( io_drawers_ind ) {
            filter_holder.get().setIoDrawers( ! filter_holder.get().getIoDrawers() );
        }

        if ( io_nodes_ind ) {
            filter_holder.get().setIoNodes( ! filter_holder.get().getIoNodes() );
        }

        if ( switches_ind ) {
            filter_holder.get().setSwitches( ! filter_holder.get().getSwitches() );
        }

        if ( torus_cables_ind ) {
            filter_holder.get().setTorusCables( ! filter_holder.get().getTorusCables() );
        }

        if ( io_cables_ind ) {
            filter_holder.get().setIoCables( ! filter_holder.get().getIoCables() );
        }

        if ( ras_ind ) {
            filter_holder.get().setRasEvents( ! filter_holder.get().getRasEvents() );
        }

        if ( ras_msg_id != string() ) {
            filter_holder.get().setRasEvents( true );
            filter_holder.get().setRasMessageIdPattern( &ras_msg_id );
        }

        if ( ras_severities_str != string() ) {
            filter_holder.get().setRasEvents( true );

            bgsched::realtime::Filter::RasSeverities ras_severities(parse_ras_severities( ras_severities_str ));
            filter_holder.get().setRasSeverities( &ras_severities );
        }

        if ( ras_job_ids_str != string() ) {
            filter_holder.get().setRasEvents( true );

            bgsched::realtime::Filter::JobIds job_ids(parse_job_ids( ras_job_ids_str ));
            filter_holder.get().setRasJobIds( &job_ids );
        }

        if ( ras_block != string() ) {
            filter_holder.get().setRasEvents( true );
            filter_holder.get().setRasComputeBlockIdPattern( &ras_block );
        }

        realtime_client(
                realtime_client_configuration,
                filter_holder,
                max_events,
                fetch_ras_message_ind,
                cont
            );

    } catch ( std::exception& rte ) {
        cout << argv[0] << ": failed, " << rte.what() << "\n";
        exit( 1 );
    } catch ( ... ) {
        cout << argv[0] << ": caught unexpected exception." << "\n";
        exit( 1 );
    }
}
