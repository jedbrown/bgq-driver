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
#include <bgsched/IOLink.h>
#include <bgsched/NodeBoard.h>
#include <bgsched/SchedUtil.h>

#include <bgsched/core/core.h>

#include "FirstFitPlugin.h"

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/propertyconfigurator.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <sys/stat.h>

// Declare the logger for this file.
static log4cxx::LoggerPtr logger(log4cxx::Logger::getLogger("ibmfirstfit.FirstFitPlugin"));

// The following macros write log messages at different log levels.
#define LOG_DEBUG_MSG(message_expr) \
 LOG4CXX_DEBUG(logger, message_expr)

#define LOG_TRACE_MSG(message_expr) \
 LOG4CXX_TRACE(logger, message_expr)

#define LOG_INFO_MSG(message_expr) \
 LOG4CXX_INFO(logger, message_expr)

#define LOG_WARN_MSG(message_expr) \
 LOG4CXX_WARN(logger, message_expr)

#define LOG_ERROR_MSG(message_expr) \
 LOG4CXX_ERROR(logger, message_expr)

#define LOG_FATAL_MSG(message_expr) \
 LOG4CXX_FATAL(logger, message_expr)

using namespace bgsched;
using namespace bgsched::allocator;
using namespace IBMFirstFit;
using namespace std;

const string LoggingPropertiesFilenameEnvVarName("FIRSTFIT_PLUGIN_LOGGING_PROPERTIES_FILE");
const string LoggingPropertiesFilename("/bgsys/local/etc/firstfit_plugin_logging.configuration");
const string DefaultConversionPattern("%d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) %c: %m%n");

/*!
 * \brief Each shape dimension can be connected as either a Torus or a Mesh.
 */
struct ShapeConnectivity {
    ResourceSpec::ConnectivitySpec::Value A;
    ResourceSpec::ConnectivitySpec::Value B;
    ResourceSpec::ConnectivitySpec::Value C;
    ResourceSpec::ConnectivitySpec::Value D;
};

static bool
isShapeValidForConnectivity(
        const ResourceSpec::ConnectivitySpec::Value connectivitySpec[],
        const Shape& shape,
        const Coordinates& machineSize,
        const bool /*isPassthrough*/
        )
{
    // Validate the shape will work with requested connectivity using the following rules:
    //
    // If the shape consists of a single midplane then the connectivity must be Torus (or Either) for all dimensions.
    // Any dimension that is equal to the size of the machine in that dimension must also be Torus (or Either).
    //
    // If a given shape dimension is less than the size of the machine in that dimension then Mesh must be specified.
    // The only exception to that requirement is if pass-through is allowed, in that situation Torus connectivity is permitted.
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        // Mesh connectivity?
        if (connectivitySpec[dim] == ResourceSpec::ConnectivitySpec::Mesh) {
            // Any dimension with a single midplane must be Torus (or Either). Any dimension that is equal to the size
            // of the machine in that dimension must also be Torus (or Either).
            if ((shape.getMidplaneSize(dim) == 1) || (shape.getMidplaneSize(dim) == machineSize[dim])) {
                return false;
            }
        } else {
            // Torus connectivity?
            if (connectivitySpec[dim] == ResourceSpec::ConnectivitySpec::Torus) {
                if ((shape.getMidplaneSize(dim) == 1) || (shape.getMidplaneSize(dim) == machineSize[dim])) {
                    continue;
                } else {
                    // Check special case where Torus is allowed for pass-through even though the First-Fit plug-in doesn't
                    // currently support pass-through.
                    //if (isPassthrough) {
                    //    continue;
                    //} else {
                    //   return false;
                    //}
                    return false; // Don't allow Torus in this situation for First-Fit plug-in
                }
            }
        }
    }
    return true;
}

static ResourceSpec::ConnectivitySpec::Value
convertToConnectivity(
        const uint32_t connectivity
        )
{
    switch(connectivity)
    {
        case 0:
            return ResourceSpec::ConnectivitySpec::Torus;
        case 1:
            return ResourceSpec::ConnectivitySpec::Mesh;
        case 2:
            return ResourceSpec::ConnectivitySpec::Either;
    }

    LOG_ERROR_MSG("Internal error, unexpected connectivity conversion");
    return ResourceSpec::ConnectivitySpec::Either;
}

static vector<ShapeConnectivity>
calculateConnectivityRotations(
        const ResourceSpec::ConnectivitySpec::Value baseConnectivitySpec[],
        const Shape& baseBlockShape,
        const Shape& currentBlockShape,
        const Coordinates& machineSize,
        const bool isPassthrough
        )
{

    vector<ShapeConnectivity> connectivityRotations;
    ShapeConnectivity shapeConnectivity;
    // Check if this is the base block shape or a rotation of the shape
    if (baseBlockShape == currentBlockShape) {
        // Validate the shape will work with requested connectivity before using
        if (isShapeValidForConnectivity(baseConnectivitySpec, baseBlockShape, machineSize, isPassthrough)) {
            // Just use the base connectivity spec and return (no other connectivity rotations exist)
            shapeConnectivity.A = baseConnectivitySpec[Dimension::A];
            shapeConnectivity.B = baseConnectivitySpec[Dimension::B];
            shapeConnectivity.C = baseConnectivitySpec[Dimension::C];
            shapeConnectivity.D = baseConnectivitySpec[Dimension::D];
            // Shape/connectivity are valid so save it
            connectivityRotations.push_back(shapeConnectivity);
        } else {
            //LOG_DEBUG_MSG("Shape/connectivity combination NOT valid -" <<
            //        " A:" << currentBlockShape.getMidplaneSize(Dimension::A) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.A) << ")" <<
            //        " B:" << currentBlockShape.getMidplaneSize(Dimension::B) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.B) << ")" <<
            //        " C:" << currentBlockShape.getMidplaneSize(Dimension::C) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.C) << ")" <<
            //        " D:" << currentBlockShape.getMidplaneSize(Dimension::D) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.D) << ")");
        }
        return connectivityRotations;
    }

    // This is a rotation on the shape so find all possible connectivity rotations
    uint32_t shapeConnectivityPermutations[Dimension::MidplaneDims];
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        // This hash of original shape+connectivity will allow deriving unique shape+connectivity rotations later
        shapeConnectivityPermutations[dim] = (baseBlockShape.getMidplaneSize(dim) * 10) + baseConnectivitySpec[dim];
    }

    sort(shapeConnectivityPermutations, shapeConnectivityPermutations+Dimension::MidplaneDims);

    ResourceSpec::ConnectivitySpec::Value currentConnectivitySpec[bgsched::Dimension::MidplaneDims];
    do {
        // LOG_TRACE_MSG(shapeConnectivityPermutations[Dimension::A] << " " << shapeConnectivityPermutations[Dimension::B] << " " << shapeConnectivityPermutations[Dimension::C] << " " << shapeConnectivityPermutations[Dimension::D]);
        // Now match the permutations of shape+connectivity against the rotated shape so we can derive connectivity rotations
        if (((shapeConnectivityPermutations[Dimension::A] / 10) == currentBlockShape.getMidplaneSize(Dimension::A)) &&
            ((shapeConnectivityPermutations[Dimension::B] / 10) == currentBlockShape.getMidplaneSize(Dimension::B)) &&
            ((shapeConnectivityPermutations[Dimension::C] / 10) == currentBlockShape.getMidplaneSize(Dimension::C)) &&
            ((shapeConnectivityPermutations[Dimension::D] / 10) == currentBlockShape.getMidplaneSize(Dimension::D))){
            // Now derive the connectivity rotation from the hash
            shapeConnectivity.A = convertToConnectivity(shapeConnectivityPermutations[Dimension::A] % 10); // Just get remainder
            shapeConnectivity.B = convertToConnectivity(shapeConnectivityPermutations[Dimension::B] % 10); // Just get remainder
            shapeConnectivity.C = convertToConnectivity(shapeConnectivityPermutations[Dimension::C] % 10); // Just get remainder
            shapeConnectivity.D = convertToConnectivity(shapeConnectivityPermutations[Dimension::D] % 10); // Just get remainder
            // Build connectivity spec for validation
            currentConnectivitySpec[Dimension::A] = shapeConnectivity.A;
            currentConnectivitySpec[Dimension::B] = shapeConnectivity.B;
            currentConnectivitySpec[Dimension::C] = shapeConnectivity.C;
            currentConnectivitySpec[Dimension::D] = shapeConnectivity.D;
            // Validate the shape will work with requested connectivity before using
            if (isShapeValidForConnectivity(currentConnectivitySpec, currentBlockShape, machineSize, isPassthrough)) {
                // Shape/connectivity are valid so save it
                connectivityRotations.push_back(shapeConnectivity);
            } else {
                //LOG_DEBUG_MSG("Shape/connectivity combination NOT valid -" <<
                //        " A:" << currentBlockShape.getMidplaneSize(Dimension::A) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.A) << ")" <<
                //        " B:" << currentBlockShape.getMidplaneSize(Dimension::B) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.B) << ")" <<
                //        " C:" << currentBlockShape.getMidplaneSize(Dimension::C) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.C) << ")" <<
                //        " D:" << currentBlockShape.getMidplaneSize(Dimension::D) << "(" << ResourceSpec::ConnectivitySpec::toString(shapeConnectivity.D) << ")");
            }
        }
    } while (next_permutation(shapeConnectivityPermutations, shapeConnectivityPermutations+Dimension::MidplaneDims));

    // This is a rotated shape so calculate available connectivity rotations
    return connectivityRotations;
}

static bool
isMidplaneDrained(
        const ResourceSpec::DrainedMidplanes& resourceSpecDrainedMidplanes,
        const Model::DrainedMidplanes& modelDrainedMidplanes,
        const string& midplaneLocation
        )
{
    // Check if midplane is in the resource spec drain list
    if (resourceSpecDrainedMidplanes.size() > 0) {
        ResourceSpec::DrainedMidplanes::const_iterator iter = resourceSpecDrainedMidplanes.find(midplaneLocation);
        // In drain list?
        if (iter != resourceSpecDrainedMidplanes.end()) {
            // Found midplane in resource spec drain list
            return true;
        }
    }

    // Check if midplane is in the model drain list
    if (modelDrainedMidplanes.size() > 0) {
        Model::DrainedMidplanes::const_iterator iter = modelDrainedMidplanes.find(midplaneLocation);
        // In drain list?
        if (iter != modelDrainedMidplanes.end()) {
            // Found midplane in model drain list
            return true;
        }
    }

    // Not in drain list
    return false;
}

static bool
isMidplaneAvailable(
        Midplane::ConstPtr& midplanePtr
        )
{
    // Hardware available and midplane not in use?
    if ((midplanePtr->getState() == Hardware::Available) &&
        (midplanePtr->getInUse() == Midplane::NotInUse)  &&
        (midplanePtr->getAvailableNodeBoardCount() == Midplane::MaxNodeBoards)) {
        return true;
    } else {
        return false;
    }
}

// Prior to calling this method it is expected that the associated midplane is available.
// This method is only applicable when looking for non-pass-through block resources.
static bool
switchesAndCablesAvailable(
        Midplane::ConstPtr& midplanePtr,
        const Shape& shape
)
{
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        Switch::ConstPtr switchPtr = midplanePtr->getSwitch(dim);

        // Midplane will be "in use" for switches with state of IncludedBothPortsInUse,
        // IncludedOutputPortInUse, IncludedInputPortInUse, Wrapped and WrappedPassthrough.
        // If midplane is "not in use" only Passthrough and NotinUse are valid states. If Passthrough is
        // set then the switch can also be used for wrapped configurations (i.e. blocks with 1 midplane
        // in the dimension.

        // Only allow wrapped switch with pass-through switch
        if ((switchPtr->getInUse() == Switch::Passthrough) && (shape.getMidplaneSize(dim) > 1)) {
            // Switch in use by passthrough
            return false;
        }

        // Check if switch is not in use or pass-through with wrapped block request
        if ((switchPtr->getInUse() == Switch::NotInUse) || (switchPtr->getInUse() == Switch::Passthrough)) {
            if (switchPtr->getState() == Hardware::Available) {
                // Switch is available, only need to check if a cable is available when the shape
                // requires more than 1 midplane in the dimension (i.e. not wrapped)
                if (shape.getMidplaneSize(dim) > 1) {
                    // Get cable for the dimension
                    Cable::ConstPtr cablePtr = switchPtr->getCable();
                    if (!cablePtr) {
                        // LOG_DEBUG_MSG("WARNING: No cable found for switch "  << switchPtr->getLocation());
                        // No cable for switch dimension that requires a cable
                        return false;
                    } else {
                        // Check cable availability
                        if (cablePtr->getState() != Hardware::Available) {
                            // Cable is unavailable
                            return false;
                        }
                    }
                }
            } else {
                // Switch is unavailable
                return false;
            }
        } else {
            // Switch is "in use"
            return false;
        }
    }
    // Switches and cables are available for the midplane
    return true;
}

static void
useInternalConfiguration()
{
    log4cxx::LayoutPtr layout_ptr(new log4cxx::PatternLayout(DefaultConversionPattern));
    log4cxx::AppenderPtr appender_ptr(new log4cxx::ConsoleAppender(layout_ptr));
    log4cxx::BasicConfigurator::configure(appender_ptr);
    LOG_WARN_MSG("Using internal logging configuration");
}

FirstFitPlugin::FirstFitPlugin()
{
    struct stat buffer;

    // If FIRSTFIT_PLUGIN_LOGGING_PROPERTIES_FILE env var is set then read logging properties
    // from that file rather than from the default configuration file location.
    const char *filename_env(getenv(LoggingPropertiesFilenameEnvVarName.c_str()));

    if (filename_env) {
        string logging_properties_filename(filename_env);
        // Check if env var config file exists?
        if (stat(logging_properties_filename.c_str(), &buffer) == 0) {
            // File exists so use config settings
            log4cxx::PropertyConfigurator::configure(log4cxx::File(logging_properties_filename));
            LOG_INFO_MSG("Logging configuration: '" << logging_properties_filename);
        } else {
            useInternalConfiguration();
        }
    } else {
        // Check if default config file exists?
        if (stat(LoggingPropertiesFilename.c_str(), &buffer) == 0) {
            // Use the default config file
            log4cxx::PropertyConfigurator::configure(log4cxx::File(LoggingPropertiesFilename));
            LOG_INFO_MSG("Logging configuration: '" << LoggingPropertiesFilename);
        } else {
            useInternalConfiguration();
        }
    }
}

FirstFitPlugin::~FirstFitPlugin()
{

}

void
FirstFitPlugin::description(
        ostream& os
        )
{
    os << "IBM FirstFit Plug-in";
}

void
FirstFitPlugin::prepare(
        const Allocator& /*allocator*/,
        Model& model,
        const std::string& blockName
        )
{
    // Attempt to allocate the block
    try {
        model.allocate(blockName);
    }
    catch(...)
    {
        throw Plugin::Exception(
                Plugin::Errors::BlockResourcesUnavailable,
                "Block was not allocated.");
    }
}

void
FirstFitPlugin::release(
        const Allocator& /*allocator*/,
        Model& model,
        const string& blockName
        )
{
    // Attempt to deallocate the block
    try {
        model.deallocate(blockName);
    }
    catch(...)
    {
        throw Plugin::Exception(
                Plugin::Errors::BlockDeallocateFailed,
                "Unable to deallocate block.");
    }
}

static Block::Ptr
findSmallBlock(
        const Model& model,
        const ResourceSpec& resourceSpec,
        bool  isSearchSubDividedOnly
)
{
    // Possible small block sizes are 32, 64, 128 and 256 compute nodes.
    // - 256: half of a midplane comprised of 8 node boards (N00..N07) or (N08..N15)
    // - 128: quarter of a midplane comprised of 4 node boards
    //        (N00..N03) or (N04..N07) or (N08..N11) or (N12..N15)
    // -  64: eighth of a midplane comprised of 2 node boards
    //        (N00..N01) or (N02..N03) or (N04..N05) or (N06..N07) or
    //        (N08..N09) or (N10..N11) or (N12..N13) or (N14..N15)
    // -  32: sixteenth of a midplane is comprised of any one node board (N00..N15)

    // Determine how many compute nodes and consecutive node boards are required to satisfy request
    uint32_t requiredNodes = resourceSpec.getNodeCount();
    uint32_t requiredNodeBoards = requiredNodes / NodeBoard::NodeCount;
    uint32_t availableNodeBoards = 0;
    uint32_t bestNodeBoardIndex = Midplane::MaxNodeBoards+1;

    // Get any drained midplanes from resource spec and model
    ResourceSpec::DrainedMidplanes resourceSpecDrainedMidplanes = resourceSpec.getDrainedMidplanes();
    Model::DrainedMidplanes modelDrainedMidplanes = model.getDrainedMidplanes();

    // Log any drained midplanes from resource spec and model
    if (resourceSpecDrainedMidplanes.size() > 0) {
        for (ResourceSpec::DrainedMidplanes::iterator iter = resourceSpecDrainedMidplanes.begin(); iter != resourceSpecDrainedMidplanes.end(); ++iter) {
            // Log drained midplanes
            LOG_TRACE_MSG("Drained midplane from resource spec: " << *iter);
        }
    }
    if (modelDrainedMidplanes.size() > 0) {
        for (Model::DrainedMidplanes::iterator iter = modelDrainedMidplanes.begin(); iter != modelDrainedMidplanes.end(); ++iter) {
            // Log drained midplanes
            LOG_TRACE_MSG("Drained midplane from model: " << *iter);
        }
    }

    // Get the list of midplanes from the model
    Midplanes::ConstPtr midplanes = model.getMidplanes();
    // Get the size of the system in each dimension
    Coordinates machineSize = midplanes->getMachineSize();

    bool slotAvail[Midplane::MaxNodeBoards];
    bool slotIOAvail[Midplane::MaxNodeBoards];
    // Search the midplane list
    for (uint32_t a = 0; a <  machineSize[Dimension::A]; ++a) {
        for (uint32_t b = 0; b < machineSize[Dimension::B]; ++b) {
            for (uint32_t c = 0; c < machineSize[Dimension::C]; ++c) {
                for (uint32_t d = 0; d < machineSize[Dimension::D]; ++d) {
                    Coordinates currentCoords(a, b, c, d);
                    Midplane::ConstPtr currentMidplane = midplanes->get(currentCoords);
                    string location = currentMidplane->getLocation();

                    // Check if midplane is in the drain list
                    if (isMidplaneDrained(resourceSpecDrainedMidplanes, modelDrainedMidplanes, location)) {
                        continue; // Found in drain list so move to next midplane to examine
                    }

                    // Check if midplane hardware is Available and midplane is not already fully in use
                    if ((currentMidplane->getState() == Hardware::Available)
                         && (currentMidplane->getInUse() != Midplane::AllInUse)) {
                        // Check if searching subdivided midplanes only
                        if (isSearchSubDividedOnly) {
                            // Looking for subdivided midplanes
                            if (currentMidplane->getInUse() != Midplane::SubdividedInUse) {
                                continue; // Skip, this is not a subdivided midplane
                            }
                        } else {
                            // Looking for midplanes not subdivided
                            if (currentMidplane->getInUse() == Midplane::SubdividedInUse) {
                                continue; // Skip, this is a subdivided midplane
                            }
                        }

                        vector<IOLink::ConstPtr> IOLinks;
                        try {
                            // Get the I/O links for the midplane
                            IOLinks = bgsched::core::getIOLinks(location);
                        } catch (...) { // Handle any exception retrieving I/O links
                            throw Plugin::Exception(
                                    Plugin::Errors::FindBlockResourcesFailed,
                                    "Error retrieving I/O links, unable to create small block.");
                        }

                        // Search for available node boards and I/O links on the midplane
                        availableNodeBoards = 0;
                        for (uint32_t i = 0; i <  Midplane::MaxNodeBoards; i++) {
                            // Assume no I/O links on the node board
                            slotIOAvail[i] = false;
                            NodeBoard::ConstPtr currentNodeBoard = currentMidplane->getNodeBoard(i);
                            if (currentNodeBoard->getState() != Hardware::Available) {
                                slotAvail[i] = false;
                            } else {
                                if (currentNodeBoard->isInUse()) {
                                    slotAvail[i] = false;
                                } else {
                                    // Node board is available but need to check I/O link availability
                                    slotAvail[i] = true;
                                    availableNodeBoards++;
                                    // Search the I/O links for the midplane to see if node board has any I/O links
                                    for (vector<IOLink::ConstPtr>::iterator it = IOLinks.begin(); it != IOLinks.end(); ++it) {
                                        string IOLinkLocation = (*it)->getLocation();
                                        // Check if IO link (e.g. R00-M1-N08-J06) is available for the node board
                                        if (currentNodeBoard->getLocation() ==  IOLinkLocation.substr(0,10)) {
                                            slotIOAvail[i] = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        // Check whether enough resources were found to continue search
                        if (availableNodeBoards >= requiredNodeBoards) {
                            // Now compare compute node requirements against node board search results
                            switch(requiredNodes) {
                                // 32 compute nodes (1 node board)
                                case NodeBoard::NodeCount:
                                    for (uint32_t index=0; index < Midplane::MaxNodeBoards; index++) {
                                        // Need 1 available node board and an I/O link
                                        if (slotAvail[index] && slotIOAvail[index]) {
                                            bestNodeBoardIndex = index;
                                            break;
                                        }
                                    }
                                    break;
                                // 64 compute nodes (2 consecutive node boards)
                                case Midplane::Eighth:
                                    for (uint32_t index=0; index < Midplane::MaxNodeBoards; index=index+2) {
                                        // Need 2 consecutive available node boards and an I/O link
                                        if (slotAvail[index] && slotAvail[index+1]) {
                                            // Need I/O links on at least 1 of the node boards
                                            if (slotIOAvail[index] || slotIOAvail[index+1]) {
                                                bestNodeBoardIndex = index;
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                // 128 compute nodes (4 consecutive node boards)
                                case Midplane::Quarter:
                                    for (uint32_t index=0; index < Midplane::MaxNodeBoards; index=index+4) {
                                        // Need 4 consecutive available node boards and an I/O link
                                        if (slotAvail[index] && slotAvail[index+1] && slotAvail[index+2] && slotAvail[index+3]) {
                                            if (slotIOAvail[index] || slotIOAvail[index+1] || slotIOAvail[index+2] || slotIOAvail[index+3]) {
                                                bestNodeBoardIndex = index;
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                // 256 compute nodes (8 consecutive node boards)
                                case Midplane::Half:
                                    for (uint32_t index=0; index < Midplane::MaxNodeBoards; index=index+8) {
                                        // Need 8 consecutive available node boards and an I/O link
                                        if (slotAvail[index] && slotAvail[index+1] && slotAvail[index+2] && slotAvail[index+3]
                                           && slotAvail[index+4] && slotAvail[index+5] && slotAvail[index+6] && slotAvail[index+7]) {
                                            if (slotIOAvail[index] || slotIOAvail[index+1] || slotIOAvail[index+2] || slotIOAvail[index+3]
                                                || slotIOAvail[index+4] || slotIOAvail[index+5] || slotIOAvail[index+6] || slotIOAvail[index+7]) {
                                                bestNodeBoardIndex = index;
                                                break;
                                            }
                                        }
                                    }
                                    break;
                            }

                            if (bestNodeBoardIndex < Midplane::MaxNodeBoards) {
                                // Found available midplane so create the small block from that
                                Block::NodeBoards nodeBoards;
                                // Build list of node boards to use for block
                                for (uint32_t index=0; index < requiredNodeBoards; index++) {
                                    nodeBoards.push_back(currentMidplane->getNodeBoard(bestNodeBoardIndex+index)->getLocation());
                                }

                                try {
                                    // Create the block
                                    return(Block::create(nodeBoards));
                                } catch (...) { // Handle any exception creating the block
                                    throw Plugin::Exception(
                                            Plugin::Errors::FindBlockResourcesFailed,
                                            "Unable to create small block.");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // No resources found - return NULL equivalency
    return Block::Ptr();
}

static Block::Ptr
findLargeBlock(
        const Allocator& allocator,
        const Model& model,
        const ResourceSpec& resourceSpec
)
{
    // Define parameters for Block::create
    Block::Midplanes midplaneLocations;
    Block::PassthroughMidplanes passthroughLocations;
    Block::DimensionConnectivity dimensionConnectivity;

    // Shapes can only be created thru the Allocator so only valid shapes can ever be created.
    // Also a ResourceSpec can only be constructed by specifying a Shape.
    Shape::ConstPtr baseBlockShape = resourceSpec.getShape();
    uint32_t midplaneCount = baseBlockShape->getMidplaneCount();

    // Get any drained midplanes from resource spec and model
    ResourceSpec::DrainedMidplanes resourceSpecDrainedMidplanes = resourceSpec.getDrainedMidplanes();
    Model::DrainedMidplanes modelDrainedMidplanes = model.getDrainedMidplanes();

    // Get the list of midplanes from the model
    Midplanes::ConstPtr midplanes = model.getMidplanes();
    // Get the size of the system in each dimension
    Coordinates machineSize = midplanes->getMachineSize();

    // Determine the connectivity (torus/mesh) and midplane requirements in each dimension to satisfy request
    ostringstream os;
    os << "Block connectivity specification: ";
    Coordinates requiredMidplaneSize(0,0,0,0);
    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
        requiredMidplaneSize[dim] = baseBlockShape->getMidplaneSize(dim);
        if (resourceSpec.getConnectivitySpec(dim) == ResourceSpec::ConnectivitySpec::Either) {
            os << " Either ";
        } else {
            if (resourceSpec.getConnectivitySpec(dim) == ResourceSpec::ConnectivitySpec::Torus) {
                os << " Torus ";
            } else {
                os << " Mesh ";
            }
        }
    }

    // Log the requested shape
    LOG_DEBUG_MSG("Block shape request (midplanes) - A:" << requiredMidplaneSize[Dimension::A]
                                                << " B:" << requiredMidplaneSize[Dimension::B]
                                                << " C:" << requiredMidplaneSize[Dimension::C]
                                                << " D:" << requiredMidplaneSize[Dimension::D]);
    // Log the requested connectivity
    LOG_DEBUG_MSG(os.str());

    // Log any drained midplanes from resource spec and model
    if (resourceSpecDrainedMidplanes.size() > 0) {
        for (ResourceSpec::DrainedMidplanes::iterator iter = resourceSpecDrainedMidplanes.begin(); iter != resourceSpecDrainedMidplanes.end(); ++iter) {
            // Log drained midplanes
            LOG_TRACE_MSG("Drained midplane from resource spec: " << *iter);
        }
    }
    if (modelDrainedMidplanes.size() > 0) {
        for (Model::DrainedMidplanes::iterator iter = modelDrainedMidplanes.begin(); iter != modelDrainedMidplanes.end(); ++iter) {
            // Log drained midplanes
            LOG_TRACE_MSG("Drained midplane from model: " << *iter);
        }
    }

    try {
        // ========= Find single midplane block request? =========
        if (midplaneCount == 1) {
            // --- Rules ---:
            // If looking for single midplane we know that all dimensions will be a wrapped Torus since this
            // is enforced by ResourceSpec::setConnectivitySpec(). Can also ignore rotation flag on the resource spec.
            for (uint32_t a = 0; a < machineSize[Dimension::A]; ++a) {
                for (uint32_t b = 0; b < machineSize[Dimension::B]; ++b) {
                    for (uint32_t c = 0; c < machineSize[Dimension::C]; ++c) {
                        for (uint32_t d = 0; d < machineSize[Dimension::D]; ++d) {
                            Coordinates currentCoords(a, b, c, d);
                            Midplane::ConstPtr currentMidplane = midplanes->get(currentCoords);
                            string location = currentMidplane->getLocation();

                            // Check if midplane is in the drain list
                            if (isMidplaneDrained(resourceSpecDrainedMidplanes, modelDrainedMidplanes, location)) {
                                continue; // Found in drain list so move to next midplane to examine
                            }

                            // Check if midplane hardware, including node boards, are Available and midplane is not already in use
                            if (isMidplaneAvailable(currentMidplane)) {
                                // The midplane is available for large block usage. Check on switch availability.
                                if (switchesAndCablesAvailable(currentMidplane, *baseBlockShape)) {
                                    // Set midplane location
                                    midplaneLocations.push_back(currentMidplane->getLocation());
                                    // Set connectivity to Torus in all dimensions
                                    for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
                                    {
                                        dimensionConnectivity[dim] = Block::Connectivity::Torus;
                                    }

                                    // Create the in-memory block (single midplane, no pass-through and Torus in all dimensions)
                                    return(Block::create(midplaneLocations, passthroughLocations, dimensionConnectivity));
                                }
                            }
                        }
                    }
                }
            }
            // Single midplane resources not available so fail the block request
            return Block::Ptr();
        }

        // ========= Find full system block? =========
        if (baseBlockShape->isFull()) {
            // --- Rules ---:
            // If looking for full system block all dimensions will be a Torus since this is enforced
            // by ResourceSpec::setConnectivitySpec(). Can also ignore rotation flag on the resource spec.

            // If any midplanes in the drain list the request can not be satisfied
            if ((resourceSpecDrainedMidplanes.size() > 0) || (modelDrainedMidplanes.size() > 0)) {
                // Midplanes in drain list so fail the block request
                return Block::Ptr();
            }

            // Walk all the midplanes and add to the block (corner will be R00-M0)
            for (uint32_t a = 0; a < machineSize[Dimension::A]; ++a) {
                for (uint32_t b = 0; b < machineSize[Dimension::B]; ++b) {
                    for (uint32_t c = 0; c < machineSize[Dimension::C]; ++c) {
                        for (uint32_t d = 0; d < machineSize[Dimension::D]; ++d) {
                            Coordinates currentCoords(a, b, c, d);
                            Midplane::ConstPtr currentMidplane = midplanes->get(currentCoords);

                            // Check if midplane hardware, including node boards, are Available and midplane is not already in use
                            if (isMidplaneAvailable(currentMidplane)) {
                                // The midplane is available for large block usage. Check on switch and cable availability.
                                if (switchesAndCablesAvailable(currentMidplane, *baseBlockShape)) {
                                    // Midplane, cables and switches are all available - add the midplane location
                                    midplaneLocations.push_back(currentMidplane->getLocation());
                                } else {
                                    // Switches/cables unavailable or in use so fail the block request
                                    return Block::Ptr();
                                }
                            } else {
                                // Midplane is unavailable or in use so fail the block request
                                return Block::Ptr();
                            }
                        }
                    }
                }
            }
            // Set connectivity to Torus in all dimensions
            for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim)
            {
                dimensionConnectivity[dim] = Block::Connectivity::Torus;
            }

            // Create the block (all midplanes, no pass-through and Torus in all dimensions)
            return(Block::create(midplaneLocations, passthroughLocations, dimensionConnectivity));
        }

        // ========= Find multiple midplane block request (using contiguous midplanes) =========
        // Handle other large block shapes allowing shape rotations if requested. The algorithm searches for
        // contiguous midplanes (i.e. no pass-through midplanes) for the possible shape rotations.
        //
        // --- Rules ---:
        // If the dimension size is 1 then it is a Torus (wrapped) in that dimension.
        // If connectivity "Either" is specified and the dimension size is 1 then Torus (wrapped) will be set.
        // If connectivity "Either" is specified and the requested shape dimension matches the machine dimension
        // then Torus will be used.
        // If connectivity "Either" is specified and the requested shape dimension is less than the machine
        // dimension a Mesh will be used.
        // When connectivity "Either" is specified the search algorithm will attempt to find a Mesh or Torus.
        // Midplanes (non-passthrough) included in a block have all the switches included.
        // An included midplane typically uses both inbound/outbound ports on the switch, but
        // -- if dimension is Mesh and is first in dimension then only the outbound port is used
        // -- if dimension is Mesh and is last in dimension then only the inbound port is used

        ResourceSpec::ConnectivitySpec::Value baseConnectivitySpec[bgsched::Dimension::MidplaneDims];
        ResourceSpec::ConnectivitySpec::Value currentConnectivitySpec[bgsched::Dimension::MidplaneDims];

        // Get the base connectivity and set the current connectivity
        for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
            baseConnectivitySpec[dim] = resourceSpec.getConnectivitySpec(dim);
        }

        Allocator::Shapes shapeRotations; // Vector of shape rotations
        // Check if shape is allowed to be rotated?
        if (resourceSpec.canRotateShape()) {
            // Get list of valid shape rotations
            shapeRotations = allocator.getRotations(*baseBlockShape);
            LOG_DEBUG_MSG("Found " << shapeRotations.size() << " possible shape rotations:");
            for (Allocator::Shapes::iterator iter = shapeRotations.begin(); iter != shapeRotations.end(); ++iter) {
                // Log rotated shape dimensions
                LOG_DEBUG_MSG("Rotated shape -  A:" << (*iter).getMidplaneSize(Dimension::A) <<
                                              " B:" << (*iter).getMidplaneSize(Dimension::B) <<
                                              " C:" << (*iter).getMidplaneSize(Dimension::C) <<
                                              " D:" << (*iter).getMidplaneSize(Dimension::D));
            }
        }

        // Currently the first-fit plug-in doesn't support pass-through so verify connectivity works for shape
        // (e.g. Torus was specified for pass-through dimension which is not support by this plug-in).
        if (isShapeValidForConnectivity(baseConnectivitySpec, *baseBlockShape, machineSize,resourceSpec.canUsePassthrough())) {
            // Insert the original shape at beginning of vector (will iterate over all possible shapes to find a block)
            shapeRotations.insert(shapeRotations.begin(), *baseBlockShape);
        }

        // Search all possible shapes until block resources found or no more shapes exist
        for (Allocator::Shapes::iterator iter = shapeRotations.begin(); iter != shapeRotations.end(); ++iter) {
            Shape currentBlockShape = *iter;
            uint32_t midplanesA = currentBlockShape.getMidplaneSize(Dimension::A);
            uint32_t midplanesB = currentBlockShape.getMidplaneSize(Dimension::B);
            uint32_t midplanesC = currentBlockShape.getMidplaneSize(Dimension::C);
            uint32_t midplanesD = currentBlockShape.getMidplaneSize(Dimension::D);

            // Calculate valid connectivity rotations for a given shape
            vector<ShapeConnectivity> connectivityRotations = calculateConnectivityRotations(baseConnectivitySpec, *baseBlockShape, currentBlockShape, machineSize, resourceSpec.canUsePassthrough());
            LOG_DEBUG_MSG("Found " << connectivityRotations.size() << " possible connectivities for shape:");
            for (vector<ShapeConnectivity>::iterator it = connectivityRotations.begin(); it != connectivityRotations.end(); ++it) {
                // Log rotated connectivity
                LOG_DEBUG_MSG("Connectivity -  A:" << ResourceSpec::ConnectivitySpec::toString((*it).A) <<
                                             " B:" << ResourceSpec::ConnectivitySpec::toString((*it).B) <<
                                             " C:" << ResourceSpec::ConnectivitySpec::toString((*it).C) <<
                                             " D:" << ResourceSpec::ConnectivitySpec::toString((*it).D));
            }

            // Search all possible connectivity permutations for shape until block resources found or no more permutations exist
            for (vector<ShapeConnectivity>::iterator connectivityIter = connectivityRotations.begin(); connectivityIter != connectivityRotations.end(); ++connectivityIter) {
                LOG_DEBUG_MSG("Checking for contiguous midplane resources using shape/connectivity -" <<
                        " A:" << midplanesA << "(" << ResourceSpec::ConnectivitySpec::toString((*connectivityIter).A) << ")" <<
                        " B:" << midplanesB << "(" << ResourceSpec::ConnectivitySpec::toString((*connectivityIter).B) << ")" <<
                        " C:" << midplanesC << "(" << ResourceSpec::ConnectivitySpec::toString((*connectivityIter).C) << ")" <<
                        " D:" << midplanesD << "(" << ResourceSpec::ConnectivitySpec::toString((*connectivityIter).D) << ")");
                // Set the connectivity permutation
                currentConnectivitySpec[Dimension::A] = (*connectivityIter).A;
                currentConnectivitySpec[Dimension::B] = (*connectivityIter).B;
                currentConnectivitySpec[Dimension::C] = (*connectivityIter).C;
                currentConnectivitySpec[Dimension::D] = (*connectivityIter).D;
                // Start searching for corner midplane at R00-M0
                for (uint32_t cornerA = 0; cornerA < machineSize[Dimension::A]; ++cornerA) {
                    for (uint32_t cornerB = 0; cornerB < machineSize[Dimension::B]; ++cornerB) {
                        for (uint32_t cornerC = 0; cornerC < machineSize[Dimension::C]; ++cornerC) {
                            for (uint32_t cornerD = 0; cornerD < machineSize[Dimension::D]; ++cornerD) {
                                // Check if block can fit into machine from this corner
                                if ((cornerA + midplanesA > machineSize[Dimension::A]) ||
                                    (cornerB + midplanesB > machineSize[Dimension::B]) ||
                                    (cornerC + midplanesC > machineSize[Dimension::C]) ||
                                    (cornerD + midplanesD > machineSize[Dimension::D])) {
                                    continue; // Won't fit so move to next corner
                                }

                                Coordinates startCoords(cornerA, cornerB, cornerC, cornerD);
                                Midplane::ConstPtr startMidplane = midplanes->get(startCoords);
                                string location = startMidplane->getLocation();

                                // Check if starting (corner) midplane is in the drain list
                                if (isMidplaneDrained(resourceSpecDrainedMidplanes, modelDrainedMidplanes, location)) {
                                    continue; // Found in drain list so move to next midplane to examine
                                }

                                // Check if starting (corner) midplane hardware is Available and midplane is not already in use
                                if (isMidplaneAvailable(startMidplane)) {
                                    // The midplane is available for large block usage but make sure
                                    // switches and required cables are available and not being used for pass-through
                                    if (switchesAndCablesAvailable(startMidplane, currentBlockShape)) {
                                        // All switches/cables are available so work out from this corner midplane
                                        // First verify midplanes are available and then verify switch/cable availability
                                        // Clear out any midplanes from previous attempts
                                        midplaneLocations.clear();
                                        // Set the candidate corner midplane (corner midplane MUST be first in vector)
                                        midplaneLocations.push_back(startMidplane->getLocation());
                                        LOG_TRACE_MSG("Trying corner midplane "  << cornerA << cornerB << cornerC << cornerD);

                                        // Check availability of midplanes
                                        bool midplanesAvailable = true;
                                        for (uint32_t a = cornerA; a < (cornerA + midplanesA) && a < machineSize[Dimension::A] && midplanesAvailable; a++) {
                                            for (uint32_t b = cornerB; b < (cornerB + midplanesB) && b < machineSize[Dimension::B]  && midplanesAvailable; b++) {
                                                for (uint32_t c = cornerC; c < (cornerC + midplanesC) && c < machineSize[Dimension::C] && midplanesAvailable; c++) {
                                                    for (uint32_t d = cornerD; d < (cornerD + midplanesD) && d < machineSize[Dimension::D] && midplanesAvailable; d++) {
                                                        Coordinates mpCoords(a, b, c, d);

                                                        // Already checked the corner midplane so skip over
                                                        if (startCoords != mpCoords) {
                                                            LOG_TRACE_MSG("Checking midplane availability "  << a << b << c << d);
                                                            Midplane::ConstPtr verifyMidplane = midplanes->get(mpCoords);
                                                            string mpLocation = verifyMidplane->getLocation();

                                                            // Check if required midplane is in the drain list
                                                            if (isMidplaneDrained(resourceSpecDrainedMidplanes, modelDrainedMidplanes, mpLocation)) {
                                                                midplanesAvailable = false;
                                                                continue; // Found in drain list so move to next corner midplane to examine
                                                            }

                                                            // Check if midplane hardware, including node boards, are Available and midplane is not already in use
                                                            if (isMidplaneAvailable(verifyMidplane)) {
                                                                // Add the midplane to candidate list - still need to verify switch/cable availability
                                                                midplaneLocations.push_back(verifyMidplane->getLocation());
                                                            } else {
                                                                midplanesAvailable = false;
                                                                continue; // Midplane is unavailable so move to next corner midplane to examine
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        // Midplanes available?
                                        if (midplanesAvailable) {
                                            bool switchesCablesAvailable = true;
                                            // The midplanes are available for large block usage but make sure
                                            // switches and required cables are available for requested connectivity
                                            for (uint32_t a = cornerA; a < (cornerA + midplanesA) && a < machineSize[Dimension::A] && switchesCablesAvailable; a++) {
                                                for (uint32_t b = cornerB; b < (cornerB + midplanesB) && b < machineSize[Dimension::B]  && switchesCablesAvailable; b++) {
                                                    for (uint32_t c = cornerC; c < (cornerC + midplanesC) && c < machineSize[Dimension::C] && switchesCablesAvailable; c++) {
                                                        for (uint32_t d = cornerD; d < (cornerD + midplanesD) && d < machineSize[Dimension::D] && switchesCablesAvailable; d++) {
                                                            Coordinates mpCoords(a, b, c, d);
                                                            // Already checked the corner midplane switches/cables so skip over
                                                            if (startCoords != mpCoords) {
                                                                LOG_TRACE_MSG("Checking switch/cable availability "  << a << b << c << d);
                                                                Midplane::ConstPtr verifyMidplane = midplanes->get(mpCoords);
                                                                if (switchesAndCablesAvailable(verifyMidplane, currentBlockShape) == false) {
                                                                    switchesCablesAvailable = false;
                                                                    break;
                                                                }
                                                            }
                                                        } // for d
                                                    } // for c
                                                } // for b
                                            } // for a
                                            // Switches and cables available?
                                            if (switchesCablesAvailable) {
                                                // Calculate connectivity settings - we have previously validated against the Shape
                                                for (Dimension dim = Dimension::A; dim <= Dimension::D; ++dim) {
                                                    // Torus connectivity?
                                                    if (currentConnectivitySpec[dim] == ResourceSpec::ConnectivitySpec::Torus) {
                                                        dimensionConnectivity[dim] = Block::Connectivity::Torus;
                                                    } else {
                                                        // Mesh connectivity?
                                                        if (currentConnectivitySpec[dim] == ResourceSpec::ConnectivitySpec::Mesh) {
                                                            dimensionConnectivity[dim] = Block::Connectivity::Mesh;
                                                        } else {
                                                            // Either connectivity - calculate
                                                            // If dimension is 1 or matches machine dimension use a Torus
                                                            if ((currentBlockShape.getMidplaneSize(dim) == 1) ||
                                                                (currentBlockShape.getMidplaneSize(dim) == machineSize[dim])) {
                                                                dimensionConnectivity[dim] = Block::Connectivity::Torus;
                                                            } else {
                                                                dimensionConnectivity[dim] = Block::Connectivity::Mesh;
                                                            }
                                                        }
                                                    }
                                                }
                                                // Create the in-memory block (multiple midplanes, no pass-through)
                                                return(Block::create(midplaneLocations, passthroughLocations, dimensionConnectivity));
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } catch (...) { // Handle any exceptions finding/creating the block
        throw Plugin::Exception(
                Plugin::Errors::FindBlockResourcesFailed,
                "Unable to create large block.");
    }

    // No available block resources found - return NULL equivalency
    return Block::Ptr();
}

Block::Ptr
FirstFitPlugin::findBlockResources(
        const Allocator& allocator,
        const Model& model,
        const ResourceSpec& resourceSpec
        )
{
    Block::Ptr block;

    try {
        if (resourceSpec.getNodeCount() > Midplane::Half)
        {
            block = findLargeBlock(allocator, model, resourceSpec);
        } else {
            // Try and find a small block on subdivided midplanes
            block = findSmallBlock(model, resourceSpec, true);
            // If no subdivided midplanes exist then try again on midplanes not subdivided
            if (!block) {
                block = findSmallBlock(model, resourceSpec, false);
            }
        }
    } catch (const bgsched::allocator::Plugin::Exception& e) {
        LOG_ERROR_MSG(e.what());
        // Rethrow the exception
        throw;
    } catch (...) { // Handle any other exception
        throw Plugin::Exception(
                Plugin::Errors::FindBlockResourcesFailed,
                "Unexpected error finding block resources.");
    }

    // Make sure block was found
    if (!block) {
        // Return equivalence of NULL Block
        return Block::Ptr();
    }

    // Set a default block name
    string prefix = "FIRSTFIT_";
    string blockName = SchedUtil::createBlockName(prefix);
    block->setName(blockName);
    // Set a default block description
    string description = "Created by IBM FirstFit plug-in";
    block->setDescription(description);

    return block;
}

extern "C"
{

Plugin*
create()
{
    Plugin* result = new FirstFitPlugin();
    return result;
}

void
destroy(Plugin* p)
{
    delete p;
}

} // extern "C"

