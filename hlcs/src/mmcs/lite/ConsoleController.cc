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


#include "ConsoleController.h"

#include "BlockController.h"

#include "server/CNBlockController.h"
#include "server/IOBlockController.h"


using namespace std;

using namespace mmcs::server;


namespace mmcs {
namespace lite {


ConsoleController::ConsoleController(
        MMCSCommandProcessor* commandProcessor,
        BGQMachineXML* machine,
        const bgq::utility::UserId& user
        ) :
    common::ConsoleController(commandProcessor, user),
    _machine(machine),
    _block_initialized(false),
    _job()
{
    // Need to create a dummy block controller to "select" it and keep the command processor
    // happy.
    BlockPtr p(new BlockControllerBase(_machine, _user.getUser(), "dummy", false));
    BlockHelperPtr helper(new BlockHelper(p));
    ConsoleController::setBlockController(helper);
}

void
ConsoleController::genIOBlockController(
        deque<string> args,
        std::string& blockName,
        std::istream& xml,
        mmcs_client::CommandReply& reply
        )
{
    // create an io block pointer
    IOBlockPtr iop(new IOBlockController(_machine, _user.getUser(), blockName, false));
    typedef boost::shared_ptr<BlockController> LiteBlockPtr;
    LiteBlockPtr lite_p(new BlockController(iop));
    lite_p->getBase()->create_block(args, reply, &xml);
    _blockController = lite_p;

    // ensure we created a block
    if ( reply.getStatus() == 0 ) {
        _block_initialized = true;
    }
}

void
ConsoleController::genCNBlockController(
        deque<string> args,
        std::string& blockName,
        std::istream& xml,
        mmcs_client::CommandReply& reply
        )
{
    // create a cn block pointer
    CNBlockPtr cnp(new CNBlockController(_machine, _user.getUser(), blockName, false));
    typedef boost::shared_ptr<BlockController> LiteBlockPtr;
    LiteBlockPtr lite_p(new BlockController(cnp));
    lite_p->getBase()->create_block(args, reply, &xml);
    _blockController = lite_p;
    //_blockController->getBase()->create_block(args, reply, &xml);
    // ensure we created a block
    if ( reply.getStatus() == 0 ) {
        _block_initialized = true;
    }
}

} } // namespace mmcs::lite
