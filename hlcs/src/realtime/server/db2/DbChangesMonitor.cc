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

#include "bgsched/BlockImpl.h"
#include "bgsched/JobImpl.h"
#include "bgsched/HardwareImpl.h"

#include "bgsched/realtime/types.h"

#include <utility/include/Log.h>

#include <boost/bind.hpp>

#include <sqladef.h>

#include <algorithm>
#include <iostream>
#include <map>
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
using std::map;
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


// Conversion functions from DB codes to enums.

static bgsched::Hardware::State hardwareStateCodeToValue( char code )
{
    return bgsched::Hardware::Impl::convertDatabaseState( (string() + code).c_str() );
}


static bgsched::Dimension::Value dimensionCodeToValue( char code )
{
    if ( code == 'A' )  { return bgsched::Dimension::A; }
    if ( code == 'B' )  { return bgsched::Dimension::B; }
    if ( code == 'C' )  { return bgsched::Dimension::C; }
    if ( code == 'D' )  { return bgsched::Dimension::D; }
    if ( code == 'E' )  { return bgsched::Dimension::E; }
    return bgsched::Dimension::InvalidDimension;
}


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


#if 0 /* not used */
static ostream& operator <<( ostream& os, const struct inotify_event& ev )
{
  ostringstream oss;
  if ( ev.mask & IN_ACCESS )          oss << " IN_ACCESS";
  if ( ev.mask & IN_ATTRIB )          oss << " IN_ATTRIB";
  if ( ev.mask & IN_CLOSE_WRITE )     oss << " IN_CLOSE_WRITE";
  if ( ev.mask & IN_CLOSE_NOWRITE )   oss << " IN_CLOSE_NOWRITE";
  if ( ev.mask & IN_CREATE )          oss << " IN_CREATE";
  if ( ev.mask & IN_DELETE )          oss << " IN_DELETE";
  if ( ev.mask & IN_DELETE_SELF )     oss << " IN_DELETE_SELF";
  if ( ev.mask & IN_MODIFY )          oss << " IN_MODIFY";
  if ( ev.mask & IN_MOVE_SELF )       oss << " IN_MOVE_SELF";
  if ( ev.mask & IN_MOVED_FROM )      oss << " IN_MOVED_FROM";
  if ( ev.mask & IN_MOVED_TO )        oss << " IN_MOVED_TO";
  if ( ev.mask & IN_OPEN )            oss << " IN_OPEN";

  os << "{inotify_event wd=" << ev.wd << " mask=" << oss.str() <<
      " cookie=" << ev.cookie << " len=" << ev.len <<
      "}";
  return os;
} // operator<<(inotify_event)
#endif

namespace realtime {
namespace server {
namespace db2 {

typedef map<string,bgsched::realtime::RasSeverity::Value> db_str_to_ras_severity_t;

static db_str_to_ras_severity_t build_db_str_to_ras_severity()
{
    db_str_to_ras_severity_t ret;

    ret[string("FATAL")] = bgsched::realtime::RasSeverity::FATAL;
    ret[string("WARN")] = bgsched::realtime::RasSeverity::WARN;
    ret[string("INFO")] = bgsched::realtime::RasSeverity::INFO;
    ret[string("UNKNOWN")] = bgsched::realtime::RasSeverity::UNKNOWN;

    return ret;
}

static const db_str_to_ras_severity_t DB_STR_TO_RAS_SEVERITY = build_db_str_to_ras_severity();

static bgsched::realtime::RasSeverity::Value ras_severity_convert_db_to_enum( const string* str_p )
{
    if ( ! str_p )  return bgsched::realtime::RasSeverity::UNKNOWN;
    db_str_to_ras_severity_t::const_iterator i(DB_STR_TO_RAS_SEVERITY.find( *str_p ));
    if ( i == DB_STR_TO_RAS_SEVERITY.end() ) {
        // didn't find it.
        return bgsched::realtime::RasSeverity::UNKNOWN;
    }
    return i->second;
}


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
// Function to convert a fixed-size padded string from DB2 to a C-style string.
// This is used for block IDs, etc.

static string padded_str_to_str(
        const char *padded_str,
        unsigned int padded_str_size
    )
{
    string ret;

    // find the end of the padded string.
    const char *padded_str_end(padded_str + padded_str_size);
    while ( (padded_str_end != padded_str) && (*(padded_str_end-1) == ' ') ) {
        --padded_str_end;
    }

    // copy the significant chars
    while ( padded_str != padded_str_end ) {
        ret += *padded_str++;
    }

    return ret;
} // padded_str_to_str()


//-------------------------------------------------------------------------
// The structures that represents the record that db2ReadLog retrieves

// Figure out the record formats by doing a db2 DESCRIBE TABLE

#pragma pack(push,1)

struct block_record_t
{
  char block_id[32];
  int32_t num_c_nodes;
  int32_t num_io_nodes;
  char owner[32];
  char user_name[32];
  char user_name_nullind;
  char is_torus[5];
  char is_torus_nullind;
  int32_t size_a;
  int32_t size_b;
  int32_t size_c;
  int32_t size_d;
  int32_t size_e;
  int32_t description_vc;
  char description_nullind;
  char options[16];
  char status;
  char action;
  char status_last_modified[10];
  char status_last_mofified_nullind;
  uint32_t mloader_img_vc;
  char mloader_img_nullind;
  char node_config[32];
  uint32_t boot_options_vc;
  char boot_options_nullind;
  char create_date[10];
  uint8_t security_key[32];
  char security_key_nullind;
  uint32_t errtext_vc;
  char errtext_nullind;
  SQLBIGINT seq_id;
  uint32_t creation_id;
};


struct job_record_t
{
  SQLBIGINT id;
  char username[32];
  char block_id[32];
  uint32_t executable_vc;
  uint32_t workingdir_vc;
  char starttime[10];
  char status;
  char status_last_modified[10];
  SQLBIGINT seq_id;
  uint32_t args_vc;
  uint32_t envs_vc;
  uint32_t mapping_vc;
  int32_t nodes_used;
  int32_t shape_a;
  int32_t shape_b;
  int32_t shape_c;
  int32_t shape_d;
  int32_t shape_e;
  int32_t processes_per_node;
  uint32_t scheduler_data_vc;
  char scheduler_data_nullind;
  char corner[18];
  char corner_nullind;
  uint32_t hostname_vc;
  int32_t pid;
};


struct midplane_record_t
{
  char serialnumber[19];
  char serialnumber_nullind;
  char productid[16];
  char machine_serialnumber[19];
  char machine_serialnumber_nullind;
  char pos_in_machine[6];
  char status;
  char is_master;
  uint32_t vpd_vc;
  char vpd_nullind;
  SQLBIGINT seq_id;
};

struct node_card_record_t
{
  char serialnumber[19];
  char serialnumber_nullind;
  char product_id[16];
  char midplane_pos[6];
  char position[3];
  char status;
  uint32_t vpd_vc;
  char vpd_nullind;
  SQLBIGINT seq_id;
};

struct switch_record_t
{
  uint32_t switch_id_vc;
  char midplane_pos[6];
  char machine_serialnumber[19];
  char machine_serialnumber_nullind;
  char dimension;
  char status;
  SQLBIGINT seq_id;
  char switch_id[8]; // varchar data is at the end.
};

struct cable_record_t
{
    char from_location[16];
    char to_location[16];
    char status;
    char create_date[10]; // timestamp
    SQLBIGINT seq_id;
};


struct event_log_record_t
{
    SQLINTEGER recid;
    char msg_id[8];
    char msg_id_nullind;
    char category[16];
    char category_nullind;
    char component[16];
    char component_nullind;
    char severity[8];
    char severity_nullind;
    char event_time[10];
    SQLBIGINT jobid;
    char jobid_nullind;
    char block[32];
    char block_nullind;
    char location[64];
    char location_nullind;
    char serialnumber[19];
    char serialnumber_nullind;
    char ecid[32];
    char ecid_nullind;
    SQLINTEGER cpu;
    char cpu_nullind;
    SQLINTEGER count;
    char count_nullind;
    char ctlaction[32];
    char ctlaction_nullind;
    // Ignore Message, RawData, Diags.
};


struct node_record_t
{
    char serialnumber[19];
    char serialnumber_nullind;
    char product_id[16];
    char product_id_nullind;
    char ecid[32];
    char ecid_nullind;
    char midplane_pos[6];
    char node_card_pos[3];
    char position[3];
    uint32_t ip_address_vc;
    char ip_address_nullind;
    char mac_address[20];
    char mac_address_nullind;
    char status;
    SQLINTEGER memory_module_size;
    SQLINTEGER memory_size;
    SQLINTEGER psro;
    uint32_t vpd_vc;
    char vpd_nullind;
    SQLFLOAT voltage;
    char voltage_nullind;
    SQLINTEGER bitsteering;
    SQLBIGINT seq_id;
    //...
};


#pragma pack(pop)


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


db2LSN DbChangesMonitor::_initCurLsn( const Configuration& configuration )
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

    db2LSN cur_lsn;

    {
      struct db2ReadLogStruct read_log_input;
      memset( &read_log_input, '\0', sizeof ( read_log_input ) );
      read_log_input.iCallerAction = DB2READLOG_QUERY;
      read_log_input.iFilterOption = DB2READLOG_FILTER_ON;

      struct db2ReadLogInfoStruct read_log_info;
      memset( &read_log_info, sizeof ( read_log_info ), '\0' );

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
        memset( &read_log_info, sizeof ( read_log_info ), '\0' );

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
  memset( &read_log_info, sizeof ( read_log_info ), '\0' );

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

    TableInfos::table::Value table(_tables_info.calcTable( TableInfo( table_space_id, table_id ) ));

    if ( TableInfos::table::UNKNOWN == table ) {
      LOG_TRACE_MSG(
          "This record is not for a recognized table."
          " table_info=" << table_info
        );
      return;
    }

    if ( function_id == 161 || function_id == 162 ) {
        _handleInsertDelete( dm_header_p, function_id, table, tid );
    } else {
        _handleUpdate( dm_header_p, table, tid );
    }
}


void DbChangesMonitor::_handleInsertDelete( const char* dm_header_p, int function_id, TableInfos::table::Value table, const tid_t& tid )
{
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

    if ( TableInfos::table::BLOCK == table ) {
        const block_record_t *block_record_p(
                reinterpret_cast<const block_record_t*>(user_data_buffer)
            );

        string block_id(padded_str_to_str(
                block_record_p->block_id, sizeof ( block_record_p->block_id )
            ));

        char status_char(block_record_p->status);
        bgsched::SequenceId seq_id(block_record_p->seq_id);

        if ( 162 == function_id ) {
            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                    new bgsched::realtime::ClientEventListener::BlockAddedEventInfo::Impl(
                            block_id,
                            bgsched::Block::Impl::statusDbCharToValue( status_char ),
                            seq_id
                        )
                );

            _transactions.change( tid, db_change_ptr );
        } else {
            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                    new bgsched::realtime::ClientEventListener::BlockDeletedEventInfo::Impl(
                            block_id,
                            seq_id
                        )
                );

            _transactions.change( tid, db_change_ptr );
        }
    } else if ( TableInfos::table::JOB == table ) {
        const job_record_t *job_record_p(
            reinterpret_cast<const job_record_t*>(user_data_buffer)
          );

        bgsched::Job::Id job_id(job_record_p->id);
        string block_id(padded_str_to_str(
            job_record_p->block_id, sizeof ( job_record_p->block_id )
          ));
        char status_char(job_record_p->status);
        bgsched::SequenceId seq_id(job_record_p->seq_id);

        if ( 162 == function_id ) {

            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                    new bgsched::realtime::ClientEventListener::JobAddedEventInfo::Impl(
                            job_id,
                            block_id,
                            bgsched::Job::Impl::statusDbCharToValue( status_char ),
                            seq_id
                        )
                );

            _transactions.change( tid, db_change_ptr );
        } else {

            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                    new bgsched::realtime::ClientEventListener::JobDeletedEventInfo::Impl(
                            job_id,
                            block_id,
                            seq_id
                        )
                );

            _transactions.change( tid, db_change_ptr );
        }
    } else if ( table == TableInfos::table::EVENT_LOG ) {
        if ( 162 == function_id ) {
            const event_log_record_t *event_log_rec_p(
                    reinterpret_cast<const event_log_record_t*>(user_data_buffer)
                );

            if ( event_log_rec_p->msg_id_nullind ) {
                LOG_DEBUG_MSG( "ignoring event log entry with NULL message ID" );
            } else {
                string msg_id(padded_str_to_str( event_log_rec_p->msg_id, sizeof ( event_log_rec_p->msg_id ) ));

                bgsched::realtime::RasSeverity::Value severity(bgsched::realtime::RasSeverity::UNKNOWN);
                if ( ! event_log_rec_p->severity_nullind ) {
                    string severity_str(padded_str_to_str( event_log_rec_p->severity, sizeof ( event_log_rec_p->severity ) ));
                    severity = ras_severity_convert_db_to_enum( &severity_str );
                }

                bgsched::Job::Id db_job_id(bgsched::Job::Id(-1));
                if ( ! event_log_rec_p->jobid_nullind ) {
                    db_job_id = event_log_rec_p->jobid;
                }

                string block_id;
                if ( ! event_log_rec_p->block_nullind ) {
                    block_id = event_log_rec_p->block;
                }

                bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                        new bgsched::realtime::ClientEventListener::RasEventInfo::Impl(
                                event_log_rec_p->recid,
                                msg_id,
                                severity,
                                block_id,
                                db_job_id
                            )
                    );

                _transactions.change( tid, db_change_ptr );
            }
        } else {
            LOG_TRACE_MSG( "ignoring delete of event log entry" );
        }
    } else {
        LOG_TRACE_MSG( "ignoring insert/delete because table is " << table );
    }
}


void DbChangesMonitor::_handleUpdate( const char *dm_header_p, TableInfos::table::Value table, const tid_t& tid )
{
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

    if ( TableInfos::table::BLOCK == table ) {
      const block_record_t *old_block_record_p(
          reinterpret_cast<const block_record_t*>(old_user_data_buffer)
        );

      char old_status_char(old_block_record_p->status);
      bgsched::SequenceId old_seq_id(old_block_record_p->seq_id);

      const block_record_t *new_block_record_p(
          reinterpret_cast<const block_record_t*>(new_user_data_buffer)
        );

      char new_status_char(new_block_record_p->status);
      bgsched::SequenceId new_seq_id(new_block_record_p->seq_id);

      if ( (new_status_char != old_status_char) ||
           (new_seq_id != old_seq_id) ) {

        string block_id(padded_str_to_str(
            old_block_record_p->block_id, sizeof ( old_block_record_p->block_id )
          ));

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::BlockStateChangedEventInfo::Impl(
                        block_id,
                        bgsched::Block::Impl::statusDbCharToValue( new_status_char ),
                        new_seq_id,
                        bgsched::Block::Impl::statusDbCharToValue( old_status_char ),
                        old_seq_id
                    )
            );

        _transactions.change( tid, db_change_ptr );
      } else {
        LOG_TRACE_MSG( "Ignoring block update change:" << old_status_char << "(" << old_seq_id << ") -> " << new_status_char << "(" << new_seq_id << ")"  );
      }
    } else if ( TableInfos::table::JOB == table ) {
      const job_record_t *old_job_record_p(
          reinterpret_cast<const job_record_t*>(old_user_data_buffer)
        );


      bgsched::Job::Id job_id(old_job_record_p->id);

      char old_status(old_job_record_p->status);
      bgsched::SequenceId old_seq_id(old_job_record_p->seq_id);

      const job_record_t *new_job_record_p(
          reinterpret_cast<const job_record_t*>(new_user_data_buffer)
        );

      char new_status(new_job_record_p->status);
      bgsched::SequenceId new_seq_id(new_job_record_p->seq_id);

      if ( (new_status != old_status) ||
           (new_seq_id != old_seq_id) ) {
        string block_id(padded_str_to_str(
            old_job_record_p->block_id, sizeof ( old_job_record_p->block_id )
          ));

        LOG_TRACE_MSG(
            "updated job " << job_id << " block '" << block_id << "':" <<
            " old_status=" << old_status << " new_status=" << new_status <<
            " old_seqId=" << old_seq_id << " new_seqId=" << new_seq_id
          );

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::JobStateChangedEventInfo::Impl(
                        job_id, block_id,
                        bgsched::Job::Impl::statusDbCharToValue( new_status ),
                        new_seq_id,
                        bgsched::Job::Impl::statusDbCharToValue( old_status ),
                        old_seq_id
                    )
            );

        _transactions.change( tid, db_change_ptr );
      } else {
        LOG_TRACE_MSG( "Ignoring change to job" );
      }
    } else if ( table == TableInfos::table::MIDPLANE ) {
      const midplane_record_t *old_mp_record_p(
          reinterpret_cast<const midplane_record_t*>(old_user_data_buffer)
        );

      char old_status(old_mp_record_p->status);
      bgsched::SequenceId old_seq_id(old_mp_record_p->seq_id);

      const midplane_record_t *new_mp_record_p(
          reinterpret_cast<const midplane_record_t*>(new_user_data_buffer)
        );

      char new_status(new_mp_record_p->status);
      bgsched::SequenceId new_seq_id(new_mp_record_p->seq_id);

      if ( (old_status != new_status) || (old_seq_id != new_seq_id) ) {
        string bp_pos(padded_str_to_str( old_mp_record_p->pos_in_machine, sizeof ( old_mp_record_p->pos_in_machine ) ));

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::MidplaneStateChangedEventInfo::Impl(
                        bp_pos,
                        hardwareStateCodeToValue( new_status ),
                        new_seq_id,
                        hardwareStateCodeToValue( old_status ),
                        old_seq_id
                    )
            );

        _transactions.change( tid, db_change_ptr );
      } else {
        LOG_TRACE_MSG( "Change to midplane ignored" );
      }

    } else if ( table == TableInfos::table::NODE_CARD ) {

        const node_card_record_t *old_nc_record_p(
            reinterpret_cast<const node_card_record_t*>(old_user_data_buffer)
          );

        char old_status(old_nc_record_p->status);
        bgsched::SequenceId old_seq_id(old_nc_record_p->seq_id);

        const node_card_record_t *new_nc_record_p(
            reinterpret_cast<const node_card_record_t*>(new_user_data_buffer)
          );

        char new_status(new_nc_record_p->status);
        bgsched::SequenceId new_seq_id(new_nc_record_p->seq_id);

        if ( old_status != new_status || old_seq_id != new_seq_id ) {

          string midplane_location(padded_str_to_str( old_nc_record_p->midplane_pos, sizeof ( old_nc_record_p->midplane_pos ) ));
          string node_board_position(padded_str_to_str( old_nc_record_p->position, sizeof ( old_nc_record_p->position ) ));

          string node_board_location(midplane_location + "-" + node_board_position);

          bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                  new bgsched::realtime::ClientEventListener::NodeBoardStateChangedEventInfo::Impl(
                          node_board_location,
                          hardwareStateCodeToValue( new_status ),
                          new_seq_id,
                          hardwareStateCodeToValue( old_status ),
                          old_seq_id
                      )
              );

          _transactions.change( tid, db_change_ptr );

        } else {
          LOG_TRACE_MSG( "Change to node card ignored" );
        }

    } else if ( table == TableInfos::table::SWITCH ) {

      const switch_record_t *old_switch_record_p(
          reinterpret_cast<const switch_record_t*>(old_user_data_buffer)
        );

      char old_status(old_switch_record_p->status);
      bgsched::SequenceId old_seq_id(old_switch_record_p->seq_id);

      const switch_record_t *new_switch_record_p(
          reinterpret_cast<const switch_record_t*>(new_user_data_buffer)
        );

      char new_status(new_switch_record_p->status);
      bgsched::SequenceId new_seq_id(new_switch_record_p->seq_id);

      if ( (old_status != new_status) || (old_seq_id != new_seq_id) ) {
        string midplane_location(padded_str_to_str( old_switch_record_p->midplane_pos, sizeof ( old_switch_record_p->midplane_pos ) ));

        bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                new bgsched::realtime::ClientEventListener::SwitchStateChangedEventInfo::Impl(
                        midplane_location,
                        dimensionCodeToValue( old_switch_record_p->dimension ),
                        hardwareStateCodeToValue( new_status ),
                        new_seq_id,
                        hardwareStateCodeToValue( old_status ),
                        old_seq_id
                    )
            );

        _transactions.change( tid, db_change_ptr );
      } else {
        LOG_TRACE_MSG( "Change to midplane ignored" );
      }

    } else if ( table == TableInfos::table::CABLE ) {

        const cable_record_t *old_cable_record_p(
                reinterpret_cast<const cable_record_t*>(old_user_data_buffer)
            );

        char old_status(old_cable_record_p->status);
        bgsched::SequenceId old_seq_id(old_cable_record_p->seq_id);

        const cable_record_t *new_cable_record_p(
                reinterpret_cast<const cable_record_t*>(new_user_data_buffer)
            );

        char new_status(new_cable_record_p->status);
        bgsched::SequenceId new_seq_id(new_cable_record_p->seq_id);

        if ( (old_status != new_status) || (old_seq_id != new_seq_id) ) {
            string from_location(padded_str_to_str( old_cable_record_p->from_location, sizeof ( old_cable_record_p->from_location ) ));
            string to_location(padded_str_to_str( old_cable_record_p->to_location, sizeof ( old_cable_record_p->to_location ) ));

            bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr;

            // If to_location is like R00-M0-N00-T04 then it's a torus cable. Just check that 5th char is M. An I/O cable would be like R00-ID-T04 or Q00-I0-T04.
            if ( to_location[4] == 'M' ) {
                db_change_ptr = bgsched::realtime::AbstractDatabaseChange::Ptr(
                        new bgsched::realtime::ClientEventListener::TorusCableStateChangedEventInfo::Impl(
                                from_location,
                                to_location,
                                hardwareStateCodeToValue( new_status ),
                                new_seq_id,
                                hardwareStateCodeToValue( old_status ),
                                old_seq_id
                            )
                    );
            } else {
                db_change_ptr = bgsched::realtime::AbstractDatabaseChange::Ptr(
                        new bgsched::realtime::ClientEventListener::IoCableStateChangedEventInfo::Impl(
                                from_location,
                                to_location,
                                hardwareStateCodeToValue( new_status ),
                                new_seq_id,
                                hardwareStateCodeToValue( old_status ),
                                old_seq_id
                            )
                    );
            }

            _transactions.change( tid, db_change_ptr );
        }

    } else if ( table == TableInfos::table::NODE ) {

        const node_record_t *old_record_p(
            reinterpret_cast<const node_record_t*>(old_user_data_buffer)
          );

        char old_status(old_record_p->status);
        bgsched::SequenceId old_seq_id(old_record_p->seq_id);

        const node_record_t *new_record_p(
            reinterpret_cast<const node_record_t*>(new_user_data_buffer)
          );

        char new_status(new_record_p->status);
        bgsched::SequenceId new_seq_id(new_record_p->seq_id);

        if ( (old_status != new_status) || (old_seq_id != new_seq_id) ) {
          string node_location(
                      padded_str_to_str( old_record_p->midplane_pos, sizeof ( old_record_p->midplane_pos ) ) + "-" +
                      padded_str_to_str( old_record_p->node_card_pos, sizeof ( old_record_p->node_card_pos ) ) + "-" +
                      padded_str_to_str( old_record_p->position, sizeof ( old_record_p->position ) )
                  );

          bgsched::realtime::AbstractDatabaseChange::Ptr db_change_ptr(
                  new bgsched::realtime::ClientEventListener::NodeStateChangedEventInfo::Impl(
                          node_location,
                          hardwareStateCodeToValue( new_status ),
                          new_seq_id,
                          hardwareStateCodeToValue( old_status ),
                          old_seq_id
                      )
              );

          _transactions.change( tid, db_change_ptr );
        } else {
          LOG_TRACE_MSG( "Change to midplane ignored" );
        }

      } else {
        LOG_TRACE_MSG( "ignoring update because it's to an unknown table" );
    }
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
