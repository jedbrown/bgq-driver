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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include <iostream>
#include <ostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include "DropoffQueue.h"

const int DROPOFF_THREADS = 50;
DropoffQueue<std::string> doq;

boost::barrier global_start_barrier(DROPOFF_THREADS + 1); // Plus one pickup thread.
boost::barrier global_done_barrier(DROPOFF_THREADS + 2); // All done.
boost::detail::atomic_count global_msg_count(0);
boost::mutex cout_lock;

void locked_cout(std::string coutstring) {
    boost::mutex::scoped_lock sl(cout_lock);
    std::cout << coutstring;
}

void locked_cout_cstr(const char* coutstring) {
    boost::mutex::scoped_lock sl(cout_lock);
    std::cout << coutstring;
}

void dropOffThread() {
    global_start_barrier.wait();
    std::cout << "DO thread start " << pthread_self() << std::endl;
    for(int i = 0; i < 100; ++i) {
        std::ostringstream msg;
        msg << ++global_msg_count;
        doq.dropOff(msg.str());
    }
    std::cout << "DO thread end " << pthread_self() << std::endl;
    global_done_barrier.wait();
}

void pickUpThread() {
    global_start_barrier.wait();
    int total_attempts = 0;
    std::vector<std::string> gotten;

    while(total_attempts < ((DROPOFF_THREADS * 100) + 5)) {
        std::string str = "";
        doq.pickUp(str);
        if(str.length() != 0) {
            if(std::find(gotten.begin(), gotten.end(), str) == gotten.end()) {
                std::ostringstream out;
                out << "got " << str << " in length " << gotten.size() << " from " << &gotten << std::endl;
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
        ++total_attempts;
        usleep(500);
    }
    std::ostringstream out;
    out << "Total attempts=" << total_attempts << std::endl;
    locked_cout(out.str());
    global_done_barrier.wait();
}

int main() {
    for(int i = 1; i <= 10; ++i) {
        std::ostringstream msg;
        msg << i;
        doq.dropOff(msg.str());
    }
    
    for(int i = 1; i <= 10; ++i) {
        std::string p = "";
        doq.pickUp(p);
        std::cout << p << std::endl;
    }
    doq.printChain();
    doq.clear();
    doq.printChain();

    // Now let's start some threads and exercise this guy.
    // First, the pickup thread.
    boost::thread put(&pickUpThread);
    for(int i = 0; i < DROPOFF_THREADS; ++i) {
        boost::thread dot(&dropOffThread);
    }

    global_done_barrier.wait();
    std::cout << "----------------After Barrier------------------" << std::endl;
    std::cout << "Total messages=" << global_msg_count << std::endl;
    std::cout << "DoQ size=" << doq.size() << std::endl;
    std::cout << "Removed items=" << doq.get_removed() << std::endl;
 
    std::cout << "----------------After Read Clear------------------" << std::endl;
    std::cout << "Cleared " << doq.clearRead() << " read nodes." << std::endl;
    std::cout << "DoQ size=" << doq.size() << std::endl;
    std::cout << "Removed items=" << doq.get_removed() << std::endl;
    std::cout << "----------------After All Clear------------------" << std::endl;
    doq.clear();
    std::cout << "Cleared " << doq.clearRead() << " nodes." << std::endl;
    std::cout << "DoQ size=" << doq.size() << std::endl;
    std::cout << "Removed items=" << doq.get_removed() << std::endl;

    return 0;
}
