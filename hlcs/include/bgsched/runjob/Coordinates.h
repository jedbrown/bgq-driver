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
 * \file bgsched/runjob/Coordinates.h
 * \brief definition and implementation of bgsched::runjob::Coordinates class.
 */

#ifndef BGSCHED_RUNJOB_COORDINATES_H_
#define BGSCHED_RUNJOB_COORDINATES_H_


#include <boost/shared_ptr.hpp>

#include <iosfwd>
#include <limits>

#include <stdint.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief Coordinates in five dimensions.
 */
class Coordinates
{
public:
    /*!
     * \brief ctor.
     */
    Coordinates();

    /*!
     * \brief ctor.
     */
    Coordinates(
            const unsigned coordinates[5]    //!< [in]
            );

    /*!
     * \brief Set A coordinate.
     */
    Coordinates& a(
            unsigned a  //!< [in]
            );

    /*!
     * \brief Set B coordinate.
     */
    Coordinates& b(
            unsigned b  //!< [in]
            );

    /*!
     * \brief Set C coordinate.
     */
    Coordinates& c(
            unsigned c  //!< [in]
            );

    /*!
     * \brief Set D coordinate.
     */
    Coordinates& d(
            unsigned d  //!< [in]
            );

    /*!
     * \brief Set E coordinate.
     */
    Coordinates& e(
            unsigned e  //!< [in]
            );

    bool valid() const; //!< [in] test validity.
    unsigned a() const;
    unsigned b() const;
    unsigned c() const;
    unsigned d() const;
    unsigned e() const;

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

/*!
 * \brief insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& os,               //!< [in]
        const Coordinates& coordinates  //!< [in]
        );

} // runjob
} // bgsched

#endif
