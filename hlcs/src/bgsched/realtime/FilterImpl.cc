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

#include "FilterImpl.h"

#include <bgsched/realtime/FilterException.h>

#include "bgsched/JobImpl.h"
#include "bgsched/BlockImpl.h"
#include "bgsched/utility.h"

#include "utility/include/Log.h"

#include <boost/xpressive/xpressive.hpp>

#include <ctype.h>

#include <iostream>


using namespace std;


LOG_DECLARE_FILE( "bgsched" );


namespace bgsched {
namespace realtime {


Filter::Impl Filter::Impl::_createAll()
{
    Impl ret;

    ret._jobs = true;
    ret._job_deleted = true;
    ret._blocks = true;
    ret._block_deleted = true;
    ret._midplanes = true;
    ret._node_boards = true;
    ret._nodes = true;
    ret._switches = true;
    ret._torus_cables = true;
    ret._io_cables = true;
    ret._io_drawers = true;
    ret._io_nodes = true;
    ret._ras_events = true;

    return ret;
}


Filter::Impl Filter::Impl::_createDefault()
{
    Impl ret;

    ret._jobs = true;
    ret._job_deleted = true;
    ret._blocks = true;
    ret._block_deleted = true;
    ret._midplanes = true;
    ret._node_boards = true;
    ret._nodes = true;
    ret._switches = true;
    ret._torus_cables = true;
    ret._io_cables = false;
    ret._io_drawers = false;
    ret._io_nodes = false;
    ret._ras_events = false;

    return ret;
}


const Filter::Impl Filter::Impl::NONE;
const Filter::Impl Filter::Impl::ALL(_createAll());
const Filter::Impl Filter::Impl::DEFAULT(_createDefault());


Filter::Impl::RePtr Filter::Impl::compilePattern( const std::string& pattern )
{
    if ( pattern == std::string() )  return RePtr();

    try {
        RePtr ret( new boost::xpressive::sregex(boost::xpressive::sregex::compile( pattern )) );
        return ret;
    } catch ( boost::xpressive::regex_error& e ) {
        throw std::invalid_argument( e.what() );
    }
    return RePtr();
}


void Filter::Impl::_checkAndSetPattern(
        const std::string* pattern_str_p,
        const std::string& pattern_name,
        std::string* s_out
    )
{
    if ( ! pattern_str_p ) {
        *s_out = string();
        return;
    }

    try {
        compilePattern( *pattern_str_p );
        *s_out = *pattern_str_p;
    } catch ( const invalid_argument& e ) {
        THROW_EXCEPTION(
                FilterException,
                FilterErrors::PatternNotValid,
                "while setting the " << pattern_name << " pattern in the real-time filter, " << e.what()
            );
    }
}


Filter::Impl::Impl()
    : _jobs(false),
      _job_deleted(false),
      _blocks(false),
      _block_deleted(false),
      _midplanes(false),
      _node_boards(false),
      _nodes(false),
      _switches(false),
      _torus_cables(false),
      _io_cables(false),
      _io_drawers(false),
      _io_nodes(false),
      _ras_events(false)
{
    // Nothing to do.
}


ostream& operator<<( ostream& os, const Filter::Impl& filter )
{
    os << "{";
    if ( filter.getJobs() ) {
        os << " jobs={";
        os << " block='" << filter.getJobBlockIdPattern() << "'";
        if ( filter.getJobDeleted() )  os << " deleted";
        os << "}";
    }
    if ( filter.getBlocks() ) {
        os << " blocks={";
        os << " id='" << filter.getBlockIdPattern() << "'";
        if ( filter.getBlockDeleted() )  os << " deleted";
        os << "}";
    }
    if ( filter.getMidplanes() )  os << " midplanes";
    if ( filter.getNodeBoards() )  os << " nodeBoards";
    if ( filter.getNodes() )  os << " nodes";
    if ( filter.getIoNodes() )  os << " ioNodes";
    if ( filter.getSwitches() )  os << " switches";
    if ( filter.getTorusCables() )  os << " torusCables";
    if ( filter.getIoCables() )  os << " ioCables";
    if ( filter.getRasEvents() ) {
        os << " ras={";
        os << " msgId='" << filter.getRasMessageIdPattern() << "'";
        os << " blockId='" << filter.getRasComputeBlockIdPattern() << "'";
        os << "}";
    }
    os << "}";
    return os;
}

} // namespace bgsched::realtime
} // namespace bgsched
