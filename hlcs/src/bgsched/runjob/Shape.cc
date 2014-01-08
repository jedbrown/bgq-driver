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
#include <bgsched/runjob/Shape.h>

#include "ShapeImpl.h"

namespace bgsched {
namespace runjob {

Shape::Shape() :
    _impl( new Impl() )
{

}

Shape::Shape(
        const unsigned shape[5]
        ) :
    _impl( new Impl(shape) )
{

}

Shape&
Shape::value(
        const std::string& value
        )
{
    _impl->value( value );
    return *this;
}

Shape&
Shape::a(
        unsigned a
        )
{
    _impl->a( a );
    return *this;
}

Shape&
Shape::b(
        unsigned b
        )
{
    _impl->b( b );
    return *this;
}

Shape&
Shape::c(
        unsigned c
        )
{
    _impl->c( c );
    return *this;
}

Shape&
Shape::d(
        unsigned d
        )
{
    _impl->d( d );
    return *this;
}

Shape&
Shape::e(
        unsigned e
        )
{
    _impl->e( e );
    return *this;
}

Shape&
Shape::core(
        unsigned core
        )
{
    _impl->core( core );
    return *this;
}

unsigned
Shape::a() const
{
    return _impl->a();
}

unsigned
Shape::b() const
{
    return _impl->b();
}

unsigned
Shape::c() const
{
    return _impl->c();
}

unsigned
Shape::d() const
{
    return _impl->d();
}

unsigned
Shape::e() const
{
    return _impl->e();
}

unsigned
Shape::core() const
{
    return _impl->core();
}

const std::string&
Shape::value() const
{
    return _impl->value();
}

bool
Shape::valid() const
{
    return _impl->valid();
}

} // runjob
} // bgsched
