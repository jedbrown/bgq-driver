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

#ifndef BGWS_BLUE_GENE_SERVICE_ACTIONS_FWD_HPP_
#define BGWS_BLUE_GENE_SERVICE_ACTIONS_FWD_HPP_


#include <boost/function.hpp>

#include <exception>
#include <map>
#include <string>


namespace bgws {
namespace blue_gene {
namespace service_actions {


class ServiceActions;


    // If error, ex_ptr will be one of InvalidLocationError, HardwareDoesntExistError, NoIdProvidedError, or other exception.
    // Otherwise ex_ptr is null and service_action_id is set.
typedef boost::function<void ( std::exception_ptr ex_ptr, const std::string& service_action_id )> StartCb;


typedef boost::function<void ( const std::string& error_message )> EndCb;

typedef boost::function<void ( std::exception_ptr ex_ptr )> CloseCb;


    // Map of service action ID to attention messages.
typedef std::map<std::string,std::string> AttentionMessagesMap;


} } } // namespace bgws::blue_gene::service_actions


#endif
