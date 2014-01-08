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


#ifndef BG_REALTIME_SERVER_H_
#define BG_REALTIME_SERVER_H_ 1


#include "Configuration.h"
#include "Status.h"

#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/Acceptor.h>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include <string>


/*! \brief Real-time stuff.
 */
namespace realtime {

/*! \brief Real-time %server
 */
namespace server {


/*! \brief Represents an instance of the real-time %server.
 *
 * Call the setters and then call run. Catch exceptions.
 */
class BgRealtimeServer
{
public:

    BgRealtimeServer(
            const std::string& application_name
        );

    void setConfigProperties( bgq::utility::Properties::Ptr properties_ptr )  { _properties_ptr = properties_ptr; }

    void run(
            bgq::utility::ServerPortConfiguration& port_config,
            bgq::utility::ServerPortConfiguration& command_port_config
        );


private:

    std::string _application_name;

    bgq::utility::Properties::Ptr _properties_ptr;
    boost::shared_ptr<Configuration> _config_ptr;

    boost::asio::io_service _io_service;

    Status _status;


    void _runIoService();

    void _handleAcceptCommandClient(
            const bgq::utility::Acceptor::AcceptArguments& args
        );
};


} } // namespace realtime::server


#endif
