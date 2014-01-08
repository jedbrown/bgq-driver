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


#include "Transactions.h"

#include "DbChangesMonitor.h"

#include <utility/include/Log.h>

#include <iostream>

#include <string.h>


using std::ostream;


LOG_DECLARE_FILE( "realtime.server" );


namespace realtime {
namespace server {
namespace db2 {


//---------------------------------------------------------------------
// class tid_t


bool tid_t::operator <( const tid_t& other ) const
{
  return (memcmp( bytes, other.bytes, sizeof ( bytes ) ) == -1);
} // tid_t::operator <()

ostream& operator <<( ostream& os, const tid_t& tid )
{
  for ( unsigned int i(0) ; i < sizeof ( tid.bytes ) ; ++i ) {
    if ( i != 0 )  os << ":";
    os << (int) tid.bytes[i];
  }
  return os;
} // operator<< for tid_t


//---------------------------------------------------------------------
// class Transactions


Transactions::Transactions(
        DbChangesMonitor& db_changes_monitor,
        const Configuration& configuration
    ) :
        _db_changes_monitor(db_changes_monitor),
        _max_transaction_size(configuration.get_max_xact_size())
{
    // Nothing to do.
}


void Transactions::change(
    const tid_t& tid,
    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr
  )
{
    Transaction &transaction(_tmap[tid]);
    transaction.change(
            db_change_ptr,
            _db_changes_monitor,
            _max_transaction_size
        );
}


void Transactions::commit( const tid_t& tid, DbChanges& db_changes )
{
    _TransactionMap::iterator i(_tmap.find( tid ));

    if ( _tmap.end() == i ) {
      LOG_TRACE_MSG( "committing " << tid << " (no changes)" );
      return;
    }

    LOG_TRACE_MSG( "committing " << tid );

    i->second.commit( db_changes, _db_changes_monitor );
    _tmap.erase( i );
}


void Transactions::rollback( const tid_t& tid )
{
    _TransactionMap::iterator i(_tmap.find( tid ));

    if ( _tmap.end() == i ) {
        LOG_TRACE_MSG( "rolling back " << tid << " (no changes)" );
      return;
    }

    LOG_INFO_MSG( "rolling back " << tid );
    i->second.rollback( _db_changes_monitor );
    _tmap.erase( i );
}


void Transactions::_print_map() const
{
    for ( _TransactionMap::const_iterator i(_tmap.begin()) ; i != _tmap.end() ; ++i ) {
        std::cout << ' ' << i->first;
    }
    std::cout << "\n";
}


} } } // namespace realtime::server::db2
