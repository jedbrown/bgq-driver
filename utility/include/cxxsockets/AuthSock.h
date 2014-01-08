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
#ifndef _AUTHSOCK_H
#define _AUTHSOCK_H

#include "SocketTypes.h"
#include <utility/include/security/Authorization.h>
#include <utility/include/UserId.h>

namespace CxxSockets {

    //! \brief Thin wrapper for the authorization class so it 
    //  works with cxxsockets
class Authorizer : public bgq::utility::security::Authorization {
    CxxSockets::SocketPtr _sock;
public:
    Authorizer(CxxSockets::SocketPtr& sock) : 
        bgq::utility::security::Authorization(sock->_fileDescriptor) {
        _sock = sock;
    }

    int send() { 
        return bgq::utility::security::Authorization::send();
    }

    int receive() {
        return bgq::utility::security::Authorization::receive();
    }
    
    int write(void* buf, size_t size, int error) {
        return bgq::utility::security::Authorization::write(buf,size,error);
    }
};

};

#endif
