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
#ifndef HLCS_SECURITY_FIXTURE_H
#define HLCS_SECURITY_FIXTURE_H

#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/scoped_ptr.hpp>

struct InitializeLoggingFixture
{
    InitializeLoggingFixture()
    {
        const bgq::utility::Properties::ConstPtr properties(
                bgq::utility::Properties::create()
                );

        bgq::utility::initializeLogging( *properties );
        BGQDB::init( properties, 1 );
        std::cout << "initialized with " << properties->getFilename() << std::endl;
    }
};

#endif
