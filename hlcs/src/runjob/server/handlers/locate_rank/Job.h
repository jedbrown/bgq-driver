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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_JOB_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_JOB_H

#include "server/handlers/locate_rank/fwd.h"

#include "common/Mapping.h"
#include "common/Uci.h"

#include "server/fwd.h"

#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/job/types.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief Abstract base for active and history jobs.
 *
 * This class calculates the coordinates of a rank using the job's shape
 * and the mapping.
 *
 * \see Mapping
 * \see job::RankMapping
 */
class Job
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Job> Ptr;

public:
    /*!
     * \brief
     */
    static Ptr create(
            const cxxdb::ConnectionPtr& db,         //!< [in]
            BGQDB::job::Id job                      //!< [in]
            );

    /*!
     * \brief dtor.
     */
    virtual ~Job() = 0;

    /*!
     * \brief
     */
    Uci find(
            uint32_t rank   //!< [in]
            );

    BGQDB::job::Id id() const { return _id; }
    const std::string& block() const { return _blockId; }
    const std::string& corner() const { return _corner; }
    int blockCreationId() const;
    const Mapping& mapping() const { return _mapping; }
    const boost::posix_time::ptime& startTime() const { return _startTime; } 

protected:
    Job(
            BGQDB::job::Id job
       );

private:
    Uci findCoordinates(
            uint32_t rank
            );

protected:
    boost::shared_ptr<Block> _block;
    const BGQDB::job::Id _id;
    unsigned _shape[6];
    std::string _blockId;
    Mapping _mapping;
    std::string _corner;
    unsigned _np;
    boost::posix_time::ptime _startTime;
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
