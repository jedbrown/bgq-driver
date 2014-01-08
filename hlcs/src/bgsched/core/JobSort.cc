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

#include <bgsched/core/JobSort.h>

#include "bgsched/core/JobSortImpl.h"

namespace bgsched {
namespace core {

const JobSort JobSort::AnyOrder = JobSort();

JobSort::JobSort()
{
    // Nothing to do.
}

JobSort::JobSort(
        Field::Value field,
        SortOrder::Value sort_order
    ) :
        _impl(new Impl(field, sort_order))
{
    // Nothing to do.
}

JobSort::Pimpl
JobSort::getPimpl() const
{
    return _impl;
}

bool
JobSort::isSorted() const
{
    return bool(_impl);
}

EnumWrapper<JobSort::Field::Value>
JobSort::getField() const
{
    if (! _impl)  { return Field::Id; }

    return _impl->getField();
}

EnumWrapper<SortOrder::Value>
JobSort::getSortOrder() const
{
    if (! _impl)  return SortOrder::Ascending;

    return _impl->getSortOrder();
}

void
JobSort::setSort(
        Field::Value field,
        SortOrder::Value sort_order
        )
{
    if (! _impl) {
        _impl.reset(new JobSort::Impl(field, sort_order));
    } else {
        _impl->setSort(field, sort_order);
    }
}

} // namespace bgsched::core
} // namespace bgsched
