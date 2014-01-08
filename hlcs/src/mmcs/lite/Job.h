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

#ifndef MMCS_LITE_JOB_H
#define MMCS_LITE_JOB_H

#include "ClassRoute.h"
#include "Connection.h"
#include "JobInfo.h"

#include "server/BlockHelper.h"

#include <ramdisk/include/services/MessageHeader.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/weak_ptr.hpp>

#include <bitset>
#include <fstream>

class BlockControllerBase;


namespace mmcs {
namespace lite {


/*!
 * \brief Thread to manage a job.
 */
class Job : public boost::enable_shared_from_this<Job>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Job> Ptr;

    /*!
     * \brief Weak pointer type.
     */
    typedef boost::weak_ptr<Job> WeakPtr;

    /*!
     * \brief all possible job status values.
     */
    enum Status {
        Invalid,
        AuthenticateControl,
        AuthenticateData,
        Connecting,
        Drained,
        Ending,
        Loading,
        Running,
        Setup,
        Starting,
        Terminating,
        NumStatuses
    };

    /*!
     * \brief convert a job status to a string.
     */
    const char* toString(
            Status s        //!< [in]
            ) {
        static const char* string[] = {
            "INVALID",
            "AUTHENTICATE CONTROL",
            "AUTHENTICATE DATA",
            "CONNECTING",
            "DRAINED",
            "ENDING",
            "LOADING",
            "RUNNING",
            "SETUP",
            "STARTING",
            "TERMINATING"
        };

        if (s >= Invalid && s < NumStatuses) {
            return string[s];
        } else {
            return "INVALID";
        }
    }

public:
    /*!
     * \brief Factory method.
     *
     * \throws anything that Job::Job throws
     */
    static Ptr create(
            const JobInfo& info,    //!< [in]
            const server::BlockPtr& block   //!< [in]
            )
    {
        return Ptr( new Job(info, block) );
    }

    /*!
     * \brief Start this job.
     */
    void start();

    /*!
     * \brief Send the job a signal.
     */
    void kill(
            int signal = SIGKILL
            );

    /*!
     * \brief dtor.
     */
    ~Job();

    // getter
    const JobInfo& getInfo() const { return _info; }

private:
    /*!
     * \brief Status.
     */
    typedef std::bitset<NumStatuses> StatusFlag;

private:
    /*!
     * \brief ctor.
     *
     * \throws std::runtime_error if job simulation is enabled and either of the control or
     * data ports cannot be found in the simulation ID directory.
     */
    Job(
            const JobInfo& info,    //!< [in]
            const server::BlockPtr& block   //!< [in]
       );

    /*!
     * \brief Set status flag and start timer for that flag.
     */
    void setStatus(
            Status flag     //!< [in]
            );

    /*!
     * \brief
     */
    uint16_t getPort(
            const char* name    //!< [in]
            );

    /*!
     * \brief
     */
    void killImpl(
            int signal  //!< [in]
            );

    /*!
     * \brief
     */
    void read(
            Connection::Ptr connection     //!< [in]
            );

    /*!
     * \brief
     */
    void readHeader(
            Connection::Ptr connection     //!< [in]
            );

    /*!
     * \brief
     */
    void write(
            Connection::Ptr connection,     //!< [in]
            boost::shared_ptr<void> msg,    //!< [in]
            size_t length                   //!< [in]
            );

    /*!
     * \brief connect handler.
     */
    void handleConnect(
            Connection::Ptr connection,                 //!< [in]
            const boost::asio::ip::tcp::endpoint& ep,   //!< [in]
            const boost::system::error_code& error      //!< [in]
            );

    /*!
     * \brief
     */
    void authenticateControl();

    /*!
     * \brief
     */
    void authenticateData();

    /*!
     * \brief
     */
    void setup();

    /*!
     * \brief
     */
    void load();

    /*!
     * \brief
     */
    void startTimer(
            Status status   //!< [in]
            );

    /*!
     * \brief
     */
    void readHeaderHandler(
            Connection::Ptr connection,                 //!< [in]
            const boost::asio::ip::tcp::endpoint& ep,   //!< [in]
            const boost::system::error_code& error      //!< [in]
            );

    /*!
     * \brief
     */
    void readMessageHandler(
            Connection::Ptr connection,                 //!< [in]
            const boost::asio::ip::tcp::endpoint& ep,   //!< [in]
            const boost::system::error_code& error,     //!< [in]
            size_t bytesTransferred                     //!< [in]
            );
    /*!
     * \brief
     */
    void writeHandler(
            Connection::Ptr connection,                 //!< [in] connection
            const boost::asio::ip::tcp::endpoint& ep,   //!< [in]
            boost::shared_ptr<void> message,            //!< [in] message that was just sent
            const boost::system::error_code& error      //!< [in] error
            );

    /*!
     * \brief
     */
    void handleTimer(
            Status status,                              //!< [in]
            const boost::system::error_code& error      //!< [in] error
            );
private:
    boost::shared_ptr<boost::asio::io_service> _io_service; //!<
    boost::asio::deadline_timer _timer;                     //!<
    boost::asio::strand _strand;                            //!< strand for protecting access to _status
    Connection::Ptr _control;                               //!< control connection
    Connection::Ptr _data;                                  //!< data connection
    JobInfo _info;                                          //!<
    const server::BlockPtr _block;                          //!<
    const ClassRoute _classRoute;                           //!<
    uint32_t _size;                                         //!<
    int32_t _sim_id;                                        //!<
    uint16_t _control_port;                                 //!<
    uint16_t _data_port;                                    //!<
    StatusFlag _status;                                     //!<
    int _exit_status;                                       //!<
    std::ostringstream _error_text;                         //!<
    std::ofstream _stdout;                                  //!<
    std::ofstream _stderr;                                  //!<
    boost::posix_time::ptime _start_time;                   //!<
};


} } // namespace mmcs::lite


#endif
