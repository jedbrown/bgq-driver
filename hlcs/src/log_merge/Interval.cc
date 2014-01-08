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


#include "Interval.hpp"

#include <boost/throw_exception.hpp>

#include <stdexcept>


namespace log_merge {


const Interval::ConstPtr Interval::DefaultPtr( new Interval() );


void Interval::validate() const
{
    if ( ! isSet() )  return;

    if ( _start >= _end ) {
        BOOST_THROW_EXCEPTION( std::runtime_error( "the interval is not valid, the start time is after the end time" ) );
    }
}


std::ostream& operator<<( std::ostream& os, const Interval& i )
{
    os << "[" << i._start << " - " << i._end << "]";
    return os;
}


} // namespace log_merge
