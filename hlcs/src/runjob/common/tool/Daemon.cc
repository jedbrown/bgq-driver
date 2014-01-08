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
#include "common/tool/Daemon.h"

#include "common/logging.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <boost/program_options/errors.hpp>

#include <boost/foreach.hpp>

namespace runjob {
namespace tool {

Daemon::Daemon() :
    _exe(),
    _exeSet( false ),
    _args(),
    _argsSet( false ),
    _subset(),
    _subsetSet( false )
{

}

void
Daemon::setExecutable(
        const Executable& exe
        )
{
    _exe = exe;
    _exeSet = true;

    this->validate();
}

void
Daemon::splitArguments(
        const std::string& args
        )
{
    _argsSet = true;
    _args.clear();

    if ( args.empty() ) return;

    // split on spaces
    boost::split( _args, args, boost::is_any_of(" ") );

    this->validate();
}

void
Daemon::setArguments(
        const Arguments& args
        )
{
    _argsSet = true;
    _args = args;

    this->validate();
}

void
Daemon::setSubset(
        const Subset& subset
        )
{
    _subset = subset;
    _subsetSet = true;

    this->validate();
}

void
Daemon::validate()
{
    if ( !_exeSet || !_argsSet || !_subsetSet ) return;

    if ( _exe.empty() && !_args.empty() ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "tool arguments provided but missing tool path"
                    )
                );
    } else if ( _exe.empty() && !_subset.value().empty() ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "tool subset provided but missing tool path"
                    )
                );
    }

    // validate length of exe and arguments can fit into
    // jobctl daemon protocol
    size_t size = 0;
    size += _exe.size();
    size += 1; // NULL terminator
    BOOST_FOREACH( const std::string& arg, _args ) {
        size += arg.size();
        size += 1; // NULL terminator
    }

    if ( static_cast<int32_t>(size) >= bgcios::jobctl::MaxArgumentSize ) {
        BOOST_THROW_EXCEPTION(
                boost::program_options::error(
                    "tool argument size of " +
                    boost::lexical_cast<std::string>(size) +
                    " bytes is larger than maximum size of " +
                    boost::lexical_cast<std::string>(bgcios::jobctl::MaxArgumentSize)
                    )
                );
    }
}

} // tool
} // runjob
