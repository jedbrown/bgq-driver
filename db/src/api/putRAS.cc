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


#include "cxxdb/cxxdb.h"
#include "dataconv.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/TxObject.h"
#include "tableapi/gensrc/DBTDiagseventlog.h"
#include "tableapi/gensrc/DBTEventlog.h"

#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <deque>
#include <map>
#include <string>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

STATUS
putRAS(
        const std::string& block,
        const std::map<std::string, std::string>& rasmap,
        const timeval& rastime,
        const job::Id jobid,
        const bool diags
)
{
    //  This putRAS path is the slow-path for RAS events and is only used for running diags.
    //  Therefore, this path will not do post-processing of RAS, will not do a lookup of the jobid, and will
    //  not honor control actions

    try {
        const cxxdb::ConnectionPtr connection( DBConnectionPool::Instance().getConnection() );
        if ( !connection ) {
            LOG_ERROR_MSG("Unable to obtain database connection");
            return CONNECTION_ERROR;
        }

        // even though this function can insert events into the tbgqdiagseventlog and tbgqeventlog
        // tables, we assume the column names and ordering are the same

        typedef std::map<std::string, std::string> ColumnMap;
        static const ColumnMap columns = boost::assign::map_list_of
            (RasEvent::JOBID, DBTEventlog::JOBID_COL)
            (RasEvent::MSG_ID, DBTEventlog::MSG_ID_COL)
            (RasEvent::CATEGORY, DBTEventlog::CATEGORY_COL)
            (RasEvent::COMPONENT, DBTEventlog::COMPONENT_COL)
            (RasEvent::SEVERITY, DBTEventlog::SEVERITY_COL)
            (RasEvent::MESSAGE, DBTEventlog::MESSAGE_COL)
            (RasEvent::LOCATION, DBTEventlog::LOCATION_COL)
            (RasEvent::ECID, DBTEventlog::ECID_COL)
            (RasEvent::CPU, DBTEventlog::CPU_COL)
            (RasEvent::COUNT, DBTEventlog::COUNT_COL)
            (RasEvent::SERIAL_NUMBER, DBTEventlog::SERIALNUMBER_COL)
            (RasEvent::CONTROL_ACTION, DBTEventlog::CTLACTION_COL)
            (RasEvent::BLOCKID, DBTEventlog::BLOCK_COL)
            ;

        ColumnsBitmap colbitmap;
        colbitmap.set(DBTEventlog::JOBID);
        colbitmap.set(DBTEventlog::EVENT_TIME);
        colbitmap.set(DBTEventlog::MSG_ID);
        colbitmap.set(DBTEventlog::CATEGORY);
        colbitmap.set(DBTEventlog::COMPONENT);
        colbitmap.set(DBTEventlog::SEVERITY);
        colbitmap.set(DBTEventlog::MESSAGE);
        colbitmap.set(DBTEventlog::LOCATION);
        colbitmap.set(DBTEventlog::ECID);
        colbitmap.set(DBTEventlog::CPU);
        colbitmap.set(DBTEventlog::COUNT);
        colbitmap.set(DBTEventlog::SERIALNUMBER);
        colbitmap.set(DBTEventlog::RAWDATA);
        colbitmap.set(DBTEventlog::BLOCK);
        colbitmap.set(DBTEventlog::CTLACTION);

        DBTEventlog dbe( colbitmap );
        DBTDiagseventlog diage( colbitmap );

        const cxxdb::AbstractStatementPtr statement(
                diags ?
                connection->prepareUpdate( diage.getInsertStatement(), diage.calcColumnNames() ) :
                connection->prepareUpdate( dbe.getInsertStatement(), dbe.calcColumnNames() )
                );

        // NULL all columns to start
        for ( ColumnMap::const_iterator i = columns.begin(); i != columns.end(); ++i ) {
            statement->parameters()[ i->second ].setNull();
        }

        // the time supplied as input is specified in UTC, we need to convert it
        // to our local time
        boost::posix_time::ptime time(
                boost::posix_time::from_time_t( rastime.tv_sec )
                );
        time += boost::posix_time::microseconds( rastime.tv_usec );
        typedef boost::date_time::c_local_adjustor<boost::posix_time::ptime> LocalAdj;
        LOG_DEBUG_MSG( "event time: " << LocalAdj::utc_to_local(time) );
        statement->parameters()[ DBTEventlog::EVENT_TIME_COL ].set( LocalAdj::utc_to_local(time) );

        std::string rawdata;
        LOG_DEBUG_MSG( rasmap.size() << " RAS details" );
        for ( std::map<std::string,std::string>::const_iterator i = rasmap.begin(); i != rasmap.end(); ++i ) {
            const ColumnMap::const_iterator name = columns.find( i->first );
            if ( name != columns.end() ) {
                if ( name->first == RasEvent::COUNT || name->first == RasEvent::CPU ) {
                    statement->parameters()[ name->second ].cast( boost::lexical_cast<int>(i->second) );
                    LOG_DEBUG_MSG( "set " << name->second << " to " << i->second );
                } else if ( name->first == RasEvent::ECID ) {
                    const cxxdb::Bytes ecid( DBTEventlog::ECID_SIZE, 0 );
                    if ( hexchar2bitdata(const_cast<unsigned char*>(&ecid[0]), DBTEventlog::ECID_SIZE, i->second) ) {
                        bool truncated;
                        statement->parameters()[ name->second ].set( ecid, &truncated );
                        if ( truncated ) {
                            LOG_WARN_MSG( name->first << " value " << i->second << " truncated" );
                        }
                    } else {
                        statement->parameters()[ name->second ].setNull();
                    }
                } else {
                    bool truncated;
                    statement->parameters()[ name->second ].set( i->second, &truncated );
                    if ( truncated ) {
                        LOG_WARN_MSG( name->first  << " value " << i->second << " truncated" );
                    } else {
                        LOG_DEBUG_MSG( "set " << name->second << " to " << i->second );
                    }
                }
            } else if (!i->second.empty()) {
                // don't write empty values into rawdata
                rawdata.append( i->first + "=" + i->second + "; " );
            }
        }

        if ( !rawdata.empty() ) {
            bool truncated;
            statement->parameters()[ DBTEventlog::RAWDATA_COL ].set( rawdata, &truncated );
            if ( truncated ) {
                LOG_WARN_MSG( DBTEventlog::RAWDATA_COL << " value " << rawdata << " truncated");
            } else {
                LOG_DEBUG_MSG( "set " << DBTEventlog::RAWDATA_COL << " to " << rawdata );
            }
        } else {
            statement->parameters()[ DBTEventlog::RAWDATA_COL ].setNull();
        }

        if ( !block.empty() ) {
            bool truncated;
            statement->parameters()[ DBTEventlog::BLOCK_COL ].set( block, &truncated );
            if ( truncated ) {
                LOG_WARN_MSG( DBTEventlog::BLOCK_COL << " value " << block << " truncated" );
            } else {
                LOG_DEBUG_MSG( "set " << DBTEventlog::BLOCK_COL << " to " << block );
            }
        }

        if ( jobid ) {
            statement->parameters()[ DBTEventlog::JOBID_COL ].cast( jobid );
        }

        boost::dynamic_pointer_cast<cxxdb::UpdateStatement>(statement)->execute();
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        return DB_ERROR;
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
    std::string sqlstr;
    std::string rasMessage("select recid from NEW TABLE (insert into tbgqeventlog (msg_id,category,component,severity,message,location,ctlaction,block");
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
        LOG_ERROR_MSG("Unable to obtain database connection");
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

        const std::string whereClause = " where location='" + rasEvent.getDetail(RasEvent::LOCATION)+ "' and status not in ('F','C')";
        const int nrows = tx.count("BGQServiceAction", whereClause.c_str());
        if (nrows >=1)
            serviceAction = true;
    }

    if (serviceAction) {
        LOG_DEBUG_MSG( "RAS event for location " << rasEvent.getDetail(RasEvent::LOCATION) << " ignored due to pending service action.");
        return OK;
    }

    sqlrc = tx.execQuery(rasMessage.c_str(), &hstmt);
    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG( "Database insert failed with error: " << sqlrc << " at " << __LINE__ );
        return DB_ERROR;
    }
    if ( recid_out ) {
        sqlrc = SQLBindCol(hstmt, 1, SQL_INTEGER, recid_out, 4, &ind1);
        sqlrc = SQLFetch(hstmt);
    }

    const bgq::util::Location location( rasEvent.getDetail(RasEvent::LOCATION), bgq::util::Location::nothrow );

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).find("BOARD_IN_ERROR") != std::string::npos) {
        if (rasEvent.getDetail(RasEvent::LOCATION).substr(4,1) != "I") {
            sqlstr = "update bgqnodecard set status = 'E' where location = '" + rasEvent.getDetail(RasEvent::LOCATION).substr(0,10) + std::string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("RAS event control action processing for BOARD_IN_ERROR failed to update hardware status, rc = " << sqlrc);
            }
        } else {
            sqlstr = "update bgqiodrawer set status = 'E' where location = '" + rasEvent.getDetail(RasEvent::LOCATION).substr(0,6) + std::string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("RAS event control action processing for BOARD_IN_ERROR failed to update I/O drawer status, rc = " << sqlrc);
            }
            sqlstr = "update bgqionode set status = 'E' where substr(location,1,6) = '" + rasEvent.getDetail(RasEvent::LOCATION).substr(0,6) + std::string("' ");
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("RAS event control action processing for BOARD_IN_ERROR failed to update I/O node status, rc = " << sqlrc);
            }
        }
    }

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).find("END_JOB") != std::string::npos) {
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
                rasEvent.getDetail(RasEvent::LOCATION) + std::string("' ");
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

    if (rasEvent.getDetail(RasEvent::CONTROL_ACTION).find("FREE_COMPUTE_BLOCK") != std::string::npos) {
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
                rasEvent.getDetail(RasEvent::LOCATION) + std::string("' and a.status <> 'F'");
        } else {
            LOG_ERROR_MSG( rasEvent.getDetail(RasEvent::LOCATION) << " is not a valid node board or I/O board location" );
            return INVALID_ARG;
        }

        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        char blockID[33];
        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR,   blockID, sizeof(blockID), &ind1);
        sqlrc = SQLFetch(hstmt);

        for (;sqlrc == SQL_SUCCESS;) {
            const std::string errmsg("errmsg=block freed due to RAS event control action");
            std::deque<std::string> args;
            args.push_back(errmsg);

            setBlockAction(std::string(blockID), DEALLOCATE_BLOCK, args);
            sqlrc = SQLFetch(hstmt);
        }

        SQLCloseCursor(hstmt);
    }

    return OK;
}

} // BGQDB
