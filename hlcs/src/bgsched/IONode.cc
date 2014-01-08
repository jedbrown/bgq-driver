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

#include <bgsched/IONode.h>

#include "bgsched/IONodeImpl.h"

using namespace bgsched;
using namespace std;

namespace bgsched {

IONode::IONode(
        Pimpl impl
        ) :
    Hardware(impl)
{
    // Nothing to do.
}

SequenceId
IONode::getSequenceId() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getSequenceId();
}

bool
IONode::isInUse() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->isInUse();
}

const string&
IONode::getIOBlockName() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getIOBlockName();
}

} // namespace bgsched
