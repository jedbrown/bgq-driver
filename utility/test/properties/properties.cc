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
#define BOOST_TEST_MODULE example
#include <boost/test/unit_test.hpp>
#include "Fixture.h"

#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <unistd.h>

using namespace bgq::utility;
using std::endl;

BOOST_FIXTURE_TEST_SUITE( suite1, Fixture )

BOOST_AUTO_TEST_CASE( file_not_found )
{
    BOOST_CHECK_THROW(
            Properties::create("/tmp/hopefully_i_do_not_exist"),
            Properties::FileError
        );
}

BOOST_AUTO_TEST_CASE( malformed_section )
{
    _file << "[section" << endl;
    BOOST_CHECK_THROW( 
            Properties::create(_filename),
            Properties::MalformedSection
            );
}

BOOST_AUTO_TEST_CASE( get_value )
{
    _file << "[section]" << endl;
    _file << "key=value" << endl;
    _file << " key2=value" << endl;
    _file << "key3 =value" << endl;
    _file << " key4 =value" << endl;
    _file << " key5 = value" << endl;
    _file << "key6= value" << endl;
    _file << "key7= value " << endl;
    _file << "key space=value" << endl;
    _file << "key space2 =value" << endl;
    _file << "key space3 = value" << endl;
    _file << "key comment = value # comment" << endl;
    _file << "value with space = value value" << endl;
    _file << "value with space and comment = value value # comment" << endl;
    Properties prop(_filename);
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key2"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key3"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key4"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key5"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key5"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key6"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key space"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key space2"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key space3"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "key comment"), 
            "value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "value with space"), 
            "value value"
            );
    BOOST_CHECK_EQUAL(
            prop.getValue("section", "value with space and comment"), 
            "value value"
            );
}

BOOST_AUTO_TEST_CASE( get_values )
{
    // create file
    _file << "[section]" << endl;
    _file << "key1=value" << endl;
    _file << "key2=value" << endl;

    // open Properties
    Properties prop(_filename);

    // get Section
    const Properties::Section& section = prop.getValues("section");

    // setup in order result vector
    Properties::Section in_order;
    in_order.push_back( Properties::Pair("key1", "value") );
    in_order.push_back( Properties::Pair("key2", "value") );

    // setup out of order result vector
    Properties::Section out_of_order;
    out_of_order.push_back( Properties::Pair("key2", "value") );
    out_of_order.push_back( Properties::Pair("key1", "value") );

    // setup too large result
    Properties::Section too_large( section.size() + 1, Properties::Pair("key", "value") );

    // compare sizes
    BOOST_CHECK_EQUAL(section.size(), in_order.size());
    BOOST_CHECK(section.size() != too_large.size());

    // compare in order
    BOOST_CHECK(
            std::equal(
                section.begin(),
                section.end(),
                in_order.begin()
                ) 
            );

    // compare out_of_order
    BOOST_CHECK(
            std::equal(
                section.begin(),
                section.end(),
                out_of_order.begin()
                ) 
            != true
            );
}

BOOST_AUTO_TEST_CASE( duplicate_key )
{
    _file << "[section]" << endl;
    _file << "key=value" << endl;
    _file << "key=value" << endl;
    BOOST_CHECK_THROW( Properties::create(_filename), Properties::DuplicateKey);
}

BOOST_AUTO_TEST_CASE( duplicate_section )
{
    _file << "[section]" << endl;
    _file << "key=value" << endl;
    _file << "[section]" << endl;
    _file << "key=value" << endl;
    BOOST_CHECK_THROW( 
            Properties::create(_filename),
            Properties::DuplicateSection
            );
}

BOOST_AUTO_TEST_CASE( missing_section )
{
    _file << "key=value" << endl;
    _file << "[section]" << endl;
    BOOST_CHECK_THROW( 
            Properties::create(_filename),
            Properties::MissingSection
            );
}

BOOST_AUTO_TEST_CASE( comment )
{
    _file << "#comment goes here" << endl;
    _file << "[section]" << endl;
    _file << "key=value # comment can also go here" << endl;
    _file << "[section two] # comment can also go here" << endl;
    _file << "key=value" << endl;
    BOOST_CHECK_NO_THROW(
            Properties::create(_filename)
            );
}

BOOST_AUTO_TEST_CASE( environmental )
{
    std::string old;
    if ( getenv(bgq::utility::Properties::EnvironmentalName.c_str()) ) {
        old = getenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    }

    int replace = 1;
    setenv(bgq::utility::Properties::EnvironmentalName.c_str(), _filename.c_str(), replace);

    // ensure ctor doesn't throw when using env
    Properties::Ptr prop = Properties::create();

    // ensure _filename is env value
    BOOST_CHECK_EQUAL(
            prop->getFilename(),
            _filename
            );

    // ensure ctor with arg does NOT use env
    setenv(bgq::utility::Properties::EnvironmentalName.c_str(), "/bogus/properties.file", replace);
    BOOST_CHECK_NO_THROW(
            prop = Properties::create(_filename)
            );
    BOOST_CHECK_EQUAL(
            prop->getFilename(),
            _filename
            );
    if ( old.empty() ) {
        unsetenv( bgq::utility::Properties::EnvironmentalName.c_str() );
    } else {
        setenv(bgq::utility::Properties::EnvironmentalName.c_str(), old.c_str(), replace);
    }
}

BOOST_AUTO_TEST_CASE( reload )
{
    _file << "[section]" << endl;
    _file << "key=value" << endl;
    Properties foo( _filename );
    BOOST_CHECK_EQUAL(
            foo.getValue( "section", "key" ),
            "value"
            );

    // close file
    _file.close();

    // open it and truncate
    _file.open(_filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

    // add another section
    _file << "[section_two]" << endl;
    _file << "key_two=value_two" << endl;

    // ensure it does not exist prior to reloading
    BOOST_CHECK_THROW(
            foo.getValue( "section_two", "key_two" ),
            std::invalid_argument
            );

    // reload
    foo.reload();

    // ensure key does not exist after reloading
    BOOST_CHECK_THROW(
            foo.getValue( "section", "key" ),
            std::invalid_argument
            );

    // ensure key_two exists in section_two
    BOOST_CHECK_EQUAL(
            foo.getValue( "section_two", "key_two" ),
            "value_two"
            );
}

void
getValue(
        Properties::ConstPtr properties,
        const std::string& section,
        const std::string& key,
        const std::string& expected_value,
        boost::barrier& barrier
        )
{
    // wait for all threads to get here
    barrier.wait();

    const size_t iterations = 100;
    for ( unsigned i = 0; i < iterations; ++i ) {
        std::string value = properties->getValue( section, key );
        // I don't think we can use BOOST_CHECK_EQUAL here in multiple threads
        BOOST_ASSERT( value == expected_value );
    }
}

BOOST_AUTO_TEST_CASE( threads )
{
#if 0
    // not really a unit test, but it's a test to ensure the reader/writer lock used to
    // handle reloading is doing its job
    const std::string section( "section" );
    const std::string key( "key" );
    const std::string value( "value" );
    _file << "[" << section << "]" << endl;
    _file << key << "=" << value << endl;
    Properties::Ptr foo( Properties::create(_filename) );

    // create some threads
    const unsigned numThreads = 8;
    boost::barrier barrier( numThreads + 1 );
    boost::thread_group threads;
    for ( unsigned i = 0; i < numThreads; ++i ) {
        threads.create_thread( 
                boost::bind(
                    &getValue,
                    foo,
                    boost::cref(section),
                    boost::cref(key),
                    boost::cref(value),
                    boost::ref(barrier)
                    )
                );
    }

    // wait for all threads to enter barrier
    barrier.wait();

    // reload properties inside loop
    const size_t iterations = 10;
    for ( unsigned int i=0; i < iterations; ++i ) {
        foo->reload();
    }

    // join all threads
    threads.join_all();
#endif
}

BOOST_AUTO_TEST_CASE( reload_garbage )
{
    _file << "[section]" << endl;
    _file << "key=value" << endl;
    Properties foo( _filename );
    BOOST_CHECK_EQUAL(
            foo.getValue( "section", "key" ),
            "value"
            );

    // close file
    _file.close();

    // open it and truncate
    _file.open(_filename.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

    // add garbage
    _file << "asdf" << endl;

    // reload should throw
    BOOST_CHECK_THROW(
            foo.reload(),
            std::exception
            );

    // ensure key still exist after reloading
    BOOST_CHECK_EQUAL(
            foo.getValue( "section", "key" ),
            "value"
            );
}

BOOST_AUTO_TEST_CASE( reload_relative_path )
{
    Properties foo( _filename );
    BOOST_CHECK_THROW(
            foo.reload( "some/relative/path/to/bg.properties" ),
            std::invalid_argument
            );
}

BOOST_AUTO_TEST_CASE( reload_fully_qualified_path )
{
    char* cwd = getcwd( NULL, 0 );
    std::cout << "cwd: " << cwd << std::endl;

    Properties foo( _filename );
    BOOST_CHECK_EQUAL(
            foo.reload( std::string(cwd) + "/" + _filename ),
            true
            );

    free( cwd );
}

BOOST_AUTO_TEST_SUITE_END()

