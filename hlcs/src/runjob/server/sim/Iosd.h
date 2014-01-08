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
#ifndef RUNJOB_SERVER_SIM_IOSD_H
#define RUNJOB_SERVER_SIM_IOSD_H

#include "server/cios/fwd.h"

#include "server/sim/SharedMemory.h"

#include "server/fwd.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <sstream>
#include <string>

#include <sys/types.h>

namespace runjob {
namespace server {
namespace sim {

class Inotify;

/*!
 * \brief manages a simulated I/O services daemon (IOSD).
 *
 * A simulated I/O services daemon is used during job simulation to facilitate development
 * and testing of a large portion of the high level control system.  The difference between
 * a production IOSD and a simulated one is where they run and how they start.  Production
 * daemons will run on the I/O nodes.  Simulated daemons run on the same host as runjob_server
 * since they are forked and exec'd from runjob_server. Its output is sent to a file in uniquely
 * directory in /tmp. For example, if an Iosd class is simulating I/O node Q00-I0-J05, the log
 * file would be in /tmp/cios500/Q00-I0-J05.log assuming the simulation counter is 500 for that
 * I/O node.
 *
 * To enable multiple users running multiple simulator environments, the simulated IOSD uses
 * ephemeral ports to communicate with runjob_server.  In the production environment, these
 * ports are fixed since the environment on the I/O node is much more controlled.  To
 * communicate these port numbers to runjob_server, the IOSD requires a unique simulation ID
 * across all instances. This ID is used to create a temporary directory in /tmp/cios1
 * assuming the ID is 1 where the port numbers are stored.
 *
 * This class uses the shared pointer idiom made popular by the Boost.Asio examples. It
 * passes a shared pointer to itself to each asynchronous handler so an object is always
 * in scope. It will delete itself when the IOSD process terminates.
 */
class Iosd : public boost::enable_shared_from_this<Iosd>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Iosd> Ptr;

public:
    /*!
     * \brief ctor.
     *
     * forks and execs the binary using the path + / + name arguments.
     *
     * \pre control and data are not connected
     * \pre id is unique among all IOSD processes
     */
    Iosd(
            const boost::shared_ptr<Inotify>& port, //!< [in]
            const Options& options,                 //!< [in] program options
            const std::string& location,            //!< [in] I/O node location
            const std::string& path,                //!< [in] full path of program to exec
            const std::string& name                 //!< [in] name of program to exec
            );

    /*!
     * \brief start an asynchronous wait for this process to terminate.
     */
    void start(
            const boost::shared_ptr<cios::Connection>& control,    //!< [in]
            const boost::shared_ptr<cios::Connection>& data        //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Iosd();

    /*!
     * \brief Set shared memory counter.
     */
    static void setCounter(
            boost::shared_ptr<SharedMemory> counter   //!< [in]
            );

private:
    void fork();

private:
    boost::weak_ptr<Inotify> _iNotify;              //!<
    const Options& _options;                        //!<
    const std::string _location;                    //!< I/O node location
    const std::string _path;                        //!<
    const std::string _name;                        //!< 
    pid_t _pid;                                     //!< pid of child process
    const uint32_t _id;                             //!< unique simulation ID
};

} // sim
} // server
} // runjob

#endif
