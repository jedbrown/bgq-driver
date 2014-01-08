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
#ifndef RUNJOB_SERVER_SIM_INOTIFY_H
#define RUNJOB_SERVER_SIM_INOTIFY_H

#include "server/cios/fwd.h"

#include "server/sim/Watch.h"

#include <utility/include/Inotify.h>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace runjob {
namespace server {
namespace sim {

/*!
 * \brief Wrapper around bgq::utility::Inotify object to obtain iosd
 * ephemeral port numbers from the filesystem.
 *
 * Files are added to the notification queue using Inotify::watch. When a notification
 * event happens, the file is opened and the simulated cios daemon's port is
 * obtained. The cios::Connection is then started using the port number.
 */
class Inotify : public boost::enable_shared_from_this<Inotify>, boost::noncopyable
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Inotify> Ptr;

public:
    /*!
     * \brief ctor.
     */
    Inotify(
            boost::asio::io_service& io_service,    //!< [in]
            const std::string& block                //!< [in]
        );

    /*!
     * \brief dtor.
     */
    ~Inotify();

    /*!
     * \brief Start reading.
     */
    void start();

    /*!
     * \brief Stop reading.
     */
    void stop();

    /*!
     * \brief Add a path to the watch list.
     */
    void watch(
            const boost::filesystem::path& path,                    //!< [in]
            const std::string& file,                                //!< [in]
            const boost::shared_ptr<cios::Connection>& connection   //!< [in]
            );

private:
    typedef boost::shared_ptr<Watch> WatchPtr;
    typedef std::vector<WatchPtr> WatchContainer;

private:
    void stopImpl();

    void startImpl();

    void watchImpl(
            const WatchPtr& watch
            );

    void handleRead(
            const boost::system::error_code&
            );

    void connect(
            const boost::filesystem::path& path,
            const boost::shared_ptr<cios::Connection>&
            );

private:
    boost::asio::strand _strand;            //!< protects the watch container.
    bool _started;                          //!< flag to ensure watch is not invoked after start.
    bgq::utility::Inotify _notify;          //!< inotify descriptor.
    bgq::utility::Inotify::Events _events;  //!< events from most recent read.
    WatchContainer _watches;                //!< events we are watching.
    const std::string _block;               //!< block name.
};

} // sim
} // server
} // runjob

#endif
