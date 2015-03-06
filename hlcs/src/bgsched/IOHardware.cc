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

#include <bgsched/IOHardware.h>
#include <bgsched/InputException.h>

#include "bgsched/IOHardwareImpl.h"
#include "bgsched/utility.h"

#include <string>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace bgsched {

IOHardware::IOHardware(
        Pimpl impl
) :
    _impl(impl)
{
    // Nothing to do
}

EnumWrapper<Hardware::State>
IOHardware::getState(
        const string& location
) const
{
    return _impl->getState(location);
}

IODrawer::ConstPtrs
IOHardware::getIODrawers() const
{
    IODrawer::ConstPtrs IODrawersContainer;

    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    vector<IODrawer::Pimpl> IODrawerPimpls(impl->getIODrawers());
    for (vector<IODrawer::Pimpl>::const_iterator iter = IODrawerPimpls.begin(); iter != IODrawerPimpls.end(); ++iter) {
        IODrawer::ConstPtr IOdrawer(new IODrawer(*iter));
        IODrawersContainer.push_back(IOdrawer);
    }

    return IODrawersContainer;
}

IODrawer::ConstPtr
IOHardware::getIODrawer(
        const string& location
) const
{
    try {
        IODrawer::Pimpl impl(_impl->getIODrawer(location));
        IODrawer::ConstPtr result(new IODrawer(impl));
        return result;
    } catch (...) {
        // Rethrow any exceptions
        throw;
    }
}

IONode::ConstPtr
IOHardware::getIONode(
        const string& location
) const
{
    try {
        IONode::Pimpl impl(_impl->getIONode(location));
        IONode::ConstPtr result(new IONode(impl));
        return result;
    } catch (...) {
        // Rethrow any exceptions
        throw;
    }
}

} // namespace bgsched

