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
 * \file bgsched/IOHardware.h
 * \brief IOHardware class definition.
 */

#ifndef BGSCHED_IO_HARDWARE_H_
#define BGSCHED_IO_HARDWARE_H_

#include <bgsched/EnumWrapper.h>
#include <bgsched/IODrawer.h>

#include <boost/shared_ptr.hpp>

#include <string>

namespace bgsched {

/*!
 * \brief Represents the physical I/O hardware and its state.
 *
 * This object provides methods to query I/O hardware configuration and state.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 *
 */
class IOHardware
{
public:

    typedef boost::shared_ptr<IOHardware> Ptr;             //!< Pointer type.
    typedef boost::shared_ptr<const IOHardware> ConstPtr;  //!< Const pointer type.

    /*!
     * \brief Get a list of all the I/O drawers.
     *
     * \return List of I/O drawers.
     */
    IODrawer::ConstPtrs getIODrawers() const;

    /*!
     * \brief Get the I/O drawer at the location given.
     *
     * \throws bgsched::InputException with value:
     * - bgsched::InputErrors::InvalidLocationString - if the I/O drawer location is not found
     *
     * \return I/O drawer at the location given.
     */
    IODrawer::ConstPtr getIODrawer(
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
    IONode::ConstPtr getIONode(
            const std::string& location //!< [in] I/O node location (e.g. R00-ID-J04)
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
    explicit IOHardware(
            Pimpl impl //!< [in] Pointer to implementation
    );

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
