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

#ifndef BGWS_UTILITY_UTILITY_HPP_
#define BGWS_UTILITY_UTILITY_HPP_


#include <string>
#include <vector>

#include <stddef.h>
#include <stdint.h>


namespace bgws {
namespace utility {


typedef std::vector<uint8_t> Bytes;


//! \brief Fills bytes_out with random bytes.
void getRandomBytes( Bytes& bytes_out );


} // namespace bgws::utility
} // namespace bgws

#endif
