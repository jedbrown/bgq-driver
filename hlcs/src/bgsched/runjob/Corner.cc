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
#include <bgsched/runjob/Corner.h>

#include "CornerImpl.h"

namespace bgsched {
namespace runjob {

Corner::Corner() :
    _impl( new Impl() )
{

}

Corner::Corner(
        const unsigned coordinates[5]
        ) :
    _impl( new Impl(coordinates) )
{
    
}

Corner&
Corner::location(
        const std::string& location
        )
{
    _impl->location( location );
    return *this;
}

Corner&
Corner::coordinates(
        const Coordinates& coordinates
        )
{
    _impl->coordinates( coordinates );
    return *this;
}

const std::string&
Corner::location() const
{
    return _impl->location();
}

const Coordinates&
Corner::coordinates() const
{
    return _impl->coordinates();
}

} // runjob
} // bgsched
