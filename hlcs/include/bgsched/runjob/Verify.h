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
 * \file bgsched/runjob/Verify.h
 * \brief definition and implementation of bgsched::runjob::Verify class.
 */

#ifndef BGSCHED_RUNJOB_VERIFY_H
#define BGSCHED_RUNJOB_VERIFY_H

#include <bgsched/runjob/Corner.h>
#include <bgsched/runjob/Environment.h>
#include <bgsched/runjob/Shape.h>
#include <bgsched/runjob/UserId.h>

#include <string>
#include <vector>

#include <sys/types.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief Data used to verify a job can be started.
 *
 * \section Overview
 *
 * This object is passed to a job scheduler from a runjob_mux before a job
 * starts running. The job can be rejected, preventing it from starting, by invoking
 * Verify::deny_job( Verify::DenyJob::Yes ).
 * The block, corner, executable, arguments, environment, and shape parameters are set to
 * whatever was given to runjob. A scheduler can modify them as it sees fit, though their
 * values must fit into the size requirements listed below in the Limitations section. The
 * Verify::scheduler_data method can be used to set any sort of data a job scheduler wishes
 * to store with a job.
 *
 * \section sub-block-job Sub-block job
 *
 * \see Corner
 * \see Coordinates
 * \see Shape
 *
 * A job's corner and shape can be set by the Verify::corner and Verify::shape members respectively.
 * The corner compute node has coordinates within its encompasing block. The shape is specified in five dimensions,
 * and can have a maximum value of 4x4x4x4x2 in AxBxCxDxE. The combination of the corner and shape must
 * exist within a single midplane of the block, and cannot be larger than a size of 4x4x4x4x2 in AxBxCxDxE
 * or the job will not start. Setting one of the corner and shape without the other will cause the job
 * to not start.
 *
 * Example:
 *
 * \include src/bgsched/docs/runjob_plugin_sample.cc
 *
 * \note If runjob was invoked with the --corner and shape arguments, those values will be included
 * in the Corner and Shape members of the Verify object as strings.
 *
 * \section Limitations
 *
 * - The scheduler data set by Verify::scheduler_data must be less than SCHEDULER_DATA_SIZE characters.
 * - The block name set by Verify::block must match an already created and allocated block. Valid block names will be less than BLOCK_NAME_SIZE characters in length, which is enforced at block creation.
 * - The executable set by Verify::exe must be less than EXE_SIZE characters.
 * - The arguments set by Verify::args must be less than LOAD_JOB_ARGS_SIZE characters. This includes a null terminator character between each argument
 * - The environment variables set by Verify::envs must be less than LOAD_JOB_ENVS_SIZE characters. This includes the equals character in each environment, and a null terminator character between each environment.
 *
 * If any of these limitations are exceeded, the job will not start.
 */
class Verify
{
public:
    /*!
     * \brief Argument container.
     */
    typedef std::vector<std::string> Arguments;

    /*!
     * \brief Environment variable container.
     */
    typedef std::vector<Environment> Environments;

    /*!
     * \brief Deny a job from starting.
     */
    struct DenyJob
    {
        enum Value
        {
            No,
            Yes
        };
    };

public:
    /*!
     * \brief Prevent or allow the job from starting.
     */
    Verify& deny_job( DenyJob::Value );

    /*!
     * \brief Prevent the job from starting with a descriptive message
     */
    Verify& deny_job( const std::string& message );

    /*!
     * \brief Set the job's executable path.
     */
    Verify& exe( const std::string& );

    /*!
     * \brief Set the job's arguments.
     */
    Verify& args( const Arguments& );

    /*!
     * \brief Set the job's environment.
     */
    Verify& envs( const Environments& );

    /*!
     * \brief Set the job's block ID.
     */
    Verify& block( const std::string& );

    /*!
     * \brief Set the job's sub-block corner.
     *
     * \see shape( const Shape& )
     */
    Verify& corner( const Corner& );

    /*!
     * \brief Set the job's sub-block shape.
     *
     * \see corner( const Corner& )
     */
    Verify& shape( const Shape& );

    /*!
     * \brief Set the job's scheduler data.
     *
     * This is an opaque field used for storing arbitrary data for association with a job. It is
     * retained with the job in the bgqjob and bgqjob_history tables.
     */
    Verify& scheduler_data( const std::string& );

    DenyJob::Value deny_job() const;

    /*!
     * \brief Get the process ID of the runjob process. 
     */
    pid_t pid() const;

    const std::string& exe() const;
    const Arguments& args() const;
    const Environments& envs() const;
    const std::string& block() const;
    const Corner& corner() const;
    const Shape& shape() const;
    const std::string& scheduler_data() const;
    const std::string& message() const;

    /*!
     * \brief Get the user and groups of the user that invoked the runjob process.
     */
    const UserId& user() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Ctor.
     */
    explicit Verify(
            const Pimpl& impl      //!< [in] Pointer to implementation
            );

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif
