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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
 * \file bgsched/IOHardwareImpl.h
 * \brief IOHardware::Impl class definition.
 */

#ifndef BGSCHED_IO_HARDWARE_IMPL_H_
#define BGSCHED_IO_HARDWARE_IMPL_H_

#include <bgsched/IOHardware.h>

#include <map>
#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Represents the physical Blue Gene I/O hardware and its state.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 *
 */
class IOHardware::Impl
{
public:

    /*!
     * \brief Initialize the I/O hardware structure for this machine.
     *
     * \throws bgsched::DatabaseException with values:
     * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
     * - bgsched::DatabaseErrors::OperationFailed - if error occurs accessing the database
     *
     * \throws bgsched::InternalException with values:
     * - bgsched::InternalErrors::UnexpectedError - if unexpected error occurs
     * - bgsched::InternalErrors::InconsistentDataError - if inconsistent data found in the database
     */
    Impl();

    /*!
     * \brief Copy ctor.
     */
    Impl(
            const IOHardware::Pimpl fromIOHardware //!< [in] From IOHardware
    );

    /*!
     * \brief Get a list of all the I/O drawers.
     *
     * \return List of I/O drawers.
     */
    const std::vector<IODrawer::Pimpl>& getIODrawers() const;

    /*!
     * \brief Get the I/O drawer at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if the I/O drawer location is not found
     *
     * \return I/O drawer at the location given.
     */
    IODrawer::Pimpl getIODrawer(
            const std::string& location //!< [in] I/O drawer location (e.g. R00-ID)
    ) const;

    /*!
     * \brief Get the I/O node at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if the I/O node location is not found
     *
     * \return I/O node at the location given.
     */
    IONode::Pimpl getIONode(
            const std::string& location //!< [in] I/O drawer location (e.g. R00-ID-J04)
    ) const;

    /*!
     * \brief Get the state of the hardware (I/O drawer or I/O node) at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if hardware at the location is not found
     *
     * \return State of hardware at given location.
     */
    EnumWrapper<Hardware::State> getState(
            const std::string& location      //!< [in] Hardware location
    ) const;

protected:

    typedef std::map<std::string, IODrawer::Pimpl> IODrawerMap; //!< Collection of I/O drawers.

protected:

    std::vector<IODrawer::Pimpl> _IODrawers;     //!< Container of I/O drawers
    IODrawerMap                  _IODrawerMap;   //!< Map the location string to the I/O drawer

};

} // namespace bgsched

#endif
