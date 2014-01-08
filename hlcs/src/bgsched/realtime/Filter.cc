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

#include <bgsched/realtime/Filter.h>

#include "FilterImpl.h"

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "bgsched" );


using namespace std;


namespace bgsched {
namespace realtime {


Filter Filter::createNone()
{
    Filter ret;
    ret._impl_ptr.reset( new Impl(Impl::NONE) );
    return ret;
}


Filter Filter::createAll()
{
    Filter ret;
    ret._impl_ptr.reset( new Impl(Impl::ALL) );
    return ret;
}


Filter::Filter()
    : _impl_ptr(new Impl(Impl::DEFAULT))
{
    // Nothing to do.
}


void Filter::setJobs( bool jobs )
{
    _impl_ptr->setJobs( jobs );
}


bool Filter::getJobs() const
{
    return _impl_ptr->getJobs();
}


void Filter::setJobStatuses( const JobStatuses* job_statuses_p )
{
    _impl_ptr->setJobStatuses( job_statuses_p );
}


const Filter::JobStatuses* Filter::getJobStatuses_p() const
{
    return (_impl_ptr->getJobStatusesPtr() ? _impl_ptr->getJobStatusesPtr().get() : NULL);
}


void Filter::setJobDeleted( bool job_deleted )
{
    _impl_ptr->setJobDeleted( job_deleted );
}


bool Filter::getJobDeleted() const
{
    return _impl_ptr->getJobDeleted();
}


void Filter::setJobBlockIdPattern(
        const string* pattern_p
    )
{
    _impl_ptr->setJobBlockIdPattern( pattern_p );
}


const string* Filter::getJobBlockIdPattern_p() const
{
    return (_impl_ptr->getJobBlockIdPattern() == string() ? NULL : &_impl_ptr->getJobBlockIdPattern());
}


void Filter::setBlocks( bool blocks )
{
    _impl_ptr->setBlocks( blocks );
}


bool Filter::getBlocks() const
{
    return _impl_ptr->getBlocks();
}


void Filter::setBlockIdPattern( const string* block_id_pattern_p )
{
    _impl_ptr->setBlockIdPattern( block_id_pattern_p );
}


const string* Filter::getBlockIdPattern_p() const
{
    return (_impl_ptr->getBlockIdPattern() == string() ? NULL : &_impl_ptr->getBlockIdPattern());
}


void Filter::setBlockStatuses( BlockStatuses* block_statuses_p )
{
    _impl_ptr->setBlockStatuses( block_statuses_p );
}


const Filter::BlockStatuses* Filter::getBlockStatuses_p() const
{
    return (_impl_ptr->getBlockStatusesPtr() ? _impl_ptr->getBlockStatusesPtr().get() : NULL);
}


void Filter::setBlockDeleted( bool block_deleted )
{
    _impl_ptr->setBlockDeleted( block_deleted );
}


bool Filter::getBlockDeleted() const
{
    return _impl_ptr->getBlockDeleted();
}


void Filter::setMidplanes( bool midplanes )  { _impl_ptr->setMidplanes( midplanes ); }

bool Filter::getMidplanes() const  { return _impl_ptr->getMidplanes(); }

void Filter::setNodeBoards( bool node_boards )  { _impl_ptr->setNodeBoards( node_boards ); }

bool Filter::getNodeBoards() const  { return _impl_ptr->getNodeBoards(); }

void Filter::setNodes( bool nodes )  { _impl_ptr->setNodes( nodes ); }

bool Filter::getNodes() const  { return _impl_ptr->getNodes(); }

void Filter::setSwitches( bool switches )  { _impl_ptr->setSwitches( switches ); };

bool Filter::getSwitches() const  { return _impl_ptr->getSwitches(); }

void Filter::setTorusCables( bool torus_cables )  { _impl_ptr->setTorusCables( torus_cables ); }

bool Filter::getTorusCables() const  { return _impl_ptr->getTorusCables(); }

void Filter::setIoCables( bool io_cables )  { _impl_ptr->setIoCables( io_cables ); }

bool Filter::getIoCables() const  { return _impl_ptr->getIoCables(); }


void Filter::setRasEvents( bool ras_events )
{
    _impl_ptr->setRasEvents( ras_events );
}


bool Filter::getRasEvents() const
{
    return _impl_ptr->getRasEvents();
}


void Filter::setRasMessageIdPattern( const string* pattern_p )
{
    _impl_ptr->setRasMessageIdPattern( pattern_p );
}


const string* Filter::getRasMessageIdPattern_p() const
{
    return (_impl_ptr->getRasMessageIdPattern() == string() ? NULL : &_impl_ptr->getRasMessageIdPattern());
}


void Filter::setRasSeverities( const RasSeverities* ras_severities_p )
{
    _impl_ptr->setRasSeverities( ras_severities_p );
}


const Filter::RasSeverities* Filter::getRasSeverities_p() const
{
    return (_impl_ptr->getRasSeveritiesPtr() ? _impl_ptr->getRasSeveritiesPtr().get() : NULL);
}


void Filter::setRasJobIds( const JobIds* job_ids_p )
{
    _impl_ptr->setRasJobIds( job_ids_p );
}


const Filter::JobIds* Filter::getRasJobIds_p() const
{
    return (_impl_ptr->getRasJobIdsPtr() ? _impl_ptr->getRasJobIdsPtr().get() : NULL);
}


void Filter::setRasComputeBlockIdPattern( const string* pattern_p )
{
    _impl_ptr->setRasComputeBlockIdPattern( pattern_p );
}


const string* Filter::getRasComputeBlockIdPattern_p() const
{
    return (_impl_ptr->getRasComputeBlockIdPattern() == string() ? NULL : &_impl_ptr->getRasComputeBlockIdPattern());
}


void Filter::setIoDrawers( bool io_drawers )  { _impl_ptr->setIoDrawers( io_drawers ); }

bool Filter::getIoDrawers() const  { return _impl_ptr->getIoDrawers(); }


void Filter::setIoNodes( bool io_nodes )  { _impl_ptr->setIoNodes( io_nodes ); }

bool Filter::getIoNodes() const  { return _impl_ptr->getIoNodes(); }


} // namespace bgsched::realtime
} // namespace bgsched
