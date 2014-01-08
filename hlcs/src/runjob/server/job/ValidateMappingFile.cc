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
        const BG_JobCoords_t& size
        ) :
    _info( info ),
    _size( size )
{
    LOGGING_DECLARE_JOB_MDC( id );

    // ensure mapping has enough ranks for the job
    if ( info.getMapping().lineCount() < static_cast<unsigned>(info.getNp()) ) {
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "job requires " << info.getNp() << " ranks, " <<
                "but mapping only has " << info.getMapping().lineCount() 
                );
    }
    
    const char names[] = {'A','B','C','D','E','T'};

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
        if ( info.getMapping().dimensions()[i].first >= max ) {
            LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                names[i] << " coordinate on line " << info.getMapping().dimensions()[i].second <<
                " is greater than job size of " << (unsigned)max
                );
        }
    }
}

} // job
} // server
} // runjob
