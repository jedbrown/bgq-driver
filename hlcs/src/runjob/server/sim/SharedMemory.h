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
#ifndef RUNJOB_SERVER_SIM_SHARED_MEMORY_H
#define RUNJOB_SERVER_SIM_SHARED_MEMORY_H

#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/scoped_ptr.hpp>

namespace runjob {
namespace server {

class Options;

namespace sim {

class Counter;

/*!
 * \brief Wrapper around Boost.Interprocess objects for sharing memory between processes.
 *
 * Each runjob_server process instantiates a single SharedMemory object and attempts to
 * open or create a managed mapped file.  The increment member uses a scoped
 * lock to acquire and increment a counter value for use by the Iosd objects.
 *
 * \see Iosd
 * \see Counter
 */
class SharedMemory
{
public:
    /*!
     * \brief ctor
     */
    SharedMemory(
            const Options& options  //!< [in] program options
            );

    /*!
     * \brief dtor
     */
    ~SharedMemory();

    /*!
     * \brief increment the counter and return the new value.
     */
    uint32_t increment();

private:
    const Options& _options;                                                        //!<
    boost::scoped_ptr<boost::interprocess::managed_mapped_file> _mapped_file;       //!<
    Counter* _value;                                                                //!<
};

} // sim
} // server
} // runjob

#endif
