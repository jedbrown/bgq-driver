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

#include <bgsched/bgsched.h>
#include <bgsched/Block.h>
#include <bgsched/BlockFilter.h>
#include <bgsched/InputException.h>

#include <bgsched/core/core.h>
#include <bgsched/core/BlockSort.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <utility/include/Log.h>

#include <execinfo.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <strings.h>

using namespace bgsched;
using namespace bgsched::core;
using namespace log4cxx;
using namespace std;

LOG_DECLARE_FILE("create_jobs");

void createLargeBlock()
{
    try {
        string owner("tbudnik");
        Block::Ptr block;
        Block::Midplanes midplanes;
        Block::PassthroughMidplanes passthroughMidplanes;
        Block::DimensionConnectivity dimensionConnectivity;

        //------------------------------------------------
        // Create 512 cnode large block
        //-------------------------------------------------
        // Set connectivity to Torus in all dimensions
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
        {
            dimensionConnectivity[dim] = Block::Connectivity::Torus;
        }
        // Set midplane
        midplanes.push_back("R00-M1");
        block = Block::create(midplanes, passthroughMidplanes, dimensionConnectivity);
        block->setName("dummyblock"); // Set the name
        block->add(owner); // Add to db
    } catch (const bgsched::InputException& e) {
        switch (e.getError().toValue())
        {
            case bgsched::InputErrors::InvalidMidplanes:
                LOG_INFO_MSG("Invalid midplanes given for Block::Create()");
                return;
            case  bgsched::InputErrors::InvalidConnectivity:
                LOG_INFO_MSG("Invalid connectivity given for Block::Create()");
                return;
            case bgsched::InputErrors::BlockNotCreated:
                LOG_INFO_MSG("Block::Create() can not create block from input arguments");
                return;
            default:
                LOG_INFO_MSG("Unexpected exception value from Block::Create()");
                return;
        }
    }
}

int deleteActiveJobs()
{
    SQLHANDLE hstmt;
    auto_ptr<BGQDB::TxObject> tx;

    try {
        tx.reset(new BGQDB::TxObject(BGQDB::DBConnectionPool::Instance()));
        // Check if got a DB connection
        if (!tx->getConnection()) {
            LOG_ERROR_MSG("Unexpected error getting database connection");
            return -1;
        }
        string sqlstr = "delete from TBGQJob where id > 0";
        SQLRETURN sqlrc = tx->execQuery(sqlstr.c_str(), &hstmt);

        if (sqlrc == SQL_SUCCESS || sqlrc == SQL_NO_DATA) {
            return 0;
        } else {
            LOG_ERROR_MSG("Unexpected database error deleting active jobs.");
            return -1;
        }
        sqlrc = SQLCloseCursor(hstmt);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling deleteActiveJobs");
        return -1;
    }

    return 0;
}

int deleteHistoryJobs()
{
    SQLHANDLE hstmt;
    auto_ptr<BGQDB::TxObject> tx;

    try {
        tx.reset(new BGQDB::TxObject(BGQDB::DBConnectionPool::Instance()));
        // Check if got a DB connection
        if (!tx->getConnection()) {
            LOG_ERROR_MSG("Unexpected error getting database connection");
            return -1;
        }
        string sqlstr = "delete from TBGQJob_history where id > 0";
        SQLRETURN sqlrc = tx->execQuery(sqlstr.c_str(), &hstmt);

        if (sqlrc == SQL_SUCCESS || sqlrc == SQL_NO_DATA) {
            return 0;
        } else {
            LOG_ERROR_MSG("Unexpected database error deleting history jobs.");
            return -1;
        }
        sqlrc = SQLCloseCursor(hstmt);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling deleteHistoryJobs");
        return -1;
    }

    return 0;
}

int createActiveJobs(int activeJobs)
{
    SQLHANDLE hstmt;
    auto_ptr<BGQDB::TxObject> tx;
    SQLRETURN sqlrc;
    string sqlstr;

    try {
        tx.reset(new BGQDB::TxObject(BGQDB::DBConnectionPool::Instance()));
        // Check if got a DB connection
        if (!tx->getConnection()) {
            LOG_ERROR_MSG("Unexpected error getting database connection");
            return -1;
        }

        Block::Ptrs blockVector; // Vector of blocks returned by getBlocks
        BlockFilter blockFilter;
        BlockSort blockSort;
        blockFilter.setName("dummyblock");

        try {
            blockVector = core::getBlocks(blockFilter, blockSort);
        } catch (...) { // Handle all exceptions
            LOG_ERROR_MSG("Unexpected error calling getBlocks()");
            return -1;
        }

        // Check if we got any blocks back
        if (blockVector.empty()) {
            // Create the dummy block name
            createLargeBlock();
        }

        LOG_INFO_MSG("Begin active job creation.");

        // Insert jobs
        for (int i=0; i < activeJobs; i++) {
            sqlstr = "insert into tbgqjob (blockid,username,schedulerData,executable,status,args,envs,workingdir,processesPerNode,nodesUsed,shapeA,shapeB,shapeC,shapeD,shapeE,hostname,pid,np) VALUES ('dummyblock','dummy','job_R','ex6','R','arg1 arg2','env1 env2 env3 env4','/bgscratch',4,32,2,2,2,2,2,'bgqsim',567,31)";
            sqlrc = tx->execQuery(sqlstr.c_str(), &hstmt);
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("Unexpected database error inserting active job.");
                return -1;
            }
        }
        sqlrc = SQLCloseCursor(hstmt);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling createActiveJobs");
        return -1;
    }

    return 0;
}

int createHistoryJobs(int historyJobs)
{
    SQLHANDLE hstmt;
    auto_ptr<BGQDB::TxObject> tx;
    SQLRETURN sqlrc;
    string sqlstr;

    try {
        tx.reset(new BGQDB::TxObject(BGQDB::DBConnectionPool::Instance()));
        // Check if got a DB connection
        if (!tx->getConnection()) {
            LOG_ERROR_MSG("Unexpected error getting database connection");
            return -1;
        }

        Block::Ptrs blockVector; // Vector of blocks returned by getBlocks
        BlockFilter blockFilter;
        BlockSort blockSort;
        blockFilter.setName("dummyblock");

        try {
            blockVector = core::getBlocks(blockFilter, blockSort);
        } catch (...) { // Handle all exceptions
            LOG_ERROR_MSG("Unexpected error calling getBlocks()");
            return -1;
        }

        // Check if we got any blocks back
        if (blockVector.empty()) {
            // Create the dummy block name
            createLargeBlock();
        }

        LOG_INFO_MSG("Begin history job creation.");

        // Insert jobs
        for (int i=0; i < historyJobs; i++) {
            sqlstr = "insert into tbgqjob_history (blockid,creationID,id,username,schedulerData,executable,status,args,envs,workingdir,processesPerNode,nodesUsed,shapeA,shapeB,shapeC,shapeD,shapeE,hostname,pid,seqId,mapping,exitstatus,np,startTime) VALUES ('dummyblock',34567,1234,'dummy','job_T','ex6','T','arg1 arg2','env1 env2 env3 env4','/bgscratch',4,32,2,2,2,2,2,'bgqsim',567,999,'ABCDET',0,32,'2011-05-22-14.22.45.000001')";
            sqlrc = tx->execQuery(sqlstr.c_str(), &hstmt);
            if (sqlrc != SQL_SUCCESS) {
                LOG_ERROR_MSG("Unexpected database error inserting history job.");
                return -1;
            }
        }
        sqlrc = SQLCloseCursor(hstmt);
    } catch (...) { // Handle all exceptions
        LOG_ERROR_MSG("Unexpected error calling createHistoryJobs");
        return -1;
    }

    return 0;

}

void printHelp()
{
    printf("Parameters:\n");
    printf("  -deleteActiveJobs                   - Delete all current 'active' jobs in database table\n");
    printf("  -createActiveJobs numberToCreate    - Number of 'active' jobs to create in database table\n");
    printf("  -deleteHistoryJobs                  - Delete all current 'history' jobs in database table\n");
    printf("  -createHistoryJobs numberToCreate   - Number of 'history' jobs to create in database table\n");
    printf("  -verbose 1|2|3|4|5|6|7              - Set the logging verbose level (1..7) (Defaults to INFO)\n");
    printf("                                          1=OFF,2=FATAL,3=ERROR,4=WARN,5=INFO,6=DEBUG,7=TRACE\n");
    printf("  -properties                         - Set the bg.properties file name\n");
    printf("  -help                               - Print this help text\n");
}

int main(int argc, char *argv[])
{
    char* argKey;
    char* argVal;

    bool isDeleteActiveJobs = false;
    bool isDeleteHistoryJobs = false;

    int historyJobs = 0;
    int activeJobs = 0;

    int rc = 0;

    // Print help text if no arguments passed
    if (argc == 1) {
        printHelp();
        return 0;
    }

    // iterate through args first to get -properties and -verbose
    string properties;

    // Default logging to Info
    uint32_t verboseLevel = 5;
    for (int i = 1; i < argc; ++i) {
        if (strcasecmp(argv[i], "-properties")  == 0 || strcasecmp(argv[i], "--properties")  == 0) {
            if (++i == argc) {
                printf("properties keyword specified without an argument value\n");
                exit(-1);
            } else {
                properties = argv[i];
            }
        } else if (strcasecmp(argv[i], "-verbose")  == 0 || strcasecmp(argv[i], "--verbose") == 0 || strcasecmp(argv[i], "-v")  == 0) {
            if (++i == argc) {
                printf("verbose keyword specified without an argument value\n");
                exit(-1);
            } else {
                try {
                    verboseLevel = boost::lexical_cast<uint32_t>(argv[i]);
                } catch (const boost::bad_lexical_cast& e) {
                    printf("verbose keyword specified with a bad value\n");
                    exit(-1);
                }
            }
        } else if (strcasecmp(argv[i], "-help")  == 0 || strcasecmp(argv[i], "--help")  == 0 || strcasecmp(argv[i], "-h")  == 0) {
            printHelp();
            exit(0);
        }
    }

    // Initialize logger and database
    bgsched::init(properties);

    log4cxx::LoggerPtr logger(Logger::getLogger("ibm.bgsched"));

    // 1=OFF,2=FATAL,3=ERROR,4=WARN,5=INFO,6=DEBUG,7=TRACE
    switch (verboseLevel) {
      case 1:
        logger->setLevel(log4cxx::Level::getOff());
        break;
      case 2:
        logger->setLevel(log4cxx::Level::getFatal());
        break;
      case 3:
        logger->setLevel(log4cxx::Level::getError());
        break;
      case 4:
        logger->setLevel(log4cxx::Level::getWarn());
        break;
      case 5:
        logger->setLevel(log4cxx::Level::getInfo());
        break;
      case 6:
        logger->setLevel(log4cxx::Level::getDebug());
        break;
      case 7:
      case 0: // Conversion error on input - use default
      default:
        logger->setLevel(log4cxx::Level::getTrace());
        break;
    }

    int argNbr = 1;
    while (argNbr < argc) {
        argKey = argv[argNbr];
        argNbr++;
        if (argNbr < argc) {
            argVal = argv[argNbr];
        } else {
            argVal = NULL;
        }
        if (strcasecmp(argKey, "-deleteActiveJobs") == 0 || strcasecmp(argKey, "--deleteActiveJobs") == 0) {
            isDeleteActiveJobs = true;
        } else if (strcasecmp(argKey, "-deleteHistoryJobs") == 0 || strcasecmp(argKey, "--deleteHistoryJobs") == 0) {
            isDeleteHistoryJobs = true;
        } else if (strcasecmp(argKey, "-createActiveJobs") == 0 || strcasecmp(argKey, "--createActiveJobs") == 0) {
            if (argVal != NULL) {
                try {
                    activeJobs = boost::lexical_cast<int>(argVal);
                } catch (const boost::bad_lexical_cast& e) {
                    printf("createActiveJobs specified with a bad value\n");
                    return -1;
                }
                argNbr++;
            } else {
                printf("createActiveJobs keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-createHistoryJobs") == 0 || strcasecmp(argKey, "--createHistoryJobs") == 0) {
            if (argVal != NULL) {
                try {
                    historyJobs = boost::lexical_cast<int>(argVal);
                } catch (const boost::bad_lexical_cast& e) {
                    printf("createHistoryJobs specified with a bad value\n");
                    return -1;
                }
                argNbr++;
            } else {
                printf("createHistoryJobs keyword specified without an argument value\n");
                return -1;
            }
        } else if (strcasecmp(argKey, "-verbose") == 0 || strcasecmp(argKey, "--verbose") == 0 || strcasecmp(argKey, "-v") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else if (strcasecmp(argKey, "-properties") == 0 || strcasecmp(argKey, "--properties") == 0) {
            // ignore, we've already parsed this guy
            ++argNbr;
        } else {
            printf("parameter %s not recognized\n", argKey);
            printHelp();
            return 0;
        }
    }

    if (isDeleteActiveJobs) {
        rc = deleteActiveJobs();
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling deleteActiveJobs");
           return -1;
        }
    }

    if (isDeleteHistoryJobs) {
        rc = deleteHistoryJobs();
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling deleteHistoryJobs");
           return -1;
        }
    }

    if (activeJobs > 0) {
        rc = createActiveJobs(activeJobs);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling createActiveJobs");
           return -1;
        }
    }

    if (historyJobs > 0) {
        rc = createHistoryJobs(historyJobs);
        if (0 != rc) {
           LOG_ERROR_MSG("Unexpected error calling createHistoryJobs");
           return -1;
        }
    }

    LOG_DEBUG_MSG("Completed Successfully");
    return 0;
}
