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

#ifndef MMCS_COMMAND_WAITJOB_H
#define MMCS_COMMAND_WAITJOB_H


#include "common/AbstractCommand.h"


namespace mmcs {
namespace lite {


/*!
 * \brief
 */
class MMCSCommand_waitjob : public common::AbstractCommand
{
public:
    /*!
     * \brief
     */
    MMCSCommand_waitjob(
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
    static MMCSCommand_waitjob* build();

private:
    static int selectJobCallback(void*, int argc, char** argv, char**);
};


} } // namespace mmcs::lite

#endif
