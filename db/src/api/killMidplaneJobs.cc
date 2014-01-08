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

#include "tableapi/DBConnectionPool.h"
#include "tableapi/TxObject.h"
#include "tableapi/gensrc/DBTBlock.h"
#include "tableapi/gensrc/DBTJob.h"

#include <utility/include/Log.h>

#include <string>
#include <vector>

LOG_DECLARE_FILE( "database" );

namespace BGQDB {

STATUS
killMidplaneJobs(
        const std::string& location,
        std::vector<job::Id>* jobs_list_out,
        const bool listOnly
        )
{
    std::string sqlstr, block;
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

} // namespace BGQDB
