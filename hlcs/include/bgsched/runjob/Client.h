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

/*!
 * \file bgsched/runjob/Client.h
 * \brief definition of bgsched::runjob::Client class.
 */

#include <boost/shared_ptr.hpp>

#ifndef BGSCHED_RUNJOB_CLIENT_H
#define BGSCHED_RUNJOB_CLIENT_H

namespace bgsched {
namespace runjob {

/*!
 * \brief The runjob client interface for job schedulers
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 * 
 * \section Overview
 *
 * This class is used as an interface for remote job launch.
 *
 * \section example Example
 *
 * The example below shows a sample usage of the Client class.
 * 
 * \include src/runjob/test/client/bgsched/sample.cc
 *
 * \section linking Linking
 *
 * When linking your application, you will need to link to runjob_client like so:
 * <pre>
 * hlcs_library_dir=\$(BGQ_INSTALL_DIR)/hlcs/lib
 * </pre>
 *
 * Then use these LDFLAGs:
 * <pre>
 * LDFLAGS += -L\$(hlcs_library_dir) -lrunjob_client -Wl,-rpath,\$(hlcs_library_dir)
 * </pre>
 *
 * \section termination Abnormal Termination
 *
 * This class uses a local socket (AF_UNIX) connection to the runjob_mux process. If this connection
 * is prematurely closed due to the process abnormally terminating, the job is delivered a SIGKILL
 * signal.
 */
class Client
{
public:
    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

public:
    /*!
     * \brief version information.
     */
    struct Version {
        /*!
         * \brief Library major version number.
         */
        static const unsigned major;

        /*!
         * \brief Library minor version number.
         */
        static const unsigned minor;

        /*!
         * \brief Library minor modification number.
         */
        static const unsigned mod;

        /*!
         * \brief driver name.
         *
         * For example, V1R1M0
         */
        static const char* driver;
    };

public:
    /*!
     * \brief Ctor.
     *
     * \note see runjob documentation for argument descriptions.
     *
     * \pre argc != 0
     * \pre argv != NULL
     *
     * \throws std::invalid_argument if ambiguous or invalid options are given in argv
     */
    Client(
            int argc,           //!< [in] number of arguments in argv
            char** argv         //!< [in] null terminated array of arguments
          );

    /*!
     * \brief Start the job.
     *
     * \returns the job's exit status suitable for examining with the WIFEXITED, WIFSIGNALED, WEXITSTATUS, and WTERMSIG macros
     */
    int start(
            int input,  //!< [in] descriptor for standard input
            int output, //!< [in] descriptor for standard output
            int error   //!< [in] descriptor for standard error
            );

    /*!
     * \brief Signal the job.
     *
     * \note this call is asynchronous, it will return before the signal has been delivered.
     *
     * \throws std::logic_error if the job has not been started with start()
     */
    void kill(
            int signal  //!< [in] signal to deliver.
            );

    /*!
     * \brief Ctor.
     */
    explicit Client(
            const Pimpl& impl      //!< [in] Pointer to implementation
            );

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif


