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

#ifndef BGSCHED_CORE_BLOCK_SORT_IMPL_H_
#define BGSCHED_CORE_BLOCK_SORT_IMPL_H_

/*!
 * \file bgsched/core/BlockSortImpl.h
 * \brief BlockSort implementation class.
 */

#include <bgsched/core/BlockSort.h>

#include <db/include/api/filtering/BlockSort.h>

namespace bgsched {
namespace core {

/*!
 * \brief Compute block sort options implementation class.
 *
 * This class is used as a parameter to getBlocks() to specify the order that compute blocks should be returned in.
 *
 */
class BlockSort::Impl
{
public:

    /*!
     * \brief Constructor, sort on field in sort_order, isSorted() will return true.
     */
    Impl(
            BlockSort::Field::Value field, //!< [in] Sort field
            SortOrder::Value sort_order    //!< [in] Sort order
        );

    /*!
     * \brief Get the configured field to sort on, only useful if isSorted() returns true.
     *
     * \return The configured field to sort on.
     */
    BlockSort::Field::Value getField() const;

    /*!
     * \brief Get the configured sort order, only useful if isSorted() returns true.
     *
     * \return The configured sort order.
     */
    SortOrder::Value getSortOrder() const;

    /*!
     * \brief Set the field to sort on and the sort order.
     */
    void setSort(
            BlockSort::Field::Value field, //!< [in] Sort field
            SortOrder::Value sort_order    //!< [in] Sort order
            );

    /*!
     * \brief Convert bgsched::core::BlockSort to BGQDB::filtering::BlockSort object.
     *
     * \return BGQDB::filtering::BlockSort object.
     */
    BGQDB::filtering::BlockSort convertToDBBlockSort() const;

private:

    BlockSort::Field::Value _field;        //!< Sort field
    SortOrder::Value        _sort_order;   //!< Sort order

};

} // namespace bgsched::core
} // namespace bgsched

#endif
