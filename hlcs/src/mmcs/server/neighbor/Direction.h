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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_SERVER_NEIGHBOR_DIRECTION_H_
#define MMCS_SERVER_NEIGHBOR_DIRECTION_H_

#include "Dimension.h"
#include "Orientation.h"

#include <string>
#include <sstream>

namespace mmcs {
namespace server {
namespace neighbor {

/*!
 * \brief
 */
class Direction 
{
public:
    /*!
     * \brief Parse the DIM_DIR keyword from the rawdata
     *
     * \throws std::invalid_argument if DIM_DIR is missing
     */
    Direction(
            const char* rawdata //!< [in]
            );

    Dimension::Value dim() const { return _dimension; }
    Orientation::Value orientation() const { return _orientation; }
    const std::string& buf() const { return _buf; }

private:
    std::string _buf;
    Dimension::Value _dimension;
    Orientation::Value _orientation;
};

/*!
 * \brief insertion operator for a Direction;
 */
std::ostream& operator<<(
        std::ostream&,      //!< [in]
        const Direction&    //!< [in]
        );
/*!
 * \brief insertion operator for a Dimension;
 */
std::ostream& operator<<(
        std::ostream&,              //!< [in]
        const Dimension::Value&     //!< [in]
        );


} } } // namespace mmcs::server::neighbor

#endif
