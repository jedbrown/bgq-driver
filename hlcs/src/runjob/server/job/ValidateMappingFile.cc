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

#include "server/job/ValidateMappingFile.h"

#include "common/Exception.h"
#include "common/JobInfo.h"
#include "common/logging.h"

#include <boost/numeric/conversion/cast.hpp>
#include <boost/system/system_error.hpp>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {

ValidateMappingFile::ValidateMappingFile(
        const BGQDB::job::Id id,
        const JobInfo& info,
        std::istream& file,
        const BG_JobCoords_t& size
        ) :
    _info( info ),
    _ranks(),
    _size( size )
{
    LOGGING_DECLARE_JOB_MDC( id );

    unsigned lineno = 0;
    std::string line;
    while( std::getline(file, line) ) {
        try {
            this->analyzeLine( lineno, line );
            ++lineno;
        } catch( const std::exception& e ) {
            LOG_WARN_MSG( e.what() );
            LOG_WARN_MSG( lineno << ": " << line );
            throw;
        }
    }

    // ensure mapping has enough ranks for the job
    if ( _ranks.size() < static_cast<unsigned>(info.getNp()) ) {
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "job requires " << info.getNp() << " ranks, " <<
                "but mapping only has " << _ranks.size() 
                );
    }
}

void
ValidateMappingFile::analyzeLine(
        const unsigned number,
        std::string& line
        )
{
    // strip comments
    const std::size_t comment = line.find_first_of('#');
    if ( comment != std::string::npos ) {
        line.erase( comment );
        if ( line.empty() ) return;
    }

    LOG_TRACE_MSG( "line: " << line );

    std::istringstream is( line );
    int coords[6];
    const char names[] = {'A','B','C','D','E','T'};

    for ( unsigned i = 0; i < 6; ++i ) {
        is >> coords[i];
        if ( !is ) {
            LOG_RUNJOB_EXCEPTION(
                    error_code::mapping_file_invalid,
                    (is.eof() ? "Missing" : "Garbage") << " " << names[i] << 
                    " coordinate on line " << number
                    );
        }
    }

    // line was valid, now see if it fits within the job
    for ( unsigned i = 0; i < 6; ++i ) {
        uint8_t max;
        switch( i ) {
            case 0: max = _size.shape.a; break;
            case 1: max = _size.shape.b; break;
            case 2: max = _size.shape.c; break;
            case 3: max = _size.shape.d; break;
            case 4: max = _size.shape.e; break;
            case 5: max = boost::numeric_cast<uint8_t>(_info.getRanksPerNode().getValue()); break;
            default: max = 0; break;
        }
        if ( coords[i] >= max ) {
            LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                names[i] << " coordinate on line " << number <<
                " is greater than job size of " << (unsigned)max
                );
        } else if ( coords[i] < 0 ) {
            LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                names[i] << " coordinate on line " << number <<
                " must be zero or greater"
                );
        }
    }

    // collapse coordinates into a single integer, need 6 bits for A, B, C, D, and T dimensions, 
    // but only one bit for E dimension
    const uint32_t nodeId = 
        ((coords[0] & 0x3F) << 26) | 
        ((coords[1] & 0x3F) << 20) | 
        ((coords[2] & 0x3F) << 14) |
        ((coords[3] & 0x3F) << 8 ) |
        ((coords[4] & 0x01) << 7 ) |
         (coords[5] & 0x3F)
         ;

    // ensure these coordinates do not exist in the mapping already
    const Rank::iterator result = std::find(
            _ranks.begin(),
            _ranks.end(),
            nodeId
            );
    if ( result != _ranks.end() ) {
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "Coordinates (" <<
                coords[0] << "," <<
                coords[1] << "," <<
                coords[2] << "," <<
                coords[3] << "," <<
                coords[4] << "," <<
                coords[5] << ") on line " << number <<
                " already exists for rank " << std::distance( _ranks.begin(), result )
                );
    }

    _ranks.push_back( nodeId );

    LOG_TRACE_MSG(
            number << " (" <<
            coords[0] << "," <<
            coords[1] << "," <<
            coords[2] << "," <<
            coords[3] << "," <<
            coords[4] << "," <<
            coords[5] << ") 0x" <<
            std::hex << nodeId
            );
}

} // job
} // server
} // runjob
