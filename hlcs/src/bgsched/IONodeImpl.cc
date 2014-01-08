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

#include "bgsched/IONodeImpl.h"

using namespace bgsched;
using namespace std;

namespace bgsched {

IONode::Impl::Impl(
        const string& location,
        const Hardware::State state,
        const SequenceId sequenceId,
        const bool inUse,
        const std::string& ioblock
        ) :
    Hardware::Impl(location),
    _inUse(inUse),
    _IOBlockName(ioblock)
{
    _state = state;
    _sequenceId = sequenceId;
}

IONode::Impl::Impl(
        const IONode::Pimpl fromIONode
) :
    Hardware::Impl(fromIONode->getLocationString())
{
    _state = fromIONode->getState();             // Set I/O node hardware state
    _sequenceId = fromIONode->getSequenceId();   // Set I/O node sequence ID
    _inUse = fromIONode->isInUse();              // Set I/O node "in use" indicator
    _IOBlockName = fromIONode->getIOBlockName(); // Set I/O node "inn use" I/O block
}

bool
IONode::Impl::isInUse() const
{
    return _inUse;
}

const string&
IONode::Impl::getIOBlockName() const
{
    return _IOBlockName;
}

} // namespace bgsched
