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
#include "bgsched/runjob/CoordinatesImpl.h"

#include <limits>

namespace bgsched {
namespace runjob {

Coordinates::Impl::Impl() :
    _a( std::numeric_limits<unsigned>::max() ),
    _b( std::numeric_limits<unsigned>::max() ),
    _c( std::numeric_limits<unsigned>::max() ),
    _d( std::numeric_limits<unsigned>::max() ),
    _e( std::numeric_limits<unsigned>::max() )
{

}

Coordinates::Impl::Impl(
        const unsigned coordinates[5]
        ) :
    _a( coordinates[0] ),
    _b( coordinates[1] ),
    _c( coordinates[2] ),
    _d( coordinates[3] ),
    _e( coordinates[4] )
{

}

bool
Coordinates::Impl::valid() const
{
    return
        _a != std::numeric_limits<unsigned>::max() &&
        _b != std::numeric_limits<unsigned>::max() &&
        _c != std::numeric_limits<unsigned>::max() &&
        _d != std::numeric_limits<unsigned>::max() &&
        _e != std::numeric_limits<unsigned>::max()
        ;
}

} // runjob
} // bgsched
