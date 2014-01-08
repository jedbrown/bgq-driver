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
 * \file bgsched/allocator/Midplanes.h
 * \brief Midplanes class definition.
 */

#ifndef BGSCHED_ALLOCATOR_MIDPLANES_H_
#define BGSCHED_ALLOCATOR_MIDPLANES_H_

#include <bgsched/Midplane.h>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace bgsched {

namespace allocator {

/*!
 * \brief Represents a set of midplanes.
 */
class Midplanes : boost::noncopyable
{
public:

    typedef boost::shared_ptr<Midplanes> Ptr;                  //!< Pointer type.
    typedef boost::shared_ptr<const Midplanes> ConstPtr;       //!< Const pointer type.

    /*!
     * \brief Get midplane at specific coordinate.
     *
     * \throws bgsched::allocator::InputException with value:
     * - bgsched::allocator::InputErrors::InvalidMidplaneCoordinates - if coordinates are invalid
     *
     * \return Midplane at specific coordinate.
     */
    Midplane::ConstPtr get(
            const bgsched::Coordinates& coordinates  //!< [in] Coordinates of midplane to retrieve
            ) const;

    /*!
     * \brief Get the machine size in midplanes for each dimension.
     *
     * \return Machine size in midplanes for each dimension.
     */
    const bgsched::Coordinates getMachineSize() const;

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
    explicit Midplanes(
            Pimpl impl      //!< [in] Pointer to implementation
            );

    /*!
     * \brief
     */
    ~Midplanes();

protected:

    Pimpl _impl;

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
