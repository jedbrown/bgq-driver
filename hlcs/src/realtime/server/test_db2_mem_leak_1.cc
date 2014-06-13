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

// This is a test program to see if there's a memory leak in DB2 from the real-time server's use of db2ReadLog.


#include <boost/lexical_cast.hpp>

#include <sqlcli.h>
#include <db2ApiDf.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include <stdint.h>
#include <string.h>


using namespace std;


#define THROW_RUNTIME_ERROR_EXCEPTION( msg )  { std::ostringstream throw_trex_ss; throw_trex_ss << msg; throw std::runtime_error( throw_trex_ss.str() ); }

#define LOG_INFO_MSG( msg ) { std::cerr << msg << std::endl; }
#define LOG_TRACE_MSG( msg ) { std::cerr << msg << std::endl; }


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

/////////////////////////////////////////////////////////////


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


class DbChangesMonitor
{
public:
    DbChangesMonitor(
            unsigned usleep_time,
            const std::string& db_name
        ) :
            _usleep_time(usleep_time),
            _db_name(db_name),
            _log_buf( _LOG_BUF_SIZE )
    { /*Nothing to do*/ }

    void run();

private:

    static const unsigned int _LOG_BUF_SIZE;


    unsigned _usleep_time;
    std::string _db_name;

    db2LSN _cur_lsn;

    std::vector<char> _log_buf;


    db2LSN _initCurLsn( const std::string& db_name );

    void _processChanges();
};


const unsigned int DbChangesMonitor::_LOG_BUF_SIZE(64 * 1024 * 1024);


db2LSN DbChangesMonitor::_initCurLsn( const std::string& db_name )
{
    /*
    SQLRETURN sqlrc(SQL_SUCCESS);

    SQLHANDLE henv(SQL_NULL_HANDLE);
    sqlrc = SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv );
    if ( sqlrc != SQL_SUCCESS ) {
        throw std::runtime_error( "failed to allocate environment handle." );
    }

    SQLHANDLE hdbc(SQL_NULL_HANDLE);
    sqlrc = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );
    if ( sqlrc != SQL_SUCCESS ) {
        throw std::runtime_error( "failed to allocate conn handle." );
    }

    sqlrc = SQLConnect(
            hdbc,
            (SQLCHAR*) db_name.c_str(), SQL_NTS,
            NULL, SQL_NTS,
            NULL, SQL_NTS
        );

    if ( sqlrc != SQL_SUCCESS ) {
        throw std::runtime_error( "SQLConnect failed" );
    }
    */

    // Use db2ReadLog(QUERY), db2ReadLog(READ)+ to get to the end of the log

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

    db2LSN cur_lsn;

    {
      struct db2ReadLogStruct read_log_input;
      memset( &read_log_input, '\0', sizeof ( read_log_input ) );
      read_log_input.iCallerAction = DB2READLOG_QUERY;
      read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

      struct db2ReadLogInfoStruct read_log_info;
      memset( &read_log_info, '\0', sizeof ( read_log_info ) );

      read_log_input.poReadLogInfo = &read_log_info;

      sql_rc = db2ReadLog( db2Version970, &read_log_input, &sql_ca );

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

      LOG_INFO_MSG(  "read_log_info:\n"
          "\tinitialLSN=" << read_log_info.initialLSN << "\n"
          "\tfirstReadLSN=" << read_log_info.firstReadLSN << "\n"
          "\tnextStartLSN=" << read_log_info.nextStartLSN << "\n"
          "\tlogRecsWritten=" << read_log_info.logRecsWritten << "\n"
          "\tlogBytesWritten=" << read_log_info.logBytesWritten << "\n"
          "\tfirstReusedLSN=" << read_log_info.firstReusedLSN << "\n"
        );

      cur_lsn = read_log_info.nextStartLSN;
    }

    {
      db2LSN end_lsn;
      end_lsn.lsnU64 = db2Uint64(-1);

      while ( true ) {
        struct db2ReadLogStruct read_log_input;
        memset( &read_log_input, '\0', sizeof ( read_log_input ) );
        read_log_input.iCallerAction = DB2READLOG_READ;
        read_log_input.piStartLSN = &cur_lsn;
        read_log_input.piEndLSN = &end_lsn;
        read_log_input.poLogBuffer = &_log_buf[0];
        read_log_input.iLogBufferSize = _log_buf.size();
        read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

        struct db2ReadLogInfoStruct read_log_info;
        memset( &read_log_info, '\0', sizeof ( read_log_info ) );

        read_log_input.poReadLogInfo = &read_log_info;

        sql_rc = db2ReadLog( db2Version970, &read_log_input, &sql_ca );

        LOG_INFO_MSG( "db2ReadLog READ sql_rc=" << sql_rc << " sql_ca=" << sql_ca );

        if ( (sql_rc != SQL_SUCCESS) || (sql_ca.sqlcode < 0) ) {
          THROW_RUNTIME_ERROR_EXCEPTION( "db2ReadLog READ failed" );
        }

        LOG_INFO_MSG(  "read_log_info:\n"
            "\tinitialLSN=" << read_log_info.initialLSN << "\n"
            "\tfirstReadLSN=" << read_log_info.firstReadLSN << "\n"
            "\tnextStartLSN=" << read_log_info.nextStartLSN << "\n"
            "\tlogRecsWritten=" << read_log_info.logRecsWritten << "\n"
            "\tlogBytesWritten=" << read_log_info.logBytesWritten << "\n"
            "\tfirstReusedLSN=" << read_log_info.firstReusedLSN << "\n"
          );

        cur_lsn = read_log_info.nextStartLSN;

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

    db2LSN end_lsn;
    end_lsn.lsnU64 = db2Uint64(-1);

    struct db2ReadLogStruct read_log_input;
    memset( &read_log_input, '\0', sizeof ( read_log_input ) );
    read_log_input.iCallerAction = DB2READLOG_READ;
    read_log_input.piStartLSN = &_cur_lsn;
    read_log_input.piEndLSN = &end_lsn;
    read_log_input.poLogBuffer = &_log_buf[0];
    read_log_input.iLogBufferSize = _log_buf.size();
    read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

    struct db2ReadLogInfoStruct read_log_info;
    memset( &read_log_info, '\0', sizeof ( read_log_info ));

    read_log_input.poReadLogInfo = &read_log_info;

    struct sqlca sql_ca;
    SQL_API_RC sql_rc(db2ReadLog( db2Version970, &read_log_input, &sql_ca ));

    LOG_TRACE_MSG( "db2ReadLog READ sql_rc=" << sql_rc << " sql_ca=" << sql_ca );

    if ( (sql_rc != SQL_SUCCESS) || (sql_ca.sqlcode < 0) ) {
      THROW_RUNTIME_ERROR_EXCEPTION( "db2ReadLog READ failed" );
    }

    const int NO_MESSAGES_WARNING_CODE(2654);

    if ( (sql_ca.sqlcode > 0) && (sql_ca.sqlcode != NO_MESSAGES_WARNING_CODE) ) {
      LOG_INFO_MSG( "db2ReadLog READ indicates unexpected warning; sql_ca=" << sql_ca );
    }

    LOG_TRACE_MSG( "read_log_info:\n"
        "\tinitialLSN=" << read_log_info.initialLSN << "\n"
        "\tfirstReadLSN=" << read_log_info.firstReadLSN << "\n"
        "\tnextStartLSN=" << read_log_info.nextStartLSN << "\n"
        "\tlogRecsWritten=" << read_log_info.logRecsWritten << "\n"
        "\tlogBytesWritten=" << read_log_info.logBytesWritten << "\n"
        "\tfirstReusedLSN=" << read_log_info.firstReusedLSN << "\n"
      );

    if ( read_log_info.logRecsWritten != 0 ) {
      _cur_lsn = read_log_info.nextStartLSN;

      /* Don't do this!
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
      */
    }
}


void DbChangesMonitor::run()
{
    _cur_lsn = _initCurLsn( _db_name );

    while ( true ) {
        usleep( _usleep_time );
        _processChanges();
    }
}


////////////////////////////////////////////////////////////////////////////////////


int main( int argc, char *argv[] )
{
    cout << "Working...\n";

    unsigned usleep_time = 1000000;
    string db_name = "BGDB0";

    if ( argc > 1 ) {
        usleep_time = boost::lexical_cast<unsigned>( argv[1] );
    }

    if ( argc > 2 ) {
        db_name = argv[2];
    }

    DbChangesMonitor db_changes_monitor(
            usleep_time,
            db_name
        );

    db_changes_monitor.run();

    cout << "Done!\n";
}
