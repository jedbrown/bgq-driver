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

#ifndef BGSCHED_CORE_JOB_SORT_H_
#define BGSCHED_CORE_JOB_SORT_H_

/*!
 * \file bgsched/core/JobSort.h
 * \brief JobSort class.
 */

#include <bgsched/core/SortOrder.h>

#include <bgsched/EnumWrapper.h>

#include <boost/shared_ptr.hpp>

namespace bgsched {
namespace core {

/*!
 * \brief Job sort options.
 *
 * This class is used as a parameter to getJobs() to specify the order that jobs should be returned in.
 *
 */
class JobSort
{
public:

    /*!
     * \brief Job fields that can be sorted on.
     */
    struct Field {

        /*!
         * \brief Enumeration for the Job fields.
         */
        enum Value {
            Id,
            User,
            Block,
            Executable,
            StartTime,
            EndTime,
            ExitStatus,
            Status,
            ComputeNodesUsed,
            RanksPerNode
        };
    };

    /*!
     * \brief Return jobs in any order
     */
    static const JobSort AnyOrder;

    /*!
     * \brief Default constructor, isSorted() will return false.
     */
    JobSort();

    /*!
     * \brief Constructor, sort on field in sort_order, isSorted() will return true.
     */
    explicit JobSort(
            Field::Value field,         //!< [in] Sort field
            SortOrder::Value sort_order //!< [in] Sort order
        );

    /*!
     * \brief Returns true if the jobs should be sorted.
     */
    bool isSorted() const;

    /*!
     * \brief Get the configured field to sort on, only useful if isSorted() returns true.
     *
     * \return The configured field to sort on.
     */
    EnumWrapper<Field::Value> getField() const;

    /*!
     * \brief Get the configured sort order, only useful if isSorted() returns true.
     *
     * \return The configured sort order.
     */
    EnumWrapper<SortOrder::Value> getSortOrder() const;

    /*!
     * \brief Set the field to sort on and the sort order.
     */
    void setSort(
            Field::Value field,         //!< [in] Sort field
            SortOrder::Value sort_order //!< [in] Sort order
            );

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Get pointer to implementation.
     *
     * \return Pointer to implementation.
     */
    Pimpl getPimpl() const;

private:

    Pimpl _impl;

};

} // namespace bgsched::core
} // namespace bgsched

#endif

