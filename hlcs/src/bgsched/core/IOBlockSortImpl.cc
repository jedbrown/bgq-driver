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

#include "bgsched/core/IOBlockSortImpl.h"

namespace bgsched {
namespace core {

IOBlockSort::Impl::Impl(
        IOBlockSort::Field::Value field,
        SortOrder::Value sort_order
    ) :
        _field(field),
        _sort_order(sort_order)
{
    // Nothing to do.
}

IOBlockSort::Field::Value
IOBlockSort::Impl::getField() const
{
    return _field;
}

SortOrder::Value
IOBlockSort::Impl::getSortOrder() const
{
    return _sort_order;
}

void
IOBlockSort::Impl::setSort(
        Field::Value field,
        SortOrder::Value sort_order
        )
{
    _field = field;
    _sort_order = sort_order;
}

BGQDB::filtering::BlockSort
IOBlockSort::Impl::convertToDBBlockSort() const
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
      case  bgsched::core::IOBlockSort::Field::IONodeCount:
          field = BGQDB::filtering::BlockSort::Field::IoNodeCount;
          break;
      case  bgsched::core::IOBlockSort::Field::CreateDate:
          field = BGQDB::filtering::BlockSort::Field::CreateDate;
          break;
      case  bgsched::core::IOBlockSort::Field::Name:
          field = BGQDB::filtering::BlockSort::Field::Name;
          break;
      case  bgsched::core::IOBlockSort::Field::Owner:
          field = BGQDB::filtering::BlockSort::Field::Owner;
          break;
      case  bgsched::core::IOBlockSort::Field::User:
          field = BGQDB::filtering::BlockSort::Field::User;
          break;
      case  bgsched::core::IOBlockSort::Field::Status:
          field = BGQDB::filtering::BlockSort::Field::Status;
          break;
      case  bgsched::core::IOBlockSort::Field::StatusLastModified:
          field = BGQDB::filtering::BlockSort::Field::StatusLastModified;
          break;
    }

    BGQDB::filtering::BlockSort blockSort(field, sortOrder);

    return blockSort;
}

} // namespace bgsched::core
} // namespace bgsched
