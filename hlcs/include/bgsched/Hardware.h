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
 * \file bgsched/Hardware.h
 * \brief Hardware class definition.
 */

#ifndef BGSCHED_HARDWARE_H_
#define BGSCHED_HARDWARE_H_

#include <bgsched/EnumWrapper.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Provides a base class for all types of hardware.
 */
class Hardware
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Hardware> Ptr;

    /*!
     * \brief Const pointer type.
     */
    typedef boost::shared_ptr<const Hardware> ConstPtr;

    /*!
     * \brief Hardware states
     */
    enum State {
         Available,       //!< Hardware is available
         Missing,         //!< Hardware is missing
         Error,           //!< Hardware is in error
         Service,         //!< Hardware is being serviced
         SoftwareFailure  //!< Software failure on hardware resource (only for midplanes and compute nodes)
    };

    /*!
     * \brief Hardware object represented as a human readable string value.
     *
     * \return Hardware object represented as a human readable string value.
     */
    std::string toString() const;

    /*!
     * \brief Get hardware state.
     *
     * \return Hardware state.
     */
    EnumWrapper<State> getState() const;

    /*!
     * \brief Get hardware location.
     *
     * \return Hardware location.
     */
    const std::string& getLocation() const;

    /*!
     * \brief
     */
    virtual ~Hardware() = 0;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Get pointer to implementation.
     *
     * \return Pointer to implementation.
     */
    Pimpl getPimpl() const;

    /*!
     * \brief
     */
    explicit Hardware(
            Pimpl impl  //!< [in] Pointer to implementation
            );

protected:

    Pimpl _impl;

};

} // namespace bgsched

#endif
