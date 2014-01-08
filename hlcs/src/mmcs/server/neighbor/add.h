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

#ifndef MMCS_SERVER_NEIGHBOR_CREATE_H_
#define MMCS_SERVER_NEIGHBOR_CREATE_H_

#include "../BlockControllerBase.h"
#include "../types.h"

#include <bgq_util/include/Location.h>

#include <db/include/api/tableapi/gensrc/DBTEventlog.h>

#include <ras/include/RasEvent.h>

namespace mmcs {
namespace server {
namespace neighbor {

/*!
 * \brief add neighbor details for this ras event if needed.
 *
 * Certain RAS events require additional details about torus topology. This function
 * will calculate the neighbor information for certain message IDs and append that
 * information to the rawdata of the event. It supports compute node and I/O node
 * locations, as well as link chips.
 *
 * \section compute Compute Nodes
 *
 * For RAS events from compute node locations, this class calculates the neighboring
 * compute node in the direction (see Dimension and Orientation)  specified of the
 * event, as well as how this neighbor is connected:
 *
 * - within the node board (this is always the E dimension)
 * - within the midplane using wrapped link chips
 * - to another midplane using link chips and optical cables.
 *
 * Events from compute nodes can also specify DIM_DIR=IO to include neighbor
 * information of the I/O link and neighboring I/O nodes.
 *
 * \section io I/O nodes
 *
 * For RAS events from I/O node locations, this class calculates the neighboring
 * I/O node (assuming an I/O torus) for the A, B, and C dimensions. For the D and IO
 * dimensions, it calculates the neighboring compute nodes and cables in use.
 *
 * \section link Link chips
 *
 * For RAS events from link chip locations, this class calculates the neighboring
 * link chip and cable. These events do not need a DIM_DIR specified in the raw data,
 * they require Mask and Register to correctly associate the optical port used.
 *
 * \see ComputeNode
 * \see IoNode
 * \see IoLink
 * \see LinkChip
 */
void addDetails(
        const BlockPtr& block,      //!< [in]
        const RasEvent& event,      //!< [in]
        BGQDB::DBTEventlog& dbe     //!< [in] neighbor details will be added to the raw data
        ) throw();

/*!
 * \brief Associate a block with an event from an I/O node that arrives on the default event listener.
 *
 * This is needed because I/O block target sets are typically closed once their boot completes. The
 * block topology is not know by the default event listener.
 *
 * \see DefaultListener
 */
BlockPtr findIoBlock(
        const bgq::util::Location& location //!< [in]
        );

} } } // namespace mmcs::server::neighbor

#endif
