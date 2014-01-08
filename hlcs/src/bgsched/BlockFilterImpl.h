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
 * \file bgsched/BlockFilterImpl.h
 * \brief BlockFilter::Impl class definition.
 */

#ifndef BGSCHED_BLOCK_FILTER_IMPL_H_
#define BGSCHED_BLOCK_FILTER_IMPL_H_

#include <bgsched/BlockFilter.h>

#include <db/include/api/filtering/BlockFilter.h>

namespace bgsched {

/*!
 * \brief Represents a block filter.
 */
class BlockFilter::Impl
{
public:

    /*!
     * \brief
     *
     *  The compute block filter is constructed with the following default settings:
     *  - Include all compute block statuses in search
     *  - Include all compute block sizes (large/small) in search
     *  - Compute block job details will be excluded in returned compute blocks
     *  - Only basic block info is returned for block objects
     */
    Impl();

    /*!
     * \brief Set compute block owner filter.
     */
    void setOwner(
            const std::string& owner          //!< [in] Compute block owner to filter on
    );

    /*!
     * \brief Set compute block user filter. This is the user that booted the block and only applies
     * to booted blocks.
     */
    void setUser(
            const std::string& user           //!< [in] Compute block user to filter on
    );

    /*!
     * \brief Set compute block status filter.
     */
    void setStatuses(
            const bgsched::BlockFilter::Statuses* statusesPtr  //!< [in] Pointer to Statuses set to filter on
    );

    /*!
     * \brief Set compute block name filter.
     */
    void setName(
            const std::string& name           //!< [in] Compute block name to filter on
    );

    /*!
     * \brief Set the level of compute block details option.
     *
     * If this filter option is true, then information about midplanes, switches and node boards will be included
     * for compute block objects.
     *
     * If this is filter option is false, then only basic block information will be set for compute block objects.
     */
    void setExtendedInfo(
            bool extendedInfo  //!< [in] true=provide extended block details, false=provide basic block details
    );

    /*!
     * \brief Set the include jobs filter option.
     *
     * If this is filter option is true, then job information will be included for compute block objects.
     *
     * If this is filter option is false, job information will not be set for compute block objects.
     */
    void setIncludeJobs(
            bool includeJobs  //!< [in] true=include job info, false=exclude job info
    );

    /*!
     * \brief Set compute block size.
     */
    void setSize(
            BlockSize::Value blockSize  //!< [in] Compute block size to filter on
    );

    /*!
     * \brief Get compute block owner filter.
     *
     * \return Compute block owner filter.
     */
    const std::string& getOwner() const;

    /*!
     * \brief Get compute block user filter. This is the user that booted the block and only applies
     * to booted blocks.
     *
     * \return Compute block user filter.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get compute block status filter.
     *
     * \return Compute block status filter.
     */
    const bgsched::BlockFilter::Statuses getStatuses() const;

    /*!
     * \brief Get compute block name filter.
     *
     * \return Compute block name filter.
     */
    const std::string& getName() const;

    /*!
     * \brief Get the level of compute block details option.
     *
     * If this filter option is true, then information about midplanes, switches and node boards will be included
     * for compute block objects.
     *
     * If this is filter option is false, then only basic block information will be set for block objects.
     *
     * \return Level of compute block details option.
     */
    bool getExtendedInfo() const;

    /*!
     * \brief Get the include jobs filter option.
     *
     * If this filter option is true, then job information will be included for compute block objects.
     *
     * If this filter option is false, job information will not be set for compute block objects.
     *
     * \return Include jobs filter option.
     */
    bool getIncludeJobs() const;

    /*!
     * \brief Get the compute block size filter option.
     *
     * \return Compute block size filter option.
     */
    bgsched::BlockFilter::BlockSize::Value getSize() const;

    /*!
     * \brief Convert bgsched::BlockFilter to BGQDB::filtering::BlockFilter object.
     *
     * \return BGQDB::filtering::BlockFilter object.
     */
    BGQDB::filtering::BlockFilter convertToDBBlockFilter() const;

protected:

    BGQDB::filtering::BlockFilter   _blockFilter; //!< Block filter

};

} // namespce bgsched

#endif
