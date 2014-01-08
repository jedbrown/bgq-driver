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
#ifndef BGQ_UTILITY_TEST_PROPERTIES_FIXTURE_H
#define BGQ_UTILITY_TEST_PROPERTIES_FIXTURE_H

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/scoped_array.hpp>

#include <cerrno>
#include <iostream>
#include <fstream>
#include <string>

struct Fixture
{
public:
    /*!
     * \brief
     */
    Fixture() :
        _filename(),
        _file(),
        _fd(0)
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );

        // create temporary file
        const char* file_template = "properties.XXXXXX";
        boost::scoped_array<char> filename( new char[strlen(file_template) + 1] );
        strncpy(filename.get(), file_template, strlen(file_template));
        filename[ strlen(file_template) ] = '\0';
        _fd = mkstemp( filename.get() );
        if (_fd == -1) {
            BOOST_FAIL("could not create temporary properties file " << filename.get() << ": " << strerror(errno));
        } else {
            _filename = filename.get();
            BOOST_TEST_MESSAGE("using temporary properties file " << _filename << " fd " << _fd);
        }

        // open temporary file
        _file.open(_filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
        if ( !_file ) {
            BOOST_FAIL("could not open file " << _filename);
        }
    }

    /*!
     * \brief
     */
    ~Fixture()
    {
        BOOST_TEST_MESSAGE("tearing down fixture");
        ::close(_fd);
        (void)unlink( _filename.c_str() );
    }

    /*!
     * \brief
     */
    std::string _filename;
    std::fstream _file;
    int _fd;
};

#endif
