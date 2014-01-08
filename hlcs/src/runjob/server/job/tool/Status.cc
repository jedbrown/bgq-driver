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
#include "server/job/tool/Status.h"

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>

namespace runjob {
namespace server {
namespace job {
namespace tool {

std::ostream&
operator<<(
        std::ostream& os,
        const Status& status
        )
{
    if ( status == Status::Error ) {
        os << "Error";
    } else if ( status == Status::Running ) {
        os << "Running";
    } else if ( status == Status::Starting ) {
        os << "Starting";
    } else if ( status == Status::Ending ) {
        os << "Ending";
    } else if ( status == Status::Terminated ) {
        os << "Terminated";
    } else {
        os << "Invalid";
    }

    return os;
}

std::istream&
operator>>(
        std::istream& is,
        Status& status
        )
{
    std::string value;
    is >> value;

    if ( value == "Error" || value == "E" ) {
        status = Status::Error;
    } else if ( value == "Running" || value == "R" ) {
        status = Status::Running;
    } else if ( value == "Starting" ) {
        status = Status::Starting;
    } else if ( value == "Ending" ) {
        status = Status::Ending;
    } else if ( value == "Terminated" || value == "T" ) {
        status = Status::Terminated;
    } else {
        is.setstate( std::ios::failbit );
    }

    return is;
}

const char*
getDatabaseValue(
        Status s
        )
{
    if ( s == Status::Error ) {
        return "E";
    } else if ( s == Status::Running ) {
        return "R";
    } else if ( s == Status::Terminated ) {
        return "T";
    } else {
        BOOST_THROW_EXCEPTION( 
                std::logic_error( "no database status for " + boost::lexical_cast<std::string>(s) )
                );
    }
}

} // tool
} // job
} // server
} // runjob
