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

#ifndef MMCS_SERVER_RACK_BITSET_H
#define MMCS_SERVER_RACK_BITSET_H

#include <bitset>

#include <string>
#include <vector>

#include <stddef.h>

namespace mmcs {
namespace server {

// Every node in a rack (or I/O drawer) is represented by a bit in a bit vector
// implemented as a standard bitset. The encode/decode methods implement
// the algorithms for mapping locations to specific bits.

// Compute constants
const unsigned int NODES_PER_RACK = 1024;
const unsigned int NODES_PER_BOARD = 32;
const unsigned int NODES_PER_MIDPLANE = 512;

// I/O constants
// Note: I/O boards may be drawers in a compute rack
// Note: Likely only to have 12 boards/rack, but use 16 anyway.
const unsigned int IO_NODES_PER_RACK = 128;
const unsigned int IO_NODES_PER_BOARD = 8;
const unsigned int IO_BOARDS_PER_RACK = 16;

class RackBitsetBase
{
protected:
    std::bitset<NODES_PER_RACK> _nodes; // 1024 nodes in a rack
    std::string _rack_loc;
    bool _IO;
    virtual std::string find_location(unsigned bit) const = 0;
    virtual int encode(const std::string& location) const = 0;
    virtual void decode(unsigned bit, unsigned& midplane, unsigned& board, unsigned& node) const = 0;
public:
    virtual ~RackBitsetBase() = 0;
    //! \brief Set the rack location for the bitset
    //! \param rack The location of the rack (e.g. R00 or Q00)
    void SetRackLoc(const std::string& rack) { _rack_loc = rack; }
    //! \brief Get the rack location string
    //! \returns rack location string
    std::string GetRackLoc() { return _rack_loc; }
    //! \brief Set the flag for the specified location
    //! \param location Node location string (e.g. R00-M0-N00-J00 or Q00-I0-N00-J00) to set
    void Set(const std::string& location);
    //! \brief Reset the flag (zero) for the specified location
    //! \param location Node location string (e.g. R00-M0-N00-J00 or Q00-I0-N00-J00) to reset
    void Reset(const std::string& location);
    //! \brief Clear all bits in bitset
    void Reset() { _nodes.reset(); }
    //! \brief Return whether the flag is set for the specified location
    //! \param location Node location string (e.g. R00-M0-N00-J00 or Q00-I0-N00-J00) to check
    bool Flagged(const std::string& location) const;
    //! \brief The number of bits that are set
    //! \returns the total number of set bits
    size_t BitsSet() const { return _nodes.count(); }
};

//! \brief Flag for each node in a rack
class RackBitset : public RackBitsetBase
{
    std::string find_location(unsigned bit) const;
    int encode(const std::string& location) const;
    void decode(unsigned bit, unsigned& midplane, unsigned& board, unsigned& node) const;
public:
    RackBitset() { }
    //! \brief ctor
    //! \param rack location string (e.g. R00 or Q00)
    RackBitset(const std::string& rack) {
        _rack_loc = rack;
        _IO = false;
    }
    //! \brief Find all locations in the bitset that aren't set
    //! \param locations An empty vector to fill
    void FindUnset(std::vector<std::string>& locations) const;
};

class IOBoardBitset : public RackBitsetBase
{
    std::string find_location(unsigned bit) const;
    int encode(const std::string& location) const;
    void decode(unsigned bit, unsigned& midplane, unsigned& board, unsigned& node) const;
public:
    IOBoardBitset() { }
    IOBoardBitset(const std::string& rack) {
        _rack_loc = rack;
        _IO = true;
    }
    //! \brief Find all locations in the bitset that aren't set
    //! \param locations An empty vector to fill
    void FindUnset(std::vector<std::string>& locations) const;
};

} } // namespace mmcs::server

#endif
