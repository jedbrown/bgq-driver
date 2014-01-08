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
 * \file MMCSThread.cc
 */

#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include "MMCSThread.h"

using namespace std;

// key for thread-specific pointer to the current thread's MMCSThread object
static pthread_key_t mmcsThread_key;

// once-only initialization of the MMCSThread key
static pthread_once_t mmcsThread_key_once = PTHREAD_ONCE_INIT;

// Allocate the mmcsThread_key
static void mmcsThread_once_init()
{
    pthread_key_create(&mmcsThread_key, NULL);
}

// get the MMCSThread pointer from thread-specific data
MMCSThread*
MMCSThread::getMyThread()
{
    MMCSThread* myThread = (MMCSThread*) pthread_getspecific(mmcsThread_key);
    return myThread;
}

// constructor
MMCSThread::MMCSThread()
    : thread_id(0), arg(NULL), _threadName(""), deleteOnExit(false)
{
    pthread_attr_init(&attr);	// initialize pthread attributes

    // initialize the thread-specific storage key
    pthread_once(&mmcsThread_key_once, mmcsThread_once_init);
}


// destructor
MMCSThread::~MMCSThread()
{
    pthread_attr_destroy(&attr);
}

// To use, create a subclass and override the threadStart() function
void*
MMCSThread::threadStart()
{
    return NULL;
}

// set/get thread attributes
void
MMCSThread::setJoinable(bool joinable)
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
MMCSThread::getJoinable()
{
    int joinable_attr;
    pthread_attr_getdetachstate(&attr, &joinable_attr);
    return joinable_attr == PTHREAD_CREATE_JOINABLE;
}

void
MMCSThread::setStacksize(size_t stacksize)
{
    if (thread_id == 0)		// can't modify after thread is started
	pthread_attr_setstacksize (&attr, stacksize);

}

// must be declared static for use with pthread_create()
void*
MMCSThread::threadExecute(void* this_p)
{
    void* returnVal;		// return value from thread

    // save a pointer to our MMCSThread object in thread-specific storage
    // so that get MyThread can retrieve it
    pthread_setspecific(mmcsThread_key, this_p);

    ((MMCSThread*)this_p)->thread_id = pthread_self();	// mark thread as started
    returnVal = ((MMCSThread*)this_p)->threadStart();
    ((MMCSThread*)this_p)->thread_id = 0;	// mark thread as ended
    if (((MMCSThread*)this_p)->getDeleteOnExit())
	delete (MMCSThread*)this_p;
    return returnVal;
}

// Start a thread
void
MMCSThread::start()
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

void MMCSThread::wait() {
    pthread_t id = thread_id;
    if(getJoinable() == true) {
        if (pthread_join(id, 0)) {
            perror("pthread_join");
        }
    }
}

// Set a stop flag and wait for the thread to end
void
MMCSThread::stop(int signo)
{
    // copy thread ID into local variable in case thread
    // exits before we can cancel or signal it
    pthread_t id = thread_id;

    if (id != 0) {
        // put mark on wall so thread knows it's time to stop
        endThread = 1;

        // cancel thread if asked
        // if (cancel) {

        // }

        // signal thread if asked
        if (signo != -1) {
            pthread_kill(id, signo);
        }

        // join thread if we can
        if (getJoinable() == true) {
            // can't join if thread not joinable
            if (pthread_join(id, 0)) {
                perror("pthread_join");
            }
        }
    }

    // reset thread ID
    thread_id = 0;
}
