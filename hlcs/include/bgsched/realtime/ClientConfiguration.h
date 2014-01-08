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

#ifndef BGSCHED_REALTIME_CLIENT_CONFIGURATION_H_
#define BGSCHED_REALTIME_CLIENT_CONFIGURATION_H_

/*!
 *  \file
 *
 *  \brief Class ClientConfiguration
 */

#include <string>
#include <vector>

namespace bgsched {
namespace realtime {

/*!
 *  \brief Real-time client configuration options.
 *
 *  If an application needs to configure the real-time client, it can override the methods in this
 *  class and pass the object to the Client constructor.
 */
class ClientConfiguration
{
public:

    /*!
     *  \brief Type for return value from getHost
     */
    typedef std::vector<std::string> Hosts;


    /*!
     *  \brief The default value for the host.
     */
    static const Hosts DEFAULT_HOST;


    /*!
     *  \brief Get the default client configuration.
     */
    static const ClientConfiguration& getDefault();


    /*!
     *  \brief Get the host to connect to.
     *
     *  The return value is a vector of strings,
     *  where each string is a comma-separated list of host:port pairs.
     *
     *  host is either a host name or IP address.
     *
     *  port is either a port number or service name.
     *
     *  host is optional, if it's not specified then 'localhost' will be used.
     *
     *  port is optional, if it's not specified then the default port, 32061, will be used.
     *
     *  The client will attempt to connect to all the endpoints derived from
     *  resolving each host:port pair.
     *
     *  If this returns an empty vector then the default will be used, which is to connect to localhost::32061.
     *
     *  \return Hosts, or DEFAULT_HOST to use the default.
     */
    virtual const Hosts& getHost() const =0;

    /*!
     *  \brief Destructor.
     */
    virtual ~ClientConfiguration();
};


} // namespace bgsched::realtime
} // namespace bgsched

#endif
