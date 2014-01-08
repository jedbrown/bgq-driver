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
#ifndef RUNJOB_CLIENT_OPTIONS_UNSUPPORTED_H
#define RUNJOB_CLIENT_OPTIONS_UNSUPPORTED_H

#include "common/fwd.h"

namespace runjob {
namespace client {
namespace options {

/*!
 * \brief Flag options are not supported as errors.
 */
class Unsupported
{
public:
    /*!
     * \brief ctor
     */
    explicit Unsupported(
            const JobInfo& info //!< [in]
            );
};

} // options
} // client
} // runjob

#endif
