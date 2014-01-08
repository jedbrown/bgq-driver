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
#ifndef RUNJOB_CLIENT_PROCTABLE_PROMPT_H
#define RUNJOB_CLIENT_PROCTABLE_PROMPT_H

#include "client/Job.h"

#include "common/message/Proctable.h"

#include <db/include/api/job/types.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

#include <boost/asio/posix/stream_descriptor.hpp>

#include <boost/asio/io_service.hpp>
#include <boost/asio/streambuf.hpp>

namespace runjob {
namespace client {

class MuxConnection;

/*!
 * \brief 
 */
class ProctablePrompt : public boost::enable_shared_from_this<ProctablePrompt>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<ProctablePrompt> Ptr;

public:
    static Ptr create(
            boost::asio::io_service& io_service,            //!< [in]
            const boost::weak_ptr<MuxConnection>& mux,      //!< [in]
            BGQDB::job::Id id,                              //!< [in]
            Job::Status& status                             //!< [in]
            );

    /*!
     * \brief
     */
    ~ProctablePrompt();

    /*!
     * \brief
     */
    void start(
            const message::Proctable::ConstPtr& proctable   //!< [in]
            );

    /*!
     * \brief
     */
    void stop();

private:
    __attribute__ ((visibility("hidden"))) ProctablePrompt(
            boost::asio::io_service& io_service,
            const boost::weak_ptr<MuxConnection>& mux,
            BGQDB::job::Id id,
            Job::Status& status
            );

    __attribute__ ((visibility("hidden"))) void read();

    __attribute__ ((visibility("hidden"))) void readHandler(
            const boost::system::error_code& error,
            size_t length
            );

private:
    boost::asio::posix::stream_descriptor _input;
    boost::asio::streambuf _buffer;
    const boost::weak_ptr<MuxConnection> _mux;
    const BGQDB::job::Id _id;
    Job::Status& _status;
    message::Proctable::ConstPtr _proctable;
};

} // client
} // runjob

#endif

