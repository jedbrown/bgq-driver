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
#include "client/options/Resource.h"

#include "common/JobInfo.h"
#include "common/MaximumLengthString.h"
#include "common/RanksPerNode.h"

#include <db/include/api/tableapi/gensrc/DBTBlock.h>
#include <boost/bind.hpp>

namespace runjob {
namespace client {
namespace options {

Resource::Resource(
        JobInfo& info
        ) : 
    Description( "Resource Options" )
{
    namespace po = boost::program_options;
    typedef MaximumLengthString<BGQDB::DBTBlock::BLOCKID_SIZE> BlockString;
    _options.add_options()
        (
         "block",
         po::value<BlockString>()
         ->notifier( boost::bind(&JobInfo::setBlock, boost::ref(info), _1) ),
         "block ID, must be initialized and requires Execute authority."
        )
        (
         "corner",
         po::value<runjob::Corner>()
         ->default_value( runjob::Corner() )
         ->notifier( boost::bind(&JobInfo::setCorner, boost::ref(info), _1) ),
         "sub-block compute node corner location: R00-M0-N04-J00"
        )
        (
         "shape",
         po::value<runjob::Shape>()
         ->default_value( runjob::Shape() )
         ->notifier( boost::bind(&JobInfo::setShape, boost::ref(info), _1) ),
         "five dimensional sub-block shape, in terms of compute nodes: 1x2x2x1x2"
        )
        (
         "ranks-per-node,p",
         po::value<RanksPerNode>()->
         default_value( runjob::defaults::ClientRanksPerNode )
         ->notifier( boost::bind(&JobInfo::setRanksPerNode, boost::ref(info), _1) ),
         "number of ranks per node: 1, 2, 4, 8, 16, 32, or 64"
        )
        (
         "np,n",
         po::value<JobInfo::Np>()
         ->notifier( boost::bind(&JobInfo::setNp, boost::ref(info), _1) ),
         "positive number of ranks in the entire job"
        )
        (
         "mapping",
         po::value<Mapping>()
         ->default_value( Mapping(Mapping::Type::Permutation, defaults::ClientMapping) )
         ->notifier( boost::bind(&JobInfo::setMapping, boost::ref(info), _1) ),
         "ABCDET permutation or path to mapping file"
        )
        ;
}

} // options
} // client
} // runjob
