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
#include <utility/include/ScopeGuard.h>

#include <boost/assign/list_of.hpp>

#include <boost/bind.hpp>

#include <exception>
#include <vector>

using namespace bgq::utility;

int
main()
{
    typedef std::vector<unsigned> Vector;
    const unsigned value = 5;
    Vector foo = boost::assign::list_of( value );

    try {
        //
        // do some operations that may throw...
        //

        // clear our vector if we throw
        ScopeGuard guard(
                boost::bind(
                    &Vector::clear,
                    boost::ref( foo )
                    )
                );

        throw std::runtime_error( "hello world" );
    } catch ( const std::runtime_error& e ) {

    }

    return 0;
}

