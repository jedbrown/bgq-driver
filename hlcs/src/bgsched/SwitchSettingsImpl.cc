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

#include "bgsched/SwitchSettingsImpl.h"

#include <utility/include/Log.h>
#include <utility/include/XMLEntity.h>

#include <string.h>

using namespace bgsched;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

SwitchSettings::Impl::Impl(
        const XMLEntity* XMLEntityPtr
) :
    _location(),
    _includeMidplane(true),
    _portsSetting(SwitchSettings::Both),
    _computeBlockName(),
    _passthrough(false)
{
    // Set the switch settings location
    _location = string(XMLEntityPtr->attrByName("switchId"));

    // include attribute values can be "T" or "F"
    if (strcmp(XMLEntityPtr->attrByName("include"),"F") == 0) {
        _includeMidplane = false;
    } else {
        _includeMidplane = true;
    }

    // enablePorts attribute values can be (I)nput, (O)utput or (B)oth
    if (strcmp(XMLEntityPtr->attrByName("enablePorts"),"I") == 0) {
        _portsSetting = SwitchSettings::In;
    } else {
        if (strcmp(XMLEntityPtr->attrByName("enablePorts"),"O") == 0) {
            _portsSetting = SwitchSettings::Out;
        } else {
            _portsSetting = SwitchSettings::Both;
        }
    }

    // passThru attribute values can be "T" or "F"
    if (strcmp(XMLEntityPtr->attrByName("passThru"),"F") == 0) {
        _passthrough = false;
    } else {
        _passthrough = true;
    }

    // Set the compute block name (if any)
    _computeBlockName = string(XMLEntityPtr->attrByName("blockId"));
}

SwitchSettings::Impl::Impl(
        const string& location,
        bool includeMidplane,
        SwitchSettings::PortsSetting portsSetting,
        const string& computeBlockName,
        bool passthrough
) :
    _location(location),
    _includeMidplane(includeMidplane),
    _portsSetting(portsSetting),
    _computeBlockName(computeBlockName),
    _passthrough(passthrough)
{
    // Nothing to do
}

SwitchSettings::Impl::Impl(
        const SwitchSettings::Pimpl fromSwitchSettings
) :
    _location(),
    _includeMidplane(true),
    _portsSetting(SwitchSettings::Both),
    _computeBlockName(),
    _passthrough(false)
{
    _location = fromSwitchSettings->_location;
    _includeMidplane = fromSwitchSettings->_includeMidplane;
    _portsSetting = fromSwitchSettings->_portsSetting;
    _computeBlockName = fromSwitchSettings->_computeBlockName;
    _passthrough = fromSwitchSettings->_passthrough;
}

const string&
SwitchSettings::Impl::getLocation() const
{
    return _location;
}

void
SwitchSettings::Impl::dump(
        ostream& os
        )
{
    os << "       Switch settings . . . . . : " << _location << endl;
    os << "         Compute block . . . . . : " << _computeBlockName << endl;
    if (isPassthrough()) {
        os << "         Passthrough . . . . . . : Yes" << endl;
    } else {
        os << "         Passthrough . . . . . . : No" << endl;
    }
    if (isIncludedMidplane()) {
        os << "         Included  . . . . . . . : Yes" << endl;
    } else {
        os << "         Included  . . . . . . . : No" << endl;
    }
    os << "         Ports setting . . . . . : " << _portsSetting << endl;
}

bool
SwitchSettings::Impl::isIncludedMidplane() const
{
    return _includeMidplane;
}

EnumWrapper<SwitchSettings::PortsSetting>
SwitchSettings::Impl::getPortsSetting() const
{
    return _portsSetting;
}

const string&
SwitchSettings::Impl::getComputeBlockName() const
{
    return _computeBlockName;
}

bool
SwitchSettings::Impl::isPassthrough() const
{
    return _passthrough;
}

ostream&
operator<<(
        ostream& os,
        const SwitchSettings::PortsSetting& portsSetting
        )
{
    // Write database character for value
    switch (portsSetting)
    {
    case SwitchSettings::In:
        os << "I";
        break;
    case SwitchSettings::Out:
        os << "O";
        break;
    case SwitchSettings::Both:
        os << "B";
        break;
    }
    return os;
}

} // namespace bgsched



