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


#ifndef MMCS_LITE_COMMAND_CREATE_BLOCK_H_
#define MMCS_LITE_COMMAND_CREATE_BLOCK_H_


#include "common/AbstractCommand.h"


namespace mmcs {
namespace lite {
namespace command {


/*!
//  create_block
//  syntax:
//     create_block <bgqblock.xml> [ options ] - define a block of nodes to be controlled by mmcs
//       options ::= outfile=<filename>        - direct the mailbox output to a file.
//                   tee                       - direct the mailbox out to both the file and stdout
*/
class CreateBlock: public common::AbstractCommand
{
public:
    CreateBlock(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "create_block <bgqblock.xml> [ options ]";}
    static  CreateBlock* build();  // factory method
    void execute(std::deque<std::string> args,
             mmcs_client::CommandReply& reply,
             common::ConsoleController* pController,
             server::BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() == 0) return false; else return true;}
    void help(std::deque<std::string> args,
              mmcs_client::CommandReply& reply);
};


} } } // namespace mmcs::lite::command

#endif
