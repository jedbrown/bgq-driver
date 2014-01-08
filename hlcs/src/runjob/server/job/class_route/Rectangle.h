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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_RECTANGLE_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_RECTANGLE_H

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
class Rectangle
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Rectangle> Ptr;

public:
    /*!
     * \brief ctor.
     */
    explicit Rectangle(
            const CR_RECT_T* impl = NULL
            );

    /*!
     * \brief Get the number of nodes in the rectangle.
     */
    unsigned size() const { return _size; }

    /*!
     * \brief
     */
    unsigned size(
            Dimension d
            ) const;

    /*!
     * \brief Get impl.
     */
    const CR_RECT_T& impl() const { return _impl; }

    /*!
     * \brief Get impl.
     */
    CR_RECT_T& impl() { return _impl; }

private:
    CR_RECT_T _impl;
    unsigned _size;
};

/*!
 * \brief Insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& os,
        const Rectangle& rectangle
        );

} // class_route
} // job
} // server
} // runjob
#endif
