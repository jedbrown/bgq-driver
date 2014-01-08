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

#ifndef BGQ_UTIL_LOCATION_H_
#define BGQ_UTIL_LOCATION_H_


#include <regex.h>
#include <stdint.h>
#include <sys/types.h>

#include <iostream>
#include <stdexcept>
#include <string>

namespace bgq {
namespace util {

/*!
 *  \brief Encapsulates the location string.
 *
 *  The string constructor and set method parse the location and set the type
 *  and other fields.
 *
 *  If the location string isn't valid, a LocationError will be thrown. A runtime_error
 *  will be thrown if an internal error occurred (such as a bad return code from regexec).
 *
 *  The location string can contain extra whitespace before and after the
 *  location text. The extra whitespace will be stripped from the string, so
 *  that calls to getLocation() or operator std::string() return a location
 *  string without extra whitespace.
 *
 *  Locations are defined in
 *  "Naming Convention Document" -- updated 07/22/09
 *  in the Blue Gene/Q Development Team database.
 *
 *  In addition to the locations defined in the document, there's types for:
 *
 *  - Switches, formatted like 'd_Rxx-Mx',
 *    where d is a dimension in A,B,C,D, and Rxx-Mx is a midplane location.
 *
 *  - I/O links on node boards, like 'I_Rxx-Mx-Nxx-Txx'
 *    where Rxx-Mx-Nxx-Txx is a port on a node board
 *
 *  - All node cards on a midplane, like 'Rxx-Mx-N'.
 *    This is a valid pseudo-location for a service action.
 */
class Location {
public:
    /*!
     * \brief The type of hardware that the location refers to.
     */
    enum Type {
        ComputeRack,
        IoRack,
        BulkPowerSupplyOnComputeRack,
        PowerModuleOnComputeRack,
        BulkPowerSupplyInIoRack,
        PowerModuleOnIoRack,
        Midplane,
        ServiceCard,
        ClockCardOnComputeRack,
        ClockCardOnIoRack,
        IoBoardOnComputeRack,
        IoBoardOnIoRack,
        NodeBoard,
        ComputeCardOnNodeBoard,
        ComputeCardOnIoBoard,
        ComputeCardCoreOnNodeBoard,
        ComputeCardCoreOnIoBoard,
        DcaOnNodeBoard,
        PciAdapterCard,
        LinkModuleOnNodeBoard,
        LinkModuleOnIoBoardInIoRack,
        LinkModuleOnIoBoardInComputeRack,
        OpticalModuleOnNodeBoard,
        OpticalModuleOnIoBoard,
        FanAssemblyInComputeRack,
        FanAssemblyInIoRack,
        FanInComputeRack,
        FanInIoRack,
        LeakDetectorInComputeRack,
        Switch,
        IoLinkOnNodeBoard,
        AllNodeBoardsOnMidplane,
        NotValid
    };


    enum ThrowsSpec {
        nothrow,
        throws_on_bad_location
    };


    /*!
     *  \brief Switch dimensions.
     */
    struct Dimension {
        enum Value {
            A, B, C, D, NONE
        };
    };


    static const unsigned ComputeRackLength = 3;
    static const unsigned IoRackLength = 3;
    static const unsigned BulkPowerSupplyOnComputeRackLength = 6;
    static const unsigned PowerModuleOneComputeRackLength = 9;
    static const unsigned BulkPowerSupplyInIoRackLength = 5;
    static const unsigned PowerModuleOnIoRackLength = 8;
    static const unsigned MidplaneLength = 6;
    static const unsigned ServiceCardLength = 8;
    static const unsigned ClockCardOnComputeRackLength = 5;
    static const unsigned ClockCardOnIoRackLength = 6;
    static const unsigned IoBoardOnComputeRackLength = 6;
    static const unsigned IoBoardOnIoRackLength = 6;
    static const unsigned NodeBoardLength = 10;
    static const unsigned ComputeCardOnNodeBoardLength = 14;
    static const unsigned ComputeCardOnIoBoardLength = 10;
    static const unsigned ComputeCardCoreOnNodeBoardLength = 18;
    static const unsigned ComputeCardCoreOnIoBoardLength = 14;
    static const unsigned DcaOnNodeBoardLength = 13;
    static const unsigned PciAdapterCardLength = 9;
    static const unsigned LinkModuleOnNodeBoardLength = 14;
    static const unsigned LinkModuleOnIoBoardLength = 10;
    static const unsigned OpticalModuleOnNodeBoardLength = 14;
    static const unsigned OpticalModuleOnIoBoardLength = 10;
    static const unsigned FanAssemblyInComputeRackLength = 9;
    static const unsigned FanAssemblyInIoRackLength = 9;
    static const unsigned FanInComputeRackLength = 12;
    static const unsigned FanInIoRackLength = 12;
    static const unsigned LeakDetectorInComputeRackLength = 5;
    static const unsigned SwitchLength = 8;
    static const unsigned IoLinkOnNodeBoardLength = 16;
    static const unsigned AllNodeBoardsOnMidplaneLength = 8;

    static const unsigned IoBoardsOnComputeRack = 4;
    static const unsigned IoBoardsOnIoRack = 12;

    static const unsigned ComputeCardsOnNodeBoard = 32;
    static const unsigned ComputeCardsOnIoBoard = 8;

    static const unsigned ComputeCardCoresOnBoard = 17;

    static const unsigned OpticalModulesOnNodeBoard = 36;
    static const unsigned OpticalModulesOnIoBoard = 24;

    static const unsigned NodeBoardsOnMidplane = 16;


    /*!
     *  \brief Constructor for uninitialized location.
     *
     *  The Location object isn't valid until the set method is called.
     */
    Location();

    /*!
     *  \brief Constructor from std::string.
     *
     *  Refer to the class documentation for the location format.
     *
     *  \throws LocationError if the location string isn't valid and throws_spec is throws_on_bad_location
     *  \throws std::runtime_error if an internal error occurred.
     */
    explicit Location( const std::string& location, ThrowsSpec throws_spec = throws_on_bad_location );

    /*!
     *  \brief Set or reset the location.
     *
     *  Refer to the class documentation for the location format.
     *
     *  \throws LocationError if the location string isn't valid and throws_spec is throws_on_bad_location
     *  \throws std::runtime_error if an internal error occurred.
     */
    void set( const std::string& location, ThrowsSpec throws_spec = throws_on_bad_location  );


    /*!
     *  \brief Get the location string.
     *
     *  The location string returned by getLocation() will have any whitespace
     *  trimmed from the original location.
     */
    const std::string& getLocation() const  { return _location; }

    operator std::string() const  { return _location; }


    Type getType() const  { return _type; }

    std::string getMidplaneLocation() const;

    Type getRackType() const;

    uint8_t getRackRow() const;

    uint8_t getRackColumn() const;

    uint8_t getBulkPowerSupply() const;

    uint8_t getPowerModule() const;

    uint8_t getMidplane() const;

    uint8_t getClockCard() const;

    uint8_t getIoBoard() const;

    uint8_t getNodeBoard() const;

    uint8_t getFan() const;

    uint8_t getComputeCard() const;

    uint8_t getCore() const;

    uint8_t getDCA() const;

    uint8_t getPciAdapterCard() const;

    uint8_t getLinkModule() const;

    uint8_t getOpticalModule() const;

    uint8_t getFanAssembly() const;

    Dimension::Value getSwitchDimension() const;

    uint8_t getPort() const;


private:

    std::string _location;

    Type _type;

    Type _rack_type;
    uint8_t _rack_row;
    uint8_t _rack_column;
    uint8_t _bulk_power_supply;
    uint8_t _power_module;
    uint8_t _midplane;
    uint8_t _clock_card;
    uint8_t _io_board;
    uint8_t _node_board;
    uint8_t _fan;
    uint8_t _compute_card;
    uint8_t _core;
    uint8_t _dca;
    uint8_t _pci_adapter_card;
    uint8_t _link_module;
    uint8_t _optical_module;
    uint8_t _fan_assembly;
    Dimension::Value _switch_dimension;
    uint8_t _port;


    void _extractRackType( regmatch_t matches[], int* match_idx_in_out );
    void _extractRackSubs( regmatch_t matches[], int* match_idx_in_out );
    void _extractBulkPowerSupply( regmatch_t matches[], int* match_idx_in_out );
    void _extractPowerModule( regmatch_t matches[], int* match_idx_in_out );
    void _extractMidplane( regmatch_t matches[], int* match_idx_in_out );
    void _extractClockCard( regmatch_t matches[], int* match_idx_in_out );
    void _extractIoBoard( regmatch_t matches[], int* match_idx_in_out );
    void _extractNodeBoard( regmatch_t matches[], int* match_idx_in_out );
    void _extractFan( regmatch_t matches[], int* match_idx_in_out );
    void _extractComputeCard( regmatch_t matches[], int* match_idx_in_out );
    void _extractCore( regmatch_t matches[], int* match_idx_in_out );
    void _extractDCA( regmatch_t matches[], int* match_idx_in_out );
    void _extractPciAdapter( regmatch_t matches[], int* match_idx_in_out );
    void _extractLinkModule( regmatch_t matches[], int* match_idx_in_out );
    void _extractOpticalModule( regmatch_t matches[], int* match_idx_in_out );
    void _extractFanAssembly( regmatch_t matches[], int* match_idx_in_out );
    void _extractPort( regmatch_t matches[], int* match_idx_in_out );
};


inline std::ostream& operator<<( std::ostream& os, const Location& location )  { return os << std::string(location); }


class LocationError : public std::invalid_argument
{
public:
    LocationError( const std::string& what );
};


} // namespace bgq::util
} // namespace bgq


#endif
