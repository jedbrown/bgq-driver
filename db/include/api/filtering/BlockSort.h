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
 * \file api/filtering/BlockSort.h
 * \brief BlockSort class definition.
 */

#ifndef BGQDB_FILTERING_BLOCK_SORT_H_
#define BGQDB_FILTERING_BLOCK_SORT_H_

#include <db/include/api/filtering/SortOrder.h>

namespace BGQDB {
namespace filtering {

/*!
 * \brief Block sort options.
 *
 * This class is used as a parameter to BGQDB::filtering::getBlocks() to specify the order that blocks should be returned in.
 */
class BlockSort
{
public:

    //! \brief Block fields that can be sorted on.
    struct Field {
        //! \brief Enumeration for the Block fields.
        enum Value {
            ComputeNodeCount,
            CreateDate,
            IoNodeCount,
            Name,
            Owner,
            User,
            Status,
            StatusLastModified
        };
    };

    static const BlockSort AnyOrder; //!< Return blocks in any order.

    //! \brief Default constructor, isSorted() will return false.
    BlockSort();

    //! \brief Constructor, sort on field in sort_order, isSorted() will return true.
    BlockSort(
            Field::Value field,         //!< [in] The field.
            SortOrder::Value sort_order //!< [in] The sort order.
        );

    //! \brief Returns true if the blocks should be sorted.
    bool isSorted() const;

    //! \brief Returns the configured field to sort on, only useful if isSorted() returns true.
    Field::Value getField() const;

    //! \brief Returns the configured sort order, only useful if isSorted() returns true.
    SortOrder::Value getSortOrder() const;

    //! \brief Set the field to sort on and the sort order.
    void setSort(
            Field::Value field,         //!< [in] The field.
            SortOrder::Value sort_order //!< [in] The sort order.
            );

private:
    BlockSort::Field::Value _field;
    SortOrder::Value        _sort_order;
    bool                    _isSorted;
};

} // filtering
} // BGQDB

#endif

