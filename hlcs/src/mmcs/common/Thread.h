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
 * \file Thread.h
 */

#ifndef MMCS_COMMON_THREAD_H_
#define MMCS_COMMON_THREAD_H_


#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <string>

#include <pthread.h>


namespace mmcs {
namespace common {


/*!
 * \brief Base class for all MMCS threads.
 *
 * For interactive and database monitor threads in MMCS
 * To use, create a subclass and override the threadStart() function
 */
class Thread : private boost::noncopyable
{
public:
    typedef boost::shared_ptr<void> Args;

private:
    pthread_t thread_id;                    //!<
    pthread_attr_t attr;                    //!<
    char endThread;                         //!<
    Args arg;                               //!<
    std::string _threadName;                //!<
    bool deleteOnExit;                      //!< delete this object when the thread exits
    static void* threadExecute(void*);      //!<

public:
    Thread();
    virtual ~Thread();
    virtual void* threadStart() = 0;        // override this for specific thread start function
    void setJoinable(bool joinable);
    bool getJoinable() const;
    void setStacksize(size_t stacksize);
    bool getDeleteOnExit() const { return deleteOnExit; }
    void setDeleteOnExit(bool bDelete) { deleteOnExit = bDelete; }
    void start();
    void stop(int signal = -1);
    void wait();
    char isThreadStopping() const { return endThread; }

    template <typename T>
    void setArg(const boost::shared_ptr<T>& a) { arg = a; } // for passing arguments to thread

    template <typename T>
    void getArg(boost::shared_ptr<T>& a) const { a = boost::static_pointer_cast<T>(arg); } // retrieve thread arguments

    pthread_t getThreadId() const { return thread_id; } // 0 if thread is not running
    const std::string& getThreadName() const { return _threadName; }
    void setThreadName(const std::string& threadName) { _threadName = threadName; }
};

} } // namespace mmcs::common

#endif
