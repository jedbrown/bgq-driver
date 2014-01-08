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
#ifndef RUNJOB_TEST_CLIENT_FIXTURE_H
#define RUNJOB_TEST_CLIENT_FIXTURE_H

#include "common/JobInfo.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

extern char** environ;

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
        // initialize logging
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );

        // unset all RUNJOB_* envs
        unsigned i = 0;
        std::string env( environ[i] );
        while ( !env.empty() ) {
            if ( !env.compare(0, 7, "RUNJOB_") ) {
                // get name
                std::string::size_type equals = env.find_first_of('=');
                std::string name = env.substr(0, equals);
                
                // unset it
                unsetenv( name.c_str() );
            }

            // get next env
            env.clear();
            if ( environ[++i] ) {
                env = environ[i];
            }
        }
    }

    /*!
     * \brief
     */
    ~Fixture()
    {
        if (_argv) {
            uint32_t i = 0;
            char* arg = _argv[i];
            while (arg != NULL) {
                delete[] arg;
                arg = _argv[++i];
            }

            delete[] _argv;
        }
    }

    char** _argv;
    uint32_t _argc;
};

#endif
