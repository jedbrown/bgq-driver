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

#include <bgsched/Cable.h>

#include "bgsched/CableImpl.h"

using namespace bgsched;
using namespace std;

namespace bgsched {

Cable::Cable(
        Pimpl impl
        ) :
    Hardware(impl)
{
    // Nothing to do
}

const string&
Cable::getDestinationLocation() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getDestinationLocation();
}

SequenceId
Cable::getSequenceId() const
{
    Pimpl impl = boost::dynamic_pointer_cast<Impl>(_impl);
    return impl->getSequenceId();
}

} // namespace bgsched
