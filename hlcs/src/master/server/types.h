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

#ifndef MASTER_SERVER_TYPES_H_
#define MASTER_SERVER_TYPES_H_

#include "common/types.h"

#include <boost/shared_ptr.hpp>

#include <utility>

class AgentManager;
typedef boost::shared_ptr<AgentManager> AgentManagerPtr;

class AgentRep;
typedef boost::shared_ptr<AgentRep> AgentRepPtr;
typedef std::pair<BinaryControllerPtr, AgentRepPtr> BinaryLocation;


class Alias;
typedef boost::shared_ptr<Alias> AliasPtr;

class AliasList;

class Behavior;
typedef boost::shared_ptr<Behavior> BehaviorPtr;

class ClientController;
typedef boost::shared_ptr<ClientController> ClientControllerPtr;

class ClientManager;
typedef boost::shared_ptr<ClientManager> ClientManagerPtr;

class Policy;

class Registrar;

#endif
