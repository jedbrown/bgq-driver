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
#include "ProcessTree.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/lexical_cast.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

ProcessTree::ProcessTree(
        pid_t pid
        ) :
    _pid( pid )
{

}

std::ostream&
operator<<(
        std::ostream& stream,
        const ProcessTree& tree
        )
{
    // walk up process tree
    pid_t pid = tree.getPid();
    while ( pid != 1 ) {
        // create path
        std::ostringstream path;
        path << "/proc/" <<pid << "/status";

        // open it
        std::ifstream input( path.str().c_str() );
        if ( !input ) {
            stream << "could not open " << path.str();
            stream.setstate( std::ios::failbit );
            return stream;
        }

        // look for parent pid line
        pid_t ppid = 0;
        std::string name;
        std::string line;
        while ( std::getline( input, line ) && ppid == 0 ) {
            // split on colon
            typedef std::vector<std::string> Tokens;
            Tokens tokens;
            boost::split( tokens, line, boost::is_any_of(":") );
            
            // we expect 2 tokens
            if ( tokens.size() != 2 ) {
                continue;
            }

            // strip whitespace
            boost::trim( tokens[0] );
            boost::trim( tokens[1] );

            // look to see if we found parent pid
            if ( tokens[0] == "PPid" ) {
                try {
                    ppid = boost::lexical_cast<pid_t>( tokens[1] );
                } catch ( const boost::bad_lexical_cast& e ) {
                    stream << e.what();
                    stream.setstate( std::ios::failbit );
                    return stream;
                }
            } else if ( tokens[0] == "Name" ) {
                name = tokens[1];
            } else {
                // some other token we don't care about
            }
        }

        // ensure we found something
        if ( ppid == 0 ) {
            stream << "could not find parent pid for " << pid;
            stream.setstate( std::ios::failbit );
            return stream;
        }

        // log its value
        stream << pid << " (" << name << ") --> " << ppid << std::endl;

        // get next parent
        pid = ppid;
    }

    return stream;
}
