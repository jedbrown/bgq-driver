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

#include "common/logging.h"
#include "common/unexpected.h"

#include <execinfo.h>
#include <cxxabi.h>

LOG_DECLARE_FILE( runjob::log );

namespace runjob {

void
log_stack_trace()
{
    const int frames = 50;
    void *trace_elems[50];
    int trace_elem_count(backtrace( trace_elems, frames ));
    char **stack_syms(backtrace_symbols( trace_elems, trace_elem_count ));

    // log mangled names first
    for ( int i = 0; i < trace_elem_count; ++i )
    {
        LOG_FATAL_MSG( stack_syms[i] );
    }

    LOG_FATAL_MSG( "demangled trace below" );

    // now attempt to demangle
    for ( int i = 0; i < trace_elem_count; ++i )
    {
        // assumes mangled symbol looks like program_name(asdfasdf-0xdeadbeef)
        const std::string symbol( stack_syms[i] );
        LOG_TRACE_MSG( "symbol: " << symbol );
        const std::string::size_type left_sentinel = symbol.find_first_of('(');
        if ( left_sentinel == std::string::npos ) {
            LOG_FATAL_MSG( stack_syms[i] );
            continue;
        }

        const std::string::size_type right_sentinel = symbol.find_first_of('-', left_sentinel);
        if ( right_sentinel == std::string::npos ) {
            LOG_FATAL_MSG( stack_syms[i] );
            continue;
        }

        // extract C++ function name
        const std::string mangled = symbol.substr( left_sentinel + 1, right_sentinel - left_sentinel - 1 );
        int status;
        char* realname = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status);
        if ( status == 0 ) {
            LOG_FATAL_MSG( realname );
            free(realname);
        } else {
            LOG_TRACE_MSG( "demangle failed: " << status );
            LOG_FATAL_MSG( stack_syms[i] );
        }
    }

    free( stack_syms );
}

} // runjob

