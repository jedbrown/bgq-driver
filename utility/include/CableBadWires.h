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

#ifndef BGQ_UTILITY_CABLE_BAD_WIRES
#define BGQ_UTILITY_CABLE_BAD_WIRES

#include <string>

#include <stdint.h>

namespace bgq {
namespace utility {

/**
 * This class is used to convert the bad fiber information detected by the link chip on the
 * node board or IO drawer to the bad wire mask to be stored in the BGQCable table based on
 * the bad fiber to bad wire mask mapping specification defined in BG/Q Issue 2417.
 *
 * References: BG/Q Issue 2417 and 3125
 */
class CableBadWires
{

public:
	/**
	 * Constructs a CableBadWires object that converts the bad fiber information on a
	 * receiving link chip to the bad wire mask for the BGQCable table.
	 *
	 * @param location		Receiving node board or IO drawer link chip location
	 * 						("Rxx-Mx-Nxx-Uxx", "Rxx-Ix-Uxx", or "Qxx-Ix-Uxx")
	 * @param reg			Receiving link chip register ("C01" or "C23")
	 * @param badFiberMask	Bad fiber mask (12 bits long: [0:11] where 0 is good and 1 is bad)
	 * @throws invalid_argument if the specified location is not a valid link chip location or
	 *         the specified register or badFiberMask is not valid.
	 */
	CableBadWires(const std::string& location, const std::string& reg, int badFiberMask);

	/**
	 * Get the receiving node board or IO drawer port location containing bad wires.
	 *
	 * @returns Receiving node board or IO drawer port location with bad wires
	 *         (i.e., "Rxx-Mx-Nxx-Txx", "Rxx-Ix-Txx", or "Qxx-Ix-Txx").
	 */
	std::string getRxPortLocation() const;
	std::string getPortLocation() const;	// WILL BE DELETED

	/**
	 * Get the bad wire mask describing the position of the bad fibers in a cable according to
	 * the bad fiber to bad wire mask mapping specification defined in BG/Q Issue 2417.
	 *
	 * @returns Bad wire mask (64 bits of which the least significant or the right most 48 bits
	 * are used for a Torus cable and 12 bits for an IO cable).
	 */
	long int getBadWireMask() const;

	/**
	 * Set the transmitting port location and aggregated bad wire mask for the cable in order to
	 * determine the transmitting link chip location/register and the aggregated bad fiber mask.
	 *
	 * @param location		Transmitting port location on the node board or IO drawer
	 *                 		(i.e., "Rxx-Mx-Nxx-Txx", "Rxx-Ix-Txx", or "Qxx-Ix-Txx").
	 * @param badWireMask 	Aggregated bad wire mask for the cable from the database
	 */
	void setTxPortAndBadWireMask(const std::string& location, uint64_t badWireMask);

	/**
	 * Get the transmitting link chip location corresponding to the receiving link chip
	 * with a bad fiber specified in the ctor.
	 * The client must call the setTxPortAndBadWireMask method before calling this method.
	 *
	 * @returns Transmitting link chip location.
	 */
	std::string getTxLinkChipLocation() const;

	/**
	 * Get the transmitting link chip register corresponding to the receiving link chip
	 * with a bad fiber specified in the ctor.
	 * The client must call the setTxPortAndBadWireMask method before calling this method.
	 *
	 * @returns Transmitting link chip register ("D01" or "D23").
	 */
	std::string getTxRegister() const;

	/**
	 * Get the aggregated bad fiber mask for the link chip with a bad fiber specified in the ctor.
	 * The client must call the setTxPortAndBadWireMask method before calling this method.
	 *
	 * @returns Bad fiber mask aggregated with the previously reported bad fiber masks
	 * for the receiving link chip location and register specified in the ctor.
	 * The least significant (i.e., the right most) 12 bits contain the value.
	 */
	long int getAggregatedBadFiberMask() const;

	/**
	 * Get the text describing the error detected.  A null string (0-length string) is returned
	 * if no error conditions are detected.
	 */
	std::string getErrorMessage() const;

private:
	bool isNodeBoardLinkChipLocation(const std::string& location) const;
	bool isIoDrawerLinkChipLocation(const std::string& location) const;
	bool isNodeBoardPortLocation(const std::string& location) const;
	bool isIoDrawerPortLocation(const std::string& location) const;
	void processNodeBoardTorusCable(const std::string& location, int linkChipIndex);
	void processNodeBoardIoCable(const std::string& location, int linkChipIndex);
	void processIoDrawerIoCable(const std::string& location, int linkChipIndex);
	int getFibersFromWiresForTorusCable(int linkChipIndex, uint64_t badWireMask) const;
	int getFibersFromWiresForIoCable(int linkChipIndex, uint64_t badWireMask) const;
	int charToInt(char number) const;

	// INPUT TO CTOR:
	std::string			_linkchip;		// Input: Receiver link chip location (Rxx-Mx-Nxx-U0x, Rxx/Qxx-Ix-U0x)
	int					_register;		// Input: Receiver link chip register
	int					_badfibers;		// Input: Receiver bad filer bits (12 bits[0:11] where 0=ok, 1=bad)

	// INPUT TO setTxPortAndAggregatedBadWireMask():
	std::string			_fromport;		// Input: From port location (Rxx-Mx-Nxx-Txx, Rxx/Qxx-Ix-Txx)
	long int			_agbadwiremask;	// Input: Aggregated bad wire mask (12 bits for IO cable, 48 bits for Torus cable)

	// SET BY CTOR:
	bool 				_rxIsNodeBoard;
	bool 				_rxIsIoDrawer;
	bool				_isTorusCable;
	int					_rxLinkChipPos;
	std::string			_port;			// Output: Receiver node board or IO drawer port location (Rxx-Mx-Nxx-Txx, Qxx-Ix-Uxx)
	uint64_t			_badwiremask;	// Output: Receiver bad wire mask (12 bits for IO cable, 48 bits for Torus cable)

	// SET BY setTxPortAndAggregatedBadWireMask():
	bool 				_txIsNodeBoard;
	bool 				_txIsIoDrawer;
	int					_txPortPos;
	std::string			_txlinkchip;	// Output: Transmitter link chip location (Rxx-Mx-Nxx-U0x, Rxx/Qxx-Ix-U0x)
	std::string			_txregister;	// Output: Transmitter link chip register ("D01" or "D23")
	int					_txbadfibers;	// Output: Transmitter bad filer bits (12 bits[0:11] where 0=ok, 1=bad)

	// GENERAL OUTPUT:
	std::string			_errormessage;	// Output: Text describing the error condition detected
};

} // utility
} // bgq

#endif
