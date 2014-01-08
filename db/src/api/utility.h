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

#ifndef BGQDB_UTILITY_H
#define BGQDB_UTILITY_H


#include "BGQDBlib.h"

#include <map>
#include <stdexcept>
#include <string>

#include <stdint.h>

namespace BGQDB {

inline unsigned int
countOnes(
        const std::string& str
        )
{
    unsigned int ones = 0;
    for (unsigned loop=0; loop < str.size(); ++loop) {
        if (str[loop] == '1') {
            ++ones;
        }
    }

    return ones;
}

inline void
insert_colons(
        std::string& str        //!< [in,out]
        )
{
    // ensure string length is an even number
    if (str.size() == 0 || str.size() % 2 != 0) {
        throw std::invalid_argument("invalid license plate");
    }

    // idea is to insert a colon after every two characters
    // so "abcd" becomes "ab:cd"
    std::string::iterator i = str.begin();
    while (i != str.end()) {
        i += 2;
        if (i != str.end()) {
            i = str.insert(i, ':');
            ++i;
        }
    }
}


typedef std::map<std::string,NodeBoardPositions> NodeBoardStartPositionToPositions;


struct SmallBlockSizeInfo
{
    NodeBoardStartPositionToPositions start_position_to_positions; //!< Valid node board positions given the starting node board position.
    DimensionConnectivity connectivity; //!< Connectivity in each dimension.
    DimensionSizes sizes; //!< Size in nodes of each dimension.
};


/*! \brief Gets the info for the small block given the size (in node boards).
 *
 *  \throws std::invalid_arg if the size is not valid.
 */
const SmallBlockSizeInfo& getSmallBlockInfoForSize( unsigned size_in_node_boards );


std::string dimensionConnectivityToDbTorusString( const DimensionConnectivity dim_conns );

DimensionConnectivity dbTorusStringToDimensionConnectivity( const std::string& s );


} // BGQDB


#endif
