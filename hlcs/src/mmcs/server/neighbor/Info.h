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

#ifndef MMCS_SERVER_NEIGHBOR_INFO_H_
#define MMCS_SERVER_NEIGHBOR_INFO_H_


#include "../types.h"

#include <bgq_util/include/Location.h>

#include <boost/shared_ptr.hpp>

#include <memory>
#include <set>
#include <sstream>

namespace mmcs {
namespace server {
namespace neighbor {

class Direction;

/*!
 * \brief Base class for add neighbor details to a RAS event
 *
 * This class provides an interface and generic functions to
 * calculate a torus neighbor.
 */
class Info
{
public:
    /*!
     * \brief Factory method
     */
    static std::auto_ptr<Info> create(
            const BlockPtr& block,                  //!< [in] block handling the RAS event
            int msgid,                              //!< [in] message ID from the RAS event
            const bgq::util::Location& location,    //!< [in] location originating the event
            const char* rawdata                     //!< [in] raw data from the RAS event
            );

    /*!
     * \brief dtor.
     */
    virtual ~Info();

    /*!
     * \brief
     */
    std::string getDetails() const { return _details.str(); }

    /*!
     * \brief
     */
    virtual void impl(
            const char* rawdata //!< [in]
            ) = 0;

protected:
    Info(
            const BlockPtr& block,
            int msgid,
            const bgq::util::Location& location
        );

    BCNodeInfo* getNeighbor( 
            BCNodeInfo* const node,
            const Direction& direction
            ) const;

    bool isWrapped(
        const Direction& direction,
        BCNodeInfo* const node,
        BCNodeInfo* const neighbor
        ) const;

protected:
    std::ostringstream _details;
    const int _msgId;
    const bgq::util::Location _location;
    const BlockPtr _block;
};

} } } // namespace mmcs::server::neighbor

#endif
