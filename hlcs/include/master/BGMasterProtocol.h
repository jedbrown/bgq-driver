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

#ifndef BGMASTERPROTOCOL_H
#define BGMASTERPROTOCOL_H

#include <string>
#include "hlcs/include/c_api/BGMasterAgentProtocolSpec.h"
#include "hlcs/include/c_api/BGMasterClientProtocolSpec.h"

#ifndef _SOCKETTYPES_H
#include "SocketTypes.h"
#endif

class BGMasterProtocol;

typedef boost::shared_ptr<BGMasterProtocol> BGMasterProtocolPtr;

//! \brief Base protocol object for BGMaster.  It encapsulates secure communications
//!        and object serialization/deserialization.
class BGMasterProtocol {
public:
    BGMasterProtocol(bgq::utility::Properties::Ptr p) : _props(p) {}
    virtual ~BGMasterProtocol();
    /*!
     * Connect to the remote host specified
     */
    virtual void initializeRequester(int ipv, const std::string& host, std::string port, std::string security, int attempts = 1);

    /*! 
     * Set a previously connected socket as the request socket.
     */
    virtual void setRequester(CxxSockets::SecureTCPSocketPtr sock);

    /*!
     * Already have a sock to use for responses.  Master uses this.
     */
    virtual void initializeResponder(CxxSockets::SecureTCPSocketPtr sock);


    /*!
     * Wait on the responder socket for an incoming request. Return when it arrives.
     */
    void getName(std::string& requestName);

    void getObject(XML::Serializable* requestObject);

    /*!
     * Send a reply on the responder socket
     */
    void sendReply(const std::string& requestName, XML::Serializable& replyObject);
    CxxSockets::SecureTCPSocketPtr& getRequester() { return _requester; }
    CxxSockets::SecureTCPSocketPtr& getResponder() { return _responder; }
    void sendOnly(const std::string& requestName, const XML::Serializable& requestObject);
protected:
    //! \brief protected default constructor for inherited classes to initialize
    //    BGMasterProtocol() {}

    //! \brief send our message and receive our reply
    void sendReceive(const std::string& requestName, const XML::Serializable& requestObject, const std::string& replyName, XML::Serializable& replyObject);

    /*!
     * remote host
     */
    CxxSockets::SockAddr _remote;
    CxxSockets::EpollingListenerSetPtr _listener;
    CxxSockets::SecureTCPSocketPtr _requester;
    CxxSockets::SecureTCPSocketPtr _responder;
    bgq::utility::Properties::Ptr _props;
    boost::mutex _sr_lock;
};

#endif
