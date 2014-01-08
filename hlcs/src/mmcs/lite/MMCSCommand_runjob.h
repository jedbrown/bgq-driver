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

#ifndef MMCS_COMMAND_RUNJOB_H
#define MMCS_COMMAND_RUNJOB_H

#include "MMCSCommand.h"

class CNBlockController;
class ConsoleController;
class IOBlockController;

namespace lite {

class JobInfo;

}

/*!
 * \brief
 */
class MMCSCommand_runjob: public MMCSCommand
{
public:
    /*!
     * \brief
     */
    MMCSCommand_runjob(
            const char* name,
            const char* description,
            const MMCSCommandAttributes& attributes
            );

    /*!
     * \brief
     */
    void execute(
            std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );

    /*!
     * \brief
     */
    void help(
            std::deque<std::string> args,
            MMCSCommandReply& reply
            );

    /*!
     * \brief
     */
    bool checkArgs(std::deque<std::string>&) { return true; }

public:
    static  MMCSCommand_runjob* build();
    static  std::string cmdname() { return "runjob"; }

private:
    /*!
     * \brief
     */
    void insert(
            MMCSCommandReply& reply,    //!< [in]
            lite::JobInfo& info         //!< [in]
            );

    /*!
     * \brief
     */
    void getConnectedIoNode(
            boost::shared_ptr<CNBlockController> block,  //!< [in]
            lite::JobInfo& job_info,                        //!< [in]
            MMCSCommandReply& reply                         //!< [in]
            );

    /*!
     * \brief
     */
    void getConnectedIoNode(
            boost::shared_ptr<IOBlockController> block,  //!< [in]
            lite::JobInfo& job_info,                        //!< [in]
            MMCSCommandReply& reply                         //!< [in]
            );
};

#endif
