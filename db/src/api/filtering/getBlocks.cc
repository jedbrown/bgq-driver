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
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/Exception.h>
#include <db/include/api/filtering/getBlocks.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <sstream>
#include <string>
#include <vector>

using namespace std;

LOG_DECLARE_FILE("database");

namespace BGQDB {
namespace filtering {

cxxdb::ResultSetPtr getBlocks(
        const BGQDB::filtering::BlockFilter& filter,
        const BGQDB::filtering::BlockSort& sort,
        cxxdb::Connection& conn
        )
{
    BGQDB::DBTBlock dbBlock;
    BGQDB::ColumnsBitmap block_cols;

    // Start by getting basic block info
    block_cols.set(dbBlock.BLOCKID);
    block_cols.set(dbBlock.CREATIONID);
    block_cols.set(dbBlock.NUMCNODES);
    block_cols.set(dbBlock.OWNER);
    block_cols.set(dbBlock.USERNAME);
    block_cols.set(dbBlock.ISTORUS);
    block_cols.set(dbBlock.STATUS);
    block_cols.set(dbBlock.ACTION);
    block_cols.set(dbBlock.SEQID);
    block_cols.set(dbBlock.MLOADERIMG);
    block_cols.set(dbBlock.NODECONFIG);
    block_cols.set(dbBlock.DESCRIPTION);
    block_cols.set(dbBlock.OPTIONS);
    block_cols.set(dbBlock.BOOTOPTIONS);
    block_cols.set(dbBlock.SIZEA);
    block_cols.set(dbBlock.SIZEB);
    block_cols.set(dbBlock.SIZEC);
    block_cols.set(dbBlock.SIZED);
    block_cols.set(dbBlock.SIZEE);
    block_cols.set(dbBlock.NUMIONODES);
    block_cols.set(dbBlock.STATUSLASTMODIFIED);

    dbBlock.setColumns(block_cols);

    // Build query from filter provided
    ostringstream queryStatement;
    string conjunction = " WHERE ";

    const BGQDB::filtering::BlockFilter::Statuses blockStatuses = filter.getStatuses();

    // Search all statuses if filter is not set
    if (blockStatuses.size() > 0) {
        queryStatement << conjunction;
        conjunction = " ( STATUS = ";
        if (blockStatuses.find(BGQDB::filtering::BlockFilter::Free) != blockStatuses.end()) {
            queryStatement << conjunction << "'" << BGQDB::BLOCK_FREE << "'";
            conjunction = " OR STATUS = ";
        }
        if (blockStatuses.find(BGQDB::filtering::BlockFilter::Allocated) != blockStatuses.end()) {
            queryStatement << conjunction << "'" << BGQDB::BLOCK_ALLOCATED << "'";
            conjunction = " OR STATUS = ";
        }
        if (blockStatuses.find(BGQDB::filtering::BlockFilter::Booting) != blockStatuses.end()) {
            queryStatement << conjunction << "'" << BGQDB::BLOCK_BOOTING << "'";
            conjunction = " OR STATUS = ";
        }
        if (blockStatuses.find(BGQDB::filtering::BlockFilter::Initialized) != blockStatuses.end()) {
            queryStatement << conjunction << "'" << BGQDB::BLOCK_INITIALIZED << "'";
            conjunction = " OR STATUS = ";
        }
        if (blockStatuses.find(BGQDB::filtering::BlockFilter::Terminating) != blockStatuses.end()) {
            queryStatement << conjunction << "'" << BGQDB::BLOCK_TERMINATING << "'";
        }
        queryStatement << " )";
        conjunction = " AND ";
    }

    // Block size filter can be Large, Small or All
    BGQDB::filtering::BlockFilter::BlockSize::Value blockSize = filter.getSize();
    if (blockSize == BGQDB::filtering::BlockFilter::BlockSize::Large) {
        queryStatement << conjunction << "NUMCNODES >= 512";
        conjunction = " AND ";
    } else {
        if (blockSize == BGQDB::filtering::BlockFilter::BlockSize::Small) {
            queryStatement << conjunction << "NUMCNODES < 512";
            conjunction = " AND ";
        }
    }

    if ( filter.getBlockType() == BlockFilter::BlockType::Compute ) {
        queryStatement << conjunction << dbBlock.NUMIONODES_COL << " = 0";
        conjunction = " AND ";
    } else if ( filter.getBlockType() == BlockFilter::BlockType::Io ) {
        queryStatement << conjunction << dbBlock.NUMCNODES_COL << " = 0";
        conjunction = " AND ";
    }

    cxxdb::ParameterNames parameter_names;

    string block_name = filter.getName();
    if (!block_name.empty()) {
        queryStatement << conjunction << "BLOCKID = ?";
        parameter_names.push_back ("blockId");
        conjunction = " AND ";
    }

    string block_owner = filter.getOwner();
    if (!block_owner.empty()) {
        queryStatement << conjunction << "OWNER = ?";
        parameter_names.push_back ("owner");
        conjunction = " AND ";
    }

    string block_user = filter.getUser();
    if (!block_user.empty()) {
        queryStatement << conjunction << "USERNAME = ?";
        parameter_names.push_back ("userName");
    }

    if (sort.isSorted()) {
        queryStatement << " ORDER BY " <<
        (sort.getField() == BlockSort::Field::ComputeNodeCount ? BGQDB::DBTBlock::NUMCNODES_COL :
        sort.getField() == BlockSort::Field::CreateDate ? BGQDB::DBTBlock::CREATEDATE_COL :
        sort.getField() == BlockSort::Field::IoNodeCount ? BGQDB::DBTBlock::NUMIONODES_COL :
        sort.getField() == BlockSort::Field::Owner ? BGQDB::DBTBlock::OWNER_COL :
        sort.getField() == BlockSort::Field::User ? BGQDB::DBTBlock::USERNAME_COL :
        sort.getField() == BlockSort::Field::Status ? BGQDB::DBTBlock::STATUS_COL :
        sort.getField() == BlockSort::Field::StatusLastModified ? BGQDB::DBTBlock::STATUSLASTMODIFIED_COL :
        BGQDB::DBTBlock::BLOCKID_COL) << " " <<
        (sort.getSortOrder() == SortOrder::Ascending ? "ASC" : "DESC");

        if (sort.getField() != BlockSort::Field::Name) {
            queryStatement << ", " << BGQDB::DBTBlock::BLOCKID_COL << " " << (sort.getSortOrder() == SortOrder::Ascending ? "ASC" : "DESC");
        }
    }

    LOG_DEBUG_MSG("Querying for blocks");

    cxxdb::ResultSetPtr rs_ptr;

    try {
        cxxdb::QueryStatementPtr stmt_ptr(dbBlock.prepareSelect(conn, queryStatement.str(), parameter_names));

        if (!block_name.empty()) {
            stmt_ptr->parameters()[BGQDB::DBTBlock::BLOCKID_COL].set(block_name);
        }

        if (!block_owner.empty()) {
            stmt_ptr->parameters()[BGQDB::DBTBlock::OWNER_COL].set(block_owner);
        }

        if (!block_user.empty()) {
            stmt_ptr->parameters()[BGQDB::DBTBlock::USERNAME_COL].set(block_user);
        }

        rs_ptr = stmt_ptr->execute();

        rs_ptr->internalize(stmt_ptr);

        return rs_ptr;

    } catch (const BGQDB::Exception& e) {
        throw; // just re-throw it
    } catch (const cxxdb::DatabaseException& e) {
        throw; // just re-throw it
    } catch (const std::exception& e) {
        throw; // just re-throw it
    }
}

} // filtering
} // BGQDB



