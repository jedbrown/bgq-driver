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


#include "ClientFilterVisitor.h"

#include <boost/xpressive/xpressive.hpp>

#include <string>


using bgsched::realtime::Filter;

using std::string;


namespace realtime {
namespace server {


ClientFilterVisitor::ClientFilterVisitor(
        const bgsched::realtime::Filter::Impl& filter
    ) :
        _send_jobs(filter.getJobs()),
        _send_job_delete(true),
        _send_blocks(filter.getBlocks()),
        _send_block_delete(true),
        _send_midplane_changes(filter.getMidplanes()),
        _send_node_board_changes(filter.getNodeBoards()),
        _send_node_changes(filter.getNodes()),
        _send_switch_changes(filter.getSwitches()),
        _send_torus_cable_changes(filter.getTorusCables()),
        _send_io_cable_changes(filter.getIoCables()),
        _send_io_drawer_changes(filter.getIoDrawers()),
        _send_io_node_changes(filter.getIoNodes()),
        _send_ras_events(filter.getRasEvents()),
        _res(false)
{
    if ( _send_jobs ) {
        _job_block_re_ptr = Filter::Impl::compilePattern( filter.getJobBlockIdPattern() );
        _job_statuses_ptr = filter.getJobStatusesPtr();
        _send_job_delete = filter.getJobDeleted();
    }

    if ( _send_blocks ) {
        _block_id_re_ptr = Filter::Impl::compilePattern( filter.getBlockIdPattern() );
        _block_statuses_ptr = filter.getBlockStatusesPtr();
        _send_block_delete = filter.getBlockDeleted();
    }

    if ( _send_ras_events ) {
        _ras_event_msg_id_re_ptr = Filter::Impl::compilePattern( filter.getRasMessageIdPattern() );
        _ras_block_id_re_ptr = Filter::Impl::compilePattern( filter.getRasComputeBlockIdPattern() );
        _ras_severities_ptr = filter.getRasSeveritiesPtr();
        _ras_job_ids_ptr = filter.getRasJobIdsPtr();
    }
}


bool ClientFilterVisitor::_checkBlockAdded( bgsched::realtime::ClientEventListener::BlockAddedEventInfo::Impl& ei )
{
    if ( ! _send_blocks ) {
        return false;
    }
    if ( ! _checkBlockStatesFilter( ei.getStatus() ) ) {
        return false;
    }
    if ( _block_id_re_ptr && ! boost::xpressive::regex_match( ei.getBlockName(), *_block_id_re_ptr ) ) {
        return false;
    }
    return true;
}


bool ClientFilterVisitor::_checkBlockStateChanged( bgsched::realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& ei )
{
    if ( ! _send_blocks ) {
        return false;
    }
    if ( ! _checkBlockStatesFilter( ei.getStatus() ) ) {
        return false;
    }
    if ( _block_id_re_ptr && ! boost::xpressive::regex_match( ei.getBlockName(), *_block_id_re_ptr ) ) {
        return false;
    }
    return true;
}


bool ClientFilterVisitor::_checkBlockDeleted( bgsched::realtime::ClientEventListener::BlockDeletedEventInfo::Impl& ei )
{
    if ( ! _send_blocks ) {
        return false;
    }
    if ( ! _send_block_delete ) {
        return false;
    }
    if ( _block_id_re_ptr && ! boost::xpressive::regex_match( ei.getBlockName(), *_block_id_re_ptr ) ) {
        return false;
    }
    return true;
}


bool ClientFilterVisitor::_checkJobAdded( bgsched::realtime::ClientEventListener::JobAddedEventInfo::Impl& ei )
{
    if ( ! _send_jobs ) {
        return false;
    }
    if ( ! _checkJobStatesFilter( ei.getStatus() ) ) {
        return false;
    }
    if ( _job_block_re_ptr && ! boost::xpressive::regex_match( ei.getComputeBlockName(), *_job_block_re_ptr ) ) {
        return false;
    }
    return true;
}


bool ClientFilterVisitor::_checkJobStateChanged( bgsched::realtime::ClientEventListener::JobStateChangedEventInfo::Impl& ei )
{
    if ( ! _send_jobs ) {
        return false;
    }
    if ( ! _checkJobStatesFilter( ei.getStatus() ) ) {
        return false;
    }
    if ( _job_block_re_ptr && ! boost::xpressive::regex_match( ei.getComputeBlockName(), *_job_block_re_ptr ) ) {
        return false;
    }
    return true;
}


bool ClientFilterVisitor::_checkJobDeleted( bgsched::realtime::ClientEventListener::JobDeletedEventInfo::Impl& ei )
{
    if ( ! _send_jobs ) {
        return false;
    }
    if ( ! _send_job_delete ) {
        return false;
    }
    if ( _job_block_re_ptr && ! boost::xpressive::regex_match( ei.getComputeBlockName(), *_job_block_re_ptr ) ) {
        return false;
    }
    return true;
}


bool ClientFilterVisitor::_checkMidplaneStateChanged( bgsched::realtime::ClientEventListener::MidplaneStateChangedEventInfo::Impl& )
{
    if ( ! _send_midplane_changes )  return false;
    return true;
}


bool ClientFilterVisitor::_checkNodeBoardStateChanged( bgsched::realtime::ClientEventListener::NodeBoardStateChangedEventInfo::Impl& )
{
    if ( ! _send_node_board_changes )  return false;
    return true;
}


bool ClientFilterVisitor::_checkNodeStateChanged( bgsched::realtime::ClientEventListener::NodeStateChangedEventInfo::Impl& )
{
    if ( ! _send_node_changes )  return false;
    return true;
}


bool ClientFilterVisitor::_checkSwitchStateChanged( bgsched::realtime::ClientEventListener::SwitchStateChangedEventInfo::Impl& )
{
    if ( ! _send_switch_changes )  return false;
    return true;
}


bool ClientFilterVisitor::_checkTorusCableStateChanged( bgsched::realtime::ClientEventListener::TorusCableStateChangedEventInfo::Impl& )
{
    if ( ! _send_torus_cable_changes )  return false;
    return true;
}


bool ClientFilterVisitor::_checkIoCableStateChanged( bgsched::realtime::ClientEventListener::IoCableStateChangedEventInfo::Impl& )
{
    if ( ! _send_io_cable_changes )  return false;
    return true;
}


bool ClientFilterVisitor::_checkRasEvent( bgsched::realtime::ClientEventListener::RasEventInfo::Impl& ei )
{
    if ( ! _send_ras_events ) {
        return false;
    }

    if ( _ras_event_msg_id_re_ptr && ! boost::xpressive::regex_match( ei.getMessageId(), *_ras_event_msg_id_re_ptr ) ) {
        return false;
    }

    if ( _ras_block_id_re_ptr ) {
        const string &block_id(ei.getBlockId());

        if ( block_id == string() ) {
            return false;
        }
        if ( ! boost::xpressive::regex_match( block_id, *_ras_block_id_re_ptr ) ) {
            return false;
        }
    }
    if ( _ras_severities_ptr ) {
        if ( _ras_severities_ptr->find( ei.getSeverity() ) == _ras_severities_ptr->end() ) {
            return false;
        }
    }
    if ( _ras_job_ids_ptr ) {
        bgsched::Job::Id job_id(ei.getJobId());
        if ( job_id == bgsched::Job::Id(-1) ) {
            return false;
        }
        if ( _ras_job_ids_ptr->find( job_id ) == _ras_job_ids_ptr->end() ) {
            return false;
        }
    }
    return true;
}


bool ClientFilterVisitor::_checkJobStatesFilter( bgsched::Job::Status job_status )
{
    if ( ! _job_statuses_ptr ) {
        return true;
    }

    bool ret(_job_statuses_ptr->find( job_status ) != _job_statuses_ptr->end());
    return ret;
}


bool ClientFilterVisitor::_checkBlockStatesFilter( bgsched::Block::Status block_status )
{
    if ( ! _block_statuses_ptr ) {
        return true;
    }

    bool ret(_block_statuses_ptr->find( block_status ) != _block_statuses_ptr->end());
    return ret;
}


bool ClientFilterVisitor::_checkIoDrawerStateChanged( bgsched::realtime::ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl& )
{
    return _send_io_drawer_changes;
}


bool ClientFilterVisitor::_checkIoNodeStateChanged( bgsched::realtime::ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl& /*i*/ )
{
    return _send_io_node_changes;
}


} } // namespace realtime::server
