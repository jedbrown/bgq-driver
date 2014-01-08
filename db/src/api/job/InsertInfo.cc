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

#include "job/InsertInfo.h"

#include <boost/foreach.hpp>

#include <string>

using std::string;

namespace BGQDB {
namespace job {

std::string InsertInfo::format( const std::vector<std::string>& args_or_envs )
{
    string ret;

    BOOST_FOREACH( const string& val, args_or_envs ) {
        BOOST_FOREACH( char c, val ) {
            // if we have a backslash or space, escape it
            if ( c == '\\' || c == ' ' ) {
                ret += '\\';
            }

            // add char
            ret += c;
        }

        // add delimeter
        ret += ' ';
    }

    return ret;
}


InsertInfo::InsertInfo()
    : _block(),
      _exe(),
      _args(),
      _envs(),
      _cwd(),
      _user_name(),
      _mapping(),
      _processes_per_node(0),
      _shape(Shape::Block),
      _scheduler_data(),
      _corner(),
      _node_board_positions(),
      _hostname(),
      _pid(-1),
      _np(0),
      _client(0)
{
    // Nothing to do
}


} } // namespace BGQDB::job
