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

#ifndef RUNJOB_CLIENT_BGSCHED_CLIENT_IMPL_H
#define RUNJOB_CLIENT_BGSCHED_CLIENT_IMPL_H

#include "client/fwd.h"

#include <bgsched/runjob/Client.h>

#include <utility/include/ExitStatus.h>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace bgsched {
namespace runjob {

/*!
 * \brief
 */
class Client::Impl
{
public:
    /*!
     * \brief Job implementation type.
     */
    typedef ::runjob::client::Job JobImpl;

public:
    /*!
     * \brief Ctor.
     */
    Impl(
            int argc,
            char** argv
        );

    /*!
     * \brief dtor.
     */
    ~Impl();

    /*!
     * \brief Start the job.
     *
     * \returns the job's exit status.
     */
    bgq::utility::ExitStatus start(
            int input,  //!< [in]
            int output, //!< [in]
            int error   //!< [in]
            );

    /*!
     * \brief
     */
    void kill(
            int signal  //!< [in]
            );

    /*!
     * \brief
     */
    boost::asio::io_service& ioService() { return _io_service; }

    /*!
     * \brief
     */
    const boost::shared_ptr<const ::runjob::client::options::Parser>& options() const { return _options; }

    /*!
     * \brief
     */
    const boost::weak_ptr<JobImpl>& job() const { return _job; }

private:
    boost::asio::io_service _io_service;
    boost::shared_ptr<const ::runjob::client::options::Parser> _options;
    boost::weak_ptr<JobImpl> _job;
};

} // runjob
} // bgsched

#endif
