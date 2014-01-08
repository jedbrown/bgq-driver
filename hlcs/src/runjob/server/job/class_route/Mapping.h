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
#ifndef RUNJOB_SERVER_JOB_CLASS_ROUTE_MAPPING_H
#define RUNJOB_SERVER_JOB_CLASS_ROUTE_MAPPING_H

#include <string>

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

/*!
 * \brief
 */
class Mapping
{
public:
    /*!
     * \brief
     */
    Mapping();

    /*!
     * \brief
     */
    int* impl() { return _impl; }

private:
    int _impl[5];
};

} // class_route
} // job
} // server
} // runjob

#endif

