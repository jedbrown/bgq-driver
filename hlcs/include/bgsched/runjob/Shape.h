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
 * \file bgsched/runjob/Shape.h
 * \brief definition and implementation of bgsched::runjob::Shape class.
 */

#ifndef BGSCHED_RUNJOB_SHAPE_H_
#define BGSCHED_RUNJOB_SHAPE_H_

#include <string>

#include <boost/shared_ptr.hpp>

namespace bgsched {
namespace runjob {

/*!
 * \brief Sub-Block job shape.
 *
 * The minimum job shape is 1x1x1x1x1 in the AxBxCxDxE dimensions. The maximum
 * job shape is 4x4x4x4x2.
 *
 * \note the shape of the job has the following limitations
 *
 * - must exist within the corner's block
 * - must exist within the corner's midplane
 * - cannot wrap around torus dimensions
 * - all dimensions must be a power of two: 1, 2, or 4 nodes
 *
 * \see Corner
 * \see Verify
 */
class Shape
{
public:
    /*!
     * \brief ctor.
     */
    Shape();

    /*!
     * \brief ctor.
     *
     * \note sets the core to 16
     */
    Shape(
            const unsigned shape[5] //!< [in]
         );

    /*!
     * \brief Set shape.
     */
    Shape& value(
            const std::string& value    //!< [in]
            );

    /*!
     * \brief Set A size.
     *
     * \note sets the core to 16
     *
     * \throws std::logic error if a == 0 or a > 4
     */
    Shape& a(
            unsigned a  //!< [in]
            );

    /*!
     * \brief Set B size.
     *
     * \note sets the core to 16
     *
     * \throws std::logic error if b == 0 or c > 4
     */
    Shape& b(
            unsigned b  //!< [in]
            );

    /*!
     * \brief Set C size.
     *
     * \note sets the core to 16
     *
     * \throws std::logic error if c == 0 or c > 4
     */
    Shape& c(
            unsigned c  //!< [in]
            );

    /*!
     * \brief Set D size.
     *
     * \note sets the core to 16
     *
     * \throws std::logic error if d == 0 or d > 4
     */
    Shape& d(
            unsigned d  //!< [in]
            );

    /*!
     * \brief Set E size.
     *
     * \note sets the core to 16
     *
     * \throws std::logic error if e == 0 or e > 2
     */
    Shape& e(
            unsigned e  //!< [in]
            );

    /*!
     * \brief Set core number.
     *
     * \note sets the shape to 1x1x1x1x1
     *
     * \throws std::logic error if core > 15
     */
    Shape& core(
            unsigned core   //!< [in]
            );

    /*!
     * \brief
     */
    bool valid() const;

    const std::string& value() const;

    unsigned a() const;   //!< Get A size.
    unsigned b() const;   //!< Get B size.
    unsigned c() const;   //!< Get C size.
    unsigned d() const;   //!< Get D size.
    unsigned e() const;   //!< Get E size.
    unsigned core() const;  //!< Get core number.

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif
