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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

/*!
 * \file Thread.cc
 */

#include "Thread.h"

#include <utility/include/Log.h>

#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstring>


LOG_DECLARE_FILE( "mmcs.common" );

namespace mmcs {
namespace common {

Thread::Thread() :
    thread_id(0),
    arg(),
    _threadName(),
    deleteOnExit(false)
{
    pthread_attr_init(&attr);	// initialize pthread attributes
}

Thread::~Thread()
{
    pthread_attr_destroy(&attr);
}

void
Thread::setJoinable(const bool joinable)
{
    if (thread_id == 0) {
        // can't modify after thread is started
	if (joinable)
	    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	else
	    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }
}

bool
Thread::getJoinable() const
{
    int joinable_attr;
    pthread_attr_getdetachstate(&attr, &joinable_attr);
    return joinable_attr == PTHREAD_CREATE_JOINABLE;
}

void
Thread::setStacksize(const size_t stacksize)
{
    if (thread_id == 0)		// can't modify after thread is started
	pthread_attr_setstacksize (&attr, stacksize);
}

void*
Thread::threadExecute(void* this_p)
{
    void* returnVal;		// return value from thread

    Thread* const this_thread = static_cast<Thread*>(this_p);

    this_thread->thread_id = pthread_self();	// mark thread as started
    try {
        returnVal = this_thread->threadStart();
    } catch ( const std::exception& e ) {
        returnVal = 0;
        LOG_WARN_MSG( e.what() );
    }
    this_thread->thread_id = 0;	// mark thread as ended
    if ( this_thread->getDeleteOnExit() )
        delete this_thread;
    return returnVal;
}

void
Thread::start()
{
    int status;
    endThread = 0;
    if (thread_id == 0)
    {
	while (1)
	{
	    status = pthread_create(&thread_id, &attr, &threadExecute, this);
	    if (status == 0 || (errno != EINTR && errno != EAGAIN))
		break;
	    else
		sleep(5);
	}
	if (status != 0)
	    perror("pthread_create");
    }
}

void
Thread::wait()
{
    pthread_t id = thread_id;
    if(getJoinable() == true) {
        const int rc = pthread_join(id, 0);
        if (rc) {
            char buf[256];
            LOG_ERROR_MSG("pthread_join error:" << strerror_r(rc, buf, sizeof(buf)) );
        }
    }
}

void
Thread::stop(const int signo)
{
    // copy thread ID into local variable in case thread
    // exits before we can signal it
    const pthread_t id = thread_id;

    if (id != 0) {
        // put mark on wall so thread knows it's time to stop
        endThread = 1;

        // signal thread if asked
        if (signo != -1) {
            pthread_kill(id, signo);
        }

        // join thread if we can
        if ( getJoinable() ) {
            // can't join if thread not joinable
            const int rc = pthread_join(id, 0);
            if (rc) {
                char buf[256];
                LOG_ERROR_MSG("pthread_join error:" << strerror_r(rc, buf, sizeof(buf)) );
            }
        }
    }

    // reset thread ID
    thread_id = 0;
}

} } // namespace mmcs::common
