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


#ifndef REALTIME_SERVER_CONFIGURATION_H
#define REALTIME_SERVER_CONFIGURATION_H


#include <utility/include/Properties.h>

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>


namespace realtime {
namespace server {


class Configuration
{
public:

    typedef boost::shared_ptr<std::string> StringPtr;

    typedef boost::shared_ptr<const std::string> ConstStringPtr;

    static const std::string PROPERTIES_SECTION_NAME;

    static const unsigned DEFAULT_MAXIMUM_TRANSACTION_SIZE;

    Configuration(
            const bgq::utility::Properties& properties
        );

    unsigned get_workers() const { return _workers; }

    const std::string& get_database_name() const { return _database_name; }
    ConstStringPtr get_database_user() const { return _database_user; }
    ConstStringPtr get_database_schema_name() const { return _database_schema_name; }

    unsigned get_max_xact_size() const  { return _maximum_transaction_size; }


private:

    unsigned _workers;

    std::string _database_name;
    StringPtr _database_user;
    StringPtr _database_schema_name;

    unsigned _maximum_transaction_size;
};


} } // namespace realtime::server

#endif
