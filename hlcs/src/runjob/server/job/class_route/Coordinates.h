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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_COORDINATES_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_COORDINATES_H

#include "server/job/class_route/Dimension.h"

#include <spi/include/mu/Classroute.h>

#include <boost/shared_ptr.hpp>

#include <iostream>

namespace runjob {
namespace server {
namespace job {
namespace class_route {

/*!
 * \brief
 */
class Coordinates
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Coordinates> Ptr;

public:
    /*!
     * \brief ctor.
     */
    explicit Coordinates(
            const CR_COORD_T* impl = NULL
            );

    /*!
     * \brief ctor.
     */
    Coordinates(
            unsigned a, //!< [in]
            unsigned b, //!< [in]
            unsigned c, //!< [in]
            unsigned d, //!< [in]
            unsigned e  //!< [in]
            );

    /*!
     * \brief
     */
    unsigned get(
            Dimension d
            ) const;

    /*!
     * \brief Get impl.
     */
    const CR_COORD_T& impl() const { return _impl; }

    /*!
     * \brief Get impl.
     */
    CR_COORD_T& impl() { return _impl; }

private:
    CR_COORD_T _impl;
};

/*!
 * \brief equality operator.
 */
bool
operator==(
        const Coordinates& rhs, //!< [in]
        const Coordinates& lhs  //!< [in]
        );

/*!
 * \brief less than operator.
 */
bool
operator<(
        const Coordinates& rhs, //!< [in]
        const Coordinates& lhs  //!< [in]
        );

/*!
 * \brief Insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& os,
        const Coordinates& c
        );

} // class_route
} // job
} // server
} // runjob

#endif
