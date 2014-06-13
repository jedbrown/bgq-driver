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

#include <bgsched/Dimension.h>
#include <bgsched/DatabaseException.h>
#include <bgsched/InternalException.h>

#include "bgsched/CableImpl.h"
#include "bgsched/SwitchImpl.h"
#include "bgsched/SwitchSettingsImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/Exception.h>
#include <db/include/api/genblock.h>

#include <utility/include/Log.h>
#include <utility/include/XMLEntity.h>

#include <boost/lexical_cast.hpp>

#include <sstream>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace bgsched {

Switch::Impl::Impl(
        const string& location,
        const XMLEntity* XMLEntityPtr
        ) :
    Hardware::Impl(location),
    _switchSettings(),
    _cable(),
    _inUse(Switch::NotInUse)
{
    // ** Note: switch settings are not set by default, must use addSwitchSettings() method
    // ** Note: cable is not set by default, must use setCable() method
    // ** Note: in use is not set by default, must use setInUse() or calculateInUse() method
    _state = convertDatabaseState(XMLEntityPtr->attrByName("status"));

    try {
        _sequenceId = boost::lexical_cast<SequenceId>(XMLEntityPtr->attrByName("statusSeqID"));
    } catch (const boost::bad_lexical_cast& e) {
        LOG_INFO_MSG(e.what());
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                "Unexpected error parsing XML."
        );
    }
}

Switch::Impl::Impl(
        const Switch::Pimpl fromSwitch
        ) :
    Hardware::Impl(fromSwitch->getLocationString()),
    _switchSettings(),
    _cable(),
    _inUse(Switch::NotInUse)
{
    _state = fromSwitch->getState(); // Set switch hardware state
    _isLiveState = fromSwitch->isLiveState(); // Set live state indicator
    _sequenceId = fromSwitch->getSequenceId(); // Set switch sequence ID

    // Set switch settings
    vector<SwitchSettings::Pimpl> switchSettingsPimpls = fromSwitch->getSwitchSettings();
    for (vector<SwitchSettings::Pimpl>::const_iterator iter = switchSettingsPimpls.begin(); iter != switchSettingsPimpls.end(); ++iter) {
        _switchSettings.push_back(SwitchSettings::Pimpl(new SwitchSettings::Impl(*iter)));
    }

    // Set switch cable
    if (fromSwitch->_cable) {
        _cable = Cable::Pimpl(new Cable::Impl(fromSwitch->_cable));
    }
    _inUse = fromSwitch->_inUse;
}

EnumWrapper<Switch::InUse>
Switch::Impl::getInUse() const
{
    return _inUse;
}

void
Switch::Impl::setInUse(
        Switch::InUse inUse
        )
{
    _inUse = inUse;
    switch (inUse) {
        case Switch::NotInUse:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not in use'.");
            break;
        case Switch::IncludedBothPortsInUse:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'included, input/output ports in use'.");
            break;
        case Switch::IncludedOutputPortInUse:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'included, output port in use'.");
            break;
        case Switch::IncludedInputPortInUse:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'included, input port in use'.");
            break;
        case Switch::Wrapped:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not included, ports are wrapped'.");
            break;
        case Switch::Passthrough:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not included, ports used for passthrough'.");
            break;
        case Switch::WrappedPassthrough:
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not included, ports are wrapped and used for passthrough'.");
            break;
    }
}

void
Switch::Impl::calculateInUse()
{
    // Calculate and set the "in use" state based on switch settings
    // Any switch settings found?
    if (_switchSettings.size() == 0) {
        // No switch settings so switch is 'not in use'
        _inUse = Switch::NotInUse;
        //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not in use'.");
    } else {
        // Check for wrapped and passthrough blocks
        if (_switchSettings.size() == 2) {
            // Set wrapped and passthrough
            _inUse = Switch::WrappedPassthrough;
            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not included, ports are wrapped and used for passthrough'.");
        } else {
            // Only one switch setting so examine it
            SwitchSettings::Pimpl switchSettings = _switchSettings[0];
            // Switch setting for passthrough?
            if (switchSettings->isPassthrough()) {
                // Set passthrough
                _inUse = Switch::Passthrough;
                //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not included, ports used for passthrough'.");
            } else {
                if (switchSettings->isIncludedMidplane()) {
                    // Included so now check port settings
                    if (SwitchSettings::Both == switchSettings->getPortsSetting().toValue()) {
                        // Set included, input/output ports in use
                        _inUse = Switch::IncludedBothPortsInUse;
                        //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'included, input/output ports in use'.");
                    } else {
                        if (SwitchSettings::In == switchSettings->getPortsSetting().toValue()) {
                            // Set included, input port in use
                            _inUse  = Switch::IncludedInputPortInUse;
                            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'included, input port in use'.");
                        } else {
                            // Set included, output port in use
                            _inUse = Switch::IncludedOutputPortInUse;
                            //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'included, output port in use'.");
                        }
                    }
                } else { // Not included so must be wrapped
                    _inUse = Switch::Wrapped;
                    //LOG_TRACE_MSG("Switch " << getLocationString() << " set to 'not included, ports are wrapped'.");
                }
            }
        }
    }
}

void
Switch::Impl::dump(
        ostream& os
        )
{
    os << "     Switch location . . . . . . : " << getLocationString() << endl;

    switch (_inUse) {
        case Switch::NotInUse:
            os << "     In use  . . . . . . . . . . : Switch is not in use" << endl;
            break;
        case Switch::IncludedBothPortsInUse:
            os << "     In use  . . . . . . . . . . : Switch included, input/output ports in use" << endl;
            break;
        case Switch::IncludedOutputPortInUse:
            os << "     In use  . . . . . . . . . . : Switch included, output port in use" << endl;
            break;
        case Switch::IncludedInputPortInUse:
            os << "     In use  . . . . . . . . . . : Switch included, input port in use" << endl;
            break;
        case Switch::Wrapped:
            os << "     In use  . . . . . . . . . . : Switch not included, ports are wrapped" << endl;
            break;
        case Switch::Passthrough:
            os << "     In use  . . . . . . . . . . : Switch not included, ports used for passthrough" << endl;
            break;
        case Switch::WrappedPassthrough:
            os << "     In use  . . . . . . . . . . : Switch not included, ports are wrapped and used for passthrough" << endl;
            break;
    }

    os << "     Hardware state  . . . . . . : " << _state << endl;
    if (isLiveState()) {
        os << "     Hardware state from database: Yes" << endl;
    } else {
        os << "     Hardware state from database: No" << endl;
    }

    os << "     Sequence ID . . . . . . . . : " << getSequenceId() << endl;

    // Dump switch settings
    if (_switchSettings.size() == 0) {
        os << "     Switch settings . . . . . . : No switch settings" << endl;
    } else {
        for (vector<SwitchSettings::Pimpl>::const_iterator iter = _switchSettings.begin(); iter != _switchSettings.end(); ++iter) {
           (*iter)->dump(os);
        }
    }

    // Dump switch cable
    if (_cable) {
        _cable->dump(os);
    } else {
        os << "     Cable . . . . . . . . . . . : None" << endl;
    }
}

vector<SwitchSettings::Pimpl>
Switch::Impl::getSwitchSettings() const
{
    return _switchSettings;
}

void
Switch::Impl::addSwitchSettings(
        const SwitchSettings::Pimpl switchSettings
        )
{
    _switchSettings.push_back(switchSettings);
}

void
Switch::Impl::removeSwitchSettings(
        const SwitchSettings::Pimpl switchSettings
        )
{
    vector<SwitchSettings::Pimpl>::iterator result = find(_switchSettings.begin(), _switchSettings.end(), switchSettings);

    if (result != _switchSettings.end()) {
        _switchSettings.erase(result);
    }
}

Cable::Pimpl
Switch::Impl::getCable() const
{
    return _cable;
}

void
Switch::Impl::setCable(
        Cable::Pimpl cable
        )
{
    _cable = cable;
}

const Coordinates
Switch::Impl::getMidplaneCoordinates() const
{
    Coordinates midplaneCoordinates(0,0,0,0);
    BGQDB::MidplaneCoordinate mp_coord;
    try {
        BGQDB::torusCoordinateForMidplane(_location.getMidplaneLocation(), mp_coord);
        midplaneCoordinates[Dimension::A] = mp_coord[Dimension::A];
        midplaneCoordinates[Dimension::B] = mp_coord[Dimension::B];
        midplaneCoordinates[Dimension::C] = mp_coord[Dimension::C];
        midplaneCoordinates[Dimension::D] = mp_coord[Dimension::D];
    } catch (const BGQDB::Exception& e) {
        ostringstream os;
        os << "Unable to get midplane coordinates for midplane location " << _location.getMidplaneLocation() << ". Error is: " << e.what();
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                os.str()
        );
    }
    return midplaneCoordinates;
}

const string
Switch::Impl::getMidplaneLocation() const
{
    return _location.getMidplaneLocation();
}

} // namespace bgsched
