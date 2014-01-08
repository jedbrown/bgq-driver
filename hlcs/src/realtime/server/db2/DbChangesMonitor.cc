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
const unsigned RecordOffset = 16;
const unsigned LogRecordTidOffset = 16;
const unsigned LogRecordNonCompensationHeaderSize = 24;
  // I don't know why this has to be 24 since the doc says it should be 22.


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


//-------------------------------------------------------------------------
// functions for displaying log records (copied from DB2 sample program)


#if 0 // unused
static void SimpleLogRecordDisplay(
    sqluint16 recordType,
    sqluint16 /* recordFlag */,
    char      *recordDataBuffer,
    sqluint32 recordDataSize
  )
{
  sqluint32 timeTransactionCommited = 0;
  sqluint16 authIdLen = 0;
  char      *authId = NULL;

  switch (recordType)
  {
    case 138:
      cout << "\n    Record type: Local pending list" << endl;
      timeTransactionCommited = *(sqluint32 *) (recordDataBuffer);
      authIdLen = *(sqluint16 *) (recordDataBuffer + 2*sizeof(sqluint32));
      authId = (char *)malloc(authIdLen + 1);
      memset(authId, '\0', (authIdLen + 1 ));
      memcpy(authId, (char *)(recordDataBuffer + 2*sizeof(sqluint32) +
          sizeof(sqluint16)), authIdLen);
      authId[authIdLen] = '\0';
      cout << "      UTC transaction committed(in secs since 70-01-01)" << ": "
          << dec << timeTransactionCommited << endl;
      cout << "      authorization ID of the application: " << authId << endl;
      break;
    case 132:
      cout << "\n    Record type: Normal commit" << endl;
      timeTransactionCommited = *(sqluint32 *) (recordDataBuffer);
      authIdLen = *(sqluint16 *) (recordDataBuffer + 2*sizeof(sqluint32));
      authId = (char *)malloc(authIdLen + 1);
      memset( authId, '\0', (authIdLen + 1 ));
      memcpy(authId, (char *)(recordDataBuffer + 2*sizeof(sqluint32) +
          sizeof(sqluint16)), authIdLen);
      authId[authIdLen] = '\0';
      cout << "      UTC transaction committed(in secs since 70-01-01)" << ": "
          << dec << timeTransactionCommited << endl;
      cout << "      authorization ID of the application: " << authId << endl;
      break;
    case 65:
      cout << "\n    Record type: Normal abort" << endl;
      authIdLen = (sqluint16) (recordDataSize);
      authId = (char *)malloc(authIdLen + 1);
      memset( authId, '\0', (authIdLen + 1 ));
      memcpy(authId, (char *)(recordDataBuffer + sizeof(sqluint16)), authIdLen);
      authId[authIdLen] = '\0';
      cout << "      authorization ID of the application: " << authId << endl;
      break;
    default:
      cout << "    Unknown simple log record: "
          << (char)recordType << " " << recordDataSize << endl;
      break;
  }
} // SimpleLogRecordDisplay()


static void UserDataDisplay(
    char      *dataBuffer,
    sqluint16 dataSize
  )
{
  sqluint16 line = 0;
  sqluint16 col = 0;
  const int rowLength = 10;

  cout << "        user data:" << endl;

  for (line = 0; line * rowLength < dataSize; line = line + 1)
  {
    cout << "        ";
    for (col = 0; col < rowLength; col = col + 1)
    {
      if (line * rowLength + col < dataSize)
      {
        cout.fill('0');
        cout.width(2);
        cout.setf(ios::uppercase);
        cout.setf(ios_base::hex,ios_base::basefield);
        cout << (int)(dataBuffer[line * rowLength + col] & 0x0ff) << " ";
      }
      else
      {
        cout << "   ";
      }
    }
    cout << "*";
    for (col = 0; col < rowLength; col = col + 1)
    {
      if (line * rowLength + col < dataSize)
      {
        if (isalpha(dataBuffer[line * rowLength + col]) ||
            isdigit(dataBuffer[line * rowLength + col]))
        {
          cout << dataBuffer[line * rowLength + col];
        }
        else
        {
          cout << ".";
        }
      }
      else
      {
        cout << " ";
      }
    }
    cout << "*" << endl;
  }
  cout.setf(ios::dec);
}


static void LogSubRecordDisplay(
    char      *recordBuffer,
    sqluint16 recordSize
  )
{
  sqluint8  recordType = 0;
  sqluint8  updatableRecordType = 0;
  sqluint16 userDataFixedLength = 0;
  char      *userDataBuffer = NULL;
  sqluint16 userDataSize = 0;

  recordType = *(sqluint8 *) (recordBuffer);
  if ((recordType != 0) && (recordType != 4) && (recordType != 16))
  {
    cout << "        Unknown subrecord type." << endl;
  }
  else if (recordType == 4)
  {
    cout << "        subrecord type: Special control" << endl;
  }
  else
    // recordType == 0 or recordType == 16
    // record Type 0 indicates a normal record
    // record Type 16, for the purposes of this program, should be treated
    // as type 0
  {
    cout << "        subrecord type: Updatable, ";
    updatableRecordType = *(sqluint8 *) (recordBuffer + 4);
    if (updatableRecordType != 1)
    {
      cout << "Internal control" << endl;
    }
    else
    {
      cout << "Formatted user data" << endl;
      userDataFixedLength = *(sqluint16 *) (recordBuffer + 6);
      cout << "        user data fixed length: "
          << dec << userDataFixedLength << endl;
      userDataBuffer = recordBuffer + 8;
      userDataSize = recordSize - 8;
      UserDataDisplay(userDataBuffer, userDataSize);
    }
  }
} // LogSubRecordDisplay()


static void ComplexLogRecordDisplay(
    sqluint16 recordType,
    sqluint16 /* recordFlag */,
    char      *recordHeaderBuffer,
    sqluint32 recordHeaderSize,
    sqluint8  componentIdentifier,
    char      *recordDataBuffer,
    sqluint32 /* recordDataSize */
  )
{
  sqluint8 functionIdentifier = 0;

  // for insert, delete, undo delete
  RID       recid;
  sqluint16 subRecordLen = 0;
  sqluint16 subRecordOffset = 0;
  char      *subRecordBuffer = NULL;

  // for update
  RID       newRID;
  sqluint16 newSubRecordLen = 0;
  sqluint16 newSubRecordOffset = 0;
  char      *newSubRecordBuffer = NULL;
  RID       oldRID;
  sqluint16 oldSubRecordLen = 0;
  sqluint16 oldSubRecordOffset = 0;
  char      *oldSubRecordBuffer = NULL;

  // for alter table attributes
  sqluint64 alterBitMask = 0;
  sqluint64 alterBitValues = 0;

  switch( recordType )
  {
    case 0x004E:
      cout << "\n    Record type: Normal" << endl;
      break;
    case 0x0043:
      cout << "\n    Record type: Compensation." << endl;
      break;
    default:
      cout << "\n    Unknown complex log record type: " << recordType << endl;
      break;
  }

  switch (componentIdentifier)
  {
    case 1:
      cout << "      component ID: DMS log record" << endl;
      break;
    default:
      cout << "      unknown component ID: " << componentIdentifier << endl;
      break;
  }

  functionIdentifier = *(sqluint8 *) (recordHeaderBuffer + 1);
  switch (functionIdentifier)
  {
    case 161:
      cout << "      function ID: Delete Record" << endl;
      subRecordLen = *( (sqluint16 *)( recordDataBuffer + sizeof(sqluint16) ) );
      recid.set( recordDataBuffer + 3 * sizeof(sqluint16) );
      subRecordOffset = *( (sqluint16 *)( recordDataBuffer + 3 * sizeof(sqluint16) +
          recid.size() ) );
      cout << "        RID: " << dec << recid.getString() << endl;
      cout << "        subrecord length: " << subRecordLen << endl;
      cout << "        subrecord offset: " << subRecordOffset << endl;
      subRecordBuffer = recordDataBuffer + 3 * sizeof(sqluint16) +
          recid.size() + sizeof(sqluint16);
      LogSubRecordDisplay( subRecordBuffer, subRecordLen );
      break;
    case 111:
      cout << "      function ID: Undo Delete Record" << endl;
      subRecordLen = *( (sqluint16 *)( recordDataBuffer + sizeof(sqluint16) ) );
      recid.set( recordDataBuffer + 3 * sizeof(sqluint16) );
      subRecordOffset = *( (sqluint16 *)( recordDataBuffer + 3 * sizeof(sqluint16) +
          recid.size() ) );
      cout << "        RID: " << dec << recid.getString() << endl;
      cout << "        subrecord length: " << subRecordLen << endl;
      cout << "        subrecord offset: " << subRecordOffset << endl;
      subRecordBuffer = recordDataBuffer + 3 * sizeof(sqluint16) +
          recid.size() + sizeof(sqluint16);
      LogSubRecordDisplay(subRecordBuffer, subRecordLen);
      break;
    case 162:
      cout << "      function ID: Insert Record" << endl;
      subRecordLen = *( (sqluint16 *)( recordDataBuffer + sizeof(sqluint16) ) );
      recid.set( recordDataBuffer + 3 * sizeof(sqluint16) );
      subRecordOffset = *( (sqluint16 *)( recordDataBuffer + 3 * sizeof(sqluint16) +
          recid.size() ) );
      cout << "        RID: " << dec << recid.getString() << endl;
      cout << "        subrecord length: " << subRecordLen << endl;
      cout << "        subrecord offset: " << subRecordOffset << endl;
      subRecordBuffer = recordDataBuffer + 3 * sizeof(sqluint16) + recid.size() +
          sizeof(sqluint16);
      LogSubRecordDisplay( subRecordBuffer, subRecordLen );
      break;
    case 163:
      cout << "      function ID: Update Record" << endl;
      oldSubRecordLen = *( (sqluint16 *)( recordDataBuffer + sizeof(sqluint16) ) );
      oldRID.set( recordDataBuffer + 3 * sizeof(sqluint16) );
      oldSubRecordOffset = *( (sqluint16 *)( recordDataBuffer + 3 * sizeof(sqluint16) +
          oldRID.size() ) );
      newSubRecordLen = *( (sqluint16 *)( recordDataBuffer  +
          sizeof(sqluint16) +
          oldRID.size()     +
          sizeof(sqluint32) +
          sizeof(sqluint16) +
          oldSubRecordLen   +
          recordHeaderSize  +
          sizeof(sqluint16) ) );
      newRID.set( recordDataBuffer + 3 * sizeof(sqluint16) +
          oldRID.size() + sizeof(sqluint16) + oldSubRecordLen +
          recordHeaderSize + sizeof(sqluint16) );

      newSubRecordOffset = *(sqluint16 *)( recordDataBuffer      +
          3 * sizeof(sqluint16) +
          oldRID.size()         +
          sizeof(sqluint16)     +
          oldSubRecordLen       +
          recordHeaderSize      +
          newRID.size()         +
          sizeof(sqluint16) );
      cout << "        oldRID: " << dec << oldRID.getString() << endl;
      cout << "        old subrecord length: " << oldSubRecordLen << endl;
      cout << "        old subrecord offset: " << oldSubRecordOffset << endl;
      oldSubRecordBuffer = recordDataBuffer + 3 * sizeof(sqluint16) +
          oldRID.size() + sizeof(sqluint16);
      LogSubRecordDisplay( oldSubRecordBuffer, oldSubRecordLen );
      cout << "        newRID: " << dec << newRID.getString() << endl;
      cout << "        new subrecord length: " << newSubRecordLen << endl;
      cout << "        new subrecord offset: " << newSubRecordOffset << endl;
      newSubRecordBuffer = recordDataBuffer      +
          3 * sizeof(sqluint16) +
          oldRID.size()         +
          sizeof(sqluint16)     +
          oldSubRecordLen       +
          recordHeaderSize      +
          3 * sizeof(sqluint16) +
          newRID.size()         +
          sizeof(sqluint16) ;
      LogSubRecordDisplay( newSubRecordBuffer, newSubRecordLen );
      break;
    case 124:
      cout << "      function ID:  Alter Table Attribute" << endl;
      alterBitMask = *(sqluint64 *) (recordDataBuffer);
      alterBitValues = *( (sqluint64 *)(recordDataBuffer + sizeof(sqluint64) ) );
      if( alterBitMask & 0x00000001 )
      {
                // Propagation attribute altered
        cout << "        Propagation attribute is changed to ";
        if (alterBitValues & 0x00000001)
        {
          cout << "ON" << endl;
        }
        else
        {
          cout << "OFF" << endl;
        }
      }
      if (alterBitMask & 0x00000002)
      {
                // Check Pending attribute altered
        cout << "        Check Pending attr. changed to: ";
        if (alterBitValues & 0x00000002)
        {
          cout << "ON" << endl;
        }
        else
        {
          cout << "OFF" << endl;
        }
      }
      if (alterBitMask & 0x00010000)
      {
                // Append Mode attribute altered
        cout << "        Append Mode attr. changed to: ";
        if (alterBitValues & 0x00010000)
        {
          cout << "ON" << endl;
        }
        else
        {
          cout << "OFF" << endl;
        }
      }
      if (alterBitMask & 0x00200000)
      {
                // LF Propagation attribute altered
        cout << "        LF Propagation attribute is changed to ";
        if (alterBitValues & 0x00200000)
        {
          cout << "ON" << endl;
        }
        else
        {
          cout << "OFF" << endl;
        }
      }
      if (alterBitMask & 0x00400000)
      {
                // LOB Propagation attribute altered
        cout << "        LOB Propagation attr.changed to: ";
        if (alterBitValues & 0x00400000)
        {
          cout << "ON" << endl;
        }
        else
        {
          cout << "OFF" << endl;
        }
      }
      break;
    default:
      cout << "      unknown function identifier: " << functionIdentifier << endl;
      break;
  }
} // ComplexLogRecordDisplay()


static void LogRecordDisplay(
    char      *recordBuffer,
    sqluint32 recordSize,
    sqluint16 recordType,
    sqluint16 recordFlag
  )
{
  sqluint32 logManagerLogRecordHeaderSize = 0;
  char      *recordDataBuffer = NULL;
  sqluint32 recordDataSize = 0;
  char      *recordHeaderBuffer = NULL;
  sqluint8  componentIdentifier = 0;
  sqluint32 recordHeaderSize = 0;

  // determine the logManagerLogRecordHeaderSize
  logManagerLogRecordHeaderSize = LogRecordNonCompensationHeaderSize;
  if( recordType == 0x0043 )  // compensation
  {
    logManagerLogRecordHeaderSize += sizeof(db2LSN);
    if( recordFlag & 0x0002 )    // propagatable
    {
      logManagerLogRecordHeaderSize += sizeof(db2LSN);
    }
  }

  switch (recordType)
  {
    case 0x008A:                // Local Pending List
    case 0x0084:                // Normal Commit
    case 0x0041:                // Normal Abort
      recordDataBuffer = recordBuffer + logManagerLogRecordHeaderSize;
      recordDataSize = recordSize - logManagerLogRecordHeaderSize;
      SimpleLogRecordDisplay( recordType,
                              recordFlag,
                              recordDataBuffer,
                              recordDataSize );
      break;
    case 0x004E:                // Normal
    case 0x0043:                // Compensation
      recordHeaderBuffer = recordBuffer + logManagerLogRecordHeaderSize;
      componentIdentifier = *(sqluint8 *) recordHeaderBuffer;
      switch (componentIdentifier)
      {
        case 1:                 // Data Manager Log Record
          recordHeaderSize = 6;
          break;
        default:
          cout << "    Unknown complex log record: size=" << recordSize
               << " type=" << std::hex << recordType << " " << componentIdentifier << endl;
          return;
      }
      recordDataBuffer = recordBuffer +
          logManagerLogRecordHeaderSize + recordHeaderSize;
      recordDataSize = recordSize -
          logManagerLogRecordHeaderSize - recordHeaderSize;
      ComplexLogRecordDisplay( recordType,
                               recordFlag,
                               recordHeaderBuffer,
                               recordHeaderSize,
                               componentIdentifier,
                               recordDataBuffer,
                               recordDataSize );
      break;
    default:
      cout << "    Unknown log record: size=" << recordSize <<
              " type=" << std::hex << recordType << endl;
      break;
  }

  return;
} // LogRecordDisplay()


static void LogBufferDisplay(
    char      *logBuffer,
    sqluint32 numLogRecords
  )
{
  sqluint32 logRecordNb = 0;
  sqluint32 recordSize = 0;
  sqluint16 recordType = 0;
  sqluint16 recordFlag = 0;
  char      *recordBuffer = NULL;

  // initialize the recordBuffer
  recordBuffer = logBuffer + RecordOffset;

  for (logRecordNb = 0; logRecordNb < numLogRecords; logRecordNb++)
  {
    recordSize = *(sqluint32 *) (recordBuffer);
    recordType = *(sqluint16 *) (recordBuffer + LogRecordTypeOffset);
    recordFlag = *(sqluint16 *) (recordBuffer + LogRecordGeneralFlagOffset);

    const tid_t *tid_p((tid_t*) (recordBuffer + LogRecordTidOffset));
    LOG_TRACE_MSG( "tid is " << *tid_p );

    LogRecordDisplay( recordBuffer, recordSize, recordType, recordFlag );

    // update the recordBuffer
    recordBuffer = recordBuffer + recordSize + RecordOffset;
  }
} // LogBufferDisplay()
#endif


//---------------------------------------------------------------------
// class DbChangesMonitor


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
  const char *record_p(&_log_buf[0] + RecordOffset);

  for ( db2Uint32 record_i(0); record_i < log_rec_count; ++record_i ) {
    sqluint32 record_size(*(reinterpret_cast<const sqluint32*>(record_p)));

    _processLogRecord( record_p, db_changes );

    // Move to the next record.
    record_p = record_p + record_size + RecordOffset;
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
      default:
          LOG_INFO_MSG( "Ignoring record because record_type is " << record_type );
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
