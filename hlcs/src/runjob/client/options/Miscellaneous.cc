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
#include "client/options/Miscellaneous.h"

#include "common/defaults.h"
#include "common/JobInfo.h"

#include <boost/bind.hpp>

namespace runjob {
namespace client {
namespace options {

const sockaddr_un Miscellaneous::sockaddr = sockaddr_un();

Miscellaneous::Miscellaneous(
        JobInfo& info
        ) :
    Description( "Miscellaneous Options" )
{
    namespace po = boost::program_options;

    _options.add_options()
        (
         "stdinrank",
         po::value<JobInfo::StdinRank>()->
         default_value(runjob::defaults::ClientStdinRank)->
         notifier( boost::bind(&JobInfo::setStdinRank, boost::ref(info), _1) ),
         "rank to send stdin to"
        )
        ("raise", po::bool_switch(), "if the job dies with a signal, raise it")
        ;
}

} // options
} // client
} // runjob
