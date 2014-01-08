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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#ifndef MMCS_LITE_CONSOLE_CONTROLLER_H_
#define MMCS_LITE_CONSOLE_CONTROLLER_H_


#include "common/ConsoleController.h"

#include "libmmcs_client/CommandReply.h"


class BGQMachineXML;


namespace mmcs {
namespace lite {


class Job;


class ConsoleController : public common::ConsoleController
{
public:
    ConsoleController(
            MMCSCommandProcessor* commandProcessor,     //!< [in] command processor
            BGQMachineXML* machine,                     //!< [in] machine XML description
            const bgq::utility::UserId& user            //!< [in] user
            );
    void genIOBlockController(std::deque<std::string> args, std::string& blockName, std::istream& xml, mmcs_client::CommandReply& reply);
    void genCNBlockController(std::deque<std::string> args, std::string& blockName, std::istream& xml, mmcs_client::CommandReply& reply);
    bool blockInitialized() { return _block_initialized; }
    void setBlockInitialized(
            bool tf
            )
    {
        _block_initialized = tf;
    }

    /*!
     * \brief Get the currently active job.
     */
    boost::weak_ptr<lite::Job> getJob() const { return _job; }

    /*!
     * \brief Set the currently active job.
     */
    void setJob(
            const boost::shared_ptr<lite::Job> job    //!< [in]
            )
    {
        _job = job;
    }

private:
    BGQMachineXML* _machine;
    bool _block_initialized;
    boost::weak_ptr<lite::Job> _job;
};


} } // namespace mmcs::lite

#endif
