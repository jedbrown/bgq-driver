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
#ifndef RUNJOB_SERVER_BLOCK_COMPUTE_CORE_H
#define RUNJOB_SERVER_BLOCK_COMPUTE_CORE_H

#include "server/Job.h"
#include "server/fwd.h"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <string>

#include <stdint.h>

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief Represents a single compute core on a ComputeNode.
 *
 * \see Compute
 * \see ComputeNode
 * \see Midplane
 */
class ComputeCore
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<ComputeCore> Ptr;

public:
    /*!
     * \brief ctor.
     */
    ComputeCore(
            size_t core_number  //!< [in]
            );

    /*!
     * \brief Get core number.
     */
    size_t getCore() const { return _core; }

    /*!
     * \brief Get the job running on this core.
     */
    const boost::weak_ptr<Job>& getJob() { return _job; }

    /*!
     * \brief Set job.
     */
    void setJob(
            const boost::weak_ptr<Job>& job //!< [in]
            );

private:
    const size_t _core;
    boost::weak_ptr<Job> _job;
};

} // block
} // server
} // runjob

#endif
