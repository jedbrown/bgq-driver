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
#include "server/job/class_route/Rectangle.h"

#include "common/logging.h"

#include <boost/assert.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
namespace class_route {

Rectangle::Rectangle(
        const CR_RECT_T* impl
        ) :
    _impl(),
    _size( 0 )
{
    if ( !impl ) {
        *CR_RECT_LL(&_impl) = (CR_COORD_T) {{0,0,0,0,0}};
        *CR_RECT_UR(&_impl) = (CR_COORD_T) {{0,0,0,0,0}};
        return;
    }

    _impl = *impl;
    _size = 1;

    for ( unsigned dimension = 0; dimension < CR_NUM_DIMS; ++dimension) {
        _size *= (CR_COORD_DIM(CR_RECT_UR(&_impl),dimension) - CR_COORD_DIM(CR_RECT_LL(&_impl),dimension) + 1);
    }
}

    
unsigned
Rectangle::size(
        Dimension dimension
        ) const
{
    const unsigned d = static_cast<int>(dimension);
    if ( d >= CR_NUM_DIMS ) {
        LOG_FATAL_MSG( "unhandled dimension: " << static_cast<int>(dimension) );
        BOOST_ASSERT( !"shouldn't get here" );
    }
    
    const unsigned result = (CR_COORD_DIM(CR_RECT_UR(&_impl),d) - CR_COORD_DIM(CR_RECT_LL(&_impl),d) + 1);
    return result;
}

std::ostream&
operator<<(
        std::ostream& os,
        const Rectangle& rectangle
        )
{
    os <<
        rectangle.size( Dimension::A ) << "x" <<
        rectangle.size( Dimension::B ) << "x" <<
        rectangle.size( Dimension::C ) << "x" <<
        rectangle.size( Dimension::D ) << "x" <<
        rectangle.size( Dimension::E ) <<
        " "
        ;
    os << "(" <<
        rectangle.impl().ll.coords[0] << "," <<
        rectangle.impl().ll.coords[1] << "," <<
        rectangle.impl().ll.coords[2] << "," <<
        rectangle.impl().ll.coords[3] << "," <<
        rectangle.impl().ll.coords[4] <<
        "):(" <<
        rectangle.impl().ur.coords[0] << "," <<
        rectangle.impl().ur.coords[1] << "," <<
        rectangle.impl().ur.coords[2] << "," <<
        rectangle.impl().ur.coords[3] << "," <<
        rectangle.impl().ur.coords[4] <<
        ")"
        ;

    return os;
}

} // class_route
} // job
} // server
} // runjob
