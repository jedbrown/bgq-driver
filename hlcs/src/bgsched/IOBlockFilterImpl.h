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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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
 * \file bgsched/IOBlockFilterImpl.h
 * \brief IOBlockFilter::Impl class definition.
 */

#ifndef BGSCHED_IOBLOCK_FILTER_IMPL_H_
#define BGSCHED_IOBLOCK_FILTER_IMPL_H_

#include <bgsched/IOBlockFilter.h>

#include <db/include/api/filtering/BlockFilter.h>

namespace bgsched {

/*!
 * \brief Represents an I/O block filter.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 */
class IOBlockFilter::Impl
{
public:

    /*!
     * \brief
     *
     *  The I/O block filter is constructed with the following default settings:
     *  - Include all I/O block statuses in search
     */
    Impl();

    /*!
     * \brief Set I/O block owner filter.
     */
    void setOwner(
            const std::string& owner          //!< [in] I/O block owner to filter on
    );

    /*!
     * \brief Set I/O block user filter. This is the user that booted the block and only applies to booted blocks.
     */
    void setUser(
            const std::string& user           //!< [in] I/O block user to filter on
    );

    /*!
     * \brief Set I/O block status filter.
     */
    void setStatuses(
            const bgsched::IOBlockFilter::Statuses* statusesPtr  //!< [in] Pointer to Statuses set to filter on
    );

    /*!
     * \brief Set I/O block name filter.
     */
    void setName(
            const std::string& name           //!< [in] I/O block name to filter on
    );

    /*!
     * \brief Set the level of I/O block details option.
     *
     * If this filter option is true, then information about I/O node locations will be included for I/O block objects.
     *
     * If this is filter option is false, then only basic block information will be set for I/O block objects.
     */
    void setExtendedInfo(
            bool extendedInfo  //!< [in] true=provide extended I/O block details, false=provide basic I/O block details
    );

    /*!
     * \brief Get I/O block owner filter.
     *
     * \return I/O block owner filter.
     */
    const std::string& getOwner() const;

    /*!
     * \brief Get I/O block user filter. This is the user that booted the I/O block and only applies to booted blocks.
     *
     * \return I/O block user filter.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get I/O block status filter.
     *
     * \return I/O block status filter.
     */
    const bgsched::IOBlockFilter::Statuses getStatuses() const;

    /*!
     * \brief Get I/O block name filter.
     *
     * \return I/O block name filter.
     */
    const std::string& getName() const;

    /*!
     * \brief Get the level of I/O block details option.
     *
     * If this filter option is true, then information about I/O node locations will be included for I/O block objects.
     *
     * If this filter option is false, then only basic I/O block information will be set for I/O block objects.
     *
     * \return Level of I/O block details option.
     */
    bool getExtendedInfo() const;

    /*!
     * \brief Convert bgsched::IOBlockFilter to BGQDB::filtering::BlockFilter object.
     *
     * \return BGQDB::filtering::BlockFilter object.
     */
    BGQDB::filtering::BlockFilter convertToDBBlockFilter() const;

protected:

    BGQDB::filtering::BlockFilter   _IOBlockFilter; //!< I/O block filter

};

} // namespce bgsched

#endif
