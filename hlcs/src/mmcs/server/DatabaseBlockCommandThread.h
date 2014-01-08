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

#ifndef MMCS_SERVER_DATABASE_BLOCK_COMMAND_THREAD_H_
#define MMCS_SERVER_DATABASE_BLOCK_COMMAND_THREAD_H_

#include "CommandProcessor.h"
#include "BlockDBPollingGovernor.h"

#include "common/Thread.h"

#include <db/include/api/BGQDBlib.h>

namespace mmcs {
namespace server {

/*!
 * \class DatabaseBlockCommandThread
 * \brief This thread gets started by the database monitor to process block transactions
 */
class DatabaseBlockCommandThread : public common::Thread
{
public:
    DatabaseBlockCommandThread() :
        Thread(),
        _creationId( 0 ),
        _action(BGQDB::NO_BLOCK_ACTION),
        _transactions(NULL),
        _commands(NULL)
    {

    }

    void* threadStart();
    std::string _name;
    std::string _userName;
    unsigned int _creationId;
    BGQDB::BLOCK_ACTION _action;
    BlockDBPollingGovernor* _transactions;
    MMCSCommandMap* _commands;
};

} } // namespace mmcs::server

#endif
