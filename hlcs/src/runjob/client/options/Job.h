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
#ifndef RUNJOB_CLIENT_OPTIONS_JOB_H
#define RUNJOB_CLIENT_OPTIONS_JOB_H

#include "client/options/Description.h"
#include "common/PositiveInteger.h"

#include <stdint.h>

namespace runjob {

class JobInfo;

namespace client {
namespace options {

/*!
 * \brief Options controlling job behavior.
 * \ingroup argument_parsing
 */
class Job : public Description
{
public:
    /*!
     * \brief timeout type.
     */
    typedef PositiveInteger<int32_t> Timeout;

public:
    /*!
     * \brief ctor.
     */
    Job(
            JobInfo& info   //!< [in]
       );

private:
    Timeout _timeout;
};

} // options
} // client
} // runjob

#endif
