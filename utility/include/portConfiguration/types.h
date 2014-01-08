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
/*!
 * \file utility/include/portConfiguration/types.h
 */


#ifndef BGQ_UTILITY_PC_TYPES_H_
#define BGQ_UTILITY_PC_TYPES_H_


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/shared_ptr.hpp>

#include <iosfwd>


namespace bgq {
namespace utility {
namespace portConfig {


/*! \brief The socket type. */
typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> Socket;

/*! \brief A shared pointer to the socket, because Sockets can't be copied. */
typedef boost::shared_ptr<Socket> SocketPtr;


/*! \brief The type of user that's connected. */
struct UserType {
    enum Value {
        Administrator, //!< The administrative certificate was presented.
        Normal,        //!< The command certificate was presented.
        None           //!< No certificate was presented.
    };
};


std::ostream& operator<<( std::ostream& os, UserType::Value user_type );


//! \brief Indicator to expect the UserId first or skip it.
struct UserIdHandling {
    enum Value {
        Process, //!< Either send the UserId or require it.
        Skip     //!< Do not send or accept the UserId.
    };
};


} } } // namespace bgq::utility::portConfig

#endif
