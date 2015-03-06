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
#include "server/mux/InsertJob.h"

#include "common/message/InsertJob.h"
#include "common/message/Result.h"

#include "common/logging.h"

#include "server/block/Container.h"
#include "server/block/Compute.h"

#include "server/job/Container.h"
#include "server/job/Create.h"

#include "server/mux/Connection.h"
#include "server/mux/ClientContainer.h"

#include "common/Message.h"

#include "server/Job.h"
#include "server/Options.h"
#include "server/Ras.h"
#include "server/Security.h"

#include <db/include/api/BGQDBlib.h>

#include <boost/assign/list_of.hpp>
#include <boost/make_shared.hpp>

#include <map>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace mux {

InsertJob::Ptr
InsertJob::create()
{
    const Ptr result(
            new InsertJob( )
            );

    return result;
}

InsertJob::InsertJob() :
    _server(),
    _mux(),
    _message(),
    _clients(),
    _hostname(),
    _shortHostname()
{

}

void
InsertJob::start()
{
    BOOST_ASSERT( _clients );
    BOOST_ASSERT( _message );
    BOOST_ASSERT( _mux );
    BOOST_ASSERT( _server );

    // set hostname so we know where this came from
    _message->getJobInfo().setHostname( _hostname );

    _clients->add(
            _message->getClientId(),
            boost::bind(
                &InsertJob::addClientHandler,
                shared_from_this()
                )
            );
}

void
InsertJob::addClientHandler()
{
    _server->getBlocks()->find(
            _message->getJobInfo().getBlock(),
            boost::bind(
                &InsertJob::findBlockHandler,
                shared_from_this(),
                _1
                )
            );
}

void
InsertJob::blockNotFound()
{
    const message::Result::Ptr msg( new message::Result() );

    const JobInfo& info = _message->getJobInfo();
    LOG_WARN_MSG( "could not find block '" << info.getBlock() << "'" );
    BGQDB::BLOCK_STATUS status;
    const BGQDB::STATUS result = BGQDB::getBlockStatus( info.getBlock(), status );
    if ( result == BGQDB::OK ) {
        msg->setMessage(
                std::string("block '" + info.getBlock() + "' with status ") +
                this->getBlockStatusString( status ) + " is not ready to run jobs"
                );
        msg->setError( error_code::block_invalid );
    } else {
        msg->setError( error_code::block_not_found );
    }

    msg->setClientId( _message->getClientId() );
    _mux->write( msg );

    // add RAS event if block is initialized
    if ( status == BGQDB::INITIALIZED ) {
        /*
           <rasevent
           id="00062004" 
           category="Block" 
           component="MMCS"
           severity="WARN"
           message="A job was submitted to block $(BG_BLOCKID) that is not ready to run jobs."
           description="A job was submitted to a block that is not ready to run jobs."
           service_action="$(CheckLevels)"
           />
         */

        Ras::create( Ras::BlockNotReady ).
            block( info.getBlock() )
            ;
    }
}

void
InsertJob::findBlockHandler(
        const block::Compute::Ptr& block
        )
{
    const JobInfo& info = _message->getJobInfo();

    if ( block ) {
    	// Block was found, continue processing the block.
    	this->findBlockHandler_4( block );
    } else {
    	// Block not found in block container. Check if block is in initialized state.
        BGQDB::BLOCK_STATUS status;
        const BGQDB::STATUS result = BGQDB::getBlockStatus( info.getBlock(), status );
        if ( result == BGQDB::OK ) {
        	if ( status != BGQDB::INITIALIZED ) {
            	// Not in initialized state, call into error processing.
                this->blockNotFound();
                return;
        	}
        } else {
        	// Could not get block status, call into error processing.
            this->blockNotFound();
            return;
        }

    	// Block not found in block container but is in initialized state. Sleep 2 seconds and try the find again.
    	// The block container list should be updated as blocks are initialized.
        LOG_WARN_MSG( "Could not find block '" << info.getBlock() <<
        		"' in Initialized list, waiting 2 seconds and checking again." );
        sleep(2);
        _server->getBlocks()->find(
                _message->getJobInfo().getBlock(),
                boost::bind(
                    &InsertJob::findBlockHandler_2,
                    shared_from_this(),
                    _1
                    )
                );
    }
    return;
}

void
InsertJob::findBlockHandler_2(
        const block::Compute::Ptr& block
        )
{
    const JobInfo& info = _message->getJobInfo();

    if ( block ) {
    	// Block was found, continue processing the block.
    	this->findBlockHandler_4( block );
    } else {
    	// Get the retry value from properties.
    	setInitializedRetryDelay();
    	// Block not found in block container. Previous attempt would have already checked if block is in
    	// initialized state. Sleep X seconds and try the find again.
        LOG_WARN_MSG( "Could not find block '" << info.getBlock() << "' in Initialized list, waiting "
        		<< _initialized_retry_delay << " seconds and checking again." );
        sleep(_initialized_retry_delay);
        _server->getBlocks()->find(
                _message->getJobInfo().getBlock(),
                boost::bind(
                    &InsertJob::findBlockHandler_3,
                    shared_from_this(),
                    _1
                    )
                );
    }
    return;
}

void
InsertJob::findBlockHandler_3(
        const block::Compute::Ptr& block
        )
{
    const JobInfo& info = _message->getJobInfo();

    if ( block ) {
    	// Block was found, continue processing the block.
    	this->findBlockHandler_4( block );
    } else {
    	// Block not found in block container. Previous attempt would have already checked if block is in
    	// initialized state. Sleep X seconds and try the find again.
        LOG_WARN_MSG( "Could not find block '" << info.getBlock() << "' in Initialized list, waiting "
        		<< _initialized_retry_delay << " seconds and checking again." );
        sleep(_initialized_retry_delay);
        _server->getBlocks()->find(
                _message->getJobInfo().getBlock(),
                boost::bind(
                    &InsertJob::findBlockHandler_4,
                    shared_from_this(),
                    _1
                    )
                );
    }
    return;
}
void
InsertJob::findBlockHandler_4(
        const block::Compute::Ptr& block
        )
{
    const JobInfo& info = _message->getJobInfo();

    LOGGING_DECLARE_LOCATION_MDC( _shortHostname );
    LOGGING_DECLARE_JOB_MDC( _message->getClientId() );

    if ( !block ) {
        this->blockNotFound();
        return;
    }

    // now that the block has been found, add its name as an MDC
    LOGGING_DECLARE_BLOCK_MDC( info.getBlock() );

    // perform security enforcement to ensure this user can execute a job
    // on the block

    const performance::Counters::Ptr counters( _server->getPerformanceCounters() );
    const performance::Counters::JobContainer::Timer::Ptr timer(
            counters->getJobs()->create()
            ->function( "server" )
            ->subFunction( "security" )
            ->mode( bgq::utility::performance::Mode::Value::Basic )
            );
    timer->dismiss();
   
    _server->getSecurity()->executeBlock(
                info.getUserId(),
                info.getBlock(),
                boost::bind(
                    &InsertJob::securityHandler,
                    shared_from_this(),
                    _1,
                    timer,
                    block
                    )
                );
}

void
InsertJob::securityHandler(
        const bool result,
        const performance::Counters::JobContainer::Timer::Ptr& timer,
        const block::Compute::Ptr& block
        )
{
    const JobInfo& info = _message->getJobInfo();

    if ( !result ) {
        const message::Result::Ptr result( new message::Result() );
        result->setClientId( _message->getClientId() );
        result->setError( error_code::permission_denied );
        result->setMessage(
                "user " +
                info.getUserId()->getUser() + 
                " does not have execute authority on block " +
                info.getBlock()
                );
        _mux->write( result );
        return;
    }
   
    // stop timer since security enforcement has completed but don't
    // un-dismiss the timer until arbitration is successful
    timer->stop();

    // this is the named parameter idiom
    const boost::shared_ptr<job::Create> job(
            boost::make_shared<job::Create>( _message->getJobInfo() )
            );
    job->
        server( _server )->
        client_id( _message->getClientId() )->
        mux_connection( _mux )->
        compute_block( block )->
        security( timer )
        ;

    // start arbitration on the compute block
    block->arbitrate( job );
}

const std::string&
InsertJob::getBlockStatusString(
        const BGQDB::BLOCK_STATUS status
        ) const
{
    typedef std::map<BGQDB::BLOCK_STATUS, std::string> Map;
    static Map map = boost::assign::map_list_of
        ( BGQDB::FREE, "Free" )
        ( BGQDB::ALLOCATED, "Allocated" )
        ( BGQDB::INITIALIZED, "Initialized" )
        ( BGQDB::BOOTING, "Booting" )
        ( BGQDB::TERMINATING, "Terminating" )
        ( BGQDB::INVALID_STATE, "Invalid" )
        ;
    const Map::const_iterator result = map.find( status );
    if ( result != map.end() ) {
        return result->second;
    } else {
        return map.at( BGQDB::INVALID_STATE );
    }
}


void
InsertJob::setInitializedRetryDelay()
{
	// Set default value.
	_initialized_retry_delay = 5;
	int result;
    const std::string initialized_retry_delay_key( "initialized_retry_delay" );
    const std::string runjob_server_section( "runjob.server");
    const bgq::utility::Properties::ConstPtr& properties = _server->getOptions().getProperties();
    // If couldn't get properties, use default value.
    if ( !properties ) {
    	return;
    }
    try {
        result = boost::lexical_cast<int>(
                properties->getValue(runjob_server_section, initialized_retry_delay_key)
                );
        // Make sure result is in range of 1 - 60.
        if ( result < 1 || result > 60 ) {
            LOG_INFO_MSG( "Value for key " << initialized_retry_delay_key << " out of range (1 - 60): "
            		<< result << ".  Using default value: " << _initialized_retry_delay << ".");
        } else {
        	// Value is found, valid, and in range, use it.
        	_initialized_retry_delay = result;
            LOG_INFO_MSG(initialized_retry_delay_key << " key found in "
            		<< runjob_server_section <<
            		" section of properties.  Using specified value: " << _initialized_retry_delay << ".");
        }
    } catch ( const boost::bad_lexical_cast& e ) {
    	LOG_INFO_MSG("Invalid " << initialized_retry_delay_key << " value: " << e.what()
    			<< ".  Must be numeric.  Using default value: " << _initialized_retry_delay << "." );
    } catch ( const std::invalid_argument& e ) {
    }

}

} // mux
} // server
} // runjob
