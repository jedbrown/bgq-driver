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
#ifndef RUNJOB_SERVER_JOB_STATUS_H
#define RUNJOB_SERVER_JOB_STATUS_H

#include "server/performance/Counters.h"

#include "server/fwd.h"

#include <boost/shared_ptr.hpp>

#include <boost/function.hpp>

#include <bitset>

namespace runjob {
namespace server {
namespace database {

class Init;

} // database

namespace job {

/*!
 * \brief Job status.
 */
class Status
{
public:
    /*!
     * \brief all possible job status values.
     */
    enum Value {
        Invalid,
        Cleanup,
        ClientStarting,
        ControlStarting,
        Debug,
        Loading,
        OutputStarting,
        Running,
        Setup,
        Terminating,
        NumStatuses
    };

public:
    /*!
     * \brief convert a job status to a string.
     */
    static const std::string& toString(
            const Value s        //!< [in]
            )
    {
        static const std::string strings[] = {
            "INVALID",
            "CLEANUP",
            "CLIENT STARTING",
            "CONTROL STARTING",
            "DEBUG",
            "LOADING",
            "OUTPUT STARTING",
            "RUNNING",
            "SETUP",
            "TERMINATING"
        };

        if (s >= Cleanup && s < NumStatuses) {
            return strings[s];
        } else {
            return strings[0];
        }
    }

    /*!
     * \brief convert a job status from a string.
     *
     * \throws std::invalid_argument
     */
    static Value fromString(
            const std::string& s    //!< [in]
            )
    {
        for ( unsigned i = 1; i < NumStatuses; ++i ) {
            if ( s == toString( static_cast<Value>(i) ) ) {
                return static_cast<Value>(i);
            }
        }

        throw std::invalid_argument(s);
    }

    /*!
     * \brief Bit vector of all status values.
     */
    typedef std::bitset<NumStatuses> Flags;

public:
    /*!
     * \brief ctor.
     */
    Status(
            const boost::shared_ptr<Server>& server    //!< [in]
          );

    /*!
     * \brief dtor.
     */
    ~Status() { }

    /*!
     * \brief Set Value.
     */
    void set(
            Value status,                       //!< [in]
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief Get Value.
     */
    Value get() const { return _status; }

    /*!
     * \brief
     */
    bool started() const { return _started; }

    /*!
     * \brief Conversion to string.
     */
    operator const std::string&() const { return toString( _status ); }

private:
    Value _status;                                              //!<
    boost::shared_ptr<database::Init> _database;                //!<
    const performance::Counters::JobContainer::Ptr _counters;   //!<
    performance::Counters::JobContainer::Timer::Ptr _counter;   //!<
    bool _started;                                              //!<
};

} // job
} // server 
} // runjob

#endif
