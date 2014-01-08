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
 * \file MMCSThread.h
 */

#ifndef _MMCSTHREAD_H
#define _MMCSTHREAD_H

#include <string>
#include <boost/utility.hpp>

/*!
 * \brief Base class for all MMCS threads.
 *
 * For interactive and database monitor threads in MMCS
 * To use, create a subclass and override the threadStart() function
 */
class MMCSThread : private boost::noncopyable
{
private:
    pthread_t thread_id;                    //!<
    pthread_attr_t attr;                    //!<
    char endThread;                         //!<
    void *arg;                              //!<
    std::string _threadName;                //!<
    bool deleteOnExit;                      //!< delete this object when the thread exits
    static void* threadExecute(void*);      //!<

public:
    MMCSThread();
    virtual ~MMCSThread();
    virtual void* threadStart();        // override this for specific thread start function
    void setJoinable(bool joinable);
    bool getJoinable();
    void setStacksize(size_t stacksize);
    bool getDeleteOnExit() { return deleteOnExit; }
    void setDeleteOnExit(bool bDelete) { deleteOnExit = bDelete; }
    void start();                       // start the thread
    void stop(int signal = -1);
    void wait();
    //    void stop(bool cancel=false, int signal=-1);                        // stop the thread (synchronously)
    char isThreadStopping() { return endThread; }       // test for thread termination
    void setArg(void* newArg) { arg = newArg; } // for passing arguments to thread
    void* getArg() { return arg; } // retrieve thread arguments
    pthread_t getThreadId() { return thread_id; } // 0 if thread is not running
    std::string& getThreadName() { return _threadName; }
    void setThreadName(std::string threadName) { _threadName = threadName; }
    static MMCSThread* getMyThread(); // get the MMCSThread pointer from thread-specific data
};

#endif
