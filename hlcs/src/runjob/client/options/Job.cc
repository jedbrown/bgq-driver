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
#include "client/options/Job.h"

#include "common/Environment.h"
#include "common/ExportedEnvironment.h"
#include "common/logging.h"
#include "common/JobInfo.h"
#include "common/MaximumLengthString.h"
#include "common/WorkingDir.h"

#include <db/include/api/tableapi/gensrc/DBTJob.h>
#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/statement.hpp>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

LOG_DECLARE_FILE( runjob::client::log );

namespace runjob {
namespace client {
namespace options {

Job::Job(
        JobInfo& info
        ) :
    Description( "Job Options" ),
    _timeout()
{
    namespace po = boost::program_options;

    // types for arguments
    typedef std::vector<std::string> StringVector;
    typedef std::vector<Environment> EnvironmentVector;
    typedef std::vector<ExportedEnvironment> ExportedEnvironmentVector;
    typedef MaximumLengthString<BGQDB::DBTJob::EXECUTABLE_SIZE> ExeString;

    std::string current_wdir;
    try {
        current_wdir = boost::filesystem::current_path().string();
    } catch ( const std::exception& e ) {
        LOG_DEBUG_MSG( "could not get working dir: " << e.what() );
    }
    _options.add_options()
        (
         "exe",
         po::value<ExeString>()
         ->notifier( boost::bind(&JobInfo::setExe, boost::ref(info), _1) ),
         "executable to run"
        )
        (
         "args",
         po::value<StringVector>()
         ->multitoken()
         ->composing()
         ->notifier( boost::bind(&JobInfo::setArgs, boost::ref(info), _1) ),
         "arguments"
        )
        (
         "envs",
         po::value<EnvironmentVector>()
         ->multitoken()
         ->composing()
         ->notifier( boost::bind(&JobInfo::addEnvironmentVector, boost::ref(info), _1) ),
         "environment variables in key=value form"
        )
        (
         "exp-env",
         po::value<ExportedEnvironmentVector>()
         ->multitoken()
         ->composing()
         ->notifier( boost::bind(&JobInfo::addExportedEnvironmentVector, boost::ref(info), _1) ),
         "export a specific environment variable"
        )
        (
         "env-all",
         po::bool_switch()
         ->notifier(
             // lambda (unnamed) function to check if the parameter value is true
             // if so, it invokes the JobInfo method to do so
             boost::phoenix::if_( boost::phoenix::arg_names::arg1 )
             [
             boost::phoenix::bind( &JobInfo::addAllEnvironment, boost::phoenix::ref(info) )
             ]
            ),
         "export all environment variables"
        )
        (
         "cwd",
         po::value<WorkingDir>()
         ->default_value( current_wdir, "current wdir" )
         ->notifier( boost::bind(&JobInfo::setCwd, boost::ref(info), _1) ),
         "current working directory"
        )
        (
         "timeout",
         po::value(&_timeout),
         "positive number of seconds to wait after runjob starts before a SIGKILL will be delivered."
        )
        ;
}

} // options
} // client
} // runjob
