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

#include "bgsched/core/JobSortImpl.h"

namespace bgsched {
namespace core {

JobSort::Impl::Impl(
        JobSort::Field::Value field,
        SortOrder::Value sort_order
    ) :
        _field(field),
        _sort_order(sort_order)
{
    // Nothing to do.
}

JobSort::Field::Value
JobSort::Impl::getField() const
{
    return _field;
}

SortOrder::Value
JobSort::Impl::getSortOrder() const
{
    return _sort_order;
}

void
JobSort::Impl::setSort(
        Field::Value field,
        SortOrder::Value sort_order
        )
{
    _field = field;
    _sort_order = sort_order;
}

BGQDB::filtering::JobSort
JobSort::Impl::convertToDBJobSort() const
{
    BGQDB::filtering::JobSort::Field::Value field = BGQDB::filtering::JobSort::Field::Id;
    BGQDB::filtering::SortOrder::Value sortOrder;

    // Convert sort order
    if (_sort_order == bgsched::core::SortOrder::Ascending) {
        sortOrder = BGQDB::filtering::SortOrder::Ascending;
    } else {
        sortOrder = BGQDB::filtering::SortOrder::Descending;
    }

    // Convert sort field
    switch (_field) {
      case  bgsched::core::JobSort::Field::Id:
          field = BGQDB::filtering::JobSort::Field::Id;
          break;
      case  bgsched::core::JobSort::Field::User:
          field = BGQDB::filtering::JobSort::Field::User;
          break;
      case  bgsched::core::JobSort::Field::Block:
          field = BGQDB::filtering::JobSort::Field::Block;
          break;
      case  bgsched::core::JobSort::Field::Executable:
          field = BGQDB::filtering::JobSort::Field::Executable;
          break;
      case  bgsched::core::JobSort::Field::StartTime:
          field = BGQDB::filtering::JobSort::Field::StartTime;
          break;
      case  bgsched::core::JobSort::Field::EndTime:
          field = BGQDB::filtering::JobSort::Field::EndTime;
          break;
      case  bgsched::core::JobSort::Field::ExitStatus:
          field = BGQDB::filtering::JobSort::Field::ExitStatus;
          break;
      case  bgsched::core::JobSort::Field::Status:
          field = BGQDB::filtering::JobSort::Field::Status;
          break;
      case  bgsched::core::JobSort::Field::RanksPerNode:
          field = BGQDB::filtering::JobSort::Field::RanksPerNode;
          break;
      case  bgsched::core::JobSort::Field::ComputeNodesUsed:
          field = BGQDB::filtering::JobSort::Field::ComputeNodesUsed;
          break;
    }

    BGQDB::filtering::JobSort JobSort(field, sortOrder);

    return JobSort;
}

} // namespace bgsched::core
} // namespace bgsched
