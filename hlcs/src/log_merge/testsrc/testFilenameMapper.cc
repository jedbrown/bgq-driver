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


#include "FilenameMapper.hpp"


#define BOOST_TEST_MODULE FilenameMapper
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


using log_merge::FilenameMapper;


BOOST_AUTO_TEST_CASE( test_MapFilenames )
{
    FilenameMapper filename_mapper( log_merge::MapFilenames::Enabled );

    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2-Q04-I5-J00-bgagentd.log" ),           "bgagentd      " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2-bgmaster_server.log" ),               "bgmaster      " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2.rchland.ibm.com-mc_server.log" ),     "mc_server     " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2.rchland.ibm.com-mmcs_server.log" ),   "mmcs_server   " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2.rchland.ibm.com-runjob_mux.log" ),    "runjob_mux    " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2.rchland.ibm.com-runjob_server.log" ), "runjob_server " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqssn2.rchland.ibm.com-subnet_mc0.log" ),   "subnet_mc     " );
    BOOST_CHECK_EQUAL( filename_mapper.map( "bgqsn2.rchland.ibm.com-bgws_server.log" ),   "bgws_server   " );
}


BOOST_AUTO_TEST_CASE( test_unknownformat )
{
    // If format is unknown, at least strips off the log part.
    BOOST_CHECK_EQUAL( FilenameMapper( log_merge::MapFilenames::Enabled ).map( "unknownFilenameFormat.log" ), "unknownFilenameFormat" );
}


BOOST_AUTO_TEST_CASE( test_unknownformatDirectory )
{
    // If mapping enabled and name not recognized and has directory, directory is stripped.
    BOOST_CHECK_EQUAL( FilenameMapper( log_merge::MapFilenames::Enabled ).map( "./unknownFilenameFormat.log" ), "unknownFilenameFormat" );
}


BOOST_AUTO_TEST_CASE( test_MappingDisabled )
{
    BOOST_CHECK_EQUAL( FilenameMapper( log_merge::MapFilenames::Disabled ).map( "bgpfs29-io-bgagentd-2010-0412-19:05:34.log" ), "bgpfs29-io-bgagentd-2010-0412-19:05:34.log" );
}


BOOST_AUTO_TEST_CASE( test_MappingDisabledDirectory )
{
    // If has directory, it's stripped.

    BOOST_CHECK_EQUAL( FilenameMapper( log_merge::MapFilenames::Disabled ).map( "./bgpfs29-io-bgagentd-2010-0412-19:05:34.log" ), "bgpfs29-io-bgagentd-2010-0412-19:05:34.log" );
}
