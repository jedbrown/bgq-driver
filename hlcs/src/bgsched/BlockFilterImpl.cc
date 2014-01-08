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

#include <bgsched/InternalException.h>

#include "bgsched/BlockFilterImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/filtering/BlockFilter.h>

#include <utility/include/Log.h>

using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

BlockFilter::Impl::Impl() :
    _blockFilter()
{
    _blockFilter.setBlockType( BGQDB::filtering::BlockFilter::BlockType::Compute );
}

void
BlockFilter::Impl::setStatuses(
        const bgsched::BlockFilter::Statuses* statusesPtr
        )
{
    // Create statuses
    BGQDB::filtering::BlockFilter::Statuses statuses;

    // Map from bgsched status to db filtering status
    if (statusesPtr != NULL) {
        if (statusesPtr->find(bgsched::Block::Allocated) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Allocated);
        }
        if (statusesPtr->find(bgsched::Block::Booting) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Booting);
        }
        if (statusesPtr->find(bgsched::Block::Free) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Free);
        }
        if (statusesPtr->find(bgsched::Block::Initialized) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Initialized);
        }
        if (statusesPtr->find(bgsched::Block::Terminating) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Terminating);
        }
    }

    _blockFilter.setStatuses(&statuses);
}

void
BlockFilter::Impl::setName(
        const string& name
        )
{
    _blockFilter.setName(name);
}

void
BlockFilter::Impl::setOwner(
        const string& owner
        )
{
    _blockFilter.setOwner(owner);
}

void
BlockFilter::Impl::setUser(
        const string& user
        )
{
    _blockFilter.setUser(user);
}

void
BlockFilter::Impl::setExtendedInfo(
        bool extendedInfo
        )
{
    _blockFilter.setExtendedInfo(extendedInfo);
}

void
BlockFilter::Impl::setIncludeJobs(
        bool includeJobs
        )
{
    _blockFilter.setIncludeJobs(includeJobs);
}

void
BlockFilter::Impl::setSize(
        bgsched::BlockFilter::BlockSize::Value blockSize
        )
{
    // Map from bgsched block sizes to db filtering block sizes
    switch (blockSize) {
        case bgsched::BlockFilter::BlockSize::Large:
            return _blockFilter.setSize(BGQDB::filtering::BlockFilter::BlockSize::Large);
        case bgsched::BlockFilter::BlockSize::Small:
            return _blockFilter.setSize(BGQDB::filtering::BlockFilter::BlockSize::Small);
        case bgsched::BlockFilter::BlockSize::All:
            return _blockFilter.setSize(BGQDB::filtering::BlockFilter::BlockSize::All);
        default:
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::UnexpectedError,
                    "Unexpected error mapping BlockFilter size"
                    );
    }
}

const bgsched::BlockFilter::Statuses
BlockFilter::Impl::getStatuses() const
{
    bgsched::BlockFilter::Statuses schedStatus;
    const BGQDB::filtering::BlockFilter::Statuses dbStatus = _blockFilter.getStatuses();

    // Map from bgsched status to db filtering status
    if (dbStatus.size() > 0) {
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Allocated) != dbStatus.end()) {
            schedStatus.insert(bgsched::Block::Allocated);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Booting) != dbStatus.end()) {
            schedStatus.insert(bgsched::Block::Booting);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Free) != dbStatus.end()) {
            schedStatus.insert(bgsched::Block::Free);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Initialized) != dbStatus.end()) {
            schedStatus.insert(bgsched::Block::Initialized);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Terminating) != dbStatus.end()) {
            schedStatus.insert(bgsched::Block::Terminating);
        }
    }

    return schedStatus;
}

const string&
BlockFilter::Impl::getName() const
{
    return _blockFilter.getName();
}

const string&
BlockFilter::Impl::getOwner() const
{
    return _blockFilter.getOwner();
}

const string&
BlockFilter::Impl::getUser() const
{
    return _blockFilter.getUser();
}

bool
BlockFilter::Impl::getExtendedInfo() const
{
    return _blockFilter.getExtendedInfo();
}

bool
BlockFilter::Impl::getIncludeJobs() const
{
    return _blockFilter.getIncludeJobs();
}

bgsched::BlockFilter::BlockSize::Value
BlockFilter::Impl::getSize() const
{
   BGQDB::filtering::BlockFilter::BlockSize::Value blockSize = _blockFilter.getSize();

   // Map from db filtering block sizes to bgsched block sizes
   switch (blockSize) {
       case BGQDB::filtering::BlockFilter::BlockSize::Large:
           return bgsched::BlockFilter::BlockSize::Large;
       case BGQDB::filtering::BlockFilter::BlockSize::Small:
           return bgsched::BlockFilter::BlockSize::Small;
       case BGQDB::filtering::BlockFilter::BlockSize::All:
           return bgsched::BlockFilter::BlockSize::All;
       default:
           THROW_EXCEPTION(
                   bgsched::InternalException,
                   bgsched::InternalErrors::UnexpectedError,
                   "Unexpected error mapping BlockFilter size"
                   );
   }
}

BGQDB::filtering::BlockFilter
BlockFilter::Impl::convertToDBBlockFilter() const
{
    return _blockFilter;
}

} // namespace bgsched
