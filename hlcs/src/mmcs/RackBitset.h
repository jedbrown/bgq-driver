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

#include <bitset>

#ifndef RACKBITSET_H
#define RACKBITSET_H

// Every node in a rack (or io drawer) is represented by a bit in a bit vector
// implemented as a standard bitset.  The encode/decode methods implement
// the algorithms for mapping locations to specific bits.

// Compute constants
const unsigned int NODES_PER_RACK = 1024;
const unsigned int NODES_PER_BOARD = 32;
const unsigned int NODES_PER_MIDPLANE = 512;

// IO constants
// Note: IO boards may be drawers in a compute rack
// Note: Likely only to have 12 boards/rack, but use 16 anyway.
const unsigned int IO_NODES_PER_RACK = 128;
const unsigned int IO_NODES_PER_BOARD = 8;
const unsigned int IO_BOARDS_PER_RACK = 16;

class RackBitsetBase {
public:
protected:
    std::bitset<NODES_PER_RACK> _nodes; // 1024 nodes in a rack
    std::string _rack_loc;
    bool _IO;
    virtual std::string find_location(unsigned bit) = 0;
    virtual int encode(std::string& location) = 0;
    virtual void decode(unsigned bit, unsigned& midplane, unsigned& board, unsigned& node) = 0;
public:
    virtual ~RackBitsetBase() {}
    //! \brief set the rack location for the bitset
    //! \param rack The location of the rack (e.g. R00 or Q00)
    void SetRackLoc(std::string& rack) { _rack_loc = rack; }
    //! \brief get the rack location string
    //! \returns rack location string
    std::string GetRackLoc() { return _rack_loc; }
    //! \brief set the flag for the specified location
    //! \param location Node location string (e.g. R00-M0-N00-J00 or Q00-I0-N00-J00) to set
    void Set(std::string& location);
    //! \brief reset the flag (zero) for the specified location
    //! \param location Node location string (e.g. R00-M0-N00-J00 or Q00-I0-N00-J00) to reset
    void Reset(std::string& location);
    //! \brief clear all bits in bitset
    void Reset() { _nodes.reset(); }
    //! \brief return whether the flag is set for the specified location
    //! \param location Node location string (e.g. R00-M0-N00-J00 or Q00-I0-N00-J00) to check
    bool Flagged(std::string& location);
    //! \brief The number of bits that are set
    //! \returns the total number of set bits
    size_t BitsSet() { return _nodes.count(); }
};

//! \brief flag for each node in a rack
class RackBitset : public RackBitsetBase {
    std::string find_location(unsigned bit);
    int encode(std::string& location);
    void decode(unsigned bit, unsigned& midplane, unsigned& board, unsigned& node);
public:
    RackBitset() { }
    //! \brief ctor
    //! \param rack location string (e.g. R00 or Q00)
    RackBitset(std::string& rack) {
        _rack_loc = rack;
        _IO = false;
    }
    //! \brief Find all locations in the bitset that aren't set
    //! \param locations An empty vector to fill
    void FindUnset(std::vector<std::string>& locations);
};

class IOBoardBitset : public RackBitsetBase {
    std::string find_location(unsigned bit);
    int encode(std::string& location);
    void decode(unsigned bit, unsigned& midplane, unsigned& board, unsigned& node);
public:
    IOBoardBitset() { }
    IOBoardBitset(std::string& rack) {
        _rack_loc = rack;
        _IO = true;
    }
    //! \brief Find all locations in the bitset that aren't set
    //! \param locations An empty vector to fill
    void FindUnset(std::vector<std::string>& locations);
};

#endif
