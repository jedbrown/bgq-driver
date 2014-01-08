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

#ifndef MASTER_LOCKING_RING_BUFFER_H_
#define MASTER_LOCKING_RING_BUFFER_H_


#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>

#include <algorithm>
#include <iterator>
#include <string>
#include <vector>


//! \brief This is just a locking version of boost's
//! ring buffer.  It is specialized for strings.
class LockingStringRingBuffer
{
    boost::circular_buffer<std::string> _ring_buff;
    boost::mutex _mutex;
public:
    LockingStringRingBuffer(unsigned buffsize) : _ring_buff(buffsize) {
        // Nothing to do.
    }
    void push_back(const std::string& item) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        _ring_buff.push_back(item);
    }

    //! \brief Get the contents of the ring buffer in a string vector.
    //! \parm messages The string vector to fill.
    void getContents(std::vector<std::string>& messages) {
        boost::mutex::scoped_lock scoped_lock(_mutex);
        std::copy(_ring_buff.begin(), _ring_buff.end(), std::back_inserter(messages) );
    }
};

#endif
