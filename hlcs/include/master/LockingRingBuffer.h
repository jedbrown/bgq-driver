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

#ifndef _LOCKING_RING_BUFFER_H
#define _LOCKING_RING_BUFFER_H

#include <string>
#include <boost/circular_buffer.hpp>

//! \brief This is just a locking version of boost's
//! ring buffer.  It is specialized for strings.
class LockingStringRingBuffer {
    boost::circular_buffer<std::string> _ring_buff;
    boost::mutex _mutex;
public:
    LockingStringRingBuffer(int buffsize) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        boost::circular_buffer<std::string> b(buffsize);
        _ring_buff = b;
    }
    void push_back(std::string item) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _ring_buff.push_back(item);
    }

    void pop_front() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _ring_buff.pop_front();
    }

    std::string& front() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _ring_buff.front();
    }

    std::string at(size_t index) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _ring_buff.at(index);
    }

    bool empty() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _ring_buff.empty();
    }

    size_t size() {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        return _ring_buff.size();
    }

    //! \brief Get the contents of the ring buffer in a string vector.
    //! \parm messages The string vector to fill.
    void getContents(std::vector<std::string>& messages) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        unsigned buff_size = _ring_buff.size();
        for(unsigned int i = 0; i < buff_size; ++i) {
            messages.push_back(_ring_buff.at(i));
        }
    }
};
#endif
