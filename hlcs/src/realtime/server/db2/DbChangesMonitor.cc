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


#include "DbChangesMonitor.h"

#include "../Configuration.h"
#include "../log_util.h"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

#include <sqladef.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <unistd.h>


using boost::bind;

using std::cout;
using std::dec;
using std::endl;
using std::ios;
using std::ios_base;
using std::ostream;
using std::ostringstream;
using std::string;
using std::vector;


LOG_DECLARE_FILE( "realtime.server" );


const unsigned LogRecordTypeOffset = 4;
const unsigned LogRecordGeneralFlagOffset = 6;
const unsigned LogRecordTidOffset = 16;
#ifdef DB2READLOG_LRI_1
const unsigned LogRecordNonCompensationHeaderSize = 40;
#else
const unsigned LogRecordNonCompensationHeaderSize = 24;
  // I don't know why this has to be 24 since the doc says it should be 22.
#endif


//-------------------------------------------------------------------------
// Some handy output functions to nicely display structs

static ostream& operator<<( ostream& os, const struct sqlca& sql_ca )
{

  os << "{sqlca: sqlcode=" << sql_ca.sqlcode;
  if ( sql_ca.sqlcode < 0 ) {
    vector<char> errmsg_txt( sql_ca.sqlerrml + 1 );
    memcpy( &errmsg_txt[0], sql_ca.sqlerrmc, sql_ca.sqlerrml );
    errmsg_txt[sql_ca.sqlerrml] = '\0';

    os << " sqlerrmc='" << &errmsg_txt[0] << "'";
  }
  os << "}";

  return os;
} // operator<<( sqlca )


#ifdef DB2READLOG_LRI_1

static ostream& operator <<( ostream& os, const db2LRI& lri )
{
    os << lri.lriType << ":" << lri.part1 << "/" << lri.part2;
    return os;
}

static ostream& operator<<( ostream& os, const db2ReadLogInfoStruct& rli )
{
    os << "\n"
          "\tinitialLSN=" << rli.initialLRI << "\n"
          "\tfirstReadLSN=" << rli.firstReadLRI << "\n"
          "\tnextStartLSN=" << rli.nextStartLRI << "\n"
          "\tlogRecsWritten=" << rli.logRecsWritten << "\n"
          "\tlogBytesWritten=" << rli.logBytesWritten << "\n"
          "\tfirstReusedLSN=" << rli.firstReusedLRI << "\n";
    return os;
}

#else

static ostream& operator <<( ostream& os, const db2LSN& lsn )
{
  for ( unsigned int i(0) ; i < sizeof ( lsn.lsnU64 ) ; ++i ) {
    if ( i != 0 ) {
      os << ":";
    }
    os << (unsigned) ((reinterpret_cast<const uint8_t*>(&lsn.lsnU64))[i]);
  }
  return os;
} // operator<<(db2LSN)


static ostream& operator<<( ostream& os, const db2ReadLogInfoStruct& rli )
{
    os << "\n"
          "\tinitialLSN=" << rli.initialLSN << "\n"
          "\tfirstReadLSN=" << rli.firstReadLSN << "\n"
          "\tnextStartLSN=" << rli.nextStartLSN << "\n"
          "\tlogRecsWritten=" << rli.logRecsWritten << "\n"
          "\tlogBytesWritten=" << rli.logBytesWritten << "\n"
          "\tfirstReusedLSN=" << rli.firstReusedLSN << "\n";
    return os;
}

#endif


namespace realtime {
namespace server {
namespace db2 {


//-------------------------------------------------------------------------
// stuff to set up DB2 current connection

/*
 * I got these members by building a small embedded SQL program that did
 * EXEC SQL CONNECT TO BGDB0;
 *
 * I needed to do this because db2ReadLog() uses the "current connection" and this is how
 * to initialize the current connection.
 */

#ifdef USE_ALTERNATIVE_SQLA
static char sqla_program_id[162] =
{
  42,0,65,68,65,76,65,73,83,65,77,80,76,69,32,32,66,66,68,121,
  74,88,72,89,48,49,49,49,49,32,50,32,8,0,66,71,80,83,89,83,
  68,66,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0
};
#else
static char sqla_program_id[162] =
{
 0,42,68,65,76,65,73,65,83,65,77,80,76,69,32,32,72,66,97,89,
 76,83,72,89,48,49,49,49,49,32,50,32,0,8,66,71,80,83,89,83,
 68,66,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 0,0
};
#endif

#include "sqladef.h"

static struct sqla_runtime_info sqla_rtinfo =
{{'S','Q','L','A','R','T','I','N'}, sizeof(wchar_t), 0, {' ',' ',' ',' '}};


static const short sqlIsLiteral   = SQL_IS_LITERAL;
static const short sqlIsInputHvar = SQL_IS_INPUT_HVAR;


//-------------------------------------------------------------------------
// class RID used to display records (from samples)


class RID
{
  private:
    char ridParts[6];
    char ridString[14];

    void toString();

  public:
    int size() { return 6; };
    void set(const char * buf );
    char *getString();
};

void RID::toString()
{
  char *ptrBuf = this->ridParts;

  sprintf( ridString, "x%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
           *ptrBuf, *(ptrBuf+1), *(ptrBuf+2),
           *(ptrBuf+3), *(ptrBuf+4), *(ptrBuf+5) );
}

void RID::set( const char *buf )
{
  strncpy( this->ridParts, buf, this->size() );
}

char* RID::getString()
{
  this->toString();
  return ridString;
}

const unsigned int DbChangesMonitor::_LOG_BUF_SIZE(64 * 1024 * 1024);


DbChangesMonitor::DbChangesMonitor(
        boost::asio::io_service& io,
        const Configuration& config
    ) :
        _config(config),
        _strand( io ),
        _tables_info( _config ),
        _transactions(
                *this,
                _config
            ),
        _inotify( io ),
        _max_transact_cond(false),
        _log_buf( _LOG_BUF_SIZE )
{
    // Nothing to do.
}


void DbChangesMonitor::addClient(
        Client::Ptr client_ptr
    )
{
    _strand.post( boost::bind( &DbChangesMonitor::_addClientImpl, this, client_ptr ) );
}


void DbChangesMonitor::removeClient( Client::Ptr client_ptr )
{
    _strand.post( boost::bind( &DbChangesMonitor::_removeClientImpl, this, client_ptr ) );
}


void DbChangesMonitor::notifyMaxXactCondition()
{
    _max_transact_cond = true;

    // Need to disconnect all clients.
    std::for_each( _clients.begin(), _clients.end(), boost::bind( &Client::notifyConnectedToDbMonitor, _1, false ) );

    _setState( State::MaxXact );
}


void DbChangesMonitor::notifyMaxXactConditionEnd()
{
    _max_transact_cond = false;

    // Tell all clients reconnected.
    std::for_each( _clients.begin(), _clients.end(), boost::bind( &Client::notifyConnectedToDbMonitor, _1, true ) );

    _setState( State::Monitoring );
}


void DbChangesMonitor::_addClientImpl( Client::Ptr client_ptr )
{
    try {
        _clients.insert( client_ptr );

        LOG_DEBUG_MSG( "Added client, now have " << _clients.size() );

        if ( _max_transact_cond ) {
            // Client will be notified connected when max transact cond is over.
            return;
        }

        if ( ! _watch ) {
            LOG_INFO_MSG( "Start monitoring because a client connected." );

            _startMonitoringLogDir();
        }

        client_ptr->notifyConnectedToDbMonitor( true );

    } catch ( std::exception& e ) {
        LOG_WARN_MSG( "Client rejected because an error occured. The error is " << e.what() );
        client_ptr->notifyConnectedToDbMonitor( false );
        throw;
    }
}


void DbChangesMonitor::_removeClientImpl( Client::Ptr client_ptr )
{
    _clients.erase( client_ptr );

    LOG_DEBUG_MSG( "Removed client, now have " << _clients.size() );
}


void DbChangesMonitor::_startMonitoringLogDir()
{
    if ( _watch ) {
        LOG_TRACE_MSG( "already monitoring" );
        return;
    }

    const string db_transaction_log_dir(_getLogPath(_config.get_database_name()));

    _watch = _inotify.watch(
            db_transaction_log_dir,
            IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO
        );

    _cur_lsn = _initCurLsn( _config );

    _startInotifyRead();

    LOG_INFO_MSG( "Monitoring the database transaction log directory." );

    _setState( State::Monitoring );
}


std::string DbChangesMonitor::_getLogPath( const std::string& db_name )
{
    char log_path[SQL_PATH_SZ + 1] = { 0 };

    db2CfgParam cfg_parameters[1];
    memset( cfg_parameters, '\0', sizeof ( cfg_parameters ) );

    cfg_parameters[0].flags = 0;
    cfg_parameters[0].token = SQLF_DBTN_LOGPATH;
    cfg_parameters[0].ptrvalue = log_path;

    db2Cfg cfg_struct;
    memset( &cfg_struct, '\0', sizeof ( cfg_struct ) );

    cfg_struct.numItems = 1;
    cfg_struct.paramArray = cfg_parameters;
    cfg_struct.flags = db2CfgDatabase;
    cfg_struct.dbname = const_cast<char*>(db_name.c_str());

    struct sqlca sql_ca;
    SQL_API_RC sql_rc(db2CfgGet( db2Version970, &cfg_struct, &sql_ca ));

    LOG_INFO_MSG( "db2CfgGet sql_rc=" << sql_rc << " sqlcode=" << sql_ca.sqlcode );
    if ( (sql_rc != SQL_SUCCESS) || (sql_ca.sqlcode != 0) ) {
    THROW_RUNTIME_ERROR_EXCEPTION( "failed to get log path for DB " << db_name );
    }

    LOG_INFO_MSG( "log path for '" << db_name << "' is '" << log_path << "'" );
    return string(log_path);
}


#ifdef DB2READLOG_LRI_1
db2LRI DbChangesMonitor::_initCurLsn( const Configuration& configuration )
#else
db2LSN DbChangesMonitor::_initCurLsn( const Configuration& configuration )
#endif
{
    // Use db2ReadLog(QUERY), db2ReadLog(READ)+ to get to the end of the log

    const string &db_name(configuration.get_database_name());

    struct sqlca sql_ca;

    {
      /*
      EXEC SQL CONNECT TO BGDB0;
      */

      sqlastrt(sqla_program_id, &sqla_rtinfo, &sql_ca);
      sqlaaloc(2,1,1,0L);
      {
        struct sqla_setdata_list sql_setdlist[1];
        sql_setdlist[0].sqltype = 460; sql_setdlist[0].sqllen = db_name.size() + 1;
        sql_setdlist[0].sqldata = (void*) db_name.c_str();
        sql_setdlist[0].sqlind = 0L;
        sqlasetdata(2,0,1,sql_setdlist,0L,0L);
      }
      sqlacall((unsigned short)29,4,2,0,0L);
      sqlastop(0L);
    }

    if ( sql_ca.sqlcode < 0 ) {
      THROW_RUNTIME_ERROR_EXCEPTION( "Failed to connect. SQLCODE=" << sql_ca.sqlcode );
    }

    SQL_API_RC sql_rc;

#ifdef DB2READLOG_LRI_1
    db2LRI cur_lsn;
#else
    db2LSN cur_lsn;
#endif

    {
      struct db2ReadLogStruct read_log_input;
      memset( &read_log_input, '\0', sizeof ( read_log_input ) );
      read_log_input.iCallerAction = DB2READLOG_QUERY;
      read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

      struct db2ReadLogInfoStruct read_log_info;
      memset( &read_log_info, sizeof ( read_log_info ), '\0' );

      read_log_input.poReadLogInfo = &read_log_info;

#ifdef DB2READLOG_LRI_1
      sql_rc = db2ReadLog( db2Version1010, &read_log_input, &sql_ca );
#else
      sql_rc = db2ReadLog( db2Version970, &read_log_input, &sql_ca );
#endif

      LOG_INFO_MSG( "db2ReadLog QUERY sql_rc=" << sql_rc << " sql_ca=" << sql_ca );

      if ( (sql_rc != SQL_SUCCESS) || (sql_ca.sqlcode < 0) ) {
        ostringstream msg;
        msg << "db2ReadLog QUERY failed with SQLCODE=" << sql_ca.sqlcode;
        if ( sql_ca.sqlcode == -2651 ) {
          msg << ". This error indicates that the database isn't configured to retain log files"
              " (LOGARCHMETH1 must not be OFF).";
        }
        THROW_RUNTIME_ERROR_EXCEPTION( msg.str() );
      }

      if ( sql_ca.sqlcode > 0 ) {
        LOG_INFO_MSG( "db2ReadLog QUERY indicates warning; sql_ca=" << sql_ca );
      }

      LOG_INFO_MSG( "read_log_info: " << read_log_info );

#ifdef DB2READLOG_LRI_1
      cur_lsn = read_log_info.nextStartLRI;
#else
      cur_lsn = read_log_info.nextStartLSN;
#endif

    }

    {
#ifdef DB2READLOG_LRI_1
        db2LRI end_lsn;
        memset( &end_lsn, -1, sizeof ( end_lsn ) );
        end_lsn.lriType = cur_lsn.lriType;
#else
        db2LSN end_lsn;
        end_lsn.lsnU64 = db2Uint64(-1);
#endif

      while ( true ) {
        struct db2ReadLogStruct read_log_input;
        memset( &read_log_input, '\0', sizeof ( read_log_input ) );
        read_log_input.iCallerAction = DB2READLOG_READ;
#ifdef DB2READLOG_LRI_1
        read_log_input.piStartLRI = &cur_lsn;
        read_log_input.piEndLRI = &end_lsn;
#else
        read_log_input.piStartLSN = &cur_lsn;
        read_log_input.piEndLSN = &end_lsn;
#endif
        read_log_input.poLogBuffer = &_log_buf[0];
        read_log_input.iLogBufferSize = _log_buf.size();
        read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

        struct db2ReadLogInfoStruct read_log_info;
        memset( &read_log_info, sizeof ( read_log_info ), '\0' );

        read_log_input.poReadLogInfo = &read_log_info;

#ifdef DB2READLOG_LRI_1
        sql_rc = db2ReadLog( db2Version1010, &read_log_input, &sql_ca );
#else
        sql_rc = db2ReadLog( db2Version970, &read_log_input, &sql_ca );
#endif

        LOG_INFO_MSG( "db2ReadLog READ sql_rc=" << sql_rc << " sql_ca=" << sql_ca );

        if ( (sql_rc != SQL_SUCCESS) || (sql_ca.sqlcode < 0) ) {
          THROW_RUNTIME_ERROR_EXCEPTION( "db2ReadLog READ failed" );
        }

        LOG_INFO_MSG( "read_log_info:" << read_log_info );

#ifdef DB2READLOG_LRI_1
        cur_lsn = read_log_info.nextStartLRI;
#else
        cur_lsn = read_log_info.nextStartLSN;
#endif

        if ( SQLU_RLOG_READ_TO_CURRENT == sql_ca.sqlcode ) {
          LOG_INFO_MSG( "read to current" );
          break;
        }
        if ( sql_ca.sqlcode > 0 ) {
          LOG_INFO_MSG( "db2ReadLog READ indicates warning; sql_ca=" << sql_ca );
        }
      }
    }

    LOG_INFO_MSG( "cur_lsn=" << cur_lsn );

    return cur_lsn;
}


void DbChangesMonitor::_processChanges()
{
  LOG_TRACE_MSG( "processing changes from _cur_lsn=" << _cur_lsn );

#ifdef DB2READLOG_LRI_1
  db2LRI end_lsn;
  memset( &end_lsn, -1, sizeof ( end_lsn ) );
  end_lsn.lriType = _cur_lsn.lriType;
#else
  db2LSN end_lsn;
  end_lsn.lsnU64 = db2Uint64(-1);
#endif

  struct db2ReadLogStruct read_log_input;
  memset( &read_log_input, '\0', sizeof ( read_log_input ) );
  read_log_input.iCallerAction = DB2READLOG_READ;
#ifdef DB2READLOG_LRI_1
  read_log_input.piStartLRI = &_cur_lsn;
  read_log_input.piEndLRI = &end_lsn;
#else
  read_log_input.piStartLSN = &_cur_lsn;
  read_log_input.piEndLSN = &end_lsn;
#endif
  read_log_input.poLogBuffer = &_log_buf[0];
  read_log_input.iLogBufferSize = _log_buf.size();
  read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

  struct db2ReadLogInfoStruct read_log_info;
  memset( &read_log_info, sizeof ( read_log_info ), '\0' );

  read_log_input.poReadLogInfo = &read_log_info;

  struct sqlca sql_ca;
#ifdef DB2READLOG_LRI_1
  SQL_API_RC sql_rc(db2ReadLog( db2Version1010, &read_log_input, &sql_ca ));
#else
  SQL_API_RC sql_rc(db2ReadLog( db2Version970, &read_log_input, &sql_ca ));
#endif

  LOG_TRACE_MSG( "db2ReadLog READ sql_rc=" << sql_rc << " sql_ca=" << sql_ca );

  if ( (sql_rc != SQL_SUCCESS) || (sql_ca.sqlcode < 0) ) {
    THROW_RUNTIME_ERROR_EXCEPTION( "db2ReadLog READ failed" );
  }

  const int NO_MESSAGES_WARNING_CODE(2654);

  if ( (sql_ca.sqlcode > 0) && (sql_ca.sqlcode != NO_MESSAGES_WARNING_CODE) ) {
    LOG_INFO_MSG( "db2ReadLog READ indicates unexpected warning; sql_ca=" << sql_ca );
  }

  LOG_TRACE_MSG( "read_log_info:" << read_log_info );

  if ( read_log_info.logRecsWritten != 0 ) {
#ifdef DB2READLOG_LRI_1
    _cur_lsn = read_log_info.nextStartLRI;
#else
    _cur_lsn = read_log_info.nextStartLSN;
#endif

    DbChanges db_changes;

    _processLogBuffer(
        read_log_info.logRecsWritten,
        db_changes
      );

    if ( _clients.empty() || db_changes.empty() || _max_transact_cond ) {
        // Don't need to notify clients.
        return;
    }

    std::for_each( _clients.begin(), _clients.end(), boost::bind( &Client::notifyDbChanges, _1, boost::ref(db_changes) ) );
  }
}


void DbChangesMonitor::_processLogBuffer(
        db2Uint32 log_rec_count,
        DbChanges& db_changes
    )
{
  const char *record_p(&_log_buf[0] + sizeof(struct db2ReadLogFilterData));

  for ( db2Uint32 record_i(0); record_i < log_rec_count; ++record_i ) {
    sqluint32 record_size(*(reinterpret_cast<const sqluint32*>(record_p)));

    _processLogRecord( record_p, db_changes );

    // Move to the next record.
    record_p = record_p + record_size + sizeof(struct db2ReadLogFilterData);
  }
} // db_changes_monitor_t::_processLogBuffer()


void DbChangesMonitor::_processLogRecord(
        const char* record_p,
        DbChanges& db_changes
    )
{
  sqluint16 record_type(*(reinterpret_cast<const sqluint16*>(record_p + LogRecordTypeOffset)));
  sqluint16 record_flag(*(reinterpret_cast<const sqluint16*>(record_p + LogRecordGeneralFlagOffset)));
  const tid_t &tid(*(reinterpret_cast<const tid_t*>(record_p + LogRecordTidOffset)));

  LOG_TRACE_MSG(
      "record:"
      " type=" << std::hex << record_type <<
      " flag=" << record_flag <<
      " tid=" << tid <<
      ")"
    );

#if 0
  if ( LOG_UPTO(thread_args_p->log_level) & LOG_MASK(LOG_DEBUG) ) {
    UserDataDisplay( const_cast<char*>(record_p), record_size );
  }
#endif

  switch ( record_type ) {
      case 0x004E: // Normal record
          {
              const char* dm_header_p(record_p + LogRecordNonCompensationHeaderSize);

              _handleDmRecord( dm_header_p, tid );

              if ( record_flag & 0x10 ) {
                  _transactions.commit( tid, db_changes );
              }
          }
          break;
      case 0x0084: // Normal Commit
          _transactions.commit( tid, db_changes );
          break;
      case 0x0041: // Rollback
          _transactions.rollback( tid );
          break;
      case 0x0043:
          // compensation, ignore this
          LOG_DEBUG_MSG( "Ignoring compensation record" );
          break;
      default:
          LOG_INFO_MSG( "Ignoring record because record_type is " << std::hex << record_type );
  }
}


void DbChangesMonitor::_handleDmRecord( const char* dm_header_p, const tid_t& tid )
{
    unsigned char component_id(*(reinterpret_cast<const unsigned char*>(dm_header_p + 0)));
    unsigned char function_id(*(reinterpret_cast<const unsigned char*>(dm_header_p + 1)));
    sqluint16 table_space_id(*(reinterpret_cast<const sqluint16*>(dm_header_p + 2)));
    sqluint16 table_id(*(reinterpret_cast<const sqluint16*>(dm_header_p + 4)));

    if ( component_id != 1 ) {
        LOG_TRACE_MSG( "Ignoring because DM record has component id " << component_id );
        return;
    }

    if ( ! (function_id == 161 || function_id == 162 || function_id == 163) ) { // dalete, insert, update
        LOG_TRACE_MSG( "Ignoring because function is " << function_id );
        return;
    }

    TableInfo table_info( table_space_id, table_id );


    LOG_TRACE_MSG( "Looking up table for " << table_info );

    AbstractTable::Ptr table_ptr(_tables_info.calcTable( table_info ));

    if ( ! table_ptr ) {
        LOG_TRACE_MSG( "Ignoring change to unknown table " << table_info );
        return;
    }

    bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr;

    if ( function_id == 161 || function_id == 162 ) {

        const char *record_header_p(dm_header_p + 20);
        char head_record_type(*(reinterpret_cast<const char*>(record_header_p + 0)));
        const char *record_p(record_header_p + 4);
        char rec_record_type(*(reinterpret_cast<const char*>(record_p + 0)));

        if ( ! ((head_record_type == 0x00 || head_record_type == 0x10 || head_record_type & 0x04) &&
                (rec_record_type & 0x01 || rec_record_type & 0x02)) ) {
            LOG_TRACE_MSG( "Ignoring change because head_record_type=" << head_record_type << " and rec_record_type=" << rec_record_type );
            return;
        }

        const char *user_data_buffer(record_p + 4);

        db_change_ptr = table_ptr->handleRecord(
                user_data_buffer,
                NULL, // old_data_buffer
                function_id == 161 ? AbstractTable::OperationType::DELETE : AbstractTable::OperationType::INSERT
            );

    } else {

        const char *old_rec_head_p(dm_header_p + 20);
        sqluint16 old_rec_size(*(reinterpret_cast<const sqluint16*>(old_rec_head_p + 2)));

        const char *old_rec_p(old_rec_head_p + 4);

        char old_rec_rec_type(*(reinterpret_cast<const char*>(old_rec_p + 0)));

        if ( ! (old_rec_rec_type & 0x01 || old_rec_rec_type & 0x02) ) {
            LOG_TRACE_MSG( "Ignoring change because old record's record_type=" << old_rec_rec_type );
            return;
        }

        const char *old_user_data_buffer(old_rec_p + 4);

        const char *new_rec_head_p(old_rec_head_p + old_rec_size + 20);

        const char *new_rec_p(new_rec_head_p + 4);

        char new_rec_rec_type(*(reinterpret_cast<const char*>(new_rec_p + 0)));

        if ( ! (new_rec_rec_type & 0x01 || new_rec_rec_type & 0x02) ) {
            LOG_TRACE_MSG( "Ignoring change because new record's record_type=" << new_rec_rec_type );
            return;
        }

        const char *new_user_data_buffer(new_rec_p + 4);

        db_change_ptr = table_ptr->handleRecord(
                new_user_data_buffer,
                old_user_data_buffer,
                AbstractTable::OperationType::UPDATE );
    }


    if ( ! db_change_ptr )  return;

    _transactions.change( tid, db_change_ptr );

}


void DbChangesMonitor::_startInotifyRead()
{
    _inotify.async_read(
            _events,
            _strand.wrap ( bind(
                    &DbChangesMonitor::_gotEvents, this,
                    boost::asio::placeholders::error
                ) )
        );
}


void DbChangesMonitor::_gotEvents(
        const boost::system::error_code& //err
    )
{
    _processChanges();

    _startInotifyRead();
}

} // namespace realtime::server::db2
} // namespace realtime::server
} // namespace realtime
