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
#ifndef RUNJOB_SERVER_DATABASE_IO_NODE_IP_ADDRESS_H
#define RUNJOB_SERVER_DATABASE_IO_NODE_IP_ADDRESS_H

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio/ip/address.hpp>

#include <sstream>
#include <string>
#include <map>

class BGQBlockNodeConfig;
class BGQIOBoardNodeConfig;

namespace runjob {
namespace server {
namespace database {

/*!
 * \brief Query the BGQNetconfig table for I/O node IP addresses.
 */
class IoNodeIpAddress
{
public:
    /*!
     * \brief ctor.
     */
    IoNodeIpAddress(
            const BGQBlockNodeConfig& block
            );

    /*!
     * \brief Get an IP address for a specific location.
     */
    const boost::asio::ip::address& get(
            const std::string& location
            ) const;

private:
    void execute();

private:
    typedef std::map<std::string, boost::asio::ip::address> IpMap;

private:
    const BGQBlockNodeConfig& _block;
    const cxxdb::ConnectionPtr _connection;
    cxxdb::QueryStatementPtr _statement;
    std::ostringstream _sql;
    IpMap _result;
};

} // database
} // server
} // runjob

#endif
