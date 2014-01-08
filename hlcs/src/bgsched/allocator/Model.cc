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

#include <bgsched/allocator/Model.h>

#include "bgsched/allocator/ModelImpl.h"

using namespace bgsched;
using namespace bgsched::allocator;

namespace bgsched {
namespace allocator {

Model::Model(
        Pimpl impl
        ) :
    _impl(impl)
{

}

Model::Pimpl
Model::getPimpl() const
{
    return _impl;
}

Model::~Model()
{

}

} // namespace bgsched::allocator
} // namespace bgsched

