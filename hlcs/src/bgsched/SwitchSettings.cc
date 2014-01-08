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

#include <bgsched/SwitchSettings.h>

#include "bgsched/SwitchSettingsImpl.h"

#include <string>

using namespace bgsched;
using namespace std;

namespace bgsched {

SwitchSettings::SwitchSettings(
        Pimpl impl
        ) :
    _impl(impl)
{
   // Nothing to do
}

const string&
SwitchSettings::getLocation() const
{
    return _impl->getLocation();
}

EnumWrapper<SwitchSettings::PortsSetting>
SwitchSettings::getPortsSetting() const
{
    return _impl->getPortsSetting();
}

bool
SwitchSettings::isIncludedMidplane() const
{
    return _impl->isIncludedMidplane();
}

const string&
SwitchSettings::getComputeBlockName() const
{
    return _impl->getComputeBlockName();
}

bool
SwitchSettings::isPassthrough() const
{
    return _impl->isPassthrough();
}

} // namespace bgsched

