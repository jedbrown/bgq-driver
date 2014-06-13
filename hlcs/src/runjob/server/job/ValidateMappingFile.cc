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
    _size( size )
{
    LOGGING_DECLARE_JOB_MDC( id );

    /*
    for (
            std::vector<std::string>::const_iterator i = info.getMapping().fullMapFileContents().begin();
            i != info.getMapping().fullMapFileContents().end();
            ++i
        )
    {
        LOG_INFO_MSG("Map file content " << *i);
    }
    */

    // calculate maximum size in every dimension
    typedef std::pair<uint8_t, uint32_t> Max; // size and line number
    std::vector<Max> mappingMax( 6, std::make_pair(0,0) );

    for ( 
            std::vector<uint32_t>::const_iterator i = info.getMapping().fileContents().begin();
            i != info.getMapping().fileContents().end();
            ++i
        )
    {
        const size_t lineNumber = std::distance( info.getMapping().fileContents().begin(), i ) + 1;
        const uint8_t a = boost::numeric_cast<uint8_t>((*i & (0x3F << 26)) >> 26);
        const uint8_t b = boost::numeric_cast<uint8_t>((*i & (0x3F << 20)) >> 20);
        const uint8_t c = boost::numeric_cast<uint8_t>((*i & (0x3F << 14)) >> 14);
        const uint8_t d = boost::numeric_cast<uint8_t>((*i & (0x3F << 8)) >> 8);
        const uint8_t e = boost::numeric_cast<uint8_t>((*i & (1 << 7)) >> 7);
        const uint8_t t = boost::numeric_cast<uint8_t>(*i & 0x3F);
        LOG_DEBUG_MSG(
                "a: " << static_cast<unsigned>(a) << " " <<
                "b: " << static_cast<unsigned>(b) << " " <<
                "c: " << static_cast<unsigned>(c) << " " <<
                "d: " << static_cast<unsigned>(d) << " " <<
                "e: " << static_cast<unsigned>(e) << " " <<
                "t: " << static_cast<unsigned>(t)
                );
        if ( a > mappingMax[0].first ) mappingMax[0] = std::make_pair(a, lineNumber);
        if ( b > mappingMax[1].first ) mappingMax[1] = std::make_pair(b, lineNumber);
        if ( c > mappingMax[2].first ) mappingMax[2] = std::make_pair(c, lineNumber);
        if ( d > mappingMax[3].first ) mappingMax[3] = std::make_pair(d, lineNumber);
        if ( e > mappingMax[4].first ) mappingMax[4] = std::make_pair(e, lineNumber);
        if ( t > mappingMax[5].first ) mappingMax[5] = std::make_pair(t, lineNumber);
    }

    const char names[] = {'A','B','C','D','E','T'};

    // see if it fits within the job
    for ( unsigned i = 0; i < 6; ++i ) {
        uint8_t jobMax;
        switch( i ) {
            case 0: jobMax = _size.shape.a; break;
            case 1: jobMax = _size.shape.b; break;
            case 2: jobMax = _size.shape.c; break;
            case 3: jobMax = _size.shape.d; break;
            case 4: jobMax = _size.shape.e; break;
            case 5: jobMax = boost::numeric_cast<uint8_t>(info.getRanksPerNode().getValue()); break;
            default: jobMax = 0; break;
        }
        if ( mappingMax[i].first >= jobMax ) {
            LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                names[i] << " coordinate on line " << mappingMax[i].second <<
                " exceeds the " << names[i] << " dimension range of " << static_cast<unsigned>(jobMax) <<
                " for the job"
                );
        }
    }
}

} // job
} // server
} // runjob
