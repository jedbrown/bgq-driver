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
#ifndef RUNJOB_CLIENT_OPTIONS_MISCELLANEOUS_H
#define RUNJOB_CLIENT_OPTIONS_MISCELLANEOUS_H

#include "client/options/Description.h"

#include "common/MaximumLengthString.h"

#include <string>

#include <sys/un.h>

namespace runjob {

class JobInfo;

namespace client {
namespace options {

/*!
 * \brief Options controlling miscellaneous behavior.
 * \ingroup argument_parsing
 */
class Miscellaneous : public Description
{
private:
    static const sockaddr_un sockaddr;
    static const size_t SocketSize = sizeof(sockaddr.sun_path);

public:
    /*!
     * \brief ctor.
     */
    Miscellaneous(
            JobInfo& info   //!< [in]
            );

    /*!
     * \brief Local socket string type.
     *
     * The maximum size of this argument is the sockaddr_un.sun_path size minus 1 for the
     * null byte.
     */
    typedef MaximumLengthString<SocketSize - 1> SocketString;
};

} // options
} // client
} // runjob

#endif
