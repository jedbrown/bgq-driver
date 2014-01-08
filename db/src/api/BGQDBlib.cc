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
#include "utility.h"
#include "job/SubBlockRas.h"

#include "cxxdb/cxxdb.h"

#include "tableapi/gensrc/bgqtableapi.h"
#include "tableapi/dbbasic.h"
#include "tableapi/TxObject.h"
#include "tableapi/utilcli.h"

#include <bgq_util/include/string_tokenizer.h>
#include <bgq_util/include/Location.h>

#include <utility/include/CableBadWires.h>
#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>
#include <utility/include/XMLEntity.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/thread/once.hpp>

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include <execinfo.h>

using namespace std;

LOG_DECLARE_FILE( "database" );


const string machineSerialNumber = "BGQ";

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
const uint32_t SERIAL_NUM_LEN =   19;
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
        LOG_ERROR_MSG(function_name << ": block name contains invalid characters");
        return false;
    }

    static const unsigned max_block_id_length = sizeof( DBTBlock()._blockid ) - 1;

    if (blockid.size() > max_block_id_length) {
        LOG_ERROR_MSG(function_name << ": block name exceeds allowed size of " << max_block_id_length);
        return false;
    }
    return true;
}


static bool s_is_initialized(false);
static bool bypass_ctlaction(false);

void
init(
        const bgq::utility::Properties::ConstPtr properties,
        const std::string& section_name
)
{
    if ( s_is_initialized ) {
        LOG_DEBUG_MSG( "BGQDB is already initialized. Ignoring." );
        return;
    }

    DBConnectionPool::init( properties, section_name );

    try {
        if ( !properties->getValue(section_name,"control_action_bypass").empty() &&
             properties->getValue(section_name,"control_action_bypass") != "false") {
            bypass_ctlaction = true;
        }
    } catch ( std::exception& e ) {
    }

    s_is_initialized = true;
}

void
init(
        const bgq::utility::Properties::ConstPtr properties,
        unsigned connection_pool_size
    )
{
    if ( s_is_initialized ) {
        LOG_DEBUG_MSG( "BGQDB is already initialized. Ignoring." );
        return;
    }

    DBConnectionPool::init( properties, connection_pool_size );

    s_is_initialized = true;
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
getNodes(
        std::ostream& xml,
        const std::string& nodeboard
)
{
    extract_db_status result;
    if ((result = extract_db_nodes(xml, nodeboard)) != DB_OK) {
        LOG_ERROR_MSG( __FUNCTION__ << " Database error occurred getting nodes");
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
getBlocks(
        std::ostream& outBlockXML
)
{
    extract_db_status result;
    if ((result = extract_db_bpblock(outBlockXML, "*ALL")) != DB_OK) {
        // Error is logged from extract_db_bpblock
        return extractStatusToDBStatus(result);
    }

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
getBPBlockXML(
        std::ostream& xml,
        const std::string& block
)
{
    extract_db_status result;

    if ((result = extract_db_bpblock(xml, block)) != DB_OK) {
        // Error is logged from extract_db_bpblock
        return extractStatusToDBStatus(result);
    }

    return OK;
}

// getting the caller TxObject to do the database access in the same transaction
static bool
isBlockFree(
        TxObject *tx,
        const std::string& block,
        bool isIOBlock
)
{
    DBTBpblockmap bmap;
    DBVBpblockstatus bstat;
    DBTSmallblock sb;
    DBTLinkblockmap lmap;
    DBTSwitchblockmap smap;
    DBVSwitchblockstatus sstat;
    DBVLinkblockstatus lstat;
    DBTIoblockmap io;
    ColumnsBitmap colBitmap;

    SQLRETURN sqlrc;

    if (isIOBlock) {
        // Check for I/O block overlap
        colBitmap.set(io.BLOCKID);
        io._columns = colBitmap.to_ulong();

        string where = "  where (substr(location,1,6)  in (select location from bgqioblockmap a, bgqblock b ";
        where += "        where b.status<>'F' and a.blockid = b.blockid )    ";
        where += "   or   location              in (select location from bgqioblockmap a, bgqblock b ";
        where += "        where b.status<>'F' and a.blockid = b.blockid )    ";
        where += "   or   location              in (select substr(location,1,6) from bgqioblockmap a, bgqblock b ";
        where += "        where b.status<>'F' and a.blockid = b.blockid )  )   ";
        where += "   and location not in (select location from bgqiodrawer where status = 'S') ";
        where += "   and  blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&io, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return false;
        }

        // Since we are querying for IO nodes/drawers that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&io);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more I/O nodes/drawers required by I/O block "  << block << " are not available.");
            return false;
        }
    } else {
        // Check for compute block overlap
        colBitmap.set(bmap.BLOCKID);
        bmap._columns = colBitmap.to_ulong();

        string where = " where ( bpid in (select bpid from ";
        where += bstat.getTableName();
        where += " where blockstatus<>'F' and blockstatus<>'E' and blockstatus<>' ') or bpid in (select posinmachine from bgqsmallblock s, bgqblock b ";
        where += " where b.blockid = s.blockid and status <> 'F' and status <>'E'))  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&bmap, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return false;
        }

        // Since we are querying for BPs that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&bmap);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more midplanes required by compute block "  << block << " are not available.");
            return false;
        }

        colBitmap.reset();
        colBitmap.set(lmap.BLOCKID);
        lmap._columns = colBitmap.to_ulong();

        where = " where linkid in (select linkid from ";
        where += lstat.getTableName();
        where += " where blockstatus<>'F' and blockstatus<>'E')  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&lmap, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return false;
        }

        // Since we are querying for links that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&lmap);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more links required by compute block "  << block << " are not available.");
            return false;
        }

        colBitmap.reset();
        colBitmap.set(smap.BLOCKID);
        smap._columns = colBitmap.to_ulong();

        where = " colBitmap where exists  (select switchid from ";
        where += sstat.getTableName();
        where += " stat where stat.switchid = colBitmap.switchid and (stat.include <>  colBitmap.include)  ";
        where += " and blockstatus<>'F' and blockstatus<>'E' and blockstatus<>' ')  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&smap, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return false;
        }

        // Since we are querying for switches that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&smap);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more switches required by compute block "  << block << " are not available.");
            return false;
        }

        colBitmap.reset();
        colBitmap.set(sb.BLOCKID);
        sb._columns = colBitmap.to_ulong();

        where = " sb  where ( posinmachine in (select bpid from ";
        where += bstat.getTableName();
        where += " where blockstatus<>'F' and blockstatus<>'E' and blockstatus<>' ') or exists (select s.nodecardpos from bgqsmallblock s, bgqblock b ";
        where += " where b.blockid = s.blockid and status <> 'F' and status <>'E' and ( " ;
        where += " sb.nodecardpos=s.nodecardpos  ";
        where += ")  and sb.posinmachine=s.posinmachine ))  and blockid = '";
        where += block;
        where += "'";

        sqlrc = tx->query(&sb, where.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return false;
        }

        // Since we are querying for node boards that are busy, we don't want to get a hit
        sqlrc = tx->fetch(&sb);
        if (sqlrc != SQL_NO_DATA_FOUND) {
            LOG_WARN_MSG( "One or more node boards required by compute block "  << block << " are not available.");
            return false;
        }
    }
    // No overlapping hardware resources found
    return true;
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
setBlockAction(
        const std::string& block,
        const BLOCK_ACTION action,
        const std::deque<std::string>& options
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    STATUS rc;

    if (block.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name '" << block << "' too long");
        return INVALID_ID;
    }

    // Get the block table entry
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.ACTION);
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
        LOG_WARN_MSG(__FUNCTION__ << " block name '" << block << "' not found");
        return NOT_FOUND;
    }

    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // Make sure it was a valid action requested and there is no action already set
    if ((action != CONFIGURE_BLOCK && action != DEALLOCATE_BLOCK && action != CONFIGURE_BLOCK_NO_CHECK) || strcmp(dbo._action, BLOCK_NO_ACTION) != 0) {
        if (strcmp(dbo._action, BLOCK_NO_ACTION) != 0) {
            LOG_DEBUG_MSG(__FUNCTION__ << " block action is already pending");
            return DUPLICATE;
        } else {
            LOG_ERROR_MSG(__FUNCTION__ << " invalid block action request");
        }
        return FAILED;
    }

    // For allocating block we will have setBlockStatus set both the status and action fields
    if (action == CONFIGURE_BLOCK || action == CONFIGURE_BLOCK_NO_CHECK) {
        std::deque<std::string> configure_options(options);
        configure_options.push_back("action");
        // Need to further differentiate which type of action to set on the block
        if (action == CONFIGURE_BLOCK_NO_CHECK) {
            // This indicate to boot I/O block with I/O nodes in error
            configure_options.push_back("no_check");
        }
        if ((rc = setBlockStatus(block, ALLOCATED, configure_options)) != OK) {
            return rc;
        }
    } else {
        strcpy(dbo._action, BLOCK_DEALLOCATING);
        string whereClause = string("where status <> 'F' and blockid  = '") + block + string("'");
        for (std::deque<std::string>::const_iterator arg = options.begin(); arg != options.end(); ++arg) {
            if ( arg->substr(0,7) == "errmsg=" ) {
                strncpy( dbo._errtext, arg->substr(7).c_str(), sizeof(dbo._errtext) - 1 );
                dbo._errtext[sizeof(dbo._errtext) - 1] = '\0';
                colBitmap.set(dbo.ERRTEXT);
                dbo._columns = colBitmap.to_ulong();
                break;
            }
        }

        sqlrc = tx.update(&dbo, whereClause.c_str());
        if ((sqlrc != SQL_NO_DATA_FOUND) && (sqlrc != SQL_SUCCESS)) {
            LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    return OK;
}

STATUS
setBlockStatus(
        const std::string& block,
        BLOCK_STATUS targetState,
        const std::deque<std::string>& options
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    bool sharedAllocate = false;                // if true, skip isBlockFree checking during allocate
    const char* errtext = NULL;
    std::string user;
    std::string qualifier;
    bool setAction(false);
    bool setNoCheckAction(false);
    bool clearActionOnFree = true;

    if (block.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    // parse options
    if ( !options.empty() ) {
        for (std::deque<std::string>::const_iterator arg = options.begin(); arg != options.end(); ++arg) {
            StringTokenizer tokens;
            tokens.tokenize(*arg, "="); // split based on the equal sign.

            if (tokens[0] == "errmsg") {
                if (tokens.size() > 1) {
                    // optional error message for setBlockStatus(DEALLOCATING)
                    char errtext_buf[sizeof(dbo._errtext)];
                    strncpy(errtext_buf, tokens[1].c_str(), sizeof(dbo._errtext));
                    errtext_buf[sizeof(dbo._errtext)-1] = '\0';
                    errtext = errtext_buf;
                }
            } else if (tokens[0] == "shared") {
                sharedAllocate = true;
            } else if (tokens[0] == "user") {
                user = tokens[1];
            } else if (tokens[0] == "qualifier") {
                qualifier = tokens[1];
            } else if (tokens[0] == "noclear") {
                clearActionOnFree = false;
            } else if (tokens[0] == "action") {
                setAction = true;
            } else if (tokens[0] == "no_check") {
                setNoCheckAction = true;
            }
        }
    }

    // get the block table entry
    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.ERRTEXT);        // to see if errtext is already set
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
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // ok, we got valid block info from the DB
    colBitmap.reset(dbo.ERRTEXT);
    colBitmap.reset(dbo.NUMIONODES);
    dbo._columns = colBitmap.to_ulong();

    BLOCK_STATUS currentState = blockCodeToStatus( dbo._status );

    if ( currentState == INVALID_STATE ) {
        LOG_ERROR_MSG(__FUNCTION__ << " block is in unknown state");
        return FAILED;
    }

    bool ioBlock = (dbo._numionodes == 0 ? false : true);

    const char *targetStateString = NULL;

    switch(targetState) {
    case ALLOCATED:
        if ((currentState == FREE && (sharedAllocate || isBlockFree(&tx, block, ioBlock)))
                || (currentState == TERMINATING && setAction == false))
        {
            // Set status to (A)llocated
            targetStateString = BLOCK_ALLOCATED;
        } else {
            if (currentState == FREE) {
                LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode(targetState) << " because required block hardware resources are unavailable");
            } else {
                LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode( targetState ) << " because the block is " << blockStatusToCode( currentState ) );
            }
            return FAILED;
        }
        break;
    case BOOTING:
        if (
                currentState == INITIALIZED ||  // for the reboot case
                currentState == ALLOCATED
           )
        {
            targetStateString = BLOCK_BOOTING;
        } else {
            LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode( targetState ) << " because the block is " << blockStatusToCode( currentState ) );
            return FAILED;
        }
        break;
    case INITIALIZED:
        if (currentState == BOOTING) {
            targetStateString = BLOCK_INITIALIZED;
        } else {
            LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode( targetState ) << " because the block is " << blockStatusToCode( currentState ) );
            return FAILED;
        }
        break;
    case FREE:                // allow free from all states for mmcs initialization
        if (currentState == ALLOCATED   ||
                currentState == INITIALIZED ||
                currentState == BOOTING ||
                currentState == TERMINATING)
        {
            targetStateString = BLOCK_FREE;

            // save any error text in the block record
            // error text is only saved if it is not already set
            if (errtext) {
                if (dbo._ind[DBTBlock::ERRTEXT] == SQL_NULL_DATA || dbo._errtext[0] == '\0') {
                    colBitmap.set(dbo.ERRTEXT);
                    dbo._columns = colBitmap.to_ulong();
                    strncpy(dbo._errtext, errtext, sizeof(dbo._errtext) - 1);
                    dbo._errtext[sizeof(dbo._errtext) - 1] = '\0';
                }
            }
        } else {
            LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode( targetState ) << " because the block is " << blockStatusToCode( currentState ) );
            return FAILED;
        }
        break;
    case TERMINATING:
        if (currentState == ALLOCATED   ||
                currentState == INITIALIZED ||
                currentState == BOOTING)
        {
            targetStateString = BLOCK_TERMINATING;

            // If any jobs are running on the block then cannot set the state to TERMINATING.
            string whereClause = string() +
                    " where blockid='" + dbo._blockid + "'";

            int nrows = tx.count("BGQJob",whereClause.c_str());

            if (nrows >=1) {
                LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode( targetState ) << " because the block has " << nrows << " jobs" );
                return FAILED;
            }
        } else {
            LOG_WARN_MSG( "Cannot set block to " << blockStatusToCode( targetState ) << " because the block is " << blockStatusToCode( currentState ) );
            return FAILED;
        }
        break;
    default:
        LOG_ERROR_MSG(__FUNCTION__ << " block is in unknown state");
        return FAILED;
    }

    if  (targetStateString == NULL) {
        LOG_ERROR_MSG(__FUNCTION__ << " block is in unknown state");
        return FAILED;
    }

    sprintf(dbo._status, "%s", targetStateString);

    if (targetState == FREE) {
        colBitmap.set(dbo.BLOCKID);
        colBitmap.set(dbo.STATUS);
        colBitmap.set(dbo.ERRTEXT);
        colBitmap.set(dbo.OPTIONS);
        colBitmap.set(dbo.USERNAME);
        dbo._columns = colBitmap.to_ulong();

        // clear the options and user name if going to FREE state
        strcpy(dbo._options,"");
        strcpy(dbo._username,"");
        //        strcpy(dbo._owner,"");    WE NO LONGER CLEAR THE OWNER ON FREE, WE CLEAR THE USERNAME
    }

    if (targetState == ALLOCATED && currentState == FREE ) {
        // Update action field only if allocate request and it came from setBlockAction
        if (setAction) {
            colBitmap.set(dbo.ACTION);
            if (setNoCheckAction) {
                // Set "N" action to indicate boot of I/O block with I/O nodes in error
                strcpy(dbo._action, BLOCK_BOOTING_NO_CHECK);
            } else {
                // Set "B" action to indicate boot action
                strcpy(dbo._action, BLOCK_BOOTING);
            }
        }

        // update username if current state is free
        colBitmap.set(dbo.USERNAME);

        if (user.empty()) {
            // get current uid, if no user name is provided (although it should always be provided)
            try {
                bgq::utility::UserId uid;
                user = uid.getUser();
                LOG_WARN_MSG( "missing user parameter, using current uid: " << user );
            } catch ( const std::runtime_error& e ) {
                LOG_ERROR_MSG( "could not get current uid: " << e.what() );
                return FAILED;
            }
        }
        // subtract 1 from size of column for null terminator
        if ( user.size() >= sizeof(dbo._username) - 1 ) {
            LOG_ERROR_MSG( "uid too large: " << user );
            return FAILED;
        }
        (void)strncpy(dbo._username, user.c_str(), sizeof(dbo._username) );

        dbo._columns = colBitmap.to_ulong();
    }

    // clear the errtext if going from FREE state to anything else
    if (currentState == FREE) {
        colBitmap.set(dbo.ERRTEXT);
        strcpy(dbo._errtext, "");
        dbo._columns = colBitmap.to_ulong();
    }

    // clear the action if the target is FREE and clearActionOnFree specified (which is the default)
    if (targetState == FREE && clearActionOnFree) {
        colBitmap.set(dbo.ACTION);
        strcpy(dbo._action, BLOCK_NO_ACTION);
        dbo._columns = colBitmap.to_ulong();
    }

    if (targetState == BOOTING) {
        // update the qualifier if one was provided
        if (!qualifier.empty() && qualifier.size() < sizeof(dbo._qualifier) )  {
            colBitmap.set(dbo.QUALIFIER);
            (void)strncpy(dbo._qualifier, qualifier.c_str(), sizeof(dbo._qualifier) );
        }

        dbo._columns = colBitmap.to_ulong();
    }

    if (targetState == FREE) {  // when freeing a block, we must clear the Software (F)ail indicator in all nodes in the block
        string sqlstr;

        // This code must handle the node being either an IO node or a compute node
        int upd_rc;
        if (ioBlock) {
            sqlstr = "update bgqionode set status = 'A' where status = 'F' and " +
                string(" ( location in (select location from bgqioblockmap where  blockid = '") +  block +  string("' ) ") +
                string(" or substr(location,1,6) in (select location from bgqioblockmap where  blockid = '") +  block +  string("' )) ");
            upd_rc = tx.execStmt(sqlstr.c_str());
        } else {
            // this query handles sub-block jobs
            sqlstr  = "UPDATE BGQNode set status='A' WHERE status='F' and ";
            sqlstr += "(";
            sqlstr += "substr(location,1,10) IN (SELECT posinmachine || '-' || nodecardpos FROM BGQSmallBlock where blockid = '";
            sqlstr += block;
            sqlstr += "') OR ";
            sqlstr += "substr(location,1,6) in (select bpid from BGQBpBlockMap where blockid = '";
            sqlstr += "'))";
            upd_rc = tx.execStmt(sqlstr.c_str());

            // this query handles large blocks
            sqlstr  = "UPDATE BGQMidplane set status='A' WHERE status='F' and ";
            sqlstr += "location in (select bpid from BGQBpBlockMap where blockid = '";
            sqlstr += block;
            sqlstr += "')";
            upd_rc = tx.execStmt(sqlstr.c_str());
        }

        if ((upd_rc != SQL_SUCCESS) && (upd_rc != SQL_NO_DATA_FOUND))
            LOG_ERROR_MSG( "Database update failed with error: " << upd_rc << " at " << __FUNCTION__ << ':' << __LINE__ );

    }

    sqlrc = tx.updateByKey(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database update failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    return OK;
}

STATUS
getBlockAction(
        std::string& id,
        BLOCK_ACTION& action,
        uint32_t& creationId,
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

    creationId = dbo._creationid;

    return OK;
}


STATUS
clearBlockAction(
                 const std::string& id,
                 uint32_t creationId
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

        if (creationId != 0) {
            string whereClause = string("where creationid = ") + boost::lexical_cast<std::string>(creationId);
            sqlrc = tx.update(&dbo, whereClause.c_str());
        } else {
            sqlrc = tx.updateByKey(&dbo);
        }
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
        string("  substr(source,1,10) = posinmachine || '-' || nodecardpos  and c.blockid = d.blockid and d.status in ( 'B','I','T') and b.status = 'A' ") +
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
        string("') as a left outer join (select location from bgqioblockmap b, bgqblock c where b.blockid = c.blockid and status = 'I' ) as b  on  a.ion = b.location or substr(a.ion,1,6) = b.location ") +
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
            midplaneMap.insert(pair<std::string, MidplaneIOInfo>(midplane, midplaneIOInfo()));
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
        LOG_WARN_MSG(__FUNCTION__ << " block name not found");
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
    colBitmap.set(dbo.BOOTOPTIONS);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", id.c_str());
    sprintf(dbo._mloaderimg, "%s", info.uloaderImg);
    sprintf(dbo._nodeconfig, "%s", info.nodeConfig);
    sprintf(dbo._options, "%s", info.options);
    sprintf(dbo._bootoptions, "%s", info.bootOptions);

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
getBringupOptions(
        std::string& buOptions
)
{
    DBTMachine dbm;
    ColumnsBitmap colBitmap;

    colBitmap.set(dbm.BRINGUPOPTIONS);
    colBitmap.set(dbm.SERIALNUMBER);
    dbm._columns = colBitmap.to_ulong();

    sprintf( dbm._serialnumber, "%s", machineSerialNumber.c_str() );
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    SQLRETURN sqlrc = tx.queryByKey(&dbm);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.fetch(&dbm);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to find bringup options in database");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    trim_right_spaces(dbm._bringupoptions);
    buOptions = dbm._bringupoptions;
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
setBlockOptions(
        const std::string& id,
        const std::string& options
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;

    if (id.size() >= sizeof(dbo._blockid)) {
        LOG_ERROR_MSG(__FUNCTION__ << " block name too long");
        return INVALID_ID;
    }

    if (options.size() >= sizeof(dbo._options)) {
        LOG_ERROR_MSG(__FUNCTION__ << " options too long");
        return INVALID_ARG;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.STATUS);
    colBitmap.set(dbo.OPTIONS);
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
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // ok, we got valid block info from the DB
    if (strcmp(dbo._status, BLOCK_FREE) != 0){
        // only allowed if block is FREE
        return FAILED;
    }

    snprintf(dbo._options, sizeof(dbo._options), "%s", options.c_str());

    sqlrc = tx.updateByKey(&dbo);
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
completeIOService(
                  const std::string& location,
                  std::string&  containingBlock
                 )
{
    string sqlstr, block;
    SQLRETURN sqlrc;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    // must be an IO drawer location of length 6 (i.e.  R00-ID ,  Q60-I0 )
    if (location.length() != 6) {
        return INVALID_ID;
    }

    // If there is a booted block that encompasses this drawer, return the name of the block so that
    // MMCS can reboot the nodes
    block.clear();
    block.append(" ( select a.blockid from bgqblock a, bgqioblockmap b where numionodes > 8 and ");
    block.append("     a.blockid = b.blockid and substr(location,1,6)  = '");
    block.append(location.substr(0,6));
    block.append("'  )");

    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    colBitmap.set(dbo.BLOCKID);
    dbo._columns = colBitmap.to_ulong();

    sqlstr.clear();
    sqlstr.append("where status in ('I', 'A', 'B', 'C') and numionodes > 0 and  blockid in ");
    sqlstr.append(block);
    sqlrc = tx.query(&dbo, sqlstr.c_str());
    if (sqlrc == SQL_SUCCESS) {
        sqlrc = tx.fetch(&dbo);

        if (sqlrc == SQL_SUCCESS) {
            trim_right_spaces((char *)dbo._blockid);
            containingBlock = string(dbo._blockid);
        } else {

            tx.close(&dbo);
            // mark the Nodes in the drawer in A  , if there is no block that needs to be rebooted

            // Note: there are other locations associated with the service action, such as the IO drawer itself, Icons, etc. and
            // the status of that hardware will be handled by the service action code.

            sqlstr = "update bgqionode set status = 'A' where iopos = '" + location + string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                return DB_ERROR;
            }

        }
    }

    return OK;
}


STATUS
killMidplaneJobs(
                 const std::string& location,
                 std::vector<job::Id>* jobs_list_out,
                 bool listOnly
                 )
{
    string sqlstr, block;
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    bool ioDrawer = false;

    // Build the query for the blocks
    if ((location.length() == 6) && (location.substr(4,1) == "I")) {
        // this is an IO drawer i.e. Q00-I2 or R00-IC
        ioDrawer = true;

        block.clear();
        block.append(" ( select blockid from bgqsmallblock where posinmachine || '-' || nodecardpos in ");
        block.append("    (select substr(source,1,10) from bgqcniolink where substr(destination,1,6) = '");
        block.append(location.substr(0,6));
        block.append("' )  union   select blockid from bgqbpblockmap where bpid in ");
        block.append("    (select substr(source,1,6) from bgqcniolink where substr(destination,1,6) = '");
        block.append(location.substr(0,6));
        block.append("' )  union   select a.blockid from bgqblock a, bgqioblockmap b where numionodes <= 8 and ");
        block.append("     a.blockid = b.blockid and substr(location,1,6)  = '");
        block.append(location.substr(0,6));
        block.append("'  )");
    } else if (location.length() == 10  && location.substr(4,1) == "I")  {
        //  this is an IO drawer port  i.e.  R00-IC-T19
        block.clear();
        block.append(" ( select blockid from bgqsmallblock a, bgqcniolink b  where destination = '");
        block.append(location.substr(0,10));
        block.append("' and   nodecardpos = substr(source,8,3) and posinmachine = substr(source,1,6) ");
        block.append("  union   select blockid from bgqbpblockmap a, bgqcniolink b  where destination  = '");
        block.append(location.substr(0,10));
        block.append("' and   bpid  = substr(source,1,6) ) ");
    } else if (location.length() == 6)  {
        //  this is a midplane i.e.  R00-M0  include passthrough
        block.clear();
        block.append(" ( select blockid from bgqsmallblock where posinmachine = '");
        block.append(location.substr(0,6));
        block.append("'  union   select blockid from bgqbpblockmap where bpid = '");
        block.append(location.substr(0,6));
        block.append("'  union   select blockid from bgqswitchblockmap where substr(switchid,3,6) = '");
        block.append(location.substr(0,6));
        block.append("') ");
    } else if (location.length() == 3 && location[0] == 'R')  {
        //  this is a compute rack  i.e.  R00  (in BG/Q this does not affect the clock card)
        ioDrawer = true;
        block.clear();
        block.append(" ( select blockid from bgqsmallblock where substr(posinmachine,1,3) = '");
        block.append(location.substr(0,3));
        block.append("'  union   select blockid from bgqbpblockmap where substr(bpid,1,3) = '");
        block.append(location.substr(0,3));
        block.append("'  union   select blockid from bgqswitchblockmap where substr(switchid,3,3) = '");
        block.append(location.substr(0,3));
        block.append("'  union   select blockid from bgqioblockmap where substr(location,1,3) = '");
        block.append(location.substr(0,3));
        block.append("') ");
    } else if (location.length() == 3 && location[0] == 'Q') {
        //  this is an I/O rack  i.e.  Q04
        ioDrawer = true;
        block.clear();
        block.append(" ( select blockid from bgqioblockmap where location like '" + location + "%' ");
        block.append(" union " );
        block.append(" select distinct cnblock from bgqcnioblockmap where ioblock in (");
        block.append(" select blockid from bgqioblockmap where location like '" + location + "%') AND ");
        block.append(" cnblockstatus = 'I'" );
        block.append(") ");
    } else if (location.length() == 5)  {
        ioDrawer = true;
        //  this is a clock card R00-K
        block.clear();
        block.append(" ( select blockid from bgqsmallblock where substr(posinmachine,1,3) = '");
        block.append(location.substr(0,3));
        block.append("'  union   select blockid from bgqbpblockmap where substr(bpid,1,3) = '");
        block.append(location.substr(0,3));
        block.append("'  union   select blockid from bgqswitchblockmap where substr(switchid,3,3) = '");
        block.append(location.substr(0,3));
        block.append("'  union   select blockid from bgqioblockmap where substr(location,1,3) = '");
        block.append(location.substr(0,3));

        // connected to primary clock
        block.append("'  union    select blockid from bgqsmallblock where posinmachine in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("') union   select blockid from bgqbpblockmap where bpid  in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("') union   select blockid from bgqioblockmap where substr(location,1,6) in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("') ");

        // connected to secondary clock
        block.append("  union    select blockid from bgqsmallblock where posinmachine in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("')) union   select blockid from bgqbpblockmap where bpid in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("')) union   select blockid from bgqioblockmap where substr(location,1,6) in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("')) ");

        // connected to tertiary clock
        block.append("  union    select blockid from bgqsmallblock where posinmachine in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation= '");
        block.append(location);
        block.append("'))) union   select blockid from bgqbpblockmap where bpid in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("'))) union   select blockid from bgqioblockmap where substr(location,1,6) in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("'))) ");

        // connected to fourth rank clock
        block.append("  union    select blockid from bgqsmallblock where posinmachine in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation= '");
        block.append(location);
        block.append("')))) union   select blockid from bgqbpblockmap where bpid in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("')))) union   select blockid from bgqioblockmap where substr(location,1,6) in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation in (select tolocation from bgqclockcable where fromlocation = '");
        block.append(location);
        block.append("')))) ");

        block.append(")");
    } else if (location.length() == 8) {
        //  this is all node cards  i.e.  R00-M0-N
        block.clear();
        block.append(" ( select blockid from bgqsmallblock where posinmachine = '");
        block.append(location.substr(0,6));
        block.append("'  union   select blockid from bgqbpblockmap where bpid = '");
        block.append(location.substr(0,6));
        block.append("') ");
    } else if (location.length() == 10) {
        // this is a specific node card i.e.  R01-M1-N03
        block.clear();
        block.append(" ( select blockid from bgqsmallblock where posinmachine = '");
        block.append(location.substr(0,6));
        block.append("' and   nodecardpos = '");
        block.append(location.substr(7,3));
        block.append("'   ");
        block.append("  union   select blockid from bgqbpblockmap where bpid = '");
        block.append(location.substr(0,6));
        block.append("'  union   select blockid from bgqswitchblockmap where substr(switchid,3,6) = '");
        block.append(location.substr(0,6));
        block.append("') ");
    } else {
        return INVALID_ID;
    }

    DBTJob dbo;
    SQLRETURN sqlrc;

    // Handle running jobs
    sqlstr.clear();
    sqlstr.append("where blockid in ");
    sqlstr.append(block);

    // Add running jobs to the output vector.
    ColumnsBitmap colBitmap;
    colBitmap.set(dbo.ID);
    dbo._columns = colBitmap.to_ulong();


    sqlrc = tx.query(&dbo, sqlstr.c_str());

    while (sqlrc == SQL_SUCCESS) {
        sqlrc = tx.fetch(&dbo);
        if (sqlrc == SQL_SUCCESS) {
            uint64_t jobid = dbo._id;
            if ( jobs_list_out ) jobs_list_out->push_back(jobid);

        }
    }

    if (listOnly) {   // just return a list of jobs, don't actually free any blocks
        // Skip the rest
        if (sqlrc == SQL_NO_DATA_FOUND) {
            return OK;
        } else {
            return DB_ERROR;
        }
    }

    // wait for jobs to end
    int jobcount = 0;            // count of running jobs
    const int sleepTime = 5;    // time to sleep between iterations
    int timeout = (10 * 60) / sleepTime; // number of iterations before timing out

    do {
        jobcount = tx.count("BGQJob", sqlstr.c_str());
        if (jobcount > 0) {
            sleep(sleepTime);
        }
    } while (jobcount > 0 && --timeout > 0);

    if  (jobcount > 0) {
        return FAILED;
    }

    // deallocate the blocks that use pieces of midplanes in the midplane list
    sqlstr.clear();
    sqlstr.append("where status in ('I', 'A', 'B') and numionodes = 0 and  blockid in ");
    sqlstr.append(block);
    DBTBlock dbb;
    ColumnsBitmap colBitmapb;
    colBitmapb.set(dbb.ACTION);
    colBitmapb.set(dbb.ERRTEXT);
    dbb._columns = colBitmapb.to_ulong();

    sprintf(dbb._action,"%s", BLOCK_DEALLOCATING);
    strncpy(dbb._errtext, "PrepareForService", sizeof(dbb._errtext) - 1);
    dbb._errtext[sizeof(dbb._errtext) - 1] = '\0';
    sqlrc = tx.update(&dbb,sqlstr.c_str());
    if (sqlrc != SQL_SUCCESS && sqlrc != SQL_NO_DATA_FOUND) {
        return DB_ERROR;
    }

    // wait for blocks to reach 'Free' state
    int count = 0;            // count of active blocks

    sqlstr.clear();
    sqlstr.append("where status <> 'F' and numionodes = 0 and blockid in  ");
    sqlstr.append(block);

    do {
        count = tx.count("BGQBlock", sqlstr.c_str());
        if (count > 0) {
            sleep(sleepTime);
        }
    } while (count > 0 && --timeout > 0);

    if ( ( count = tx.count("BGQBlock", sqlstr.c_str()) ) > 0) {
        return FAILED;
    }

    // do the IO blocks after the CN blocks so that our db polling code will free them in order
    if (ioDrawer) {
        sqlstr.clear();
        sqlstr.append("where status in ('I', 'A', 'B') and numcnodes = 0 and  blockid in ");
        sqlstr.append(block);
        sqlrc = tx.update(&dbb,sqlstr.c_str());
        if (sqlrc != SQL_SUCCESS && sqlrc != SQL_NO_DATA_FOUND) {
            return DB_ERROR;
        }

        // wait for blocks to reach 'Free' state
        count = 0;            // count of active blocks

        sqlstr.clear();
        sqlstr.append("where status <> 'F' and numcnodes = 0  and blockid in  ");
        sqlstr.append(block);

        do {
            count = tx.count("BGQBlock", sqlstr.c_str());
            if (count > 0) {
                sleep(sleepTime);
            }
        } while (count > 0 && --timeout > 0);

        if ( ( count = tx.count("BGQBlock", sqlstr.c_str()) ) > 0) {
            return FAILED;
        }
    }

    return OK;
}


#define setfield(fieldnm, fieldvalue)  if (diags) diage.fieldnm = fieldvalue; else dbe.fieldnm = fieldvalue;
#define fieldcpy(fieldnm, fieldvalue)  if (diags) strcpy(diage.fieldnm,fieldvalue); else strcpy(dbe.fieldnm,fieldvalue);

STATUS
putRAS(
        const std::string& block,
        const std::map<std::string, std::string>& rasmap,
        timeval rastime,
        job::Id jobid,
        bool diags
)
{

    //  This putRAS path is the slow-path for RAS events and is only used for running diags.
    //  Therefore, this path will not do post-processing of RAS, will not do a lookup of the jobid, and will
    //  not honor the END_JOB control action

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }
    DBTEventlog dbe;
    DBTDiagseventlog diage;
    ColumnsBitmap colBitmap;

    SQLRETURN sqlrc = SQL_SUCCESS;
    SQLRETURN ctl_rc = SQL_SUCCESS;
    string sqlstr;

    char ctlAction[33];
    char time_str[32];

    colBitmap.reset();
    setfield(_rawdata[0] , 0);
    ctlAction[0] = 0;

    // jobid was provided, no need to look it up
    if (jobid > 0) {
        setfield(_jobid, jobid);
        colBitmap.set(dbe.JOBID);
    }

    // Provide the timestamp so that the RAS entry in the database reflects the time that
    // the event occurred, not when it was inserted
    colBitmap.set(dbe.EVENT_TIME);
    time_t t =  rastime.tv_sec;
    struct tm lcl;
    strftime(time_str, sizeof(time_str), "%Y-%m-%d-%H.%M.%S.", localtime_r(&t, &lcl));
    if (diags) {
        snprintf(diage._event_time,sizeof(diage._event_time),"%s%06lu", time_str, rastime.tv_usec);
        diage._ind[diage.EVENT_TIME]=SQL_NTS;
    } else {
        snprintf(dbe._event_time,sizeof(dbe._event_time),"%s%06lu", time_str, rastime.tv_usec);
        dbe._ind[dbe.EVENT_TIME]=SQL_NTS;
    }

    // Go thru the RAS event detail
    for (std::map<std::string,std::string>::const_iterator pos = rasmap.begin(); pos != rasmap.end(); ++pos)    {
        if (pos->first == RasEvent::MSG_ID)  {
            colBitmap.set(dbe.MSG_ID);
            fieldcpy(_msg_id,pos->second.c_str());
        } else if (pos->first == RasEvent::CATEGORY)  {
            colBitmap.set(dbe.CATEGORY);
            fieldcpy(_category,pos->second.c_str());
        } else if (pos->first == RasEvent::COMPONENT)  {
            colBitmap.set(dbe.COMPONENT);
            fieldcpy(_component,pos->second.c_str());
        } else if (pos->first == RasEvent::SEVERITY)  {
            colBitmap.set(dbe.SEVERITY);
            fieldcpy(_severity,pos->second.c_str());
        } else if (pos->first == RasEvent::MESSAGE)  {
            colBitmap.set(dbe.MESSAGE);
            fieldcpy(_message,pos->second.c_str());
        } else if (pos->first == RasEvent::LOCATION)  {
            colBitmap.set(dbe.LOCATION);
            fieldcpy(_location,pos->second.c_str());
        } else if (pos->first == RasEvent::ECID)  {
            colBitmap.set(dbe.ECID);
            if (diags) {
                hexchar2bitdata((unsigned char*)diage._ecid,2*sizeof(diage._ecid),pos->second.c_str());
            } else {
                hexchar2bitdata((unsigned char*)dbe._ecid,2*sizeof(dbe._ecid),pos->second.c_str());
            }
        } else if (pos->first == RasEvent::CPU)  {
            colBitmap.set(dbe.CPU);
            setfield(_cpu, atoi(pos->second.c_str()));
        } else if (pos->first == RasEvent::COUNT)  {
            colBitmap.set(dbe.COUNT);
            setfield(_count, atoi(pos->second.c_str()));
        } else if (pos->first == RasEvent::SERIAL_NUMBER)  {
            colBitmap.set(dbe.SERIALNUMBER);
            fieldcpy(_serialnumber,pos->second.c_str());
        } else if (pos->first == RasEvent::CONTROL_ACTION) {
            colBitmap.set(dbe.CTLACTION);
            fieldcpy(_ctlaction,pos->second.c_str());
            strcpy(ctlAction,pos->second.c_str());
        }


        else {

            if (pos->second != "") {  // don't write empty values into RAWDATA
                colBitmap.set(dbe.RAWDATA);
                if (diags) {
                    strcat(diage._rawdata,pos->first.c_str());
                    strcat(diage._rawdata,"=");
                    strcat(diage._rawdata,pos->second.c_str());
                    strcat(diage._rawdata,"; ");
                } else {
                    strcat(dbe._rawdata,pos->first.c_str());
                    strcat(dbe._rawdata,"=");
                    strcat(dbe._rawdata,pos->second.c_str());
                    strcat(dbe._rawdata,"; ");
                }
            }
        }
    }

    // if we have a block, set it in the event
    if ( !block.empty() ) {
        colBitmap.set(dbe.BLOCK);
        fieldcpy(_block,block.c_str());
    }

    setfield(_columns, colBitmap.to_ulong());
    if (diags) {
        sqlrc = tx.insert(&diage);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        }
     } else {
        sqlrc = tx.insert(&dbe);
        if (sqlrc != SQL_SUCCESS) {
             LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        }
    }

    // we will not handle control actions for diags
    if ((strlen(ctlAction) > 0) && !diags &&  colBitmap[dbe.LOCATION]) {

        StringTokenizer tokens;
        tokens.tokenize(string(ctlAction), ", ");  // split on comma or space

        for (uint32_t tok=0; tok< tokens.size(); ++tok) {
            LOG_INFO_MSG(__FUNCTION__ << " Location " << dbe._location << " had a RAS event with control action " << tokens[tok] << " which is not honored during diagnostics");
        }
    }

    if ((sqlrc == SQL_SUCCESS) && (ctl_rc == SQL_SUCCESS)) {
        return OK;
    } else {
        return DB_ERROR;
    }
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
putRAS(
        const RasEvent& rasEvent,
        const std::string& block,
        const job::Id job,
        const uint32_t qualifier,
        std::vector<job::Id>* jobs_list_out,
        uint32_t* recid_out
)
{
    SQLRETURN sqlrc = SQL_SUCCESS;
    SQLHANDLE hstmt;
    SQLLEN ind1, ind2;
    string sqlstr;
    string rasMessage("select recid from NEW TABLE (insert into tbgqeventlog (msg_id,category,component,severity,message,location,ctlaction,block");
    if ( job != 0 ) rasMessage.append(",jobid");
    if ( qualifier != 0 ) rasMessage.append(",qualifier");
    rasMessage.append(") values(");

    rasMessage.append("'").append(rasEvent.getDetail(RasEvent::MSG_ID)).append("'");
    rasMessage.append(",'").append(rasEvent.getDetail(RasEvent::CATEGORY)).append("'");
    rasMessage.append(",'").append(rasEvent.getDetail(RasEvent::COMPONENT)).append("'");
    rasMessage.append(",'").append(rasEvent.getDetail(RasEvent::SEVERITY)).append("'");
    rasMessage.append(",'").append(rasEvent.getDetail(RasEvent::MESSAGE)).append("'");
    if (rasEvent.getDetail(RasEvent::LOCATION).empty()) {
        rasMessage.append(",NULL");
    } else {
        rasMessage.append(",'").append(rasEvent.getDetail(RasEvent::LOCATION)).append("'");
    }

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).length() == 0) {
        rasMessage.append(",NULL");
    } else {
        rasMessage.append(",'").append(rasEvent.getDetail(RasEvent::CONTROL_ACTION)).append("'");
    }

    if ( !block.empty() ) {
        rasMessage.append(",'").append(block).append("'");
    } else {
        rasMessage.append(",NULL");
    }
    if ( job != 0 ) {
        rasMessage.append( "," + boost::lexical_cast<std::string>(job) );
    }
    if ( qualifier != 0 ) {
        rasMessage.append( "," + boost::lexical_cast<std::string>(qualifier) );
    }
    rasMessage.append("))");
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    // For health check warnings, make sure the card is not being serviced
    bool serviceAction = false;
    if ((rasEvent.getDetail(RasEvent::SEVERITY)=="WARN") &&
        (rasEvent.getDetail(RasEvent::COMPONENT)=="MMCS") &&
        ((rasEvent.getDetail(RasEvent::CATEGORY)=="Node_Board") ||
         (rasEvent.getDetail(RasEvent::CATEGORY)=="IO_Board") ||
         (rasEvent.getDetail(RasEvent::CATEGORY)=="Service_Card")) &&
        (rasEvent.getDetail(RasEvent::MSG_ID).substr(0,5) =="00061")) {

        string whereClause = " where location='" + rasEvent.getDetail(RasEvent::LOCATION)+ "' and status not in ('F','C')";
        int nrows = tx.count("BGQServiceAction",whereClause.c_str());
        if (nrows >=1)
            serviceAction = true;
    }

    if (serviceAction) {
        LOG_INFO_MSG( "RAS event for location " << rasEvent.getDetail(RasEvent::LOCATION) << " ignored due to pending service action.");
        return OK;
    }

    sqlrc = tx.execQuery(rasMessage.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ( recid_out ) {
        sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, recid_out, 4, &ind1);
        sqlrc = SQLFetch(hstmt);
    }

    const bgq::util::Location location( rasEvent.getDetail(RasEvent::LOCATION), bgq::util::Location::nothrow );

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).find("BOARD_IN_ERROR") != string::npos) {
        if (rasEvent.getDetail(RasEvent::LOCATION).substr(4,1) != "I") {
            sqlstr = "update bgqnodecard set status = 'E' where location = '" + rasEvent.getDetail(RasEvent::LOCATION).substr(0,10) + string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG(__FUNCTION__ << " RAS event control action processing for BOARD_IN_ERROR failed to update hardware status, rc = " << sqlrc);
            }
        } else {
            sqlstr = "update bgqiodrawer set status = 'E' where location = '" + rasEvent.getDetail(RasEvent::LOCATION).substr(0,6) + string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG(__FUNCTION__ << " RAS event control action processing for BOARD_IN_ERROR failed to update I/O drawer status, rc = " << sqlrc);
            }
            sqlstr = "update bgqionode set status = 'E' where substr(location,1,6) = '" + rasEvent.getDetail(RasEvent::LOCATION).substr(0,6) + string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG(__FUNCTION__ << " RAS event control action processing for BOARD_IN_ERROR failed to update I/O node status, rc = " << sqlrc);
            }
        }
    }

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).find("END_JOB") != string::npos) {
        if ( jobs_list_out && location.getType() == bgq::util::Location::NodeBoard) {
            sqlstr = std::string() +
                "SELECT id from bgqjob where blockid in ("
                "SELECT blockid from bgqsmallblock where posinmachine || '-' || nodecardpos ='" + location.getLocation() + "' "
                "UNION "
                "SELECT blockid from bgqbpblockmap where bpid = '" + location.getMidplaneLocation() + "' "
                "UNION "
                "SELECT blockid from bgqswitchblockmap where substr(switchid,3,6) = '" + location.getMidplaneLocation() + "' "
                ")"
                ;
        } else if (
                jobs_list_out &&
                ( location.getType() == bgq::util::Location::IoBoardOnIoRack ||
                  location.getType() == bgq::util::Location::IoBoardOnComputeRack )
                )
        {
            sqlstr = "select unique b.id from bgqcnioblockmap a, bgqjob b where cnblock = b.blockid and substr(ion,1,6) = '" +
                rasEvent.getDetail(RasEvent::LOCATION) + string("' ");
        } else if ( jobs_list_out ) {
            LOG_ERROR_MSG( rasEvent.getDetail(RasEvent::LOCATION) << " is not a valid node board or I/O board location" );
            return INVALID_ARG;
        }

        BGQDB::job::Id id = 0;
        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        sqlrc = SQLBindCol(hstmt, 1, SQL_C_SBIGINT, &id, 8,           &ind1);
        sqlrc = SQLFetch(hstmt);

        for (;sqlrc == SQL_SUCCESS;) {
            jobs_list_out->push_back(id);
            sqlrc = SQLFetch(hstmt);
        }

        SQLCloseCursor(hstmt);
    }

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).find("FREE_COMPUTE_BLOCK") != string::npos) {
        if ( location.getType() == bgq::util::Location::NodeBoard) {
            sqlstr = std::string() +
                "SELECT blockid from bgqblock where status <> '" + SOFTWARE_FAILURE + "' and blockid in ("
                "SELECT blockid from bgqsmallblock where posinmachine || '-' || nodecardpos ='" + location.getLocation() + "' "
                "UNION "
                "SELECT blockid from bgqbpblockmap where bpid = '" + location.getMidplaneLocation() + "' "
                "UNION "
                "SELECT blockid from bgqswitchblockmap where substr(switchid,3,6) = '" + location.getMidplaneLocation() + "' "
                ")"
                ;
        } else if (
                location.getType() == bgq::util::Location::IoBoardOnIoRack ||
                location.getType() == bgq::util::Location::IoBoardOnComputeRack
                )
        {
            sqlstr = "select a.blockid from bgqblock a, bgqcnioblockmap b where b.cnblock = a.blockid and substr(b.ion,1,6) = '" +
                rasEvent.getDetail(RasEvent::LOCATION) + string("' and a.status <> 'F'");
        } else {
            LOG_ERROR_MSG( rasEvent.getDetail(RasEvent::LOCATION) << " is not a valid node board or I/O board location" );
            return INVALID_ARG;
        }

        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        char blockID[33];
        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR,   blockID, sizeof(blockID), &ind1);
        sqlrc = SQLFetch(hstmt);

        for (;sqlrc == SQL_SUCCESS;) {
            const string errmsg("errmsg=block freed due to RAS event control action");
            deque<string> args;
            args.push_back(errmsg);

            setBlockAction(string(blockID), DEALLOCATE_BLOCK, args);
            sqlrc = SQLFetch(hstmt);
        }

        SQLCloseCursor(hstmt);
    }

    return OK;
}


STATUS
postProcessRAS(
               const uint32_t recid,
               std::vector<job::Id>& jobsToKill,
               SparingInfo& sInfo
               )
{

    //  This postProcessRAS path is considered the fast-path for RAS events, since MMCS does a direct insert of the RAS event
    //  into the database, acks the event to mcServer, then comes back and post processes the event.
    //  Post processing is for looking up a jobid when the RAS event didn't have it already, and handling control actions.
    //  Diags RAS does not go through this path, it goes through the putRAS path.


    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }
    DBTEventlog dbe;
    char rec[17], job[33];
    string whereClause(" where recid=");
    string sqlstr;
    BGQDB::job::Id id = 0;
    SQLHANDLE hstmt;
    SQLLEN ind1, ind2;

    sprintf(rec, "%u", recid);
    whereClause += rec;

    // initialize mask for wire sparing
    sInfo.wireMask = 0;
    sInfo.txMask = 0;
    sInfo.rxMask = 0;

    ColumnsBitmap colBitmap;
    colBitmap.set(dbe.JOBID);
    colBitmap.set(dbe.CTLACTION);
    colBitmap.set(dbe.BLOCK);
    colBitmap.set(dbe.LOCATION);
    colBitmap.set(dbe.MESSAGE);
    colBitmap.set(dbe.RAWDATA);
    dbe._columns = colBitmap.to_ulong();

    dbe._jobid = 0; //initialize this to zero, in case the query returns null
    std::vector<job::Id> subBlockJobsToKill;
    SQLRETURN sqlrc = SQL_SUCCESS;
    SQLRETURN ctl_rc = SQL_SUCCESS;

    sqlrc = tx.query(&dbe, whereClause.c_str());
    sqlrc = tx.fetch(&dbe);

    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG(__FUNCTION__ << " RAS event post-processing for recid " << recid << " failed during database retrieval");
        return OK;
    }

    tx.close(&dbe);

    const bgq::util::Location location(string(dbe._location), bgq::util::Location::nothrow);

    if (((dbe._jobid == 0) || (dbe._ind[DBTEventlog::JOBID] == SQL_NULL_DATA))
        && (location.getType() != bgq::util::Location::ComputeCardOnIoBoard)
        && (strcmp(dbe._block,"DefaultControlEventListener" )!=0) )    {  // fill in the jobid, but not for Default listener or IO nodes

        // assume this is a sub-block job since the RAS event insertion code in mmcs_server should handle
        // full-block jobs

        subBlockJobsToKill = BGQDB::job::subBlockRas( string(dbe._block), location );
        if ( !subBlockJobsToKill.empty() ) {
            // ras can only be associated with one job, so pick the first one
            snprintf( job, sizeof(job), "%lld", subBlockJobsToKill[0] );
        }

        if (!id && subBlockJobsToKill.empty()) {  // no jobs found, punt
            LOG_DEBUG_MSG(__FUNCTION__ << " RAS event post-processing for recid " << recid << " failed to find job");
        } else {
            sqlstr = "update bgqeventlog set jobid = " + string(job) + string(" where recid = ") + string(rec);
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG(__FUNCTION__ << " RAS event post-processing for recid " << recid << " failed to update jobid");
            }
        }

    } else   // RAS event already has a jobid
        id = dbe._jobid;

    // For IO blocks, they will have a block of DefaultControlEventListener, unless they are being redirected, so we want to
    // provide the actual block name here
    if ((strcmp(dbe._block,"DefaultControlEventListener" ) == 0) &&
        (location.getType() == bgq::util::Location::ComputeCardOnIoBoard)) {

        sqlstr = "select a.blockid,a.qualifier  from bgqblock a , bgqioblockmap b  where status <> 'F'  and a.blockid = b.blockid  and " +
            string(" (location = '") + string(dbe._location) +
            string("' or location = substr('") + string(dbe._location) + string("',1,6)) ");
        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR,    dbe._block, sizeof(dbe._block), &ind1);
        sqlrc = SQLBindCol(hstmt, 2, SQL_C_CHAR,    dbe._qualifier, sizeof(dbe._qualifier), &ind2);
        sqlrc = SQLFetch(hstmt);
        SQLCloseCursor(hstmt);

        if (sqlrc == 0) {
            sqlstr = "update bgqeventlog set block  = '" + string(dbe._block) + string("',qualifier='" + string(dbe._qualifier) + "'  where recid = ") + string(rec);
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG(__FUNCTION__ << " RAS event post-processing for recid " << recid << " failed to update block");
            }
        }
    }

    if (strlen(dbe._ctlaction) > 0  && !bypass_ctlaction) {  // handle control actions

        //  we are doing this in a particular order
        //  it is known that the checking of IO connections for FREE_COMPUTE_BLOCK must happen before SOFTWARE_IN_ERROR

        if (strstr(dbe._ctlaction, "END_JOB")) {

            if (location.getType() == bgq::util::Location::IoBoardOnComputeRack ||
                location.getType() == bgq::util::Location::IoBoardOnIoRack      ||
                location.getType() == bgq::util::Location::ComputeCardOnIoBoard) {
                // this is an IO node or IO board, so check for the job running on an attached compute block

                // Note: this logic results in all full block and sub-block jobs being killed, for blocks attached
                // to the IO node

                if (location.getType() == bgq::util::Location::ComputeCardOnIoBoard) {
                    sqlstr = "select b.id from bgqcnioblockmap a, bgqjob b where cnblock = b.blockid and ion = '" +  string(dbe._location) + string("' ");
                } else {
                    sqlstr = "select unique b.id from bgqcnioblockmap a, bgqjob b where cnblock = b.blockid and substr(ion,1,6) = '" +  string(dbe._location) + string("' ");
                }

                sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
                sqlrc = SQLBindCol(hstmt, 1, SQL_C_SBIGINT, &id, 8,           &ind1);
                sqlrc = SQLFetch(hstmt);

                for (;sqlrc == SQL_SUCCESS;) {
                    jobsToKill.push_back(id);
                    sqlrc = SQLFetch(hstmt);
                }

                SQLCloseCursor(hstmt);
            } else {
                // assume compute node location
                if (id) {
                    // ensure this job is still active, not point in signaling a job that has already terminated
                    sqlstr = "select count(*) from tbgqjob where id=" + boost::lexical_cast<std::string>( id );
                    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
                    int count = 0;
                    sqlrc = SQLBindCol(hstmt, 1, SQL_C_LONG, &count, sizeof(count), &ind1);
                    if ( SQLFetch(hstmt) == SQL_SUCCESS && count) {
                        jobsToKill.push_back(id);
                    } else {
                        LOG_DEBUG_MSG( "RAS event post-processing for recid " << recid << " skipping END_JOB for " << id << " since it has already terminated" );
                    }
                    SQLCloseCursor(hstmt);
                }

                // add sub-block jobs
                jobsToKill.insert( jobsToKill.end(), subBlockJobsToKill.begin(), subBlockJobsToKill.end() );
            }
        }


        if (strstr(dbe._ctlaction, "FREE_COMPUTE_BLOCK")) {
            string errmsg("errmsg=");
            errmsg.append("freed by RAS event with recid ");
            errmsg.append( boost::lexical_cast<std::string>(recid) );
            errmsg.append( ": " );
            errmsg.append(dbe._message);
            deque<string> args;
            args.push_back(errmsg);

            if (
                    location.getType() != bgq::util::Location::ComputeCardOnIoBoard &&
                    location.getType() != bgq::util::Location::IoBoardOnComputeRack &&
                    location.getType() != bgq::util::Location::IoBoardOnIoRack
               )
            {
                const STATUS result = setBlockAction(string(dbe._block), DEALLOCATE_BLOCK, args);
                switch (result ) {
                    case BGQDB::DUPLICATE:
                    case BGQDB::OK:
                        // do nothing
                        break;
                    default:
                        LOG_ERROR_MSG(__FUNCTION__ << " RAS event post-processing for recid " << recid << " failed to free  block '" << dbe._block << "' (" << result << ")" );
                        break;
                }

            } else {
                // since its legal for an IO node to send RAS with FREE_COMPUTE_BLOCK, find out if there are connected compute blocks
                std::vector<string> connected;
                std::vector<string> ionode;
                ionode.push_back(string(dbe._location));
                STATUS db_status(BGQDB::checkIONodeConnection(ionode, connected ));

                if ( db_status == BGQDB::OK ) {
                    for(unsigned int cn = 0 ; cn < connected.size(); cn++ ) {
                        setBlockAction(connected[cn], DEALLOCATE_BLOCK, args);
                    }
                }
            }

        }

        if (strstr(dbe._ctlaction, "COMPUTE_IN_ERROR")) {
            if (location.getType() == bgq::util::Location::ComputeCardOnIoBoard) {
                sqlstr = "update bgqionode set status = 'E' where location = '" + string(dbe._location) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
            } else {
                sqlstr = "update bgqnode set status = 'E' where location = '" + string(dbe._location) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
            }
        }

        if (strstr(dbe._ctlaction, "SOFTWARE_IN_ERROR")) {
            if (location.getType() == bgq::util::Location::ComputeCardOnIoBoard) {
                sqlstr = "update bgqionode set status = '";
                sqlstr += SOFTWARE_FAILURE;
                sqlstr += "' where status in ('A','F') and location = '" + string(dbe._location) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
            } else {
                sqlstr = "update bgqnode set status = '";
                sqlstr += SOFTWARE_FAILURE;
                sqlstr += "' where status in ('A','F') and location = '" + string(dbe._location) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
            }
        }

        if (strstr(dbe._ctlaction, "BOARD_IN_ERROR")) {
            if (string(dbe._location).substr(4,1) != "I") {
                sqlstr = "update bgqnodecard set status = 'E' where location = '" + string(dbe._location).substr(0,10) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
            } else {
                sqlstr = "update bgqiodrawer set status = 'E' where location = '" + string(dbe._location).substr(0,6) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
                sqlstr = "update bgqionode set status = 'E' where substr(location,1,6) = '" + string(dbe._location).substr(0,6) + string("' ");
                ctl_rc = tx.execStmt(sqlstr.c_str());
            }
        }

         if (strstr(dbe._ctlaction, "DCA_IN_ERROR")) {

             sqlstr = "update bgqnodecarddca set status = 'E' where location = '" + string(dbe._location).substr(0,13) + string("' ");

             ctl_rc = tx.execStmt(sqlstr.c_str());
        }

        if (strstr(dbe._ctlaction, "RACK_IN_ERROR")) {
            sqlstr = "update bgqnodecard set status = 'E' where substr(location,1,3) = '" + string(dbe._location).substr(0,3) + string("' ");
            ctl_rc = tx.execStmt(sqlstr.c_str());
            sqlstr = "update bgqiodrawer set status = 'E' where substr(location,1,3) = '" + string(dbe._location).substr(0,3) + string("' ");
            (void)tx.execStmt(sqlstr.c_str()); // rack may not have top hat drawers, so ignore the return code
        }

        if (strstr(dbe._ctlaction, "CABLE_IN_ERROR")) {
            // pull the register and the mask from the RAWDATA
            // call a function to get back the locations and the mask
            char maskFromEvent[33];
            char regFromEvent[33];
            int  maskInt;
            char fromLoc[17];

            char* matchMask = strstr(dbe._rawdata, "Mask=");
            char* matchReg = strstr(dbe._rawdata, "Register=");
            if (matchMask == NULL || matchReg == NULL)
                ctl_rc = SQL_ERROR;
            else {
                sscanf(matchMask, "Mask=%x", &maskInt);
                sscanf(matchReg, "Register=%s", regFromEvent);

                try {
                    bgq::utility::CableBadWires badWires(string(dbe._location), string(regFromEvent).substr(0,3), maskInt);

                    long int newMask = badWires.getBadWireMask();
                    string portLoc = badWires.getPortLocation();

                    // update the column in the database, the receiver always sends the RAS event but that can
                    // either be the fromlocation or the tolocation column
                    sqlstr = "update bgqcable set status = 'E' where tolocation = '" + portLoc + string("' or fromlocation = '" + portLoc + string("'"));
                    ctl_rc = tx.execStmt(sqlstr.c_str());

                } catch ( std::exception& e ) {
                    LOG_WARN_MSG( e.what() );
                    ctl_rc = SQL_ERROR;
                }
            }
        }

        if (strstr(dbe._ctlaction, "BQL_SPARE")) {
            // pull the register and the mask from the RAWDATA
            // call a function to get back the locations and the mask
            char maskFromEvent[33];
            char regFromEvent[33];
            int  maskInt;

            char* matchMask = strstr(dbe._rawdata, "Mask=");
            char* matchReg = strstr(dbe._rawdata, "Register=");
            if (matchMask == NULL || matchReg == NULL)
                ctl_rc = SQL_ERROR;
            else {
                sscanf(matchMask, "Mask=%x", &maskInt);
                sscanf(matchReg, "Register=%s", regFromEvent);

                try {
                    bgq::utility::CableBadWires badWires(string(dbe._location), string(regFromEvent).substr(0,3), maskInt);
                    const bgq::util::Location location( dbe._location );

                    long int newMask = badWires.getBadWireMask();
                    const string portLoc = badWires.getRxPortLocation();

                    std::ostringstream selectClause;
                    std::ostringstream updateClause;
                    updateClause << "update bgqcable set badwiremask  =  BITOR(badwiremask," << newMask << ") WHERE ";
                    selectClause << "select fromlocation,tolocation,badwiremask from bgqcable WHERE ";
                    switch ( location.getType() ) {
                        case bgq::util::Location::LinkModuleOnNodeBoard:
                            if ( location.getLinkModule() == 4 ) {
                                // this is the I/O link chip
                                updateClause << "fromlocation='" << portLoc << "'";
                                selectClause << "fromlocation='" << portLoc << "'";
                            } else {
                                // The RAS event always comes from the receiver which can be either the
                                // tolocation or fromlocation column
                                updateClause << "tolocation='" << portLoc << "' OR fromlocation='" << portLoc << "'";
                                selectClause << "fromlocation='" << portLoc << "' OR tolocation='" << portLoc << "'";
                            }
                            break;

                        case bgq::util::Location::LinkModuleOnIoBoardInIoRack:
                        case bgq::util::Location::LinkModuleOnIoBoardInComputeRack:
                            updateClause << "tolocation='" << portLoc << "'";
                            selectClause << "tolocation='" << portLoc << "'";
                            break;
                        default:
                            LOG_WARN_MSG("location '" << location << "' is not a link chip location");
                            ctl_rc = SQL_ERROR;
                            break;
                    }

                    ctl_rc = tx.execStmt(updateClause.str().c_str());

                    // get the sender location from the database
                    sqlrc = tx.execQuery(selectClause.str().c_str(), &hstmt);
                    char fromLocation[17];
                    char toLocation[17];
                    sqlrc = SQLBindCol(hstmt, 1, SQL_CHAR, fromLocation, sizeof(fromLocation), &ind1);
                    sqlrc = SQLBindCol(hstmt, 2, SQL_CHAR, toLocation, sizeof(toLocation), &ind1);
                    sqlrc = SQLBindCol(hstmt, 3, SQL_C_SBIGINT, &newMask, 8, &ind2);
                    sqlrc = SQLFetch(hstmt);
                    SQLCloseCursor(hstmt);

                    if (sqlrc == SQL_SUCCESS && ctl_rc == SQL_SUCCESS) {
                        // Pass sender and newMask into BgqBadWires. get back the transmitter link chip, the transmitter register and a 12-bit mask
                        trim_right_spaces(fromLocation);
                        trim_right_spaces(toLocation);
                        if ( portLoc == fromLocation ) {
                            badWires.setTxPortAndBadWireMask(string(toLocation), newMask);
                        } else if ( portLoc == toLocation ) {
                            badWires.setTxPortAndBadWireMask(string(fromLocation), newMask);
                        } else {
                            BOOST_ASSERT( !"unhandled port location" );
                        }

                        // return the information to the caller so it can send a message to mcServer
                        sInfo.txReg = badWires.getTxRegister();
                        sInfo.rxReg = string(regFromEvent).substr(0,3);
                        sInfo.txLoc = badWires.getTxLinkChipLocation();
                        sInfo.rxLoc = string(dbe._location);
                        sInfo.wireMask = badWires.getAggregatedBadFiberMask();
                        sInfo.txMask = badWires.getTxFiberMask();
                        sInfo.rxMask = badWires.getRxFiberMask();
                    } else {
                        ctl_rc = SQL_ERROR;
                    }

                } catch ( std::exception& e ) {
                    ctl_rc = SQL_ERROR;
                }
            }
        }

        if (ctl_rc == SQL_SUCCESS) {
            LOG_INFO_MSG(__FUNCTION__ << " Location " << dbe._location << " had RAS event " << recid << " with control action(s) " << dbe._ctlaction);
        } else {
            LOG_ERROR_MSG(__FUNCTION__ << " Invalid control action(s) " << dbe._ctlaction << " specified for location " << dbe._location << ". Return code " << ctl_rc);
        }

    }


    return OK;
}


STATUS
checkRack(
        const std::vector<std::string>& locations,
        std::vector<string>* invalid
)
{

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

    for (int i=0; i < (int)locations.size() ;i++) {

        tokens.tokenize(locations[i], ", ");  // split on comma or space

        for (uint32_t tok=0; tok< tokens.size(); ++tok) {

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

        for (int io=0; io < (int)IORack.size() ;io++) {
            sqlstr.append(" ('");
            sqlstr.append(IORack[io]);
            sqlstr.append("' ) ");
            if ((io+1) < (int)IORack.size())
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
            invalid->push_back(loc);
            sqlrc = SQLFetch(hstmt);

        }

        SQLCloseCursor(hstmt);
    }

    if (CNRack.size() > 0) {
        sqlstr.clear();
        sqlstr.append("  select CN.loc  from (values  ");

        for (int cn=0; cn < (int)CNRack.size() ;cn++) {
            sqlstr.append(" ('");
            sqlstr.append(CNRack[cn]);
            sqlstr.append("' ) ");
            if ((cn+1) < (int)CNRack.size())
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
            invalid->push_back(loc);
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

STATUS
queryMissing(
        const std::string& block,
        std::vector<string>& missing,
        DIAGS_MODE diags
)
{
    char col1[65];
    string sqlstr;
    SQLLEN ind1,ind2;
    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    unsigned numcnodes=0, numionodes=0;
    bool foundMissing = false;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    sqlstr.append("select numcnodes,numionodes from bgqblock where blockid='");
    sqlstr.append(block);
    sqlstr.push_back('\'');
    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_LONG, &numcnodes, 0, &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }
    if ((sqlrc = SQLBindCol(hstmt, 2, SQL_C_LONG, &numionodes, 0, &ind2)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    if (sqlrc == SQL_NO_DATA_FOUND) {
        LOG_ERROR_MSG("Block " << block << " not found");
        return NOT_FOUND;
    }
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // bool  smallblock = (numcnodes < 512);
    bool  ioblock = (numionodes > 0);

    SQLCloseCursor(hstmt);

    // diags NO_DIAGS  (A)    NORMAL_DIAGS (A,E,F)   SVCACTION_DIAGS (A,E,S,F)
    if (diags == NO_DIAGS) {
        if (ioblock) {
            sqlstr = "select 'IODRAWER: ' || a.location from bgqiodrawer a, bgqioblockmap b where b.blockid = '"
                + block
                + string("' and substr(b.location,1,6) = a.location group by a.location having min(status) not in ('A','F') ")
                + string(" union all select 'IONODE: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
                + block
                + string("' and b.location = a.location group by a.location having min(status) not in ('A','F') ")
                + string(" union all select 'IONODE: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
                + block
                + string("' and substr(a.location,1,6) = b.location group by a.location having min(status) not in ('A','F') ");
        } else {
            sqlstr =  "select 'MIDPLANE: ' || location from bgqmidplane a, bgqbpblockmap b where b.blockid = '"
                + block
                + string("' and b.bpid = a.location group by location having min(status) <> 'A' ")
                + string(" union all  select 'NODEBOARD: ' || c.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c ")
                + string(" where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and c.midplanepos = a.location  group by c.location ")
                + string(" having min(c.status) <> 'A' ")
                + string("  union all select 'NODE: ' || d.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c, bgqnode d  where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and c.midplanepos = a.location and d.nodecardpos = c.position and d.midplanepos = c.midplanepos  ")
                + string(" group by d.location having min(d.status) not in ('A','F') ")
                + string("  union all select 'NODE: ' || d.location from bgqsmallblock b, bgqnode d  where b.blockid = '")
                + block
                + string("' and b.posinmachine = d.midplanepos  and d.nodecardpos = b.nodecardpos  ")
                + string(" group by d.location having min(d.status) not in ('A','F') ")
                + string(" union  all select 'CABLE: ' || linkid from tbgqlinkblockmap where blockid = '")
                + block
                + string("' and linkid in (select source || destination from bgqlink where status <> 'A' ) ")
                + string(" union  select 'MIDPLANE: ' || location from bgqmidplane a, bgqsmallblock b where b.blockid = '")
                + block
                + string("' and b.posinmachine = a.location group by location having min(status) <> 'A' ")
                + string(" union select 'NODEBOARD: ' || c.location ")
                + string(" from bgqsmallblock b, bgqnodecard c where b.blockid = '")
                + block
                + string("' and b.posinmachine = c.midplanepos and c.position = b.nodecardpos group by c.location having min(c.status) <> 'A' ")
                + string(" union select 'PASS THROUGH NODEBOARD: ' || location from bgqnodecard where midplanepos in (select substr(switchid,3,6) from bgqswitchblockmap where blockid='")
                + block
                + string("' and substr(switchid,3,6) not in (select bpid from bgqbpblockmap where blockid='")
                + block
                + string("')) and status <> 'A'")
                ;
        }
    } else {
        string statusFlags;
        if (diags == NORMAL_DIAGS) {
            statusFlags = "('A', 'E', 'F')";
        } else { // service action
            statusFlags = "('A', 'E', 'S', 'F')";
        }

        if (ioblock) {
            sqlstr = "select 'IODRAWER: ' || a.location from bgqiodrawer a, bgqioblockmap b where b.blockid = '"
                + block
                + string("' and b.location = a.location and status not in ") + statusFlags
                + string(" union all select 'IONODE: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
                + block
                + string("' and substr(a.location,1,6) = b.location and status not in ") + statusFlags;
        } else {
            sqlstr =  "select 'MIDPLANE: ' || location from bgqmidplane a, bgqbpblockmap b where b.blockid = '"
                + block
                + string("' and b.bpid = a.location  and status not in ") + statusFlags
                + string(" union all  select 'NODEBOARD: ' || c.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c ")
                + string(" where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and c.midplanepos = a.location   ")
                + string(" and c.status not in ") + statusFlags + string(" union all select 'NODE: ' || d.location from bgqmidplane a, bgqbpblockmap b, bgqnode d  where b.blockid = '")
                + block
                + string("' and b.bpid = a.location and d.midplanepos = a.location  ")
                + string(" and d.status not in ") + statusFlags;
        }
    }

    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    // bind cols
    if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, col1, 65, &ind1)) != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch(hstmt);
    for (;sqlrc == SQL_SUCCESS;) {
        trim_right_spaces(col1);
        missing.push_back(col1);
        foundMissing = true;
        sqlrc = SQLFetch(hstmt);
    }

    SQLCloseCursor(hstmt);

    if (foundMissing == false) {   //check for duplicate MAC or IP addresses
        sqlstr = "select 'DUPLICATE IP: ' || d.location from bgqmidplane a, bgqbpblockmap b, bgqnodecard c, bgqnode d  "
            + string(" where b.blockid = '")
            + block
            + string("' and b.bpid = a.location and c.midplanepos = a.location  and d.nodecardpos = c.position and d.midplanepos = c.midplanepos and d.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ")
            + string(" union select 'DUPLICATE IP: ' || d.location ")
            + string(" from bgqmidplane a, bgqsmallblock b, bgqnodecard c, bgqnode d  where b.blockid = '")
            + block
            + string("' and b.posinmachine = a.location and c.midplanepos = a.location and c.position = b.nodecardpos and d.nodecardpos = c.position and d.midplanepos = c.midplanepos  and d.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ")

            + string(" union  select 'DUPLICATE IP: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
            + block
            + string("' and b.location = a.location and a.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ")
            + string(" union  select 'DUPLICATE IP: ' || a.location from bgqionode a, bgqioblockmap b where b.blockid = '")
            + block
            + string("' and substr(a.location,1,6) = b.location  and a.location in ")
            + string(" (select location from bgqnetconfig  where itemvalue in ")
            + string("  (select itemvalue from bgqnetconfig where itemname like 'ip%address%' group by itemvalue having count(*) > 1))  ");



        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }

        if ((sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, col1, 65, &ind1)) != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }

        sqlrc =  SQLFetch(hstmt);
        for(;sqlrc == SQL_SUCCESS;) {
            missing.push_back(col1);
            sqlrc = SQLFetch(hstmt);
        }

        SQLCloseCursor(hstmt);
    }

    return OK;
}

STATUS
queryError(
        const std::string& block,
        std::vector<string>& missing
)
{
    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    std::ostringstream sql;
    sql << "select numionodes from bgqblock where blockid='" << block << "'";
    SQLHANDLE hstmt;
    SQLRETURN sqlrc = tx.execQuery( sql.str().c_str(), &hstmt );
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    SQLLEN ind1;
    unsigned numionodes=0;
    sqlrc = SQLBindCol( hstmt, 1, SQL_C_LONG, &numionodes, 0, &ind1 );
    if ( sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = SQLFetch( hstmt );
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database fetch failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if ( numionodes == 0 ) {
        LOG_INFO_MSG( block << " is not an I/O block" );
        return INVALID_ARG;
    }

    sql.str("");
    sql <<

 "WITH badIon AS ("

" SELECT location"
  " FROM bgqIoNode"
  " WHERE status = 'E'"
" UNION ALL"
" SELECT ion.location"
  " FROM bgqIoDrawer AS d"
       " JOIN"
       " bgqIoNode AS ion"
       " ON ion.ioPos = d.location"
  " WHERE d.status <> 'A'"

" ), ioLoc AS ("

" SELECT location"
  " FROM bgqIoBlockMap"
  " WHERE blockId = '" << block << "'"
" UNION ALL"
" SELECT ion.location"
  " FROM bgqIoBlockMap AS ibm"
       " JOIN"
       " bgqIoDrawer AS d"
       " ON ibm.location = d.location"
       " JOIN"
       " bgqIoNode AS ion"
       " ON ion.ioPos = d.location"
  " WHERE ibm.blockId = '" << block << "'"

" )"

" SELECT badIon.location"
  " FROM badIon"
       " JOIN"
       " ioLoc"
       " ON badIon.location = ioLoc.location"
  " GROUP BY badIon.location"

       ;

    sqlrc = tx.execQuery( sql.str().c_str(), &hstmt );
    if ( sqlrc != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database query failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    char location[DBVIonode::LOCATION_SIZE + 1];
    sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, location, sizeof(location), &ind1);
    if ( sqlrc != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database bind column failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    while ( (sqlrc = SQLFetch(hstmt)) == SQL_SUCCESS ) {
        missing.push_back(location);
    }

    SQLCloseCursor(hstmt);

    return OK;
}

STATUS
createNodeConfig(
        const std::deque<std::string>& args
        )
{
    long long int GB = 1024 * 1024 * 1024;
    long long int Cust64K = 64 * 1024;
    DBTNodeconfig nc;
    DBTDomainmap dm;
    ColumnsBitmap ncBitmap, dmBitmap;
    SQLRETURN sqlrc;

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    sqlrc = tx.setAutoCommit(false);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database auto commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    bool haveDomain = false;
    int id = 0;

    std::vector<DomainInfo> domains;

    // parse args
    for (deque<string>::const_iterator i = args.begin(); i != args.end(); ++i) {
        const std::string& arg = *i;
        if ( i== args.begin() ) {  // node config name must come first
            if ((arg.length() > sizeof(nc._nodeconfig)) || !checkIdentifierValidCharacters(arg)) {
                LOG_ERROR_MSG(__FUNCTION__ << " config name exceeds allowed size or has invalid characters");
                return INVALID_ID;
            }
            ncBitmap.set(nc.NODECONFIG);
            sprintf(nc._nodeconfig, "%s", arg.c_str());
            dmBitmap.set(dm.NODECONFIG);
            sprintf(dm._nodeconfig, "%s", arg.c_str());
        } else if ( !arg.compare(0,10,"raspolicy=") ) {
            if (haveDomain) {
                LOG_ERROR_MSG(__FUNCTION__ << " raspolicy must precede domain info");
                return DB_ERROR;
            }
            ncBitmap.set(nc.RASPOLICY);
            sprintf(nc._raspolicy, "%s", arg.substr(10).c_str());
        } else if ( !arg.compare(0,13,"nodeoptions={") ) {
            if (haveDomain) {
                LOG_ERROR_MSG(__FUNCTION__ << " nodeoptions must precede domain info");
                return DB_ERROR;
            }
            ncBitmap.set(nc.NODEOPTIONS);

            std::string nodeOptionsString = arg.substr(13); // since the node options can contain blanks, group it together

            while(((*i).find_last_of("}")+1) != (*i).length() && (i+1) != args.end() ) {
                i++;
                nodeOptionsString += " ";
                nodeOptionsString += *i;
            }

            sprintf(nc._nodeoptions, "%s", nodeOptionsString.substr(0,nodeOptionsString.length()-1).c_str());
        } else if ( !arg.compare(0,8,"domain={") ) {

            if (!haveDomain) {  // insert the nodeconfig now
                nc._columns = ncBitmap.to_ulong();
                sqlrc = tx.insert(&nc);
                if (sqlrc != SQL_SUCCESS) {
                    LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                    return DB_ERROR;
                }
            }

            StringTokenizer tokens;

            dmBitmap.set(dm.DOMAINID);
            sprintf(dm._domainid,"domain%i",id++); // provide a default domain id

            std::string domainString = arg.substr(7); // since the domain info can contain blanks, group it together

            while(((*i).find_last_of("}")+1) != (*i).length() && (i+1) != args.end() ) {
                i++;
                domainString += " ";
                domainString += *i;
            }

            tokens.tokenize(domainString, "{} "); // split based on the brackets or a space

            // process the information for this domain
            for (uint32_t tok=0; tok< tokens.size(); ++tok) {
                if ( !tokens[tok].compare(0,6,"cores=") ) {
                    StringTokenizer cores;
                    cores.tokenize(tokens[tok].substr(6),"-");
                    if (cores.size() == 1) {
                        dmBitmap.set(dm.STARTCORE);
                        dmBitmap.set(dm.ENDCORE);
                        try {
                            dm._startcore = boost::lexical_cast<int>(cores[0]);
                            dm._endcore = dm._startcore;
                        } catch (const boost::bad_lexical_cast& e) {
                            LOG_ERROR_MSG(__FUNCTION__ << " invalid core numbers ");
                            return INVALID_ARG;
                        }
                    } else if (cores.size() == 2) {
                        dmBitmap.set(dm.STARTCORE);
                        dmBitmap.set(dm.ENDCORE);
                        try {
                            dm._startcore = boost::lexical_cast<int>(cores[0]);
                            dm._endcore = boost::lexical_cast<int>(cores[1]);
                        } catch (const boost::bad_lexical_cast& e) {
                            LOG_ERROR_MSG(__FUNCTION__ << " invalid core numbers ");
                            return INVALID_ARG;
                        }
                    } else {
                        LOG_ERROR_MSG(__FUNCTION__ << " invalid core numbers ");
                        return INVALID_ARG;
                    }
                } else if ( !tokens[tok].compare(0,3,"id=") ) {
                    sprintf(dm._domainid, "%s", tokens[tok].substr(3).c_str());
                } else if ( !tokens[tok].compare(0,8,"options=") ) {
                    dmBitmap.set(dm.DOMAINOPTIONS);
                    const unsigned rc = snprintf(dm._domainoptions, sizeof(dm._domainoptions) - 1, "%s", tokens[tok].substr(8).c_str());
                    if ( rc >= sizeof(dm._domainoptions) - 1 ) {
                        LOG_ERROR_MSG( "domain options size of " << tokens[tok].size() << " greater than maximum " << sizeof(dm._domainoptions) - 1 );
                        return INVALID_ARG;
                    }
                } else if ( !tokens[tok].compare(0,7,"memory=") ) {
                    const std::string memory(tokens[tok].substr(7));
                    const std::string::size_type dash = memory.find_first_of( '-' );
                    if ( dash == std::string::npos ) {
                        LOG_ERROR_MSG(__FUNCTION__ << " invalid hex values for memory ");
                        return INVALID_ARG;
                    }

                    const std::string starting = memory.substr(0, dash);
                    const std::string ending = memory.substr(dash + 1);

                    dmBitmap.set(dm.STARTADDR);
                    dmBitmap.set(dm.ENDADDR);
                    istringstream sa(starting);
                    if (!(sa>>std::hex>>dm._startaddr)  ||
                            !(boost::algorithm::all( starting, boost::algorithm::is_xdigit())) ) {
                        LOG_ERROR_MSG(__FUNCTION__ << " invalid hex value for starting memory address ");
                        return INVALID_ARG;
                    }
                    if ( ending == "-1" ) {
                        // special case for sentinel value, it isn't hexadecimal
                        dm._endaddr = -1;
                    } else {
                        istringstream ea(ending);
                        if (!(ea>>std::hex>>dm._endaddr)  ||
                                !(boost::algorithm::all( ending, boost::algorithm::is_xdigit())) ) {
                            LOG_ERROR_MSG(__FUNCTION__ << " invalid hex value for ending memory address ");
                            return INVALID_ARG;
                        }
                    }
                } else if ( !tokens[tok].compare(0,9,"custaddr=") ) {
                    dmBitmap.set(dm.CUSTADDR);
                    istringstream ca(tokens[tok].substr(9));
                    if (!(ca>>std::hex>>dm._custaddr)  ||
                        !(boost::algorithm::all( tokens[tok].substr(9), boost::algorithm::is_xdigit())) ) {
                        LOG_ERROR_MSG(__FUNCTION__ << " invalid hex value for customization address ");
                        return INVALID_ARG;
                    }
                } else if ( !tokens[tok].compare(0,7,"images=") ) {
                    dmBitmap.set(dm.DOMAINIMG);
                    sprintf(dm._domainimg, "%s", tokens[tok].substr(7).c_str());
                } else {
                    // assume options
                    dmBitmap.set(dm.DOMAINOPTIONS);

                    // ensure we have enough space to store these options, add 1 for the space
                    const size_t size = strlen(dm._domainoptions) + tokens[tok].size() + 1;
                    if ( size > sizeof(dm._domainoptions) - 1 ) {
                        LOG_ERROR_MSG( "domain options size of " << size << " greater than maximum " << sizeof(dm._domainoptions) - 1 );
                        return INVALID_ARG;
                    }

                    strcat(dm._domainoptions, " ");
                    strcat(dm._domainoptions, tokens[tok].c_str());
                }
            }

            if (!dmBitmap.test(dm.DOMAINIMG) ||                     // cannot omit images
                (haveDomain && !dmBitmap.test(dm.STARTCORE)) )  {  // cannot omit cores in a multi-domain node config
                LOG_ERROR_MSG(__FUNCTION__ << " no images provided for domain or cores omitted");
                return INVALID_ARG;
            }

            if (!dmBitmap.test(dm.STARTCORE)) {  // cores omitted, provide default values
                dmBitmap.set(dm.STARTCORE);
                dmBitmap.set(dm.ENDCORE);
                dm._startcore = 0;
                dm._endcore = 15;
            }

            if (!dmBitmap.test(dm.STARTADDR)) {  // memory omitted, provide default values
                dmBitmap.set(dm.STARTADDR);
                dmBitmap.set(dm.ENDADDR);
                dm._startaddr = dm._startcore * GB;
                dm._endaddr =  ((dm._endcore + 1) * GB)-1; // 1 GB per core
            }

            if ((dm._startcore > dm._endcore) || (dm._startcore < 0) || (dm._endcore > 16)) {
                LOG_ERROR_MSG(__FUNCTION__ << " cores (" << dm._startcore << "-" << dm._endcore << ") out of range");
                return INVALID_ARG;
            }

            if (dm._endaddr  == -1) {
                // no need to check ranges
            } else if ((dm._startaddr > dm._endaddr) || (dm._startaddr < 0) || (dm._endaddr >= (16 * GB))) {
                LOG_ERROR_MSG(__FUNCTION__ << " addresses (" << dm._startaddr << "-" << dm._endaddr << ") out of range");
                return INVALID_ARG;
            }

            if (!dmBitmap.test(dm.CUSTADDR)) {  // customization addr omitted, provide default value
                dmBitmap.set(dm.CUSTADDR);
                dm._custaddr = ((dm._endaddr > Cust64K) ? (dm._endaddr - Cust64K) : 0);
            }

            // overlap checking
            DomainInfo domainInfo;
            domainInfo.startCore = dm._startcore;
            domainInfo.endCore = dm._endcore;
            domainInfo.startAddr = dm._startaddr;
            domainInfo.endAddr = dm._endaddr;
            for(unsigned prevDomain = 0 ; prevDomain < domains.size() ; prevDomain++) {
                if ((domainInfo.startCore >= domains[prevDomain].startCore && domainInfo.startCore <= domains[prevDomain].endCore) ||
                        (domainInfo.endCore >= domains[prevDomain].startCore && domainInfo.endCore <= domains[prevDomain].endCore)) {
                    LOG_ERROR_MSG(__FUNCTION__ << " overlapping cores");
                    return INVALID_ARG;
                }
                if ((domainInfo.startAddr >= domains[prevDomain].startAddr && domainInfo.startAddr <= domains[prevDomain].endAddr) ||
                        (domainInfo.endAddr >= domains[prevDomain].startAddr && domainInfo.endAddr <= domains[prevDomain].endAddr)) {
                    LOG_ERROR_MSG(__FUNCTION__ << " overlapping memory addresses");
                    return INVALID_ARG;
                }
            }

            domains.push_back(domainInfo);

            dm._columns = dmBitmap.to_ulong();

            sqlrc = tx.insert(&dm);
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
                return DB_ERROR;
            }
            haveDomain = true;
            dmBitmap.reset(); // wipe out this domain info, since there can be multiple domains
            dmBitmap.set(dm.NODECONFIG); // but preserve the name of the node config for the next domain

            if (i == args.end() )
                break;
        } else {  // unrecognized value
            LOG_ERROR_MSG(__FUNCTION__ << " unrecognized argument");
            return INVALID_ARG;
        }
    }

    if (!haveDomain) {  // no domain was provided, so this is an invalid node config
        LOG_ERROR_MSG(__FUNCTION__ << " no domain provided");
        return INVALID_ARG;
    }

    if ( (sqlrc = tx.commit()) != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__  );
        return DB_ERROR;
    }

    return OK;
}

STATUS
copyBlock(
        const std::string& block,
        const std::string& new_block,
        const std::string& owner
)
{
    DBTBlock dbo;
    ColumnsBitmap colBitmap;
    string insertstm;
    SQLRETURN sqlrc;

    if (!isBlockIdValid(new_block, __FUNCTION__)) {
        return INVALID_ID;
    }

    colBitmap.set(dbo.BLOCKID);
    colBitmap.set(dbo.NUMIONODES);
    colBitmap.set(dbo.NUMCNODES);
    colBitmap.set(dbo.DESCRIPTION);
    colBitmap.set(dbo.ISTORUS);
    colBitmap.set(dbo.SIZEA);
    colBitmap.set(dbo.SIZEB);
    colBitmap.set(dbo.SIZEC);
    colBitmap.set(dbo.SIZED);
    colBitmap.set(dbo.SIZEE);
    colBitmap.set(dbo.MLOADERIMG);
    colBitmap.set(dbo.NODECONFIG);
    colBitmap.set(dbo.OWNER);
    colBitmap.set(dbo.BOOTOPTIONS);
    dbo._columns = colBitmap.to_ulong();

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", block.c_str());

    TxObject tx(DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        LOG_ERROR_MSG(__FUNCTION__ << " Unable to obtain database connection");
        return CONNECTION_ERROR;
    }

    sqlrc = tx.setAutoCommit(false);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database auto commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    sqlrc = tx.queryByKey(&dbo);
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

    snprintf(dbo._blockid, sizeof(dbo._blockid), "%s", new_block.c_str());
    sprintf(dbo._description,"%s","Generated via copyblock");
    snprintf(dbo._owner, sizeof(dbo._owner), "%s", owner.c_str());

    sqlrc = tx.insert(&dbo);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
        return DB_ERROR;
    }

    if (dbo._numionodes > 0 ) {  // io block
        insertstm = "insert into tbgqioblockmap (BLOCKID,LOCATION) "
            + string(" select '") + new_block + string("' ,LOCATION ")
            + string(" from tbgqioblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    } else
    if (dbo._numcnodes < 512) {  //smallblock
        insertstm = "insert into tbgqsmallblock (BLOCKID,POSINMACHINE,NODECARDPOS) "
            + string(" select '") + new_block + string("' ,POSINMACHINE,NODECARDPOS ")
            + string(" from tbgqsmallblock where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    } else {
        insertstm = "insert into tbgqbpblockmap (BLOCKID,BPID, ACOORD, BCOORD, CCOORD, DCOORD) "
            + string(" select '") + new_block + string("' ,BPID, ACOORD, BCOORD, CCOORD, DCOORD ")
            + string(" from tbgqbpblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }

        insertstm = "insert into tbgqswitchblockmap (BLOCKID,SWITCHID,INCLUDE,ENABLEPORTS) "
            + string(" select '") + new_block + string("' ,SWITCHID,INCLUDE,ENABLEPORTS ")
            + string(" from tbgqswitchblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }

        insertstm = "insert into tbgqlinkblockmap (BLOCKID,LINKID) "
            + string(" select '") + new_block + string("' ,LINKID ")
            + string(" from tbgqlinkblockmap where blockid = '") + block + string("'");

        sqlrc = tx.execStmt(insertstm.c_str());
        if (sqlrc != SQL_SUCCESS) {
            LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__ );
            return DB_ERROR;
        }
    }

    if ( (sqlrc = tx.commit()) != SQL_SUCCESS ) {
        LOG_ERROR_MSG( "Database commit failed with error: " << sqlrc << " at " << __FUNCTION__ << ':' << __LINE__  );
        return DB_ERROR;
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
    if (strcmp(code, BLOCK_ALLOCATED) == 0)
        return ALLOCATED;
    if (strcmp(code, BLOCK_BOOTING) == 0)
        return BOOTING;
    if (strcmp(code, BLOCK_INITIALIZED) == 0)
        return INITIALIZED;
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
