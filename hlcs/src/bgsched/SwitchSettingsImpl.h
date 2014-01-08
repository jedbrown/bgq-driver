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
 * \file bgsched/SwitchSettingsImpl.h
 * \brief SwitchSettings::Impl class definition.
 */

#ifndef BGSCHED_SWITCH_SETTINGS_IMPL_H_
#define BGSCHED_SWITCH_SETTINGS_IMPL_H_

#include <bgsched/SwitchSettings.h>

#include <string>

class XMLEntity;

namespace bgsched {

/*!
 * Represents the switch settings.
 */
class SwitchSettings::Impl
{
public:

    /*!
     * \brief
     */
    Impl(
            const XMLEntity* XMLEntityPtr                //!< [in] XML describing the switch settings
    );

    /*!
     * \brief
     */
    Impl(
            const std::string& location,                 //!< [in] Switch settings location
            bool includeMidplane,                        //!< [in] true: midplane is included, false: indicates wrapped midplane or pass-through
            SwitchSettings::PortsSetting portsSetting,   //!< [in] Switch port settings
            const std::string& computeBlockName,         //!< [in] "In use" Compute block name
            bool passthrough                             //!< [in] true: pass-through, false: not pass-through
    );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of the switch settings.
     */
    Impl(
            const SwitchSettings::Pimpl fromSwitchSettings    //!< [in] From switch settings
    );

    /*!
     * \brief Dump the switch settings to stream.
     */
    void dump(
            std::ostream& os  //!< [in/out] Stream to write to
    );

    /*!
     * \brief Get switch settings location.
     *
     * \return Switch settings location.
     */
    const std::string& getLocation() const;

    /*!
     * \brief Get ports setting.
     *
     * \return Ports setting.
     */
    EnumWrapper<SwitchSettings::PortsSetting> getPortsSetting() const;

    /*!
     * \brief Return true if midplane is included, false indicates wrapped midplane or pass-through.
     *
     * \return true if midplane is included, false indicates wrapped midplane or pass-through.
     */
    bool isIncludedMidplane() const;

    /*!
     * \brief Get compute block using the switch.
     *
     * \return Compute block using the switch.
     */
    const std::string& getComputeBlockName() const;

    /*!
     * \brief Set the "in use" compute block name.
     */
    void setComputeBlockName(
            const std::string& computeBlockName  //!< [in] Compute block name
    );

    /*!
     * \brief Get indicator that compute block is using the switch for pass-through.
     *
     * \return Indicator that compute block is using the switch for pass-through.
     */
    bool isPassthrough() const;

protected:

    std::string                   _location;          //!< Switch settings location
    bool                          _includeMidplane;   //!< true: midplane is included, false: indicates wrapped midplane or pass-through
    SwitchSettings::PortsSetting  _portsSetting;      //!< Switch port settings
    std::string                   _computeBlockName;  //!< Compute block name
    bool                          _passthrough;       //!< true: pass-through, false: not pass-through

};

/*!
 * \brief ostream operator.
 */
std::ostream&
operator<<(
        std::ostream& os,                                 //!< [in/out] Stream to write to
        const SwitchSettings::PortsSetting& portsSetting  //!< [in] Port settings
        );

} // namespace bgsched

#endif

