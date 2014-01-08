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
#ifndef RUNJOB_SERVER_BLOCK_COMPUTE_NODE_H
#define RUNJOB_SERVER_BLOCK_COMPUTE_NODE_H

#include "server/block/Compute.h"
#include "server/block/fwd.h"
#include "server/block/Io.h"

#include "server/job/fwd.h"

#include "common/fwd.h"
#include "common/Uci.h"

#include "server/fwd.h"

#include <bgq_util/include/Location.h>

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <iosfwd>
#include <string>

#include <stdint.h>

class BGQNodePos;

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief Represents a single compute node with 16 cores.
 *
 * The primary purpose of this class is to manage job arbitration.
 *
 * \see Compute
 * \see ComputeCore
 * \see Midplane
 */
class ComputeNode
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<ComputeNode> Ptr;

    /*!
     * \brief
     */
    enum class Status
    {
        Available,
        SoftwareFailure
    };

public:
    /*!
     * \brief ctor.
     */
    ComputeNode(
            const Uci& midplane,                        //!< [in] midplane location
            const BGQNodePos& compute,                  //!< [in] position within midplane
            const Uci& connectedIo,                     //!< [in] location of connected I/O node
            const Io::Links& links                      //!< [in] I/O link container
            );

    /*!
     * \brief Get UCI location.
     */
    const Uci& getLocation() const { return _location; }

    /*!
     * \brief Get the connected I/O node.
     */
    const boost::shared_ptr<IoNode>& getIo() const { return _io; }

    /*!
     * \brief Get the connected I/O node.
     */
    const boost::shared_ptr<IoNode>& getConnectedIo() const { return _connectedIo; }

    /*!
     * \brief
     */
    bool available(
            const boost::shared_ptr<job::Create>& job
            ) const;

    /*!
     * \brief Set job.
     */
    void setJob(
            const boost::weak_ptr<Job>& job //!< [in]
            )
    {
        _job = job;
    }

    /*!
     * \brief
     */
    void unavailable(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief
     */
    void unavailable();

    /*!
     * \brief Arbitrate sub-node job resources.
     */
    bool arbitrate(
        const SubBlock& resource,                   //!< [in]
        const boost::shared_ptr<job::Create>& job   //!< [in]
        );

private:
    Uci _location;
    boost::shared_ptr<IoNode> _io;
    boost::shared_ptr<IoNode> _connectedIo;
    boost::weak_ptr<Job> _job;
    Status _status;
    boost::array<boost::shared_ptr<ComputeCore>, bgq::util::Location::ComputeCardCoresOnBoard - 1> _cores;
    const boost::shared_ptr<job::SubNodePacing> _pacing;
};

std::ostream&
operator<<(
        std::ostream&,          //!< [in]
        ComputeNode::Status     //!< [in]
        );

} // block
} // server
} // runjob

#endif
