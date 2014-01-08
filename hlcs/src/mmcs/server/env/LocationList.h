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


#ifndef MMCS_SERVER_ENV_LOCATION_LIST_H
#define MMCS_SERVER_ENV_LOCATION_LIST_H

#include "libmmcs_client/CommandReply.h"

#include <utility/include/Singleton.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

#include <string>
#include <vector>


namespace mmcs {
namespace server {
namespace env {


class Location;


class LocationList : public bgq::utility::Singleton<LocationList>
{
public:
    bool start(
            const std::string& type,
            const std::string& location,
            const unsigned seconds
            );

    bool stop(
            const std::string& location
            );

    void list(
            mmcs_client::CommandReply& reply
            );

private:
    friend class Location;
    void remove(
            const std::string& location
            );

private:
    typedef std::vector< boost::shared_ptr<Location> > Locations;

private:
    boost::mutex _mutex;
    Locations _locations;
};

} } } // namespace mmcs::server::env

#endif
