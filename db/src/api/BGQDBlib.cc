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

#include "BGQDBlib.h"
#include "dataconv.h"
#include "extract_db.h"
#include "swfailure.h"
#include "utility.h"

#include "cxxdb/cxxdb.h"

#include "tableapi/gensrc/bgqtableapi.h"
#include "tableapi/dbbasic.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/TxObject.h"
#include "tableapi/utilcli.h"

#include <bgq_util/include/string_tokenizer.h>
#include <bgq_util/include/Location.h>

#include <control/include/mcServer/defaults.h>

#include <utility/include/Log.h>
#include <utility/include/XMLEntity.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/once.hpp>

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <execinfo.h>

using namespace std;

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

const uint32_t  Nodes_Per_Midplane = 512;
const uint32_t  Midplane_A_Dimension = 4;
const uint32_t  Midplane_B_Dimension = 4;
const uint32_t  Midplane_C_Dimension = 4;
const uint32_t  Midplane_D_Dimension = 4;
const uint32_t  Midplane_E_Dimension = 2;
const uint32_t  Nodes_Per_Node_Board = 32;
const char* BLOCK_FREE =             "F";
const char* BLOCK_ALLOCATED =        "A";
const char* BLOCK_BOOTING =          "B";
const char* BLOCK_BOOTING_NO_CHECK = "N";
const char* BLOCK_DEALLOCATING =     "D";
const char* BLOCK_INITIALIZED =      "I";
const char* BLOCK_TERMINATING =      "T";
const char* BLOCK_NO_ACTION =        " ";
const char* HARDWARE_AVAILABLE =     "A";
const char* HARDWARE_MISSING =       "M";
const char* HARDWARE_ERROR =         "E";
const char* HARDWARE_SERVICE =       "S";
const char* SOFTWARE_FAILURE =       "F";
const std::string DEFAULT_MLOADERIMG = "/bgsys/drivers/ppcfloor/boot/firmware";
const std::string DEFAULT_COMPUTENODECONFIG = "CNKDefault";
const std::string DEFAULT_IONODECONFIG = "IODefault";


namespace replacement_history_types {
    const std::string Midplane("Midplane");
    const std::string NodeCard("NodeCard");
    const std::string NodeBoardDCA("NodeBoardDCA");
    const std::string Node("Node");
    const std::string IoNode("IONode");
    const std::string IoRack("IORack");
    const std::string IoDrawer("IODrawer");
    const std::string ServiceCard("ServiceCard");
    const std::string ClockCard("ClockCard");
    const std::string LinkChip("LinkChip");
    const std::string IoLinkChip("IOLinkChip");
    const std::string BulkPower("BulkPower");
}



// see initializeMachineSizeInMidplanes
static DimensionSizes s_machine_size_in_midplanes;
static STATUS s_machine_size_in_midplanes_status;
static boost::once_flag s_machine_size_in_midplanes_once_flag;


static SQLRETURN
calcMachineBPDimensionSize(
        const std::string& start_midplane_location,
        char dimension,
        TxObject &tx,
        uint32_t& size_out
)
{
    // Walk through the links in the dimension starting from start_midplane_location to calculate the dimension size.
    int dim_size = 1;

    DBVSwitchlinks sl;

    ColumnsBitmap colBitmap;
    colBitmap.set(sl.DESTINATION);

    sl._columns = colBitmap.to_ulong();

    string midplane_location = start_midplane_location;

    SQLRETURN sqlrc;

    while (true) {
        // Figure out the next midplane, its the destination from this source.
        string condition = string(" where source='") + dimension + "_" + midplane_location + "'";
        sqlrc = tx.query(&sl,condition.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return sqlrc;
        }

        sqlrc = tx.fetch(&sl);
        if (sqlrc == SQL_NO_DATA) {
            if (dim_size == 1) {
                // In this case there are no links in the dimension so the dimension size is 1.
                sqlrc = SQL_SUCCESS;
                break;
            }

            // The database is corrupt, since ran out of links.
            LOG_ERROR_MSG( "Fetched no data getting links, the database is not correct. At " << __FUNCTION__ << ':' << __LINE__ );
            return sqlrc;
        }
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return sqlrc;
        }

        midplane_location = sl._destination + 2; // extract midplane location, it's after <dim>_

        if (midplane_location == start_midplane_location) {
            // We're back at the beginning, so we're done.
            break;
        }

        ++dim_size;

        if ( dim_size == 1000 ) {
            LOG_ERROR_MSG( "Dimension size > 1000, flagging as invalid link table contents. at " << __FUNCTION__ << ':' << __LINE__ );
            return SQL_ERROR;
        }
    }

    size_out = dim_size;

    return sqlrc;
}


static void
initializeMachineSizeInMidplanes()
{
    // Calculate the machine size in each dimension in midplanes.

    SQLRETURN sqlrc;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        s_machine_size_in_midplanes_status = CONNECTION_ERROR;
        return;
    }

    DBVBasepartition bp;

    ColumnsBitmap bp_cols;
    bp_cols.set(bp.BPID);
    bp._columns = bp_cols.to_ulong();

    // First find the midplane at R00-M0
    string condition = " where row=0 and column=0 and midplane=0";
    if ((sqlrc = tx.query(&bp,condition.c_str()))!= SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        s_machine_size_in_midplanes_status = DB_ERROR;
        return;
    }
    if ((sqlrc = tx.fetch(&bp))!= SQL_SUCCESS){
        LOG_ERROR_MSG(__FUNCTION__ << " Midplane not found");
        s_machine_size_in_midplanes_status = NOT_FOUND;
        return;
    }

    string basebp = bp._bpid;

    // Walk through the links in each dimension to calculate each dimension's size.
    sqlrc = calcMachineBPDimensionSize( basebp, 'A', tx, s_machine_size_in_midplanes[Dimension::A] );
    if ( sqlrc != SQL_SUCCESS ) { s_machine_size_in_midplanes_status = DB_ERROR; return; }

    sqlrc = calcMachineBPDimensionSize( basebp, 'B', tx, s_machine_size_in_midplanes[Dimension::B] );
    if ( sqlrc != SQL_SUCCESS ) { s_machine_size_in_midplanes_status = DB_ERROR; return; }

    sqlrc = calcMachineBPDimensionSize( basebp, 'C', tx, s_machine_size_in_midplanes[Dimension::C] );
    if ( sqlrc != SQL_SUCCESS ) { s_machine_size_in_midplanes_status = DB_ERROR; return; }

    sqlrc = calcMachineBPDimensionSize( basebp, 'D', tx, s_machine_size_in_midplanes[Dimension::D] );
    if ( sqlrc != SQL_SUCCESS ) { s_machine_size_in_midplanes_status = DB_ERROR; return; }

    s_machine_size_in_midplanes_status = OK;
}


static STATUS
extractStatusToDBStatus(
        extract_db_status extract_db_status
)
{
    switch ( extract_db_status ) {
    case DB_OK:
        return OK;
    case DB_NO_DATA:
        return NOT_FOUND;
    case DB_COMM_ERR:
        return CONNECTION_ERROR;
    default:
        return DB_ERROR;
    }
}


bool
checkIdentifierValidCharacters(
        const std::string& id
)
{
    // Check that the block_id only has valid characters.
    return (id.size() == strspn(id.c_str(),"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_"));
}


bool
isBlockIdValid(
        const std::string& blockid,
        const char *function_name
)
{
    if ( blockid.empty() ) {
        LOG_ERROR_MSG(function_name << ": empty name is invalid");
        return false;
    }

    if (!checkIdentifierValidCharacters(blockid)) {
        LOG_ERROR_MSG(function_name << ": block name '" << blockid << "' contains invalid characters");
        return false;
    }

    static const unsigned max_block_id_length = sizeof( DBTBlock()._blockid ) - 1;

    if (blockid.size() > max_block_id_length) {
        LOG_ERROR_MSG(function_name << ": block name '" << blockid << "' exceeds allowed size of " << max_block_id_length);
        return false;
    }

    if (blockid == mc_server::DefaultListener) {
        LOG_ERROR_MSG(function_name << ": block name " << blockid << " is reserved for internal usage");
        return false;
    }

    return true;
}

STATUS
getMachineXML(
        std::ostream& xml,
        std::vector<std::string>* memory
)
{
    if (extract_compact_machine(xml, memory) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting machine XML");
        return DB_ERROR;
    }
    return OK;
}


STATUS
getBPs(
        std::ostream& outBPXML
)
{
    extract_db_status result;
    if ((result = extract_db_bplist(outBPXML)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting midplanes");
        return extractStatusToDBStatus(result);
    }

    return OK;
}

STATUS
getBPNodeCards(
        std::ostream& xml,
        const std::string& midplane
)
{
    extract_db_status result;
    if ((result = extract_db_nodecards(xml, midplane)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting node boards");
        return extractStatusToDBStatus(result);
    }

    return OK;
}

STATUS
getSwitches(
        std::ostream& outSwitchXML
)
{
    extract_db_status result;
    if ((result = extract_db_switches(outSwitchXML)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting switches");
        return extractStatusToDBStatus(result);
    }

    return OK;
}

STATUS
getBPWireList(
        std::ostream& outWireXML
)
{
    extract_db_status result;
    if ((result = extract_db_bpwires(outWireXML)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting midplane cables");
        return extractStatusToDBStatus(result);
    }

    return OK;
}

STATUS
getIOWireList(
        std::ostream& xml,
        const std::string& midplane
)
{
    extract_db_status result;
    if ((result = extract_db_iowires(xml, midplane)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting I/O links");
        return extractStatusToDBStatus(result);
    }

    return OK;
}

STATUS
checkIOLinks(
        std::ostream& xml,
        const std::string& block
)
{
    extract_db_status result;
    if ((result = extract_db_iolinks(xml, block)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred checking I/O links");
        return extractStatusToDBStatus( result );
    }

    return OK;
}


STATUS
getMachineBPSize(
        DimensionSizes& midplane_sizes_out
)
{
    boost::call_once( s_machine_size_in_midplanes_once_flag, &initializeMachineSizeInMidplanes );

    if ( s_machine_size_in_midplanes_status != OK ) {
        return s_machine_size_in_midplanes_status;
    }

    midplane_sizes_out = s_machine_size_in_midplanes;

    return OK;
}


STATUS
getMachineBPSize(
        uint32_t& A,
        uint32_t& B,
        uint32_t& C,
        uint32_t& D
)
{
    DimensionSizes sizes;

    STATUS db_status = getMachineBPSize( sizes );

    if ( db_status != OK ) {
        return db_status;
    }

    A = sizes[Dimension::A];
    B = sizes[Dimension::B];
    C = sizes[Dimension::C];
    D = sizes[Dimension::D];

    return db_status;

    return OK;
}


STATUS
getBlockIds(
        const string& whereClause,
        std::vector<string>& outBlockList
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    int result;

    colBitmap.set(dbo.BLOCKID);
    dbo._columns = colBitmap.to_ulong();

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    outBlockList.clear();        // clear the output vector

    result = tx.query(&dbo, whereClause.c_str());
    while (result == SQL_SUCCESS)
    {
        result = tx.fetch(&dbo);
        if (result == SQL_SUCCESS) {
            trim_right_spaces((char *)dbo._blockid);
            outBlockList.push_back(string(dbo._blockid));
        }
    }
    if (result == SQL_NO_DATA_FOUND) {
        return OK;
    } else {
        LOG_ERROR_MSG( "Database error: " << result << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
}

STATUS
getBlockXML(
        std::ostream& xml,
        const std::string& block,
        bool diags
)
{
    extract_db_status result;

    result = extract_compact_block(xml, block, diags);
    if (result != DB_OK) {
        // Error is logged from extract_compact_bpblock
        return extractStatusToDBStatus(result);
    }

    return OK;
}

STATUS
deleteBlock(
        const std::string& block
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (block.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", block.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // ok, we got valid block info from the DB
    if (strcmp(dbo._status, BLOCK_FREE) != 0) {
        LOG_ERROR_MSG(__FUNCTION__ << " Block " << block << " must be in Free state to be removed. Current state is: " << dbo._status);
        return FAILED;
    }

    sqlrc = tx.delByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database delete failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ( (sqlrc = tx.commit()) != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__  );
        return DB_ERROR;
    }

    return OK;
}


STATUS
getBlockAction(
        std::string& id,
        BLOCK_ACTION& action,
        const string& exclude
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    string whereClause(" where ");
    if ( !exclude.empty() ) {
        whereClause+= string("  not blockid in ") + exclude;
        whereClause+= string(" and ");
    }

    whereClause+= string("(action = '");


    // fall-through return info
    id.clear();
    action = NO_BLOCK_ACTION;

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.ACTION);
    colBitmap.set(dbo.CREATIONID);
    dbo._columns = colBitmap.to_ulong();

    whereClause += BLOCK_BOOTING;
    whereClause += "' or action = '";
    whereClause += BLOCK_BOOTING_NO_CHECK;
    whereClause += "' or action = '";
    whereClause += BLOCK_DEALLOCATING;
    whereClause += "') order by statuslastmodified";

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.query(&dbo, whereClause.c_str());
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        return OK;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // Block to which the action has to be performed
    trim_right_spaces(dbo._blockid);
    id = std::string(dbo._blockid, strnlen( dbo._blockid, sizeof(dbo._blockid) ) );

    // Now we have to find out which action it was
    trim_right_spaces(dbo._action);
    if (strcmp(dbo._action, BLOCK_BOOTING) == 0) {
        action = CONFIGURE_BLOCK;
    } else if(strcmp(dbo._action, BLOCK_BOOTING_NO_CHECK) == 0) {
        action = CONFIGURE_BLOCK_NO_CHECK;
    } else if(strcmp(dbo._action, BLOCK_DEALLOCATING) == 0) {
        action = DEALLOCATE_BLOCK;
    } else {
        action = NO_BLOCK_ACTION;
    }

    // now clear the action field
    //    strcpy(dbo._action, NO_ACTION);
    //    sqlrc = tx.updateByKey(&dbo);
    //    if (sqlrc != SQL_SUCCESS) {
    //        LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
    //        return DB_ERROR;
    //    }
    //
    // THE CODE ABOVE WAS COMMENTED OUT BECAUSE THE clearBlockAction MUST BE CALLED TO CLEAR THE ACTION FIELD

    return OK;
}


STATUS
clearBlockAction(
                 const std::string& id
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.ACTION);
    dbo._columns = colBitmap.to_ulong();

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc;
    if (id == "*ALL") {
        // Clear the action for all blocks
        string upd = string("update bgqblock set action = ' '");
        sqlrc = tx.execStmt(upd.c_str());
        if (sqlrc != SQL_SUCCESS && sqlrc != SQL_NO_DATA_FOUND)  {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    } else {
        // Clear the action field for a single block
        snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
        strcpy(dbo._action, BLOCK_NO_ACTION);
        sqlrc = tx.updateByKey(&dbo);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}

//  The following accepts a vector of IO node locations and returns the connected compute blocks that are booted.
//  This is to prevent IO node(s) from being rebooted while there are connected compute blocks.
STATUS
checkIONodeConnection(
                      const std::vector<std::string>& locations,
                      std::vector<std::string>& connected
                      )
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    SQLLEN ind1;

    string id;
    int index = 0;
    id = "(";
    for (std::vector<std::string>::const_iterator loc = locations.begin(); loc != locations.end(); ++loc, ++index) {
        id += string("'");
        id += *loc;
        id += string("'");
        if ( (index+1) == (int)locations.size()) {
            id += ")";
        } else {
            id += ",";
        }
    }

    if (index == 0) {
        LOG_ERROR_MSG(__FUNCTION__ << " no IO node locations provided");
        return INVALID_ID;
    }


    // The query allows for a location to be either an IO node or an IO board
    string sqlstr;
    sqlstr = "select c.blockid  from  bgqcniolink b, bgqsmallblock c, bgqblock d  " +
        string(" where ionstatus = 'A' and ( b.ion in ") + id + string(" OR  substr(b.ion,1,6) in ") + id + string(" ) ") +
        string("  and substr(source,1,10) = posinmachine || '-' || nodecardpos  and c.blockid = d.blockid and d.status in ( 'B','I','T') ") +
        string("  union select c.blockid  from  bgqcniolink b, bgqbpblockmap c, bgqblock d ") +
        string(" where  ionstatus = 'A' and ( b.ion in ") + id + string(" OR  substr(b.ion,1,6) in ") + id + string(" ) ") +
        string("  and substr(source,1,6) = bpid  and c.blockid = d.blockid and d.status in ('B','I','T')");
    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    char cnBlock[33];

    // bind column
    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, cnBlock, sizeof(cnBlock), &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc =  SQLFetch(hstmt);
    for(;sqlrc == SQL_SUCCESS;) {
        trim_right_spaces(cnBlock);
        connected.push_back(cnBlock);
        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);
    return OK;
}



//  The following accepts an IO block name and returns the connected compute blocks that are booted.
//  This is to prevent an IO block from being freed while there are connected compute blocks.
STATUS
checkIOBlockConnection(
                     const std::string& id,
                     std::vector<string>* connected
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    SQLLEN ind1;

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    dbo._columns = colBitmap.to_ulong();
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    tx.close(&dbo);

    string sqlstr;
    sqlstr = "select c.blockid  from tbgqioblockmap a, bgqcniolink b, bgqsmallblock c, bgqblock d    where  a.blockid = '" + id +
        string("' and (ion = location  or substr(ion,1,6) = location) and ") +
        string("  substr(source,1,10) = posinmachine || '-' || nodecardpos  and c.blockid = d.blockid and d.status in ('B','I','T') and b.status = 'A' ") +
        string("  union select c.blockid  from tbgqioblockmap a, bgqcniolink b, bgqbpblockmap c, bgqblock d  where  a.blockid = '") + id +
        string("' and (ion = location  or substr(ion,1,6) = location) and substr(source,1,6) = bpid  and c.blockid = d.blockid and d.status in ('B','I','T') and b.status = 'A' ");
    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    char cnBlock[33];

    // bind column
    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, cnBlock, sizeof(cnBlock), &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc =  SQLFetch(hstmt);
    for(;sqlrc == SQL_SUCCESS;) {
        trim_right_spaces(cnBlock);
        connected->push_back(cnBlock);
        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);
    return OK;
}


STATUS
checkBlockConnection(
                     const std::string& id,
                     std::vector<string>* unconnected
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    SQLLEN ind1;

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    dbo._columns = colBitmap.to_ulong();
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());

    sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    tx.close(&dbo);

    string sqlstr;
    sqlstr = "select distinct a.ion from  (select ion from bgqbpblockmap a, bgqcniolink b where bpid = substr(source,1,6) and blockid = '" + id +
        string("' union all select ion from bgqsmallblock a, bgqcniolink b where posinmachine = substr(source,1,6) and nodecardpos = substr(source,8,3) and blockid = '") + id +
        string("') as a left outer join (select location from bgqioblockmap b, bgqblock c where b.blockid = c.blockid and status = 'I' and action <> '") + BLOCK_DEALLOCATING + 
        string("') as b  on  a.ion = b.location or substr(a.ion,1,6) = b.location ") +
        string(" left outer join bgqionode n on a.ion = n.location and status <> 'A' where (n.location is NOT NULL) or (b.location is NULL and n.location is NULL)  order by 1");

    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    char ion[17];

    // bind column
    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, ion, sizeof(ion), &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc =  SQLFetch(hstmt);
    for(;sqlrc == SQL_SUCCESS;) {
        unconnected->push_back(ion);
        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);
    return OK;
}

STATUS
checkBlockIO(
        const string& block,
        std::vector<string>* unconnectedIONodes,
        std::vector<string>* midplanesFailingIORules,
        std::vector<string>* unconnectedAvailableIONodes
)
{
    bool isLargeBlock = true;
    std::map<std::string, MidplaneIOInfo> midplaneMap; // Midplane to connected I/O node mapping
    std::map<std::string, MidplaneIOInfo>::iterator mapIter;

    std::map<std::string, int> availableIONMap; // Map with list of 'Available' but unconnected I/O nodes for the block
    std::map<std::string, int>::iterator IONIter;

    deque<std::string> unconnectedION;

    STATUS result;
    BGQDB::DBTBlock dbo;
    ColumnsBitmap colBitmap;

    // Validate the block name
    if (block.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return BGQDB::INVALID_ID;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return BGQDB::CONNECTION_ERROR;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMCNODES); // Used to calculate small or large block

    dbo._columns = colBitmap.to_ulong();
    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", block.c_str());

    // Query the block table to find block
    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return BGQDB::DB_ERROR;
    }

    // Get the block
    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name not found");
        return BGQDB::NOT_FOUND;
    }

    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return BGQDB::DB_ERROR;
    }

    // OK, we got valid block info from the database now check if large or small block
    if (dbo._numcnodes < 512) {
        isLargeBlock = false;
    }

    // Done with the block table so close it
    tx.close(&dbo);

    // Get the list of unconnected I/O for the block
    result = BGQDB::checkBlockConnection(block, unconnectedIONodes);
    if (result != BGQDB::OK) {
        return result;
    }

    // Check if we got back unconnected I/O nodes
    if ((*unconnectedIONodes).size() > 0) {
        // Put the unconnected I/O nodes in a deque for faster lookup
        for (std::vector<string>::iterator it = (*unconnectedIONodes).begin(); it != (*unconnectedIONodes).end(); ++it) {
            unconnectedION.push_back(*it);
        }
    }

    // Now get all the I/O links for the block
    SQLHANDLE hstmt;

    BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());
    if (!tx2.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return BGQDB::CONNECTION_ERROR;
    }

    string sqlstr = "select source, status, ion, ionstatus from bgqcniolink where (substr(source,1,6) in (select bpid from bgqbpblockmap where blockid = '"
        + block
        + string("' )  or substr(source,1,10) in (select  posinmachine || '-' || nodecardpos from bgqsmallblock where blockid = '")
        + block
        + string("') )");

    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG(__FUNCTION__ << " No I/O links found for block " << block);
        return BGQDB::DB_ERROR;
    }

    // Columns
    char  source[15];
    char  status[2];
    char  ion[11];
    char  ionstatus[2];

    // Bind columns
    SQLLEN ind1, ind2, ind3, ind4;
    SQLBindCol(hstmt, 1, SQL_C_CHAR, source, sizeof(source), &ind1);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, status, sizeof(status), &ind2);
    SQLBindCol(hstmt, 3, SQL_C_CHAR, ion, sizeof(ion), &ind3);
    SQLBindCol(hstmt, 4, SQL_C_CHAR, ionstatus, sizeof(ionstatus), &ind4);

    // Get all the I/O links and buildup the available I/O links for each midplane
    sqlrc =  SQLFetch(hstmt);
    for (;sqlrc == SQL_SUCCESS;) {
        string IOLink = source;
        string midplane = IOLink.substr(0,6);
        string IOLinkStatus = status;
        string IONode = ion;
        string IONodeStatus = ionstatus;
        //LOG_INFO_MSG("IO link: " << IOLink << " Mp: " << midplane << " IO node: " << IONode << " IO link status: " << IOLinkStatus << " IO node status: " << IONodeStatus);

        // Lookup the midplane in the map
        mapIter = midplaneMap.find(midplane);
        if (mapIter == midplaneMap.end()) {
            // Didn't find one in the map so add a new midplane key
            midplaneMap.insert(pair<std::string, MidplaneIOInfo>(midplane, MidplaneIOInfo()));
            mapIter = midplaneMap.find(midplane);
        }

        // Determine if the midplane I/O link count should be incremented. The condition where the count can be
        // incremented is when the following rules are met:
        // 1) The I/O link has (A)vailable status
        // 2) The I/O node has (A)vailable status
        // 3) The I/O node is connected (e.g not in the unconnected I/O node list)

        // First check if I/O node is connected. If I/O node is unconnected we have to verify that either
        // the link or the I/O node is bad. Letting a "good" I/O node get through that is not booted will
        // cause problems in training if the compute block is allowed to boot.
        if (!unconnectedION.empty()) {
            if (find(unconnectedION.begin(), unconnectedION.end(), IONode) != unconnectedION.end()) {
                // The I/O node is unconnected so don't increment the I/O link count for this midplane. If the
                // unconnected I/O node has both "Available" I/O link status and "Available" I/O node
                // status we need to stop the compute block from booting.
                if ((IOLinkStatus.compare(0,1,"A") == 0) && (IONodeStatus.compare(0,1,"A") == 0)) {
                    // Force the midplane link count negative so the midplane fails the I/O rules
                    (mapIter->second).IOLinkCount = -9999;
                    // Add the I/O node location to unconnected but 'Available' I/O node map - ignore dup return code
                    availableIONMap.insert(pair<string, int>(IONode, 0));
                }
                sqlrc = SQLFetch(hstmt);
                continue;
            }
        }

        // The I/O node is connected so now check that the I/O link and I/O node status are both Available
        if ((IOLinkStatus.compare(0,1,"A") == 0) && (IONodeStatus.compare(0,1,"A") == 0)) {
            // All the conditions pass so increment the I/O link count for the midplane
            (mapIter->second).IOLinkCount = (mapIter->second).IOLinkCount + 1;
            // Associate the I/O node to the midplane
            (mapIter->second).IONodes.push_back(IONode);
        }

        // Get the next I/O link
        sqlrc = SQLFetch(hstmt);
    }

    int requiredIOLinks = 1;

    if (isLargeBlock) {
        // Large blocks (512 cnodes or bigger) require 2 connected I/O links per midplane
        requiredIOLinks = 2;
    } else {
        // Small blocks (256 cnodes or smaller) require 1 connected I/O link
        requiredIOLinks = 1;
    }

    // Iterate thru the midplane map and check I/O links for each midplane
    for (mapIter = midplaneMap.begin(); mapIter != midplaneMap.end(); mapIter++) {
        //LOG_TRACE_MSG("Midplane " << mapIter->first << " has " << (mapIter->second).IOLinkCount << " connected I/O links");
        //for (std::vector<std::string>::iterator it = (mapIter->second).IONodes.begin(); it != (mapIter->second).IONodes.end(); ++it) {
        //    LOG_INFO_MSG("Midplane " << mapIter->first << " connected to I/O node " << *it);
        //}

        if ((mapIter->second).IOLinkCount < requiredIOLinks) {
            // Add this to list of midplanes failing the I/O rule
            midplanesFailingIORules->push_back(mapIter->first);
        } else {
            // Need to perform one last check for large blocks to make sure that if only 2 I/O links that they
            // are connected to two different I/O nodes. Two I/O links to same I/O node would be 1:512 I/O ratio
            // that is not supported.
            if (isLargeBlock && (mapIter->second).IOLinkCount == requiredIOLinks) {
                // Only 2 I/O links so compare I/O node names
                //LOG_INFO_MSG("Midplane " << mapIter->first << " has connected I/O links to I/O node " << (mapIter->second).IONodes[0] << " and " << (mapIter->second).IONodes[1]);
                if ((mapIter->second).IONodes[0].compare((mapIter->second).IONodes[1]) == 0) {
                    // Add this to list of midplanes failing the I/O rule
                    midplanesFailingIORules->push_back(mapIter->first);
                }
            }
        }
    }

    // Iterate thru the unconnected but 'Available' I/O node map and return locations in vector
    for (IONIter = availableIONMap.begin(); IONIter != availableIONMap.end(); IONIter++) {
        //LOG_INFO_MSG("I/O node " << IONIter->first << " is 'Available' but is not booted");
        unconnectedAvailableIONodes->push_back(IONIter->first);
    }

    SQLCloseCursor(hstmt);
    return BGQDB::OK;
}


STATUS
getBlockStatus(
        const std::string& id,
        BLOCK_STATUS& currentState
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    currentState = blockCodeToStatus( dbo._status );
    if ( currentState == INVALID_STATE ) {
        LOG_ERROR_MSG(__FUNCTION__ << " block is in unknown state");
        return FAILED;
    }

    return OK;
}

STATUS
getJtagID(
            const std::vector<std::string>& locations,
            std::map<std::string,uint32_t>&   jtag
            )
{
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    SQLLEN ind1,ind2;

    string sqlstr;
    sqlstr = "select a.location, max(case substr(hex(substr(ecid,19,2)),2,2) when '00' then 0 else 1 end) from bgqnodecard a, bgqnodeall b  " +
        string("where  a.position = b.nodecardpos and a.midplanepos = b.midplanepos group by a.location ") +
        string("  union all  select a.location, max(case substr(hex(substr(ecid,19,2)),2,2) when '00' then 0 else 1 end)  ") +
        string(" from bgqiodrawer a, bgqionodeall b  where  a.location = b.iopos  group by a.location  ");


    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    char loc[17];
    int  ecidVersion;

    uint32_t ecid0 = 0x09003049, ecidx = 0x29003049;


    // bind column
    sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, loc, sizeof(loc), &ind1);
    sqlrc = SQLBindCol(hstmt, 2, SQL_INTEGER, &ecidVersion, 4, &ind2);

    sqlrc =  SQLFetch(hstmt);
    for(;sqlrc == SQL_SUCCESS;) {

        // check if its in the list
        if(std::find(locations.begin(), locations.end(), string(loc)) != locations.end()) {
            jtag[string(loc)] = (ecidVersion == 0 ? ecid0 : ecidx);
        }

        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);
    return OK;

}

STATUS
getCustomization(
                 const std::string& id,
                 std::map<std::string,std::string>& nodeCust
                 )
{
    DBTBlock dbo;
    char  nameValuePair[273];
    char  location[17];
    char  prevLocation[17];
    SQLLEN ind1, ind2;
    SQLHANDLE hstmt;
    TxObject tx(DBConnectionPool::Instance());

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    string cust;

    string sqlstr = "select a.location, case when length(rtrim(interface)) > 0 then rtrim(interface) || '_' else '' end concat  rtrim(itemname) || '=' || rtrim(itemvalue) from bgqnetconfig a, bgqioblockmap b where b.blockid = '"
        + id + string("' and (a.location = b.location or substr(a.location,1,6) = b.location)  order by 1, 2");

    SQLRETURN sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);

    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, location,      sizeof(location),      &ind1);
    sqlrc = SQLBindCol(hstmt, 2, SQL_C_CHAR, nameValuePair, sizeof(nameValuePair), &ind2);
    sqlrc = SQLFetch(hstmt);

    if (sqlrc != SQL_SUCCESS) {  // nothing to return
        SQLCloseCursor(hstmt);
        return OK;
    }

    strcpy(prevLocation, location);
    cust.clear();
    while( sqlrc == SQL_SUCCESS) {

        trim_right_spaces(location);
        trim_right_spaces(nameValuePair);

        if (strcmp(prevLocation, location)==0) {
            cust += string(nameValuePair);
            cust.append("\a",1);
        } else {
            cust.append("\0",1);
            nodeCust[string(prevLocation)] = cust;
            cust.clear();
            cust += string(nameValuePair);
            cust.append("\a",1);
        }

        strcpy(prevLocation, location);
        sqlrc = SQLFetch(hstmt);
    }

    cust.append("\0",1);
    nodeCust[string(prevLocation)] = cust;

    SQLCloseCursor(hstmt);

    return OK;
}

STATUS
getBlockInfo(
             const std::string& id,
             BlockInfo& info
)
{
    DBTBlock dbo;
    DBTDomainmap dm;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.MLOADERIMG);
    colBitmap.set(dbo.NODECONFIG);
    colBitmap.set(dbo.STATUS);
    colBitmap.set(dbo.OPTIONS);
    colBitmap.set(dbo.BOOTOPTIONS);
    colBitmap.set(dbo.NUMCNODES);
    colBitmap.set(dbo.NUMIONODES);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << "(" << id << ") block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
         LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
         return DB_ERROR;
    }

    // can we fit the info in the BlockBootInfo?
    if (strlen(dbo._mloaderimg) > sizeof(info.uloaderImg)) {
        LOG_ERROR_MSG(__FUNCTION__ << " buffer to receive microloader image is too small");
        return FAILED;
    }

    if (strlen(dbo._nodeconfig) > sizeof(info.nodeConfig)) {
        LOG_ERROR_MSG(__FUNCTION__ << " buffer to receive node config is too small");
        return FAILED;
    }

    if (strlen(dbo._bootoptions) > sizeof(info.bootOptions)) {
        LOG_ERROR_MSG(__FUNCTION__ << " buffer to receive boot options is too small");
        return FAILED;
    }

    strcpy(info.uloaderImg, dbo._mloaderimg);
    strcpy(info.nodeConfig, dbo._nodeconfig);
    strcpy(info.status,dbo._status);
    strcpy(info.options,dbo._options);
    strcpy(info.bootOptions,dbo._bootoptions);
    info.cnodes = dbo._numcnodes;
    info.ionodes = dbo._numionodes;

    // get the path to the image from the domain table
    string whereClause = string("where nodeconfig='")+string(info.nodeConfig)+string("' order by startcore  ");

    tx.close(&dbo);
    colBitmap.set();
    dm._columns = colBitmap.to_ulong();
    sqlrc = tx.query(&dm, whereClause.c_str());
    if (sqlrc != SQL_SUCCESS) {
          LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
          return DB_ERROR;
    }

    DomainInfo domainInfo;
    sqlrc = tx.fetch(&dm);
    while (sqlrc == SQL_SUCCESS) {
        strcpy(domainInfo.imageSet, dm._domainimg);
        strcpy(domainInfo.options, dm._domainoptions);
        domainInfo.startCore = dm._startcore;
        domainInfo.endCore = dm._endcore;
        domainInfo.startAddr = dm._startaddr;
        domainInfo.endAddr = dm._endaddr;
        domainInfo.custAddr = dm._custaddr;
        strncpy(domainInfo.id, dm._domainid, domainInfo.IdSize);
        info.domains.push_back(domainInfo);
        sqlrc = tx.fetch(&dm);
    }
    tx.close(&dm);
    return OK;
}

STATUS
setBootOptions(
        const std::string& id,
        const std::string& options
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    SQLRETURN sqlrc;

    if (options.size() > sizeof(dbo._bootoptions)) {
        LOG_ERROR_MSG(__FUNCTION__ << " boot options exceeds allowed size");
        return INVALID_ARG;
    }

    string whereClause;
    if (id == "*ALLIO") {
        whereClause = string("where  numcnodes = 0 ");
    } else if (id == "*ALLCOMPUTE") {
        whereClause = string("where  numionodes = 0 ");
    } else if (id != "*ALL") {
        whereClause = string("where blockid='") + id + string("' ");
    }

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    if ( options == "NULL" ) {
        string upd = string("update bgqblock set bootoptions = NULL ") + whereClause;
        sqlrc = tx.execStmt(upd.c_str());
    } else {
        colBitmap.set(dbo.BOOTOPTIONS);
        dbo._columns = colBitmap.to_ulong();
        snprintf(dbo._bootoptions, sizeof(dbo._bootoptions), "%s", options.c_str());
        sqlrc = tx.update(&dbo,whereClause.c_str());
    }

    if (sqlrc != SQL_SUCCESS) {
        if (sqlrc == SQL_NO_DATA) {
            return NOT_FOUND;
        } else if (sqlrc == SQL_SUCCESS_WITH_INFO) {
            return OK;
        } else {
            LOG_ERROR_MSG( "Database operation failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}


STATUS
addBootOption(
        const std::string& id,
        const std::string& option
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    string stmtWithWhereClause("update ");

    stmtWithWhereClause +=  dbo.getTableName();
    stmtWithWhereClause +=  " set bootoptions = bootoptions || ',' || '";
    stmtWithWhereClause +=  option;
    stmtWithWhereClause +=  "'  where length(rtrim(bootOptions))>0 ";

    if (id == "*ALLIO") {
        stmtWithWhereClause +=  "   and  numcnodes = 0 ";
    } else if (id == "*ALLCOMPUTE") {
        stmtWithWhereClause +=  "   and  numionodes = 0 ";
    } else if (id != "*ALL") {
        stmtWithWhereClause +=  "   and blockid = '";
        stmtWithWhereClause +=  id;
        stmtWithWhereClause +=  "'";
    }

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.execStmt(stmtWithWhereClause.c_str());

    if ((sqlrc != SQL_SUCCESS) && (sqlrc != SQL_NO_DATA) && (sqlrc != SQL_SUCCESS_WITH_INFO)) {
        LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    stmtWithWhereClause = "update ";
    stmtWithWhereClause +=  dbo.getTableName();
    stmtWithWhereClause +=  " set bootoptions =  '";
    stmtWithWhereClause +=  option;
    stmtWithWhereClause +=  "'  where (length(rtrim(bootOptions))=0 or bootOptions is null) ";

    if (id == "*ALLIO") {
        stmtWithWhereClause +=  "   and  numcnodes = 0 ";
    } else if (id == "*ALLCOMPUTE") {
        stmtWithWhereClause +=  "   and  numionodes = 0 ";
    } else if (id != "*ALL") {
        stmtWithWhereClause +=  "   and blockid = '";
        stmtWithWhereClause +=  id;
        stmtWithWhereClause +=  "'";
    }

    SQLRETURN sqlrc2 = tx.execStmt(stmtWithWhereClause.c_str());

    if ((sqlrc2 != SQL_SUCCESS) && (sqlrc2 != SQL_NO_DATA) && (sqlrc2 != SQL_SUCCESS_WITH_INFO)) {
        LOG_ERROR_MSG( "Database update failed with error: " << sqlrc2 << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    } else if ((sqlrc == SQL_NO_DATA) && (sqlrc2 == SQL_NO_DATA)) {
        return NOT_FOUND;
    }

    return OK;
}

STATUS
setBlockInfo(
        const std::string& id,
        const BlockInfo& info
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    if (strlen(info.uloaderImg) > sizeof(dbo._mloaderimg)) {
        LOG_ERROR_MSG(__FUNCTION__ << " microloader image name too long");
        return INVALID_ARG;
    }

    if (strlen(info.nodeConfig) > sizeof(dbo._nodeconfig)) {
        LOG_ERROR_MSG(__FUNCTION__ << " node config name too long");
        return INVALID_ARG;
    }

    if (strlen(info.bootOptions) > sizeof(dbo._bootoptions)) {
        LOG_ERROR_MSG(__FUNCTION__ << " boot options too long");
        return INVALID_ARG;
    }

    string whereClause = string("where blockid='") + id + string("' and status in ('F','T','A') ");

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.MLOADERIMG);
    colBitmap.set(dbo.NODECONFIG);
    colBitmap.set(dbo.OPTIONS);

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    sprintf(dbo._mloaderimg, "%s", info.uloaderImg);
    sprintf(dbo._nodeconfig, "%s", info.nodeConfig);
    sprintf(dbo._options, "%s", info.options);
    if ( info.bootOptions[0] != '\0' ) {
        colBitmap.set(dbo.BOOTOPTIONS);
        sprintf(dbo._bootoptions, "%s", info.bootOptions);
    }
    
    dbo._columns = colBitmap.to_ulong();

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.update(&dbo,whereClause.c_str());
    if (sqlrc != SQL_SUCCESS) {
        if (sqlrc == SQL_NO_DATA) {
            return NOT_FOUND;
        } else {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}

STATUS
getBlockOwner(
        const std::string& id,
        std::string& owner
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.OWNER);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    trim_right_spaces(dbo._owner);
    owner = dbo._owner;
    return OK;
}


STATUS
getBlockUser(
        const std::string& id,
        std::string& user,
        int& qualifier
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.USERNAME);
    colBitmap.set(dbo.QUALIFIER);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    trim_right_spaces(dbo._username);
    trim_right_spaces(dbo._qualifier);
    user = dbo._username;
    if ( dbo._qualifier[0] == '\0' ) {
        qualifier = 0;
    } else {
        try {
            qualifier = boost::lexical_cast<int>(dbo._qualifier);
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_WARN_MSG( "could not convert qualifier '" << dbo._qualifier << "' into integer");
        }
    }

    return OK;
}


STATUS
setBlockDesc(
        const std::string& id,
        const std::string& description
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    string whereClause = string("where blockid='") + id + string("' and status = 'F'");

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    if (description.size() >= sizeof(dbo._description)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block description text too long");
        return INVALID_ARG;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.DESCRIPTION);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    snprintf(dbo._description, sizeof(dbo._description), "%s", description.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.update(&dbo,whereClause.c_str());
    if (sqlrc != SQL_SUCCESS) {
        if (sqlrc == SQL_NO_DATA) {
            LOG_WARN_MSG(__FUNCTION__ << " block name not found");
            return NOT_FOUND;
        } else {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}

STATUS
getBlockSecurityKey(
        const std::string& id,
        unsigned char *key,
        size_t keyBufferSize
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.SECURITYKEY);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
         LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
         return DB_ERROR;
    }

    size_t lenToCopy = (keyBufferSize < sizeof( dbo._securitykey )) ? keyBufferSize : sizeof( dbo._securitykey );
    memcpy(key, dbo._securitykey, lenToCopy);
    return OK;
}

STATUS
setBlockSecurityKey(
        const std::string& id,
        unsigned char *key,
        size_t keyLen
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }
    if (keyLen > sizeof(dbo._securitykey)) {
        LOG_ERROR_MSG(__FUNCTION__ << " security key too long");
        return INVALID_ARG;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.SECURITYKEY);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    size_t lenToCopy = (keyLen < sizeof( dbo._securitykey )) ? keyLen : sizeof( dbo._securitykey );
    memset(dbo._securitykey, 0, sizeof(dbo._securitykey));
    memcpy(dbo._securitykey, key, lenToCopy );

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.updateByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        if (sqlrc == SQL_NO_DATA) {
            LOG_WARN_MSG(__FUNCTION__ << " block name not found");
            return NOT_FOUND;
        } else {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}

STATUS
augmentRAS(RasEvent& rasEvent)
{
    if ((rasEvent.getDetail(RasEvent::ECID).length() == 0 ) ||
        (rasEvent.getDetail(RasEvent::SERIAL_NUMBER).length() == 0 ))   {

        TxObject tx(DBConnectionPool::Instance());
        if (!tx.getConnection()) {
            LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
            return CONNECTION_ERROR;
        }
        string sqlstr;
        char sn[20], ecid[65];
        SQLHANDLE hstmt;
        SQLLEN ind1, ind2;
        string loc = rasEvent.getDetail(RasEvent::LOCATION);

        sqlstr = "select ecid, serialnumber from bgqnodeall where location = '" + loc +
            string("' union select ecid,serialnumber from bgqionodeall where location = '") + loc +
            string("' union select ecid, serialnumber from bgqlinkchipall where location = '") + loc +
            string("' union select ecid, serialnumber from bgqiolinkchipall where location = '") + loc +
            string("' union select cast(null as char(32)), serialnumber from bgqnodecardall where location = '") + loc +
            string("' union select cast(null as char(32)), serialnumber from bgqnodecarddcaall where location = '") + loc + string("' ");

        SQLRETURN sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, ecid, sizeof(ecid), &ind1);
        sqlrc = SQLBindCol(hstmt, 2, SQL_C_CHAR, sn,   sizeof(sn),   &ind2);
        sqlrc = SQLFetch(hstmt);
        SQLCloseCursor(hstmt);

        if (sqlrc == 0 && ind1 != SQL_NULL_DATA && rasEvent.getDetail(RasEvent::ECID).length() == 0 )
            rasEvent.setDetail(RasEvent::ECID, ecid);

        if (sqlrc == 0 && ind2 != SQL_NULL_DATA && rasEvent.getDetail(RasEvent::SERIAL_NUMBER).length() == 0 )
            rasEvent.setDetail(RasEvent::SERIAL_NUMBER, sn);

    }
    return OK;
}

STATUS
checkRack(
        const std::vector<std::string>& locations,
        std::vector<string>& invalid
)
{
    invalid.clear();

    string sqlstr;
    SQLLEN ind1,ind2;
    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    char loc[4];

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    StringTokenizer tokens;
    std::vector<std::string> IORack, CNRack;

    for (size_t i=0; i < locations.size(); i++) {

        tokens.tokenize(locations[i], ", ");  // split on comma or space

        for (size_t tok=0; tok < tokens.size(); ++tok) {
            if (tokens[tok].size() > 3) {
                LOG_ERROR_MSG(__FUNCTION__ << " rack location too long");
                return INVALID_ID;
            }
            if (tokens[tok].substr(0,1) == "Q") {
                IORack.push_back(tokens[tok]);
            } else {
                CNRack.push_back(tokens[tok]);
            }

        }
    }

    if (IORack.size() > 0) {
        sqlstr.clear();
        sqlstr.append("  select IO.loc  from (values  ");

        for (size_t io=0; io < IORack.size(); io++) {
            sqlstr.append(" ('");
            sqlstr.append(IORack[io]);
            sqlstr.append("' ) ");
            if ((io+1) < IORack.size())
                sqlstr.append(" , ");
        }

        sqlstr.append("  ) as IO (loc) where loc  not in (select location from bgqiorack)");
        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }

        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR,    loc , sizeof(loc), &ind1);
        sqlrc = SQLFetch(hstmt);
        while (sqlrc == 0) {
            invalid.push_back(loc);
            sqlrc = SQLFetch(hstmt);

        }

        SQLCloseCursor(hstmt);
    }

    if (CNRack.size() > 0) {
        sqlstr.clear();
        sqlstr.append("  select CN.loc  from (values  ");

        for (size_t cn=0; cn < CNRack.size() ;cn++) {
            sqlstr.append(" ('");
            sqlstr.append(CNRack[cn]);
            sqlstr.append("' ) ");
            if ((cn+1) < CNRack.size())
                sqlstr.append(" , ");

        }

        sqlstr.append("  ) as CN (loc) where loc  not in (select substr(location,1,3) from bgqmidplane)");
        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }

        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR,    loc , sizeof(loc), &ind1);
        sqlrc = SQLFetch(hstmt);
        while (sqlrc == 0) {
            invalid.push_back(loc);
            sqlrc = SQLFetch(hstmt);
        }

        SQLCloseCursor(hstmt);
    }

    return OK;
}

STATUS
getBlockErrorText(
        const std::string& id,
        std::string& text
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    colBitmap.set(dbo.ERRTEXT);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbo);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if (dbo._ind[DBTBlock::ERRTEXT] != SQL_NULL_DATA) {
        dbo._errtext[sizeof(dbo._errtext)-1] = '\0';
        text = std::string(dbo._errtext);
    }

    return OK;
}


//---------------------------------------------------------------------
// class Dimension


const std::string& Dimension::toString(
        Value dim
    )
{
    static const string strings[] = {
        "A",
        "B",
        "C",
        "D"
    };

    static const string InvalidStr( "INVALID" );


    if ( dim <= Dimension::D ) {
        return strings[dim];
    }

    return InvalidStr;
}


bool operator==( const MidplaneCoordinate& lhs, const MidplaneCoordinate& rhs )
{
    return (lhs[Dimension::A] == rhs[Dimension::A] &&
            lhs[Dimension::B] == rhs[Dimension::B] &&
            lhs[Dimension::C] == rhs[Dimension::C] &&
            lhs[Dimension::D] == rhs[Dimension::D]);
}


size_t MidplaneCoordinateHashFn::operator()( const MidplaneCoordinate& coord ) const
{
    size_t seed(0);
    boost::hash_combine( seed, coord[Dimension::A] );
    boost::hash_combine( seed, coord[Dimension::B] );
    boost::hash_combine( seed, coord[Dimension::C] );
    boost::hash_combine( seed, coord[Dimension::D] );
    return seed;
}

std::string blockActionToString( BLOCK_ACTION action )
{
    typedef std::map<BGQDB::BLOCK_ACTION, std::string> BlockActionMap;
    static const BlockActionMap blockActionStrings = boost::assign::map_list_of
        (BGQDB::NO_BLOCK_ACTION, "No action")
        (BGQDB::CONFIGURE_BLOCK, "Boot")
        (BGQDB::DEALLOCATE_BLOCK, "Free")
        (BGQDB::CONFIGURE_BLOCK_NO_CHECK, "Boot with NO_CHECK")
        ;
    return blockActionStrings.at(action);
}

std::string blockStatusToString( BLOCK_STATUS status )
{
    typedef std::map<BGQDB::BLOCK_STATUS, std::string> BlockStatusMap;
    static const BlockStatusMap blockStatusStrings = boost::assign::map_list_of
        (BGQDB::INVALID_STATE, "UNKNOWN")
        (BGQDB::FREE, "FREE")
        (BGQDB::ALLOCATED, "ALLOCATED")
        (BGQDB::INITIALIZED, "INITIALIZED")
        (BGQDB::BOOTING, "BOOTING")
        (BGQDB::TERMINATING, "TERMINATING")
        ;
    return blockStatusStrings.at(status);

}

std::string blockCodeToString(  const char* code )
{
    return blockStatusToString(blockCodeToStatus(code));
}

const char* blockStatusToCode( BLOCK_STATUS state )
{
    return (state == FREE ? BLOCK_FREE :
            state == ALLOCATED ? BLOCK_ALLOCATED :
            state == BOOTING ? BLOCK_BOOTING :
            state == INITIALIZED ? BLOCK_INITIALIZED :
            BLOCK_TERMINATING);
}

BLOCK_STATUS blockCodeToStatus( const char* code )
{
    if (strcmp(code, BLOCK_FREE) == 0)
        return FREE;
    if (strcmp(code, BLOCK_INITIALIZED) == 0)
        return INITIALIZED;
    if (strcmp(code, BLOCK_ALLOCATED) == 0)
        return ALLOCATED;
    if (strcmp(code, BLOCK_BOOTING) == 0)
        return BOOTING;
    if (strcmp(code, BLOCK_TERMINATING) == 0)
        return TERMINATING;

    return INVALID_STATE;
}


bool checkValueExists(
        const std::string& table_name,
        const std::string& column_name,
        const std::string& value,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    if ( ! conn_ptr ) {
        conn_ptr = DBConnectionPool::Instance().getConnection();
    }

    cxxdb::ParameterNames param_names;
    param_names.push_back( "value" );

    cxxdb::QueryStatementPtr stmt_ptr(conn_ptr->prepareQuery( string() + "SELECT 1 AS c FROM " + table_name + " WHERE " + column_name + " = ? FETCH FIRST 1 ROWS ONLY", param_names ));

    stmt_ptr->parameters()["value"].set( value );

    cxxdb::ResultSetPtr rs_ptr(stmt_ptr->execute());

    if ( ! rs_ptr->fetch() ) {
        return false;
    }

    return true;
}


bool checkLocationExists(
        const bgq::util::Location& location,
        cxxdb::ConnectionPtr conn_ptr
    )
{
    bgq::util::Location::Type loc_type(location.getType());

    if ( loc_type == bgq::util::Location::ComputeRack )  return checkValueExists( "bgqMidplane", "location", string(location) + "-M0", conn_ptr );
    if ( loc_type == bgq::util::Location::IoRack )  return checkValueExists( "bgqIoRack", "location", string(location), conn_ptr );
    if ( loc_type == bgq::util::Location::Midplane )   return checkValueExists( "bgqMidplane", "location", string(location), conn_ptr );
    if ( loc_type == bgq::util::Location::NodeBoard )  return checkValueExists( "bgqNodeCard", "location", string(location), conn_ptr );
    if ( loc_type == bgq::util::Location::AllNodeBoardsOnMidplane )  return checkValueExists( "bgqMidplane", "location", location.getMidplaneLocation(), conn_ptr );
    if ( loc_type == bgq::util::Location::DcaOnNodeBoard )   return checkValueExists( "bgqNodeCardDCA", "location", string(location), conn_ptr );
    if ( loc_type == bgq::util::Location::IoBoardOnComputeRack || loc_type == bgq::util::Location::IoBoardOnIoRack )   return checkValueExists( "bgqIODrawer", "location", string(location), conn_ptr );
    if ( loc_type == bgq::util::Location::PowerModuleOnComputeRack || loc_type == bgq::util::Location::PowerModuleOnIoRack )   return checkValueExists( "bgqBulkPowerSupply", "location", string(location), conn_ptr );
    if ( loc_type == bgq::util::Location::ClockCardOnComputeRack || loc_type == bgq::util::Location::ClockCardOnIoRack )   return checkValueExists( "bgqClockCard", "location", string(location), conn_ptr );

    BOOST_THROW_EXCEPTION( std::runtime_error( string() + "Unexpected hardware location for checkLocationExists, location is " + string(location) ) );

    return true;
}


} // namespace BGQDB


namespace std {

std::ostream& operator<<( std::ostream& os, const BGQDB::MidplaneCoordinate& coord )
{
    os << "(" << coord[BGQDB::Dimension::A] << "," << coord[BGQDB::Dimension::B] << "," << coord[BGQDB::Dimension::C] << "," << coord[BGQDB::Dimension::D] << ")";
    return os;
}

} // namespace std
