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


#include "Transaction.h"

#include "DbChangesMonitor.h"

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {
namespace db2 {


Transaction::Transaction()
    : _max_xact_cond(false)
{
    // Nothing to do
}


void Transaction::change(
        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr,
        DbChangesMonitor& db_changes_monitor,
        unsigned max_transaction_size
    )
{
  if ( _max_xact_cond ) {
    // already know this transaction is too big.
    return;
  }

  // if can merge with the last element, then merge it
  // otherwise just add it at the end.

  if ( _merge( db_change_ptr ) ) {
    // successfully merged
    return;
  }

  if ( _db_changes.size() == max_transaction_size ) {
    LOG_WARN_MSG( "Hit maximum transaction size " << max_transaction_size );
    _max_xact_cond = true;
    _db_changes = DbChanges(); // clear _db_changes
    db_changes_monitor.notifyMaxXactCondition();
    return;
  }

  LOG_TRACE_MSG( "Adding db_change to transaction" );

  _db_changes.push_back( db_change_ptr );
}

void Transaction::commit(
        DbChanges& db_changes,
        DbChangesMonitor& db_changes_monitor
    )
{
  if ( _end( db_changes_monitor ) ) {
    return;
  }

  for ( DbChanges::iterator i(_db_changes.begin()) ; i != _db_changes.end() ; ++i ) {
    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(*i);

    db_changes.push_back( db_change_ptr );
  }
}

void Transaction::rollback(
        DbChangesMonitor& db_changes_monitor
    )
{
  _end( db_changes_monitor );
}

bool Transaction::_end( DbChangesMonitor& db_changes_monitor )
{
  if ( ! _max_xact_cond ) {
    return false;
  }

  db_changes_monitor.notifyMaxXactConditionEnd();

  return true;
}

bool Transaction::_merge( bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr )
{
  if ( _db_changes.empty() ) {
    // can't merge if empty.
    return false;
  }

  // Go through the events in reverse order and try to merge each.

  for ( DbChanges::reverse_iterator i(_db_changes.rbegin()) ; i != _db_changes.rend() ; ++i ) {
    bgsched::realtime::AbstractDatabaseChange &cur_change(**i);

    bool did_merge(cur_change.merge( *db_change_ptr ));
    if ( did_merge )  return true; // merged
  }

  return false; // did not merge.
}


} } } // namespace realtime::server::db2
