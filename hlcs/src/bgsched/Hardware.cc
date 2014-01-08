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

#include <bgsched/Hardware.h>

#include "bgsched/HardwareImpl.h"

#include <sstream>

using namespace bgsched;
using namespace std;

namespace bgsched {

Hardware::Hardware(
        Pimpl impl
        ) :
    _impl(impl)
{
    // Nothing to do
}

Hardware::Pimpl
Hardware::getPimpl() const
{
    return _impl;
}

Hardware::~Hardware()
{
    // Nothing to do
}

string
Hardware::toString() const
{
    ostringstream os;
    os << *_impl;
    return os.str();
}

EnumWrapper<Hardware::State>
Hardware::getState() const
{
    return _impl->getState();
}

const string&
Hardware::getLocation() const
{
    return _impl->getLocationString();
}

} // namespace bgsched
