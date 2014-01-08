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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef CONSOLE_CONTROLLER_H_
#define CONSOLE_CONTROLLER_H_

#include "BlockControllerBase.h"
#include "BlockHelper.h"

#include <bgq_util/include/pthreadmutex.h>

#include <utility/include/UserId.h>

#include <boost/utility.hpp>

#include <map>
#include <stack>

class BlockController;
class MMCSCommandProcessor;
class MMCSConsolePort;

class ConsoleController : private boost::noncopyable
{
public:
    /*!
     * \brief ctor.
     */
    static volatile int signal_number;

    ConsoleController(
            MMCSCommandProcessor* mmcsCommandProcessor,         //!< [in]
            const bgq::utility::UserId& user,                   //!< [in]
            CxxSockets::UserType utype = CxxSockets::Normal,
            BlockHelperPtr blockController = BlockHelperPtr()   //!< [in]
            );

    /*!
     * \brief dtor.
     */
    virtual ~ConsoleController();

    //
    // Controlled block controller
public:
    BlockHelperPtr& getBlockHelper() { return _blockController; }
    BlockPtr& getBlockBaseController() { return _blockController->getBase(); }
protected:
    void setBlockController(BlockHelperPtr& blockController) { _blockController = blockController; }
    BlockHelperPtr _blockController;  // nodes controlled by this console

    //
    // Command execution
    //
public:
    void     run();
    int      quit() { return _quit; }
    void     quit(int status) { _quit = status; }
    MMCSCommandProcessor*  getCommandProcessor() { return _commandProcessor; }
protected:
    int     _quit;		        // set by 'quit' command
    MMCSCommandProcessor*  _commandProcessor;	// list of MMCS commands that can be executed
private:
    void     serviceCommands();
    std::string getHistoryFile() const;
    void     writeHistoryFile() const;

    //
    // Command input sources
    //
public:
    bool             pushInput(FILE* f);
    MMCSConsolePort*  getConsolePort() { return _consolePort; }
    void setConsolePort(MMCSConsolePort* consolePort) { _consolePort = consolePort; }
protected:
    std::stack<FILE *>    _inputs;      // command input file(s) currently being processed
    MMCSConsolePort*  _consolePort;     // socket connection to client (MMCSServerSideConnection) or server (MMCSClientSideServerPort)

public:
    const bgq::utility::UserId& getUser() const;
    const CxxSockets::UserType& getUserType() const { return _utype; }

protected:
    const bgq::utility::UserId _user;   // current user
    const CxxSockets::UserType _utype;
public:
    //
    // Output redirection
    //
    void setRedirecting(bool onoff) { _redirecting = onoff; }
    bool getRedirecting() { return _redirecting; }
    void setFd(int fd) { _fd = fd; }
    const int getFd() const { return _fd; }
protected:
    bool   _redirecting;	// this DBBlockController is being used for mailbox redirection
    int    _fd;

    //
    // Reply format
    //
public:
    unsigned getReplyFormat() { return _replyFormat; }
    void     setReplyFormat(unsigned replyFormat) { _replyFormat = replyFormat; }
protected:
    unsigned         _replyFormat;      // command reply format

protected:
    mutable PthreadMutex  _midplaneControllerMutex; // to serialize access to fields in ConsoleController
};
#endif
