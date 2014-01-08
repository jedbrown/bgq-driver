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
#ifndef RUNJOB_MUX_CLIENT_PLUGIN_UPDATE_RESOURCES_H_
#define RUNJOB_MUX_CLIENT_PLUGIN_UPDATE_RESOURCES_H_

#include "common/fwd.h"

#include <hlcs/include/bgsched/runjob/Verify.h>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief 
 */
class PluginUpdateResources
{
public:
    /*!
     * \brief ctor.
     */
    PluginUpdateResources(
            JobInfo& job,                           //!< [in]
            const bgsched::runjob::Verify& data     //!< [in]
            );
};

} // client
} // mux
} // runjob

#endif
