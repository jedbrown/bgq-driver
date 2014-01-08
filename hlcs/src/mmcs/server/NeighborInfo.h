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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_SERVER_NEIGHBOR_INFO_H_
#define MMCS_SERVER_NEIGHBOR_INFO_H_


#include "types.h"

#include <bgq_util/include/Location.h>

#include <boost/shared_ptr.hpp>

#include <set>


namespace mmcs {
namespace server {


class NeighborInfo
{
public:
    NeighborInfo(
            const boost::shared_ptr<BlockControllerBase>& block,
            const char* location,
            char* rawdata
            );

private:
    void compute(
            const boost::shared_ptr<BlockControllerBase>& block,
            char* rawdata
            );

    void linkChip(
            const boost::shared_ptr<BlockControllerBase>& block,
            char* rawdata
            );

private:
    void ioLink(
            char* rawdata
            );

    void computeNodeIoLink(
            char* rawdata
            );

    void ioNodeIoLink(
            char* rawdata
            );

    int next_coord(
            unsigned start,
            unsigned size,
            bool torus
            );

    int prev_coord(
            unsigned start,
            unsigned size,
            bool torus
            );


public:
    static const std::set<int> MessageIds;

private:
    const bgq::util::Location _location;
};

} } // namespace mmcs::server

#endif
