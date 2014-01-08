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


#ifndef REALTIME_SERVER_DB2_TRANSACTIONS_H_
#define REALTIME_SERVER_DB2_TRANSACTIONS_H_


#include "Transaction.h"

#include "../Configuration.h"
#include "../types.h"

#include "bgsched/realtime/AbstractDatabaseChange.h"

#include <iosfwd>
#include <map>


namespace realtime {
namespace server {
namespace db2 {


class DbChangesMonitor;


struct tid_t {
    unsigned char bytes[6];

    bool operator <( const tid_t& other ) const;
};

std::ostream& operator <<( std::ostream& os, const tid_t& tid );


/*! \brief Current database transactions that are being tracked.
 */
class Transactions
{
public:

    Transactions(
            DbChangesMonitor& db_changes_monitor, //!< [ref]
            const Configuration& configuration //!< [in]
        );

    void change(
            const tid_t& tid,
            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr
        );

    void commit(
            const tid_t& tid,
            DbChanges& db_changes
        );

    void rollback( const tid_t& tid );


private:

    typedef std::map<tid_t, Transaction> _TransactionMap;


    DbChangesMonitor& _db_changes_monitor;
    unsigned _max_transaction_size;

    _TransactionMap _tmap;


    void _print_map() const;
};


} } } // namespace realtime::server::db2


#endif
