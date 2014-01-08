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


#ifndef MMCS_LITE_COMMAND_DUMP_BLOCK_H_
#define MMCS_LITE_COMMAND_DUMP_BLOCK_H_


#include "common/AbstractCommand.h"


namespace mmcs {
namespace lite {
namespace command {


/*!
** dump_block
** dump_block
** Print the BGQBlock configuration object
*/
class DumpBlock: public common::AbstractCommand
{
public:
    DumpBlock(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "dump_block";}
    static  DumpBlock* build();    // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};


} } } // namespace mmcs::lite::command

#endif
