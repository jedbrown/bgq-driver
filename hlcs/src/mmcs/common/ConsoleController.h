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

#ifndef MMCS_COMMON_CONSOLE_CONTROLLER_H_
#define MMCS_COMMON_CONSOLE_CONTROLLER_H_

#include "fwd.h"

#include "libmmcs_client/ConsolePort.h"

#include <utility/include/UserId.h>

#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/types.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <stack>
#include <string>

#include <stdio.h>

namespace mmcs {

class MMCSCommandProcessor;

namespace common {

class ConsoleController : private boost::noncopyable
{
public:
    /*!
     * \brief ctor.
     */
    ConsoleController(
            MMCSCommandProcessor* mmcsCommandProcessor,         //!< [in]
            const bgq::utility::UserId& user,                   //!< [in]
            CxxSockets::UserType utype = CxxSockets::Normal     //!< [in]
            );

    /*!
     * \brief dtor.
     */
    virtual ~ConsoleController();

    //
    // Controlled block controller
public:
    const server::BlockHelperPtr& getBlockHelper() const { return _blockController; }
protected:
    void setBlockController(const server::BlockHelperPtr& blockController) { _blockController = blockController; }
    server::BlockHelperPtr _blockController;  // nodes controlled by this console

    //
    // Command execution
    //
public:
    void     run();
    int      quit() const { return _quit; }
    void     quit(int status) { _quit = status; }
    MMCSCommandProcessor*  getCommandProcessor() const { return _commandProcessor; }
protected:
    int     _quit;  // set by 'quit' command
    MMCSCommandProcessor*  _commandProcessor; // list of MMCS commands that can be executed
private:
    void     serviceCommands();
    std::string getHistoryFile() const;
    void     writeHistoryFile() const;

    //
    // Command input sources
    //
public:
    bool             pushInput(FILE* f);
    mmcs_client::ConsolePort*  getConsolePort() const { return _consolePort; }
    void setConsolePort(mmcs_client::ConsolePort* consolePort) { _consolePort = consolePort; }
protected:
    std::stack<FILE *>    _inputs;      // command input file(s) currently being processed
    mmcs_client::ConsolePort*  _consolePort;     // socket connection to client (MMCSServerSideConnection) or server (MMCSClientSideServerPort)

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
    bool getRedirecting() const { return _redirecting; }
protected:
    bool   _redirecting; // this DBBlockController is being used for mailbox redirection

public:
    void setPortConfiguration( boost::shared_ptr<const bgq::utility::ClientPortConfiguration> portConfig_ptr )  { _portConfig_ptr = portConfig_ptr; }
    const boost::shared_ptr<const bgq::utility::ClientPortConfiguration>& getPortConfiguration() const  { return _portConfig_ptr; }

private:
    boost::shared_ptr<const bgq::utility::ClientPortConfiguration> _portConfig_ptr;
};

} } // namespace mmcs::common

#endif
