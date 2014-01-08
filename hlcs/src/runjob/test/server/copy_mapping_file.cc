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
#define BOOST_TEST_MODULE copy_mapping_file
#include <boost/test/unit_test.hpp>

#include "common/Exception.h"
#include "common/JobInfo.h"
#include "common/Mapping.h"

#include "server/job/CopyMappingFile.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <fstream>

using namespace runjob::server;

struct InitializeLoggingFixture {
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};

BOOST_GLOBAL_FIXTURE( InitializeLoggingFixture );
    
const std::string propertiesFileName( "mapping.properties" );
const std::string mappingFileName( "copy_mapping_file.mapping" );

BOOST_AUTO_TEST_CASE( disabled )
{
    // with no mapping archive path configured, the file should not be copied
    std::ofstream properties( propertiesFileName, std::ios_base::trunc );
    properties << "[runjob.server]" << std::endl;
    properties.flush();
                            
    std::ofstream mapping( mappingFileName, std::ios_base::trunc );
    mapping << "0 0 0 0 0 0\n";
    mapping.flush();

    runjob::JobInfo info;
    info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                mappingFileName
                )
            );

    const job::CopyMappingFile copy(0, info, bgq::utility::Properties::create(propertiesFileName) );
    BOOST_CHECK_EQUAL( copy.result(), "" );
}

BOOST_AUTO_TEST_CASE( enabled_and_writable )
{
    // configure path to write to our working dir, this should always work
    char cwd[PATH_MAX];
    (void)getcwd( cwd, PATH_MAX );

    std::ofstream properties( propertiesFileName, std::ios_base::trunc );
    properties << "[runjob.server]" << std::endl;
    properties << job::CopyMappingFile::KeyName << "=" << cwd << std::endl;
    properties.flush();
                            
    std::ofstream mapping( mappingFileName, std::ios_base::trunc );
    mapping << "0 0 0 0 0 0\n";
    mapping.flush();

    runjob::JobInfo info;
    info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                mappingFileName
                )
            );

    const job::CopyMappingFile copy(0, info, bgq::utility::Properties::create(propertiesFileName) );
    BOOST_CHECK_EQUAL(
            copy.result(),
            std::string() + cwd + "/0"
            );
}

BOOST_AUTO_TEST_CASE( enabled_and_not_writable )
{
    std::ofstream properties( propertiesFileName, std::ios_base::trunc );
    properties << "[runjob.server]" << std::endl;
    properties << job::CopyMappingFile::KeyName << "=" << "/dev/null" << std::endl;
    properties.flush();
                            
    std::ofstream mapping( mappingFileName, std::ios_base::trunc );
    mapping << "0 0 0 0 0 0\n";
    mapping.flush();

    runjob::JobInfo info;
    info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                mappingFileName
                )
            );

    BOOST_CHECK_THROW(
            job::CopyMappingFile(0, info, bgq::utility::Properties::create(propertiesFileName) ),
            runjob::Exception
            );
}

BOOST_AUTO_TEST_CASE( enabled_with_big_mapping )
{
    // configure path to write to our working dir, this should always work
    char cwd[PATH_MAX];
    (void)getcwd( cwd, PATH_MAX );

    std::ofstream properties( propertiesFileName, std::ios_base::trunc );
    properties << "[runjob.server]" << std::endl;
    properties << job::CopyMappingFile::KeyName << "=" << cwd << std::endl;
    properties.flush();
                
    // generate an ABCDET mapping for 512 nodes with 16 ranks per node
    size_t count = 0;
    std::vector<std::string> mappings( 512 * 16 );
    std::ofstream mapping( mappingFileName, std::ios_base::trunc );
    for ( unsigned a = 0; a < 4; ++a ) {
        for ( unsigned b = 0; b < 4; ++b ) {
            for ( unsigned c = 0; c < 4; ++c ) {
                for ( unsigned d = 0; d < 4; ++d ) {
                    for ( unsigned e = 0; e < 2; ++e ) {
                        for ( unsigned t = 0; t < 16; ++t ) {
                            mappings[count].append( boost::lexical_cast<std::string>(a) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(b) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(c) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(d) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(e) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(t) );
                            mapping << mappings[count] << std::endl;
                            ++count;
                        }
                    }
                }
            }
        }
    }
    mapping.flush();

    runjob::JobInfo info;
    info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                mappingFileName
                )
            );

    const job::CopyMappingFile copy(0, info, bgq::utility::Properties::create(propertiesFileName) );
    BOOST_CHECK_EQUAL(
            copy.result(),
            std::string() + cwd + "/0"
            );

    // open the copied file for reading and ensure it equals the original mapping
    std::ifstream input( "0" );
    count = 0;
    BOOST_CHECK( input );
    while ( input ) {
        std::string line;
        std::getline( input, line );
        if ( line.empty() ) continue;
        if ( line.at(0) == '#' ) continue;
        BOOST_CHECK_EQUAL( line, mappings.at(count) );
        ++count;
    }
    BOOST_CHECK_EQUAL( count, mappings.size() );
}

BOOST_AUTO_TEST_CASE( enabled_with_big_permuted_mapping )
{
    // configure path to write to our working dir, this should always work
    char cwd[PATH_MAX];
    (void)getcwd( cwd, PATH_MAX );

    std::ofstream properties( propertiesFileName, std::ios_base::trunc );
    properties << "[runjob.server]" << std::endl;
    properties << job::CopyMappingFile::KeyName << "=" << cwd << std::endl;
    properties.flush();
                
    // generate an CBADET mapping for 512 nodes with 1 ranks per node
    size_t count = 0;
    std::vector<std::string> mappings( 512 * 1 );
    std::ofstream mapping( mappingFileName, std::ios_base::trunc );
    for ( unsigned c = 0; c < 4; ++c ) {
        for ( unsigned b = 0; b < 4; ++b ) {
            for ( unsigned a = 4; a > 0; --a ) {
                for ( unsigned d = 0; d < 4; ++d ) {
                    for ( unsigned e = 0; e < 2; ++e ) {
                        for ( unsigned t = 0; t < 1; ++t ) {
                            mappings[count].append( boost::lexical_cast<std::string>(a) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(b) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(c) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(d) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(e) ).append( " " );
                            mappings[count].append( boost::lexical_cast<std::string>(t) );
                            mapping << mappings[count] << std::endl;
                            ++count;
                        }
                    }
                }
            }
        }
    }
    mapping.flush();

    runjob::JobInfo info;
    info.setMapping(
            runjob::Mapping(
                runjob::Mapping::Type::File,
                mappingFileName
                )
            );

    const job::CopyMappingFile copy(1, info, bgq::utility::Properties::create(propertiesFileName) );
    BOOST_CHECK_EQUAL(
            copy.result(),
            std::string() + cwd + "/1"
            );

    // open the copied file for reading and ensure it equals the original mapping
    std::ifstream input( "1" );
    std::string line;
    if ( !input ) {
        std::cerr << "could not open input: " << strerror(errno) << std::endl;
    }

    count = 0;
    while ( std::getline(input, line) ) {
        if ( line.empty() ) continue;
        if ( line.at(0) == '#' ) continue;
        BOOST_CHECK_EQUAL( line, mappings.at(count) );
        ++count;
    }
    BOOST_CHECK_EQUAL( count, mappings.size() );
}
