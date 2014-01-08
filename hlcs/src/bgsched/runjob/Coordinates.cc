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
#include <bgsched/runjob/Coordinates.h>

#include "CoordinatesImpl.h"

#include <iostream>

namespace bgsched {
namespace runjob {

Coordinates::Coordinates() :
    _impl( new Impl() )
{

}

Coordinates::Coordinates(
        const unsigned coordinates[5]
        ) :
    _impl( new Impl(coordinates) )
{

}
Coordinates&
Coordinates::a(
        unsigned a
        )
{
    _impl->a( a );
    return *this;
}

Coordinates&
Coordinates::b(
        unsigned b
        )
{
    _impl->b( b );
    return *this;
}

Coordinates&
Coordinates::c(
        unsigned c
        )
{
    _impl->c( c );
    return *this;
}

Coordinates&
Coordinates::d(
        unsigned d
        )
{
    _impl->d( d );
    return *this;
}

Coordinates&
Coordinates::e(
        unsigned e
        )
{
    _impl->e( e );
    return *this;
}

unsigned
Coordinates::a() const
{
    return _impl->a();
}

unsigned
Coordinates::b() const
{
    return _impl->b();
}

unsigned
Coordinates::c() const
{
    return _impl->c();
}

unsigned
Coordinates::d() const
{
    return _impl->d();
}

unsigned
Coordinates::e() const
{
    return _impl->e();
}

bool
Coordinates::valid() const
{
    return _impl->valid();
}

std::ostream&
operator<<(
        std::ostream& os,
        const Coordinates& c
        )
{
    os <<
        "(" <<
        c.a() << "," <<
        c.b() << "," <<
        c.c() << "," <<
        c.d() << "," <<
        c.e() <<
        ")"
        ;

    return os;
}

} // runjob
} // bgsched
