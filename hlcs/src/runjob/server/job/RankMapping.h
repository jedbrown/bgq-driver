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
#ifndef RUNJOB_SERVER_JOB_RANK_MAPPING_H
#define RUNJOB_SERVER_JOB_RANK_MAPPING_H

#include "common/tool/Proctable.h"

#include "common/fwd.h"
#include "common/Uci.h"

#include "server/fwd.h"

#include <spi/include/kernel/location.h>

#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class RankMapping
{
public:
    /*!
     * \brief
     */
    RankMapping(
            const boost::shared_ptr<Job>& job,  //!< [in]
            runjob::tool::Proctable& container  //!< [in]
            );

    /*!
     * \brief
     */
    ~RankMapping();

private:
    BG_JobCoords_t getJobShape(
            const boost::shared_ptr<Job>& job
            );

    void findLocation(
            const boost::shared_ptr<Job>& job,
            unsigned rank,
            BG_CoordinateMapping_t& coordinates
            );

private:
    runjob::tool::Proctable& _proctable;
};

} // job
} // server
} // runjob

#endif
