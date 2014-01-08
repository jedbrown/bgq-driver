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
#ifndef RUNJOB_CLIENT_DEBUGGER_H
#define RUNJOB_CLIENT_DEBUGGER_H

#include "common/tool/Daemon.h"

#include "common/message/Proctable.h"
#include "common/message/Result.h"

#include "common/fwd.h"

#include "client/fwd.h"

#include <boost/system/error_code.hpp>

#include <boost/asio.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

/*!
 * \brief structure for MPI Debug support.
 */
typedef struct {
    const char* host_name;         /*!<  Something to pass to inet_addr */
    const char* executable_name;   /*!<  The name of the image */
    int    pid;                    /*!<  The pid of the process */
} MPIR_PROCDESC;

/*!
 * \brief Dummy function invoked periodically by runjob at various points 
 * during program execution.
 */
extern "C" void MPIR_Breakpoint();

namespace runjob {
namespace client {

/*!
 * \brief
 */
class Debugger : public boost::enable_shared_from_this<Debugger>
{
public:
    /*!
     * \brief
     */
    enum class State
    {
        Idle = 0,
        Spawned = 1,
        Aborting = 2
    };

    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Debugger> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            boost::asio::io_service& io_service //!< [in]
            );

public:
    /*!
     * \brief dtor.
     */
    ~Debugger();

    /*!
     * \brief
     */
    void start(
            const boost::weak_ptr<MuxConnection>& mux //!< [in]
            );

    /*!
     * \brief
     */
    void stop();

    /*!
     * \brief
     */
    bool attached();

    /*!
     * \brief
     */
    const tool::Daemon& tool();

    /*!
     * \brief
     */
    void fillProctable(
            const runjob::message::Proctable::ConstPtr& proctable,  //!< [in]
            const JobInfo& info                                     //!< [in]
            );

    /*!
     * \brief
     */
    void handle(
            const runjob::message::Result::ConstPtr& result
            );

private:
    __attribute__ ((visibility("hidden"))) Debugger(
            boost::asio::io_service& io_service
            );

    void __attribute__ ((visibility("hidden"))) wait();

    void __attribute__ ((visibility("hidden"))) callback(
            const boost::system::error_code& error
            );

    bool __attribute__ ((visibility("hidden"))) isAttached() const;
    
    void __attribute__ ((visibility("hidden"))) parseArguments();

private:
    boost::weak_ptr<MuxConnection> _mux;
    boost::asio::deadline_timer _timer;
    tool::Daemon _tool;
    int _being_debugged;
    bool _stopped;
};

} // client
} // runjob

#endif


