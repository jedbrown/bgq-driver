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
#include "job/SubBlockRas.h"
#include "tableapi/TxObject.h"
#include "tableapi/DBConnectionPool.h"
#include "tableapi/gensrc/DBTEventlog.h"

#include <bgq_util/include/Location.h>
#include <control/include/mcServer/defaults.h>
#include <utility/include/CableBadWires.h>
#include <utility/include/Log.h>

#include <deque>
#include <string>
#include <vector>

using std::string;

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

extern bool bypass_ctlaction;

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
        LOG_ERROR_MSG("Unable to obtain database connection");
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
    colBitmap.set(dbe.MSG_ID);
    dbe._columns = colBitmap.to_ulong();

    dbe._jobid = 0; //initialize this to zero, in case the query returns null
    std::vector<job::Id> subBlockJobsToKill;
    SQLRETURN ctl_rc = SQL_SUCCESS;

    tx.query(&dbe, whereClause.c_str());
    SQLRETURN sqlrc = tx.fetch(&dbe);

    if (sqlrc != SQL_SUCCESS) {
        LOG_ERROR_MSG("query for recid " << recid << " failed");
        return OK;
    }

    tx.close(&dbe);

    const bgq::util::Location location(string(dbe._location), bgq::util::Location::nothrow);

    if (((dbe._jobid == 0) || (dbe._ind[DBTEventlog::JOBID] == SQL_NULL_DATA))
        && (location.getType() != bgq::util::Location::ComputeCardOnIoBoard)
        && (dbe._block != mc_server::DefaultListener) )    {  // fill in the jobid, but not for Default listener or IO nodes

        // assume this is a sub-block job since the RAS event insertion code in mmcs_server should handle
        // full-block jobs

        subBlockJobsToKill = BGQDB::job::subBlockRas( string(dbe._block), location );
        if ( !subBlockJobsToKill.empty() ) {
            // ras can only be associated with one job, so pick the first one
            snprintf( job, sizeof(job), "%lld", subBlockJobsToKill[0] );
        }

        if (!id && subBlockJobsToKill.empty()) {  // no jobs found, punt
            LOG_DEBUG_MSG("RAS event post-processing for recid " << recid << " failed to find job");
        } else {
            sqlstr = "update bgqeventlog set jobid = " + string(job) + string(" where recid = ") + string(rec);
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("RAS event post-processing for recid " << recid << " failed to update jobid");
            }
        }

    } else   // RAS event already has a jobid
        id = dbe._jobid;

    // For IO blocks, they will have a block of DefaultControlEventListener, unless they are being redirected, so we want to
    // provide the actual block name here
    if (
            dbe._block == mc_server::DefaultListener &&
            (location.getType() == bgq::util::Location::ComputeCardOnIoBoard ||
             location.getType() == bgq::util::Location::IoBoardOnComputeRack ||
             location.getType() == bgq::util::Location::IoBoardOnIoRack)
        )
    {
        sqlstr = "select a.blockid,a.qualifier from bgqblock a , bgqioblockmap b  where status <> 'F'  and a.blockid = b.blockid  and " +
            string(" (location = '") + string(dbe._location) +
            string("' or location = substr('") + string(dbe._location) + string("',1,6)) ");
        tx.execQuery(sqlstr.c_str(), &hstmt);
        SQLBindCol(hstmt, 1, SQL_C_CHAR, dbe._block, sizeof(dbe._block), &ind1);
        SQLBindCol(hstmt, 2, SQL_C_CHAR, dbe._qualifier, sizeof(dbe._qualifier), &ind2);
        sqlrc = SQLFetch(hstmt);
        SQLCloseCursor(hstmt);

        if (sqlrc == SQL_SUCCESS) {
            sqlstr = "update bgqeventlog set block = '" + string(dbe._block) + string("',qualifier='" + string(dbe._qualifier) + "' where recid = ") + string(rec);
            sqlrc = tx.execStmt(sqlstr.c_str());
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("RAS event post-processing for recid " << recid << " failed to update block");
            }
        }
    }

    if (strlen(dbe._ctlaction) > 0 && !bypass_ctlaction) {  // handle control actions
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

                tx.execQuery(sqlstr.c_str(), &hstmt);
                SQLBindCol(hstmt, 1, SQL_C_SBIGINT, &id, 8, &ind1);
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
                    tx.execQuery(sqlstr.c_str(), &hstmt);
                    int count = 0;
                    SQLBindCol(hstmt, 1, SQL_C_LONG, &count, sizeof(count), &ind1);
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
            std::deque<string> args;
            args.push_back(errmsg);

            if (
                    location.getType() != bgq::util::Location::ComputeCardOnIoBoard &&
                    location.getType() != bgq::util::Location::IoBoardOnComputeRack &&
                    location.getType() != bgq::util::Location::IoBoardOnIoRack &&
                    dbe._block != mc_server::DefaultListener
               )
            {
                const STATUS result = setBlockAction(string(dbe._block), DEALLOCATE_BLOCK, args);
                switch (result ) {
                    case BGQDB::DUPLICATE:
                    case BGQDB::OK:
                        // do nothing
                        break;
                    default:
                        LOG_ERROR_MSG("RAS event post-processing for recid " << recid << " failed to free block '" << dbe._block << "' (" << result << ")" );
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
            char regFromEvent[33];
            int maskInt;

            char* matchMask = strstr(dbe._rawdata, "Mask=");
            char* matchReg = strstr(dbe._rawdata, "Register=");
            if (matchMask == NULL || matchReg == NULL)
                ctl_rc = SQL_ERROR;
            else {
                sscanf(matchMask, "Mask=%x", &maskInt);
                sscanf(matchReg, "Register=%32s", regFromEvent);

                try {
                    const bgq::utility::CableBadWires badWires(string(dbe._location), string(regFromEvent).substr(0,3), maskInt);

                    const string portLoc = badWires.getPortLocation();

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
            char regFromEvent[33];
            int maskInt;

            char* matchMask = strstr(dbe._rawdata, "Mask=");
            char* matchReg = strstr(dbe._rawdata, "Register=");
            if (matchMask == NULL || matchReg == NULL)
                ctl_rc = SQL_ERROR;
            else {
                sscanf(matchMask, "Mask=%x", &maskInt);
                sscanf(matchReg, "Register=%32s", regFromEvent);

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
                    tx.execQuery(selectClause.str().c_str(), &hstmt);
                    char fromLocation[17];
                    char toLocation[17];
                    SQLBindCol(hstmt, 1, SQL_CHAR, fromLocation, sizeof(fromLocation), &ind1);
                    SQLBindCol(hstmt, 2, SQL_CHAR, toLocation, sizeof(toLocation), &ind1);
                    SQLBindCol(hstmt, 3, SQL_C_SBIGINT, &newMask, 8, &ind2);
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

        const bool multiple( strstr(dbe._ctlaction, ",") );
        if (ctl_rc == SQL_SUCCESS) {
            LOG_INFO_MSG("Location " << dbe._location << " had 0x" << dbe._msg_id << " RAS event " << recid << " with control action" << (multiple ? "s " : " ") << dbe._ctlaction);
        } else {
            LOG_ERROR_MSG("Invalid control action" << (multiple ? "s " : " ") << dbe._ctlaction << " specified for location " << dbe._location << " with recid " << recid << " from event 0x" << dbe._msg_id);
        }
    }

    return OK;
}

} // BGQDB
