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

#ifndef BGSCHED_REALTIME_FILTER_IMPL_H_
#define BGSCHED_REALTIME_FILTER_IMPL_H_


#include <bgsched/realtime/Filter.h>

#include <boost/shared_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/string.hpp>

#include <boost/xpressive/xpressive_fwd.hpp>

#include <iosfwd>
#include <set>
#include <string>


namespace bgsched {
namespace realtime {


class Filter::Impl
{
public:

    typedef boost::shared_ptr<Filter::BlockStatuses> BlockStatusesPtr;
    typedef boost::shared_ptr<Filter::JobStatuses> JobStatusesPtr;
    typedef boost::shared_ptr<RasSeverities> RasSeveritiesPtr;
    typedef boost::shared_ptr<JobIds> JobIdsPtr;

    typedef boost::shared_ptr<boost::xpressive::sregex> RePtr;


    static const Impl NONE;
    static const Impl ALL;
    static const Impl DEFAULT;


    static RePtr compilePattern( const std::string& pattern );


    Impl();


    void setJobs( bool jobs )  { _jobs = jobs; }
    bool getJobs() const  { return _jobs; }

    void setJobBlockIdPattern( const std::string* job_block_id_pattern_p )
    { _checkAndSetPattern( job_block_id_pattern_p, "job block ID", &_job_block_id_pattern ); }

    const std::string& getJobBlockIdPattern() const
    { return _job_block_id_pattern; }

    void setJobStatuses( const Filter::JobStatuses* job_statuses_p )
    { _job_statuses_ptr.reset( job_statuses_p ? new Filter::JobStatuses(*job_statuses_p) : NULL ); }

    const JobStatusesPtr& getJobStatusesPtr() const { return _job_statuses_ptr; }

    void setJobDeleted( bool job_deleted )  { _job_deleted = job_deleted; }
    bool getJobDeleted() const  { return _job_deleted; }

    void setBlocks( bool blocks )  { _blocks = blocks; }
    bool getBlocks() const  { return _blocks; }

    void setBlockIdPattern( const std::string* block_id_pattern_p )
    { _checkAndSetPattern( block_id_pattern_p, "block ID", &_block_id_pattern ); }

    const std::string& getBlockIdPattern() const
    { return _block_id_pattern; }

    void setBlockStatuses( const Filter::BlockStatuses* block_statuses_p )
    { _block_statuses_ptr.reset( block_statuses_p ? new Filter::BlockStatuses(*block_statuses_p) : NULL ); }

    const BlockStatusesPtr& getBlockStatusesPtr() const
    { return _block_statuses_ptr; }

    void setBlockDeleted( bool block_deleted )  { _block_deleted = block_deleted; }
    bool getBlockDeleted() const  { return _block_deleted; }

    void setMidplanes( bool midplanes )  { _midplanes = midplanes; }
    bool getMidplanes() const  { return _midplanes; }

    void setNodeBoards( bool node_boards )  { _node_boards = node_boards; }
    bool getNodeBoards() const  { return _node_boards; }

    void setNodes( bool nodes )  { _nodes = nodes; }
    bool getNodes() const  { return _nodes; }

    void setSwitches( bool switches )  { _switches = switches; }
    bool getSwitches() const  { return _switches; }

    void setTorusCables( bool torus_cables )  { _torus_cables = torus_cables; }
    bool getTorusCables() const  { return _torus_cables; }

    void setIoCables( bool io_cables )  { _io_cables = io_cables; }
    bool getIoCables() const  { return _io_cables; }

    void setRasEvents( bool ras_events )  { _ras_events = ras_events; }
    bool getRasEvents() const  { return _ras_events; }

    void setRasMessageIdPattern( const std::string* ras_message_id_pattern_p )
    { _checkAndSetPattern( ras_message_id_pattern_p, "RAS message ID", &_ras_message_id_pattern ); }

    const std::string& getRasMessageIdPattern() const
    { return _ras_message_id_pattern; }

    void setRasSeverities( const RasSeverities* ras_severities_p )
    { _ras_severities_ptr.reset( ras_severities_p ? new RasSeverities(*ras_severities_p) : NULL ); }

    const RasSeveritiesPtr& getRasSeveritiesPtr() const  { return _ras_severities_ptr; }

    void setRasJobIds( const JobIds* ras_job_ids_p )
    { _ras_job_ids_ptr.reset( ras_job_ids_p ? new JobIds(*ras_job_ids_p) : NULL ); }

    const JobIdsPtr& getRasJobIdsPtr() const  { return _ras_job_ids_ptr; }

    void setRasComputeBlockIdPattern( const std::string* ras_compute_block_id_pattern_p )
    { _checkAndSetPattern( ras_compute_block_id_pattern_p, "RAS compute block ID", &_ras_compute_block_id_pattern ); }

    const std::string& getRasComputeBlockIdPattern() const
    { return _ras_compute_block_id_pattern; }

    void setIoDrawers( bool io_drawers )  { _io_drawers = io_drawers; }
    bool getIoDrawers() const  { return _io_drawers; }

    void setIoNodes( bool io_nodes )  { _io_nodes = io_nodes; }
    bool getIoNodes() const  { return _io_nodes; }


private:

    static Impl _createAll();
    static Impl _createDefault();

    static void _checkAndSetPattern(
            const std::string* pattern_str_p,
            const std::string& pattern_name,
            std::string* s_out
        );


    bool _jobs;
    std::string _job_block_id_pattern;
    JobStatusesPtr _job_statuses_ptr;
    bool _job_deleted;

    bool _blocks;
    std::string _block_id_pattern;
    BlockStatusesPtr _block_statuses_ptr;
    bool _block_deleted;

    bool _midplanes, _node_boards, _nodes, _switches, _torus_cables, _io_cables, _io_drawers, _io_nodes;

    bool _ras_events;
    std::string _ras_message_id_pattern;
    RasSeveritiesPtr _ras_severities_ptr;
    JobIdsPtr _ras_job_ids_ptr;
    std::string _ras_compute_block_id_pattern;


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int /*version*/ )
    {
        ar & _jobs;
        ar & _job_block_id_pattern;
        ar & _job_statuses_ptr;
        ar & _job_deleted;

        ar & _blocks;
        ar & _block_id_pattern;
        ar & _block_statuses_ptr;
        ar & _block_deleted;

        ar & _midplanes;
        ar & _node_boards;
        ar & _nodes;
        ar & _switches;
        ar & _torus_cables;
        ar & _io_cables;
        ar & _io_drawers;
        ar & _io_nodes;

        ar & _ras_events;
        ar & _ras_message_id_pattern;
        ar & _ras_severities_ptr;
        ar & _ras_job_ids_ptr;
        ar & _ras_compute_block_id_pattern;
    }
};


std::ostream& operator<<( std::ostream& os, const Filter::Impl& filter );


} // namespace bgsched::realtime
} // namespace bgsched

#endif
