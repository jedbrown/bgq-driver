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

#ifndef _MMCSCONSOLEPORT_H
#define _MMCSCONSOLEPORT_H

#include <utility/include/cxxsockets/SocketTypes.h>

#include <boost/utility.hpp>

#include <string>
#include <stdexcept>

/*!
 * \brief An interface between the mmcs server and the mmcs console.
 */
class MMCSConsolePort : private boost::noncopyable
{
    int procMessage(std::string& m, char* buf, bool& done, bool nonblock, int rcvsz, unsigned timeout = 0);
public:
    /*!
     * \brief exception class.
     */
    class Error: public std::runtime_error {
    public:
        int errcode;
        Error(int err=0, const char* what="") : std::runtime_error(what), errcode(err) {}
    };

    virtual ~MMCSConsolePort() { }
    void sendMessage(std::string&);
    int pollReceiveMessage(std::string&, unsigned timeout=0);
    int receiveMessage(std::string&, unsigned timeout=0);
    int checkConnection();
    //    void Write(const char* buf, unsigned len);
    void Write(CxxSockets::Message& msg, unsigned len);
    CxxSockets::SocketPtr getSock() const { return _sock; }

protected:
    /*!
     * \brief ctor
     * \param[in] fd
     */
    MMCSConsolePort() {};
    MMCSConsolePort(CxxSockets::SocketPtr& sock);
    CxxSockets::SocketPtr _sock;
};

/*!
 * \brief Client-side console port.
 */
class MMCSConsolePortClient : public MMCSConsolePort
{
public:
    MMCSConsolePortClient(bgq::utility::PortConfiguration::Pairs& portpairs);
    MMCSConsolePortClient(CxxSockets::SocketPtr sock) : MMCSConsolePort(sock) { }
    ~MMCSConsolePortClient();
};

/*!
 * \brief Server-side console port.
 */
class MMCSConsolePortServer : public MMCSConsolePort
{
    CxxSockets::PollingListenerSetPtr _listener;
public:
    MMCSConsolePortServer(bgq::utility::PortConfiguration::Pairs& portpairs);
    ~MMCSConsolePortServer();
    MMCSConsolePortClient* accept();
};

#endif
