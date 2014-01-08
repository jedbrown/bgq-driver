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

#ifndef MMCS_SERVER_NEIGHBOR_COMPUTE_NODE_H
#define MMCS_SERVER_NEIGHBOR_COMPUTE_NODE_H

#include "Info.h"

namespace mmcs {
namespace server {
namespace neighbor {

class Direction;

/*!
 * \brief compute node specific neighbor information.
 */
class ComputeNode : public Info
{
public:
    /*!
     * \brief ctor
     */
    ComputeNode(
            const BlockPtr& block,                  //!< [in]
            int msgid,                              //!< [in]
            const bgq::util::Location& location     //!< [in]
            );

    /*!
     * \copydoc Info::impl
     */
    void impl(
            const char* rawdata //!< [in]
            );

private:
    void addPassThrough(
            const Direction& direction,
            const std::string& me,
            const std::string& neighbor
            );
};

} } } // namespace mmcs::server::neighbor

#endif
