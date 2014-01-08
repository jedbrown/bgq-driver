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
#ifndef RUNJOB_SERVER_BLOCK_MIDPLANE_H
#define RUNJOB_SERVER_BLOCK_MIDPLANE_H

#include "common/Coordinates.h"
#include "common/fwd.h"
#include "common/Uci.h"

#include "server/block/Compute.h"
#include "server/block/fwd.h"
#include "server/block/Io.h"

#include "server/job/fwd.h"

#include "server/fwd.h"

#include <control/include/bgqconfig/BGQTopology.h>

#include <boost/multi_array.hpp>
#include <boost/weak_ptr.hpp>

#include <string>
#include <vector>

class BGQMidplaneNodeConfig;

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief Contains up to 512 compute nodes.
 *
 * The primary purpose of this class is to enforce job arbitration. It creates a 5
 * dimensional array of runjob::server::block::ComputeNode objects. Each compute node can
 * run one or more jobs in the case of sub-block jobs.
 *
 * \note even though this class is called %Midplane, it can contain less than
 * 512 nodes for small blocks.
 *
 * \see Compute
 * \see ComputeCore
 * \see ComputeNode
 * \see Io
 */
class Midplane
{
public:
    /*!
     * \brief
     */
    typedef std::vector<std::string> Nodeboards;

public:
    /*!
     * \brief ctor.
     */
    Midplane(
            const std::string& block,           //!< [in] block name
            const std::string& location,        //!< [in] location string of the midplane
            BGQMidplaneNodeConfig& config,      //!< [in] midplane configuration
            const Io::Links& links              //!< [in] I/O link container
            );

    /*!
     * \brief dtor.
     */
    ~Midplane();

    /*!
     * \brief Arbitrate a sub-block resource in this midplane.
     * \returns true if the job should be loaded
     * \returns false otherwise
     */
    bool arbitrate(
            const SubBlock& resource,                   //!< [in]
            const boost::shared_ptr<job::Create>& job   //!< [in]
            );

    /*!
     * \brief Arbitrate a full-block job in this midplane.
     *
     * \returns true if the job should be loaded
     * \returns false otherwise
     */
    bool arbitrate(
            const boost::shared_ptr<job::Create>& job     //!< [in]
            );

    /*!
     * \brief
     */
    void unavailable(
            const boost::shared_ptr<Job>& job       //!< [in]
            );

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
    const boost::shared_ptr<ComputeNode>& node(
            const Coordinates& coordinates  //!< [in]
            ) const;

    /*!
     * \brief
     */
    const Uci& location() const { return _location; }

    /*!
     * \brief
     */
    const Nodeboards& nodeboards() const { return _nodeboards; }

    /*!
     * \brief If less than the full midplane is in use.
     */
    bool small() const { return _smallBlock; }

    /*!
     * \brief
     */
    const Compute::IoRatio& io() const { return _io; }

private:
    void getCoordinates(
            const BGQMidplaneNodeConfig& config
            );

    void resize(
            BGQMidplaneNodeConfig&
            );

    bool available(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateCorner(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateCornerLocation(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateCornerCoordinates(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateShape(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateSubBlockNp(
            const boost::shared_ptr<job::Create>& job
            );
    
    bool validateStdinRank(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateStraceRank(
            const boost::shared_ptr<job::Create>& job
            );

    bool addSingleCoreJob(
            const boost::shared_ptr<job::Create>& job
            );
    
    bool addMultiNodeJob(
            const boost::shared_ptr<job::Create>& job
            );

    const Coordinates* getCorner(
            const Corner& corner
            );

private:
    /*!
     * \brief Number of dimensions.
     */
    static const unsigned NumDimensions = 5;

    /*!
     * \brief ComputeNode container type.
     */
    typedef boost::multi_array<boost::shared_ptr<ComputeNode>, NumDimensions> NodeArray;

    /*!
     * \brief Mapping of location strings to coordinates.
     */
    typedef std::map<Uci, Coordinates> CornerMap;

private:
    friend class Reconnect;
    const std::string _block;               //!< block name
    const Uci _location;                    //!< location
    const bool _smallBlock;                 //!< true if this 'midplane' uses 32, 64, 128, or 256 nodes
    Coordinates _coordinates;               //!< coordinates within block
    Coordinates _offset;                    //!< offset for small blocks
    NodeArray _nodes;                       //!< compute node container
    Nodeboards _nodeboards;                 //!< Nodeboard container
    CornerMap _corners;                     //!< mapping of corner location strings to coordinates
    Compute::IoRatio _io;                   //!< number of computes per I/O link
    boost::weak_ptr<Job> _job;              //!< job running on the entire midplane
};

} // block
} // server
} // runjob

#endif

