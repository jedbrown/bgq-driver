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

#ifndef MMCS_CLIENT_CONSOLE_PORT_H_
#define MMCS_CLIENT_CONSOLE_PORT_H_

#include <utility/include/cxxsockets/types.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <boost/utility.hpp>

#include <stdexcept>
#include <string>

namespace mmcs_client {

/*!
 * \brief An interface between the mmcs server and the mmcs console.
 */
class ConsolePort : private boost::noncopyable
{
private:
    int procMessage(
            std::string& m,
            char* buf,
            bool& done,
            const int rcvsz
            );

public:
    /*!
     * \brief exception class.
     */
    class Error: public std::runtime_error 
    {
    public:
        const int errcode;
        Error(int err=0, const char* what="") : std::runtime_error(what), errcode(err) {}
    };

    virtual ~ConsolePort() = 0;

    void sendMessage(
            std::string&
            ) const;

    int pollReceiveMessage(
            std::string&,
            const unsigned timeout=0
            );

    int receiveMessage(
            std::string&
            );

    void checkConnection() const;

    void write(
            const CxxSockets::Message& msg
            ) const;

    void setSock(const CxxSockets::TCPSocketPtr& sock) { _sock = sock; }
    CxxSockets::TCPSocketPtr getSock() const { return _sock; }

protected:
    /*!
     * \brief ctor
     */
    ConsolePort() {};
    ConsolePort(const CxxSockets::TCPSocketPtr& sock);
    CxxSockets::TCPSocketPtr _sock;
};

/*!
 * \brief Client-side console port.
 */
class ConsolePortClient : public ConsolePort
{
public:
    ConsolePortClient(
            const bgq::utility::ClientPortConfiguration& port_config
            );

    ConsolePortClient(
            const CxxSockets::TCPSocketPtr& sock
            );
};

/*!
 * \brief Server-side console port.
 */
class ConsolePortServer : public ConsolePort
{
private:
    CxxSockets::ListenerSetPtr _listener;

public:
    ConsolePortServer(
            const bgq::utility::PortConfiguration::Pairs& portpairs
            );

    ConsolePortClient* accept();
};

} // namespace mmcs_client

#endif
