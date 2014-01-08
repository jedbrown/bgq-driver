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

#ifndef MASTER_PROTOCOL_H_
#define MASTER_PROTOCOL_H_


#include <utility/include/cxxsockets/SecureTCPSocket.h>
#include <utility/include/cxxsockets/SockAddr.h>
#include <utility/include/Properties.h>

#include <xml/include/library/XML.h>

#include <boost/thread/mutex.hpp>

#include <string>


//! \brief Base protocol object for BGMaster.  It encapsulates secure communications
//!        and object serialization/deserialization.
class Protocol
{
public:
    Protocol();

    virtual ~Protocol();

    /*!
     * Connect to the remote host specified
     */
    virtual void initializeRequester(
            const bgq::utility::Properties::ConstPtr& props,
            int ipv, 
            const std::string& host, 
            const std::string& port, 
            unsigned attempts = 1
            );

    /*! 
     * Set a previously connected socket as the request socket.
     */
    virtual void setRequester(
            CxxSockets::SecureTCPSocketPtr sock
            );

    /*!
     * Already have a sock to use for responses.  Master uses this.
     */
    virtual void initializeResponder(
            CxxSockets::SecureTCPSocketPtr sock
            );

    /*!
     * Wait on the responder socket for an incoming request. Return when it arrives.
     */
    void getName(
            std::string& requestName
            );

    void getObject(
            XML::Serializable* requestObject
            );

    /*!
     * Send a reply on the responder socket
     */
    void sendReply(
            const std::string& requestName, 
            const XML::Serializable& replyObject
            );

    CxxSockets::SecureTCPSocketPtr& getRequester() { return _requester; }

    CxxSockets::SecureTCPSocketPtr& getResponder() { return _responder; }

    void sendOnly(
            const std::string& requestName, 
            const XML::Serializable& requestObject
            );

protected:
    //! \brief send our message and receive our reply
    void sendReceive(
            const std::string& requestName, 
            const XML::Serializable& requestObject, 
            const std::string& replyName, 
            XML::Serializable& replyObject
            );

    CxxSockets::SecureTCPSocketPtr _requester;
    CxxSockets::SecureTCPSocketPtr _responder;
    boost::mutex _sr_lock;
};

#endif
