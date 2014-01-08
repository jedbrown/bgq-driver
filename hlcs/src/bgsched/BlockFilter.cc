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

#include <bgsched/BlockFilter.h>

#include "bgsched/BlockFilterImpl.h"

using namespace bgsched;
using namespace std;

namespace bgsched {

BlockFilter::BlockFilter() :
    _impl(new Impl())
{
     // Nothing to do
}

BlockFilter::Pimpl
BlockFilter::getPimpl() const
{
    return _impl;
}

void
BlockFilter::setStatuses(
        const Statuses* statusesPtr
        )
{
    return _impl->setStatuses(statusesPtr);
}

void
BlockFilter::setName(
        const string& name
        )
{
    return _impl->setName(name);
}

void
BlockFilter::setOwner(
        const string& owner
        )
{
    return _impl->setOwner(owner);
}

void
BlockFilter::setUser(
        const string& user
        )
{
    return _impl->setUser(user);
}

void
BlockFilter::setExtendedInfo(
        bool extendedInfo
        )
{
    return _impl->setExtendedInfo(extendedInfo);
}

void
BlockFilter::setIncludeJobs(
        bool includeJobs
        )
{
    return _impl->setIncludeJobs(includeJobs);
}

void
BlockFilter::setSize(
        BlockSize::Value blockSize
        )
{
    return _impl->setSize(blockSize);
}

const BlockFilter::Statuses
BlockFilter::getStatuses() const
{
    return _impl->getStatuses();
}

const string&
BlockFilter::getName() const
{
    return _impl->getName();
}

const string&
BlockFilter::getOwner() const
{
    return _impl->getOwner();
}

const string&
BlockFilter::getUser() const
{
    return _impl->getUser();
}

bool
BlockFilter::getExtendedInfo() const
{
    return _impl->getExtendedInfo();
}

bool
BlockFilter::getIncludeJobs() const
{
    return _impl->getIncludeJobs();
}

BlockFilter::BlockSize::Value
BlockFilter::getSize() const
{
    return _impl->getSize();
}

} // namespace bgsched
