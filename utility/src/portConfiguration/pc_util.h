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

#ifndef BGQ_UTILITY_PC_UTIL_H_
#define BGQ_UTILITY_PC_UTIL_H_


#include <utility/include/portConfiguration/types.h>

#include <boost/shared_ptr.hpp>

#include <string>


namespace bgq {
namespace utility {
namespace pc_util {


/*! \brief Extracts the peer certificate's CN value.
 *
 *  \return The peer's CN or NULL if there's no peer certificate.
 *
 *  \throws runtime_error if there's a peer certificate and fails to get CN.
 */
boost::shared_ptr<std::string> extractPeerCn(
        portConfig::Socket& ssl_stream
    );


}}} // namespace bgq::utility::pc_util


#endif
