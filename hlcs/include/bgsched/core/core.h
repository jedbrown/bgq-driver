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
 * \file bgsched/core/core.h
 * \brief Core method definitions.
 */

#ifndef BGSCHED_CORE_CORE_H_
#define BGSCHED_CORE_CORE_H_

#include <bgsched/core/BlockSort.h>
#include <bgsched/core/IOBlockSort.h>
#include <bgsched/core/JobSort.h>

#include <bgsched/Block.h>
#include <bgsched/BlockFilter.h>
#include <bgsched/Coordinates.h>
#include <bgsched/ComputeHardware.h>
#include <bgsched/IOBlock.h>
#include <bgsched/IOBlockFilter.h>
#include <bgsched/IOHardware.h>
#include <bgsched/IOLink.h>
#include <bgsched/Job.h>
#include <bgsched/JobFilter.h>
#include <bgsched/Midplane.h>
#include <bgsched/Node.h>
#include <bgsched/NodeBoard.h>

#include <string>

namespace bgsched {
namespace core {

/*!
 * \brief Get Blue Gene compute hardware information.
 *
 * Retrieves an instance of a Blue Gene compute hardware object containing information
 * for all the midplanes, node boards, switches and cables in the system.
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::UnexpectedError - if any type of error occurs retrieving compute hardware information
 *
 * \return Blue Gene compute hardware information.
 */
ComputeHardware::ConstPtr getComputeHardware();

/*!
 * \brief Get Blue Gene I/O hardware information.
 *
 * Retrieves an instance of a Blue Gene I/O hardware object containing information for all the
 * I/O drawers and I/O nodes in the system.
 *
 * \note This method was added in V1R2M0.
 * \ingroup V1R2
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::UnexpectedError - if any type of error occurs retrieving I/O hardware information
 *
 * \return Blue Gene I/O hardware information.
 */
IOHardware::ConstPtr getIOHardware();

/*!
 * \brief Get the machine size in midplanes for each dimension.
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \return Machine size in midplanes for each dimension.
 */
Coordinates getMachineSize();

/*!
 * \brief Get midplane coordinates for midplane location.
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::InvalidLocationString - if midplane location was not found
 *
 * \throws bgsched::DatabaseException with value:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \return Midplane coordinates.
 */
Coordinates getMidplaneCoordinates(
        const std::string& midplaneLocation    //!< [in] Midplane location to get coordinates for
);

/*!
 * \brief Get coordinates within the midplane for the node location.
 *
 * For example, a node at N00-J00 is always at 0,0,0,0,0.
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::InvalidLocationString - if the location is not a node location.
 *
 * \return Node coordinates.
 */
Coordinates getNodeMidplaneCoordinates(
        const std::string& nodeLocation        //!< [in] Node location to get coordinates for
);

/*!
 * \brief Get node boards for a Blue Gene midplane.
 *
 * Retrieves the node board objects associated with the midplane.
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::XMLParseError - if internal XML format is bad
 * - bgsched::InternalErrors::InconsistentDataError - if 16 node boards are not found for the midplane
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return Node boards for a Blue Gene midplane.
 */
NodeBoard::ConstPtrs getNodeBoards(
        const std::string& midplaneLocation    //!< [in] Midplane location to get node boards for
);

/*!
 * \brief Get nodes for a Blue Gene node board.
 *
 * Retrieves the node objects associated with the node board.
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::InvalidLocationString - if node board location was not found
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::InconsistentDataError - if 32 nodes are not found for the node board
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return Nodes for a Blue Gene node board.
 */
Node::ConstPtrs getNodes(
        const std::string& nodeBoardLocation   //!< [in] Node board location to get nodes for
);

/*!
 * \brief Get nodes for every node board on a Blue Gene midplane.
 *
 * Retrieves the node objects associated with the midplane.
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::InvalidLocationString - if midplane location was not found
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::InconsistentDataError - if 512 nodes are not found for the midplane
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return Nodes for every node board on a Blue Gene midplane.
 */
Node::ConstPtrs getMidplaneNodes(
        const std::string& midplaneLocation   //!< [in] Midplane location to get nodes for
);

/*!
 * \brief Get I/O links for a specific Blue Gene midplane or all midplanes.
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::InvalidLocationString - if midplane location was not found
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::DataNotFound - if no I/O links found for the midplane(s) in the database
 * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::XMLParseError - if internal XML format is bad
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return I/O links for a specific Blue Gene midplane or all midplanes.
 */
IOLink::ConstPtrs getIOLinks(
        const std::string& midplaneLocation    //!< [in] Midplane location to get I/O links for or empty string for all midplanes
);

/*!
 * \brief Get "available" I/O links for a specific Blue Gene midplane or all midplanes. An I/O link is considered
 * "available" if both the I/O link hardware state is available and the destination I/O node is also available.
 *
 * \note This method was added in V1R1M1.
 * \ingroup V1R1M1
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::InvalidLocationString - if midplane location was not found
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::DataNotFound - if no I/O links found for the midplane(s) in the database
 * - bgsched::DatabaseErrors::ConnectionError - if error occurs connecting to the database
 * - bgsched::DatabaseErrors::UnexpectedError - if receive unexpected return code from internal database function
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::XMLParseError - if internal XML format is bad
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return Available I/O links for a specific Blue Gene midplane or all midplanes. If an empty vector is returned this implies
 * that no I/O links are "available" for the midplane resources specified.
 */
IOLink::ConstPtrs getAvailableIOLinks(
        const std::string& midplaneLocation    //!< [in] Midplane location to get "available" I/O links for or empty string for all midplanes
);

/*!
 * \brief Get compute blocks using filter.
 *
 * Retrieves the compute blocks matching the block filter criteria.
 *
 * Note: An empty user string means no security filtering will be performed and all results will be returned.
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::OperationFailed - if database operation failed
 *
 * \throws bgsched::RuntimeException with values:
 * - bgsched::RuntimeErrors::AuthorityError - if user not found
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return Compute blocks matching the block filter criteria.
 */
Block::Ptrs getBlocks(
        const BlockFilter& filter,                   //!< [in] Compute filter to search on
        const BlockSort& sort = BlockSort::AnyOrder, //!< [in] Compute blocks will be returned in this order
        const std::string& user = std::string()      //!< [in] Check read authority to compute blocks for user
);

/*!
 * \brief Get I/O blocks using filter.
 *
 * Retrieves the I/O blocks matching the block filter criteria.
 *
 * Note: An empty user string means no security filtering will be performed and all results will be returned.
 *
 * \note This method was added in V1R2M0.
 * \ingroup V1R2
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::OperationFailed - if database operation failed
 *
 * \throws bgsched::RuntimeException with values:
 * - bgsched::RuntimeErrors::AuthorityError - if user not found
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::InconsistentDataError - if I/O node locations not found in database
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return I/O blocks matching the block filter criteria.
 */
IOBlock::Ptrs getIOBlocks(
        const IOBlockFilter& filter,                     //!< [in] I/O block filter to search on
        const IOBlockSort& sort = IOBlockSort::AnyOrder, //!< [in] I/O blocks will be returned in this order
        const std::string& user = std::string()          //!< [in] Check read authority to I/O blocks for user
);

/*!
 *
 * \brief Get jobs using filter.
 *
 * Retrieves the jobs matching the filter criteria.
 *
 * \throws bgsched::DatabaseException with values:
 * - bgsched::DatabaseErrors::DatabaseError - if error occurs accessing the database
 * - bgsched::DatabaseErrors::OperationFailed - if database operation failed
 *
 * \throws bgsched::InternalException with values:
 * - bgsched::InternalErrors::UnexpectedError - if any type of unexpected error occurs
 *
 * \return Jobs matching the job filter criteria.
 */
Job::ConstPtrs getJobs(
        const JobFilter& filter,                 //!< [in] Job filter to search on
        const JobSort& sort = JobSort::AnyOrder, //!< [in] Jobs will be returned in this order
        const std::string& user = std::string()  //!< [in] Check read authority to jobs for user
);


} // namespace bgsched::core
} // namespace bgsched

#endif

