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
#include <db/include/api/filtering/JobSort.h>

namespace BGQDB {
namespace filtering {

const JobSort JobSort::AnyOrder = JobSort();

JobSort::JobSort() :
    _field(),
    _sort_order(),
    _isSorted(false)
{
    // Nothing to do.
}

JobSort::JobSort(
        Field::Value field,
        SortOrder::Value sort_order
        ):
    _field(field),
    _sort_order(sort_order),
    _isSorted(true)
{
    // Nothing to do.
}

bool
JobSort::isSorted() const
{
    return _isSorted;
}

JobSort::Field::Value
JobSort::getField() const
{
    if (_isSorted) {
        return _field;
    } else {
        return Field::Id;
    }
}

SortOrder::Value
JobSort::getSortOrder() const
{
    if (_isSorted) {
        return _sort_order;
    } else {
        return SortOrder::Ascending;
    }
}

void
JobSort::setSort(
        Field::Value field,
        SortOrder::Value sort_order
)
{
    _field = field;
    _sort_order = sort_order;
    _isSorted = true;
}

} // filtering
} // BGQDB


