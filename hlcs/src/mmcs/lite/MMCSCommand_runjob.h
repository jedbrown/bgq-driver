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

#ifndef MMCS_LITE_MMCS_COMMAND_RUNJOB_H
#define MMCS_LITE_MMCS_COMMAND_RUNJOB_H

#include "common/AbstractCommand.h"


namespace mmcs {

namespace server {
    class CNBlockController;
    class IOBlockController;
}

namespace lite {


class JobInfo;


/*!
 * \brief
 */
class MMCSCommand_runjob : public common::AbstractCommand
{
public:
    /*!
     * \brief
     */
    MMCSCommand_runjob(
            const char* name,
            const char* description,
            const Attributes& attributes
            );

    /*!
     * \brief
     */
    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget* pTarget=NULL
            );

    /*!
     * \brief
     */
    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );

    /*!
     * \brief
     */
    bool checkArgs(std::deque<std::string>&) { return true; }

public:
    static MMCSCommand_runjob* build();

private:
    /*!
     * \brief
     */
    void insert(
            mmcs_client::CommandReply& reply,    //!< [in]
            JobInfo& info         //!< [in]
            );

    /*!
     * \brief
     */
    void getConnectedIoNode(
            boost::shared_ptr<server::CNBlockController> block,  //!< [in]
            JobInfo& job_info,                        //!< [in]
            mmcs_client::CommandReply& reply                         //!< [in]
            );

    /*!
     * \brief
     */
    void getConnectedIoNode(
            boost::shared_ptr<server::IOBlockController> block,  //!< [in]
            JobInfo& job_info,                        //!< [in]
            mmcs_client::CommandReply& reply                         //!< [in]
            );
};


} } // namespace mmcs::lite


#endif
