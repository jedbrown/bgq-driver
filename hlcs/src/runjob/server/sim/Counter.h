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
#ifndef RUNJOB_SERVER_SIM_COUNTER_H
#define RUNJOB_SERVER_SIM_COUNTER_H

#include <boost/interprocess/sync/interprocess_mutex.hpp>

namespace runjob {
namespace server {
namespace sim {

/*!
 * \brief counter to be shared across multiple runjob_server processes on a single host.
 *
 * \see SharedMemory
 */
struct Counter
{
    /*!
     * \brief ctor.
     */
    Counter() : 
        _mutex(),
        _count(0)
    {

    }

    boost::interprocess::interprocess_mutex _mutex;     //!< mutex for atomic access to the counter
    uint32_t _count;                                    //!< counter value
};

} // sim
} // server
} // runjob

#endif
