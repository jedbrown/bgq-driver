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
 * \file utility/include/cxxsockets/SockAddrList.h
 */

#ifndef CXXSOCKET_SOCK_ADDR_LIST_H
#define CXXSOCKET_SOCK_ADDR_LIST_H

#include <utility/include/cxxsockets/SockAddr.h>

#include <deque>
#include <string>
#include <vector>

namespace CxxSockets {

//! \brief List of SockAddrs
class SockAddrList
{
public:
    typedef std::deque<SockAddr>::iterator iterator;
    typedef std::deque<SockAddr>::const_iterator const_iterator;

    //! \brief Default constructor
    SockAddrList() {}

    //! \brief Constructor
    SockAddrList(
            const unsigned short family,    //!< [in] Address family e.g. AF_INET, AF_INET6, AF_UNSPEC
            const std::string& nodename,    //!< [in] hostname or IP address string
            const std::string& service      //!< [in] port or service name from /etc/services
            );

    //! forwarded operations
    size_t size() const { return _sockque.size(); }
    iterator begin() { return _sockque.begin(); }
    iterator end() { return _sockque.end(); }
    const_iterator begin() const { return _sockque.begin(); }
    const_iterator end() const { return _sockque.end(); }
    void push_back(const SockAddr& sa) { _sockque.push_back(sa); }

private:
    //! \brief Double-ended-queue of SockAddrs
    std::deque<SockAddr> _sockque;
};

}

#endif

