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

#include "server/job/CopyMappingFile.h"

#include "common/error.h"
#include "common/Exception.h"
#include "common/JobInfo.h"
#include "common/Mapping.h"
#include "common/logging.h"
#include "common/properties.h"

#include <utility/include/Properties.h>
#include <utility/include/ScopeGuard.h>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <cerrno>
#include <fstream>

#include <unistd.h> // unlink(2)

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job {
    
const std::string CopyMappingFile::KeyName( "mapping_file_archive" );

CopyMappingFile::CopyMappingFile(
        const BGQDB::job::Id id,
        const JobInfo& info,
        const bgq::utility::Properties::ConstPtr& properties
        ) :
    _id( id ),
    _copiedFilename( )
{
    const Mapping& mapping( info.getMapping() );
    if ( mapping.type() != Mapping::Type::File ) return;

    try {
        _copiedFilename = properties->getValue(PropertiesSection, KeyName);
        // fall through
    } catch ( const std::exception& e ) {
        // key not found, don't copy the mapping file
        LOG_ERROR_MSG( e.what() );
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "Key 'mapping_file_archive' in section [runjob.server] of properties file must be specified if using mapping files with runjob"
                );
    }

    if (_copiedFilename.empty()) {
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "A value for key 'mapping_file_archive' in section [runjob.server] of properties file must be specified if using mapping files with runjob"
                );
    }

    this->impl( mapping );
}

void
CopyMappingFile::impl(
        const Mapping& mapping
        )
{
    _copiedFilename += "/";
    _copiedFilename += boost::lexical_cast<std::string>(_id);
    LOG_DEBUG_MSG( "Writing output file to " << _copiedFilename );

    std::ofstream file( _copiedFilename, std::ios_base::trunc );
    if ( !file ) {
        char buf[256];
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "Could not open " << _copiedFilename << ": " << strerror_r(errno, buf, sizeof(buf)) 
                );
    }

    bgq::utility::ScopeGuard removeGuard(
            boost::bind( &unlink, _copiedFilename.c_str() )
            );

    file << "# copied from " << mapping.value() << std::endl;
    if ( !file ) {
        char buf[256];
        LOG_RUNJOB_EXCEPTION(
                error_code::mapping_file_invalid,
                "Could not write to " << _copiedFilename << ": " << strerror_r(errno, buf, sizeof(buf))
                );
    }

    size_t lineCount = 1;
    BOOST_FOREACH( const std::string i, mapping.fullMapFileContents() ) {
        file << i << std::endl;
        if ( !file ) {
            char buf[256];
            LOG_RUNJOB_EXCEPTION(
                    error_code::mapping_file_invalid,
                    "Could not write line " << lineCount << " of " << mapping.fullMapFileContents().size() <<
                    "to " << _copiedFilename << ": " << strerror_r(errno, buf, sizeof(buf))
                    );
        }

        ++lineCount;
    }

    LOG_DEBUG_MSG( "Wrote " << lineCount << " lines successfully" );
    removeGuard.dismiss();
}

} // job
} // server
} // runjob
