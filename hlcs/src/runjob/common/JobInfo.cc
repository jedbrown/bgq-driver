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
#include "common/JobInfo.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/Shape.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/filesystem/path.hpp>
#include <boost/program_options/errors.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <iostream>

#include <sys/stat.h>

namespace runjob {

LOG_DECLARE_FILE( runjob::log );

JobInfo::JobInfo() :
    _block(),
    _subBlock(),
    _args(),
    _envs(),
    _cwd(),
    _hostname(),
    _pid(0),
    _uid(),
    _ranks( defaults::ClientRanksPerNode ),
    _np(),
    _mapping(),
    _schedulerData(),
    _strace(),
    _stdinRank( 0 ),
    _umask( umask(0) )
{
    // reset umask
    umask( _umask );
}

void
JobInfo::setExe(
        const std::string& exe
        )
{
    _exe = exe;
    this->validateArgumentSize();
}

void
JobInfo::setArgs(
        const std::vector<std::string>& args
        )
{
    _args = args;
    this->validateArgumentSize();
}

void
JobInfo::setCorner(
        const Corner& corner
        )
{
    _subBlock.setCorner( corner );

    if ( _subBlock.corner().isCoreSet() && _ranks.getValue() != 1 ) {
        _ranks = 1;
    }
}

void
JobInfo::setRanksPerNode(
        const RanksPerNode& ranks
        )
{
    _ranks = ranks;

    if ( _subBlock.corner().isCoreSet() && _ranks.getValue() != 1 ) {
        _ranks = 1;
    }
}

void
JobInfo::addAllEnvironment()
{
    if ( !environ ) return;

    for ( unsigned int i = 0; environ[i] != NULL; ++i ) {
        try {
            const Environment env = boost::lexical_cast<Environment>( environ[i] );
            this->addEnvironment( env );
        } catch ( const boost::bad_lexical_cast& e ) {
            // this is really an options parsing error, so rethrow as an appropriate type
            throw boost::program_options::invalid_option_value( "--envs" );
        }
    }
}

void
JobInfo::addExportedEnvironmentVector(
        const std::vector<ExportedEnvironment>& envs
        )
{
    BOOST_FOREACH( const ExportedEnvironment& env, envs ) {
        this->addEnvironment( env );
    }
}

void
JobInfo::addEnvironmentVector(
        const EnvironmentVector& envs
        )
{
    BOOST_FOREACH( const Environment& env, envs ) {
        this->addEnvironment( env );
    }
}

void
JobInfo::addEnvironment(
        const Environment& env
        )
{
    // make sure environment was not previously added
    EnvironmentVector::const_iterator result =
        std::find_if(
                _envs.begin(),
                _envs.end(),
                boost::bind(
                    std::equal_to<std::string>(),
                    env.getKey(),
                    boost::bind(
                        &Environment::getKey,
                        _1
                        )
                    )
                );
    if ( result != _envs.end() ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "duplicate environment '" + 
                    env.getKey() + 
                    "' found. Previous value " + 
                    std::string( env )
                    )
                );
    }

    _envs.push_back( env );

    // count current size
    size_t size = 0;
    BOOST_FOREACH( const Environment& env, _envs ) {
        size += env.getKey().size();
        size += env.getValue().size();
        size += 1; // equals char
        size += 1; // null terminator
        LOG_DEBUG_MSG( "added environment: " << env.getKey() << " " << size );
        LOG_TRACE_MSG( env.getValue() );
    }

    // ensure all environments fit into required buffer
    if ( size >= static_cast<size_t>(bgcios::jobctl::MaxVariableSize) ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "environment variables size of " +
                    boost::lexical_cast<std::string>(size) +
                    " is greater than maximum size of " +
                    boost::lexical_cast<std::string>( bgcios::jobctl::MaxVariableSize )
                    )
                );
    }
}

void
JobInfo::validateArgumentSize() const
{
    size_t size = 0;
    size += _exe.size();
    size += 1; // NULL terminator
    BOOST_FOREACH( const std::string& arg, _args ) {
        size += arg.size();
        size += 1; // NULL terminator
        LOG_DEBUG_MSG( "added argument: " << arg << " " << size );
    }

    if ( static_cast<int32_t>(size) >= bgcios::jobctl::MaxArgumentSize ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "argument size of " +
                    boost::lexical_cast<std::string>(size) +
                    " bytes is larger than maximum size of " +
                    boost::lexical_cast<std::string>(bgcios::jobctl::MaxArgumentSize)
                    )
                );
    }
}

void
JobInfo::setCwd(
        const std::string& cwd
        )
{
    _cwd = cwd;
    if ( _cwd.empty() ) {
        // provide some reasonable default
        _cwd = "/";
        LOG_WARN_MSG( "could not get working directory, defaulting to " << _cwd );
    }

    this->validateMappingPath();
} 

void
JobInfo::setMapping(
        const Mapping& mapping
        ) 
{
    _mapping = mapping;
    if ( _cwd.empty() ) return;

    this->validateMappingPath();
} 

void
JobInfo::validateMappingPath()
{
    if ( _mapping.type() == Mapping::Type::None ) return;
    if ( _mapping.type() == Mapping::Type::Permutation ) return;

    if ( boost::filesystem::path(_mapping.value()).is_complete() ) return;

    // relative path, prepend cwd
    if ( _cwd.at(_cwd.size() - 1) == '/' ) {
        _mapping = Mapping( Mapping::Type::File, _cwd + _mapping.value() );
    } else {
        _mapping = Mapping( Mapping::Type::File, _cwd + "/" + _mapping.value() );
    }

    // validate size fits into jobctl restrictions
    if ( _mapping.value().size() > static_cast<unsigned>(bgcios::jobctl::MaxPathSize) ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "mapping path size of " +
                    boost::lexical_cast<std::string>(_mapping.value().size()) +
                    " bytes is larger than maximum size of " +
                    boost::lexical_cast<std::string>(bgcios::jobctl::MaxPathSize)
                    )
                );
    }
}

} // runjob
