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
#ifndef RUNJOB_SERVER_JOB_IO_H 
#define RUNJOB_SERVER_JOB_IO_H 

#include "server/block/fwd.h"

#include "server/cios/fwd.h"

#include "server/job/Status.h"

#include "common/JobInfo.h"
#include "common/Uci.h"

#include "server/fwd.h"

#include <ramdisk/include/services/MessageHeader.h>

#include <boost/logic/tribool.hpp>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <map>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief A single I/O node controlling compute nodes for a job.
 */
class IoNode
{
public:
    /*!
     * \brief mapping of I/O node location to this type.
     */
    typedef std::map<Uci, IoNode> Map;

    /*!
     * \brief Callback type.
     */
    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief ctor.
     */
    IoNode(
            const boost::shared_ptr<block::IoNode>& node,   //!< [in]
            unsigned computes                               //!< [in]
          );
    
    /*!
     * \brief Get and set the drained flag.
     */
    bool drained(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the setup flag.
     */
    bool setup(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the loaded flag.
     */
    bool loaded(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the output started flag.
     */
    bool output_started(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the ended flag.
     */
    bool ended(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get the Running flag.
     */
    bool running() const { return _flags.test( Status::Running ); }

    /*!
     * \brief Get signal in flight flag.
     */
    bool signalInFlight() const { return _signalInFlight; }

    /*!
     * \brief Get and set the error flag.
     */
    bool error(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the killed flag.
     */
    bool killed(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the hardware failure flag.
     */
    bool hardwareFailure(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );

    /*!
     * \brief Get and set the exited flag.
     */
    bool exited(
            const boost::tribool& value = boost::logic::indeterminate   //!< [in]
            );
    
    /*!
     * \brief Get the cleanup flag.
     */
    bool cleanup() const { return _flags.test( Status::Cleanup ); }

    /*!
     * \brief Write a message on the control connection.
     */
    void writeControl(
            const boost::shared_ptr<cios::Message>& msg   //!< [in]
            );

    /*!
     * \brief Write a message on the data connection.
     */
    void writeData(
            const boost::shared_ptr<cios::Message>& msg   //!< [in]
            );

    /*!
     * \brief Handle a control message.
     */
    void handleControl(
            const bgcios::MessageHeader* header,                //!< [in]
            const boost::shared_ptr<cios::Message>& message,    //!< [in]
            const boost::shared_ptr<Job>& job                   //!< [in]
            );

    /*!
     * \brief Handle a control message.
     */
    void handleData(
            const bgcios::MessageHeader* header,                //!< [in]
            const boost::shared_ptr<cios::Message>& message,    //!< [in]
            const boost::shared_ptr<Job>& job,                  //!< [in]
            const Callback& callback                            //!< [in]
            );

    /*!
     * \brief Get the number of compute nodes this I/O node manages.
     */
    unsigned getComputes() const {
        return _numComputes;
    }

    /*!
     * \brief Set the number of compute nodes this I/O node manages.
     */
    void setComputes(
            unsigned computes
            )
    {
        _numComputes = computes;
    }

    /*!
     * \brief Get the block::IoNode object.
     */
    const boost::shared_ptr<block::IoNode>& get() const { return _node; }

private:
    void setupFailure(
            const bgcios::MessageHeader* header,
            const boost::shared_ptr<Job>& job
            );

    void loadFailure(
            const bgcios::MessageHeader* header,
            const boost::shared_ptr<Job>& job
            );
    
    void startFailure(
            const bgcios::MessageHeader* header,
            const boost::shared_ptr<Job>& job
            );

    void cleanupFailure(
            const bgcios::MessageHeader* header,
            const boost::shared_ptr<Job>& job
            );

    void prologFailure(
            const bgcios::MessageHeader* header,
            const boost::shared_ptr<Job>& job
            );

    void epilogFailure(
            const boost::shared_ptr<cios::Message>& msg,
            const boost::shared_ptr<Job>& job
            );

private:
    friend class runjob::server::Job;
    Status::Flags _flags;
    unsigned _numComputes;
    bool _drained;
    bool _killed;
    bool _hardwareFailure;
    bool _ended;
    bool _error;
    bool _exited;
    bool _loaded;
    bool _setup;
    bool _output_started;
    bool _signalInFlight;
    boost::shared_ptr<block::IoNode> _node;
};

} // job
} // server
} // runjob

#endif
