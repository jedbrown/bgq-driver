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
#ifndef BGSCHED_RUNJOB_COORDINATES_IMPL_H_
#define BGSCHED_RUNJOB_COORDINATES_IMPL_H_

#include <bgsched/runjob/Coordinates.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief
 */
class Coordinates::Impl 
{
public:
    Impl();

    Impl(
            const unsigned coordinates[5]
        );

    void a( unsigned a ) { _a = a; }
    void b( unsigned b ) { _b = b; }
    void c( unsigned c ) { _c = c; }
    void d( unsigned d ) { _d = d; }
    void e( unsigned e ) { _e = e; }

    unsigned a() const { return _a; }
    unsigned b() const { return _b; }
    unsigned c() const { return _c; }
    unsigned d() const { return _d; }
    unsigned e() const { return _e; }

    bool valid() const;

private:
    unsigned _a;
    unsigned _b;
    unsigned _c;
    unsigned _d;
    unsigned _e;
};

} // runjob
} // bgsched

#endif
