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
 * \file bgsched/Switch.h
 * \brief Switch class definition.
 */

#ifndef BGSCHED_SWITCH_H_
#define BGSCHED_SWITCH_H_

#include <bgsched/Cable.h>
#include <bgsched/Coordinates.h>
#include <bgsched/EnumWrapper.h>
#include <bgsched/Hardware.h>
#include <bgsched/SwitchSettings.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace bgsched {

/*!
 * \brief Represents a switch.
 *
 * A switch is not a single piece of hardware, but represents the torus connections between midplanes.
 */
class Switch : public Hardware
{
public:

    typedef boost::shared_ptr<Switch> Ptr;               //!< Pointer type.
    typedef boost::shared_ptr<const Switch> ConstPtr;    //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Switch in use indicator.
     *
     * When a switch is in use, the getSwitchSettings() method can be used to obtain additional
     * details about switch settings including the compute block(s) using the switch.
     */
    enum InUse {
        NotInUse = 0,            //!< Switch is not in use
        IncludedBothPortsInUse,  //!< Switch is included, and both input/output ports are in use (Torus config)
        IncludedOutputPortInUse, //!< Switch is included, and only output port is in use (Mesh config)
        IncludedInputPortInUse,  //!< Switch is included, and only input port is in use (Mesh config)
        Wrapped,                 //!< Switch is not included and ports are wrapped
        Passthrough,             //!< Switch is not included and ports are used for passthrough
        WrappedPassthrough       //!< Switch is not included and ports are wrapped and used for passthrough
    };

    /*!
     * \brief Get the switch "in use" indicator.
     *
     * \return Switch "in use" indicator.
     */
    EnumWrapper<Switch::InUse> getInUse() const;

    /*!
     * \brief Get switch sequence ID.
     *
     * \return Switch sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get switch settings for the switch.
     *
     * \return Switch settings.
     */
    SwitchSettings::ConstPtrs getSwitchSettings() const;

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
    Cable::ConstPtr getCable() const;

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
    explicit Switch(
            Pimpl impl      //!< [in] Pointer to implementation
            );
};

} // namespace bgsched

#endif
