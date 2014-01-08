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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#include <stdlib.h>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <string.h>
#include <string>

#include <bgq_util/include/Location.h>

#include <utility/include/CableBadWires.h>
#include <utility/include/Log.h>

LOG_DECLARE_FILE( "utility" );

using bgq::util::Location;
using bgq::util::LocationError;
using namespace std;

/**
 * Link chip registers
 */
const std::string REG_C01 = "C01";	// Receiver
const std::string REG_C23 = "C23";	// Receiver
const std::string REG_D01 = "D01";	// Transmitter
const std::string REG_D23 = "D23";	// Transmitter

/**
 * LINK CHIP REGISTER CONSTANTS
 */
const int C01 = 1;
const int C23 = 2;
const int D01 = 3;
const int D23 = 4;

/**
 * NODE BOARD TORUS CABLE (Rxx-Mx-Nxx-T04:T11):
 * The table that denotes the number of bits to shift left to convert the bad fiber mask to the
 * bad wire mask from a link chip position (0-8) for Torus cables. -1 denotes an invalid link chip.
 */
const int NodeTorusShift[9] = { 36, 36, 24, 24, -1, 12, 12, 0, 0 };

/**
 * NODE BOARD TORUS CABLE (Rxx-Mx-Nxx-T04:T11):
 * The table that derives the Torus cable port position on the IO drawer (T04-T11) from the link chip
 * position (0-8) and registers (C01, C23, D01, D23) with the bad fibers.
 */
const int NodeTorusPort[9][4] = {
			{ 11, 8, 9, 10 },	// U00
			{ 7, 4, 5, 6 },		// U01
			{ 7, 4, 5, 6 },		// U02
			{ 11, 8, 9, 10 },	// U03
			{ -1, -1, -1, -1 },	// U04
			{ 11, 8, 9, 10 },	// U05
			{ 7, 4, 5, 6 },		// U06
			{ 7, 4, 5, 6 },		// U07
			{ 11, 8, 9, 10 }	// U08
};

/**
 * NODE BOARD IO CABLE (Rxx-Mx-Nxx-T00:T03):
 * The table that denotes the number of bits to shift left to convert the bad fiber mask to the
 * bad wire mask from Link chip U04's registers (C01[0:5], C01[6:11], C23[0:5], C23[6:11], D01[0:5],
 * D01[6:11], D23[0:5], C01[6:11]) for for the IO cable.
 */
const int NodeIoShift[8] = { 6, 6, 6, 6, 0, 0, 0, 0 };

/**
 * NODE BOARD IO CABLE (Rxx-Mx-Nxx-T00:T03):
 * The table that derives the Torus cable port position on the node board (T00-T03) from link chip
 * U04's registers (C01[0:5], C01[6:11], C23[0:5], C23[6:11], D01[0:5], D01[6:11], D23[0:5], D23[6:11])
 * with the bad fibers.
 */
const int NodeIoPort[8] = { 2, 3, 0, 1, 0, 1, 2, 3 };

/**
 * IO DRAWER IO CABLE (Rxx-Ix-T00:T23):
 * The table that denotes the number of bits to shift left to convert the bad fiber mask to the
 * bad wire mask from a given link chip position (0-5) and registers (C01[0:5], C01[6:11], C23[0:5],
 * C23[6:11], D01[0:5], D01[6:11], D23[0:5], D23[6:11]) for the Torus cable.
 */
const int IoIoShift[6][8] = {
			{ 6, 6, 0, 0, 6, 6, 0, 0 },	// U00
			{ 6, 6, 0, 0, 6, 6, 0, 0 },	// U01
			{ 6, 0, 0, 0, 6, 6, 0, 6 },	// U02
			{ 6, 0, 0, 0, 6, 6, 0, 6 },	// U03
			{ 6, 6, 0, 0, 6, 6, 0, 0 },	// U04
			{ 6, 6, 0, 0, 6, 6, 0, 0 },	// U05
};

/**
 * IO DRAWER IO CABLE (Rxx-Ix-T00:T23):
 * The table that derives the Torus cable port position on the IO drawer (T00-T23) from the link chip
 * position (0-5) and registers (C01[0:5], C01[6:11], C23[0:5], C23[6:11], D01[0:5], D01[6:11],
 * D23[0:5], D23[6:11]) with the bad fibers.
 */
const int IoIoPort[6][8] = {
			{ 5, 7, 1, 3, 1, 3, 5, 7 },			// U00
			{ 13, 15, 9, 11, 9, 11, 13, 15 },	// U01
			{ 21, 23, 17, 19, 17, 19, 21, 23 },	// U02
			{ 16, 18, 20, 22, 20, 22, 16, 18 },	// U03
			{ 8, 10, 12, 14, 12, 14, 8, 10 },	// U04
			{ 0, 2, 4, 6, 4, 6, 0, 2 }			// U05
};

/**
 * IO PORT TO LINK CHIP
 * The table that converts the IO drawer port index (0-23 for T00-T23) to the link chip index
 * (0-5 for U00-U05).
 */
const int IoPortToLinkChip[24] = {
		5, 0, 5, 0, 5, 0, 5, 0,	// T00-T07
		4, 1, 4, 1, 4, 1, 4, 1, // T08-T15
		3, 2, 3, 2, 3, 2, 3, 2	// T16-T23
};

/**
 * IO PORT TO LINK CHIP REGISTER
 * This table converts the transmitting IO drawer port index (0-23) to the transmitting
 * link chip register.
 */
const string IoTxReg[24] = {
		REG_D23, REG_D01, REG_D23, REG_D01, REG_D01, REG_D23, REG_D01, REG_D23,	// T00-T07
		REG_D23, REG_D01, REG_D23, REG_D01,	REG_D01, REG_D23, REG_D01, REG_D23,	// T08-T15
		REG_D23, REG_D01, REG_D23, REG_D01,	REG_D01, REG_D23, REG_D01, REG_D23	// T16-T23
};

/**
 * TORUS PORT TO LINK CHIP REGISTER
 * This table converts the transmitting node board port index (0-11) to the transmitting
 * link chip register.
 */
const string NodeTxReg[12] = {
		REG_D01, REG_D01, REG_D23, REG_D23, "", REG_D01, REG_D23, "", "", REG_D01, REG_D23, ""	// T00-T11
}; // (Note: T04, T07, T08, T11 are receiving ports and not applicable.)

/**
 * Converts the position number (0-35) to string
 */
const string PosToStr[36] = {
			"00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
			"10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
			"20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
			"30", "31", "32", "33", "34", "35"
};

namespace bgq {
namespace utility {

CableBadWires::CableBadWires(const string& location, const string& reg, int badFiberMask)
: _rxIsNodeBoard(false),
  _rxIsIoDrawer(false),
  _isTorusCable(false),
  _rxLinkChipPos(-1),
  _txIsNodeBoard(false),
  _txIsIoDrawer(false),
  _txPortPos(-1),
  _txbadfibers(0)
{
	_badfibers = badFiberMask & 0xfff; // Copy the least significatn 12 bits only
	// Get the link chip index (0-based) and the node/IO board location from the location.
	string boardLocation;
	if (true == (_rxIsNodeBoard = isNodeBoardLinkChipLocation(location))) {
		_rxLinkChipPos = charToInt(location[13]);
		if (_rxLinkChipPos != 4) {
			_isTorusCable = true;
		}
		boardLocation = location.substr(0,10);
	}
	else if (true == (_rxIsIoDrawer = isIoDrawerLinkChipLocation(location))) {
		_rxLinkChipPos = charToInt(location[9]);
		boardLocation = location.substr(0,6);
	}
	else {	// Invalid location detected.
		_errormessage = "Invalid location";
		LOG_DEBUG_MSG( _errormessage );
		throw invalid_argument(_errormessage);
	}
	LOG_DEBUG_MSG(  
                "boardLocation=" << boardLocation << " nodeBoard=" << _rxIsNodeBoard
                << " ioDrawer=" << _rxIsIoDrawer << " torusCable=" << _isTorusCable
                );

	// Verify the register designation.
	if (0 == strcmp(reg.c_str(), REG_C01.c_str())) {
		_register = C01;
	}
	else if (0 == strcmp(reg.c_str(), REG_C23.c_str())) {
		_register = C23;
	}
	else if (0 == strcmp(reg.c_str(), REG_D01.c_str())) {
		_register = D01;
	}
	else if (0 == strcmp(reg.c_str(), REG_D23.c_str())) {
		_register = D23;
	}
	else {	// Invalid register.
		_errormessage = "Invalid link chip register";
		LOG_DEBUG_MSG( _errormessage );
		throw invalid_argument(_errormessage);
	}

	// Verify and store the bad fiber mask.
	if (badFiberMask == 0) {
		_errormessage = "Invalid bad fiber mask value of 0";
		LOG_DEBUG_MSG( _errormessage );
		throw invalid_argument(_errormessage);
	}

	// Process the Torus cable and IO cable in the node board.
	if (_rxIsNodeBoard) {
		if (_isTorusCable) {
			processNodeBoardTorusCable(boardLocation, _rxLinkChipPos);
		}
		else {
			processNodeBoardIoCable(boardLocation, _rxLinkChipPos);
		}
	}
	// Process the IO cable in the IO drawer.
	else {
		processIoDrawerIoCable(boardLocation, _rxLinkChipPos);
	}
	LOG_DEBUG_MSG( "_port=" << _port << " _badwiremask=" << hex << _badwiremask );

} // End ctor


string CableBadWires::getRxPortLocation() const
{
	return _port;

} // End getRxPortLocation(...)


string CableBadWires::getPortLocation() const	// WILL BE DELETED
{
	return _port;

} // End getPortLocation(...)


long int CableBadWires::getBadWireMask() const
{
	return _badwiremask;

} // End getBadWireMask(...)


string CableBadWires::getErrorMessage() const
{
	return _errormessage;

} // End getErrorMessage(...)


void CableBadWires::setTxPortAndBadWireMask(const string& location, uint64_t badWireMask)
{
	// Verify the input port location.
	_txIsIoDrawer = isIoDrawerPortLocation(location);
	if (true == (_txIsNodeBoard = isNodeBoardPortLocation(location))) { // Rxx-Mx-Nxx-Txx
		_fromport = location;
		_txPortPos = atoi(location.substr(12,14).c_str());
	}
	else if (true == (_txIsIoDrawer = isIoDrawerPortLocation(location))) { // Rxx-Ix-Txx
		_fromport = location;
		_txPortPos = atoi(location.substr(8,10).c_str());
	}
	else {	// Invalid location detected.
		_errormessage = "Invalid port location";
		LOG_DEBUG_MSG( _errormessage );
		throw invalid_argument(_errormessage);
	}

	// Determine the transmitting link chip location based on the input "from port" location.
	if (_txIsNodeBoard) { // Rxx-Mx-Nxx-Txx
		if (_isTorusCable) {
			_txlinkchip = location.substr(0,10) + "-U" + PosToStr[_rxLinkChipPos];
		}
		else {
			_txlinkchip = location.substr(0,10) + "-U04";
		}
	}
	else { // Rxx-Ix-Txx
		int linkChipPos = IoPortToLinkChip[_txPortPos];
		_txlinkchip = location.substr(0,6) + "-U" + PosToStr[linkChipPos];
	}

	// Determine the transmitting link chip register based on the input "from port" location.
	if (_txIsNodeBoard) { // Rxx-Mx-Nxx-Txx
		_txregister = NodeTxReg[_txPortPos];
	}
	else { // Rxx-Ix-Txx
		_txregister = IoTxReg[_txPortPos];
	}

	// Determine the aggregated bad fiber mask based on the aggregated bad wire mask.
	if (_txIsNodeBoard) { // Rxx-Mx-Nxx-Txx
		if (_isTorusCable) {
			_txbadfibers = getFibersFromWiresForTorusCable(_rxLinkChipPos, badWireMask);
		}
		else {
			_txbadfibers = getFibersFromWiresForIoCable(_rxLinkChipPos, badWireMask);
		}
	}
	else { // Rxx-Ix-Txx
		_txbadfibers = getFibersFromWiresForIoCable(_rxLinkChipPos, badWireMask);
	}

	LOG_DEBUG_MSG( 
                "setTxPortAndBadWireMask(): fromport=" << _fromport << "(" << dec << _txPortPos
                << ") node/IO=" << _txIsNodeBoard << "/" << _txIsIoDrawer
                << " TxLinkChip=" << _txlinkchip << "(" << _txregister << ")"
                << " AggFiberMask=" << hex << _txbadfibers
                );

} // End setTxPortAndBadWireMask(...)


string CableBadWires::getTxLinkChipLocation() const
{
	return _txlinkchip;

} // End getTxLinkChipLocation(...)


string CableBadWires::getTxRegister() const
{
	return _txregister;

} // End setTxPortAndBadWireMask(...)


long int CableBadWires::getAggregatedBadFiberMask() const
{
	return _txbadfibers;

} // End getAggregatedBadFiberMask(...)


//
// PRIVATE METHODS:
//


void CableBadWires::processNodeBoardTorusCable(const string& location, int linkChipIndex)
{
	LOG_DEBUG_MSG(
                "processNodeBoardTorusCable: location=" << location << " linkChipIndex=" << linkChipIndex
                << " _register=" << _register <<  " _badfibers=" << _badfibers
                );
	_badwiremask = _badfibers;
	switch(_register) {
		case C01:
			_badwiremask <<= NodeTorusShift[linkChipIndex];
			_port = location + "-T" + PosToStr[NodeTorusPort[linkChipIndex][0]];
			break;
		case C23:
			_badwiremask <<= NodeTorusShift[linkChipIndex];
			_port = location + "-T" + PosToStr[NodeTorusPort[linkChipIndex][1]];
			break;
		case D01:
			_badwiremask <<= NodeTorusShift[linkChipIndex];
			_port = location + "-T" + PosToStr[NodeTorusPort[linkChipIndex][2]];
			break;
		case D23:
			_badwiremask <<= NodeTorusShift[linkChipIndex];
			_port = location + "-T" + PosToStr[NodeTorusPort[linkChipIndex][3]];
			break;
	}
	LOG_DEBUG_MSG( 
                "processNodeBoardTorusCable: _port=" << _port << " _badwiremask=" << hex << _badwiremask
                );

} // End processNodeBoardTorusCable(...)


void CableBadWires::processNodeBoardIoCable(const string& location, int linkChipIndex)
{
	LOG_DEBUG_MSG( __FUNCTION__ << " location=" << location << " linkChipIndex=" << linkChipIndex );
	// Separate bad fiber mask into corresponding registers.
	int c01_0_5 = 0;
	int c01_6_11 = 0;
	int c23_0_5 = 0;
	int c23_6_11 = 0;
	int d01_0_5 = 0;
	int d01_6_11 = 0;
	int d23_0_5 = 0;
	int d23_6_11 = 0;
	switch(_register) {
		case C01:
			c01_0_5 = (_badfibers & 0xfc0) >> 6;
			c01_6_11 = _badfibers & 0x3f;
			break;
		case C23:
			c23_0_5 = (_badfibers & 0xfc0) >> 6;
			c23_6_11 = _badfibers & 0x3f;
			break;
		case D01:
			d01_0_5 = (_badfibers & 0xfc0) >> 6;
			d01_6_11 = _badfibers & 0x3f;
			break;
		case D23:
			d23_0_5 = (_badfibers & 0xfc0) >> 6;
			d23_6_11 = _badfibers & 0x3f;
			break;
	}
        LOG_DEBUG_MSG( 
                "c01_0_5=" << hex << c01_0_5 << " c01_6_11=" << hex << c01_6_11
                << " c23_0_5=" << hex << c23_0_5 << " c23_6_11=" << hex << c23_6_11
                << " d01_0_5=" << hex << d01_0_5 << " d01_6_11=" << hex << d01_6_11
                << " d23_0_5=" << hex << d23_0_5 << " d23_6_11=" << hex << d23_6_11
                );

	// Determine the bad wire mask based on the link chip index.
	_badwiremask = 0;
	if (c01_0_5 != 0) {
		_badwiremask |= (c01_0_5 << NodeIoShift[0]);
		_port = location + "-T" + PosToStr[NodeIoPort[0]];
	}
	else if (c23_0_5 != 0) {
		_badwiremask |= (c23_0_5 << NodeIoShift[2]);
		_port = location + "-T" + PosToStr[NodeIoPort[2]];
	}
	else if (d01_0_5 != 0) {
		_badwiremask |= (d01_0_5 << NodeIoShift[4]);
		_port = location + "-T" + PosToStr[NodeIoPort[4]];
	}
	else if (d23_0_5 != 0) {
		_badwiremask |= (d23_0_5 << NodeIoShift[6]);
		_port = location + "-T" + PosToStr[NodeIoPort[6]];
	}
	if (c01_6_11 != 0) {
		_badwiremask |= (c01_6_11 << NodeIoShift[1]);
		_port = location + "-T" + PosToStr[NodeIoPort[1]];
	}
	else if (c23_6_11 != 0) {
		_badwiremask |= (c23_6_11 << NodeIoShift[3]);
		_port = location + "-T" + PosToStr[NodeIoPort[3]];
	}
	else if (d01_6_11 != 0) {
		_badwiremask |= (d01_6_11 << NodeIoShift[5]);
		_port = location + "-T" + PosToStr[NodeIoPort[5]];
	}
	else if (d23_6_11 != 0) {
		_badwiremask |= (d23_6_11 << NodeIoShift[7]);
		_port = location + "-T" + PosToStr[NodeIoPort[7]];
	}

} // End processNodeBoardIoCable(...)


void CableBadWires::processIoDrawerIoCable(const string& location, int linkChipIndex)
{
	LOG_DEBUG_MSG( __FUNCTION__ << ": location=" << location << " linkChipIndex=" << linkChipIndex );
	// Separate bad fiber mask into corresponding registers.
	int c01_0_5 = 0;
	int c01_6_11 = 0;
	int c23_0_5 = 0;
	int c23_6_11 = 0;
	int d01_0_5 = 0;
	int d01_6_11 = 0;
	int d23_0_5 = 0;
	int d23_6_11 = 0;
	switch(_register) {
		case C01:
			c01_0_5 = (_badfibers & 0xfc0) >> 6;
			c01_6_11 = _badfibers & 0x3f;
			break;
		case C23:
			c23_0_5 = (_badfibers & 0xfc0) >> 6;
			c23_6_11 = _badfibers & 0x3f;
			break;
		case D01:
			d01_0_5 = (_badfibers & 0xfc0) >> 6;
			d01_6_11 = _badfibers & 0x3f;
			break;
		case D23:
			d23_0_5 = (_badfibers & 0xfc0) >> 6;
			d23_6_11 = _badfibers & 0x3f;
			break;
	}
	// Determine the bad wire mask based on the link chip index.
	_badwiremask = 0;
	if (c01_0_5 != 0) {
		_badwiremask |= (c01_0_5 << IoIoShift[linkChipIndex][0]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][0]];
	}
	else if (c23_0_5 != 0) {
		_badwiremask |= (c23_0_5 << IoIoShift[linkChipIndex][2]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][2]];
	}
	else if (d01_0_5 != 0) {
		_badwiremask |= (d01_0_5 << IoIoShift[linkChipIndex][4]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][4]];
	}
	else if (d23_0_5 != 0) {
		_badwiremask |= (d23_0_5 << IoIoShift[linkChipIndex][6]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][6]];
	}
	if (c01_6_11 != 0) {
		_badwiremask |= (c01_6_11 << IoIoShift[linkChipIndex][1]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][1]];
	}
	else if (c23_6_11 != 0) {
		_badwiremask |= (c23_6_11 << IoIoShift[linkChipIndex][3]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][3]];
	}
	else if (d01_6_11 != 0) {
		_badwiremask |= (d01_6_11 << IoIoShift[linkChipIndex][5]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][5]];
	}
	else if (d23_6_11 != 0) {
		_badwiremask |= (d23_6_11 << IoIoShift[linkChipIndex][7]);
		_port = location + "-T" + PosToStr[IoIoPort[linkChipIndex][7]];
	}

} // End processIoDrawerIoCable(...)


int CableBadWires::getFibersFromWiresForTorusCable(int linkChipIndex, uint64_t badWireMask) const
{
	badWireMask &= 0xffffffffffffull;	// Use the lower 48 bits only.
	switch(_register) {
		case C01:
			badWireMask >>= NodeTorusShift[linkChipIndex];
			break;
		case C23:
			badWireMask >>= NodeTorusShift[linkChipIndex];
			break;
		case D01:
			badWireMask >>= NodeTorusShift[linkChipIndex];
			break;
		case D23:
			badWireMask >>= NodeTorusShift[linkChipIndex];
			break;
	}
	int badFiberMask = badWireMask & 0xfff;	// Copy the lower 12 bits only.
	LOG_DEBUG_MSG( 
                __FUNCTION__ << ": linkChipIndex=" << linkChipIndex 
                << " wires=" << hex << badWireMask << " fibers=" << hex << badFiberMask
                );
                return badFiberMask;

} // End getFibersFromWiresForTorusCable(...)


int CableBadWires::getFibersFromWiresForIoCable(int linkChipIndex, uint64_t badWireMask) const
{
	int badFiberMask = badWireMask & 0xfff;	// Copy the lower 12 bits only.
	LOG_DEBUG_MSG(
                __FUNCTION__ << ": linkChipIndex=" << linkChipIndex
                << " wires=" << hex << badWireMask << " fibers=" << hex << badFiberMask
                );
	return badFiberMask;

} // End getFibersFromWiresForIoCable(...)


bool CableBadWires::isNodeBoardLinkChipLocation(const string& location) const
{
	//Location loc(location);
	//return (loc.getType() == Location::LinkModuleOnNodeBoard);
	bool answer = false;
	if (location.length() == 14) {
		// Ensure that the location is Rxx-Mx-Nxx-U0x.
		if (location[0] == 'R'
			&& ((location[1] >= '0' && location[1] <= '9') || (location[1] >= 'A' && location[1] <= 'V'))
			&& ((location[2] >= '0' && location[2] <= '9') || (location[2] >= 'A' && location[2] <= 'V'))
			&& location[3] == '-'
			&& location[4] == 'M'
			&& (location[5] == '0' || location[5] == '1')
			&& location[6] == '-'
			&& location[7] == 'N'
			&& ((location[8] == '0' && location[9] >= '0' && location[9] <= '9')
			   || (location[8] == '1' && location[9] >= '0' && location[9] <= '5'))
			&& location[10] == '-'
			&& location[11] == 'U'
			&& location[12] == '0'
			&& location[13] >= '0' && location[13] <= '8')
		{
			answer = true;
		}
	}
	return answer;

} // End isNodeBoardLinkChipLocation(...)


bool CableBadWires::isIoDrawerLinkChipLocation(const string& location) const
{
	//Location loc(location);
	//return (lloc.getType() == Location::LinkModuleOnIoBoard);
	bool answer = false;
	if (location.length() == 10) {
		// Ensure that the location is Rxx-Ix-U0x or Qxx-Ix-U0x.
		if ((location[0] == 'R' || location[0] == 'Q')
			&& ((location[1] >= '0' && location[1] <= '9') || (location[1] >= 'A' && location[1] <= 'V'))
			&& ((location[2] >= '0' && location[2] <= '9') || (location[2] >= 'A' && location[2] <= 'V'))
			&& location[3] == '-'
			&& location[4] == 'I'
			&& ((location[5] >= '0' && location[5] <= '9') || (location[5] >= 'A' || location[5] <= 'F'))
			&& location[6] == '-'
			&& location[7] == 'U'
			&& location[8] == '0'
			&& location[9] >= '0' && location[9] <= '5')
		{
			answer = true;
		}
	}
	return answer;

} // End isIoDrawerLinkChipLocation(...)


bool CableBadWires::isNodeBoardPortLocation(const string& location) const
{
	bool answer = false;
	if (location.length() == 14) {
		// Ensure that the location is Rxx-Mx-Nxx-T00 - T11.
		if (location[0] == 'R'
			&& ((location[1] >= '0' && location[1] <= '9') || (location[1] >= 'A' && location[1] <= 'V'))
			&& ((location[2] >= '0' && location[2] <= '9') || (location[2] >= 'A' && location[2] <= 'V'))
			&& location[3] == '-'
			&& location[4] == 'M'
			&& (location[5] == '0' || location[5] == '1')
			&& location[6] == '-'
			&& location[7] == 'N'
			&& ((location[8] == '0' && location[9] >= '0' && location[9] <= '9')
			   || (location[8] == '1' && location[9] >= '0' && location[9] <= '5'))
			&& location[10] == '-'
			&& location[11] == 'T'
			&& (location[12] == '0' || location[12] == '1')
			&& location[13] >= '0' && location[13] <= '9')
		{
			answer = true;
		}
	}
	return answer;

} // End isNodeBoardPortLocation(...)


bool CableBadWires::isIoDrawerPortLocation(const string& location) const
{
	bool answer = false;
	if (location.length() == 10) {
		// Ensure that the location is Rxx-Ix-T00 - T24 or Qxx-Ix-T00 - T23.
		if ((location[0] == 'R' || location[0] == 'Q')
			&& ((location[1] >= '0' && location[1] <= '9') || (location[1] >= 'A' && location[1] <= 'V'))
			&& ((location[2] >= '0' && location[2] <= '9') || (location[2] >= 'A' && location[2] <= 'V'))
			&& location[3] == '-'
			&& location[4] == 'I'
			&& ((location[5] >= '0' && location[5] <= '9') || (location[5] >= 'A' && location[5] <= 'F'))
			&& location[6] == '-'
			&& location[7] == 'T'
			&& location[8] >= '0' && location[8] <= '2'
			&& location[9] >= '0' && location[9] <= '9')
		{
			answer = true;
		}
	}
	return answer;

} // End isIoDrawerPortLocation(...)


int CableBadWires::charToInt(char number) const
{
	int answer = 0;
	switch(number) {
		case '0': answer = 0; break;
		case '1': answer = 1; break;
		case '2': answer = 2; break;
		case '3': answer = 3; break;
		case '4': answer = 4; break;
		case '5': answer = 5; break;
		case '6': answer = 6; break;
		case '7': answer = 7; break;
		case '8': answer = 8; break;
		case '9': answer = 9; break;
	}
	return answer;

} // Ende charToInt(...)

} // utility
} // bgq
