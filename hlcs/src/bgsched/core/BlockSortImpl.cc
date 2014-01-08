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

#include "bgsched/core/BlockSortImpl.h"

namespace bgsched {
namespace core {

BlockSort::Impl::Impl(
        BlockSort::Field::Value field,
        SortOrder::Value sort_order
    ) :
        _field(field),
        _sort_order(sort_order)
{
    // Nothing to do.
}

BlockSort::Field::Value
BlockSort::Impl::getField() const
{
    return _field;
}

SortOrder::Value
BlockSort::Impl::getSortOrder() const
{
    return _sort_order;
}

void
BlockSort::Impl::setSort(
        Field::Value field,
        SortOrder::Value sort_order
        )
{
    _field = field;
    _sort_order = sort_order;
}

BGQDB::filtering::BlockSort
BlockSort::Impl::convertToDBBlockSort() const
{
    BGQDB::filtering::BlockSort::Field::Value field = BGQDB::filtering::BlockSort::Field::Name;
    BGQDB::filtering::SortOrder::Value sortOrder;

    // Convert sort order
    if (_sort_order == bgsched::core::SortOrder::Ascending) {
        sortOrder = BGQDB::filtering::SortOrder::Ascending;
    } else {
        sortOrder = BGQDB::filtering::SortOrder::Descending;
    }

    // Convert sort field
    switch (_field) {
      case  bgsched::core::BlockSort::Field::ComputeNodeCount:
          field = BGQDB::filtering::BlockSort::Field::ComputeNodeCount;
          break;
      case  bgsched::core::BlockSort::Field::CreateDate:
          field = BGQDB::filtering::BlockSort::Field::CreateDate;
          break;
      case  bgsched::core::BlockSort::Field::Name:
          field = BGQDB::filtering::BlockSort::Field::Name;
          break;
      case  bgsched::core::BlockSort::Field::Owner:
          field = BGQDB::filtering::BlockSort::Field::Owner;
          break;
      case  bgsched::core::BlockSort::Field::User:
          field = BGQDB::filtering::BlockSort::Field::User;
          break;
      case  bgsched::core::BlockSort::Field::Status:
          field = BGQDB::filtering::BlockSort::Field::Status;
          break;
      case  bgsched::core::BlockSort::Field::StatusLastModified:
          field = BGQDB::filtering::BlockSort::Field::StatusLastModified;
          break;
    }

    BGQDB::filtering::BlockSort blockSort(field, sortOrder);

    return blockSort;
}

} // namespace bgsched::core
} // namespace bgsched
