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

#include <utility/include/cxxsockets/SocketTypes.h>
#include <utility/include/portConfiguration/PortConfiguration.h>
#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include "MMCSProperties.h"

static log4cxx::LoggerPtr connect_logger(log4cxx::Logger::getLogger( "ibm.mmcs.SecureConnect" ));

namespace SecureConnect {
    class SecureConnectException : public std::runtime_error {
    public:
        int errcode;
        SecureConnectException(int err = 0, const std::string& what = "") : std::runtime_error(what), errcode(err) {}
    };

    void Connect(bgq::utility::PortConfiguration::Pairs& portpairs,
                                CxxSockets::SecureTCPSocketPtr& sockp) {
        std::string connected_host = "";
        std::string connected_port = "";
        bool failed = true;
        std::ostringstream portstrings;

        BOOST_FOREACH(bgq::utility::PortConfiguration::Pair portpair, portpairs) {
            connected_host = portpair.first;
            connected_port = portpair.second;
            unsigned int attempts = 0;

            try {
                CxxSockets::SockAddrList remote_list(AF_UNSPEC, connected_host, connected_port);
                BOOST_FOREACH(CxxSockets::SockAddr& remote, remote_list) {
                    CxxSockets::SecureTCPSocketPtr
                        sock(new CxxSockets::SecureTCPSocket(remote.family(), 0,
                                                             CxxSockets::SECURE,
                                                             CxxSockets::CRYPTALL));

                    try {
                        ++attempts;
                        bgq::utility::ClientPortConfiguration port_config(0, bgq::utility::ClientPortConfiguration::ConnectionType::Command);
                        port_config.setProperties(MMCSProperties::getProperties(), "");
                        port_config.notifyComplete();
                        sock->Connect(remote, port_config);
                        sockp = sock;
                        failed = false;
                        if(attempts > 1) // Let 'em know the nth try succeeded
                            LOG4CXX_INFO(connect_logger, "Connected to server successfully.");
                        break;
                    } catch(CxxSockets::CxxError& e) {
                        if(e.errcode == CxxSockets::SSL_ERROR) {
                            throw SecureConnectException(e.errcode, e.what());
                        }
                        LOG4CXX_INFO(connect_logger, "Connect attempt failed to " << connected_host
                                     << ":" << connected_port << " " << e.what());
                        if(attempts < (remote_list.size() * portpairs.size())) {
                            LOG4CXX_INFO(connect_logger, "Will try other port pairs.");
                        }
                        else throw SecureConnectException(e.errcode, e.what());
                        continue;
                    }
                }
            } catch (CxxSockets::CxxError& e) {
                throw SecureConnectException(e.errcode, e.what());
            }
        }
    }
};
