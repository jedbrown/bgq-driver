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
#ifndef RUNJOB_CLIENT_OPTIONS_RESOURCE_H
#define RUNJOB_CLIENT_OPTIONS_RESOURCE_H

#include "client/options/Description.h"

namespace runjob {

class JobInfo;

namespace client {
namespace options {

/*!
 * \brief Options controlling resource behavior.
 * \ingroup argument_parsing
 */
class Resource : public Description
{
public:
    /*!
     * \brief ctor.
     */
    Resource(
            JobInfo& info   //!< [in]
            );
};

} // options
} // client
} // runjob

#endif
