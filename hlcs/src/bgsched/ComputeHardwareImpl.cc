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

#include <bgsched/Coordinates.h>
#include <bgsched/DatabaseException.h>
#include <bgsched/InputException.h>
#include <bgsched/InternalException.h>
#include <bgsched/RuntimeException.h>

#include "bgsched/CableImpl.h"
#include "bgsched/ComputeHardwareImpl.h"
#include "bgsched/SwitchSettingsImpl.h"
#include "bgsched/utility.h"

#include <db/include/api/BGQDBlib.h>

#include <bgq_util/include/Location.h>

#include <utility/include/Log.h>
#include <utility/include/XMLEntity.h>

#include <boost/scoped_ptr.hpp>

#include <iomanip>
#include <iostream>
#include <vector>

LOG_DECLARE_FILE("bgsched");

using namespace bgsched;
using namespace std;

namespace {
// Message strings
const string DB_Access_Error_Str("Error occurred while accessing database.");
const string DB_Connection_Error_Str("Communication error occurred while attempting to connect to database.");
const string Data_Not_Found_Str("Data record(s) not found.");
const string Midplanes_Without_Cables_Str("Midplanes cannot exist without cables.");
const string Midplane_Without_Switches_Str("Multiple midplanes exist, but there are no switches.");
const string Unexpected_Return_Code_Str("Unexpected return code from internal database function.");
const string XML_Parse_Error_Str("Error parsing XML data.");
const string Switch_Without_Midplane_Str("Switch exists without associated midplane.");
const string Cables_Without_Switches_Str("Cables exist, but there are no switches.");
const string Cable_Without_Switch_Str("Cable exists without associated switch.");
const string Unexpected_Exception_Str("Received unexpected exception from internal method.");

Dimension::Value locationDimensionToBgschedDimension(
        bgq::util::Location::Dimension::Value ld
        )
{
    return (ld == bgq::util::Location::Dimension::A ? Dimension::A :
            ld == bgq::util::Location::Dimension::B ? Dimension::B :
            ld == bgq::util::Location::Dimension::C ? Dimension::C :
            Dimension::D);
}
} // anonymous namespace

namespace bgsched {

ComputeHardware::Impl::Impl( ) :
    _midplanes(),
    _midplaneMap(),
    _extent(0,0,0,0),
    _rows(0),
    _columns(0)
{
    try {
        initializeDatabase( );
    } catch (...) {
        // Rethrow any exceptions
        throw;
    }
}

void
ComputeHardware::Impl::initializeDatabase( )
{
    BGQDB::STATUS result;
    MidplaneMap::iterator midplaneIter;
    stringstream midplanesXML, nodeBoardsXML, cablesXML, switchesXML;
    vector<const XMLEntity*> XMLEntities;
    vector<const XMLEntity*> subEntities, subSubEntities;
    Coordinates coords(0,0,0,0);
    uint32_t nodeBoardCount = 0;
    uint32_t switchCount = 0;
    uint32_t cableCount = 0;
    uint32_t rows = 0;
    uint32_t columns = 0;
    uint32_t A_Dim, B_Dim, C_Dim, D_Dim;
    bool XMLDump = false;

    // Environmental to dump XML, note value does not matter, as long as it's not NULL
    if (getenv("BG_DUMP_XML") == NULL) {
        XMLDump = false;
    } else {
        XMLDump = true;
    }

    //LOG_TRACE_MSG("Calling BGQDB::getMachineBPSize()");
    result = BGQDB::getMachineBPSize(A_Dim, B_Dim, C_Dim, D_Dim);
    switch (result) {
        case BGQDB::OK:
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
                    );
            break;
        case BGQDB::NOT_FOUND: // Serial number is bad
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DataNotFound,
                    Data_Not_Found_Str
                    );
            break;
        case BGQDB::CONNECTION_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::ConnectionError,
                    DB_Connection_Error_Str
                    );
            break;
        default:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
                    );
    }

    LOG_TRACE_MSG("Machine dimension: A:" << A_Dim << " B:" << B_Dim << " C:" << C_Dim << " D:" << D_Dim);

    // Set the midplane multi-dimensional array based on A, B, C, D dimensions
    _midplanes.resize(boost::extents[A_Dim][B_Dim][C_Dim][D_Dim]);

    //-----------------------------------------------
    //---------------------Midplanes-----------------
    //-----------------------------------------------
    //LOG_TRACE_MSG("Calling BGQDB::getBPs()");
    result = BGQDB::getBPs(midplanesXML);
    //LOG_TRACE_MSG("Done calling BGQDB::getBPs()");
    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            if (XMLDump) {
                XMLEntity::dumpXML(midplanesXML, "midplanes", false);
            }
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
            );
        case BGQDB::NOT_FOUND: // Midplane table is empty (empty XML file created)
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
        default :
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::UnexpectedError,
                    Unexpected_Return_Code_Str
            );
    }

    // Parse the midplane XML file and build midplane objects
    try {
        boost::scoped_ptr<XMLEntity> xmlEntity(XMLEntity::readXML(midplanesXML));
        XMLEntities = xmlEntity->subentities();
        // Read all midplanes from XML file to container
        subEntities = XMLEntities[0]->subentities();
        for (vector<const XMLEntity*>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQBP") {
                // Get location for midplane constructor
                string location((*iter)->attrByName("BPId"));
                // Construct the midplane object from XML stream
                Midplane::Pimpl midplane(new Midplane::Impl(location, *iter));
                // Determine max row/column for setting hardware configuration
                bgq::util::Location midplaneLocation = midplane->getLocation();
                if (midplaneLocation.getRackRow() > rows) {
                    rows = midplaneLocation.getRackRow();
                }
                if (midplaneLocation.getRackColumn() > columns) {
                    columns = midplaneLocation.getRackColumn();
                }

                coords = midplane->getCoordinates();
                _midplaneMap[location] = midplane;
                _midplanes [coords[Dimension::A]] [coords[Dimension::B]] [coords[Dimension::C]] [coords[Dimension::D]]= midplane;
            } else {
                // Force XML dump to log
                bgsched::logXML(midplanesXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQBP tag."
                );
            }
        }
        // Set the hardware rows/columns (need to bump by 1)
        _rows = rows + 1;
        _columns = columns + 1;
        LOG_DEBUG_MSG("Blue Gene machine configuration: " << _rows << " row(s)  X  " << _columns << " column(s)");
        LOG_TRACE_MSG("Got " << _midplaneMap.size() << " midplanes");

        // Set machine extent
        for ( Dimension e = Dimension::A; e <= Dimension::D; ++e ) {
            _extent[e] = _midplanes.shape()[e] - 1;
        }
    // Handle midplane exception
    } catch (const bgsched::InternalException& e) {
        // Force XML dump to log
        bgsched::logXML(midplanesXML, true);
        LOG_ERROR_MSG("Exception in " << __FUNCTION__ << " is " << e.what());
        // Rethrow the exception
        throw;
    // Handle XML exception
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(midplanesXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    //-----------------------------------------------
    //-------------------Node boards-----------------
    //-----------------------------------------------
    // Iterate thru the midplane map and add node boards
    //LOG_TRACE_MSG("Adding node boards to midplanes");
    for (midplaneIter = _midplaneMap.begin(); midplaneIter!=_midplaneMap.end(); ++midplaneIter) {
        // Get the midplane location from the map
        string midplane = midplaneIter->first;

        // Clear out any previous entry
        nodeBoardsXML.str("");
        nodeBoardsXML.clear();

        //LOG_TRACE_MSG("Calling BGQDB::getBPNodeCards() for midplane: " << midplane);
        BGQDB::STATUS result = BGQDB::getBPNodeCards(nodeBoardsXML, midplane);
        //LOG_TRACE_MSG("Done calling BGQDB::getBPNodeCards() for midplane: " << midplane);
        switch (result) {
            case BGQDB::OK:
                // Dump XML to /tmp if envvar is set
                if (XMLDump) {
                    XMLEntity::dumpXML(nodeBoardsXML, "nodeboards", false);
                }
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
            boost::scoped_ptr<XMLEntity> xmlEntity( XMLEntity::readXML(nodeBoardsXML) );
            XMLEntities = xmlEntity->subentities();
            // Read all node boards from XML file to container
            subEntities = XMLEntities[0]->subentities();
            nodeBoardCount = 0;
            for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
                if ((*iter)->name() == "BGQNodeCard") {
                    // Build up the location, example R01-M0-N00
                    string location(midplane);
                    location.append("-");
                    location.append((*iter)->attrByName("NodeCardId"));
                    // Construct the node board object from XML stream
                    NodeBoard::Pimpl nodeBoard(new NodeBoard::Impl(location,*iter));
                    bgq::util::Location nodeBoardLocation = nodeBoard->getLocation();
                    (midplaneIter->second)->addNodeBoard(nodeBoardLocation.getNodeBoard(), nodeBoard);
                    nodeBoardCount++;
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
        // Handle node board exception
        } catch (const bgsched::InternalException& e) {
            // Force XML dump to log
            bgsched::logXML(nodeBoardsXML, true);
            LOG_ERROR_MSG( "Exception in " << __FUNCTION__ << " is " << e.what() );
            // Rethrow the exception
            throw;
        // Handle XML exception
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
        if (nodeBoardCount != Midplane::MaxNodeBoards) {
            THROW_EXCEPTION(
                    bgsched::InternalException,
                    bgsched::InternalErrors::InconsistentDataError,
                    "Number of node boards found, " << nodeBoardCount << " , is not equal to 16"
            );
        }
    }
    //LOG_TRACE_MSG("Done adding node boards to midplanes");

    //-----------------------------------------------
    //----------------------Switches-----------------
    //-----------------------------------------------
    //LOG_TRACE_MSG("Calling BGQDB::getSwitches()");
    result= BGQDB::getSwitches(switchesXML);
    //LOG_TRACE_MSG("Done calling BGQDB::getSwitches()");
    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            if (XMLDump) {
                XMLEntity::dumpXML(switchesXML, "switches", false);
            }
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
            );
        case BGQDB::NOT_FOUND: // Switch table is empty (empty XML file created)
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    DatabaseErrors::DataNotFound,
                    Midplane_Without_Switches_Str
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

    // Parse the switches XML file and build switch objects
    try {
        boost::scoped_ptr<XMLEntity> xmlEntity( XMLEntity::readXML(switchesXML) );
        XMLEntities = xmlEntity->subentities();
        // Read all switches from XML file to container
        subEntities = XMLEntities[0]->subentities();
        switchCount = 0;
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            if ((*iter)->name() == "BGQSwitch") {
                // Get location for switch constructor
                string location((*iter)->attrByName("switchId"));
                // Construct the switch object from XML stream
                Switch::Pimpl switchImpl(new Switch::Impl(location,*iter));
                // Check if any switch settings to add to switch
                subSubEntities = (subEntities[switchCount])->subentities();
                for (vector<const XMLEntity *>::iterator iter1 = subSubEntities.begin(); iter1 != subSubEntities.end(); ++iter1) {
                    if ((*iter1)->name() == "BGQSwitchSetting") {
                        // Check if  block state is something other than (F)ree - otherwise skip
                        if (strcmp((*iter1)->attrByName("blockStatus"),"F") != 0) {
                            // Construct the switch settings object from XML stream
                            SwitchSettings::Pimpl switchSettingsImpl(new SwitchSettings::Impl(*iter1));
                            switchImpl->addSwitchSettings(switchSettingsImpl);
                        } else {
                            LOG_TRACE_MSG("Ignoring switch settings for switch " << location);
                        }

                    } else {
                        // No switch settings - this is normal if switch is not in use
                        LOG_TRACE_MSG("No switch settings exist for switch " << location);
                    }
                }
                // Calculate/set the "in use" state after collecting switch settings
                switchImpl->calculateInUse();

                bgq::util::Location switchLocation = switchImpl->getLocation();
                // Find the midplane associated with the switch in the midplane map
                midplaneIter = _midplaneMap.find(switchLocation.getMidplaneLocation());
                if (midplaneIter != _midplaneMap.end()) {
                    (*midplaneIter).second->addSwitch(switchLocation.getSwitchDimension(), switchImpl);
                    switchCount++;
                } else {
                    THROW_EXCEPTION(
                            bgsched::InternalException,
                            bgsched::InternalErrors::InconsistentDataError,
                            Switch_Without_Midplane_Str
                    );
                }
            } else {
                // Force XML dump to log
                bgsched::logXML(switchesXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQSwitch tag."
                );
            }
        }
        LOG_TRACE_MSG("Got " << switchCount << " switches");
    // Handle switch exception
    } catch (const bgsched::InternalException& e) {
        // Force XML dump to log
        bgsched::logXML(switchesXML, true);
        LOG_ERROR_MSG( "Exception in " << __FUNCTION__ << " is " << e.what() );
        // Rethrow the exception
        throw;
    // Handle XML exception
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(switchesXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }

    //-----------------------------------------------
    //----------------------Cables-------------------
    //-----------------------------------------------
    //LOG_TRACE_MSG("Calling BGQDB::getBPWireList()");
    result = BGQDB::getBPWireList(cablesXML);
    //LOG_TRACE_MSG("Done calling BGQDB::getBPWireList()");
    switch (result) {
        case BGQDB::OK:
            // Dump XML to /tmp if envvar is set
            if (XMLDump) {
                XMLEntity::dumpXML(cablesXML, "cables", false);
            }
            break;
        case BGQDB::DB_ERROR:
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DatabaseError,
                    DB_Access_Error_Str
                    );
        case BGQDB::NOT_FOUND: // Cables table is empty (empty XML file created)
            THROW_EXCEPTION(
                    bgsched::DatabaseException,
                    bgsched::DatabaseErrors::DataNotFound,
                    Midplanes_Without_Cables_Str
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

    // Parse the cables XML file and build cable objects
    try {
        boost::scoped_ptr<XMLEntity> xmlEntity( XMLEntity::readXML(cablesXML) );
        XMLEntities = xmlEntity->subentities();
        // Read all cables from XML file to container
        subEntities = XMLEntities[0]->subentities();
        for (vector<const XMLEntity *>::iterator iter = subEntities.begin(); iter != subEntities.end(); ++iter) {
            // Torus cables go between switches
            if ((*iter)->name() == "BGQWire") {
                // Get location for cable constructors
                string cableLocation((*iter)->attrByName("source"));
                // Construct the cable object from XML stream
                Cable::Pimpl cable(new Cable::Impl(cableLocation,*iter));
                bgq::util::Location location = cable->getLocation();
                // Find the switch associated with the cable in the midplane map
                midplaneIter = _midplaneMap.find(location.getMidplaneLocation());
                if (midplaneIter != _midplaneMap.end()) {
                    Switch::Pimpl switchPimpl = (*midplaneIter).second->getSwitch(locationDimensionToBgschedDimension(location.getSwitchDimension()));
                    if (!switchPimpl) {
                        LOG_ERROR_MSG("Cound not find switch for cable " << cableLocation);
                        THROW_EXCEPTION(
                                bgsched::InternalException,
                                bgsched::InternalErrors::InconsistentDataError,
                                Cable_Without_Switch_Str
                                );
                    } else {
                        switchPimpl->setCable(cable);
                    }
                    cableCount++;
                } else {
                    THROW_EXCEPTION(
                            bgsched::InternalException,
                            bgsched::InternalErrors::InconsistentDataError,
                            Switch_Without_Midplane_Str
                            );
                }
            } else {
                // Force XML dump to log
                bgsched::logXML(cablesXML, true);
                THROW_EXCEPTION(
                        bgsched::InternalException,
                        bgsched::InternalErrors::XMLParseError,
                        "Unexpected tag, " << (*iter)->name() << " in XML. Expecting BGQWire tag."
                        );
            }
        }
        LOG_TRACE_MSG("Got " << cableCount << " cables");
    // Handle cable exception
    } catch (const bgsched::InternalException& e) {
        // Force XML dump to log
        bgsched::logXML(cablesXML, true);
        LOG_ERROR_MSG( "Exception in " << __FUNCTION__ << " is " << e.what() );
        // Rethrow the exception
        throw;
    // Handle XML exception
    } catch (const XMLException& e) {
        // Force XML dump to log
        bgsched::logXML(cablesXML, true);
        LOG_ERROR_MSG("XML Exception: " << e);
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                XML_Parse_Error_Str
        );
    }
}

ComputeHardware::Impl::Impl(
        const ComputeHardware::Pimpl fromComputeHardware
        ) :
    _midplanes(),
    _midplaneMap(),
    _extent(0,0,0,0),
    _rows(fromComputeHardware->_rows),
    _columns(fromComputeHardware->_columns)
{
    //LOG_TRACE_MSG("ComputeHardware::Impl copy constructor");
    _extent = fromComputeHardware->_extent;
    _midplanes.resize(boost::extents[(_extent[Dimension::A])+1][(_extent[Dimension::B])+1][(_extent[Dimension::C])+1][(_extent[Dimension::D])+1]);
    // Iterate thru the midplane map and copy midplanes
    for (MidplaneMap::iterator midplaneIter = fromComputeHardware->_midplaneMap.begin(); midplaneIter!=fromComputeHardware->_midplaneMap.end(); ++midplaneIter) {
        // Get the midplane location from the map
        string location = midplaneIter->first;
        // Get the midplane from the map
        Midplane::Pimpl midplane = midplaneIter->second;
        // Get the midplane coordinates
        Coordinates coords = midplane->getCoordinates();
        // Make a copy of the midplane object
        Midplane::Pimpl newMidplane = Midplane::Pimpl(new Midplane::Impl(midplane));
        // Store new midplane in the map using the location as the key
        _midplaneMap[location] = newMidplane;
        // Also store midplane in multi-array by coordinates
        _midplanes [coords[Dimension::A]] [coords[Dimension::B]] [coords[Dimension::C]] [coords[Dimension::D]]= newMidplane;
    }
}

void
ComputeHardware::Impl::dump(
       ostream& os
       )
{
    uint32_t rows = getMachineRows();
    uint32_t cols = getMachineColumns();
    os << "BG/Q system configuration  . . . : " << rows << " row(s) X " << cols << " column(s)" << endl;
    // Iterate thru the midplane map and dump midplanes
    for (MidplaneMap::iterator midplaneIter = _midplaneMap.begin(); midplaneIter!=_midplaneMap.end(); ++midplaneIter) {
        // Get the midplane from the map
        Midplane::Pimpl midplane = midplaneIter->second;
        // Get the midplane coordinates
        Coordinates coords = midplane->getCoordinates();
        os << " Midplane coordinates  . . . . . : " "A:" << coords[Dimension::A] << " B:" << coords[Dimension::B] << " C:" << coords[Dimension::C] << " D:" << coords[Dimension::D] << endl;
        // Dump the midplane contents
        midplane->dump(os);
        os << endl;
    }
}

Midplane::Pimpl
ComputeHardware::Impl::getMidplane(
        const Coordinates& coords
        ) const
{
    LOG_TRACE_MSG( "Getting midplane with coordinates: " << "< " << coords[Dimension::A] << ","
            << coords[Dimension::B] << "," << coords[Dimension::C] << "," << coords[Dimension::D] << " >");
    for ( Dimension dimension = Dimension::A; dimension <= Dimension::D; ++dimension ) {
        if ( coords[dimension] >= getMachineSize(dimension) ) {
            THROW_EXCEPTION(
                    bgsched::InputException,
                    bgsched::InputErrors::InvalidMidplaneCoordinates,
                    "Invalid midplane coordinates"
                    );
        }
    }

    Midplane::Pimpl impl = _midplanes [coords[Dimension::A]] [coords[Dimension::B]] [coords[Dimension::C]] [coords[Dimension::D]];
    return impl;
}

Midplane::Pimpl
ComputeHardware::Impl::getMidplane(
        const string& location
        ) const
{
    MidplaneMap::const_iterator mp = _midplaneMap.find(location.substr(0,bgq::util::Location::MidplaneLength));
    if ( mp == _midplaneMap.end() ) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Invalid midplane location " << location
                );
    }
    Midplane::Pimpl impl = mp->second;
    return impl;
}

Midplane::Pimpl
ComputeHardware::Impl::getMidplane(
        const bgq::util::Location& location
        ) const
{
    return getMidplane(location.getMidplaneLocation());
}

const ComputeHardware::Impl::MidplaneArray&
ComputeHardware::Impl::getMidplanes() const
{
    return _midplanes;
}

uint32_t
ComputeHardware::Impl::getMachineSize(
        const Dimension& dimension
        ) const
{
    if (dimension > Dimension::D) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidDimension,
                "Dimension exceeds range boundary");
    } else {
        return _midplanes.shape()[dimension];
    }
}

const Coordinates&
ComputeHardware::Impl::getMachineExtent() const
{
    return _extent;
}

uint32_t
ComputeHardware::Impl::getMachineRows() const
{
    return _rows;
}

uint32_t
ComputeHardware::Impl::getMachineColumns() const
{
    return _columns;
}

EnumWrapper<Hardware::State>
ComputeHardware::Impl::getState(
        const string& location
        ) const
{
    return getHardware(location)->getState();
}

void
ComputeHardware::Impl::setState(
        const string& locationString,
        Hardware::State state,
        bool fromDatabase
        )
{
    try {
        bgq::util::Location location(locationString);
        NodeBoard::Pimpl nodeBoardPtr;
        Switch::Pimpl switchPtr;
        MidplaneMap::const_iterator iter = _midplaneMap.find(location.getMidplaneLocation());
        Midplane::Pimpl midplanePtr = iter->second;

        // Don't allow a conflict between hardware state and "in use" status
        switch (location.getType()) {
            // Handles following hardware: midplanes, node boards, switches
            case bgq::util::Location::Midplane:
                // Check if midplane is "in use"
                if (midplanePtr->getInUse() == Midplane::NotInUse) {
                    midplanePtr->setState(state, fromDatabase);
                } else {
                    // Midplane must not be "in use" before changing midplane hardware state
                    THROW_EXCEPTION(
                            bgsched::RuntimeException,
                            bgsched::RuntimeErrors::HardwareInUseError,
                            "Midplane state for location " << locationString << " cannot be updated when midplane is 'in use'"
                    );
                }
                break;
            case bgq::util::Location::NodeBoard:
                nodeBoardPtr = midplanePtr->getNodeBoard(location.getNodeBoard());
                // Check if node board is "in use"
                if (nodeBoardPtr->isInUse()) {
                    // Node board must not be "in use" before changing node board hardware state
                    THROW_EXCEPTION(
                            bgsched::RuntimeException,
                            bgsched::RuntimeErrors::HardwareInUseError,
                            "Node board state for location " << locationString << " cannot be updated when node board is 'in use'"
                    );
                } else {
                    nodeBoardPtr->setState(state, fromDatabase);
                }
                break;
            case bgq::util::Location::Switch:
                switchPtr = midplanePtr->getSwitch(locationDimensionToBgschedDimension(location.getSwitchDimension()));
                // Check if switch is "in use"
                if (switchPtr->getInUse() == Switch::NotInUse) {
                    switchPtr->setState(state, fromDatabase);
                } else {
                    // Switch must not be "in use" before changing switch hardware state
                    THROW_EXCEPTION(
                            bgsched::RuntimeException,
                            bgsched::RuntimeErrors::HardwareInUseError,
                            "Switch state for location " << locationString << " cannot be updated when switch is 'in use'"
                    );
                }
                break;
            default:
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::InvalidLocationString,
                        "Invalid hardware location " << locationString
                );
        }
        LOG_INFO_MSG("Successfully set hardware state to " << state << " for location " << locationString);
    } catch (const bgq::util::LocationError& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Invalid hardware location " << locationString
        );
    }
}

Hardware::Pimpl
ComputeHardware::Impl::getHardware(
        const string& locationString
        ) const
{
    try {
        bgq::util::Location location(locationString);
        MidplaneMap::const_iterator iter = _midplaneMap.find(location.getMidplaneLocation());
        MidplaneMap::mapped_type midplane = iter->second;

        switch (location.getType()) {
            // Handles following hardware: midplanes, node boards, switches
            case bgq::util::Location::Midplane:
                return midplane;
            case bgq::util::Location::NodeBoard:
                return midplane->getNodeBoard(location.getNodeBoard());
            case bgq::util::Location::Switch:
                return midplane->getSwitch(locationDimensionToBgschedDimension(location.getSwitchDimension()));
            default:
                THROW_EXCEPTION(
                        bgsched::InputException,
                        bgsched::InputErrors::InvalidLocationString,
                        "Invalid hardware location " << locationString
                );
        }
    } catch (const bgq::util::LocationError& e) {
        THROW_EXCEPTION(
                bgsched::InputException,
                bgsched::InputErrors::InvalidLocationString,
                "Invalid hardware location " << locationString
        );
    }
}

bool
ComputeHardware::Impl::validateShape(
        const Shape& shape
        ) const
{
    for (Dimension i = Dimension::A; i <= Dimension::D; ++i) {
        if (shape.getMidplaneSize(i) > getMachineSize(i)) {
            return false;
        }
    }

    return true;
}

} // namespace bgsched
