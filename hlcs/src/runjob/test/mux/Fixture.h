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
#ifndef RUNJOB_TEST_SERVER_FIXTURE_H
#define RUNJOB_TEST_SERVER_FIXTURE_H

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

struct Fixture
{
public:
    /*!
     * \brief
     */
    Fixture() :
        _argv(NULL),
        _argc(0)
    {
    
    }

    /*!
     * \brief
     */
    ~Fixture()
    {
        if ( !_argv ) return;

        uint32_t i = 0;
        char* arg = _argv[i];
        while (arg != NULL) {
            delete[] arg;
            arg = _argv[++i];
        }

        delete[] _argv;
    }

    /*!
     * \brief
     */
    char** _argv;
    uint32_t _argc;
};

#endif
