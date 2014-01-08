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

#include "RackBitset.h"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

namespace mmcs {
namespace server {

void
parse_location(
        const std::string& location,
        unsigned& midplane,
        unsigned& board,
        unsigned& node
        )
{
    // Location format of R00-M0-N00-J00 or Q00-I0-J00 or R00-I0-J00
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    const boost::char_separator<char> sep("-");
    const tokenizer tok(location, sep);
    tokenizer::iterator curr_tok = tok.begin();

    std::string curr_string;
    if (location.length() > 10) {
        curr_string = *(++curr_tok);
        midplane = boost::lexical_cast<unsigned>(curr_string.erase(0,1));
    } else {
        midplane = 0;  // I/O always gets a zero midplane.
    }
    curr_string = *(++curr_tok);
    std::string boardstr = curr_string.erase(0,1);
    if (boardstr == "A")
        boardstr = "10";
    if (boardstr == "B")
        boardstr = "11";
    if (boardstr == "C")
        boardstr = "12";
    if (boardstr == "D")
        boardstr = "13";
    if (boardstr == "E")
        boardstr = "14";
    if (boardstr == "F")
        boardstr = "15";

    board = boost::lexical_cast<unsigned>(boardstr);
    curr_string = *(++curr_tok);
    node = boost::lexical_cast<unsigned>(curr_string.erase(0,1));
}

int
RackBitset::encode(
        const std::string& location
        ) const
{
    unsigned midplane, board, node;
    midplane = board = node = 0;
    parse_location(location, midplane, board, node);
    int node_offset = (NODES_PER_BOARD * board) + (midplane * NODES_PER_MIDPLANE);
    return node + node_offset;
}

void
RackBitset::decode(
        unsigned bit,
        unsigned& midplane,
        unsigned& board,
        unsigned& node
        ) const
{
    node = bit % NODES_PER_BOARD;
    if (bit >= NODES_PER_MIDPLANE) {
        midplane = 1;
        board = (bit - NODES_PER_MIDPLANE) / NODES_PER_BOARD;
    } else {
        midplane = 0;
        board = bit / NODES_PER_BOARD;
    }
}

void
RackBitset::FindUnset(
        std::vector<std::string>& locations
        ) const
{
    for (unsigned int i = 0; i < NODES_PER_RACK; ++i) {
        if(_nodes.test(i) == false) {
            locations.push_back(find_location(i));
        }
    }
}

std::string
RackBitset::find_location(
        unsigned bit
        ) const
{
    std::string location;
    unsigned midplane, board, node;
    midplane = board = node = 0;
    decode(bit, midplane, board, node);
    std::ostringstream loc;

    std::string boardstr;
    boardstr = "N";
    if(board < 10)
        boardstr += "0";
    boardstr += boost::lexical_cast<std::string>(board);

    std::string nodestr = "J";
    if (node < 10)
        nodestr += "0";
    nodestr += boost::lexical_cast<std::string>(node);

    loc << _rack_loc << "-M" << boost::lexical_cast<std::string>(midplane)
        << "-" << boardstr << "-" << nodestr;
    location = loc.str();
    return location;
}

RackBitsetBase::~RackBitsetBase()
{

}

void
RackBitsetBase::Set(
        const std::string& location
        )
{
    if (!_IO) {  // Compute
        if (location.length() == 3 && location.find("R") != std::string::npos) {
            // If it's a rack, set all of the rack's bits.
            _nodes.set();
        } else if (location.length() == 6 && location.find("R") != std::string::npos) {
            // Compute midplane, set the 512 bits from here.
            const std::string temp = location + "-N00-J00";
            unsigned base_index = encode(temp);
            for (unsigned i = 0; i < NODES_PER_MIDPLANE; ++i)
                _nodes.set(base_index + i, true);
        } else if (location.length() == 10 && location.find("N") != std::string::npos) {
            // Node board
            const std::string temp = location + "-J00";
            unsigned base_index = encode(temp);
            for (unsigned i = 0; i < NODES_PER_BOARD; ++i)
                _nodes.set(base_index + i, true);
        } else {
            unsigned index = encode(location);
            _nodes.set(index, true);
        }
    } else { // I/O
        if (location.length() == 3 &&
           (location.find("R") != std::string::npos || location.find("Q") != std::string::npos)) {
            // I/O rack or drawer
            _nodes.set();
        } else if(location.length() == 6 && location.find("I") != std::string::npos) {
            const std::string temp = location + "-J00";
            unsigned base_index = encode(temp);
            for (unsigned i = 0; i < IO_NODES_PER_BOARD; ++i)
                _nodes.set(base_index + i, true);
        } else {
            unsigned index = encode(location);
            _nodes.set(index, true);
        }
    }
}

void
RackBitsetBase::Reset(
        const std::string& location
        )
{
    unsigned index = encode(location);
    _nodes.set(index, false);
}

bool
RackBitsetBase::Flagged(
        const std::string& location
        ) const
{
    unsigned index = encode(location);
    return _nodes.test(index);
}

void
IOBoardBitset::FindUnset(
        std::vector<std::string>& locations
        ) const
{
    for(unsigned int i = 0; i < IO_NODES_PER_RACK; ++i) {
        if (_nodes.test(i) == false) {
            locations.push_back(find_location(i));
        }
    }
}

std::string
IOBoardBitset::find_location(
        unsigned bit
        ) const
{
    unsigned midplane, board, node;
    midplane = board = node = 0;
    decode(bit, midplane, board, node);
    std::ostringstream loc;
    std::string nodestr = "J";
    if (node < 10)
        nodestr += "0";
    loc << _rack_loc << "-I" << std::hex << board << "-" << nodestr << node;
    return loc.str();
}

int
IOBoardBitset::encode(
        const std::string& location
        ) const
{
    unsigned midplane, board, node;
    midplane = board = node = 0;
    parse_location(location, midplane, board, node);
    int node_offset = (IO_NODES_PER_BOARD * board);
    return node + node_offset;
}

void
IOBoardBitset::decode(
        unsigned bit,
        unsigned& midplane,
        unsigned& board,
        unsigned& node
        ) const
{
    node = bit % IO_NODES_PER_BOARD;
    midplane = 0;
    board = bit / IO_NODES_PER_BOARD;
}

} } // namespace mmcs::server
