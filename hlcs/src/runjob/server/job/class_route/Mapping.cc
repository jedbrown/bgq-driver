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
#include "server/job/class_route/Mapping.h"

#include "common/logging.h"

#include <spi/include/mu/Classroute_inlines.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job  {
namespace class_route {

Mapping::Mapping() :
    _impl()
{
    // default mapping order for system class routes is ABCDE, but we
    // reverse that to EDCBA for PAMI
    char buf[6];
    char* s = buf;
    for ( unsigned x = 1; x <= CR_NUM_DIMS; ++x ) {
        _impl[ x - 1 ] = CR_NUM_DIMS - x;
        *s++ = CR_DIM_NAMES[ CR_NUM_DIMS - x ];
    }

    buf[5] = '\0';
    LOG_TRACE_MSG( buf );
}

} // class_route
} // job
} // server
} // runjob
