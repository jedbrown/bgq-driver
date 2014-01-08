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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MASTER_TYPES_H_
#define MASTER_TYPES_H_


#include <xml/include/library/XML.h>

#include <log4cxx/log4cxx.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <utility>


class AgentBase;
typedef boost::shared_ptr<AgentBase> AgentBasePtr;

class AgentProtocol;
typedef boost::shared_ptr<AgentProtocol> AgentProtocolPtr;

class BinaryController;
typedef boost::shared_ptr<BinaryController> BinaryControllerPtr;

class ClientProtocol;
typedef boost::shared_ptr<ClientProtocol> ClientProtocolPtr;

class Protocol;
typedef boost::shared_ptr<Protocol> ProtocolPtr;

typedef boost::shared_ptr<XML::Serializable> MsgBasePtr;


#define LOGGING_DECLARE_ID_MDC(value) \
    log4cxx::MDC _location_mdc( "ID", std::string("{") + boost::lexical_cast<std::string>(value) + "} " );


#endif
