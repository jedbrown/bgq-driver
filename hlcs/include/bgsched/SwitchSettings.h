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
 * \file bgsched/SwitchSettings.h
 * \brief SwitchSettings class definition.
 */

#ifndef BGSCHED_SWITCH_SETTINGS_H_
#define BGSCHED_SWITCH_SETTINGS_H_

#include <bgsched/EnumWrapper.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Represents switch settings.
 */
class SwitchSettings
{
public:

    typedef boost::shared_ptr<SwitchSettings> Ptr;             //!< Pointer type.
    typedef boost::shared_ptr<const SwitchSettings> ConstPtr;  //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                             //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;                   //!< Collection of const pointers.

    /*!
     * \brief Ports settings.
     *
     * The ports setting indicates whether input or output ports are in use.
     */
    enum PortsSetting {
        In = 0,  //!< Input port is enabled, output disabled
        Out,     //!< Otput port is enabled, input disabled
        Both     //!< Input and output ports enabled
    };

    /*!
     * \brief Get switch settings location.
     *
     * \return Switch settings location.
     */
    const std::string& getLocation() const;

    /*!
     * \brief Get ports setting.
     *
     * \return Ports settings.
     */
    EnumWrapper<PortsSetting> getPortsSetting() const;

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
     * \brief Get indicator that compute block is using the switch for pass-through.
     *
     * \return Indicator that compute block is using the switch for pass-through.
     */
    bool isPassthrough() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief
     */
    explicit SwitchSettings(
            Pimpl impl      //!< [in] Pointer to implementation
            );

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
