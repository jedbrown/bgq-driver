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
#include "common/WorkingDir.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/filesystem/path.hpp>

#include <iostream>

namespace runjob {

WorkingDir::WorkingDir(
        const std::string& value
        ) :
    _value( value )
{

}

std::ostream&
operator<<(
        std::ostream& stream,
        const WorkingDir& cwd
        )
{
    return stream << cwd.get();
}

std::istream&
operator>>(
        std::istream& stream,
        WorkingDir& cwd
        )
{
    std::string value;
    stream >> value;
    if ( !stream ) return stream;

    // ensure size
    if ( value.size() >= static_cast<unsigned>(bgcios::jobctl::MaxPathSize) ) {
        std::cerr <<
            "path '" << value << "' is longer than maximum size of " << 
            bgcios::jobctl::MaxPathSize << " characters" << 
            std::endl
            ;

        stream.setstate( std::ios::failbit );
        return stream;
    }

    // ensure complete path
    if ( !boost::filesystem::path(value).is_complete() ) {
        std::cerr <<
            "path '" << value << " is not a fully qualified path" <<
            std::endl;

        stream.setstate( std::ios::failbit );
        return stream;
    }

    cwd = WorkingDir( value );

    return stream;
}

} // runjob

