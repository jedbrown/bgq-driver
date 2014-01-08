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

#ifndef INTIALIZED_FIXTURE
#define INTIALIZED_FIXTURE

#include "Environment.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <string>

struct Fixture
{
    static bgq::utility::Properties::Ptr properties_ptr;
    static std::string db_name;
    static std::string db_user;
    static std::string db_pwd;
    static std::string db_schema;

    Fixture()
    {
        properties_ptr = bgq::utility::Properties::create( std::string() );
        bgq::utility::initializeLogging( *properties_ptr );
        db_name = properties_ptr->getValue( "database", "name" );
        db_user = properties_ptr->getValue( "database", "user" );
        db_pwd = properties_ptr->getValue( "database", "password" );
        db_schema = properties_ptr->getValue( "database", "schema_name" );

        cxxdb::Environment::initializeApplicationEnvironment();
    }

    ~Fixture()
    {
        cxxdb::Environment::resetApplicationEnvironment();
    }
};

bgq::utility::Properties::Ptr Fixture::properties_ptr;
std::string Fixture::db_name;
std::string Fixture::db_user;
std::string Fixture::db_pwd;
std::string Fixture::db_schema;

#endif
