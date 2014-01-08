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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "CreateNodeConfig.h"

#include <bgq_util/include/string_tokenizer.h>
#include <db/include/api/tableapi/gensrc/DBTNodeconfig.h>
#include <db/include/api/tableapi/gensrc/DBTDomainmap.h>
#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/TxObject.h>
#include <utility/include/Log.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <sstream>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace command {

namespace {

BGQDB::STATUS
impl(
        const std::deque<std::string>& args
    )
{
    using namespace BGQDB;
    const long long int GB = 1024 * 1024 * 1024;
    const long long int Cust64K = 64 * 1024;
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
    for (std::deque<std::string>::const_iterator i = args.begin(); i != args.end(); ++i) {
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
                    std::istringstream sa(starting);
                    if (!(sa>>std::hex>>dm._startaddr) || !(boost::algorithm::all(starting, boost::algorithm::is_xdigit())) ) {
                        LOG_ERROR_MSG(__FUNCTION__ << " invalid hex value for starting memory address: " << starting);
                        return INVALID_ARG;
                    }
                    if ( ending == "-1" ) {
                        // special case for sentinel value, it isn't hexadecimal
                        dm._endaddr = -1;
                    } else {
                        std::istringstream ea(ending);
                        if (!(ea>>std::hex>>dm._endaddr) || !(boost::algorithm::all( ending, boost::algorithm::is_xdigit())) ) {
                            LOG_ERROR_MSG(__FUNCTION__ << " invalid hex value for ending memory address: " << ending);
                            return INVALID_ARG;
                        }
                    }
                } else if ( !tokens[tok].compare(0,9,"custaddr=") ) {
                    dmBitmap.set(dm.CUSTADDR);
                    std::istringstream ca(tokens[tok].substr(9));
                    if (!(ca>>std::hex>>dm._custaddr) || !(boost::algorithm::all( tokens[tok].substr(9), boost::algorithm::is_xdigit())) ) {
                        LOG_ERROR_MSG(__FUNCTION__ << " invalid hex value for customization address: " << tokens[tok].substr(9) );
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

} // anonymous namespace 

CreateNodeConfig*
CreateNodeConfig::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new CreateNodeConfig("create_node_config", "create_node_config <configname> [<nodeoptions>] <domaininfo>", commandAttributes);
}

void
CreateNodeConfig::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
CreateNodeConfig::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    const BGQDB::STATUS result( impl(args) );

    switch (result) {
        case BGQDB::OK:
            reply << mmcs_client::OK << "Node config created" << mmcs_client::DONE;
            break;
        case BGQDB::DUPLICATE:
            reply << mmcs_client::FAIL << "Node config " << args[0] << " already exists" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << mmcs_client::FAIL << "Node config name " << args[0] << " invalid or too big" << mmcs_client::DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << mmcs_client::FAIL << "Unable to connect to database" << mmcs_client::DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << mmcs_client::FAIL << "Database failure or duplicate name" << mmcs_client::DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << mmcs_client::FAIL << "Invalid arguments provided, or images missing" << mmcs_client::DONE;
            break;
        case BGQDB::FAILED:
            reply << mmcs_client::FAIL << "Invalid node configuration" << mmcs_client::DONE;
            break;
        default:
            reply << mmcs_client::FAIL << "Unexpected return code from BGQDB::createNodeConfig : " << result << mmcs_client::DONE;
            break;
        }
}

void
CreateNodeConfig::help(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Create a node configuration for booting a block."
          << ";Optionally specify <nodeoptions> in the format:  nodeoptions={option option}"
          << ";For <domaininfo> specify one or more domain specifications, where each is of the format:"
          << ";  domain={cores=startcore-endcore memory=startaddr-endaddr images=imagename,imagename,..  id=domainid custaddr=addr options=option option} "
          << ";Multiple domains can be specified, but they cannot overlap cores or memory."
          << ";Images are required."
          << ";Memory addresses must be specified in hexadecimal format without 0x prefix, the ending address can be -1"
          << ";  to use all available memory."
          << ";Cores can range from 0 to 16."
          << ";Cores and memory can be omitted for a single-domain node config, and will default to all cores and all memory."
          << ";Memory can also be omitted for a multi-domain node config, and will default to 1GB per core."
          << ";Optionally specify custaddr to provide an address for the node-specific customizations, which come from the database."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
