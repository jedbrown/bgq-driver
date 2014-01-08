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

#ifndef BGWS_TEAL_FWD_HPP_
#define BGWS_TEAL_FWD_HPP_


#include <boost/function.hpp>

#include <cstdint>
#include <exception>


namespace bgws {
namespace teal {


typedef std::int64_t Id;
typedef std::int32_t State;


/*! \brief Callback for close or remove alert.
 *  \param exc_ptr one of errors::NotFound, errors::InvalidState, errors::Duplicate (only on close alert) or other exception.
 */
typedef boost::function<void ( std::exception_ptr exc_ptr )> CloseRemoveCallbackFn;


class Teal;


} } // namespace bgws::teal


#endif
