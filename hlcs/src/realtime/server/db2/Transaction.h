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

#ifndef REALTIME_SERVER_DB2_TRANSACTION_H_
#define REALTIME_SERVER_DB2_TRANSACTION_H_


#include "../types.h"

#include "bgsched/realtime/AbstractDatabaseChange.h"


namespace realtime {
namespace server {
namespace db2 {


class DbChangesMonitor;


/*! \brief A database transaction. */
class Transaction
{
  public:

    Transaction();

    void change(
            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr,
            DbChangesMonitor& db_changes_monitor,
            unsigned max_transaction_size
        );

    void commit(
            DbChanges& db_changes,
            DbChangesMonitor& db_changes_monitor
        );

    void rollback(
            DbChangesMonitor& db_changes_monitor
        );

  private:

    bool _end( DbChangesMonitor& db_changes_monitor );

    bool _merge( bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr );


    bool _max_xact_cond;

    DbChanges _db_changes;
};


} } } // namespace realtime::server::db2


#endif
