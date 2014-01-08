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
#ifndef BGSCHED_RUNJOB_NODE_IMPL_H
#define BGSCHED_RUNJOB_NODE_IMPL_H

#include <bgsched/runjob/Coordinates.h>
#include <bgsched/runjob/Node.h>

#include <string>

namespace bgsched {
namespace runjob {

class Node::Impl
{
public:
    Impl(
            const std::string& location,
            const Coordinates& coordinates
        );

    const std::string& location() const { return _location; }
    const Coordinates& coordinates() const { return _coordinates; }

private:
    std::string _location;
    Coordinates _coordinates;

};

} // runjob
} // bgsched

#endif
