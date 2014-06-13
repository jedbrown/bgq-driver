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

#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>

#include "bgsched/HardwareImpl.h"
#include "bgsched/IODrawerImpl.h"
#include "bgsched/IOHardwareImpl.h"
#include "bgsched/IONodeImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/Exception.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>
#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/Log.h>

#include <boost/scoped_ptr.hpp>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace {
// Message strings
const string DB_Access_Error_Str("Error occurred while accessing database.");
const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
const string IODrawers_Not_Found_Str("No I/O drawers found in the database.");
const string Unexpected_Return_Code_Str("Unexpected return code from internal database function.");
const string Unexpected_Exception_Str("Received unexpected exception from internal method.");
} // anonymous namespace

namespace bgsched {

IOHardware::Impl::Impl()
{
    static const string IONodeSQL(

"WITH inUseBlocks AS ( "

" SELECT blockId FROM bgqIoBlock WHERE status <> 'F' "

" ), "

" inUseLocations AS ( "

" SELECT ibm.location, ibm.blockId "
" FROM bgqIoBlockMap AS ibm "
"      JOIN "
"      inUseBlocks AS ub "
"      ON ibm.blockId = ub.blockId "

" ) "

" SELECT ion.location, ion.ioPos AS ioDrawer, ion.status, ion.seqId, ul.blockId AS ioBlock "
" FROM bgqIoNode AS ion "
"      LEFT OUTER JOIN "
"      inUseLocations AS ul "
"      ON ion.location = ul.location OR ion.ioPos = ul.location "
" ORDER BY ion.location "

    );

    IODrawerMap::iterator IODrawerIter;
    cxxdb::ConnectionPtr connectionPtr;
    BGQDB::DBTIodrawer dbIODrawer;
    BGQDB::ColumnsBitmap IODrawerCols;

    // I/O drawer columns to retrieve
    IODrawerCols.set(dbIODrawer.LOCATION);
    IODrawerCols.set(dbIODrawer.STATUS);
    IODrawerCols.set(dbIODrawer.SEQID);

    dbIODrawer.setColumns(IODrawerCols);

    string mapIODrawerLocation;
    string IODrawerLocation;
    string IONodeLocation;
    string IOBlockId;
    Hardware::State state;
    SequenceId seqId;

    ostringstream queryStatement;
    cxxdb::ParameterNames parameterNames;
    cxxdb::ResultSetPtr resultSetPtr;

    //-----------------------------------------------
    //-------------------I/O drawers-----------------
    //-----------------------------------------------
    try {
        // Get database connection
        connectionPtr = BGQDB::DBConnectionPool::Instance().getConnection();

        LOG_DEBUG_MSG("Querying for I/O drawers");
        queryStatement << " ORDER BY LOCATION ASC";
        cxxdb::QueryStatementPtr stmtPtr(dbIODrawer.prepareSelect(*connectionPtr, queryStatement.str(), parameterNames));
        resultSetPtr = stmtPtr->execute();

        while (resultSetPtr->fetch()) {
            // Create the I/O drawer objects and put in a map for fast lookup and vector for fast retrieval
           IODrawer::Pimpl IODrawerPimpl(IODrawer::Impl::createFromDatabase(resultSetPtr->columns()));
           _IODrawerMap[IODrawerPimpl->getLocationString()] = IODrawerPimpl;
           _IODrawers.push_back(IODrawerPimpl);
        }
    } catch (const bgsched::InternalException& e) {
        // just re-throw it
        throw;
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str + " Error text is: " + e.what()
        );
    } catch (const cxxdb::DatabaseException& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                DB_Access_Error_Str + " Error text is: " + e.what()
        );
    } catch (const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    }

    LOG_TRACE_MSG("Got " << _IODrawerMap.size() << " I/O drawers");

    // Make sure we got at least one I/O drawer or system is not correct
    if (_IODrawerMap.size() == 0) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::InconsistentDataError,
                IODrawers_Not_Found_Str
        );
    }

    //-----------------------------------------------
    //-------------------I/O nodes-------------------
    //-----------------------------------------------
    try {
        //LOG_TRACE_MSG("Adding I/O nodes to I/O drawer");
        cxxdb::QueryStatementPtr queryStmtPtr(connectionPtr->prepareQuery(IONodeSQL, parameterNames));
        resultSetPtr = queryStmtPtr->execute();

        // Get the first I/O drawer in the map
        IODrawerIter = _IODrawerMap.begin();
        // Get the I/O drawer location from the map
        mapIODrawerLocation = IODrawerIter->first;

        bool IONodeInUse = false;
        while (resultSetPtr->fetch()) {
            // Get the I/O node info
            IONodeLocation = resultSetPtr->columns()["location"].getString();
            IODrawerLocation = resultSetPtr->columns()["iodrawer"].getString();
            // Check if I/O block or null
            if (resultSetPtr->columns()["ioblock"].isNull()) {
                IOBlockId.clear();
                IONodeInUse = false;
            } else {
                IOBlockId = resultSetPtr->columns()["ioblock"].getString();
                IONodeInUse = true;
            }
            state = Hardware::Impl::convertDatabaseState(resultSetPtr->columns()["status"].getString().c_str());
            seqId = resultSetPtr->columns()["seqid"].as<SequenceId>();

            // Check if current I/O drawer location matches the I/O drawer for the I/O node fetched from the database
            if (mapIODrawerLocation.compare(IODrawerLocation) != 0) {
                // I/O drawer is different so find the I/O drawer in the map
                IODrawerIter = _IODrawerMap.find(IODrawerLocation);
                if (IODrawerIter == _IODrawerMap.end()) {
                    THROW_EXCEPTION(
                            bgsched::InternalException,
                            bgsched::InternalErrors::InconsistentDataError,
                            "I/O drawer " <<  IODrawerLocation << " not found in database for I/O node " << IONodeLocation
                    );
                } else {
                    // Get the I/O drawer location from the map
                    mapIODrawerLocation = IODrawerIter->first;
                }
            }
            // Construct an I/O node object and add it to the associated I/O drawer object
            IONode::Pimpl IONodePimpl(new IONode::Impl(IONodeLocation, state, seqId, IONodeInUse, IOBlockId));
            IODrawerIter->second->addIONode(IONodePimpl);
        }
    } catch (const bgsched::InternalException& e) {
        // just re-throw it
        throw;
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str + " Error text is: " + e.what()
        );
    } catch (const cxxdb::DatabaseException& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                DB_Access_Error_Str + " Error text is: " + e.what()
        );
    } catch (const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    }
}

const vector<IODrawer::Pimpl>&
IOHardware::Impl::getIODrawers() const
{
    return _IODrawers;
}

IODrawer::Pimpl
IOHardware::Impl::getIODrawer(
        const string& location
) const
{
    IODrawerMap::const_iterator iodrawer = _IODrawerMap.find(location);
    if (iodrawer == _IODrawerMap.end()) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Could not find I/O drawer at location " << location
                );
    }
    IODrawer::Pimpl impl = iodrawer->second;
    return impl;
}

IONode::Pimpl
IOHardware::Impl::getIONode(
        const string& location
) const
{
    IONode::Pimpl IONodePimpl;
    // Validate location size
    if (location.length() != 10) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Invalid I/O node location " << location
        );
    }

    // Get the I/O node
    try {
        // Find the I/O drawer the I/O node is in
        IODrawer::Pimpl IODrawerPimpl = getIODrawer(location.substr (0,6));
        vector<IONode::Pimpl> IONodeContainer(IODrawerPimpl->getIONodes());
        // Search the I/O drawer for the I/O node
        for (vector<IONode::Pimpl>::iterator iter = IONodeContainer.begin(); iter!=IONodeContainer.end(); ++iter) {
            // Examine the I/O node to see if one we want
            IONodePimpl = *iter;
            // Do the locations match?
            if (location.compare(IONodePimpl->getLocation()) == 0) {
                return IONodePimpl;
            }
        }
    } catch (const bgsched::InputException& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Could not find I/O node at location " << location
        );
    }
    // If we got this far the I/O node was not found so throw exception
    THROW_EXCEPTION(
            bgsched::InputException,
            bgsched::InputErrors::InvalidLocationString,
            "Could not find I/O node at location " << location
    );
}

IOHardware::Impl::Impl(
        const IOHardware::Pimpl fromIOHardware
        ) :
    _IODrawers(),
    _IODrawerMap()
{
    // Iterate thru the I/O drawer map and copy I/O drawers
    for (IODrawerMap::iterator iter = fromIOHardware->_IODrawerMap.begin(); iter!=fromIOHardware->_IODrawerMap.end(); ++iter) {
        // Get the I/O drawer location from the map
        string location = iter->first;
        // Get the I/O drawer from the map
        IODrawer::Pimpl IODrawerPimpl = iter->second;
        // Make a copy of the I/O drawer object
        IODrawer::Pimpl newIODrawer = IODrawer::Pimpl(new IODrawer::Impl(IODrawerPimpl));
        // Store new I/O drawer in the map using the location as the key
        _IODrawerMap[location] = newIODrawer;
        // Also store I/O drawer in vector
        _IODrawers.push_back(newIODrawer);
    }
}

EnumWrapper<Hardware::State>
IOHardware::Impl::getState(
        const string& location
        ) const
{
    // Validate location size
    if ((location.length() != 10) && (location.length() != 6)) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Invalid hardware location " << location
        );
    }

    // Check if I/O drawer
    if (location.length() == 6) {
        try {
            IODrawer::Pimpl IODrawerPimpl = this->getIODrawer(location);
            return IODrawerPimpl->getState();
        } catch (const bgsched::InputException& e) {
            throw; // Rethrow
        }
    } else {
        try {
            IONode::Pimpl IONodePimpl = this->getIONode(location);
            return IONodePimpl->getState();
        } catch (const bgsched::InputException& e) {
            throw; // Rethrow
        }
    }
}

} // namespace bgsched
