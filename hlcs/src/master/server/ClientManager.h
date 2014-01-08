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

#ifndef MASTER_CLIENT_MANAGER_H_
#define MASTER_CLIENT_MANAGER_H_


#include "types.h"

#include <boost/thread.hpp>

#include <vector>


//! \brief ClientManager class maintains a list of all of the BGMaster clients
class ClientManager
{
public:
    ClientManager();

    //! \brief end thread processing of all clients
    void cancel();

    //! \brief Add a new client to the list
    void addClient(ClientControllerPtr c);

    //! \brief Remove a client from the list
    void removeClient(ClientControllerPtr c);
    std::vector<ClientControllerPtr> getClients();
    bool _ending;
private:
    boost::mutex _clientMutex;
    std::vector<ClientControllerPtr> _clients;
};


#endif
