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
#include "server/job/class_route/Parameters.h"

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

Parameters
Parameters::create()
{
    return Parameters();
}

Parameters::Parameters() :
    _world(),
    _coordinates(),
    _np(),
    _jobLeader(),
    _mapping()
{

}

Parameters::~Parameters()
{

}

} // class_route
} // job
} // server
} // runjob
