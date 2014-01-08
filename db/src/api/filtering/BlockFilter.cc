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
#include <db/include/api/filtering/BlockFilter.h>

using namespace std;

namespace BGQDB {
namespace filtering {

BlockFilter::BlockFilter() :
    _extendedInfo(false),
    _includeJobs(false),
    _name(),
    _owner(),
    _user(),
    _size(BlockSize::All),
    _status(),
    _blockType(BlockType::Any)
{
 /* Nothing to do */
}

void
BlockFilter::setOwner(
        const string& owner
)
{
    _owner = owner;
}

void
BlockFilter::setUser(
        const string& user
)
{
    _user = user;
}

void
BlockFilter::setStatuses(
        const Statuses* statusesPtr
)
{
    _status.clear();

    if (statusesPtr != NULL) {
        _status = *statusesPtr;
    }
}

void
BlockFilter::setName(
        const string& name
)
{
    _name = name;
}

void
BlockFilter::setExtendedInfo(
        bool extendedInfo
)
{
    _extendedInfo = extendedInfo;
}

void
BlockFilter::setIncludeJobs(
        bool includeJobs
)
{
    _includeJobs = includeJobs;
}

void
BlockFilter::setSize(
        BlockSize::Value blockSize
)
{
    _size = blockSize;
}

const string&
BlockFilter::getOwner() const
{
    return _owner;
}

const string&
BlockFilter::getUser() const
{
    return _user;
}

const BlockFilter::Statuses
BlockFilter::getStatuses() const
{
    return _status;
}

const string&
BlockFilter::getName() const
{
    return _name;
}

bool
BlockFilter::getExtendedInfo() const
{
    return _extendedInfo;
}

bool
BlockFilter::getIncludeJobs() const
{
    return _includeJobs;
}

BlockFilter::BlockSize::Value
BlockFilter::getSize() const
{
    return _size;
}

BlockFilter::BlockType::Value
BlockFilter::getBlockType() const
{
    return _blockType;
}

} // filtering
} // BGQDB

