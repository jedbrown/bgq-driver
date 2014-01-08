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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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

#include <bgsched/core/core.h>
#include "bgsched/core/BlockSortImpl.h"
#include "bgsched/core/IOBlockSortImpl.h"
#include "bgsched/core/JobSortImpl.h"

#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/RuntimeException.h>

#include "bgsched/BlockFilterImpl.h"
#include "bgsched/BlockImpl.h"
#include "bgsched/Coordinates.h"
#include "bgsched/ComputeHardwareImpl.h"
#include "bgsched/HardwareImpl.h"
#include "bgsched/IOBlockFilterImpl.h"
#include "bgsched/IOBlockImpl.h"
#include "bgsched/IOHardwareImpl.h"
#include "bgsched/IOLinkImpl.h"
#include "bgsched/JobFilterImpl.h"
#include "bgsched/JobImpl.h"
#include "bgsched/NodeImpl.h"

#include "bgsched/utility.h"

#include <bgq_util/include/Location.h>

#include <control/include/bgqconfig/BGQTopology.h>

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/Exception.h>
#include <db/include/api/genblock.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/filtering/getBlocks.h>
#include <db/include/api/filtering/getJobs.h>

#include <db/include/api/job/types.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>
#include <db/include/api/tableapi/DBConnectionPool.h>

#include <hlcs/include/security/Enforcer.h>
#include <hlcs/include/security/wrapper.h>

#include <utility/include/Log.h>
#include <utility/include/UserId.h>
#include <utility/include/XMLEntity.h>

#include <boost/assign.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>

#include <sstream>
#include <string>
#include <vector>

using namespace boost::assign;
using namespace std;

namespace {
    // Message strings
    const string DB_Access_Error_Str("Error occurred while accessing database.");
    const string Data_Not_Found_Str("Data record(s) not found.");
    const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
    const string Unexpected_Exception_Str("Received unexpected exception from internal method.");
    const string XML_Parse_Error_Str("Error parsing XML data.");
    const string Unexpected_Return_Code_Str("Unexpected return code from internal database function.");
} // anonymous namespace

LOG_DECLARE_FILE("bgsched");

namespace bgsched {
namespace core {

ComputeHardware::ConstPtr
getComputeHardware()
{
    try {
        ComputeHardware::Pimpl impl(new ComputeHardware::Impl());
        return ComputeHardware::ConstPtr(new ComputeHardware(impl));
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str
                );
    }
}

IOHardware::ConstPtr
getIOHardware()
{
    try {
        IOHardware::Pimpl impl(new IOHardware::Impl());
        return IOHardware::ConstPtr(new IOHardware(impl));
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str
                );
    }
}

Coordinates
getMachineSize()
{
    uint32_t a, b, c, d;
    Coordinates coordinates(0,0,0,0);

    //LOG_TRACE_MSG("Calling BGQDB::getMachineBPSize");
    BGQDB::STATUS result = BGQDB::getMachineBPSize(a, b, c, d);
    switch (result) {
        case BGQDB::OK:
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
            );
        case BGQDB::NOT_FOUND:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DataNotFound,
                    Data_Not_Found_Str
            );
        case BGQDB::CONNECTION_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::ConnectionError,
                    DB_Connection_Error_Str
            );
        default:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }
    coordinates[Dimension::A]= a;
    coordinates[Dimension::B]= b;
    coordinates[Dimension::C]= c;
    coordinates[Dimension::D]= d;
    return coordinates;
}

Coordinates
getMidplaneCoordinates(
        const std::string& midplaneLocation
        )
{
    Coordinates midplaneCoordinates(0,0,0,0);
    BGQDB::MidplaneCoordinate mp_coord;
    try {
        BGQDB::torusCoordinateForMidplane(midplaneLocation, mp_coord);
        midplaneCoordinates[Dimension::A] = mp_coord[Dimension::A];
        midplaneCoordinates[Dimension::B] = mp_coord[Dimension::B];
        midplaneCoordinates[Dimension::C] = mp_coord[Dimension::C];
        midplaneCoordinates[Dimension::D] = mp_coord[Dimension::D];
    } catch (const BGQDB::Exception& e) {
        ostringstream os;
        BGQDB::STATUS status = e.getStatus();
        switch (status) {
            case BGQDB::DB_ERROR:
            case BGQDB::CONNECTION_ERROR:
                os << "Unable to get midplane coordinates for midplane location " << midplaneLocation << ". Error is: " << e.what();
                THROW_EXCEPTION(
                        bgsched::DatabaseException,
                        bgsched::DatabaseErrors::DatabaseError,
                        os.str()
                );
            case BGQDB::NOT_FOUND:
                os << "Unable to get midplane coordinates. Midplane location " << midplaneLocation << " was not found.";
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::InvalidLocationString,
                        os.str()
                );
            default:
                THROW_EXCEPTION(
                        bgsched::DatabaseException,
                        bgsched::DatabaseErrors::UnexpectedError,
                        Unexpected_Return_Code_Str
                );
        }
    }
    return midplaneCoordinates;
}

Coordinates
getNodeMidplaneCoordinates(
        const std::string& nodeLocation
        )
{
    try {
        const bgq::util::Location location( nodeLocation );

        if ( location.getType() != bgq::util::Location::ComputeCardOnNodeBoard ) {
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidLocationString,
                    (string() + "Unable to get coordinates for location is not a node, " + nodeLocation + ".")
            );
        }

        const BGQTopology::Coord coordinates = BGQTopology::ABCDEglobalMap[ location.getNodeBoard() ][ location.getComputeCard() ];

        return Coordinates( coordinates.a, coordinates.b, coordinates.c, coordinates.d, coordinates.e );
    } catch ( bgq::util::LocationError &/*e*/ ) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                (string() + "Unable to get coordinates for the node. Node location " + nodeLocation + " was not found.")
        );

    }
}

vector<Node::ConstPtr>
getNodes(
        const string& nodeBoard
        )
{
    BGQDB::DBTNode dbNode;
    BGQDB::ColumnsBitmap nodeCols;

    nodeCols.set(dbNode.MIDPLANEPOS);
    nodeCols.set(dbNode.NODECARDPOS);
    nodeCols.set(dbNode.POSITION);
    nodeCols.set(dbNode.STATUS);
    nodeCols.set(dbNode.SEQID);
    dbNode.setColumns(nodeCols);

    cxxdb::ConnectionPtr conn_ptr;
    try {
        // Get database connection
        conn_ptr = BGQDB::DBConnectionPool::Instance().getConnection();
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }

    // Deconstruct the midplane and node board position for the query
    string midplanePos = nodeBoard.substr(0,6);
    string nodeBoardPos = nodeBoard.substr(7,3);

    string query = "WHERE MIDPLANEPOS = '" + midplanePos + "' AND NODECARDPOS = '" + nodeBoardPos + string("'");
    cxxdb::ResultSetPtr rs_ptr;
    cxxdb::ParameterNames parameter_names;
    cxxdb::QueryStatementPtr stmt_ptr;

    try {
        stmt_ptr = dbNode.prepareSelect(*conn_ptr, query, parameter_names);
        rs_ptr = stmt_ptr->execute();
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    Node::ConstPtrs nodeVector;
    Node::Pimpl nodeImplPtr;
    string location;
    string hardwareState;
    Hardware::State state;
    SequenceId sequenceId;

    while (rs_ptr->fetch()) {
        try {
            // Get the node data
            location = rs_ptr->columns()[BGQDB::DBTNode::MIDPLANEPOS_COL].getString();
            location.append("-");
            location.append(rs_ptr->columns()[BGQDB::DBTNode::NODECARDPOS_COL].getString());
            location.append("-");
            location.append(rs_ptr->columns()[BGQDB::DBTNode::POSITION_COL].getString());
            sequenceId = rs_ptr->columns()[BGQDB::DBVNode::SEQID_COL].as<SequenceId>();
            hardwareState = rs_ptr->columns()[BGQDB::DBVNode::STATUS_COL].getString();
            state = Hardware::Impl::convertDatabaseState(hardwareState.c_str());

            // Construct the node object from node data
            nodeImplPtr.reset(new Node::Impl(location,state,sequenceId));
            Node::Ptr nodePtr(new Node(nodeImplPtr));
            nodeVector.push_back(nodePtr);
        } catch (const BGQDB::Exception& e) {
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str + " Error text is: " + e.what()
            );
        } catch (const cxxdb::DatabaseException& e) {
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
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

    // Check if we got any nodes for the node board
    if (nodeVector.size() == 0) {
        ostringstream os;
        os << "Unable to get nodes. Node board location " << nodeBoard << " was not found.";
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                os.str()
        );
    }

    // Check if we got 32 nodes for the node board
    if (nodeVector.size() != NodeBoard::NodeCount) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::InconsistentDataError,
                "Number of nodes found, " << nodeVector.size() << " , is not equal to " << NodeBoard::NodeCount
        );
    }

    return nodeVector;
}

vector<Node::ConstPtr>
getMidplaneNodes(
        const std::string& midplane   //!< [in] Midplane location to get nodes for
        )
{
    BGQDB::DBTNode dbNode;
    BGQDB::ColumnsBitmap nodeCols;

    nodeCols.set(dbNode.MIDPLANEPOS);
    nodeCols.set(dbNode.NODECARDPOS);
    nodeCols.set(dbNode.POSITION);
    nodeCols.set(dbNode.STATUS);
    nodeCols.set(dbNode.SEQID);
    dbNode.setColumns(nodeCols);

    cxxdb::ConnectionPtr conn_ptr;
    try {
        // Get database connection
        conn_ptr = BGQDB::DBConnectionPool::Instance().getConnection();
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::ConnectionError,
                DB_Connection_Error_Str
        );
    }

    string query = "WHERE MIDPLANEPOS = '" + midplane + string("'");
    cxxdb::ResultSetPtr rs_ptr;
    cxxdb::ParameterNames parameter_names;
    cxxdb::QueryStatementPtr stmt_ptr;

    try {
        stmt_ptr = dbNode.prepareSelect(*conn_ptr, query, parameter_names);
        rs_ptr = stmt_ptr->execute();
    } catch (...) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                DB_Access_Error_Str
        );
    }

    Node::ConstPtrs nodeVector;
    Node::Pimpl nodeImplPtr;
    string location;
    string hardwareState;
    Hardware::State state;
    SequenceId sequenceId;

    while (rs_ptr->fetch()) {
        try {
            // Get the node data
            location = rs_ptr->columns()[BGQDB::DBTNode::MIDPLANEPOS_COL].getString();
            location.append("-");
            location.append(rs_ptr->columns()[BGQDB::DBTNode::NODECARDPOS_COL].getString());
            location.append("-");
            location.append(rs_ptr->columns()[BGQDB::DBTNode::POSITION_COL].getString());
            sequenceId = rs_ptr->columns()[BGQDB::DBTNode::SEQID_COL].as<SequenceId>();
            hardwareState = rs_ptr->columns()[BGQDB::DBTNode::STATUS_COL].getString();
            state = Hardware::Impl::convertDatabaseState(hardwareState.c_str());

            // Construct the node object from node data
            nodeImplPtr.reset(new Node::Impl(location,state,sequenceId));
            Node::Ptr nodePtr(new Node(nodeImplPtr));
            nodeVector.push_back(nodePtr);
        } catch (const BGQDB::Exception& e) {
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str + " Error text is: " + e.what()
            );
        } catch (const cxxdb::DatabaseException& e) {
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
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

    // Check if we got any nodes for the node board
    if (nodeVector.size() == 0) {
        ostringstream os;
        os << "Unable to get nodes. Midplane location " << midplane << " was not found.";
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                os.str()
        );
    }

    // Check if we got 512 nodes for the midplane
    if (nodeVector.size() != Midplane::NodeCount) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::InconsistentDataError,
                "Number of nodes found, " << nodeVector.size() << " , is not equal to " << Midplane::NodeCount
        );
    }

    return nodeVector;
}

vector<NodeBoard::ConstPtr>
getNodeBoards(
        const string& midplane
        )
{
    NodeBoard::ConstPtrs nodeBoardVector;
    NodeBoard::Pimpl nodeBoardImplPtr;
    stringstream nodeBoardsXML;
    vector<const XMLEntity *> XMLEntityPtrVector;
    vector<const XMLEntity *> subEntities;

    //LOG_TRACE_MSG("Calling BGQDB::getBPNodeCards for midplane: " << midplane);
    BGQDB::STATUS result = BGQDB::getBPNodeCards(nodeBoardsXML, midplane);
    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            // XMLEntity::dumpXML(nodeBoardsXML, "nodeboards", false);
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
            );
        case BGQDB::CONNECTION_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::ConnectionError,
                    DB_Connection_Error_Str
            );
        default :
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    // Parse the node board XML file
    try {
        boost::scoped_ptr<XMLEntity> XMLEntityPtr( XMLEntity::readXML(nodeBoardsXML) );
        XMLEntityPtrVector = XMLEntityPtr->subentities();
        // Read all node boards from XML file to vector
        subEntities = XMLEntityPtrVector[0]->subentities();
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQNodeCard") {
                // Build up the location, example R01-M0-N00
                string location(midplane);
                location.append("-");
                location.append((*iter)->attrByName("NodeCardId"));
                // Construct the node board object from XML stream
                nodeBoardImplPtr.reset(new NodeBoard::Impl(location,*iter));
                NodeBoard::Ptr nodeBoardPtr(new NodeBoard(nodeBoardImplPtr));
                nodeBoardVector.push_back(nodeBoardPtr);
            } else {
                // Force XML dump to log
                bgsched::logXML(nodeBoardsXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQNodeCard tag."
                );
            }
        }
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(nodeBoardsXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    // Check if we got 16 node boards for the midplane
    if (nodeBoardVector.size() != Midplane::MaxNodeBoards) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::InconsistentDataError,
                "Number of node boards found, " << nodeBoardVector.size() << " , is not equal to " <<  Midplane::MaxNodeBoards
        );
    }

    return nodeBoardVector;
}

vector<IOLink::ConstPtr>
getIOLinks(
        const string& midplane)
{
    vector<IOLink::ConstPtr> IOLinkVector;
    IOLink::Pimpl IOLinkImplPtr;
    stringstream IOLinksXML;
    vector<const XMLEntity *> XMLEntityPtrVector;
    vector<const XMLEntity *> subEntities;
    BGQDB::MidplaneCoordinate mp_coord;
    ostringstream os;
    BGQDB::STATUS status;

    /*
    if (midplane.empty()) {
        LOG_TRACE_MSG("Calling BGQDB::getIOWireList for all midplanes");
    } else {
        LOG_TRACE_MSG("Calling BGQDB::getIOWireList for midplane: " << midplane);
    }
    */
    BGQDB::STATUS result = BGQDB::getIOWireList(IOLinksXML, midplane);
    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            // XMLEntity::dumpXML(IOLinksXML, "IOLink", false);
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
            );
        case BGQDB::NOT_FOUND: // I/O links not found - could be bad DB or bad midplane name
            // Check if midplane name is valid if using single midplane location
            if (midplane != "*ALL") {
                try {
                    BGQDB::torusCoordinateForMidplane(midplane, mp_coord);
                } catch (const BGQDB::Exception& e) {
                    status = e.getStatus();
                    switch (status) {
                       case BGQDB::DB_ERROR:
                       case BGQDB::CONNECTION_ERROR:
                           THROW_EXCEPTION(
                                   bgsched::DatabaseException,
                                   bgsched::DatabaseErrors::DatabaseError,
                                   DB_Access_Error_Str
                           );
                       case BGQDB::NOT_FOUND:
                           os << "Midplane location " << midplane << " was not found.";
                           THROW_EXCEPTION(
                                   bgsched::InputException,
                                   bgsched::InputErrors::InvalidLocationString,
                                   os.str()
                           );
                       default:
                           break;
                       }
                }
                // If no exception is thrown that means the DB is bad since the midplane location is valid
                os << "No I/O links found for midplane " << midplane << " in database";
                THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DataNotFound,
                    os.str()
                );
            }
            // Couldn't find I/O links across entire machine
            os << "No I/O links found in database for all midplanes";
            THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DataNotFound,
                os.str()
            );

        case BGQDB::CONNECTION_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::ConnectionError,
                    DB_Connection_Error_Str
            );
        default :
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    // Parse the I/O links XML file
    try {
        boost::scoped_ptr<XMLEntity> XMLEntityPtr( XMLEntity::readXML(IOLinksXML) );
        XMLEntityPtrVector = XMLEntityPtr->subentities();
        // Read all IO links from XML file to vector
        subEntities = XMLEntityPtrVector[0]->subentities();
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQIOWire") {
                // Get location for I/O link constructor
                string location((*iter)->attrByName("source"));
                // Construct the I/O link object from XML stream
                IOLinkImplPtr.reset(new IOLink::Impl(location,*iter));
                IOLink::Ptr IOLinkPtr(new IOLink(IOLinkImplPtr));
                IOLinkVector.push_back(IOLinkPtr);
            } else {
                // Force XML dump to log
                bgsched::logXML(IOLinksXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQIOWire tag."
                );
            }
        }
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(IOLinksXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    return IOLinkVector;
}

vector<IOLink::ConstPtr>
getAvailableIOLinks(
        const string& midplane)
{
    vector<IOLink::ConstPtr> IOLinkVector;
    IOLink::Pimpl IOLinkImplPtr;
    stringstream IOLinksXML;
    vector<const XMLEntity *> XMLEntityPtrVector;
    vector<const XMLEntity *> subEntities;
    BGQDB::MidplaneCoordinate mp_coord;
    ostringstream os;
    BGQDB::STATUS status;

    /*
    if (midplane.empty()) {
        LOG_TRACE_MSG("Calling BGQDB::getIOWireList for all midplanes");
    } else {
        LOG_TRACE_MSG("Calling BGQDB::getIOWireList for midplane: " << midplane);
    }
    */
    BGQDB::STATUS result = BGQDB::getIOWireList(IOLinksXML, midplane);
    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            // XMLEntity::dumpXML(IOLinksXML, "IOLink", false);
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
            );
        case BGQDB::NOT_FOUND: // I/O links not found - could be bad DB or bad midplane name
            // Check if midplane name is valid if using single midplane location
            if (midplane != "*ALL") {
                try {
                    BGQDB::torusCoordinateForMidplane(midplane, mp_coord);
                } catch (const BGQDB::Exception& e) {
                    status = e.getStatus();
                    switch (status) {
                       case BGQDB::DB_ERROR:
                       case BGQDB::CONNECTION_ERROR:
                           THROW_EXCEPTION(
                                   bgsched::DatabaseException,
                                   bgsched::DatabaseErrors::DatabaseError,
                                   DB_Access_Error_Str
                           );
                       case BGQDB::NOT_FOUND:
                           os << "Midplane location " << midplane << " was not found.";
                           THROW_EXCEPTION(
                                   bgsched::InputException,
                                   bgsched::InputErrors::InvalidLocationString,
                                   os.str()
                           );
                       default:
                           break;
                       }
                }
                // If no exception is thrown that means the DB is bad since the midplane location is valid
                os << "No I/O links found for midplane " << midplane << " in database";
                THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DataNotFound,
                    os.str()
                );
            }
            // Couldn't find I/O links across entire machine
            os << "No I/O links found in database for all midplanes";
            THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DataNotFound,
                os.str()
            );

        case BGQDB::CONNECTION_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::ConnectionError,
                    DB_Connection_Error_Str
            );
        default :
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    // Parse the I/O links XML file
    try {
        boost::scoped_ptr<XMLEntity> XMLEntityPtr( XMLEntity::readXML(IOLinksXML) );
        XMLEntityPtrVector = XMLEntityPtr->subentities();
        // Read all IO links from XML file to vector
        subEntities = XMLEntityPtrVector[0]->subentities();
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQIOWire") {
                // Get location for I/O link constructor
                string location((*iter)->attrByName("source"));
                // Construct the I/O link object from XML stream
                IOLinkImplPtr.reset(new IOLink::Impl(location,*iter));
                // Add the I/O link if both the I/O link hardware state and destination I/O node are available
                if ((IOLinkImplPtr->getState() == Hardware::Available) && (IOLinkImplPtr->getIONodeState() == Hardware::Available)) {
                    IOLink::Ptr IOLinkPtr(new IOLink(IOLinkImplPtr));
                    IOLinkVector.push_back(IOLinkPtr);
                }
            } else {
                // Force XML dump to log
                bgsched::logXML(IOLinksXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQIOWire tag."
                );
            }
        }
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(IOLinksXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    return IOLinkVector;
}

Block::Ptrs
getBlocks(
        const bgsched::BlockFilter& filter,
        const bgsched::core::BlockSort& sort,
        const string& user
        )
{
    Block::Ptrs blockVector;
    cxxdb::ConnectionPtr conn_ptr;
    cxxdb::ResultSetPtr rs_ptr;
    cxxdb::QueryStatementPtr job_stmt_ptr;

    boost::scoped_ptr<hlcs::security::Enforcer> enforcer_ptr;
    boost::scoped_ptr<bgq::utility::UserId> userid_ptr;

    try {
        // Get database connections
        conn_ptr = BGQDB::DBConnectionPool::Instance().getConnection();

        // An empty user string means no security filtering will be performed and all results will be returned.
        bool checkUserAuthority = true;

        if (user.empty()) {
            checkUserAuthority = false;
        } else {
            // Get properties set on bgsched::init()
            bgq::utility::Properties::Ptr properties(bgsched::getProperties());
            enforcer_ptr.reset(new hlcs::security::Enforcer(properties, conn_ptr));
            // Build user id with allow remote user indicator
            userid_ptr.reset(new bgq::utility::UserId(user,true));
            LOG_TRACE_MSG("Performing security filtering on compute blocks for user " << user);
        }

        // Convert from bgsched::BlockFilter format to BGQDB::filtering::BlockFilter format
        bgsched::BlockFilter::Pimpl blockFilterPimpl = filter.getPimpl();
        BGQDB::filtering::BlockFilter dbBlockFilter(blockFilterPimpl->convertToDBBlockFilter());

        if (sort.isSorted()) {
            // Convert from bgsched::core::BlockSort format to BGQDB::filtering::BlockSort format
            bgsched::core::BlockSort::Pimpl blockSortPimpl = sort.getPimpl();
            BGQDB::filtering::BlockSort dbBlockSort = blockSortPimpl->convertToDBBlockSort();
            rs_ptr = BGQDB::filtering::getBlocks(dbBlockFilter, dbBlockSort, *conn_ptr);
        } else {
            rs_ptr = BGQDB::filtering::getBlocks(dbBlockFilter, BGQDB::filtering::BlockSort::AnyOrder, *conn_ptr);
        }

        bool includeBlock = true;

        while (rs_ptr->fetch()) {
            // Assume compute block should be included
            includeBlock = true;

            // See if security check for read authority is needed
            if (checkUserAuthority) {
                // Call the enforcer to make sure the user has read authority to compute block
                if (! enforcer_ptr->validate(
                        hlcs::security::Object(
                                hlcs::security::Object::Block,
                                rs_ptr->columns()[BGQDB::DBVComputeblock::BLOCKID_COL].getString()
                        ),
                        hlcs::security::Action::Read,
                        *userid_ptr))
                {
                    includeBlock = false;
                }
            }

            // Check if compute block should be included in returned container of compute blocks
            if (includeBlock) {
                Block::Pimpl blockImplPtr(Block::Impl::createFromDatabase(rs_ptr->columns(), filter.getExtendedInfo(), *conn_ptr));

                // Determine if jobs should be collected
                if (filter.getIncludeJobs()) {
                    if (! job_stmt_ptr) {
                        BGQDB::DBTJob dbJob;
                        BGQDB::ColumnsBitmap job_cols;
                        job_cols.set(dbJob.ID);
                        dbJob.setColumns(job_cols);

                        job_stmt_ptr = dbJob.prepareSelect(
                                *conn_ptr,
                                string() + "WHERE " + BGQDB::DBTJob::BLOCKID_COL + "=? ORDER BY " + BGQDB::DBTJob::ID_COL + " ASC",
                                list_of( "blockId" )
                        );
                    }

                    // Get job ID of jobs, if any, using this compute block
                    LOG_TRACE_MSG("Querying for job ids for " << blockImplPtr->getName() );

                    job_stmt_ptr->parameters()[BGQDB::DBTJob::BLOCKID_COL].set( blockImplPtr->getName() );

                    cxxdb::ResultSetPtr job_rs_ptr(job_stmt_ptr->execute());

                    while ( job_rs_ptr->fetch() ) {
                        blockImplPtr->addJobId(job_rs_ptr->columns()[BGQDB::DBTJob::ID_COL].as<Job::Id>());
                    }
                }

                blockVector.push_back(Block::Ptr(new Block(blockImplPtr)));
            }
        }

        LOG_TRACE_MSG("Got " << blockVector.size() << " compute blocks");
    } catch (const bgsched::InternalException& e) {
        // just re-throw it
        throw;
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const cxxdb::DatabaseException& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const std::runtime_error& e) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                e.what()
        );
    } catch (const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    }
    return blockVector;
}

IOBlock::Ptrs
getIOBlocks(
        const bgsched::IOBlockFilter& filter,
        const bgsched::core::IOBlockSort& sort,
        const string& user
        )
{
    IOBlock::Ptrs IOBlockVector;
    cxxdb::ConnectionPtr conn_ptr;
    cxxdb::ResultSetPtr rs_ptr;
    cxxdb::QueryStatementPtr job_stmt_ptr;

    boost::scoped_ptr<hlcs::security::Enforcer> enforcer_ptr;
    boost::scoped_ptr<bgq::utility::UserId> userid_ptr;

    try {
        // Get database connections
        conn_ptr = BGQDB::DBConnectionPool::Instance().getConnection();

        // An empty user string means no security filtering will be performed and all results will be returned.
        bool checkUserAuthority = true;

        if (user.empty()) {
            checkUserAuthority = false;
        } else {
            // Get properties set on bgsched::init()
            bgq::utility::Properties::Ptr properties(bgsched::getProperties());
            enforcer_ptr.reset(new hlcs::security::Enforcer(properties, conn_ptr));
            // Build user id with allow remote user indicator
            userid_ptr.reset(new bgq::utility::UserId(user,true));
            LOG_TRACE_MSG("Performing security filtering on I/O blocks for user " << user);
        }

        // Convert from bgsched::IOBlockFilter format to BGQDB::filtering::BlockFilter format
        bgsched::IOBlockFilter::Pimpl IOBlockFilterPimpl = filter.getPimpl();
        BGQDB::filtering::BlockFilter dbIOBlockFilter(IOBlockFilterPimpl->convertToDBBlockFilter());

        if (sort.isSorted()) {
            // Convert from bgsched::core::IOBlockSort format to BGQDB::filtering::BlockSort format
            bgsched::core::IOBlockSort::Pimpl IOBlockSortPimpl = sort.getPimpl();
            BGQDB::filtering::BlockSort dbIOBlockSort = IOBlockSortPimpl->convertToDBBlockSort();
            rs_ptr = BGQDB::filtering::getBlocks(dbIOBlockFilter, dbIOBlockSort, *conn_ptr);
        } else {
            rs_ptr = BGQDB::filtering::getBlocks(dbIOBlockFilter, BGQDB::filtering::BlockSort::AnyOrder, *conn_ptr);
        }

        bool includeBlock = true;

        while (rs_ptr->fetch()) {
            // Assume I/O block should be included
            includeBlock = true;

            // See if security check for read authority is needed
            if (checkUserAuthority) {
                // Call the enforcer to make sure the user has read authority to I/O block
                if (! enforcer_ptr->validate(
                        hlcs::security::Object(
                                hlcs::security::Object::Block,
                                rs_ptr->columns()[BGQDB::DBVIoblock::BLOCKID_COL].getString()
                        ),
                        hlcs::security::Action::Read,
                        *userid_ptr))
                {
                    includeBlock = false;
                }
            }

            // Check if block should be included in returned container of blocks
            if (includeBlock) {
                IOBlock::Pimpl IOBlockImplPtr(IOBlock::Impl::createFromDatabase(rs_ptr->columns(), filter.getExtendedInfo(), *conn_ptr));
                IOBlockVector.push_back(IOBlock::Ptr(new IOBlock(IOBlockImplPtr)));
            }
        }

        LOG_TRACE_MSG("Got " << IOBlockVector.size() << " I/O blocks");
    } catch (const bgsched::InternalException& e) {
        // just re-throw it
        throw;
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const cxxdb::DatabaseException& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const std::runtime_error& e) {
        THROW_EXCEPTION(
                bgsched::RuntimeException,
                bgsched::RuntimeErrors::AuthorityError,
                e.what()
        );
    } catch (const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    }
    return IOBlockVector;
}

Job::ConstPtrs
getJobs(
        const bgsched::JobFilter& filter,
        const bgsched::core::JobSort& sort,
        const string& user
        )
{
    Job::ConstPtrs jobVector;
    cxxdb::ConnectionPtr conn_ptr;
    cxxdb::ResultSetPtr rs_ptr;

    try {
        // Get properties set on bgsched::init()
        bgq::utility::Properties::Ptr properties(bgsched::getProperties());
        // Need to pass database connection to getJobs() security API
        conn_ptr = BGQDB::DBConnectionPool::Instance().getConnection();
        // Convert from bgsched::JobFilter format to BGQDB::filtering::JobFilter format
        bgsched::JobFilter::Pimpl jobFilterPimpl = filter.getPimpl();
        BGQDB::filtering::JobFilter dbJobFilter(jobFilterPimpl->convertToDBJobFilter());

        //LOG_INFO_MSG("About to call database::getJobs()");
        if (sort.isSorted()) {
            // Convert from bgsched::core::JobSort format to BGQDB::filtering::JobSort format
            bgsched::core::JobSort::Pimpl jobSortPimpl = sort.getPimpl();
            BGQDB::filtering::JobSort dbJobSort = jobSortPimpl->convertToDBJobSort();
            hlcs::security::getJobs(dbJobFilter, dbJobSort, conn_ptr, properties, 0, 0, NULL, &rs_ptr, user);
        } else {
            hlcs::security::getJobs(dbJobFilter, BGQDB::filtering::JobSort::AnyOrder, conn_ptr, properties, 0, 0, NULL, &rs_ptr, user);
        }
        //LOG_INFO_MSG("Completed call to database::getJobs()");


        if (rs_ptr != NULL) {
            while (rs_ptr->fetch()) {
                Job::Pimpl jobImplPtr(Job::Impl::createFromDatabase(rs_ptr->columns()));
                jobVector.push_back(Job::Ptr(new Job(jobImplPtr)));
            }
        }
        LOG_TRACE_MSG("Got " << jobVector.size() << " jobs matching job filter");
    } catch (const bgsched::InternalException& e) {
        // just re-throw it
        throw;
    } catch (const BGQDB::Exception& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::DatabaseError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const cxxdb::DatabaseException& e) {
        THROW_EXCEPTION(
                bgsched::DatabaseException,
                bgsched::DatabaseErrors::OperationFailed,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const std::invalid_argument& e) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    } catch (const std::exception& e) {
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::UnexpectedError,
                Unexpected_Exception_Str + " Error text is: " + e.what()
        );
    }
    return jobVector;
}


} // namespace bgsched::core
} // namespace bgsched
