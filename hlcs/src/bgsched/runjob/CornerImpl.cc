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
#include "bgsched/runjob/CornerImpl.h"

namespace bgsched {
namespace runjob {

Corner::Impl::Impl() :
    _location(),
    _coordinates()
{

}

Corner::Impl::Impl(
        const unsigned coordinates[5]
        ) :
    _location(),
    _coordinates( coordinates )
{

}

void
Corner::Impl::location(
        const std::string& location
        )
{
    _location = location;
}

void
Corner::Impl::coordinates(
        const Coordinates& coordinates
        )
{
    _coordinates = coordinates;
}

} // runjob
} // bgsched

