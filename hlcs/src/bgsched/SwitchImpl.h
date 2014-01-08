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

/*!
 * \file bgsched/SwitchImpl.h
 * \brief Switch::Impl class definition.
 */

#ifndef BGSCHED_SWITCH_IMPL_H_
#define BGSCHED_SWITCH_IMPL_H_

#include <bgsched/Cable.h>
#include <bgsched/Dimension.h>
#include <bgsched/Switch.h>
#include <bgsched/types.h>

#include "bgsched/HardwareImpl.h"

#include <iosfwd>
#include <string>

class XMLEntity;

namespace bgsched {

/*!
 * Represents a Switch.
 */
class Switch::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::XMLParseError - if error parsing XML
     */
    Impl(
            const std::string& location,        //!< [in] Switch location
            const XMLEntity* XMLEntityPtr       //!< [in] XML describing the Switch
            );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of a switch.
     */
    Impl(
            const Switch::Pimpl fromSwitch    //!< [in] From switch
            );

    /*!
     * \brief Get the switch "in use" indicator.
     *
     * \return Switch "in use" indicator.
     */
    EnumWrapper<Switch::InUse> getInUse() const;

    /*!
     * \brief Set the switch "in use" indicator".
     */
    void setInUse(
            Switch::InUse inUse //!< [in] Switch "in use" indicator
            );

    /*!
     * \brief Calculate and set the switch "in use" indicator from switch settings
     */
    void calculateInUse();

    /*!
     * \brief Dump the switch to stream.
     */
    void dump(
           std::ostream& os  //!< [in/out] Stream to write to
           );

    /*!
     * \brief Get switch dimension.
     *
     * \return Switch dimension.
     */
    Dimension getDimension() const
    {
        return Dimension(getLocation().getSwitchDimension());
    }

    /*!
     * \brief Get switch settings for the switch.
     *
     * \return Switch settings.
     */
    std::vector<SwitchSettings::Pimpl> getSwitchSettings() const;

    /*!
     * \brief Add switch settings.
     */
    void addSwitchSettings(
            const SwitchSettings::Pimpl switchSettings  //!< [in] Switch settings to add
            );

    /*!
     * \brief Remove settings.
     */
    void removeSwitchSettings(
            const SwitchSettings::Pimpl switchSettings  //!< [in] Switch settings to remove
            );

    /*!
     * \brief Get the switch cable.
     *
     * Cables go "out" from midplanes with lower coordinates to midplanes with higher coordinates,
     * until the end of the dimension is reached. The cable for the last midplane in the dimension
     * goes to the midplane with the lowest coordinates.
     *
     * The cable object contains the destination switch.
     *
     * \return Switch cable.
     */
    Cable::Pimpl getCable() const;

    /*!
     * \brief Set switch cable.
     */
    void setCable(
            Cable::Pimpl cable  //!< [in] Cable
            );

    /*!
     * \brief Get the machine coordinates of the midplane the switch is associated with.
     *
     * \throws bgsched::DatabaseException with value:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     *
     * \return Machine coordinates of the midplane the switch is associated with.
     */
    const Coordinates getMidplaneCoordinates() const;

    /*!
     * \brief Get the midplane location the switch is associated with.
     *
     * \return Midplane location the switch is associated with.
     */
    const std::string getMidplaneLocation() const;

protected:

    std::vector<SwitchSettings::Pimpl> _switchSettings;      //!< Switch settings container
    Cable::Pimpl                       _cable;               //!< Switch cable
    Switch::InUse                      _inUse;               //!< Switch "in use" indicator

};

} // namespace bgsched

#endif
