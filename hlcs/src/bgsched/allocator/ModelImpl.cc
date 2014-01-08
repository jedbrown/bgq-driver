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

#include <bgsched/allocator/InputException.h>
#include <bgsched/allocator/Midplanes.h>

#include "bgsched/allocator/AllocatorImpl.h"
#include "bgsched/allocator/InternalException.h"
#include "bgsched/allocator/MidplanesImpl.h"
#include "bgsched/allocator/ModelImpl.h"

#include <bgsched/Coordinates.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/SchedUtil.h>

#include "bgsched/MidplaneImpl.h"
#include "bgsched/ShapeImpl.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>

#include <cmath>
#include <iomanip>
#include <iostream>

using namespace bgsched;
using namespace bgsched::allocator;
using namespace bgq::util;
using namespace std;

namespace {
    // Message strings
    const string InvalidMidplaneCoordinates_Str("Invalid midplane coordinates.");
} // anonymous namespace

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace allocator {

Model::Impl::Impl(
        Allocator::Impl* allocator,
        ComputeHardware::Pimpl hardware
        ) :
    _allocator(allocator),
    _hardware(hardware),
    _mutex(),
    _drainedMidplanes(),
    _drainedMidplaneMutex()
{
    // Nothing to do
}

Model::Impl::~Impl()
{
    // Nothing to do
}

ComputeHardware::Pimpl
Model::Impl::getComputeHardware()
{
   LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
   // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
   boost::unique_lock<boost::shared_mutex> lock(_mutex);
   LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");
   return _hardware;
}

Midplanes::Ptr
Model::Impl::getMidplanes()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");
    Midplanes::Pimpl impl(new Midplanes::Impl(_hardware));
    Midplanes::Ptr result(new Midplanes(impl));
    return result;
}

Midplane::Ptr
Model::Impl::getMidplane(
        const Coordinates& coordinates
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");
    try {
        Midplane::Pimpl impl = _hardware->getMidplane(coordinates);
        Midplane::Ptr result(new Midplane(impl));
        return result;
    } catch (const bgsched::InputException& e) {
        THROW_EXCEPTION(
                bgsched::allocator::InputException,
                bgsched::allocator::InputErrors::InvalidMidplaneCoordinates,
                InvalidMidplaneCoordinates_Str
                );
    }
}

Midplane::Ptr
Model::Impl::getMidplane(
        const string& location
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_mutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");
    try {
        Midplane::Pimpl impl = _hardware->getMidplane(location);
        Midplane::Ptr result(new Midplane(impl));
        return result;
    } catch (const bgsched::InputException& e) {
        THROW_EXCEPTION(
                bgsched::allocator::InputException,
                bgsched::allocator::InputErrors::InvalidLocationString,
                "Invalid midplane location: " << location );
    }
}

Model::DrainedMidplanes
Model::Impl::getDrainedMidplanes()
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain shared read lock");
    // Lock mutex as shared read by multiple threads, mutex is automatically unlocked when stack is unwound
    boost::shared_lock<boost::shared_mutex> lock(_drainedMidplaneMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained shared read lock");
    return _drainedMidplanes;
}

void
Model::Impl::addDrainedMidplane(
        const string& midplaneLocation
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_drainedMidplaneMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Add the midplane location to the drain list
    if (! _drainedMidplanes.insert(midplaneLocation).second) {
        LOG_WARN_MSG("Midplane " <<  midplaneLocation << " already exists in drain list");
    } else {
        LOG_TRACE_MSG("Added midplane " << midplaneLocation << " to model drain list");
    }
}

void
Model::Impl::removeDrainedMidplane(
        const string& midplaneLocation
        )
{
    LOG_TRACE_MSG(__FUNCTION__ << " Trying to obtain exclusive write lock");
    // Lock mutex as single writer across multiple threads, mutex is automatically unlocked when stack is unwound
    boost::unique_lock<boost::shared_mutex> lock(_drainedMidplaneMutex);
    LOG_TRACE_MSG(__FUNCTION__ << " Obtained exclusive write lock");

    // Remove the midplane location from the drain list
    if (_drainedMidplanes.erase(midplaneLocation) == 0) {
        LOG_WARN_MSG("Midplane " << midplaneLocation << " does not exist in drain list");
    } else {
        LOG_TRACE_MSG("Removed midplane " << midplaneLocation << " from model drain list");
    }
}

} // namespace bgsched::allocator
} // namespace bgsched
