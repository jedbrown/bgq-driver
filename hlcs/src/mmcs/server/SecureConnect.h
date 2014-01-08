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

#ifndef MMCS_SERVER_SECURE_CONNECT_H_
#define MMCS_SERVER_SECURE_CONNECT_H_

#include <utility/include/cxxsockets/types.h>
#include <utility/include/portConfiguration/PortConfiguration.h>

namespace mmcs {
namespace server {

namespace SecureConnect {

/*!
 * \brief Connect to a host.
 *
 * \throws std::runtime_error if the connection could not be established
 */
void
Connect(
        const bgq::utility::PortConfiguration::Pairs& portpairs,  //!< [in]
        CxxSockets::SecureTCPSocketPtr& socket  //!< [out]
        );

}

} } // namespace mmcs::server

#endif
