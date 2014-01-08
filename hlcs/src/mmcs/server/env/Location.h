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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_ENV_LOCATION_H
#define MMCS_ENV_LOCATION_H

#include "Polling.h"

#include <boost/asio/io_service.hpp>

#include <string>

namespace mmcs {
namespace server {
namespace env {

class Location : public Polling
{
public:
    Location(
            boost::asio::io_service& io_service,
            const std::string& location,
            const std::string& type,
            unsigned seconds
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server  //!< [in]
            );

    void stop();

    void done();

    const std::string& getLocation() const { return _location; }
    const std::string& getType() const { return _type; }
    std::string getDescription() const { return _location; }

    unsigned getInterval() const { return _seconds; }

private:
    void makeTargetSetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server
            );

    void openTargetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server
            );

    void readHandler(
            std::istream& response,
            const int handle,
            const boost::shared_ptr<McServerConnection>& mc_server
            );

private:
    const std::string _location;
    const std::string _type;
};

} } } // namespace mmcs::server::env

#endif
