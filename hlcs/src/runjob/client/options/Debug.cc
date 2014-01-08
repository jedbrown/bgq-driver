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
#include "client/options/Debug.h"

#include "client/options/Label.h"

#include "common/tool/Daemon.h"

#include "common/JobInfo.h"
#include "common/Strace.h"

#include <boost/bind.hpp>

namespace runjob {
namespace client {
namespace options {


Debug::Debug(
        JobInfo& info,
        tool::Daemon& tool
        ) :
    Description( "Debug Options" )
{
    namespace po = boost::program_options;
    _options.add_options()
        (
         "label",
         po::value<Label>()
         ->implicit_value( Label::Long )
         ->default_value( Label::None ),
         "prefix job output with stdout, stderr, and rank"
        )
        (
         "strace",
         po::value<runjob::Strace>()
         ->default_value( Strace(Strace::None) )
         ->notifier( boost::bind(&JobInfo::setStrace, boost::ref(info), _1) ),
         "specify  none, or n where n is a rank to enable system call tracing"
        )
        (
         "start-tool",
         po::value<tool::Daemon::Executable>()
         ->default_value( std::string() )
         ->notifier( boost::bind(&tool::Daemon::setExecutable, boost::ref(tool), _1) ),
         "path to tool to start with the job"
        ) 
        (
         "tool-args",
         po::value<std::string>()
         ->default_value( std::string() )
         ->notifier( boost::bind(&tool::Daemon::splitArguments, boost::ref(tool), _1) ),
         "arguments for the tool"
        )
        (
         "tool-subset",
         po::value<tool::Subset>()
         ->default_value( tool::Subset() )
         ->notifier( boost::bind(&tool::Daemon::setSubset, boost::ref(tool), _1) ),
         "rank subset to use when launching the tool daemon"
        )
        ;
}

} // options
} // client
} // runjob
