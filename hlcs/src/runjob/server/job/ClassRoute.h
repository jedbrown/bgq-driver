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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_H

#include "common/Coordinates.h"

#include "server/job/class_route/Coordinates.h"
#include "server/job/class_route/fwd.h"

#include <hwi/include/bqc/classroute.h>

#include <iostream>
#include <stdint.h>

namespace runjob {
namespace server {
namespace job  {

/*!
 * \brief A single node's class route information.
 */
class ClassRoute
{
public:
    /*!
     * \brief ctor.
     */
    ClassRoute(
            class_route::Parameters& parameters
            );

    const class_route::Coordinates& coordinates() const { return _coordinates; }

    ClassRoute_t allNodes() const { return _allNodes; }   //!< Get all nodes interrupt class route.
    
    ClassRoute_t npNodes() const { return _npNodes; } //!< Get np nodes class route.

private:
    ClassRoute_t _allNodes;
    ClassRoute_t _npNodes;
    class_route::Coordinates _coordinates;
};

/*!
 * \brief insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& os,       //!< [in]
        const ClassRoute& cr    //!< [in]
        );

} // job
} // server
} // runjob

#endif
