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
#include "server/sim/SharedMemory.h"

#include "common/defaults.h"
#include "common/properties.h"

#include "server/sim/Counter.h"
#include "server/Options.h"

#include <utility/include/Log.h>
#include <utility/include/ScopeGuard.h>

#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/bind.hpp>

#include <sys/types.h>

LOG_DECLARE_FILE("runjob.server");

namespace runjob {
namespace server {
namespace sim {

SharedMemory::SharedMemory(
        const Options& options
        ) :
    _options(options),
    _mapped_file(),
    _value(NULL)
{
    if ( !_options.getSim() ) {
        return;
    }

    // get shared memory name from properties file
    std::string name = defaults::ServerIosdIdCounterName;
    try {
        name = _options.getProperties()->getValue(PropertiesSection, "iosd_id_counter_name");
        LOG_INFO_MSG( "using shared memory name '" << name << "'" );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( e.what() );
        LOG_WARN_MSG( "using default value " << name );
    }
    
    // get shared memory size from properties file
    size_t size = defaults::ServerIosdIdCounterSize;
    try {
        size = boost::lexical_cast<size_t>(
                _options.getProperties()->getValue(PropertiesSection, "iosd_id_counter_size")
                );
        LOG_INFO_MSG( "using shared memory size '" << size << "'" );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( e.what() );
        LOG_WARN_MSG( "using default value " << size );
    } catch (const boost::bad_lexical_cast& e) {
        LOG_ERROR_MSG( "bad iosd_id-counter-size key in runjob.server section of properties file" );
        throw;
    }

    try {
        // set umask so anyone can write to shared memory file when we create it
        const mode_t access = 0777;
        const mode_t old_umask = umask( ~(access & 0777) );
        LOG_DEBUG_MSG( "old umask " << std::oct << std::setfill('0') << std::setw(4) << old_umask );
        bgq::utility::ScopeGuard mask_guard( boost::bind(&umask, old_umask) );

        // create shared memory file
        _mapped_file.reset(
                new boost::interprocess::managed_mapped_file(
                    boost::interprocess::open_or_create,
                    name.c_str(),
                    size
                    )
                );

        // create counter
        _value = _mapped_file->find_or_construct<Counter>(boost::interprocess::unique_instance)();

        // acquire lock and get counter value
        const boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(_value->_mutex);
        LOG_DEBUG_MSG( "counter value " << _value->_count );
    } catch ( const boost::interprocess::interprocess_exception& e ) {
        LOG_FATAL_MSG( "could not create shared memory region: " << e.what() );
        LOG_FATAL_MSG( e.get_native_error() );
        throw;
    }
}

uint32_t
SharedMemory::increment()
{
    // acquire lock
    boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(_value->_mutex);

    // get value
    uint32_t result = ++(_value->_count);

    return result;
}

SharedMemory::~SharedMemory()
{
    // note we don't invoke boost::interprocess::destroy<Counter>() here because we want
    // the object to persist in shared memory across runjob_server sessions
}

} // sim
} // server
} // runjob
