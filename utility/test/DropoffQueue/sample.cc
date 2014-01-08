/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2011                              */
/* Eclipse Public License (EPL)                                     */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include <iostream>
#include <ostream>
#include <string>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <utility/include/DropoffQueue.h>

// This is a not completely trivial example of how to use
// the dropoff queue.  It starts a number of threads to drop
// a number of messages into the queue and another thread to
// pick up those messages.  Note that because the ratio of
// dropoff threads to pickup threads is high, the dropoff
// threads will complete their work before the pickup thread
// is done.  This leaves a number of read messages on the
// queue.  You need to be aware of this if you are creating
// dropoff queues dynamically.  Memory is only cleaned up
// when the dropoff queus are destructed, clear() is called,
// or dropOff() is called.

// Let's use five threads for dropping off messages.
const unsigned int DROPOFF_THREADS = 5;
const unsigned int MESSAGES_PER_THREAD = 10;
const unsigned int TOTAL_MESSAGES = DROPOFF_THREADS * MESSAGES_PER_THREAD;

// Dropoff queue instance.
DropoffQueue<std::string> doq;

boost::barrier global_start_barrier(DROPOFF_THREADS + 1); // Plus one pickup thread.
boost::barrier global_done_barrier(DROPOFF_THREADS + 2); // Plus one pickup and main thread.
boost::detail::atomic_count global_msg_count(0);  // Atomic counter for generating message numbers.

boost::mutex cout_lock;

void locked_cout(std::string coutstring) {
    boost::mutex::scoped_lock sl(cout_lock);
    std::cout << coutstring;
}

void dropOffThread() {
    global_start_barrier.wait();
    for(unsigned int i = 0; i < MESSAGES_PER_THREAD; ++i) {
        std::ostringstream msg;
        msg << ++global_msg_count;
        doq.dropOff(msg.str());
        usleep(2000); // Rest a bit between dropoffs to give the pickUpThread an opportunity.
    }
    global_done_barrier.wait();
}

void pickUpThread() {
    global_start_barrier.wait();
    std::vector<std::string> gotten;
    
    while(gotten.size() < TOTAL_MESSAGES) {
        std::string str = "";
        doq.pickUp(str);
        if(str.length() != 0) {
            if(std::find(gotten.begin(), gotten.end(), str) == gotten.end()) {
                std::ostringstream out;
                out << "got message " << str << " of " << gotten.size() + 1 << " messages so far. " << std::endl;
                locked_cout(out.str());
                gotten.push_back(str);
            } else {
                std::ostringstream out;
                out << "GOT A DUPE: " << str << " Total gotten = " << gotten.size() << std::endl;
                locked_cout(out.str());
                 BOOST_FOREACH(std::string& curr, gotten) {
                     std::ostringstream m;
                     m << curr << std::endl;
                     locked_cout(m.str());
                 }
                exit(EXIT_FAILURE);
            }
            
        } else {
            std::ostringstream out;
            out << "doq empty: " << doq.size() <<  std::endl;
            locked_cout(out.str());
        }
        usleep(500);
    }
    std::ostringstream out;
    global_done_barrier.wait();
}

int main() {
    // Now let's start some threads and exercise this guy.
    // First, the pickup thread.
    boost::thread put(&pickUpThread);
    // Now the dropoff threads.
    for(unsigned int i = 0; i < DROPOFF_THREADS; ++i) {
        boost::thread dot(&dropOffThread);
    }

    // Wait for them all to be done.
    global_done_barrier.wait();
    std::cout << "Total messages sent=" << global_msg_count << std::endl;
    std::cout << "Number of messages on Dropoff Queue, both read and unread=" 
              << doq.size() << std::endl;
    std::cout << "Number of read items that have been removed from the Dropoff Queue=" 
              << doq.get_removed() << std::endl;
    std::cout << "Number of unread items on the Dropoff Queue=" << doq.get_unread() << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    std::cout << "Cleared " << doq.clearRead() << " remaining read messages." << std::endl;
    std::cout << "Number of messages on Dropoff Queue, both read and unread=" 
              << doq.size() << std::endl;
    std::cout << "Number of read items that have been removed from the Dropoff Queue=" 
              << doq.get_removed() << std::endl;

    return 0;
}
