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
#ifndef RUNJOB_CLIENT_OPTIONS_DEBUG_H
#define RUNJOB_CLIENT_OPTIONS_DEBUG_H

#include "common/tool/Daemon.h"

#include "common/fwd.h"

#include "client/options/Description.h"

namespace runjob {
namespace client {
namespace options {

/*!
 * \brief Options controlling various debug behaviors.
 * \ingroup argument_parsing
 */
class Debug : public Description
{
public:
    /*!
     * \brief ctor.
     */
    Debug(
            JobInfo& info,      //!< [in]
            tool::Daemon& tool  //!< [in]
         );

};

} // options
} // client
} // runjob

#endif
