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

#ifndef REALTIME_SERVER_DB2_DBCHANGESMONITOR_H_
#define REALTIME_SERVER_DB2_DBCHANGESMONITOR_H_


#include "TableInfos.h"
#include "Transactions.h"

#include "../DbChangesMonitor.h"
#include "../types.h"

#include <utility/include/Inotify.h>

#include <sqlcli.h>
#include <db2ApiDf.h>

#include <iosfwd>
#include <set>
#include <string>
#include <vector>


namespace realtime {
namespace server {


class Configuration;


/*! \brief Parts specific to monitoring DB2.
 *
 */
namespace db2 {


/*! \brief Checks for changes to the database and passes them on to clients.
 *
 */
class DbChangesMonitor : public server::DbChangesMonitor
{
public:

    DbChangesMonitor(
            boost::asio::io_service& io,
            const Configuration& config
        );

    void addClient(
            Client::Ptr client_ptr
        );

    void removeClient( Client::Ptr client_ptr );

    void notifyMaxXactCondition();

    void notifyMaxXactConditionEnd();


private:

    typedef std::set<Client::Ptr> _Clients;

    static const unsigned int _LOG_BUF_SIZE;


    const Configuration& _config;

    boost::asio::io_service::strand _strand;

    TableInfos _tables_info;
    Transactions _transactions;

    bgq::utility::Inotify _inotify;
    bgq::utility::Inotify::Watch _watch;
    bgq::utility::Inotify::Events _events;

    bool _max_transact_cond;

    _Clients _clients;

#ifdef DB2READLOG_LRI_1
    db2LRI _cur_lsn;
#else
    db2LSN _cur_lsn;
#endif

    std::vector<char> _log_buf;


    void _addClientImpl( Client::Ptr client_ptr );

    void _removeClientImpl( Client::Ptr client_ptr );

    void _startMonitoringLogDir();

    std::string _getLogPath( const std::string& db_name );

#ifdef DB2READLOG_LRI_1
    db2LRI _initCurLsn( const Configuration& configuration );
#else
    db2LSN _initCurLsn( const Configuration& configuration );
#endif

    void _processChanges();

    void _processLogBuffer(
            db2Uint32 log_rec_count,
            DbChanges& db_changes
        );

    void _processLogRecord(
            const char* record_p,
            DbChanges& db_changes
        );

    void _handleDmRecord( const char* dm_header_p, const tid_t& tid );

    void _startInotifyRead();

    void _gotEvents(
            const boost::system::error_code& err
        );
};


/*! \page dbm Database monitor (DBM)

The DBM monitors the
DB2 DB xact logs using Linux's inotify and DB2 APIs.

-# The DBM must have authority to read the
DB2 xact logs,
if it doesn't it will fail.
-# The DB2 APIs used by the DBM
require that the database be configured
with archive logging,
which means that the LOGARCHMETH1
configuration option on the DB
must not be OFF.
If LOGARCHMETH1 is OFF, the DBM will fail.
-# The DB2 APIs only get xact log
info for a table when the table is set
to have DATA CAPTURE CHANGES.
If a table that the DBM
is monitoring is not configured with
DATA CAPTURE CHANGES,
clients won't get notifications for that table.

The DBM first queries the DB to get
info about the tables it monitors
from the catalog.
To do this, it uses the DB connection
configuration information.
If it can't perform these queries,
because the DB is down or the
database user name or password are
incorrect it will fail.

Note:
The DBM doesn't start
monitoring the DB2 xact logs until the first client
connects.
So if it doesn't have authority to read the
DB2 xact logs it will not fail until the first client connects.

The DBM monitors the following tables:
- TBGQBLOCK
- TBGQJOB
- TBGQMIDPLANE
- TBGQNODECARD
- TBGQNODE
- TBGQSWITCH
- TBGQCABLE
- TBGQEVENTLOG

\section maxtransact Large-transaction mode

When a large transaction is being processed by the DBM,
the DBM goes into <i>large-transaction</i> mode.
The number of operations required in a transaction
to put the DBM into large-transaction mode
is configurable (\ref serverConfiguration).

One way that a large transaction could occur
is if someone did
<pre>DELETE * FROM tbgqBlock WHERE blockId LIKE 'TEST%'</pre>
and there are more than <var>x</var> blocks,
or use
<pre>%db2 +c "INSERT INTO tbgqeventlog ( msg_type ) VALUES( 'test' )"</pre>
<var>x</var> times.
Eventually, the DBM will enter large transaction mode.
To end the transaction and have the DBM return to normal mode, run
<pre>%db2 "COMMIT"</pre>

\note
It's not if any transaction in the DB is over the max size,
it's only changes in the monitored tables.
So environmental monitoring won't cause this to occur.

When the DBM goes into large-transaction mode,
any connected clients are disconnected from the DBM.

Once the large transaction is committed or rolled back,
the DBM returns to normal operating mode.

Clients will be re-connected to the
DBM returns to normal operating mode.


 */

} // namespace realtime::server::db2
} // namespace realtime::server
} // namespace realtime


#endif
