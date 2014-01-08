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
#ifndef RUNJOB_MUX_CLIENT_TIMERS_H_
#define RUNJOB_MUX_CLIENT_TIMERS_H_

#include "mux/client/fwd.h"

#include "mux/performance/Counters.h"

#include <db/include/api/job/types.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief Performance timers for job submission.
 */
class Timers
{
public:
    /*!
     * \brief ctor.
     */
    explicit Timers(
            const Id& id,                                   //!< [in]
            performance::Counters::Container::Ptr counters  //!< [in]
            );

    /*!
     * \brief Start the initialization timer.
     */
    void start_init();

    /*!
     * \brief Start the plugin verify timer.
     */
    void start_plugin_verify();

    /*!
     * \brief start the plugin started timer.
     */
    void start_plugin_started();

    /*!
     * \brief Update all timers with the job ID.
     */
    void update(
            BGQDB::job::Id id
            );

    /*!
     * \brief Stop the init timer.
     */
    void stop_init();

    /*!
     * \brief Stop the plugin verify timer.
     */
    void stop_plugin_verify();

    /*!
     * \brief Add the argument parsing timer.
     */
    void set_argument_parsing(
            const boost::posix_time::time_duration& duration    ///!< [in]
            );
        
private:
    const Id& _id;                                                  //!<
    performance::Counters::Container::Ptr _counters;                //!<
    performance::Counters::Container::Timer::Ptr _init;             //!<
    performance::Counters::Container::Timer::Ptr _plugin_verify;    //!<
    performance::Counters::Container::Timer::Ptr _plugin_started;   //!<
    boost::posix_time::time_duration _argument_parsing;             //!<
};

} // client
} // mux
} // runjob

#endif
