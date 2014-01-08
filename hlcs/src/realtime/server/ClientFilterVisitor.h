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

#ifndef REALTIME_SERVER_CLIENT_FILTER_VISITOR_H_
#define REALTIME_SERVER_CLIENT_FILTER_VISITOR_H_


#include "bgsched/realtime/AbstractDatabaseChangeVisitor.h"

#include "bgsched/BlockImpl.h"
#include "bgsched/JobImpl.h"

#include "bgsched/realtime/ClientEventListenerImpl.h"
#include "bgsched/realtime/FilterImpl.h"


namespace realtime {
namespace server {


class ClientFilterVisitor : public bgsched::realtime::AbstractDatabaseChangeVisitor
{
public:

    ClientFilterVisitor(
            const bgsched::realtime::Filter::Impl& filter
        );

    void visitBlockAdded( bgsched::realtime::ClientEventListener::BlockAddedEventInfo::Impl& i ) { _res = _checkBlockAdded( i ); }
    void visitBlockStateChanged( bgsched::realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& i ) { _res = _checkBlockStateChanged( i ); }
    void visitBlockDeleted( bgsched::realtime::ClientEventListener::BlockDeletedEventInfo::Impl& i ) { _res = _checkBlockDeleted( i ); }
    void visitJobAdded( bgsched::realtime::ClientEventListener::JobAddedEventInfo::Impl& i ) { _res = _checkJobAdded( i ); }
    void visitJobStateChanged( bgsched::realtime::ClientEventListener::JobStateChangedEventInfo::Impl& i ) { _res = _checkJobStateChanged( i ); }
    void visitJobDeleted( bgsched::realtime::ClientEventListener::JobDeletedEventInfo::Impl& i ) { _res = _checkJobDeleted( i ); }
    void visitMidplaneStateChanged( bgsched::realtime::ClientEventListener::MidplaneStateChangedEventInfo::Impl& i ) { _res = _checkMidplaneStateChanged( i ); }
    void visitNodeBoardStateChanged( bgsched::realtime::ClientEventListener::NodeBoardStateChangedEventInfo::Impl& i ) { _res = _checkNodeBoardStateChanged( i ); }
    void visitNodeStateChanged( bgsched::realtime::ClientEventListener::NodeStateChangedEventInfo::Impl& i ) { _res = _checkNodeStateChanged( i ); }
    void visitSwitchStateChanged( bgsched::realtime::ClientEventListener::SwitchStateChangedEventInfo::Impl& i ) { _res = _checkSwitchStateChanged( i ); }
    void visitTorusCableStateChanged( bgsched::realtime::ClientEventListener::TorusCableStateChangedEventInfo::Impl& i ) { _res = _checkTorusCableStateChanged( i ); }
    void visitIoCableStateChanged( bgsched::realtime::ClientEventListener::IoCableStateChangedEventInfo::Impl& i ) { _res = _checkIoCableStateChanged( i ); }
    void visitRasEvent( bgsched::realtime::ClientEventListener::RasEventInfo::Impl& i ) { _res = _checkRasEvent( i ); }

    void visitIoDrawerStateChanged( bgsched::realtime::ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl& i ) { _res = _checkIoDrawerStateChanged( i ); }
    void visitIoNodeStateChanged( bgsched::realtime::ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl& i ) { _res = _checkIoNodeStateChanged( i ); }

    bool getRes() const  { return _res; }


private:

    bool _send_jobs;
    bgsched::realtime::Filter::Impl::RePtr _job_block_re_ptr;
    bgsched::realtime::Filter::Impl::JobStatusesPtr _job_statuses_ptr;
    bool _send_job_delete;

    bool _send_blocks;
    bgsched::realtime::Filter::Impl::RePtr _block_id_re_ptr;
    bgsched::realtime::Filter::Impl::BlockStatusesPtr _block_statuses_ptr;
    bool _send_block_delete;

    bool _send_midplane_changes, _send_node_board_changes, _send_node_changes, _send_switch_changes, _send_torus_cable_changes, _send_io_cable_changes, _send_io_drawer_changes, _send_io_node_changes;

    bool _send_ras_events;
    bgsched::realtime::Filter::Impl::RePtr _ras_event_msg_id_re_ptr;
    bgsched::realtime::Filter::Impl::RePtr _ras_block_id_re_ptr;
    bgsched::realtime::Filter::Impl::RasSeveritiesPtr _ras_severities_ptr;
    bgsched::realtime::Filter::Impl::JobIdsPtr _ras_job_ids_ptr;

    bool _res;


    bool _checkJobStatesFilter( bgsched::Job::Status job_status );

    bool _checkBlockStatesFilter( bgsched::Block::Status block_status );


    bool _checkBlockAdded( bgsched::realtime::ClientEventListener::BlockAddedEventInfo::Impl& );
    bool _checkBlockStateChanged( bgsched::realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& );
    bool _checkBlockDeleted( bgsched::realtime::ClientEventListener::BlockDeletedEventInfo::Impl& );
    bool _checkJobAdded( bgsched::realtime::ClientEventListener::JobAddedEventInfo::Impl& );
    bool _checkJobStateChanged( bgsched::realtime::ClientEventListener::JobStateChangedEventInfo::Impl& );
    bool _checkJobDeleted( bgsched::realtime::ClientEventListener::JobDeletedEventInfo::Impl& );
    bool _checkMidplaneStateChanged( bgsched::realtime::ClientEventListener::MidplaneStateChangedEventInfo::Impl& );
    bool _checkNodeBoardStateChanged( bgsched::realtime::ClientEventListener::NodeBoardStateChangedEventInfo::Impl& );
    bool _checkNodeStateChanged( bgsched::realtime::ClientEventListener::NodeStateChangedEventInfo::Impl& );
    bool _checkSwitchStateChanged( bgsched::realtime::ClientEventListener::SwitchStateChangedEventInfo::Impl& );
    bool _checkTorusCableStateChanged( bgsched::realtime::ClientEventListener::TorusCableStateChangedEventInfo::Impl& );
    bool _checkIoCableStateChanged( bgsched::realtime::ClientEventListener::IoCableStateChangedEventInfo::Impl& );
    bool _checkRasEvent( bgsched::realtime::ClientEventListener::RasEventInfo::Impl& );
    bool _checkIoDrawerStateChanged( bgsched::realtime::ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl& );
    bool _checkIoNodeStateChanged( bgsched::realtime::ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl& );
};

} } // namespace realtime::server

#endif
