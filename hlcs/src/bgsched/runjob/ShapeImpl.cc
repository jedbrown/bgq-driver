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
#include "bgsched/runjob/ShapeImpl.h"

#include <bgq_util/include/Location.h>

#include <boost/lexical_cast.hpp>

#include <stdexcept>

namespace bgsched {
namespace runjob {

Shape::Impl::Impl() :
    _value(),
    _a( 0 ),
    _b( 0 ),
    _c( 0 ),
    _d( 0 ),
    _e( 0 ),
    _core( bgq::util::Location::ComputeCardCoresOnBoard - 1 ) // default to use all cores
{

}

Shape::Impl::Impl(
        const unsigned shape[5]
        ) :
    _value(),
    _a( 0 ),
    _b( 0 ),
    _c( 0 ),
    _d( 0 ),
    _e( 0 ),
    _core( bgq::util::Location::ComputeCardCoresOnBoard - 1 ) // default to use all cores
{
    this->a( shape[0] );
    this->b( shape[1] );
    this->c( shape[2] );
    this->d( shape[3] );
    this->e( shape[4] );
}

bool
Shape::Impl::valid() const
{
    return 
        _a != 0 &&
        _b != 0 &&
        _c != 0 &&
        _d != 0 &&
        _e != 0
        ;
}

void
Shape::Impl::a(
        unsigned a
        )
{
    if ( a > 4 ) {
        throw std::logic_error(
                "A dimension of size " +
                boost::lexical_cast<std::string>(a) +
                " is larger than maximum size of 4"
                );
    } else if ( a == 3 ) {
        throw std::logic_error(
                "A dimension of size " +
                boost::lexical_cast<std::string>(a) +
                " is not a power of two"
                );
    } else if ( !a ) {
        throw std::logic_error(
                "A dimension of size 0 is not valid"
                );
    }

    _a = a;
    _core = bgq::util::Location::ComputeCardCoresOnBoard - 1;
}

void
Shape::Impl::b(
        unsigned b
        )
{
    if ( b > 4 ) {
        throw std::logic_error(
                "B dimension of size " +
                boost::lexical_cast<std::string>(b) +
                " is larger than maximum size of 4"
                );
    } else if ( b == 3 ) {
        throw std::logic_error(
                "B dimension of size " +
                boost::lexical_cast<std::string>(b) +
                " is not a power of two"
                );
    } else if ( !b ) {
        throw std::logic_error(
                "B dimension of size 0 is not valid"
                );
    }

    _b = b;
    _core = bgq::util::Location::ComputeCardCoresOnBoard - 1;
}

void
Shape::Impl::c(
        unsigned c
        )
{
    if ( c > 4 ) {
        throw std::logic_error(
                "C dimension of size " +
                boost::lexical_cast<std::string>(c) +
                " is larger than maximum size of 4"
                );
    } else if ( c == 3 ) {
        throw std::logic_error(
                "C dimension of size " +
                boost::lexical_cast<std::string>(c) +
                " is not a power of two"
                );
    } else if ( !c ) {
        throw std::logic_error(
                "C dimension of size 0 is not valid"
                );
    }

    _c = c;
    _core = bgq::util::Location::ComputeCardCoresOnBoard - 1;
}

void
Shape::Impl::d(
        unsigned d
        )
{
    if ( d > 4 ) {
        throw std::logic_error(
                "D dimension of size " +
                boost::lexical_cast<std::string>(d) +
                " is larger than maximum size of 4"
                );
    } else if ( d == 3 ) {
        throw std::logic_error(
                "D dimension of size " +
                boost::lexical_cast<std::string>(d) +
                " is not a power of two"
                );
    } else if ( !d ) {
        throw std::logic_error(
                "D dimension of size 0 is not valid"
                );
    }

    _d = d;
    _core = bgq::util::Location::ComputeCardCoresOnBoard - 1;
}

void
Shape::Impl::e(
        unsigned e
        )
{
    if ( e > 2 ) {
        throw std::logic_error(
                "E dimension of size " +
                boost::lexical_cast<std::string>(e) +
                " is larger than maximum size of 2"
                );
    } else if ( !e ) {
        throw std::logic_error(
                "E dimension of size 0 is not valid"
                );
    }

    _e = e;
    _core = bgq::util::Location::ComputeCardCoresOnBoard - 1;
}

void
Shape::Impl::core(
        unsigned core
        )
{
    if ( core >= bgq::util::Location::ComputeCardCoresOnBoard - 1 ) {
        throw std::logic_error(
                "Core " +
                boost::lexical_cast<std::string>(core) +
                " is not between 0 and 15"
                );
    }
    _core = core;
    _a = 1;
    _b = 1;
    _c = 1;
    _d = 1;
    _e = 1;
}

} // runjob
} // bgsched
