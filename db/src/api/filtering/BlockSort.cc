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
#include <db/include/api/filtering/BlockSort.h>

namespace BGQDB {
namespace filtering {

const BlockSort BlockSort::AnyOrder = BlockSort();

BlockSort::BlockSort() :
    _field(),
    _sort_order(),
    _isSorted(false)
{
    // Nothing to do.
}

BlockSort::BlockSort(
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
BlockSort::isSorted() const
{
    return _isSorted;
}

BlockSort::Field::Value
BlockSort::getField() const
{
    if (_isSorted) {
        return _field;
    } else {
        return Field::Name;
    }
}

SortOrder::Value
BlockSort::getSortOrder() const
{
    if (_isSorted) {
        return _sort_order;
    } else {
        return SortOrder::Ascending;
    }
}

void
BlockSort::setSort(
        Field::Value field,
        SortOrder::Value sort_order
)
{
    _field = field;
    _sort_order = sort_order;
}

} // filtering
} // BGQDB
