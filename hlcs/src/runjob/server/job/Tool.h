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
#ifndef RUNJOB_SERVER_JOB_TOOL_H
#define RUNJOB_SERVER_JOB_TOOL_H

#include "common/tool/Daemon.h"
#include "common/tool/Proctable.h"

#include "common/Uci.h"

#include "server/job/tool/End.h"
#include "server/job/tool/Status.h"

#include "server/fwd.h"

#include <hlcs/include/runjob/commands/error.h>

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <map>
#include <string>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class Tool : public boost::enable_shared_from_this<Tool>
{
public:
    /*!
     * \brief
     */
    typedef boost::function<
        void(
                unsigned,                       // tool ID
                runjob::commands::error::rc,    // error
                const std::string&              // descriptive message
                )
        > Callback;
   
    /*!
     * \brief
     */
    typedef std::map<Uci, tool::Status> Io;

    /*!
     * \brief
     */
    typedef boost::shared_ptr<Tool> Ptr;

public:
    /*!
     * \brief ctor.
     */
    Tool(
            unsigned id,                                //!< [in]
            const boost::shared_ptr<Job>& job,          //!< [in]
            const runjob::tool::Daemon& tool,           //!< [in]
            runjob::tool::Proctable& proctable,         //!< [in]
            const Callback& callback                    //!< [in]
        );

    /*!
     * \brief dtor.
     */
    ~Tool();

    /*!
     * \brief
     */
    Ptr update( 
            tool::Status status,    //!< [in]
            const Uci& location     //!< [in]
            );

    /*!
     * \brief
     */
    void failure(
            const boost::shared_ptr<bgcios::jobctl::StartToolAckMessage>& ack,  //!< [in]
            const Uci& location                                                 //!< [in]
            );

    /*!
     * \brief
     */
    void exited(
            const boost::shared_ptr<bgcios::jobctl::ExitToolMessage>& etm,  //!< [in]
            const Uci& location                                             //!< [in]
            );

private:
    void updateDatabase(
            tool::Status status
            );

private:
    friend class tool::End;

    boost::weak_ptr<Job> _job;
    unsigned _id;
    Io _io;
    std::string _error;
    Callback _callback;
}; 

} // tool
} // job
} // server

#endif
