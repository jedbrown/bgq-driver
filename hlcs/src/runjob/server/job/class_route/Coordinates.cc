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
#include "server/job/class_route/Coordinates.h"

#include "common/logging.h"

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
namespace class_route {

Coordinates::Coordinates(
        const CR_COORD_T* impl
        ) :
    _impl()
{
    if ( !impl ) {
        _impl.coords[0] = 0;
        _impl.coords[1] = 0;
        _impl.coords[2] = 0;
        _impl.coords[3] = 0;
        _impl.coords[4] = 0;
        return;
    }

    _impl = *impl;
}

Coordinates::Coordinates(
        unsigned a, 
        unsigned b,
        unsigned c,
        unsigned d,
        unsigned e
        )
{
    _impl.coords[0] = a;
    _impl.coords[1] = b;
    _impl.coords[2] = c;
    _impl.coords[3] = d;
    _impl.coords[4] = e;
}

unsigned
Coordinates::get(
        Dimension dimension
        ) const
{
    const unsigned d = static_cast<int>(dimension);
    if ( d >= CR_NUM_DIMS ) {
        LOG_FATAL_MSG( "unhandled dimension: " << static_cast<int>(dimension) );
        BOOST_ASSERT( !"shouldn't get here" );
    }
    
    const unsigned result = CR_COORD_DIM(&_impl, d);
    return result;
}

bool
operator==(
        const Coordinates& rhs,
        const Coordinates& lhs
        )
{
    return
        rhs.get( Dimension::A ) == lhs.get( Dimension::A ) &&
        rhs.get( Dimension::B ) == lhs.get( Dimension::B ) &&
        rhs.get( Dimension::C ) == lhs.get( Dimension::C ) &&
        rhs.get( Dimension::D ) == lhs.get( Dimension::D ) &&
        rhs.get( Dimension::E ) == lhs.get( Dimension::E )
        ;
}

bool
operator<(
        const Coordinates& rhs,
        const Coordinates& lhs
        )
{
    if ( rhs.get(Dimension::A) != lhs.get(Dimension::A) ) {
        return lhs.get(Dimension::A) < rhs.get(Dimension::A);
    }
    if ( rhs.get(Dimension::B) != lhs.get(Dimension::B) ) {
        return lhs.get(Dimension::B) < rhs.get(Dimension::B);
    }
    if ( rhs.get(Dimension::C) != lhs.get(Dimension::C) ) {
        return lhs.get(Dimension::C) < rhs.get(Dimension::C);
    }
    if ( rhs.get(Dimension::D) != lhs.get(Dimension::D) ) {
        return lhs.get(Dimension::D) < rhs.get(Dimension::D);
    }
    return lhs.get(Dimension::E) < rhs.get(Dimension::E);
}

std::ostream&
operator<<(
        std::ostream& os,
        const Coordinates& c
        )
{
    os << "(";
    for ( unsigned i = 0; i < CR_NUM_DIMS; ++i ) {
        if ( i != 0 ) {
            os << ",";
        }
        os << c.get( static_cast<Dimension>(i) );
    }
    os << ")";

    return os;
}

} // class_route
} // job
} // server
} // runjob
