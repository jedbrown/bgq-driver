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

#include "../WhereClause.hpp"


#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE WhereClause
#include <boost/test/unit_test.hpp>


using namespace bgws;


BOOST_AUTO_TEST_CASE( test1 )
{
    // Create WhereClause, get string is empty.

    WhereClause where_clause;
    BOOST_CHECK_EQUAL( where_clause.getString(), "" );
}


BOOST_AUTO_TEST_CASE( test_add )
{
    // Can add a condition, getString is WHERE <condition>

    WhereClause where_clause;

    where_clause.add( "e.recid=?" );

    BOOST_CHECK_EQUAL( where_clause.getString(), " WHERE (e.recid=?)" );
}


BOOST_AUTO_TEST_CASE( test_add_two )
{
    // Add two conditions, gets separated by AND

    WhereClause where_clause;

    where_clause.add( "e.recid > ?" );
    where_clause.add( "e.recid < ?" );

    BOOST_CHECK_EQUAL( where_clause.getString(), " WHERE (e.recid > ?) AND (e.recid < ?)" );
}
