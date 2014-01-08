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

#ifndef BGSCHED_REALTIME_FILTER_H_
#define BGSCHED_REALTIME_FILTER_H_

/*!
 *  \file
 *
 *  \brief Class Filter.
 */


#include <bgsched/realtime/types.h>

#include <bgsched/Hardware.h>
#include <bgsched/Job.h>
#include <bgsched/Block.h>

#include <boost/shared_ptr.hpp>

#include <stdint.h>

#include <set>
#include <string>


namespace bgsched {
namespace realtime {


/*!
 *  \brief Event filter.
 *
 *  The Filter can be set on the Client to tell the real-time server the types of events that the application is interested in.
 *
 *  Many of the properties are patterns. See the man pages for GNU regcomp for the syntax of the pattern.
 */
class Filter
{
public:
    typedef uint64_t Id; //!< %Filter ID.

    typedef std::set<Hardware::State> HardwareStates; //!< Container for hardware states.
    typedef std::set<Job::Id> JobIds; //!< %Job IDs
    typedef std::set<Job::Status> JobStatuses; //!< Container for job statuses.
    typedef std::set<Block::Status> BlockStatuses; //!< Container for block statuses.
    typedef std::set<RasSeverity::Value> RasSeverities; //!< Container for RAS severities.


    /*!
     *  \brief Create filter that sends no events.
     *
     *  This is useful if the application wants to receive only a few types of events.
     */
    static Filter createNone();

    /*!
     *  \brief Create filter that sends all events.
     */
    static Filter createAll();

    /*!
     *  \brief Create default filter.
     *
     *  The filter is constructed with the following default settings:
     *  - Jobs will be sent. jobs=true
     *  - Not filtered by job name. jobName=NULL
     *  - Not filtered by job status. jobStatuses=NULL
     *  - %Job deletions sent. jobDeleted=true
     *  - Not filtered by job block ID. jobBlockIdPattern=NULL
     *  - Blocks will be sent. blocks=true
     *  - Not filtered by block ID. blockIdPattern=NULL
     *  - Not filtered by block status. blockStatuses=NULL
     *  - %Block deletions sent. blockDeleted=true
     *  - Midplane state changes will be sent.
     *  - Node board state changes will be sent.
     *  - Node state changes will be sent.
     *  - Switch state changes will be sent.
     *  - Torus cable state changes will be sent.
     *  - I/O cable state changes will not be sent.
     *  - RAS events are NOT sent. rasEvents=false
     *  - RAS events not filtered by message ID. rasMessageIdPattern=NULL
     *  - RAS events not filtered by severity. rasSeverities=NULL
     *  - RAS events not filtered by job ID. rasJobIds=NULL
     *  - RAS events not filtered by block ID. rasBlockIdPattern=NULL
     */
    Filter();

    /*!
     *  \brief Set the jobs filter option.
     *
     *  If this is true then job-type changes will be sent.
     *
     *  If this is false, job-type changes will not be sent, and the other jobs-type filter
     *  options don't apply, such as jobIds and jobStatuses.
     *
     *  The default is true.
     */
    void setJobs(
        bool jobs //!< [in] New setting for filter option. false=do not send jobs.
      );

    /*!
     *  \brief Get the jobs filter option.
     */
    bool getJobs() const;

    /*!
     *  \brief Set the job statuses filter option.
     *
     *  If this option is not set then job events won't be filtered by status. Otherwise, only the
     *  events for jobs where the status is changing to one of the specified statuses will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setJobStatuses(
            const JobStatuses* job_statuses_p //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the job statuses filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const JobStatuses* getJobStatuses_p() const;

    /*!
     *  \brief Set the job deleted filter option.
     *
     *  If this is true then job deletions will be sent. Otherwise, job deletions won't be sent.
     *
     *  The default is true.
     */
    void setJobDeleted(
            bool job_deleted //!< [in] New setting for the filter option. false=do not send job deletions
        );

    /*!
     *  \brief Get the job deleted filter option.
     */
    bool getJobDeleted() const;

    /*!
     *  \brief Set the job block ID pattern.
     *
     *  If this is not set, then job events aren't filtered by block ID. If this is set, then
     *  only job events with a block ID matching the pattern will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setJobBlockIdPattern(
            const std::string* pattern_p //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the job block ID pattern filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const std::string* getJobBlockIdPattern_p() const;

    /*!
     *  \brief Set the blocks filter option.
     *
     *  If this is true, then block-type events will be sent.
     *
     *  If this is false, block-type events will not be sent, and the other block-type
     *  filter options don't apply, such as blockIdPattern and blockStatuses.
     *
     *  The default is true.
     */
    void setBlocks(
            bool blocks //!< [in] New setting for the filter option. false=do not send blocks.
        );

    /*!
     *  \brief Get the block filter option.
     */
    bool getBlocks() const;

    /*!
     *  \brief Set the block ID pattern filter option.
     *
     *  If this is not set, then block events aren't filtered by ID. If this is set, then only
     *  block events with an ID matching the pattern will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setBlockIdPattern(
            const std::string* block_id_pattern_p //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the block ID pattern filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const std::string* getBlockIdPattern_p() const;

    /*!
     *  \brief Set the block statuses filter option.
     *
     *  If this option is not set then block events won't be filtered by status. Otherwise,
     *  only the events for blocks where the status is changing to one of the specified
     *  statuses will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setBlockStatuses(
            BlockStatuses* block_statuses_p //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the block statuses filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const BlockStatuses* getBlockStatuses_p() const;

    /*!
     *  \brief Set the block deleted filter option.
     *
     *  If this is true then block deletions will be sent. Otherwise, block deletions
     *  won't be sent.
     *
     *  The default is true.
     */
    void setBlockDeleted(
            bool block_deleted //!< [in] New setting for the filter option. false=do not send block deletions.
        );

    /*!
     *  \brief Get the block deleted filter option.
     */
    bool getBlockDeleted() const;

    /*!
     *  \brief Set the midplanes filter option.
     *
     *  If this is true, then midplane events will be sent.
     *
     *  If this is false, midplane events will not be sent.
     *
     *  The default is true.
     */
    void setMidplanes(
            bool midplanes //!< [in] New setting for the filter option. false=do not send midplane state changes.
        );

    /*!
     *  \brief Get the midplanes filter option.
     */
    bool getMidplanes() const;

    /*!
     *  \brief Set the node boards filter option.
     *
     *  If this is true, then node board events will be sent.
     *
     *  If this is false, node board events will not be sent.
     *
     *  The default is true.
     */
    void setNodeBoards(
            bool node_boards //!< [in] New setting for the filter option. false=do not send node board state changes.
        );

    /*!
     *  \brief Get the node boards filter option.
     */
    bool getNodeBoards() const;

    /*!
     *  \brief Set the nodes filter option.
     *
     *  If this is true, then node events will be sent.
     *
     *  If this is false, node events will not be sent.
     *
     *  The default is true.
     */
    void setNodes(
            bool node //!< [in] New setting for the filter option. false=do not send node state changes.
        );

    /*!
     *  \brief Get the nodes filter option.
     */
    bool getNodes() const;

    /*!
     *  \brief Set the switches filter option.
     *
     *  If this is true, then switch events will be sent.
     *
     *  If this is false, switch events will not be sent.
     *
     *  The default is true.
     */
    void setSwitches(
            bool switches //!< [in] New setting for the filter option. false=do not send switch state changes.
        );

    /*!
     *  \brief Get the switchs filter option.
     */
    bool getSwitches() const;

    /*!
     *  \brief Set the torus cables filter option.
     *
     *  If this is true, then torus cable events will be sent.
     *
     *  If this is false, torus cable events will not be sent.
     *
     *  The default is true.
     */
    void setTorusCables(
            bool torus_cables //!< [in] New setting for the filter option. false=do not send torus cable state changes.
        );

    /*!
     *  \brief Get the torus cables filter option.
     */
    bool getTorusCables() const;


    /*!
     *  \brief Set the I/O cables filter option.
     *
     *  If this is true, then I/O cable events will be sent.
     *
     *  If this is false, I/O cable events will not be sent.
     *
     *  The default is true.
     */
    void setIoCables(
            bool io_cables //!< [in] New setting for the filter option. false=do not send I/O cable state changes.
        );

    /*!
     *  \brief Get the I/O cables filter option.
     */
    bool getIoCables() const;


    /*!
     *  \brief Set the RAS events filter option.
     *
     *  If this is true, then RAS event-type events will be sent.
     *
     *  If this is false, RAS event-type events will not be sent, and the other RAS event-type
     *  filter options don't apply, such as rasMessageIdPattern and rasSeverities.
     *
     *  The default is false.
     */
    void setRasEvents(
            bool ras_events //!< [in] New setting for the filter option. true=send ras events.
        );

    /*!
     *  \brief Get the RAS events filter option.
     */
    bool getRasEvents() const;

    /*!
     *  \brief Set the RAS message ID pattern filter option.
     *
     *  If this option is not set then RAS events won't be filtered by message ID. Otherwise,
     *  only the events for RAS events where the message ID matches the pattern will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setRasMessageIdPattern(
            const std::string* pattern_p  //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the RAS message ID pattern filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const std::string* getRasMessageIdPattern_p() const;

    /*!
     *  \brief Set the RAS severities filter option.
     *
     *  If this option is not set then RAS events won't be filtered by severity. Otherwise,
     *  only the events for RAS events where the severities contains the severity will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setRasSeverities(
            const RasSeverities* ras_severities_p  //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the RAS severities filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const RasSeverities* getRasSeverities_p() const;

    /*!
     *  \brief Set the RAS job IDs filter option.
     *
     *  If this option is not set then RAS events won't be filtered by job ID. Otherwise,
     *  only the events for RAS events where the job IDs contains the job ID will be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setRasJobIds(
            const JobIds* job_ids_p  //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the RAS job IDs filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const JobIds* getRasJobIds_p() const;

    /*!
     *  \brief Set the RAS compute block ID pattern filter option.
     *
     *  If this option is not set then RAS events won't be filtered by compute block ID.
     *  Otherwise, only the events for RAS events where the block ID matches the pattern will
     *  be sent.
     *
     *  The default is <i>not set</i>.
     */
    void setRasComputeBlockIdPattern(
            const std::string* pattern_p  //!< [in] New setting for the filter option. NULL=unset.
        );

    /*!
     *  \brief Get the RAS compute block ID pattern filter option.
     *
     *  \return NULL if the value is not set or a pointer to the value.
     */
    const std::string* getRasComputeBlockIdPattern_p() const;


    /*!
     *  \brief Set the I/O drawers filter option.
     *
     *  This is new for V1R2M0.
     *
     *  If this is true, then I/O drawer events will be sent.
     *
     *  If this is false, I/O drawer events will not be sent.
     *
     *  The default is false.
     *
     *  \ingroup V1R2
     */
    void setIoDrawers(
            bool io_drawers //!< [in] New setting for the filter option. false=do not send I/O drawer state changes.
        );

    /*!
     *  \brief Get the I/O drawers filter option.
     *
     *  This is new for V1R2M0.
     *
     *  \ingroup V1R2
     */
    bool getIoDrawers() const;


    /*!
     *  \brief Set the I/O nodes filter option.
     *
     *  This is new for V1R2M0.
     *
     *  If this is true, then I/O node events will be sent.
     *
     *  If this is false, I/O node events will not be sent.
     *
     *  The default is false.
     *
     *  \ingroup V1R2
     */
    void setIoNodes(
            bool io_nodes //!< [in] New setting for the filter option. false=do not send I/O node state changes.
        );

    /*!
     *  \brief Get the I/O nodes filter option.
     *
     *  This is new for V1R2M0.
     *
     *  \ingroup V1R2
     */
    bool getIoNodes() const;


    class Impl;
    typedef boost::shared_ptr<Impl> Pimpl;


private:

    Pimpl _impl_ptr;

    friend class Client;
};


} // namespace bgsched::realtime
} // namespace bgsched

#endif
