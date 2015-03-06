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

#include <bgsched/IODrawer.h>

#include "bgsched/IODrawerImpl.h"

using namespace std;

namespace bgsched {

IODrawer::IODrawer(
        Pimpl impl
        ) :
    Hardware(impl)
{
    // Nothing to do
}

SequenceId
IODrawer::getSequenceId() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getSequenceId();
}

uint32_t
IODrawer::getAvailableIONodeCount() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getAvailableIONodeCount();
}

IONode::ConstPtrs
IODrawer::getIONodes() const
{
    IONode::ConstPtrs IONodesContainer;

    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    vector<IONode::Pimpl> IONodePimpls(impl->getIONodes());
    for (vector<IONode::Pimpl>::const_iterator iter = IONodePimpls.begin(); iter != IONodePimpls.end(); ++iter) {
        IONode::ConstPtr ionode(new IONode(*iter));
        IONodesContainer.push_back(ionode);
    }

    return IONodesContainer;
}

} // namespace bgsched
