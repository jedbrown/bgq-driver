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

#include "DBBlockController.h"

#include "BCNodeInfo.h"
#include "BlockControllerBase.h"
#include "BlockControllerTarget.h"
#include "BootFailure.h"
#include "CNBlockController.h"
#include "DBConsoleController.h"
#include "DefaultListener.h"
#include "IOBlockController.h"
#include "ras.h"
#include "RunJobConnection.h"
#include "neighbor/add.h"

#include "common/Properties.h"

#include <control/include/mcServer/defaults.h>

#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/BGQDBlib.h>
#include <db/include/api/dataconv.h>
#include <db/include/api/ioUsage.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <ras/include/RasEvent.h>
#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>

#include <utility/include/Log.h>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/scope_exit.hpp>

#include <openssl/rand.h>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

using mmcs::common::Properties;

namespace {

string
errString(
        SQLHANDLE handle
        )
{
    SQLCHAR message[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER sqlcode;
    SQLSMALLINT length, i;

    i = 1;
    string errmsg;

    while (
            SQLGetDiagRec(
                SQL_HANDLE_STMT,
                handle,
                i,
                sqlstate,
                &sqlcode,
                message,
                SQL_MAX_MESSAGE_LENGTH + 1,
                &length
                ) == SQL_SUCCESS
          )
    {
        errmsg += " SQLSTATE: ";
        errmsg += string((char*)sqlstate);
        errmsg += " Message: ";
        errmsg += string((char*)message);
        i++;
    }

    return errmsg;
}

} // anonymous namespace

namespace mmcs {
namespace server {


const std::string&
DBBlockController::strDBError(
        const BGQDB::STATUS result
        )
{
    typedef std::map<BGQDB::STATUS, std::string> Map;
    static std::string unknown( "Unexpected database error" );
    static const Map errors = boost::assign::map_list_of
        (BGQDB::OK, "" )
        (BGQDB::DB_ERROR, "Unable to access database")
        (BGQDB::FAILED, "Operation failed")
        (BGQDB::INVALID_ID, "Invalid identifier")
        (BGQDB::NOT_FOUND, "Not found")
        (BGQDB::DUPLICATE, "Duplicate")
        (BGQDB::XML_ERROR, "File error")
        (BGQDB::CONNECTION_ERROR, "Database connection error")
        ;

    const Map::const_iterator message = errors.find( result );
    if ( message != errors.end() ) {
        return message->second;
    } else {
        return unknown;
    }
}

DBBlockController::DBBlockController(
        const std::string& userName,
        const std::string& blockName
        ) :
    BlockHelper(new BlockControllerBase(NULL, userName, blockName, true)),
    _allocate_block_start(0),
    dbe(),
    _dbj(),
    _sqlstmt(0),
    _sqlquery(0),
    _tx(NULL),
    _cbc(this),
    _counters( getBase() ),
    _status_mutex(),
    _rasInsertionCount(0),
    _rasInsertionTime()
{

}

DBBlockController::DBBlockController(
        BlockPtr ptr
        ) :
    BlockHelper(ptr),
    _allocate_block_start(0),
    dbe(),
    _dbj(),
    _sqlstmt(0),
    _sqlquery(0),
    _tx(NULL),
    _cbc(this),
    _counters( ptr ),
    _status_mutex(),
    _rasInsertionCount(0),
    _rasInsertionTime()
{

}

DBBlockController::~DBBlockController()
{
    if ( _rasInsertionCount ) {
        const double rate = static_cast<double>(_rasInsertionCount) / _rasInsertionTime.total_milliseconds() * 1000.0;
        LOG_DEBUG_MSG(
                "Inserted " << _rasInsertionCount << " RAS events in " <<
                _rasInsertionTime.total_milliseconds() << "ms: " <<
                rate << " per second"
                );
    }

    // Skip I/O blocks
    if ( !getBase()->isIoBlock() && this->getBlockName() != mc_server::DefaultListener ) {
        DefaultListener::get()->remove( getBase()->_blockName );
    }

    _cbc.stop();
}

BGQDB::STATUS
DBBlockController::setBlockStatus(
        const BGQDB::BLOCK_STATUS status,
        const std::deque<std::string>& option
        )
{
    BGQDB::STATUS result = BGQDB::OK;

    boost::mutex::scoped_lock lock(_status_mutex);
    if ( getBase()->_rebooting && status == BGQDB::INITIALIZED ) {
        getBase()->_rebooting = false;
    } else {
        result = BGQDB::setBlockStatus(this->getBlockName(), status, option);
    }

    _status_notifier.notify_all();

    if ( !result ) {
        LOG_INFO_MSG("Block status changed to " << BGQDB::blockStatusToString(status));
    }

    return result;
}

void
DBBlockController::Die()
{
    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    deque<string> dq;
    mmcs_client::CommandReply reply;
    this->disconnect(dq, reply);

    BGQDB::BlockInfo bInfo;
    const BGQDB::STATUS result = setBlockStatus(BGQDB::FREE);
    if (result != BGQDB::OK) {
        // Note that the setBlockStatus() call will dump errors to the log
        if (BGQDB::getBlockInfo(getBase()->_blockName, bInfo) == BGQDB::OK) {
            LOG_ERROR_MSG("Request to change block status to FREE failed. Current block status is " << BGQDB::blockCodeToString(bInfo.status));
        } else {
            LOG_ERROR_MSG("Request to change block status to FREE failed.");
        }
    }

    mutex.Unlock();
}

void
DBBlockController::allocateBlock(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
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
        reply << mmcs_client::FAIL << "Block is being deallocated, retry later." << mmcs_client::DONE;
        return;
    }

    // Process local arguments. non-local arguments are passed to BlockController::create_block and BlockController::connect
    if (args.size() == 0) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }
    const string blockName = args[0];
    vector<int> removeArgs;   // arguments that are not to be passed to BlockController::create_block
    bool no_connect = false;  // skip connecting to block resources?

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
        }
    }

    for (unsigned i = 0; i < removeArgs.size(); ++i) {
        args.erase(args.begin() + removeArgs[i]);
    }

    PthreadMutexHolder mutex;

    int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    if ((result = BGQDB::getBlockInfo(blockName, bInfo)) != BGQDB::OK) {
        reply << mmcs_client::FAIL << strDBError(result) << ": unable to retrieve block information." << mmcs_client::DONE;
        return;
    }

    // Must have valid node config info
    if (bInfo.domains.size() == 0) {
        reply << mmcs_client::FAIL << "Block has invalid node config or domain information." << mmcs_client::DONE;
        return;
    }

    // Block status must be Free or Allocated
    if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) != 0 && strcmp(bInfo.status, BGQDB::BLOCK_ALLOCATED) != 0) {
        reply << mmcs_client::FAIL << "Block is already allocated." << mmcs_client::DONE;
        return;
    }

    // Generate a boot cookie
    getBase()->getBootCookie();

    // Set block status to Allocated
    if ( strcmp( bInfo.status, BGQDB::BLOCK_FREE ) == 0 ) {
        // Set owner name
        setBlockStatus_options.push_back(std::string("user=") + getBase()->_userName);
        if ((result = setBlockStatus(BGQDB::ALLOCATED, setBlockStatus_options)) != BGQDB::OK) {
            mutex.Unlock();		// must unlock the mutex before setDisconnecting()
            LOG_ERROR_MSG("Setting block status to ALLOCATED failed. Current block status is " << BGQDB::blockCodeToString(bInfo.status));
            reply << mmcs_client::FAIL << "Setting block status to ALLOCATED failed, hardware resources may be allocated to a different block." << mmcs_client::DONE;
            getBase()->setDisconnecting(true, reply.str());
            return;
        }
    }

    mutex_rc = mutex.Unlock();
    assert(mutex_rc == 0);

    time(&_allocate_block_start);
    // Set securityKey
    unsigned char keyBuffer[PERSONALITY_LEN_SECKEY] = {0};
    const int rc = RAND_bytes( keyBuffer, sizeof(keyBuffer) );
    if ( rc == 0 ) {
        reply << mmcs_client::FAIL << "Unable to set security key on block: " << rc << mmcs_client::DONE;
        return;
    }
    result = BGQDB::setBlockSecurityKey(blockName, keyBuffer, sizeof(keyBuffer) );

    std::ostringstream key;
    for ( unsigned i = 0; i < sizeof(keyBuffer); ++i ) {
        key << std::hex << static_cast<unsigned>( keyBuffer[i] );
    }
    LOG_TRACE_MSG( "Security key: 0x" << key.str() );

    if (result != BGQDB::OK) {
        reply << mmcs_client::FAIL << strDBError(result) << ": unable to set security key on block." << mmcs_client::DONE;
        (void)setBlockStatus(BGQDB::FREE);
        getBase()->setDisconnecting(true, reply.str());
        return;
    }

    allocateTimer->dismiss( false );
    allocateTimer->stop();

    // Create the block
    create_block(args, reply);
    if (reply.getStatus() != 0) {
        return;
    }

    // Establish connections to the block resources
    if (!no_connect) {
        const PerformanceCounters::Timer::Ptr timer = _counters.create()
            ->id( this->getBlockName() )
            ->mode( bgq::utility::performance::Mode::Value::Basic )
            ->function( "boot" )
            ->subFunction( "create target set" )
            ;
        timer->dismiss();

        // Connect to the targets in the block.
        // Use a perm target set when this mmcs server is running the database monitor,
        // otherwise use a temporary target set.

        const BlockControllerTarget target(getBase(), "{*}", reply);
        args.pop_front();	// remove blockname argument
        args.push_back(Properties::getProperty(DFT_TGTSET_TYPE));

        if (getBase()->isIoBlock()) {
            boost::dynamic_pointer_cast<IOBlockController>(getBase())->connect(args, reply, &target);
        } else {
            boost::dynamic_pointer_cast<CNBlockController>(getBase())->connect(args, reply, &target);
        }

        if (reply.getStatus() != 0) {
            (void)setBlockStatus(BGQDB::FREE);
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
DBBlockController::create_block(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    BGQDB::STATUS result;

    const PerformanceCounters::Timer::Ptr getXmlTimer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "load block XML" )
        ;
    getXmlTimer->dismiss();

    if (args.size() == 0) {
        reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
        return;
    }

    deque<string> createArgs(args); // make a copy of our command arguments
    const string blockName = createArgs[0]; createArgs.pop_front();

    // Extract block xml from the database
    stringstream blockStream;
    const bool diagsArg = (find(args.begin(), args.end(), "diags") != args.end());
    if ((result = BGQDB::getBlockXML(blockStream, blockName, diagsArg)) != BGQDB::OK) {
        reply << mmcs_client::FAIL << strDBError(result) << ": unable to retrieve block xml." << mmcs_client::DONE;
        (void)setBlockStatus(BGQDB::TERMINATING);
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

    if (args.empty()) {
        createArgs.push_front(string("foobar")); // add a bogus filename to the front of the list
    }

    // Create the block
    getBase()->create_block(createArgs, reply, &blockStream);
    if (reply.getStatus() != 0) {
        (void)setBlockStatus(BGQDB::TERMINATING);
        getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
        return;
    }

    if (!getBase()->linkless_mps().empty() && !getBase()->_diags) {
        const unsigned count = getBase()->linkless_mps().size();
        reply << mmcs_client::FAIL << count << " midplane";
        if ( count == 1 ) {
            reply << " has";
        } else {
            reply << "s have";
        }
        reply << " no I/O link:" << std::endl;
        const std::ostream_iterator<std::string> iterator( reply, " " );
        std::copy( getBase()->linkless_mps().begin(), getBase()->linkless_mps().end(), iterator );
        reply << mmcs_client::DONE;
        (void)setBlockStatus(BGQDB::TERMINATING);
        getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox
        return;
    }

    LOG_TRACE_MSG( "Block creation ID: " << getBase()->_block->blockId() );

    // Reply is already set by create_block
    createTimer->dismiss( false );
    return;
}

void
DBBlockController::initMachineConfig(
        mmcs_client::CommandReply& reply
        )
{
    const PerformanceCounters::Timer::Ptr timer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "load machine XML" )
        ;
    timer->dismiss();

    // Create a new BGQMachineXML object from the database
    stringstream machineStream;
    if (BGQDB::getMachineXML(machineStream) != BGQDB::OK) {
        reply << mmcs_client::FAIL << "Cannot load machine XML." << mmcs_client::DONE;
        return;
    }

    // Create the BGQMachine classes from the xml
    if (getBase()->_machineXML != NULL) {
        delete getBase()->_machineXML;
        getBase()->_machineXML = NULL;
    }

    try {
        getBase()->_machineXML = BGQMachineXML::create(machineStream);
    } catch (const XMLException& e) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    } catch (const BGQNodeConfigException& e) {
        reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
        return;
    }

    if (getBase()->_machineXML == NULL) {
        reply << mmcs_client::FAIL << "Cannot create from machine XML." << mmcs_client::DONE;
        return;
    }

    timer->dismiss( false );

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
DBBlockController::boot_block(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    const PerformanceCounters::Timer::Ptr bootTimer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "boot" )
        ->subFunction( "boot block" )
        ;
    bootTimer->dismiss();

    const boost::posix_time::ptime bst( getBase()->getBootStartTime() );
    if ( !bst.is_not_a_date_time() ) { // We're waiting on a boot
        reply << mmcs_client::FAIL << "Block has been booting since " << bst << mmcs_client::DONE;
        return;
    }

    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    bool useDb = false;	// use boot images and options from the bgqblock table
    deque<string> arg_bootoptions, arg_domains;
    string arg_uloader, arg_steps;
    string db_uloader, db_domains;
    deque<string> boot_args;	// options to pass to BlockController::boot_block
    bool booted = false;
    BOOST_SCOPE_EXIT( ( &_allocate_block_start ) ( &booted) ) {
        if (!booted) {
            _allocate_block_start = 0;
        }
    } BOOST_SCOPE_EXIT_END;

    // Check for block in proper state
    if ((result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo)) != BGQDB::OK) {
        reply << mmcs_client::FAIL << strDBError(result) << ": unable to retrieve block information." << mmcs_client::DONE;
        return;
    }

    // Parse the boot_block arguments
    time(&_allocate_block_start);
    bool updating = false;
    if (args.empty()) {
        // If no arguments are specified, use the load images and boot options from the bgqblock table.
        useDb = true;
    } else {
        for (unsigned i = 0; i < args.size(); ++i) {
            if (args[i] == "update") {
                useDb = true;
                updating = true;
                continue;
            }
            if (args[i].compare(0, 8, "uloader=") == 0) {
                if (!arg_uloader.empty()) {
                    reply << mmcs_client::FAIL << "Only one uloader permitted." << mmcs_client::DONE;
                    return;
                }
                arg_uloader = args[i];
                continue;
            }
            if (args[i].compare(0, 6, "steps=") == 0) {
                if (!arg_steps.empty()) {
                    reply << mmcs_client::FAIL << "Only one comma separated list of steps permitted." << mmcs_client::DONE;
                    return;
                }
                arg_steps = args[i];
                continue;
            }
            if (args[i].compare(0, 7, "domain=")  == 0) {
                std::string domainstr;

                // Reconstitute the domain string.
                // Loop through the args and append until we get to a '}'
                unsigned int argindex = i;
                for (std::string curstring = args[i]; argindex < args.size(); ++argindex) {
                    curstring = args[argindex];
                    if (curstring.find("cores=") != std::string::npos) {
                        curstring.replace(curstring.find('-'), 1, 1, '$');
                    }
                    if (curstring.find("memory=") != std::string::npos) {
                        curstring.replace(curstring.find('-'), 1, 1, '$');
                    }

                    curstring += " ";
                    domainstr += curstring;
                    if (curstring.find('}') != std::string::npos) {
                        break;
                    }
                    ++i;
                }

                // Validate the content
                if (domainstr.find("id=") == std::string::npos) {
                    reply << mmcs_client::FAIL << "Cannot set domain info without an id specified." << mmcs_client::DONE;
                    return;
                }

                if (domainstr.find("cores=") != std::string::npos && useDb) {
                    reply << mmcs_client::FAIL << "Cannot change core config on an update." << mmcs_client::DONE;
                    return;
                }

                if (domainstr.find("memory=") != std::string::npos && useDb) {
                    reply << mmcs_client::FAIL << "Cannot change memory config on an update." << mmcs_client::DONE;
                    return;
                }

                if (domainstr.find("custaddr=") != std::string::npos && useDb) {
                    reply << mmcs_client::FAIL << "Cannot change customization address on an update." << mmcs_client::DONE;
                    return;
                }

                if (domainstr.find("options=") != std::string::npos && useDb) {
                    reply << mmcs_client::FAIL << "Cannot change domain options on an update." << mmcs_client::DONE;
                    return;
                }

                arg_domains.push_back(domainstr);
                continue;
            } else {
                arg_bootoptions.push_back(args[i]);
            }
        }
    }

    // Need to transform the bInfo domain stuff to domain syntax to pass as args.
    bool foundId = false;
    LOG_DEBUG_MSG( bInfo.domains.size() << " domain" << (bInfo.domains.size() == 1 ? "" : "s") << " found" );
    for (
            std::vector<BGQDB::DomainInfo>::const_iterator it = bInfo.domains.begin();
            it != bInfo.domains.end();
            ++it
        )
    {
        std::ostringstream dmnstrm;
        dmnstrm << "domain={";

        if (it->endCore) {
            // We have a core range.  We use an end core because a zero start core is valid.
            // Don't use '-' as a separator so we don't confuse negative numbers.
            dmnstrm << "cores=" << it->startCore << "$" << it->endCore << " ";
        }
        if (it->endAddr) {
            // Address range specified
            dmnstrm << "memory=" << it->startAddr << "$" << it->endAddr << " ";
        }

        std::string images = it->imageSet;

        // If we are using the DB entries and boot_block update, then we overwrite
        // the images field from the args to the matching DB domain.
        if (useDb && updating) {
            // Loop through arg domains to see if this domain's id is specified.
            bool found = false;
            for (
                    std::deque<std::string>::const_iterator arg_it = arg_domains.begin();
                    arg_it != arg_domains.end();
                    ++arg_it
                )
            {
                if (arg_it->find(it->id) != std::string::npos) {
                    // Found the id. Overwrite the image picked up above.
                    found = true;
                    std::string workstring = *arg_it;
                    const boost::char_separator<char> space_sep(" ");
                    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
                    const tokenizer fields(workstring, space_sep);
                    workstring.erase(workstring.begin(), workstring.begin() + workstring.find("{") + 1);
                    BOOST_FOREACH(std::string field, fields) {
                        if (field.find("images=") != std::string::npos) {
                            field.erase(field.begin(),field.begin() + field.find("images=") + 7);
                            images = field;
                        }
                    }
                }
            }
            if (!found) {
                reply << mmcs_client::FAIL << "Invalid domain ID specified on update." << mmcs_client::DONE;
                return;
            }
        }

        dmnstrm << "custaddr=" << it->custAddr << " ";

        if (images.length() != 0) {
            dmnstrm << "images=" << images << " ";
        }

        const std::string options = it->options;
        if (options.length() != 0) {
            dmnstrm << "options=" << options << " ";
        }
        const std::string id = it->id;
        if (id.length() != 0) {
            dmnstrm << "id=" << id << " ";
            foundId = true;
        }
        dmnstrm << "} ";
        db_domains += dmnstrm.str();
    }
    if (foundId == false) {
        reply << mmcs_client::FAIL << "Domain id to update not specified." << mmcs_client::DONE;
        return;
    }

    if (strlen(bInfo.uloaderImg) > 0) {
        db_uloader.assign("uloader=").append(bInfo.uloaderImg);
    }

    if (useDb) {
        // Merging boot_block images and options with those from the database
        const string bootOptionsProps = Properties::getProperty(MMCS_BOOT_OPTIONS);

        if (!arg_uloader.empty()) {
            boot_args.insert(boot_args.end(), arg_uloader);
        } else {
            boot_args.push_back(db_uloader);
        }

        BGQDB::trim_right_spaces(bInfo.bootOptions);
        if (strlen(bInfo.bootOptions) > 0) {
            boot_args.push_back(string(bInfo.bootOptions));
        } else if (!bootOptionsProps.empty()) {
            boot_args.push_back(bootOptionsProps);
        }

        if (!arg_bootoptions.empty()) {
            boot_args.insert(boot_args.end(), arg_bootoptions.begin(), arg_bootoptions.end());
        }

        if (!arg_steps.empty()) {
            boot_args.insert(boot_args.end(), arg_steps);
        }

        // We already did the merge when we parsed through the block info
        boot_args.push_back(db_domains);
    } else {
        // Not merging boot_block images and options with those from the database
        if (!arg_uloader.empty()) {
            boot_args.insert(boot_args.end(), arg_uloader);
        } else {
            reply << mmcs_client::FAIL << "uloader not specified." << mmcs_client::DONE;
            return;
        }
        if (!arg_bootoptions.empty()) {
            boot_args.insert(boot_args.end(), arg_bootoptions.begin(), arg_bootoptions.end());
        }
        if (!arg_domains.empty()) {
            boot_args.insert(boot_args.end(), arg_domains.begin(), arg_domains.end());
        }
        if (!arg_steps.empty()) {
            boot_args.insert(boot_args.end(), arg_steps);
        }
    }

    std::ostringstream logstream;
    logstream << "Boot arguments: ";
    for (std::deque<string>::const_iterator it = boot_args.begin(); it != boot_args.end(); ++it) {
        logstream << *it << " | ";
    }

    LOG_DEBUG_MSG(logstream.str());

    if ( !strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) ) {
        if ( arg_steps.empty() ) {
            reply << mmcs_client::FAIL << "Boot steps required for block in " << BGQDB::blockCodeToString(bInfo.status) << " status." << mmcs_client::DONE;
            return;
        }
    } else if ( strcmp(bInfo.status, BGQDB::BLOCK_ALLOCATED) ) {
        reply << mmcs_client::FAIL << "Invalid block status: " << BGQDB::blockCodeToString(bInfo.status) << mmcs_client::DONE;
        return;
    }

    if (getBase()->isIoBlock() == false && getBase()->_diags == false) {
        // If we're a diags block, we don't care about I/O rules.
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
        if ((result = BGQDB::checkBlockIO(getBase()->_blockName, &unconnectedIONodes, &midplanesFailingIORules, &unconnectedAvailableIONodes)) != BGQDB::OK) {
            LOG_ERROR_MSG("Check block I/O for block " << getBase()->_blockName << " failed, result=" << result);
            reply << mmcs_client::FAIL << strDBError(result) << ": unable to check block I/O." << mmcs_client::DONE;
            return;
        }

        if (unconnectedAvailableIONodes.size() > 0) {
            std::ostringstream failmsg;
            failmsg << "Cannot boot block " << getBase()->_blockName << ". The following I/O nodes in the block are not booted, or in an I/O block that is deallocating: ";
            for (
                    std::vector<string>::const_iterator it = unconnectedAvailableIONodes.begin();
                    it != unconnectedAvailableIONodes.end();
                    ++it
                )
            {
                failmsg << *it << " ";
            }
            LOG_ERROR_MSG(failmsg.str());
            reply << mmcs_client::FAIL << failmsg.str() << mmcs_client::DONE;
            return;
        }

        if (midplanesFailingIORules.size() > 0) {
            std::ostringstream failmsg;
            failmsg << "Cannot boot block " << getBase()->_blockName << ". The following midplanes in the block do not have the minimum connected I/O nodes: ";
            for (
                    std::vector<string>::const_iterator it = midplanesFailingIORules.begin();
                    it != midplanesFailingIORules.end();
                    ++it
                )
            {
                failmsg << *it << " ";
            }
            LOG_ERROR_MSG(failmsg.str());
            reply << mmcs_client::FAIL << failmsg.str() << mmcs_client::DONE;
            return;
        }

        // If we are not already booting due to step-wise boots, enforce
        // the Revised Mundy rule.
        if (!strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) == 0) {
            BGQMidplaneNodeConfig::ComputeCount cc;
            getBase()->_block->getBridgeCount(cc);
            try {
                BGQDB::ioUsage::update(cc, getBase()->_blockName);
            } catch (const BGQDB::ioUsage::UsageExceeded& e) {
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
                reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
                return;
            }
        }
    }

    // Set block status to booting
    if (!strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) == 0) {
        // If we're not yet booting (because we're going stepwise), set the state.
        deque<string> options;
        options.push_back(std::string("qualifier=") +  boost::lexical_cast<std::string>(getBase()->_bootCookie));
        if ((result = setBlockStatus(BGQDB::BOOTING, options)) != BGQDB::OK)
        {
            LOG_ERROR_MSG("Setting block status to BOOTING failed. Current block status is " << BGQDB::blockCodeToString(bInfo.status));
            reply << mmcs_client::FAIL << "Setting block status to BOOTING failed." << mmcs_client::DONE;
            return;
        }

        DefaultListener::get()->add( getBase()->_block );
    }

    // Boot the block
    std::map<std::string,std::string> nodeCust;
    BGQDB::getCustomization(getBase()->_blockName, nodeCust);

    // If it's a full system block, bypass environmentals during boot.
    bool full_system = false;
    if ( !getBase()->isIoBlock() ) {
        const BGQBlockNodeConfig* config = getBase()->_block;
        const BGQMachineXML* machine = getBase()->_machineXML;
        const unsigned nodes = config->aNodeSize() * config->bNodeSize() * config->cNodeSize() * config->dNodeSize() * config->eNodeSize();
        full_system = (nodes == machine->_midplanes.size() * NODES_PER_MIDPLANE);
    }

    BOOST_SCOPE_EXIT( ( &full_system) ) {
        if (full_system) {
            Properties::setProperty("bypass_envs", "false");
        }
    } BOOST_SCOPE_EXIT_END;

    if (full_system) {
        Properties::setProperty("bypass_envs", "true");
    }

    getBase()->boot_block(boot_args, reply, nodeCust, bootTimer);
    booted = true;
    if (reply.getStatus() != 0) {
        const std::deque<std::string> errmsg( 1, "errmsg=" + reply.str() );
        (void)setBlockStatus(BGQDB::TERMINATING, errmsg);

        getBase()->setDisconnecting(true, reply.str()); // reset icon connections and stop mailbox

        BootFailure( getBase(), reply.str() );
        return;
    }

    if (getBase()->_check_complete) {
        // Start waiting for the boot to complete
        _cbc.start();
    }

    bootTimer->dismiss( false );

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
DBBlockController::waitBoot(
        deque<string> cmdargs,
        mmcs_client::CommandReply& reply,
        const bool interactive,
        const bool rebooting
        )
{
    BGQDB::STATUS result = BGQDB::OK;
    BGQDB::BlockInfo bInfo;
    float boot_slope = 0;

    if (getBase()->isIoBlock() == false) {
        boot_slope = atof(Properties::getProperty(CN_BOOT_SLOPE).c_str());
    } else {
        boot_slope = atof(Properties::getProperty(IO_BOOT_SLOPE).c_str());
    }

    double wait_time = atoi(Properties::getProperty(MINIMUM_BOOT_WAIT).c_str()); // Minimum wait time in seconds
    const int maximum_timeout = atoi(Properties::getProperty(WAIT_BOOT_FREE_TIME).c_str()) * 60;  // in seconds

    if (cmdargs.size() > 0) {
        try {
            // Input is minutes, so multiply by 60.0 to get seconds
            wait_time = boost::lexical_cast<double>(cmdargs[0]) * 60.0;
            if (wait_time <= 0) {
                reply << mmcs_client::FAIL << "args?" << mmcs_client::DONE;
                return;
            }
        } catch ( const boost::bad_lexical_cast& e ) {
            reply << mmcs_client::FAIL << "Bad timeout value: " << cmdargs[0] << mmcs_client::DONE;
            return;
        }
    } else {
        wait_time = getBase()->_numNodesTotal * boot_slope + wait_time;
        LOG_TRACE_MSG("nnt=" << getBase()->_numNodesTotal << " slope=" << boot_slope << " wt=" << wait_time);
        if ( wait_time > maximum_timeout ) {
            wait_time = maximum_timeout;
            LOG_TRACE_MSG( "overriding wait time to " << wait_time );
        }
    }

    {
        boost::mutex::scoped_lock lock( _status_mutex );

        result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo);
        if ( result != BGQDB::OK ) {
            reply << mmcs_client::FAIL << "Could not get block status: " << strDBError(result) << mmcs_client::DONE;
            return;
        }

        if ( rebooting ) {
            // Skip the status checks below, block status remains Initialized for the duration
            // of the reboot_nodes command
        } else if ( !strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) ) {
            reply << mmcs_client::OK << mmcs_client::DONE;
            return;
        } else if ( strcmp(bInfo.status, BGQDB::BLOCK_BOOTING) ) {
            reply << mmcs_client::FAIL << "Block status of " << BGQDB::blockCodeToString(bInfo.status) << " is not BOOTING." << mmcs_client::DONE;
            return;
        }

        // The time we start waiting depends if this is interactive (wait_boot command from bg_console)
        // or executed from an allocate command
        const boost::posix_time::ptime start =
            interactive || rebooting ?
            boost::get_system_time() :
            boost::posix_time::from_time_t( _allocate_block_start )
            ;
        LOG_INFO_MSG(__FUNCTION__ << "(" << wait_time << ")");

        // Wait on condition variable in a loop to guard against spurious unblocks
        while (
                rebooting ? getBase()->_rebooting :
                !strcmp(bInfo.status, BGQDB::BLOCK_BOOTING)
              )
        {
            const bool condition_result = _status_notifier.timed_wait(
                    lock,
                    start + boost::posix_time::seconds(wait_time)
                    );

            result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo);
            if ( result != BGQDB::OK ) {
                reply << mmcs_client::FAIL << "Could not get block status after condition variable: " << strDBError(result) << mmcs_client::DONE;
                return;
            }
            if ( rebooting ) {
                LOG_DEBUG_MSG( "Rebooting " << (getBase()->_rebooting ? "true" : "false") );
            } else {
                LOG_TRACE_MSG( "Block status is " << BGQDB::blockCodeToString(bInfo.status) );
            }

            if ( !condition_result ) {
                LOG_WARN_MSG("Block initialization timed out.");
                break;
            }
        }

        if ( !strcmp(bInfo.status, BGQDB::BLOCK_INITIALIZED) ) {
            // LOG_DEBUG_MSG("Block initialization successful");
            reply << mmcs_client::OK << mmcs_client::DONE;
            return;
        }

        LOG_WARN_MSG("Block initialization failed, block status is " << BGQDB::blockCodeToString(bInfo.status) );
        // Fall through
    }

    // Check boot completion status
    if (getBase()->isDisconnecting()) {
        reply << mmcs_client::FAIL << getBase()->disconnectReason() << mmcs_client::DONE;
        return;
    }

    // If we get here, we failed. Now figure out why.
    if (rebooting) {
        reply << mmcs_client::FAIL << "Timeout from node(s): ";
        for (
                vector<BCNodeInfo*>::const_iterator i = getBase()->getNodes().begin();
                i  != getBase()->getNodes().end();
                ++i
            )
        {
            if ((*i)->_initialized == false) {
                LOG_WARN_MSG( __FUNCTION__ << (*i)->location() << " not initialized");
                reply << (*i)->location() << " ";
            }
        }
        reply <<  " Block left in BOOTING state" << mmcs_client::DONE;
        return;
    }

    string errmsg = "Block initialization timed out";
    std::string blockErrorText;
    result = BGQDB::getBlockErrorText(getBase()->_blockName, blockErrorText);
    if (result == BGQDB::OK && !blockErrorText.empty()) {
        errmsg.append(": ").append(blockErrorText);
    }
    (void)setBlockStatus(BGQDB::TERMINATING);

    getBase()->setDisconnecting(true, errmsg);

    BootFailure( getBase(), errmsg ).timeout();

    reply << mmcs_client::FAIL << "Boot timeout" << mmcs_client::DONE;
}

void
DBBlockController::waitFree(
        mmcs_client::CommandReply& reply
        )
{
    while (1) {
        boost::mutex::scoped_lock lock( _status_mutex );

        BGQDB::BlockInfo bInfo;
        const BGQDB::STATUS result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo);
        if ( result == BGQDB::OK && !strcmp(bInfo.status, BGQDB::BLOCK_FREE) ) {
            reply << mmcs_client::OK << mmcs_client::DONE;
            return;
        } else if ( result != BGQDB::OK ) {
            reply << mmcs_client::FAIL << "Could not get block status: " << result << mmcs_client::DONE;
            return;
        }

        // Wait for next status change
        LOG_TRACE_MSG( __FUNCTION__ << ": block status is " << BGQDB::blockCodeToString(bInfo.status));
        _status_notifier.wait( lock );
    }
}

void
DBBlockController::freeBlock(
        deque<string> args,
        mmcs_client::CommandReply& reply
        )
{
    LOG_TRACE_MSG(__FUNCTION__);

    PerformanceCounters::Timer::Ptr timer = _counters.create()
        ->id( this->getBlockName() )
        ->mode( bgq::utility::performance::Mode::Value::Basic )
        ->function( "shutdown" )
        ->subFunction( "free block" )
        ;
    timer->dismiss();

    const boost::posix_time::ptime bst( getBase()->getBootStartTime() );
    if ( !bst.is_not_a_date_time() ) { // We're waiting on a boot
        reply << mmcs_client::FAIL << "Block has been booting since " << bst << mmcs_client::DONE;
        return;
    }

    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    BGQDB::STATUS result;
    if (getBase()->isIoBlock() && !getBase()->_diags) {
        std::vector<std::string> compute_blocks;
        result = BGQDB::checkIOBlockConnection(getBase()->_blockName, &compute_blocks);
        if (result != BGQDB::OK) {
            reply << mmcs_client::FAIL << strDBError(result)
                  << ": Database cannot determine connected compute blocks." << mmcs_client::DONE;
            return;
        }

        if (!compute_blocks.empty()) {
            // For each compute block, we have to find it in the map and see if it is diags.
            std::vector<std::string> diags_blocks;
            for (
                    std::vector<std::string>::const_iterator it = compute_blocks.begin();
                    it != compute_blocks.end();
                    ++it
                )
            {
                const DBBlockPtr blockp = DBConsoleController::findBlock(*it);
                if (blockp) {
                    // The find incremented the block thread count so we have to decrement it again.
                    blockp->getBase()->decrBlockThreads();
                    if (blockp->getBase()->_diags) {
                        diags_blocks.push_back(*it);
                    }
                }
            }

            BOOST_FOREACH(const std::string& curr_block, diags_blocks) {
                compute_blocks.erase(std::remove(compute_blocks.begin(), compute_blocks.end(), curr_block));
            }

            // This will have changed if all connected computes are diags blocks.
            if (compute_blocks.size() != 0) {
                reply << mmcs_client::FAIL << "Connected compute block" << (compute_blocks.size() == 1 ? "" : "s") << " ";
                for (
                        std::vector<std::string>::const_iterator it = compute_blocks.begin();
                        it != compute_blocks.end();
                        ++it
                    )
                {
                    if (it != compute_blocks.begin()) {
                        reply << ", ";
                    }
                    reply << *it;
                }
                reply << " " << (compute_blocks.size() == 1 ? "is" : "are") << " still booted. "
                      << (compute_blocks.size() == 1 ? "It" : "They") << " must be freed before I/O block "
                      << getBase()->_blockName << " can be freed." << mmcs_client::DONE;
                return;
            }
        }
    }

    // Get the current block status
    BGQDB::BlockInfo bInfo;
    result = BGQDB::getBlockInfo(getBase()->_blockName, bInfo);
    if (result != BGQDB::OK) {
        if (result == BGQDB::INVALID_ID) {
            reply << mmcs_client::FAIL << strDBError(result) << ": invalid block id." << mmcs_client::DONE;
        } else {
            reply << mmcs_client::FAIL << strDBError(result) << ": " << __FUNCTION__ << "() getBlockInfo failed." << mmcs_client::DONE;
        }
        return;
    }

    // If the block is already free, return 'OK'
    if (strcmp(bInfo.status, BGQDB::BLOCK_FREE) == 0) {
        reply << mmcs_client::OK << mmcs_client::DONE;
        return;
    }

    // Change the block status
    result = setBlockStatus(BGQDB::TERMINATING);
    if (result != BGQDB::OK) {	// setBlockStatus failed
        // Log an error
        LOG_ERROR_MSG("Unable to set block status to TERMINATING for block " << getBase()->getBlockName() << ", current block status is " << BGQDB::blockCodeToString(bInfo.status));

        // Return an error
        if (strcmp(bInfo.status, BGQDB::BLOCK_TERMINATING) != 0) {
            if (result == BGQDB::FAILED) {
                reply << mmcs_client::FAIL << "Free block failed, is there a running job associated with the block?" << mmcs_client::DONE;
            } else {
                reply << mmcs_client::FAIL << "Free block failed." << mmcs_client::DONE;
            }
            return;
        }
    }

    // Done with the mmcs_sever portion of freeing a block.
    // The free timer needs to be reset before it can be inserted into the tbgqcomponentperf table
    timer->dismiss( false );
    timer.reset();
    _counters.output( getBase()->_bootCookie );

    if (!getBase()->_diags && getBase()->isStarted()) {
        getBase()->shutdown_block(reply, args);
        if (reply.getStatus() != mmcs_client::CommandReply::STATUS_OK && getBase()->_block_shut_down == false) {
            (void)setBlockStatus(BGQDB::ALLOCATED);
            return;
        }
    } else if ( getBase()->_diags && !getBase()->isConnected() ) {
        const std::deque<string> args( 1, common::Properties::getProperty(DFT_TGTSET_TYPE) );
        const BlockControllerTarget target(getBase(), "{*}", reply);
        getBase()->connect( args, reply, &target );
    }

    // Set the DBBlockController to disconnecting
    std::string blockErrorText;
    result = BGQDB::getBlockErrorText(getBase()->_blockName, blockErrorText);
    if (result != BGQDB::OK || blockErrorText[0] == '\0') {
        getBase()->setDisconnecting(true, "block has been freed"); // reset icon connections and stop mailbox
    }  else {
        getBase()->setDisconnecting(true, blockErrorText);
    }

    // Make sure redirection ends
    getBase()->resetRedirection();

    for (
            vector<BCNodeInfo*>::const_iterator iter = getBase()->getNodes().begin();
            iter != getBase()->getNodes().end();
            ++iter
        )
    {
        (*iter)->reset_state();	// reset the node state
    }

    if ( !getBase()->isIoBlock() && std::find(args.begin(), args.end(), "abnormal") != args.end() ) {
        this->abnormalComputeShutdown();
    }

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
DBBlockController::disconnect(
        const deque<string>& args,
        mmcs_client::CommandReply& reply
        )
{
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;

    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&getMutex());
    assert(mutex_rc == 0);

    LOG_INFO_MSG("Disconnecting block.");

    getBase()->disconnect(args, reply);

    BGQDB::BLOCK_STATUS blockState = BGQDB::INVALID_STATE;
    BGQDB::getBlockStatus(getBase()->_blockName, blockState);
    if (blockState != BGQDB::TERMINATING) {
        result = setBlockStatus(BGQDB::TERMINATING);
        if (result != BGQDB::OK) {
            if (BGQDB::getBlockInfo(getBase()->_blockName, bInfo) == BGQDB::OK) {
                LOG_WARN_MSG("Setting block status to TERMINATING failed, current block status is " << BGQDB::blockCodeToString(bInfo.status));
                const DBBlockPtr dbbc = DBConsoleController::findBlock(getBase()->_blockName);
                if (dbbc) {
                    LOG_INFO_MSG("Block " << getBase()->_blockName << " found.");
                    // The find incremented the block thread count so we have to decrement it again.
                    dbbc->getBase()->decrBlockThreads();
                } else {
                    LOG_INFO_MSG("Block " << getBase()->_blockName << " not found.");
                }
            } else {
                LOG_WARN_MSG("Setting block status to TERMINATING failed.");
            }
        }
    }

    (void)setBlockStatus(BGQDB::ALLOCATED); // don't care about result, error will be logged from BGQDB

    mutex.Unlock();
}

const std::string&
DBBlockController::getBlockName() const
{
    return getBase()->_blockName;
}

void
DBBlockController::processConsoleMessage(
        MCServerMessageSpec::ConsoleMessage& consoleMessage
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    getBase()->processConsoleMessage(consoleMessage);
}

void
DBBlockController::postProcessRASMessage(
        const int recid
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<BGQDB::job::Id> jobsToKill;
    BGQDB::SparingInfo sInfo;

    const BGQDB::STATUS recstat = BGQDB::postProcessRAS(recid, jobsToKill, sInfo);
    if (recstat != BGQDB::OK) {
        LOG_ERROR_MSG("Could not post-process RAS id " << recid << ". Error " << recstat);
        return;
    }
    BOOST_FOREACH( const BGQDB::job::Id i, jobsToKill ) {
        RunJobConnection::instance().kill(i, bgcios::jobctl::SIGHARDWAREFAILURE, recid);
    }

    if (sInfo.wireMask != 0) {
        // call the mcserver function for BQL sparing
        const MCServerMessageSpec::DynamicSparingRequest::LinkChipSpareXmtr fromlink(
                sInfo.txLoc,
                sInfo.txReg,
                sInfo.txMask
                );
        const MCServerMessageSpec::DynamicSparingRequest::LinkChipSpareRcvr tolink(
                sInfo.rxLoc,
                sInfo.rxReg,
                sInfo.rxMask
                );
        const MCServerMessageSpec::DynamicSparingRequest sparingreq(fromlink, tolink);
        MCServerMessageSpec::DynamicSparingReply sparingrep;
        try {
            MCServerRef* temp;
            mmcs_client::CommandReply reply;
            BlockControllerBase::mcserver_connect(temp, getBase()->_userName, reply);
            boost::scoped_ptr<MCServerRef> ref( temp );
            if (reply.getStatus()) {
                LOG_ERROR_MSG( reply.str() );
                return;
            }
            LOG_TRACE_MSG("Connected to mc_server.");

            ref->dynamicSparing(sparingreq, sparingrep);
        } catch (const std::exception &e) {
            LOG_ERROR_MSG(sparingrep._rc << " " << sparingrep._rt << " " << e.what());
        }
    }
}

int
DBBlockController::processRASMessage(
        RasEvent& rasEvent
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    bool filtered;
    bitset<30> map;

    SQLRETURN sqlrc;
    char sqlstr[512];

    // Skip barrier RAS events since they are never inserted
    if (rasEvent.msgId() != 0x00040096) {
        // Augment RAS event with missing info, if needed, such as ECID and serial number
        BGQDB::STATUS result = BGQDB::augmentRAS(rasEvent);
        if (result != BGQDB::OK) {
            LOG_ERROR_MSG(__FUNCTION__ << " failed to augment RAS with ECID and SN, error: " << result);
        }
    }

    // Let BlockController do its part
    if ((filtered = getBase()->processRASMessage(rasEvent)) == false) {
        const boost::posix_time::ptime start( boost::posix_time::microsec_clock::local_time() );

        // RAS "fast-path" for all except diagnostics-generated RAS events
        if (!getBase()->_diags) {
            if (_sqlstmt == 0) {  // only need to do this once
                int pos = 0;
                _tx.reset(new BGQDB::TxObject(BGQDB::DBConnectionPool::Instance()));
                if ( !_tx->getConnection() ) {
                    LOG_WARN_MSG( "No database connection to insert RAS event: " << rasEvent );
                    return 0;
                }
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

                // Job ID association
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

            // Obtain the time of the RAS event, instead of using the CURRENT TIMESTAMP on the insert
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
            for (
                    std::map<std::string,std::string>::const_iterator pos = rasEvent.getDetails().begin();
                    pos != rasEvent.getDetails().end();
                    ++pos
                )
            {
                if (pos->first == RasEvent::MSG_ID)  {
                    dbe._ind[dbe.MSG_ID]=SQL_NTS;
                    strcpy(dbe._msg_id,pos->second.c_str());
                } else if (pos->first == RasEvent::CATEGORY)  {
                    dbe._ind[dbe.CATEGORY]=SQL_NTS;
                    strcpy(dbe._category,pos->second.c_str());
                } else if (pos->first == RasEvent::COMPONENT)  {
                    dbe._ind[dbe.COMPONENT]=SQL_NTS;
                    strcpy(dbe._component,pos->second.c_str());
                } else if (pos->first == RasEvent::SEVERITY)  {
                    dbe._ind[dbe.SEVERITY]=SQL_NTS;
                    strcpy(dbe._severity,pos->second.c_str());
                } else if (pos->first == RasEvent::MESSAGE)  {
                    dbe._ind[dbe.MESSAGE]=SQL_NTS;
                    if (pos->second.length() >= sizeof(dbe._message)) {
                        LOG_WARN_MSG("Message truncated, length " << pos->second.length() );
                        strncpy(dbe._message,pos->second.c_str(), sizeof(dbe._message)-1);
                        dbe._message[sizeof(dbe._message)-1] = '\0';
                    } else {
                        strcpy(dbe._message,pos->second.c_str());
                    }
                } else if (pos->first == RasEvent::LOCATION)  {
                    dbe._ind[dbe.LOCATION]=SQL_NTS;
                    strcpy(dbe._location,pos->second.c_str());
                } else if (pos->first == RasEvent::CONTROL_ACTION)  {
                    dbe._ind[dbe.CTLACTION]=SQL_NTS;
                    strcpy(dbe._ctlaction,pos->second.c_str());
                } else if (pos->first == RasEvent::COUNT)  {
                    dbe._ind[dbe.COUNT]=0;
                    dbe._count = atoi(pos->second.c_str());
                } else if (pos->first == RasEvent::JOBID)  {
                    dbe._ind[dbe.JOBID]=0;
                    dbe._jobid = atoi(pos->second.c_str());
                } else if (pos->first == RasEvent::ECID)  {
                    if (pos->second.length() > 0) {
                        dbe._ind[dbe.ECID]=32;
                        BGQDB::hexchar2bitdata(const_cast<unsigned char*>(dbe._ecid), sizeof(dbe._ecid), pos->second);
                    }
                } else if (pos->first == RasEvent::CPU)  {
                    dbe._ind[dbe.CPU]=0;
                    dbe._cpu = atoi(pos->second.c_str());
                } else if (pos->first == RasEvent::BLOCKID) {
                    dbe._ind[dbe.BLOCK]=SQL_NTS;
                    _dbj._ind[_dbj.BLOCKID]=SQL_NTS;
                    strcpy(dbe._block,pos->second.c_str());
                    strcpy(_dbj._blockid,dbe._block);
                } else if (pos->first == RasEvent::SERIAL_NUMBER)  {
                    dbe._ind[dbe.SERIALNUMBER]=SQL_NTS;
                    strcpy(dbe._serialnumber,pos->second.c_str());
                } else if (pos->first == RasEvent::COOKIE) {
                    dbe._ind[dbe.QUALIFIER]=SQL_NTS;
                    strcpy(dbe._qualifier,pos->second.c_str());
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
                    LOG_WARN_MSG("Raw data truncated, length " << rawdata.size() );
                }
                (void)strncpy(dbe._rawdata, rawdata.c_str(), sizeof(dbe._rawdata) - 1);
                dbe._rawdata[sizeof(dbe._rawdata) - 1] = '\0';
            }

            // Add neighbor details if necessary
            neighbor::addDetails( this->getBase(), rasEvent, dbe );

            if (dbe._ind[dbe.BLOCK] == SQL_NULL_DATA) {  // Check if the block name came through on the RAS details
                // Provide the block name
                strcpy(dbe._block,getBase()->_blockName.c_str());
                strcpy(_dbj._blockid, dbe._block);
                dbe._ind[dbe.BLOCK]=SQL_NTS;
                _dbj._ind[_dbj.BLOCKID]=SQL_NTS;
            }

            if (dbe._ind[dbe.QUALIFIER] == SQL_NULL_DATA) {
                // include the boot cookie so RAS events for the same boot can be grouped together
                sprintf(dbe._qualifier, "%u",getBase()->_bootCookie);
                dbe._ind[dbe.QUALIFIER]=SQL_NTS;
            }

            // Lookup the job if it was not provided in the RAS event
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
            if (sqlrc != 0) {
                LOG_ERROR_MSG(__FUNCTION__ << " execute failed with " <<  errString(_sqlstmt));
            }

            if ((dbe._jobid == 0) ||
                (dbe._ind[dbe.CTLACTION] == SQL_NTS) ||
                (strcmp(dbe._block,"DefaultControlEventListener" ) == 0)) {
                // Must do the post-processing of this RAS event
                sqlrc =  SQLFetch(_sqlstmt);
                if (sqlrc != 0) {
                    LOG_ERROR_MSG("Fetching recid failed with " <<  errString(_sqlstmt));
                }
            }

            (void)SQLCloseCursor(_sqlstmt);

            // Add total time to insert this RAS event to our total for some bookkeeping
            const boost::posix_time::ptime end( boost::posix_time::microsec_clock::local_time() );
            const boost::posix_time::time_duration duration( end - start );
            _rasInsertionTime += duration;
            ++_rasInsertionCount;
        } else {  // this is diags
            LOG_TRACE_MSG( "Inserting diags" );
            // Write a RAS event log record
            BGQDB::STATUS result = BGQDB::putRAS(
                    getBase()->_blockName,
                    rasEvent.getDetails(),
                    rasEvent.time(),
                    0, // This is diags so there won't be a real jobid.
                    getBase()->_diags
                    );
            if (result != BGQDB::OK) {
                LOG_ERROR_MSG(__FUNCTION__ << " putRAS failed with return code=" << result);
            }
            dbe._recid = 0;
        }
    } else {
        // Nothing was inserted, so no recid to process
        dbe._recid = 0;
    }

    return dbe._recid;
}

void
DBBlockController::abnormalComputeShutdown()
{
    // abnormal compute block shut down leaves the linked I/O nodes in an uknonwn state, changing
    // their status to Software (F)ailure here will require a reboot of them before subsequent
    // compute blocks can be booted
    const cxxdb::ConnectionPtr connection( BGQDB::DBConnectionPool::instance().getConnection() );
    if ( !connection ) {
        LOG_WARN_MSG( "Could not get database connection" );
        return;
    }

    try {
        const cxxdb::QueryStatementPtr query = connection->prepareQuery(
                std::string() +
                "SELECT location from BGQIoNode WHERE status='" + BGQDB::HARDWARE_AVAILABLE + "' AND "
                "location in (SELECT ion from BGQCnIoBlockMap WHERE cnblock=?)",
                boost::assign::list_of("block")
                );
        query->parameters()[ "block" ].set( this->getBlockName() );

        const cxxdb::ResultSetPtr results = query->execute();
        std::vector<std::string> ioNodes;
        while ( results->fetch() ) {
            ioNodes.push_back( results->columns()[ "location" ].getString() );
        }

        const cxxdb::UpdateStatementPtr update = connection->prepareUpdate(
                std::string() +
                "UPDATE BGQIoNode SET status='" + BGQDB::SOFTWARE_FAILURE + "' "
                "WHERE status='" + BGQDB::HARDWARE_AVAILABLE + "' AND " +
                "location=?",
                boost::assign::list_of("location")
                );
        for ( std::vector<std::string>::const_iterator i = ioNodes.begin(); i != ioNodes.end(); ++i ) {
            update->parameters()[ "location" ].set( *i );
            unsigned rows;
            update->execute( &rows );
            if ( rows ) {
                LOG_DEBUG_MSG( "Updated " << *i << " to " << BGQDB::SOFTWARE_FAILURE << " status" );
            }
        }
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }
}

} } // namespace mmcs::server
