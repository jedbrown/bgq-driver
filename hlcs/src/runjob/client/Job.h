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
#ifndef RUNJOB_CLIENT_JOB_H_
#define RUNJOB_CLIENT_JOB_H_

#include "client/fwd.h"

#include "common/fwd.h"

#include <db/include/api/job/types.h>

#include <utility/include/ExitStatus.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace client {

/*!
 * \brief starts, monitors, and stops a %job.
 */
class Job : public boost::enable_shared_from_this<Job>
{
public:
    /*!
     * \brief all possible job status values.
     */
    enum Status {
        Connecting,
        Debug,
        Inserting,
        Running,
        Starting,
        Terminated,
        Invalid
    };

    /*!
     * \brief Convert a Status value into a string.
     */
    static const std::string& toString(
            Status s    //!< [in]
            ) {
        static const std::string names[] = {
            "Connecting",
            "Debug",
            "Inserting",
            "Running",
            "Starting",
            "Terminated",
            "Invalid"
        };

        if ( s > Connecting && s < Invalid ) {
            return names[s];
        } else {
            return names[Invalid];
        }
    }

public:
    /*!
     * \brief ctor.
     *
     * \throws std::invalid argument if info is missing executable
     */
    Job(
            boost::asio::io_service& io_service,                        //!< [in] asio service
            const boost::shared_ptr<const options::Parser>& options,    //!< [in] program options
            bgq::utility::ExitStatus&                                   //!< [in]
       );

    /*!
     * \brief dtor.
     */
    ~Job();

    /*!
     * \brief Get Status.
     */
    Status getStatus() const { return _status; }

    /*!
     * \brief Start handling the job.
     */
    void start(
            int input,  //!< [in]
            int output, //!< [in]
            int error   //!< [in]
            );

    /*!
     * \brief Handle a response message from the runjob_mux.
     */
    bool handle(
            const boost::shared_ptr<Message>& msg        //!< [in] response
            );

    /*!
     * \brief set exit status.
     */
    void exitStatus(
            const bgq::utility::ExitStatus&
            );

    /*!
     * \brief get ID.
     */
    BGQDB::job::Id getId() const { return _id; }

    /*!
     * \brief Kill the job.
     */
    void kill(
            int signal = SIGKILL //!< [in]
            );

private:
    void __attribute__ ((visibility("hidden"))) add(
            const boost::system::error_code& error
            );

    void __attribute__ ((visibility("hidden"))) validate();

    void __attribute__ ((visibility("hidden"))) inserting(
            const boost::shared_ptr<Message>& msg
            );

    void __attribute__ ((visibility("hidden"))) starting(
            const boost::shared_ptr<Message>& msg
            );

    void __attribute__ ((visibility("hidden"))) debug(
            const boost::shared_ptr<Message>& msg
            );

    void __attribute__ ((visibility("hidden"))) running(
            const boost::shared_ptr<Message>& msg
            );
    
private:
    Status _status;                                         //!<
    boost::shared_ptr<const options::Parser> _options;      //!<
    const JobInfo& _info;                                   //!<
    boost::asio::io_service& _io_service;                   //!<
    boost::weak_ptr<MuxConnection> _mux;                    //!<
    boost::shared_ptr<Timeout> _timeout;                    //!<
    boost::shared_ptr<Input> _stdin;                        //!<
    boost::shared_ptr<Output> _stdout;                      //!<
    boost::shared_ptr<Output> _stderr;                      //!<
    bgq::utility::ExitStatus& _exitStatus;                  //!<
    BGQDB::job::Id _id;
    boost::shared_ptr<Debugger> _debugger;
};

} // client
} // runjob

#endif
