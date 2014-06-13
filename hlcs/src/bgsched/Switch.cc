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

#include <bgsched/Switch.h>

#include "bgsched/SwitchImpl.h"

#include <string>

using namespace bgsched;
using namespace std;

namespace bgsched {

Switch::Switch(
        Pimpl impl
        ) :
    Hardware(impl)
{
    // Nothing to do
}

EnumWrapper<Switch::InUse>
Switch::getInUse() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getInUse();
}

SequenceId
Switch::getSequenceId() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getSequenceId();
}

vector<SwitchSettings::ConstPtr>
Switch::getSwitchSettings() const
{
    vector<SwitchSettings::ConstPtr> switchSettingsContainer;

    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    vector<SwitchSettings::Pimpl> switchSettingsPimpls(impl->getSwitchSettings());
    for (vector<SwitchSettings::Pimpl>::const_iterator iter = switchSettingsPimpls.begin(); iter != switchSettingsPimpls.end(); ++iter) {
        SwitchSettings::ConstPtr switchSettings(new SwitchSettings(*iter));
        switchSettingsContainer.push_back(switchSettings);
    }

    return switchSettingsContainer;
}

Cable::ConstPtr
Switch::getCable() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    Cable::Pimpl cable = impl->getCable();
    if (!cable) {
        return Cable::ConstPtr();
    } else {
        return Cable::ConstPtr(new Cable(cable));
    }
}

const Coordinates
Switch::getMidplaneCoordinates() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplaneCoordinates();
}

const string
Switch::getMidplaneLocation() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getMidplaneLocation();
}

} // namespace bgsched
