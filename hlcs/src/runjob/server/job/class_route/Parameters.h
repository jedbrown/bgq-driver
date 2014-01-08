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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_PARAMETERS_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_PARAMETERS_H

#include "server/job/class_route/Coordinates.h"
#include "server/job/class_route/fwd.h"

#include "server/job/fwd.h"

#include "common/Coordinates.h"

#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

/*!
 * \brief
 */
class Parameters
{
public:
    /*!
     * \brief Factory.
     */
    static Parameters create();

    /*!
     * \brief dtor
     */
    ~Parameters();

    Parameters& world( Rectangle* r ) { _world = r; return *this; }
    Parameters& coordinates( const Coordinates& c ) { _coordinates = c; return *this; }
    Parameters& np( Np* np ) { _np = np; return *this; }
    Parameters& jobLeader( Coordinates* c ) { _jobLeader = c; return *this; }
    Parameters& mapping( Mapping* mapping ) { _mapping = mapping; return *this; }

private:
    Parameters();

private:
    friend class runjob::server::job::ClassRoute;

    Rectangle* _world;
    Coordinates _coordinates;
    Np* _np;
    Coordinates* _jobLeader;
    Mapping* _mapping;
};

} // class_route
} // job
} // server
} // runjob

#endif
