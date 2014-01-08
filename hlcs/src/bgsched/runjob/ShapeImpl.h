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
#ifndef BGSCHED_RUNJOB_SHAPE_IMPL_H_
#define BGSCHED_RUNJOB_SHAPE_IMPL_H_

#include <bgsched/runjob/Shape.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief
 */
class Shape::Impl 
{
public:
    Impl();

    Impl(
            const unsigned shape[5]
        );

    void value( const std::string& value ) { _value = value; }

    void a( unsigned a );
    void b( unsigned b );
    void c( unsigned c );
    void d( unsigned d );
    void e( unsigned e );
    void core( unsigned core );

    unsigned a() const { return _a; }
    unsigned b() const { return _b; }
    unsigned c() const { return _c; }
    unsigned d() const { return _d; }
    unsigned e() const { return _e; }
    unsigned core() const { return _core; }

    bool valid() const;

    const std::string& value() const { return _value; }

private:
    std::string _value;
    unsigned _a;
    unsigned _b;
    unsigned _c;
    unsigned _d;
    unsigned _e;
    unsigned _core;
};

} // runjob
} // bgsched

#endif
