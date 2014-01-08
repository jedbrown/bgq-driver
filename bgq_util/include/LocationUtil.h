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
/* (C) Copyright IBM Corp.  2006, 2011                              */
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

#ifndef _LOCATIONUTIL_H_
#define _LOCATIONUTIL_H_

#include "Regexp.h"
#include <xml/include/c_api/MCServerMessageSpec.h>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

static const unsigned allPorts[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,35 };
static const unsigned noPorts[] = {};
static const std::vector<unsigned> allJtagPorts( allPorts, allPorts+36 );
static const std::vector<unsigned> noJtagPorts( noPorts, noPorts );
static const std::string hexchars = "1234568790ABCDEFabcdef";
static const char computeRack = 'R';
static const char ioRack = 'Q';

static const std::string midplanes[] = {
    "-M0",
    "-M1" };
static const unsigned numMidplanes = 2;

static const std::string fanAssemblies[] = {
    "-H0",
    "-H1",
    "-H2" };
static const unsigned numFanAssemblies = 3;


static const std::string fans[] = {
    "-F0",
    "-F1" };
static const unsigned numFans = 2;

static const std::string clockCards[] = {
    "-K0",
    "-K1" };
static const unsigned numClockCards = 2;

static const std::string leakDetectors[] = {
    "-L" };
static const unsigned numLeakDetectors = 1;

static const std::string bulkPowerEnclosures[] = {
    "-B0",
    "-B1",
    "-B2",
    "-B3" };
static const unsigned numBulkPowerEnclosures = 4;

static const std::string bulkPowerModules[] = {
    "-P0",
    "-P1",
    "-P2",
    "-P3",
    "-P4",
    "-P5",
    "-P6",
    "-P7",
    "-P8" };
static const unsigned numBulkPowerModules = 9;

static const std::string linkModules[] = {
    "-U00",
    "-U01",
    "-U02",
    "-U03",
    "-U04",
    "-U05",
    "-U06",
    "-U07",
    "-U08" };
static const unsigned numLinkModules = 9;
static const unsigned numLinkModulesOnIoBoard = 6;

static const std::string pciAdapterCards[] = {
    "-A0",
    "-A1",
    "-A2",
    "-A3",
    "-A4",
    "-A5",
    "-A6",
    "-A7" };
static const unsigned numPCIAdapterCards = 8;

static const std::string opticalModules[] = {
    "-O00",
    "-O01",
    "-O02",
    "-O03",
    "-O04",
    "-O05",
    "-O06",
    "-O07",
    "-O08",
    "-O09",
    "-O10",
    "-O11",
    "-O12",
    "-O13",
    "-O14",
    "-O15",
    "-O16",
    "-O17",
    "-O18",
    "-O19",
    "-O20",
    "-O21",
    "-O22",
    "-O23",
    "-O24",
    "-O25",
    "-O26",
    "-O27",
    "-O28",
    "-O29",
    "-O30",
    "-O31",
    "-O32",
    "-O33",
    "-O34",
    "-O35",
};
static const unsigned numOpticalModules = 36;
static const unsigned numOpticalModulesOnIoBoard = 24;

static const std::string linkCables[] = {
    "-E0",
    "-E1",
    "-E2",
    "-E3",
    "-E4",
    "-E5" };
static const unsigned numLinkCables= 6;


static const std::string computeNodes[] = {
    "-J00",
    "-J01",
    "-J02",
    "-J03",
    "-J04",
    "-J05",
    "-J06",
    "-J07",
    "-J08",
    "-J09",
    "-J10",
    "-J11",
    "-J12",
    "-J13",
    "-J14",
    "-J15",
    "-J16",
    "-J17",
    "-J18",
    "-J19",
    "-J20",
    "-J21",
    "-J22",
    "-J23",
    "-J24",
    "-J25",
    "-J26",
    "-J27",
    "-J28",
    "-J29",
    "-J30",
    "-J31" };
static const unsigned numComputeNodes = 32;
static const unsigned numComputeNodesOnIoBoard = 8;

static const std::string TorusIoPorts[] = {
    "-T00",
    "-T01",
    "-T02",
    "-T03",
    "-T04",
    "-T05",
    "-T06",
    "-T07",
    "-T08",
    "-T09",
    "-T10",
    "-T11",
    "-T12",
    "-T13",
    "-T14",
    "-T15",
    "-T16",
    "-T17",
    "-T18",
    "-T19",
    "-T20",
    "-T21",
    "-T22",
    "-T23"};

static const unsigned numTorusIoPortsOnNodeBoard = 12;
static const unsigned numTorusIoPortsOnIoBoard = 24;



// an array that maps the XX int of a JXX location to JTag address
//#define jtagPorts PROCESSOR_CARD_JTAG_PORT

static const std::string _processorCardNames [] =
{"J00", "J01", "J02", "J03", "J04", "J05", "J06", "J07",
    "J08", "J09", "J10", "J11", "J12", "J13", "J14", "J15",
    "J16", "J17", "J18", "J19", "J20", "J21", "J22", "J23",
    "J24", "J25", "J26", "J27", "J28", "J29", "J30", "J31"};



static const std::string nodeBoards [] = {
    "-N00",
    "-N01",
    "-N02",
    "-N03",
    "-N04",
    "-N05",
    "-N06",
    "-N07",
    "-N08",
    "-N09",
    "-N10",
    "-N11",
    "-N12",
    "-N13",
    "-N14",
    "-N15"
};
static const unsigned numNodeBoards = 16;

static const std::string ioBoards [] = {
    "-I0",
    "-I1",
    "-I2",
    "-I3",
    "-I4",
    "-I5",
    "-I6",
    "-I7",
    "-I8",
    "-I9",
    "-IA",
    "-IB",
    "-IC",
    "-ID",
    "-IE",
    "-IF"
};
static const unsigned numIoBoards = 12;
static const unsigned startIoBoardsInComputeRack = numIoBoards;
static const unsigned numIoBoardsInComputeRack = 4;


static const std::string dcas [] = {
    "-D0",
    "-D1"
};
static const unsigned numDCAs = 2;
static const unsigned numDCAsonIOBoard = 1;

static const std::string serviceCards [] = {

    "-S"
};
static const unsigned numServiceCards = 1;

static const std::string virtualCards [] = {
    "-VC"
};
static const unsigned numVirtualCards = 1;

class LocationUtil;

/*!
 * Struct required by map
 * Master sort routine for Locations.  All lists of locations passed to
 * mc must be sorted in the same order to avoid deadlocks with fine-grain locking.
 */
struct CardLocCompare
{
    bool operator() (const std::string& loc1, const std::string& loc2 ) const
    {
        return LocCompare( loc1, loc2 );
    };

    static bool LocCompare (const std::string& loc1, const std::string& loc2 )
    {
        return ( loc1.compare( loc2 ) < 0  );
    };

};


/*!
 * \brief Card Location utilities
 * This class has static functions for utilities to convert from string location names to MC_CardLocation objects.
 */
class LocationUtil 
{
public:
    /*!
     * \brief Returns true if location exists in machine set.
     * 
     * \param[in] string set representing existing machine hardware
     * \param[in] string the location to check for
     * 
     * \return true if location found in set
     */
    static bool isInMachine(const std::set<std::string>& machine, const std::string& location) {
        return (machine.find(location) != machine.end());
    };

    /*!
     * \brief Creates an array of strings for all racks.
     *
     * \param[in]  int number of rows
     * \param[in]  int number of columns
     *
     * \return array of strings, e.g. "R00R01R10R11".
     */
    static char*
    CreateRackList( unsigned rows, unsigned columns, char rackType = 'R' )
    {
        char* racks = (char*)calloc( rows*columns, 3 );
        for( unsigned r=0; r<rows; r++ )
        {
            for( unsigned c=0; c<columns; c++ )
            {
                std::ostringstream rack;
                rack << std::uppercase << std::hex << r << c;
                unsigned charpos = 3*(r+c+(columns-1)*r);
                racks[charpos] = rackType;
                racks[charpos+1] = rack.str()[0];
                racks[charpos+2] = rack.str()[1];
            }
        }
        return racks;
    };

    /*!
     * \brief Return the location type.
     *
     * \param[in] location string
     * \return MCServerMessageSpec::LocationType 
     */
    static MCServerMessageSpec::LocationType getLocationType (const std::string& location)
    {
        // Note: the ordering of these checks is significant, you want to go from 
        // more specific to less.  Length checks would make this more robust
        //if( location.find('') != location.npos )
        //return MCServerMessageSpec::LinkPort;
        if( location.find('U') != location.npos )
            return MCServerMessageSpec::LinkModule;
        if( location.find('J') != location.npos )
            return MCServerMessageSpec::ComputeCard;
        if( (location.find('I') != location.npos) && (location.size()== 6)  )
            return MCServerMessageSpec::IoBoard;
        if( location.find('T') != location.npos  )
            return MCServerMessageSpec::TorusIoPort;
        if( location.find('O') != location.npos )
            return MCServerMessageSpec::OpticalModule;
        if( location.find('S') != location.npos )
            return MCServerMessageSpec::ServiceCard;
        if( location.find("-A") != location.npos )
            return MCServerMessageSpec::PCIAdapterCard;
        if( location.find('D') != location.npos && location.size() == 13 )
            return MCServerMessageSpec::DCA;
        if( location.find('K') != location.npos )
            return MCServerMessageSpec::Clock;
        if( location.find('H') != location.npos )
            return MCServerMessageSpec::FanAssembly;
        if( location.find('F') != location.npos && location.size() == 9 )
            return MCServerMessageSpec::Fan;
        if( location.find('P') != location.npos )
            return MCServerMessageSpec::BulkPowerModule;
        if( location.find('B') != location.npos && (location.size() == 5 || location.size() == 6))
            return MCServerMessageSpec::BulkPowerEnclosure;
        if( location.find('N') != location.npos ) 
            return MCServerMessageSpec::NodeBoard;
        if( location.find('M') != location.npos  )
            return MCServerMessageSpec::Midplane;
        return MCServerMessageSpec::any;
    };
};

#endif //_LOCATIONUTIL_H_
