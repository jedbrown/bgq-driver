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
/*!
 * \file utility/include/CableBadWires.h
 */

#ifndef BGQ_UTILITY_CABLE_BAD_WIRES
#define BGQ_UTILITY_CABLE_BAD_WIRES

#include <vector>
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
     * @param location        Receiving node board or IO drawer link chip location
     *                         ("Rxx-Mx-Nxx-Uxx", "Rxx-Ix-Uxx", or "Qxx-Ix-Uxx")
     * @param reg            Receiving link chip register ("C01" or "C23")
     * @param badFiberMask    Bad fiber mask (12 bits long: [0:11] where 0 is good and 1 is bad)
     * @throws invalid_argument if the specified location is not a valid link chip location or
     *         the specified register or badFiberMask is not valid.
     */
    CableBadWires(const std::string& location, const std::string& reg, int badFiberMask);

    /**
     * Indicates that the cable status must be set to ERROR since there are more bad wires than
     * the sparing mechanism can handle.
     *
     * @returns true if the cable status must be set to ERROR.
     */
    bool cableInError() const;

    /**
     * Get the receiving node board or IO drawer port location containing bad wires.
     *
     * @returns Receiving node board or IO drawer port location with bad wires
     *         (i.e., "Rxx-Mx-Nxx-Txx", "Rxx-Ix-Txx", or "Qxx-Ix-Txx").
     */
    std::string getRxPortLocation() const;
    std::string getPortLocation() const;    // WILL BE DELETED

    /**
     * Get the bad wire mask describing the position of the bad fibers in a cable according to
     * the bad fiber to bad wire mask mapping specification defined in BG/Q Issue 2417.
     *
     * @returns Bad wire mask (64 bits of which the least significant or the right most 48 bits
     * are used for a Torus cable and 12 bits for an IO cable).
     */
    uint64_t getBadWireMask() const;

    /**
     * Set the transmitting port location and aggregated bad wire mask for the cable in order to
     * determine the transmitting link chip location/register and the aggregated bad fiber mask.
     *
     * @param location        Transmitting port location on the node board or IO drawer
     *                         (i.e., "Rxx-Mx-Nxx-Txx", "Rxx-Ix-Txx", or "Qxx-Ix-Txx").
     * @param badWireMask     Aggregated bad wire mask for the cable from the database.
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
     * Get the 12-bit bad fiber mask in the transmitting register describing the position of
     * of the bad fibers according to the bad fiber to bad wire mask mapping specification
     * defined in BG/Q Issue 2417.
     *
     * @returns Bad fiber mask (the least significant or the right most 12 bits are used).
     */
    int getTxRegContents() const;

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
     * Get the bad fiber mask for the RX register (C01 or C23) on the receiving link chip.
     *
     * @returns Aggregated bad fiber mask for the RX register.
     * The least significant (i.e., the right most) 12 bits contain the value.
     */
    int getRxFiberMask() const;

    /**
     * Get the bad fiber mask for the TX register (D01 or D23) on the transmitting link chip.
     *
     * @returns Aggregated bad fiber mask for the TX register.
     * The least significant (i.e., the right most) 12 bits contain the value.
     */
    int getTxFiberMask() const;

    /**
     * Get the text describing the error detected.  A null string (0-length string) is returned
     * if no error conditions are detected.
     */
    std::string getErrorMessage() const;

    /**
     * Translate the bad wire mask between two Torus/IO ports into pairs of optical module locations.
     * For example, if the bad wire mask is 0x1001 between R00-M0-N00-T08 and R01-M0-N00-T10,
     * a list containing "R01-M0-N00-O20:R00-M0-N00-O21" and "R01-M0-N00-O32:R00-M0-N00-O33" is
     * returned to indicate that there are bad wires between (1) R01-M0-N00-O20 and R00-M0-N00-O21
     * and (2) R01-M0-N00-O32 and R00-M0-N00-O33.
     *
     * @param fromPort        Port location on a node board or IO drawer on one end of a Torus/IO cable
     *                         ("Rxx-Mx-Nxx-Txx", "Rxx-Ix-Txx", or "Qxx-Ix-Txx").
     * @param toPort        Port location on a node board or IO drawer on the other end of the cable.
     * @param badWireMask    Bad fiber mask (64 bits long: [63:0] where 0 is good and 1 is bad)
     * @returns A list of colon-separated optical module location pairs with bad wires.
     * @throws invalid_argument if an invalid Torus/IO port location was specified or if the
     * connection between two specified ports is not supported.
     */
    static std::vector<std::string> getBadOpticalConnections(const std::string& fromPort, const std::string& toPort, long int badWireMask);

private:
    bool isNodeBoardLinkChipLocation(const std::string& location) const;
    bool isIoDrawerLinkChipLocation(const std::string& location) const;
    bool processNodeBoardTorusCable(const std::string& location, int linkChipIndex);
    bool processNodeBoardIoCable(const std::string& location, int linkChipIndex);
    bool processIoDrawerIoCable(const std::string& location, int linkChipIndex);
    int getFibersFromWiresForTorusCable(int linkChipIndex, uint64_t badWireMask) const;
    int getFibersFromWiresForIoCable(int linkChipIndex, uint64_t badWireMask) const;
    bool checkTorusCable(uint64_t badWireMask) const;
    bool checkIoCable(uint64_t badWireMask) const;
    int onBits(uint64_t mask) const;

    static bool isNodeBoardPortLocation(const std::string& portLocation);
    static bool isNodeBoardIoPort(const std::string& portLocation);
    static bool isIoDrawerPortLocation(const std::string& portLocation);
    static std::string getBoardLocation(const std::string& portLocation);
    static int getPortPosition(const std::string& portLocation);
    static bool isTorusSender(const std::string& portLocation);
    static int charToInt(char number);

    // INPUT TO CTOR:
    std::string            _linkchip;        // Input: Receiver link chip location (Rxx-Mx-Nxx-U0x, Rxx/Qxx-Ix-U0x)
    int                    _register;        // Input: Receiver link chip register
    unsigned int           _badfibers;        // Input: Receiver bad fiber bits (12 bits[0:11] where 0=ok, 1=bad)

    // INPUT TO setTxPortAndAggregatedBadWireMask():
    std::string            _fromport;        // Input: From port location (Rxx-Mx-Nxx-Txx, Rxx/Qxx-Ix-Txx)
    long int            _agbadwiremask;    // Input: Aggregated bad wire mask (12 bits for IO cable, 48 bits for Torus cable)

    // SET BY CTOR:
    bool                 _rxIsNodeBoard;
    bool                 _rxIsIoDrawer;
    bool                _isTorusCable;
    int                    _rxLinkChipPos;
    std::string            _port;            // Output: Receiver node board or IO drawer port location (Rxx-Mx-Nxx-Txx, Qxx-Ix-Uxx)
    int                    _portPos;
    uint64_t            _badwiremask;    // Output: Receiver bad wire mask (12 bits for IO cable, 48 bits for Torus cable)
    bool                _cableInError;    // Output: Cable status must be set to ERROR

    // SET BY setTxPortAndAggregatedBadWireMask():
    bool                 _txIsNodeBoard;
    bool                 _txIsIoDrawer;
    int                    _txPortPos;
    std::string            _txlinkchip;    // Output: Transmitter link chip location (Rxx-Mx-Nxx-U0x, Rxx/Qxx-Ix-U0x)
    std::string            _txregister;    // Output: Transmitter link chip register ("D01" or "D23")
    int                    _badfibermask;    // Output: Bad fiber bits (12 bits)
    int                    _rxRegValue;    // Output: RX register value on the receiver (12 bits)
    int                    _txRegValue;    // Output: TX register value on the transmitter (12 bits)
    // *******************************************************************************************
    // NOTE: A CableBadWires object is constructed by the BGQDBlib::postProcessRAS method to
    //       process a BQL_SPARE RAS event for a specific link chip receiver register (C01 or C23)
    //       on a node board or IO drawer.
    // *******************************************************************************************


    // GENERAL OUTPUT:
    std::string            _errormessage;    // Output: Text describing the error condition detected
};

} // utility
} // bgq

#endif
