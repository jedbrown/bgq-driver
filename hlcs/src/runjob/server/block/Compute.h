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
#ifndef RUNJOB_SERVER_BLOCK_COMPUTE_H
#define RUNJOB_SERVER_BLOCK_COMPUTE_H

#include "server/block/fwd.h"
#include "server/block/Io.h"

#include "server/job/fwd.h"

#include "common/fwd.h"
#include "common/Uci.h"

#include "server/Block.h"
#include "server/fwd.h"

#include <bgq_util/include/Location.h>

#include <boost/asio/strand.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>
#include <vector>

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief Compute block.
 *
 * The primary purpose for this class is for job arbitration. Arbitration happens on two different levels
 *
 * # Full-block jobs.
 * # Sub-block jobs.
 *
 * The arbitration is largely the same in either scenario. Sub-block jobs can only run within a single
 * midplane, but full-block jobs use all midplanes in the block.
 *
 * \see ComputeNode
 * \see ComputeCore
 * \see Container
 * \see Io
 * \see Midplane
 *
 */
class Compute : public Block, public boost::enable_shared_from_this<Compute>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Compute> Ptr;
    
    /*!
     * \brief mapping of locations to Midplane objects.
     */
    typedef std::map<std::string, Midplane> Midplanes;

    /*!
     * \brief
     */
    typedef std::vector<Uci> Nodes;

    /*!
     * \brief I/O to compute ratio.
     */
    typedef std::map<Uci, unsigned> IoRatio;

public:
    /*!
     * \brief ctor.
     */
    static Ptr create(
            const std::string& name,                                //!< [in] block name
            const boost::shared_ptr<BGQBlockNodeConfig>& block,     //!< [in] block configuration
            const boost::shared_ptr<Server>& server,                //!< [in] configuration objects
            const Io::Links& links                                  //!< [in] I/O links 
            );

    /*!
     * \brief Get midplane container.
     */
    const Midplanes& midplanes() const { return _midplanes; }

    /*!
     * \brief Arbitrate a job against the requested resources.
     */
    void arbitrate(
            const boost::shared_ptr<job::Create>& job   //!< [in]
            );

    /*!
     * \brief
     */
    void unavailable(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief
     */
    const IoRatio& io() const { return _io; }

    /*!
     * \brief
     */
    unsigned size() const { return _size; }

private:
    Compute(
            const std::string& name,
            const boost::shared_ptr<BGQBlockNodeConfig>& block,
            const boost::shared_ptr<Server>& server,
            const Io::Links& links
            );

    bool validateStdinNp(
            const boost::shared_ptr<job::Create>& job
            );

    bool validateStraceNp(
            const boost::shared_ptr<job::Create>& job
            );

    void arbitrateImpl(
            const boost::shared_ptr<job::Create>& job
            );

    void arbitrateFullBlock(
            const boost::shared_ptr<job::Create>& job
            );

    void arbitrateSubBlock(
            const boost::shared_ptr<job::Create>& job
            );

    std::string getMidplane(
            const boost::shared_ptr<job::Create>& job,
            const Corner& corner
            );

    std::string getMidplaneFromCoordinates(
            const boost::shared_ptr<job::Create>& job,
            const Coordinates& coordinates
            );

    void unavailableJobImpl(
            const boost::shared_ptr<Job>& job
            );

private:
    friend class Reconnect;
    boost::asio::strand _strand;                            //!<
    Midplanes _midplanes;                                   //!<
    const unsigned _size;                                   //!<
    IoRatio _io;                                            //!<
};

} // block
} // server
} // runjob

#endif
