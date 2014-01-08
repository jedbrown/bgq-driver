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

#ifndef MMCS_SERVER_NEIGHBOR_IO_NODE_H
#define MMCS_SERVER_NEIGHBOR_IO_NODE_H

#include "Info.h"

namespace mmcs {
namespace server {
namespace neighbor {

/*!
 * \brief I/O node specifc neighbor information
 */
class IoNode : public Info
{
public:
    /*!
     * \brief
     */
    IoNode(
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
    void extra(
            BCNodeInfo* const me,
            BCNodeInfo* const neighbor,
            const Direction& direction
            );
};

} } } // namespace mmcs::server::neighbor

#endif
