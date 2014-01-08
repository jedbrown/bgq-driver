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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <algorithm>
#include <sstream>
#include <bitset>
#include <openssl/rand.h>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/scope_exit.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>

#include <hlcs/include/runjob/commands/KillJob.h>

#include <control/include/bgqconfig/BGQPersonality.h>
#include <control/include/bgqconfig/BGQWirePort.h>
#include <control/include/bgqconfig/BGQTopology.h>
#include <control/include/mcServer/MCServerRef.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/BGQDBlib.h>
#include <db/include/api/Exception.h>
#include <db/include/api/ioUsage.h>
#include <db/include/api/dataconv.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <ras/include/RasEvent.h>
#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>
#include <utility/include/Exec.h>

#include "DBBlockController.h"
#include "IOBlockController.h"
#include "CNBlockController.h"
#include "DBConsoleController.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "RunJobConnection.h"
#include "ras.h"
#include "NeighborInfo.h"

LOG_DECLARE_FILE( "mmcs" );

#define LOGGING_DECLARE_DBBLOCK_ID_MDC \
    log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + getBase()->_blockName + "} " );
#define LOGGING_DECLARE_DBBLOCK_USER_MDC \
    log4cxx::MDC _block_user_mdc_( "user", std::string("[") + getBase()->_userName + "] " );

// Static member initialization
std::map<BGQDB::job::Id,int> DBBlockController::_pending_kills;

string  errString(SQLHANDLE hndl /* handle */ )     {
  SQLCHAR message[SQL_MAX_MESSAGE_LENGTH + 1];
  SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1];
  SQLINTEGER sqlcode;
  SQLSMALLINT length, i;

  i = 1;
  string errmsg = "";

  while (SQLGetDiagRec(SQL_HANDLE_STMT,
		       hndl,
		       i,
		       sqlstate,
		       &sqlcode,
		       message,
		       SQL_MAX_MESSAGE_LENGTH + 1,
		       &length) == SQL_SUCCESS)
    {
        errmsg += " SQLSTATE: ";
        errmsg += string((char*)sqlstate);
        errmsg += " Message: ";
        errmsg += string((char*)message);
        i++;
    }
  return errmsg;
}

string
DBBlockController::strDBError(int result)
{
    string errmsg;

    switch (result) {
        case BGQDB::OK:
            errmsg = "";
            break;
        case BGQDB::DB_ERROR:
            errmsg = "Unable to access database";
            break;
        case BGQDB::FAILED:
            errmsg = "Operation failed";
            break;
        case BGQDB::INVALID_ID:
            errmsg = "Invalid identifier";
            break;
        case BGQDB::NOT_FOUND:
            errmsg = "Not found";
            break;
        case BGQDB::DUPLICATE:
            errmsg = "Duplicate";
            break;
        case BGQDB::XML_ERROR:
            errmsg = "File error";
            break;
        case BGQDB::CONNECTION_ERROR:
            errmsg = "Database connection error";
            break;
        default:
            errmsg = "Unexpected database error";
            break;
    }
    return errmsg;
}

DBBlockController::DBBlockController(string userName, string blockName) :
    BlockHelper(new BlockControllerBase(NULL, userName.c_str(), blockName.c_str(), true)),
    _allocate_block_start(0),
    dbe(),
    _dbj(),
    _sqlstmt(0),
    _sqlquery(0),
    _tx(NULL),
    _cbc(this),
    _counters( getBase() ),
    _set_state_mutex(),
    _rasInsertionCount(0),
    _rasInsertionTime()
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
}

DBBlockController::DBBlockController(BlockPtr ptr) :
    BlockHelper(ptr),
    _allocate_block_start(0),
    dbe(),
    _dbj(),
    _sqlstmt(0),
    _sqlquery(0),
    _tx(NULL),
    _cbc(this),
    _counters( ptr ),
    _set_state_mutex(),
    _rasInsertionCount(0),
    _rasInsertionTime()
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
}

DBBlockController::~DBBlockController() {
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    if ( _rasInsertionCount ) {
        const double rate = static_cast<double>(_rasInsertionCount) / _rasInsertionTime.total_milliseconds() * 1000.0;
        LOG_INFO_MSG(
                "inserted " << _rasInsertionCount << " ras events in " <<
                _rasInsertionTime.total_milliseconds() << "ms: " <<
                rate << " per second"
                );
    }

    _cbc.stop();
}

BGQDB::STATUS
DBBlockController::setBlockStatus(std::string blockname,
                                  BGQDB::BLOCK_STATUS status,
                                  const std::deque<std::string>& option) {
    boost::mutex::scoped_lock(_set_state_mutex);
    return BGQDB::setBlockStatus(blockname, status, option);
}

void DBBlockController::Die()
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_INFO_MSG(__FUNCTION__);
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    bool disconnecting = getBase()->isDisconnecting(); // must do this before DBBlockController::disconnect()
    PthreadMutexHolder mutex; // 'smart' mutex holder: destructor will release the mutex

    //
    // ** Start of critical section
    // ** allow only one thread at a time for each individual block
    //

    int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);
    deque<string> dq;
    DBBlockController::disconnect(dq);

    if (disconnecting)
    {
        result = setBlockStatus(getBase()->_blockName, BGQDB::FREE);
        if (result != BGQDB::OK)
        {
            if (BGQDB::getBlockInfo(getBase()->_blockName, bInfo) == BGQDB::OK) {
                LOG_ERROR_MSG("Die() setBlockStatus(" << getBase()->_blockName << ", FREE) failed, result=" << result << ", current block state=" << bInfo.status);
            } else {
                LOG_ERROR_MSG("Die() setBlockStatus(" << getBase()->_blockName << ", FREE) failed, result=" << result);
            }
        }
        else
            LOG_INFO_MSG("Die() setBlockStatus(" << getBase()->_blockName << ", FREE) successful");
    } else {
        if(!disconnecting) {
            LOG_DEBUG_MSG("Not disconnecting.  Will not free block.");
        }
    }

    mutex_rc = mutex.Unlock();
    assert(mutex_rc == 0);
    //
    // ** end of critical section
    //
}

void
DBBlockController::allocateBlock(deque<string> args, MMCSCommandReply& reply)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    deque<string> setBlockStatus_options;

    const PerformanceCounters::Timer::Ptr allocateTimer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "allocate block" )
        ;
    allocateTimer->dismiss();

    if (getBase()->isDisconnecting()) {
        reply << FAIL << "block is being deallocated, retry later" << DONE;
        return;
    }
    if (getBase()->isCreated()) {
        reply << FAIL << "block is already allocated" << DONE;
        return;
    }

    // process local arguments. non-local arguments are passed to BlockController::create_block and BlockController::connect
    if (args.size() == 0) {
        reply << FAIL << "args?" << DONE;
        return;
    }
    string blockName = args[0];
    vector<int> removeArgs;	// arguments that are not to be passed to BlockController::create_block
    bool no_connect = false;	// skip connecting to block resources?

    for (unsigned i = 1; i < args.size(); ++i) {
        // split based on the equal sign
        vector<string> tokens;
        tokens = BlockControllerBase::Tokenize(args[i], "=");

        if (tokens[0] == "no_connect") {
            no_connect = true;
            removeArgs.push_back(i);
        } else if (tokens[0] == "shared") {
            setBlockStatus_options.push_back("shared");
            no_connect = true;
        }//  else {
        //     reply << FAIL << "unknown argument: " << args[i] << DONE;
        //     return;
        // }
    }

    for (unsigned i = 0; i < removeArgs.size(); ++i) {
        args.erase(args.begin() + removeArgs[i]);
    }

    //
    // Begin the actual block allocation and creation
    //
    // ** Start of critical section
    // ** allow only one thread at a time for each individual block
    //
    PthreadMutexHolder mutex; // 'smart' mutex holder: destructor will release the mutex

    int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK) {
        reply << FAIL << strDBError(result) << ": unable to retrieve block information" << DONE;
        return;
    }

    // must have valid node config info
    if (bInfo.domains.size() == 0) {
        reply << FAIL <<  "block has invalid node config or domain information" << DONE;
        return;
    }

    // block state must be Free or Allocated
    if(strcmp(bInfo.status, BGQDB::BLOCK_FREE) != 0 &&
       strcmp(bInfo.status, BGQDB::BLOCK_ALLOCATED) != 0) {
        reply << FAIL << "block is already allocated" << DONE;
        return;
    }

    // generate a boot cookie
    getBase()->getBootCookie();

    // set block state to Allocated
    if ( strcmp( bInfo.status, BGQDB::BLOCK_FREE ) == 0 ) {
        // set owner name
        setBlockStatus_options.push_back(
                std::string("user=") + getBase()->_userName
                );

        if ((result = setBlockStatus(blockName, BGQDB::ALLOCATED, setBlockStatus_options)) != BGQDB::OK) {
            mutex.Unlock();		// must unlock the mutex before setDisconnecting()
            LOG_ERROR_MSG("DBBlockController::allocateBlock() setBlockStatus(" << getBase()->_blockName << ", ALLOCATED) failed, result=" << boost::lexical_cast<unsigned>(result) << ", current block state=" << bInfo.status);
            reply << FAIL << strDBError(result) << ": unable to set block state - hardware resources may be allocated to a different block" << DONE;
            getBase()->setDisconnecting(true, reply.str());
            return;
        } else {
            LOG_INFO_MSG("DBBlockController::allocateBlock() setBlockStatus(" << getBase()->_blockName << ", ALLOCATED) succeeded");
        }
    }
    mutex_rc = mutex.Unlock();
    assert(mutex_rc == 0);
    //
    // ** end of critical section
    //

    time(&_allocate_block_start);
    // Set securityKey
    unsigned char keyBuffer[PERSONALITY_LEN_SECKEY] = {0};
    const int rc = RAND_bytes( keyBuffer, sizeof(keyBuffer) );
    if ( rc == 0 ) {
        reply << FAIL << "unable to set security key on block: " << rc << DONE;
        return;
    }
    result = BGQDB::setBlockSecurityKey(blockName, keyBuffer, sizeof(keyBuffer) );

    std::ostringstream key;
    for ( unsigned i = 0; i < sizeof(keyBuffer); ++i ) {
        key << std::hex << static_cast<unsigned>( keyBuffer[i] );
    }
    LOG_TRACE_MSG( "security key: 0x" << key.str() );

    if (result != BGQDB::OK) {
        reply << FAIL << strDBError(result) << ": unable to set security key on block" << DONE;
        result = setBlockStatus(blockName, BGQDB::FREE);
        if(result != BGQDB::OK) {
            LOG_ERROR_MSG("setBlockStatus(" << blockName << ", FREE) failed (set security key), error " << result);
        }
        else
            LOG_INFO_MSG("setBlockStatus(" << blockName << ", FREE) succeeded (set security key).");

        getBase()->setDisconnecting(true, reply.str());
        return;
    }

    allocateTimer->dismiss( false );
    allocateTimer->stop();

    //
    // create the block
    //
    create_block(args, reply);
    if (reply.getStatus() != 0)
        return;


    //
    // establish connections to the block resources
    //
    if (!no_connect) {

        const PerformanceCounters::Timer::Ptr timer = _counters.create()
            ->id( this->getBlockName() )
            ->mode( bgq::utility::performance::Mode::Value::Basic )
            ->function( "boot" )
            ->subFunction( "create target set" )
            ;
        timer->dismiss();

        // connect to the targets in the block
        // use a perm target set when this mmcs server is running the database monitor,
        // otherwise use a temporary target set

        BlockControllerTarget target(BlockPtr(getBase()), "{*}", reply);
        args.pop_front();	// remove blockname argument
        args.push_back(MMCSProperties::getProperty(DFT_TGTSET_TYPE));
        if(getBase()->isIOBlock())
            boost::dynamic_pointer_cast<IOBlockController>(getBase())->connect(args, reply, &target);
        else
            boost::dynamic_pointer_cast<CNBlockController>(getBase())->connect(args, reply, &target);
        if (reply.getStatus() != 0) {
            int result = setBlockStatus(blockName, BGQDB::FREE);
            if(result != BGQDB::OK) {
                LOG_ERROR_MSG("setBlockStatus(" << blockName << ", FREE) failed (connect), error " << result);
            }
            else
                LOG_INFO_MSG("setBlockStatus(" << blockName << ", FREE) succeeded (connect).");

            getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
            return;
        } else {
            timer->dismiss( false );
        }
    }
    // reply is already set
    return;
}

void
DBBlockController::create_block(deque<string> args, MMCSCommandReply& reply)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_INFO_MSG(__FUNCTION__);
    BGQDB::STATUS result;

    const PerformanceCounters::Timer::Ptr getXmlTimer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "load block XML" )
        ;
    getXmlTimer->dismiss();

    if (args.size() == 0)
    {
        reply << FAIL << "args?" << DONE;
        return;
    }

    deque<string> createArgs(args); // make a copy of our command arguments
    string blockName = createArgs[0]; createArgs.pop_front();

    // extract the block xml from the database
    stringstream blockStream;

    bool diagsArg = (find(args.begin(), args.end(), "diags") != args.end());
    if ((result = BGQDB::getBlockXML(blockStream, blockName, diagsArg)) != BGQDB::OK)
    {
        reply << FAIL << strDBError(result) << ": unable to retrieve block xml" << DONE;
        int result = setBlockStatus(blockName, BGQDB::TERMINATING);
        if(result != BGQDB::OK) {
            LOG_ERROR_MSG("setBlockStatus(" << blockName << ", TERMINATING) failed (getBlockXML), error " << result);
        }
        else
            LOG_INFO_MSG("setBlockStatus(" << blockName << ", TERMINATING) succeeded (getBlockXML).");

        getBase()->setDisconnecting(true, reply.str());
        return;
    }
    getXmlTimer->dismiss( false );
    getXmlTimer->stop();

    const PerformanceCounters::Timer::Ptr createTimer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "create block" )
        ;
    createTimer->dismiss();

    if (args.size() == 0)
    createArgs.push_front(string("foobar")); // add a bogus filename to the front of the list

    //
    // create the block
    //
    getBase()->create_block(createArgs, reply, &blockStream);
    if (reply.getStatus() != 0)
    {
        int result = setBlockStatus(blockName, BGQDB::TERMINATING);
        if(result != BGQDB::OK) {
            LOG_ERROR_MSG("setBlockStatus(" << blockName << ", TERMINATING) failed (create_block), error " << result);
        }
        else
            LOG_INFO_MSG("setBlockStatus(" << blockName << ", TERMINATING) succeeded (create_block).");

        getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
        return;
    }

    int linkless = getBase()->linkless_mps();
    if(linkless > 0 && getBase()->_diags == false) {
        int result = setBlockStatus(blockName, BGQDB::TERMINATING);
        if(result != BGQDB::OK) {
            LOG_WARN_MSG("setBlockStatus(" << blockName << ", TERMINATING) failed (linkless_mps()), error " << result);
        }
        else
            LOG_INFO_MSG("setBlockStatus(" << blockName << ", TERMINATING) succeeded (linkless_mps).");

        reply << FAIL << linkless << " midplanes have no I/O link" << DONE;
        getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
    }
    // Get the create ID
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        LOG_ERROR_MSG("unable to connect to database");
        reply << FAIL << "Unable to connect to database." << DONE;
    }

    std::ostringstream sqlstrm;
    sqlstrm << "select CREATIONID from TBGQBLOCK where BLOCKID=\'" << blockName << "\'";
    LOG_DEBUG_MSG("Sending query \"" << sqlstrm.str() << "\" to database.");
    SQLHANDLE hstmt;
    int createId;
    SQLLEN index;
    SQLRETURN sqlresult = tx.execQuery( sqlstrm.str().c_str(), &hstmt);
    sqlresult = SQLBindCol(hstmt, 1, SQL_INTEGER, &createId, 4, &index);
    sqlresult = SQLFetch(hstmt);
    SQLCloseCursor(hstmt);

    if(sqlresult != 0) {
        std::ostringstream errstrm;
        errstrm << "SQL query to get CREATIONID failed.";
        LOG_ERROR_MSG(errstrm.str());
        reply << FAIL << errstrm.str() << DONE;
        return;
    }
    getBase()->_createId = createId;

    // reply is already set by create_block
    createTimer->dismiss( false );
    return;
}

// extract BGQMachineXML configuration information from the database
void
DBBlockController::initMachineConfig(MMCSCommandReply& reply)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_INFO_MSG(__FUNCTION__);
    const PerformanceCounters::Timer::Ptr timer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "load machine XML" )
        ;
    timer->dismiss();

    // Create a new BGQMachineXML object from the database
    stringstream machineStream;
    if (BGQDB::getMachineXML(machineStream) != BGQDB::OK)
    {
        reply << FAIL << "can't load machine XML " << DONE;
        return;
    }
    // Create the BGQMachine classes from the xml
    if (getBase()->_machineXML != NULL)
    {
        delete getBase()->_machineXML;
        getBase()->_machineXML = NULL;
    }

    try
    {
        getBase()->_machineXML = BGQMachineXML::create(machineStream);
    }
    catch (XMLException& e)
    {
        reply << FAIL << e.what() << DONE;;
        return;
    }
    catch (BGQNodeConfigException& e)
    {
        reply << FAIL << e.what() << DONE;
        return;
    }
    if (getBase()->_machineXML == NULL)
    {
        reply << FAIL << "can't create from machine XML " << DONE;
        return;
    }
    timer->dismiss( false );
}

void DBBlockController::sendPending() {
    std::map<BGQDB::job::Id,int> new_kill_fails;
    for(std::map<BGQDB::job::Id,int>::const_iterator
            it = _pending_kills.begin(); it != _pending_kills.end(); ++it) {
        if(RunJobConnection::Kill(it->first, it->second) != 0)
            new_kill_fails[it->first] = it->second;
    }
    _pending_kills.clear();
    _pending_kills = new_kill_fails;
}

void
DBBlockController::boot_block(deque<string> args, MMCSCommandReply& reply)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);

    const PerformanceCounters::Timer::Ptr bootTimer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "boot block" )
        ;
    bootTimer->dismiss();

    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    bool useDb = false;	// use boot images and options from the bgqblock table
    deque<string> arg_uloader, arg_bootoptions, arg_domains, arg_steps;
    string db_uloader, db_domains;
    deque<string> boot_args;	// options to pass to BlockController::boot_block
    bool booted = false;
    BOOST_SCOPE_EXIT( ( &_allocate_block_start ) ( & booted) ) {
        if(!booted)
            _allocate_block_start = 0;
    } BOOST_SCOPE_EXIT_END;


    // check for block in proper state
    if ((result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo)) != BGQDB::OK)
    {
        reply << FAIL << strDBError(result) << ": unable to retrieve block information" << DONE;
        return;
    }

    if ((!strcmp(bInfo.status, BGQDB::BLOCK_ALLOCATED) == 0) &&
        (!strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) == 0))  // BOOTING is OK if we're doing stepwise.
    {
        if (strcmp(bInfo.status, BGQDB::BLOCK_DEALLOCATING) == 0) // this can occur if bgqblock record status is set to
        {		                                   // 'D' directly, such as by runjob timeout
            std::string blockErrorText = "";
            result = BGQDB::getBlockErrorText(getBase()->_blockName, blockErrorText);
            if (result != BGQDB::OK || blockErrorText[0] == '\0')
                getBase()->setDisconnecting(true, "block has been freed by RAS event or runjob");
            else
                getBase()->setDisconnecting(true, blockErrorText);
            LOG_ERROR_MSG("DBBlockController::boot(" << getBase()->_blockName << ") disconnecting: " << getBase()->disconnectReason());
            reply << FAIL << getBase()->disconnectReason() << DONE;
        }
        else
            reply << FAIL << "invalid block state: " << bInfo.status << DONE;
        return;
    }


    // parse the boot_block arguments
    time(&_allocate_block_start);
    bool updating = false;
    if (args.size() == 0) // if no arguments are specified, use the load images and boot options
        useDb = true;	  // from the bgqblock table
    else {
        bool gotsteps = false;
        for (unsigned i = 0; i < args.size(); ++i)
            {
                if (args[i] == "update") { useDb = true; updating = true; continue; }
                // Need to support domain syntax instead
                // domain={cores=0-1 memory=0-3fff images=/path/to/image,/path/to/another/image id=0}:
                if (args[i].compare(0,8,"uloader=") == 0) { arg_uloader.push_back(args[i]); continue; }
                if (args[i].compare(0,6,"steps=") == 0) {
                    if(gotsteps) {
                        reply << FAIL << "Only one comma separated list of steps permitted" << DONE;
                        return;
                    }
                    gotsteps = true;
                    arg_steps.push_back(args[i]);
                    continue;
                }
                if (args[i].compare(0,7,"domain=")  == 0) {
                    std::string domainstr;

                    // Reconstitute the domain string.
                    // Loop through the args and append until we get to a '}'
                    unsigned int argindex = i;
                    for(std::string curstring = args[i];
                        argindex < args.size();
                        ++argindex) {
                        curstring = args[argindex];
                        if(curstring.find("cores=") != std::string::npos) {
                            curstring.replace(curstring.find('-'), 1, 1, '$');
                        }
                        if(curstring.find("memory=") != std::string::npos) {
                            curstring.replace(curstring.find('-'), 1, 1, '$');
                        }

                        curstring += " ";
                        domainstr += curstring;
                        if(curstring.find('}') != std::string::npos)
                            break;
                        ++i;
                    }

                    // Validate the content
                    if(domainstr.find("id=") == std::string::npos) {
                        reply << FAIL << "Cannot set domain info without an id specified" << DONE;
                        return;
                    }

                    if(domainstr.find("cores=") != std::string::npos && useDb) {
                        reply << FAIL << "Cannot change core config on an update" << DONE;
                        return;
                    }

                    if(domainstr.find("memory=") != std::string::npos && useDb) {
                        reply << FAIL << "Cannot change memory config on an update" << DONE;
                        return;
                    }

                    if(domainstr.find("custaddr=") != std::string::npos && useDb) {
                        reply << FAIL << "Cannot change customization address on an update" << DONE;
                        return;
                    }

                    if(domainstr.find("options=") != std::string::npos && useDb) {
                        reply << FAIL << "Cannot change domain options on an update" << DONE;
                        return;
                    }

                    arg_domains.push_back(domainstr); continue;
                }
                else {
                    arg_bootoptions.push_back(args[i]);
                }
            }
    }

    // get the boot images and options from the bgqblock table
    //if (strlen(bInfo.ioloadImg) > 0)
    //    db_ioload.assign("ioload=").append(bInfo.ioloadImg);
    //    if (strlen(bInfo.nodeImg) > 0)
    // Need to transform the bInfo domain stuff to domain syntax to pass as args.
    if(bInfo.domains.size() > 0) {
        // We've got domains
        std::ostringstream dmnstrm;
        bool foundId = false;
        for(std::vector<BGQDB::DomainInfo>::iterator it = bInfo.domains.begin();
            it != bInfo.domains.end(); ++it) {
            dmnstrm << "domain={";


            if(it->endCore) {
                // We have a core range.  We use an end core
                // because a zero start core is valid
                // Don't use '-' as a separator so we don't confuse negative numbers
                dmnstrm << "cores=" << it->startCore << "$" << it->endCore << " ";
            }
            if(it->endAddr) {
                // Address range specified
                dmnstrm << "memory=" << it->startAddr << "$" << it->endAddr << " ";
            }

            std::string images = it->imageSet;

            // If we are using the DB entries and boot_block update, then
            // we overwrite the images field from the args to the matching
            // DB domain.
            if(useDb && updating) {
                // loop through arg domains to see if this domain's id
                // is specified.
                bool found = false;
                for(std::deque<std::string>::iterator arg_it = arg_domains.begin();
                    arg_it != arg_domains.end(); ++arg_it) {
                    if(arg_it->find(it->id) != std::string::npos) {
                        // Found the id.  Overwrite the image picked up above.
                        found = true;
                        std::string workstring = *arg_it;
                        boost::char_separator<char> space_sep(" ");
                        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                        tokenizer fields(workstring, space_sep);
                        workstring.erase(workstring.begin(), workstring.begin() + workstring.find("{") + 1);
                        BOOST_FOREACH(std::string field, fields) {
                            if(field.find("images=") != std::string::npos) {
                                field.erase(field.begin(),field.begin() + field.find("images=") + 7);
                                images = field;
                            }
                        }
                    }
                }
                if(!found) {
                    reply << FAIL << "Invalid domain ID specified on update." << DONE;
                    return;
                }
            }

            dmnstrm << "custaddr=" << it->custAddr << " ";

            if(images.length() != 0) {
                dmnstrm << "images=" << images << " ";
            }

            std::string options = it->options;
            if(options.length() != 0) {
                dmnstrm << "options=" << options << " ";
            }
            std::string id = it->id;
            if(id.length() != 0) {
                dmnstrm << "id=" << id << " ";
                foundId = true;
            }
            dmnstrm << "} ";
            db_domains += dmnstrm.str();
        }
        if(foundId == false) {
            reply << FAIL << "Domain id to update not specified" << DONE;
            return;
        }
    } else {
        // No domains?
    }

    if (strlen(bInfo.uloaderImg) > 0)
        db_uloader.assign("uloader=").append(bInfo.uloaderImg);

    // merging boot_block images and options with those from the database
    if (useDb == true)
    {

        string bootOptionsProps = MMCSProperties::getProperty(MMCS_BOOT_OPTIONS);

        if (!arg_uloader.empty())
            boot_args.insert(boot_args.end(),arg_uloader.begin(),arg_uloader.end());
        else
            boot_args.push_back(db_uloader);

        trim_right_spaces(bInfo.bootOptions);
        if (strlen(bInfo.bootOptions) > 0)
            boot_args.push_back(string(bInfo.bootOptions));
        else
            if (!bootOptionsProps.empty())
                boot_args.push_back(bootOptionsProps);

        if (!arg_bootoptions.empty())
            boot_args.insert(boot_args.end(),arg_bootoptions.begin(),arg_bootoptions.end());

        // We already did the merge when we parsed through the block info
        boot_args.push_back(db_domains);
    }

    // not merging boot_block images and options with those from the database
    else
    {
        if (!arg_uloader.empty())
            boot_args.insert(boot_args.end(),arg_uloader.begin(),arg_uloader.end());
        else {
            reply << FAIL << "uloader not specified" << DONE;
            return;
        }
        if (!arg_bootoptions.empty())
            boot_args.insert(boot_args.end(),arg_bootoptions.begin(),arg_bootoptions.end());
        if (!arg_domains.empty()) {
            boot_args.insert(boot_args.end(),arg_domains.begin(),arg_domains.end());
        }
        if(!arg_steps.empty()) {
            boot_args.insert(boot_args.end(), arg_steps.begin(), arg_steps.end());
        }
    }

    std::ostringstream logstream;
    logstream << "boot_args: ";
    for(std::deque<string>::iterator it = boot_args.begin(); it != boot_args.end();
        ++it) {
        logstream << *it << " | ";
    }

    LOG_DEBUG_MSG(logstream.str());

    if(getBase()->isIOBlock() == false && getBase()->_diags == false) { // If we're a diags block, we don't care about I/O rules
        // If we're a compute block, we need to make sure that connected I/O nodes are booted.
        // To support I/O blocks with some I/O nodes in "Error" a check is needed to make sure each midplane
        // has at least a minimum number of I/O links going to a booted I/O node. For small blocks at least one I/O link must be
        // "available" and have a connected (booted) I/O node. For large blocks each midplane must have at least two "available" I/O
        // links with connected (booted) I/O nodes. The minimum I/O links is based on the Mundy rule which requires a minimum of a
        // 1:256 compute to I/O ratio. The I/O rules are checked by using the BGQDB::checkBlockIO() method which examines the
        // machine for "connected" I/O nodes and "available" I/O links.

        std::vector<string> unconnectedIONodes;
        std::vector<string> midplanesFailingIORules;
        std::vector<string> unconnectedAvailableIONodes;
        if((result = BGQDB::checkBlockIO(getBase()->_blockName, &unconnectedIONodes, &midplanesFailingIORules, &unconnectedAvailableIONodes)) != BGQDB::OK) {
            LOG_ERROR_MSG("DBBlockController::boot(): checkBlockIO(" << getBase()->_blockName << ") failed, result=" << result);
            reply << FAIL << strDBError(result) << ": unable to check block I/O" << DONE;
            return;
        }

        if(unconnectedAvailableIONodes.size() > 0) {
            std::ostringstream failmsg;
            failmsg << "Cannot boot block " << getBase()->_blockName << ". The following I/O nodes in the block are not booted: ";
            for(std::vector<string>::iterator it = unconnectedAvailableIONodes.begin(); it != unconnectedAvailableIONodes.end(); ++it) {
                failmsg << *it << " ";
            }
            LOG_ERROR_MSG(failmsg.str());
            reply << FAIL << failmsg.str() << DONE;
            return;
        }

        if(midplanesFailingIORules.size() > 0) {
            std::ostringstream failmsg;
            failmsg << "Cannot boot block " << getBase()->_blockName << ". The following midplanes in the block do not have the minimum connected I/O nodes: ";
            for(std::vector<string>::iterator it = midplanesFailingIORules.begin(); it != midplanesFailingIORules.end(); ++it) {
                failmsg << *it << " ";
            }
            LOG_ERROR_MSG(failmsg.str());
            reply << FAIL << failmsg.str() << DONE;
            return;
        }

        // If we are not already booting due to step-wise boots, enforce
        // the Revised Mundy rule.
        if(!strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) == 0) {
            BGQMidplaneNodeConfig::ComputeCount cc;
            getBase()->_block->getBridgeCount(cc);
            //            BGQDB::ioUsage::init();
            try {
                BGQDB::ioUsage::update(cc, getBase()->_blockName);
            } catch(BGQDB::ioUsage::UsageExceeded& e) {
                // Generate a Mundy RAS.
                RasEventImpl event(MMCSOps_000D);
                event.setDetail(RasEvent::BLOCKID, getBase()->_blockName);
                RasEventHandlerChain::handle( event );
                event.setDetail(RasEvent::MESSAGE, e.what());
                event.setDetail("NODE", e.getIoNode());
                event.setDetail("COUNT", boost::lexical_cast<std::string>(e.getCount()));
                event.setDetail("LIMIT", boost::lexical_cast<std::string>(BGQDB::ioUsage::getUsageLimit()));
                BGQDB::putRAS( event, getBase()->_blockName, 0 /* job ID */, getBase()->_bootCookie );

                LOG_WARN_MSG("I/O usage failure: " << e.what());
                reply << FAIL << e.what() << DONE;
                return;
            }
        }
    }

    // set block state to booting
    if(!strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) == 0) {
        // If we're not yet booting (because we're going stepwise), set the state.
        deque<string> options;
        options.push_back(std::string("qualifier=") +  boost::lexical_cast<std::string>(getBase()->_bootCookie));
        if ((result = setBlockStatus(getBase()->_blockName, BGQDB::BOOTING, options)) != BGQDB::OK)
            {
                LOG_ERROR_MSG("DBBlockController::boot(): setBlockStatus(" << getBase()->_blockName << ", BOOTING) failed, result=" << result << ", current block state=" << bInfo.status);
                reply << FAIL << strDBError(result) << ": unable to set block state to BOOTING" << DONE;
                return;
            } else
            LOG_INFO_MSG("boot_block(): setBlockStatus(" << getBase()->_blockName << ", BOOTING) success");
    }
    // boot the block

    std::map<std::string,std::string> nodeCust;
    BGQDB::getCustomization(getBase()->_blockName, nodeCust);

    // If it's a full system block, bypass environmentals during boot.
    bool full_system = false;
    if(getBase()->_machineXML->_midplanes.size() == getBase()->_blockXML->_midplanes.size()) {
        full_system = true;
    }

    BOOST_SCOPE_EXIT( ( &_allocate_block_start ) ( &full_system) ) {
        if(full_system)
            MMCSProperties::setProperty("bypass_envs", "false");
    } BOOST_SCOPE_EXIT_END;

    if(full_system) {
        MMCSProperties::setProperty("bypass_envs", "true");
    }

    getBase()->boot_block(boot_args, reply, nodeCust, bootTimer);
    booted = true;
    if (reply.getStatus() != 0)
    {
        // update the block error text
        BGQDB::setBlockErrorText(getBase()->_blockName, reply.str());

        int result = setBlockStatus(getBase()->_blockName, BGQDB::TERMINATING);
        if(result != BGQDB::OK) {
            LOG_WARN_MSG("setBlockStatus(" << getBase()->_blockName << ", TERMINATING) failed (set error text), error " << result);
        }
        else
            LOG_INFO_MSG("setBlockStatus(" << getBase()->_blockName << ", TERMINATING) succeeded (set error text).");

        getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox

        RasEventImpl event(MMCSOps_000A);
        event.setDetail(RasEvent::BLOCKID, getBase()->_blockName);
        RasEventHandlerChain::handle( event );
        event.setDetail( RasEvent::MESSAGE, reply.str() );
        BGQDB::putRAS( event, getBase()->_blockName, 0 /* job ID */ );

        return;
    }

    if(MMCSProperties::getProperty(BOOT_COMPLETE) == "false") {
        setBlockStatus(getBase()->_blockName, BGQDB::INITIALIZED);
    } else {
        if(getBase()->doCheckCompete()) {
            // Start waiting for the boot to complete
            _cbc.start();
        }
    }

    bootTimer->dismiss( false );

    reply << OK << DONE;
}

void
DBBlockController::waitBoot(deque<string> cmdargs, MMCSCommandReply& reply, bool interactive, bool rebooting)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::STATUS result = BGQDB::OK;
    BGQDB::BlockInfo bInfo;
    float boot_slope = 0;


    if(getBase()->isIOBlock() == false)
        boot_slope = atof(MMCSProperties::getProperty(CN_BOOT_SLOPE).c_str());
    else
        boot_slope = atof(MMCSProperties::getProperty(IO_BOOT_SLOPE).c_str());

    double wait_time = atoi(MMCSProperties::getProperty(MINIMUM_BOOT_WAIT).c_str()); // Minimum wait time in seconds
    int override_timeout = atoi(MMCSProperties::getProperty(WAIT_BOOT_FREE_TIME).c_str()) * 60;  // in seconds

    if (cmdargs.size() > 0)
        {
            // check that timeout value is valid
            wait_time = strtol(cmdargs[0].c_str(),NULL,10) * 60;
            if (wait_time <= 0)
                {
                    reply << FAIL << "args?" << DONE;
                    return;
                }
        }
    else {
        LOG_TRACE_MSG("nnt=" << getBase()->_numNodesTotal << " slope=" << boot_slope << " wt=" << wait_time);
        wait_time = getBase()->_numNodesTotal * boot_slope + wait_time;
        (override_timeout > wait_time)?wait_time = wait_time:wait_time = override_timeout;
    }

    LOG_INFO_MSG("DBBlockController::waitBoot(" << getBase()->_blockName << "," << wait_time << ")");

    // wait for the nodes to complete initialization
    time_t current_time;
    time(&current_time);
    time_t boot_start = current_time;
    if(!interactive)  // Interactive means somebody typed "wait_boot" in a console.
        boot_start = getAllocateStartTime();
    for(;(current_time - boot_start) < wait_time; time(&current_time)) {
        if (getBase()->isDisconnecting())
            break;
        if(rebooting) {
            if(getBase()->_rebooted) {
                break;
            } else {
                sleep(5);
                continue;
            }
        } else {
            result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo);
            if (result == BGQDB::OK)
                if (strcmp(bInfo.status,BGQDB::BLOCK_BOOTING) == 0)
                    { // wait for allocation to complete
                        sleep(5);
                        //                    wait_time -= 5;
                        continue;
                    }
                else  // successful allocation
                    break;
            else
                break;
        }
    }

    // check boot completion status
    if (getBase()->isDisconnecting())                            // this can occur for a KERNEL_FATAL ras event or
        {                                                 // deallocate by another console thread
            reply << FAIL << getBase()->disconnectReason() << DONE;
            return;
        }

    if(rebooting) {
        if(getBase()->_rebooted) {
            LOG_INFO_MSG("DBBlockController::waitBoot(" << getBase()->_blockName << ") node reboot complete");
            getBase()->setRebooting(false);
            reply << OK << DONE;
            return;
        }
    } else if ((result == BGQDB::OK && strcmp(bInfo.status,BGQDB::BLOCK_INITIALIZED) == 0)) {
        LOG_INFO_MSG("DBBlockController::waitBoot(" << getBase()->_blockName << ") block initialization successful");
        reply << OK << DONE;
        return;
    }

    // If we get here, we failed.  Now figure out why.
    if ((wait_time <= 0 && override_timeout == -1) || (getBase()->getRebooting()))
        {

            reply << FAIL << "timeout from node(s): ";

            for (vector<BCNodeInfo*>::iterator iter = getBase()->getNodes().begin();iter != getBase()->getNodes().end(); ++iter) 	{

                if ((*iter)->_initialized == false) {
                    LOG_INFO_MSG("DBBlockController::waitBoot(" << getBase()->_blockName << ") block initialization unsuccessful, "
                                 << (*iter)->location() << " not initialized");
                    reply << (*iter)->location() << " ";
                }
            }
            reply <<  " block left in Booting state" << DONE;
            return;
        }
    else
        {
            string errmsg = "block initialization timed out";
            std::string blockErrorText;
            result = BGQDB::getBlockErrorText(getBase()->_blockName, blockErrorText);
            if (result == BGQDB::OK && blockErrorText[0] != '\0')
                errmsg.append(": ").append(blockErrorText);
            int result = setBlockStatus(getBase()->_blockName, BGQDB::TERMINATING);
            if(result != BGQDB::OK) {
                LOG_WARN_MSG("setBlockStatus(" << getBase()->_blockName << ", TERMINATING) failed (getErrorText), error " << result);
            }
            else
                LOG_INFO_MSG("setBlockStatus(" << getBase()->_blockName << ", TERMINATING) succeeded (getErrorText).");

            getBase()->setDisconnecting(true, errmsg);
            reply << FAIL << "boot timeout from node(s): ";

            unsigned bad_count = 0;
            std::string firstFailed;
            for(std::map<std::string, BCTargetInfo*>::iterator iter = getBase()->_targetLocationMap.begin();
                iter != getBase()->_targetLocationMap.end(); ++iter) {
                BCNodeInfo* node = dynamic_cast<BCNodeInfo*>(iter->second);
                if(node) {
                    if(node->_initialized == false) {
                        if(bad_count == 0) {
                            LOG_ERROR_MSG("waitBoot(" << getBase()->_blockName << ") block initialization failed, "
                                          << iter->first << " not initialized");
                            reply << iter->first;
                            firstFailed = iter->first;
                        }
                        else if(bad_count <= 64) { // Only print first 64 bad nodes.
                            reply << "," << iter->first;
                        }
                        ++bad_count;
                    }
                }
            }

            if(bad_count > 64) {
                reply << " and " << bad_count - 64 << " others";
            }

            reply << ".  block freed" << DONE;
            ostringstream blockErrText;
            blockErrText << "wait_boot for " << getBase()->_blockName << " failed. Node "
                         << firstFailed << " timed out.";
            BGQDB::setBlockErrorText(getBase()->_blockName, blockErrText.str());

            RasEventImpl boot_fail(MMCSOps_000A);
            boot_fail.setDetail(RasEvent::BLOCKID, getBase()->_blockName);
            RasEventHandlerChain::handle(boot_fail);
            boot_fail.setDetail( RasEvent::MESSAGE, blockErrText.str() );
            BGQDB::putRAS(boot_fail, getBase()->_blockName, 0);

            return;
        }
}

void
DBBlockController::freeBlock(deque<string> args, MMCSCommandReply& reply)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    boost::mutex::scoped_lock scope_free(getBase()->_init_free_lock);

    PerformanceCounters::Timer::Ptr timer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "shutdown" )
        ->subFunction( "free block" )
        ;
    timer->dismiss();

    time_t bst = getBase()->getBootStartTime();
    if(bst) { // We're waiting on a boot
        struct tm tod_tm;		// output from localtime
        char time_str[16];		// output from ctime
        ostringstream pfx;		// log entry prefix

        pfx.str("");		// clear the work buffer

        localtime_r(const_cast<const time_t*>(&bst), &tod_tm);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", &tod_tm);
        pfx << time_str;
        reply << FAIL << "Block has been booting since " << pfx.str() << DONE;
        return;
    }

    //
    // ** Start of critical section
    // ** allow only one thread at a time for each individual block
    //
    PthreadMutexHolder mutex;	// 'smart' mutex holder: destructor will release the mutex
    int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    if(getBase()->isIOBlock() == true && getBase()->_diags == false) {
        std::vector<std::string> compute_blocks;
        result = BGQDB::checkIOBlockConnection(getBase()->_blockName, &compute_blocks);
        if(result != BGQDB::OK) {
            reply << FAIL << strDBError(result)
                  << ": Database cannot determine connected compute blocks." << DONE;
            return;
        }

        if(compute_blocks.size() != 0) {
            // For each compute block, we have to find it in the map and see if it
            // is diags.
            std::vector<std::string> diags_blocks;
            for(std::vector<std::string>::iterator it = compute_blocks.begin();
                it != compute_blocks.end(); ++it) {
                const DBBlockPtr blockp = DBConsoleController::findBlock(*it);
                if(blockp) {
                    // The find incremented the block thread count so we have to decrement it again.
                    blockp->getBase()->decrBlockThreads();
                    if(blockp->getBase()->_diags) {
                        diags_blocks.push_back(*it);
                    }
                }
            }

            BOOST_FOREACH(std::string& curr_block, diags_blocks){
                compute_blocks.erase(std::remove(compute_blocks.begin(), compute_blocks.end(), curr_block));
            }

            // This will have changed if all connected computes are diags blocks.
            if(compute_blocks.size() != 0) {
                reply << FAIL << "Connected compute block" << (compute_blocks.size() == 1 ? "" : "s") << " ";
                for(std::vector<std::string>::iterator it = compute_blocks.begin();
                    it != compute_blocks.end(); ++it) {
                    if(it != compute_blocks.begin())
                        reply << ", ";
                    reply << *it;
                }
                reply << " " << (compute_blocks.size() == 1 ? "is" : "are") << " still booted. "
                      << (compute_blocks.size() == 1 ? "It" : "They") << " must be freed before IO block "
                      << getBase()->_blockName << " can be freed." << DONE;
                return;
            }
        }
    }

    // get the current block state
    result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo);
    if (result != BGQDB::OK)
    {
        if (result == BGQDB::INVALID_ID)
            reply << FAIL << strDBError(result) << ": invalid block id" << DONE;
        else
            reply << FAIL << strDBError(result) << ": DBBlockController::freeBlock() getBlockInfo failed" << DONE;
        return;
    }

    // if the block is already free, return 'OK'
    if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0)
    {
        reply << OK << DONE;
        return;
    }

    // change the block state
    result = setBlockStatus(getBase()->_blockName, BGQDB::TERMINATING);
    if (result != BGQDB::OK)	// setBlockStatus failed
    {
        mutex.Unlock();

        // log an error
        LOG_ERROR_MSG(strDBError(result) << ": unable to set block state to TERMINATING for block " << getBase()->getBlockName() << ", current block state=" << bInfo.status);

        // return an error
        if (strcmp(bInfo.status, BGQDB::BLOCK_TERMINATING) != 0) {
            if (result == BGQDB::FAILED)
                reply << FAIL << strDBError(result) << ": free block failed;is there a running job associated with the block?" << DONE;
            else
                reply << FAIL << strDBError(result) << ": free block failed" << DONE;
            return;
        }
    }

    LOG_INFO_MSG("freeBlock() setBlockStatus(" << getBase()->_blockName << ", TERMINATING) successful");

    // done with the mmcs_sever portion of freeing a block
    // the free timer needs to be reset before it can be inserted into the tbgqcomponentperf table
    timer->dismiss( false );
    timer.reset();
    _counters.output( getBase()->_bootCookie );

    mutex_rc = mutex.Unlock();
    assert(mutex_rc == 0);
    //
    // ** end of critical section
    //
    if (!getBase()->_diags && getBase()->isStarted()) {
        getBase()->shutdown_block(reply, 0, false);
        if(reply.getStatus() != MMCSCommandReply::STATUS_OK && getBase()->_block_shut_down == false) {
            // Failed!
            result = setBlockStatus(getBase()->_blockName, BGQDB::ALLOCATED);
            if (result != BGQDB::OK) {	// setBlockStatus failed
                LOG_ERROR_MSG("Could not set block status to ALLOCATED");
            } else {
                LOG_INFO_MSG("freeBlock setBlockStatus to ALLOCATED");
            }
            return;
        }
    }

    // set the DBBlockController to disconnecting
    std::string blockErrorText;
    result = BGQDB::getBlockErrorText(getBase()->_blockName, blockErrorText);
    if (result != BGQDB::OK || blockErrorText[0] == '\0')
        getBase()->setDisconnecting(true, "block has been freed"); // reset icon connections and stop mailbox
    else
        getBase()->setDisconnecting(true, blockErrorText);

    // Make sure redirection ends.
    getBase()->resetRedirection();
    for (vector<BCNodeInfo*>::iterator iter = getBase()->getNodes().begin();
         iter != getBase()->getNodes().end(); ++iter) {
        (*iter)->reset_state();	// reset the node state
    }

    reply << OK << DONE;
}

void
DBBlockController::disconnect(deque<string>& args)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;

    //
    // ** Start of critical section
    // ** allow only one thread at a time for each individual block
    //
    PthreadMutexHolder mutex;	// 'smart' mutex holder: destructor will release the mutex
    int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    LOG_INFO_MSG("disconnecting block");

    getBase()->disconnect(args);

    BGQDB::BLOCK_STATUS blockState = BGQDB::INVALID_STATE;
    BGQDB::getBlockStatus(getBase()->_blockName, blockState);
    if (blockState != BGQDB::TERMINATING) {
        result = setBlockStatus(getBase()->_blockName, BGQDB::TERMINATING);
        if (result != BGQDB::OK) {
            if (BGQDB::getBlockInfo(getBase()->_blockName, bInfo) == BGQDB::OK) {
                LOG_WARN_MSG("DBBlockController::disconnect() setBlockStatus(" << getBase()->_blockName << ", TERMINATING) failed, result=" << result << ", current block state=" << bInfo.status);
                BlockHelperPtr dbbc = DBConsoleController::findBlock(getBase()->_blockName);
                if(dbbc) {
                    LOG_INFO_MSG("Block " << getBase()->_blockName << " found.");
                    // The find incremented the block thread count so we have to decrement it again.
                    dbbc->getBase()->decrBlockThreads();
                } else
                    LOG_INFO_MSG("Block " << getBase()->_blockName << " not found.");

            } else {
                LOG_WARN_MSG("DBBlockController::disconnect() setBlockStatus(" << getBase()->_blockName << ", TERMINATING) failed, result=" << result);
            }
        } else {
            LOG_WARN_MSG("DBBlockController::disconnect() setBlockStatus(" << getBase()->_blockName << ", TERMINATING) successful");
        }
    }

    result = setBlockStatus(getBase()->_blockName, BGQDB::ALLOCATED);
    if (result != BGQDB::OK) {
        if (BGQDB::getBlockInfo(getBase()->_blockName, bInfo) == BGQDB::OK) {
            LOG_WARN_MSG("DBBlockController::disconnect() setBlockStatus(" << getBase()->_blockName << ", ALLOCATED) failed, result=" << result << ", current block state=" << bInfo.status);
        } else {
            LOG_WARN_MSG("DBBlockController::disconnect() setBlockStatus(" << getBase()->_blockName << ", ALLOCATED) failed, result=" << result);
        }
    } else {
        LOG_INFO_MSG("DBBlockController::disconnect() setBlockStatus(" << getBase()->_blockName << ", ALLOCATED) successful");
    }

    mutex.Unlock();
    //
    // ** end of critical section
    //
    //
}

void
DBBlockController::processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    getBase()->processConsoleMessage(consoleMessage);
}

void
DBBlockController::postProcessRASMessage(int recid) {
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    std::vector<BGQDB::job::Id> jobsToKill;
    const int signal = bgcios::jobctl::SIGHARDWAREFAILURE;
    BGQDB::SparingInfo sInfo;

    BGQDB::STATUS recstat = BGQDB::postProcessRAS(recid, jobsToKill, sInfo);
    if(recstat != BGQDB::OK) {
        LOG_ERROR_MSG("Could not post-process ras id " << recid << ".  Error " << recstat);
    }
    if (jobsToKill.size() > 0) {

        // First, check to see if we ever connected to runjob_server
        bool connected = RunJobConnection::isConnected();
        if(!connected) {  // If we're not connected, try to reconnect and drain pending.
            LOG_ERROR_MSG("runjob_server was not connected and cannot be reached, job cannot be killed.");
            for (unsigned i = 0; i < jobsToKill.size(); ++i) {
                RasEventImpl event(MMCSOps_000C);
                event.setDetail(RasEvent::SEVERITY, "WARN");
                event.setDetail("JOB", boost::lexical_cast<std::string>(jobsToKill[i]));
                BGQDB::putRAS(event, getBase()->_blockName, 0);
                _pending_kills[jobsToKill[i]] = signal;
            }
            return;
        }

        for (unsigned i = 0; i < jobsToKill.size(); ++i) {
            LOG_DEBUG_MSG("Killing job " << jobsToKill[i] << " due to RAS event " << recid);

            if(RunJobConnection::Kill(jobsToKill[i], signal, recid) != 0) {
                // Failed to send.  Add it to the map so we can send it when
                // the runjob connection comes back.
                _pending_kills[jobsToKill[i]] = signal;
            }
        }
    }

    if (sInfo.wireMask != 0) {
        // call the mcserver function for BQL sparing
        MCServerMessageSpec::DynamicSparingRequest::LinkChipSpareXmtr fromlink(sInfo.txLoc,
									       sInfo.txReg,
									       sInfo.txMask);
        MCServerMessageSpec::DynamicSparingRequest::LinkChipSpareRcvr tolink(sInfo.rxLoc,
									     sInfo.rxReg,
									     sInfo.rxMask);
        MCServerMessageSpec::DynamicSparingRequest sparingreq(fromlink, tolink);
        MCServerMessageSpec::DynamicSparingReply sparingrep;
        try {
            MCServerRef* ref;
            MMCSCommandReply reply;
            BlockControllerBase::mcserver_connect(ref, getBase()->_userName, reply);
            boost::scoped_ptr<MCServerRef> mc_server(ref);
            if (reply.getStatus()) {
                LOG_ERROR_MSG( reply.str() );
                return;
            }
            LOG_DEBUG_MSG("connected to mcServer");

            ref->dynamicSparing(sparingreq, sparingrep);
        } catch (const std::exception &e) {
            LOG_ERROR_MSG(sparingrep._rc << " " << sparingrep._rt << " " << e.what());
        }
    }
}

int
DBBlockController::processRASMessage(RasEvent& rasEvent)
{
    LOGGING_DECLARE_DBBLOCK_ID_MDC; LOGGING_DECLARE_DBBLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    bool filtered;
    bitset<30> map;

    SQLRETURN sqlrc;
    char sqlstr[512];

    // skip barrier RAS events since they are never inserted
    if (rasEvent.msgId() != 0x00040096) {
        // augment RAS event with missing info, if needed, such as ECID and Serial Number
        BGQDB::STATUS result = BGQDB::augmentRAS(rasEvent);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG("processRASMessage failed to augment RAS with ECID and SN, error: " << result);
        }
    }

    // let BlockController do its part
    if ((filtered = getBase()->processRASMessage(rasEvent)) == false)
    {
        const boost::posix_time::ptime start( boost::posix_time::microsec_clock::local_time() );

        // RAS "fast-path" for all except diagnostics-generated RAS events
        if (!getBase()->_diags) {
            if (getBase()->_blockName == "DefaultControlEventListener") {
                LOG_INFO_MSG("Received RAS event: " << rasEvent);
            }
            if (_sqlstmt == 0) {  // only need to do this once
                int pos = 0;
                _tx.reset(new BGQDB::TxObject(BGQDB::DBConnectionPool::Instance()));
                _tx->alloc(&_sqlstmt);
                map.reset();
                map.set(dbe.MSG_ID);
                map.set(dbe.CATEGORY);
                map.set(dbe.COMPONENT);
                map.set(dbe.SEVERITY);
                map.set(dbe.EVENT_TIME);
                map.set(dbe.JOBID);
                map.set(dbe.BLOCK);
                map.set(dbe.LOCATION);
                map.set(dbe.SERIALNUMBER);
                map.set(dbe.ECID);
                map.set(dbe.CPU);
                map.set(dbe.COUNT);
                map.set(dbe.CTLACTION);
                map.set(dbe.MESSAGE);
                map.set(dbe.RAWDATA);
                map.set(dbe.QUALIFIER);
                dbe._columns = map.to_ulong();
                strcpy(sqlstr, "select recid from NEW TABLE ( ");
                strcat(sqlstr, dbe.getInsertStatement().c_str());
                strcat(sqlstr, " ) ");
                sqlrc = SQLPrepare(_sqlstmt, (SQLCHAR *)sqlstr, SQL_NTS);

                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 8, 0, dbe._msg_id, sizeof(dbe._msg_id) , &dbe._ind[dbe.MSG_ID]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 16, 0, dbe._category, sizeof(dbe._category) , &dbe._ind[dbe.CATEGORY]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 16, 0, dbe._component, sizeof(dbe._component) , &dbe._ind[dbe.COMPONENT]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 8, 0, dbe._severity, sizeof(dbe._severity) , &dbe._ind[dbe.SEVERITY]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 0, 0, dbe._event_time, sizeof(dbe._event_time) , &dbe._ind[dbe.EVENT_TIME]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_SBIGINT, SQL_BIGINT, 0, 0, &dbe._jobid, 0,  &dbe._ind[dbe.JOBID]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 32, 0, dbe._block, sizeof(dbe._block) , &dbe._ind[dbe.BLOCK]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 64, 0, dbe._location, sizeof(dbe._location) , &dbe._ind[dbe.LOCATION]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 19, 0, dbe._serialnumber, sizeof(dbe._serialnumber) , &dbe._ind[dbe.SERIALNUMBER]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BINARY, 32, 0, dbe._ecid, 32, &dbe._ind[dbe.ECID]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dbe._cpu, 0,  &dbe._ind[dbe.CPU]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &dbe._count, 0,  &dbe._ind[dbe.COUNT]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 256, 0, dbe._ctlaction, sizeof(dbe._ctlaction) , &dbe._ind[dbe.CTLACTION]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 1024, 0, dbe._message, sizeof(dbe._message) , &dbe._ind[dbe.MESSAGE]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 3072, 0, dbe._rawdata, sizeof(dbe._rawdata) , &dbe._ind[dbe.RAWDATA]);
                SQLBindParameter( _sqlstmt, ++pos, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 32, 0, dbe._qualifier, sizeof(dbe._qualifier) , &dbe._ind[dbe.QUALIFIER]);
                SQLBindCol(_sqlstmt, 1, SQL_C_LONG, &dbe._recid, 4, &dbe._ind[dbe.RECID]);

                // job ID association
                SQLAllocHandle( SQL_HANDLE_STMT, _tx->getConnection()->getConnHandle(), &_sqlquery );
                SQLPrepare( _sqlquery, (SQLCHAR *)
                    "SELECT id FROM bgqjob_history "
                    "WHERE corner is null "
                    "AND entrydate >= ? "
                    "AND blockid=? "
                    "UNION ALL select id FROM bgqjob "
                    "WHERE corner is null "
                    "AND blockid=? "
                    "AND starttime <= ?"
                    ,
                    SQL_NTS);
                SQLBindParameter( _sqlquery, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 0, 0, _dbj._starttime, sizeof(_dbj._starttime), &_dbj._ind[_dbj.STARTTIME]);
                SQLBindParameter( _sqlquery, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 32, 0, _dbj._blockid, sizeof(_dbj._blockid) , &_dbj._ind[_dbj.BLOCKID]);
                SQLBindParameter( _sqlquery, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 32, 0, _dbj._blockid, sizeof(_dbj._blockid) , &_dbj._ind[_dbj.BLOCKID]);
                SQLBindParameter( _sqlquery, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_TIMESTAMP, 0, 0, _dbj._entrydate, sizeof(_dbj._entrydate), &_dbj._ind[_dbj.ENTRYDATE]);
                SQLBindCol(_sqlquery, 1, SQL_C_SBIGINT, &dbe._jobid, 8, &dbe._ind[dbe.JOBID]);
            }
            dbe._rawdata[0] = 0;  // init the rawdata to a null string
            dbe._jobid = 0;       // init the jobid
            dbe._recid = 0;

            for (int col = 0; col < dbe.N_EVENTLOG_ATTR; col++) {
                dbe._ind[col] = SQL_NULL_DATA; // default all columns to null
            }

            // obtain the time of the RAS event, instead of using the CURRENT TIMESTAMP on the insert
            time_t t =  rasEvent.time().tv_sec;
            struct tm lcl;
            char time_str[32];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d-%H.%M.%S.", localtime_r(&t, &lcl));
            snprintf(dbe._event_time,sizeof(dbe._event_time),"%s%06lu", time_str, rasEvent.time().tv_usec);
            strcpy(_dbj._starttime, dbe._event_time);
            strcpy(_dbj._entrydate, dbe._event_time);
            dbe._ind[dbe.EVENT_TIME]=SQL_NTS;
            _dbj._ind[_dbj.STARTTIME]=SQL_NTS;
            _dbj._ind[_dbj.ENTRYDATE]=SQL_NTS;

            std::string rawdata;
            for (std::map<std::string,std::string>::iterator pos = rasEvent.getDetails().begin(); pos != rasEvent.getDetails().end(); ++pos)    {
                if (pos->first == RasEvent::MSG_ID)  {
                    dbe._ind[dbe.MSG_ID]=SQL_NTS;
                    strcpy(dbe._msg_id,pos->second.c_str());
                } else
                    if (pos->first == RasEvent::CATEGORY)  {
                        dbe._ind[dbe.CATEGORY]=SQL_NTS;
                        strcpy(dbe._category,pos->second.c_str());
                    } else
                        if (pos->first == RasEvent::COMPONENT)  {
                            dbe._ind[dbe.COMPONENT]=SQL_NTS;
                            strcpy(dbe._component,pos->second.c_str());
                        } else
                            if (pos->first == RasEvent::SEVERITY)  {
                                dbe._ind[dbe.SEVERITY]=SQL_NTS;
                                strcpy(dbe._severity,pos->second.c_str());
                            } else
                                if (pos->first == RasEvent::MESSAGE)  {
                                    dbe._ind[dbe.MESSAGE]=SQL_NTS;
                                    if (pos->second.length() >= sizeof(dbe._message)) {
                                        LOG_WARN_MSG("message truncated, length " << pos->second.length() );
                                        strncpy(dbe._message,pos->second.c_str(), sizeof(dbe._message)-1);
                                        dbe._message[sizeof(dbe._message)-1] = '\0';
                                    }
                                    else
                                        strcpy(dbe._message,pos->second.c_str());
                                } else
                                    if (pos->first == RasEvent::LOCATION)  {
                                        dbe._ind[dbe.LOCATION]=SQL_NTS;
                                        strcpy(dbe._location,pos->second.c_str());
                                    } else
                                        if (pos->first == RasEvent::CONTROL_ACTION)  {
                                            dbe._ind[dbe.CTLACTION]=SQL_NTS;
                                            strcpy(dbe._ctlaction,pos->second.c_str());
                                        } else
                                            if (pos->first == RasEvent::COUNT)  {
                                                dbe._ind[dbe.COUNT]=0;
                                                dbe._count = atoi(pos->second.c_str());
                                            } else
                                                if (pos->first == RasEvent::JOBID)  {
                                                    dbe._ind[dbe.JOBID]=0;
                                                    dbe._jobid = atoi(pos->second.c_str());
                                                } else
                                                    if (pos->first == RasEvent::ECID)  {
                                                        if (pos->second.length() > 0) {
                                                            dbe._ind[dbe.ECID]=32;
                                                            hexchar2bitdata((unsigned char*)dbe._ecid,2*sizeof(dbe._ecid),pos->second.c_str());
                                                        }
                                                    } else
                                                        if (pos->first == RasEvent::CPU)  {
                                                            dbe._ind[dbe.CPU]=0;
                                                            dbe._cpu = atoi(pos->second.c_str());
                                                        } else
                                                            if (pos->first == RasEvent::BLOCKID) {
                                                                dbe._ind[dbe.BLOCK]=SQL_NTS;
                                                                _dbj._ind[_dbj.BLOCKID]=SQL_NTS;
                                                                strcpy(dbe._block,pos->second.c_str());
                                                                strcpy(_dbj._blockid,dbe._block);
                                                            } else
                                                                if (pos->first == RasEvent::SERIAL_NUMBER)  {
                                                                    dbe._ind[dbe.SERIALNUMBER]=SQL_NTS;
                                                                    strcpy(dbe._serialnumber,pos->second.c_str());
                                                                } else {
                                                                    rawdata.append( pos->first);
                                                                    rawdata.append( "=" );
                                                                    rawdata.append( pos->second );
                                                                    rawdata.append( "; " );
                                                                }
            }

            if ( !rawdata.empty() ) {
                dbe._ind[dbe.RAWDATA]=SQL_NTS;
                if ( rawdata.size() > sizeof(dbe._rawdata) ) {
                    LOG_WARN_MSG("raw data truncated, length " << rawdata.size() );
                }
                (void)strncpy(dbe._rawdata, rawdata.c_str(), sizeof(dbe._rawdata) - 1);
                dbe._rawdata[sizeof(dbe._rawdata) - 1] = '\0';
            }

            if ( NeighborInfo::MessageIds.find(rasEvent.msgId()) != NeighborInfo::MessageIds.end() ) {
                NeighborInfo neighbor( getBase(), dbe._location, dbe._rawdata );
            }

            if (dbe._ind[dbe.BLOCK] == SQL_NULL_DATA) {  // check if the block name came through on the RAS details
                // Provide the block name
                strcpy(dbe._block,getBase()->_blockName.c_str());
                strcpy(_dbj._blockid, dbe._block);
                dbe._ind[dbe.BLOCK]=SQL_NTS;
                _dbj._ind[_dbj.BLOCKID]=SQL_NTS;
            }

            // include the boot cookie so RAS events for the same boot can be grouped together
            sprintf(dbe._qualifier, "%u",getBase()->_bootCookie);
            dbe._ind[dbe.QUALIFIER]=SQL_NTS;

            // lookup the job if it was not provided in the RAS event
            if ( dbe._jobid == 0 ) {
                sqlrc = SQLExecute(_sqlquery);
                if (sqlrc != 0)
                    LOG_ERROR_MSG( __FUNCTION__ << " query execute failed with " <<  errString(_sqlquery));
                (void)SQLFetch(_sqlquery);
                (void)SQLCloseCursor(_sqlquery);
                if ( dbe._jobid != 0 ) {
                    dbe._ind[dbe.JOBID]=0;
                }
            }

            // Insert the RAS event into the database
            sqlrc = SQLExecute(_sqlstmt);
            if (sqlrc != 0)
                LOG_ERROR_MSG("processRASMessage execute failed with " <<  errString(_sqlstmt));

            if ((dbe._jobid == 0) ||
                (dbe._ind[dbe.CTLACTION] == SQL_NTS) ||
                (strcmp(dbe._block,"DefaultControlEventListener" ) == 0)) {
                // we must do the post-processing of this RAS event
                sqlrc =  SQLFetch(_sqlstmt);
                if (sqlrc != 0)
                    LOG_ERROR_MSG("fetching recid failed with " <<  errString(_sqlstmt));
            }

            (void)SQLCloseCursor(_sqlstmt);

            // add total time to insert this RAS event to our total for some bookkeeping
            const boost::posix_time::ptime end( boost::posix_time::microsec_clock::local_time() );
            const boost::posix_time::time_duration duration( end - start );
            _rasInsertionTime += duration;
            ++_rasInsertionCount;
        } else {  // this is diags
            // write a RAS event log record
            BGQDB::STATUS result = BGQDB::putRAS(
                                                 getBase()->_blockName,
                                                 rasEvent.getDetails(),
                                                 rasEvent.time(),
                                                 0, // This is diags so there won't be a real jobid.
                                                 getBase()->_diags
                                                 );

            if (result != BGQDB::OK)
            {
                LOG_ERROR_MSG("processRASMessage putRAS failed with rc=" << result);
            }
        }
        // save the time of this ras event
        time(&getBase()->_last_ras_event_time);
    } else {
        // nothing was inserted, so no recid to process
        dbe._recid = 0;
    }

    return dbe._recid;
}
