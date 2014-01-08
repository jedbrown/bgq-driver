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

#include <bgsched/InternalException.h>

#include "bgsched/IOBlockFilterImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/filtering/BlockFilter.h>

#include <utility/include/Log.h>

using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

IOBlockFilter::Impl::Impl() :
    _IOBlockFilter()
{
    _IOBlockFilter.setBlockType(BGQDB::filtering::BlockFilter::BlockType::Io);
}

void
IOBlockFilter::Impl::setStatuses(
        const bgsched::IOBlockFilter::Statuses* statusesPtr
        )
{
    // Create statuses
    BGQDB::filtering::BlockFilter::Statuses statuses;

    // Map from bgsched status to db filtering status
    if (statusesPtr != NULL) {
        if (statusesPtr->find(bgsched::IOBlock::Allocated) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Allocated);
        }
        if (statusesPtr->find(bgsched::IOBlock::Booting) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Booting);
        }
        if (statusesPtr->find(bgsched::IOBlock::Free) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Free);
        }
        if (statusesPtr->find(bgsched::IOBlock::Initialized) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Initialized);
        }
        if (statusesPtr->find(bgsched::IOBlock::Terminating) != statusesPtr->end()) {
            statuses.insert(BGQDB::filtering::BlockFilter::Terminating);
        }
    }

    _IOBlockFilter.setStatuses(&statuses);
}

void
IOBlockFilter::Impl::setName(
        const string& name
        )
{
    _IOBlockFilter.setName(name);
}

void
IOBlockFilter::Impl::setOwner(
        const string& owner
        )
{
    _IOBlockFilter.setOwner(owner);
}

void
IOBlockFilter::Impl::setUser(
        const string& user
        )
{
    _IOBlockFilter.setUser(user);
}

void
IOBlockFilter::Impl::setExtendedInfo(
        bool extendedInfo
        )
{
    _IOBlockFilter.setExtendedInfo(extendedInfo);
}

const bgsched::IOBlockFilter::Statuses
IOBlockFilter::Impl::getStatuses() const
{
    bgsched::IOBlockFilter::Statuses schedStatus;
    const BGQDB::filtering::BlockFilter::Statuses dbStatus = _IOBlockFilter.getStatuses();

    // Map from bgsched I/O block status to db filtering status
    if (dbStatus.size() > 0) {
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Allocated) != dbStatus.end()) {
            schedStatus.insert(bgsched::IOBlock::Allocated);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Booting) != dbStatus.end()) {
            schedStatus.insert(bgsched::IOBlock::Booting);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Free) != dbStatus.end()) {
            schedStatus.insert(bgsched::IOBlock::Free);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Initialized) != dbStatus.end()) {
            schedStatus.insert(bgsched::IOBlock::Initialized);
        }
        if (dbStatus.find(BGQDB::filtering::BlockFilter::Terminating) != dbStatus.end()) {
            schedStatus.insert(bgsched::IOBlock::Terminating);
        }
    }

    return schedStatus;
}

const string&
IOBlockFilter::Impl::getName() const
{
    return _IOBlockFilter.getName();
}

const string&
IOBlockFilter::Impl::getOwner() const
{
    return _IOBlockFilter.getOwner();
}

const string&
IOBlockFilter::Impl::getUser() const
{
    return _IOBlockFilter.getUser();
}

bool
IOBlockFilter::Impl::getExtendedInfo() const
{
    return _IOBlockFilter.getExtendedInfo();
}

BGQDB::filtering::BlockFilter
IOBlockFilter::Impl::convertToDBBlockFilter() const
{
    return _IOBlockFilter;
}

} // namespace bgsched
