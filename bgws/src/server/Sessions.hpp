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

#ifndef BGWS_SESSIONS_HPP_
#define BGWS_SESSIONS_HPP_


#include "DynamicConfiguration.hpp"
#include "Session.hpp"
#include "types.hpp"
#include "UserInfo.hpp"

#include "utility/utility.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <map>
#include <string>
#include <vector>

#include <stdint.h>


namespace bgws {


class Sessions : boost::noncopyable
{
public:


    struct Result {
        enum Value {
            Ended,
            NotFound,
            NotAuthorized
        };
    };

    typedef std::vector<std::string> SessionInfos;


    Sessions(
            boost::asio::io_service& io_service,
            DynamicConfiguration::ConstPtr dynamic_configuration_ptr
        );

    void setNewDynamicConfiguration( DynamicConfiguration::ConstPtr dynamic_configuration_ptr )  { _dynamic_configuration_ptr = dynamic_configuration_ptr; }

    void createSession(
            const std::string& username,
            UserInfo::UserType user_type,
            SessionPtr* session_ptr_out
        );

    bool checkSession(
            const std::string& session_id,
            SessionPtr* session_ptr_out
        );

    void end(
            const std::string& session_id,
            const UserInfo& user_info,
            Result::Value* result_out
        );

    SessionInfos getSessionInfos(
            const UserInfo& user_info
        );


    void sessionTimeoutHandler(
            const boost::system::error_code&,
            const std::string& session_id
        );


    uint32_t getTimeoutSecs() const  { return _dynamic_configuration_ptr->getSessionTimeoutSeconds(); }


private:

    typedef std::map<std::string,SessionPtr> _IdToSession;


    static std::string _generateSessionId();
    static std::string _genSessionIdFromBytes( const utility::Bytes& bytes );


    boost::asio::io_service &_io_service;
    DynamicConfiguration::ConstPtr _dynamic_configuration_ptr;

    boost::mutex _mtx;

    _IdToSession _id_to_session;

};


} // namespace bgws {


#endif
