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
#include "common/ExportedEnvironment.h"

#include <cstdlib>

namespace runjob {

ExportedEnvironment::ExportedEnvironment(
        const std::string& key,
        const std::string& value
        ) :
    Environment( key, value )
{

}

std::ostream&
operator<<(
        std::ostream& stream,
        const ExportedEnvironment& env
        )
{
    stream << env.getKey() << "=" << env.getValue();
    return stream;
}

std::istream&
operator>>(
        std::istream& stream,
        ExportedEnvironment& env
        )
{
    std::string name;
    stream >> name;
    if ( getenv(name.c_str() ) == NULL ) {
        std::cerr << "environment '" << name << "' missing value." << std::endl;
        stream.setstate( std::ios::failbit );
    } else {
        env = ExportedEnvironment( 
                name, 
                getenv( name.c_str() )
                );
    }

    return stream;
}

}
