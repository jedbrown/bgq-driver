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

#ifndef BGSCHED_REALTIME_ABSTRACT_DATABASE_CHANGE_VISITOR_H_
#define BGSCHED_REALTIME_ABSTRACT_DATABASE_CHANGE_VISITOR_H_


#include "ClientEventListenerImpl.h"
#include "ClientEventListenerV2Impl.h"


namespace bgsched {
namespace realtime {


class AbstractDatabaseChangeVisitor
{
public:

    virtual void visitBlockAdded( ClientEventListener::BlockAddedEventInfo::Impl& ) =0;
    virtual void visitBlockStateChanged( ClientEventListener::BlockStateChangedEventInfo::Impl& ) =0;
    virtual void visitBlockDeleted( ClientEventListener::BlockDeletedEventInfo::Impl& ) =0;
    virtual void visitJobAdded( ClientEventListener::JobAddedEventInfo::Impl& ) =0;
    virtual void visitJobStateChanged( ClientEventListener::JobStateChangedEventInfo::Impl& ) =0;
    virtual void visitJobDeleted( ClientEventListener::JobDeletedEventInfo::Impl& ) =0;
    virtual void visitMidplaneStateChanged( ClientEventListener::MidplaneStateChangedEventInfo::Impl& ) =0;
    virtual void visitNodeBoardStateChanged( ClientEventListener::NodeBoardStateChangedEventInfo::Impl& ) =0;
    virtual void visitNodeStateChanged( ClientEventListener::NodeStateChangedEventInfo::Impl& ) =0;
    virtual void visitSwitchStateChanged( ClientEventListener::SwitchStateChangedEventInfo::Impl& ) =0;
    virtual void visitTorusCableStateChanged( ClientEventListener::TorusCableStateChangedEventInfo::Impl& ) =0;
    virtual void visitIoCableStateChanged( ClientEventListener::IoCableStateChangedEventInfo::Impl& ) =0;
    virtual void visitRasEvent( ClientEventListener::RasEventInfo::Impl& ) =0;

    virtual void visitIoDrawerStateChanged( ClientEventListenerV2::IoDrawerStateChangedEventInfo::Impl& ) =0;
    virtual void visitIoNodeStateChanged( ClientEventListenerV2::IoNodeStateChangedEventInfo::Impl& ) =0;

    virtual ~AbstractDatabaseChangeVisitor() { /* Nothing to do */ }

};


} // namespace bgsched::realtime
} // namespace bgsched


#endif
