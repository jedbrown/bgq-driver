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

#include <bgsched/core/BlockSort.h>

#include "bgsched/core/BlockSortImpl.h"

namespace bgsched {
namespace core {

const BlockSort BlockSort::AnyOrder = BlockSort();

BlockSort::BlockSort()
{
    // Nothing to do.
}

BlockSort::BlockSort(
        Field::Value field,
        SortOrder::Value sort_order
    ) :
        _impl(new Impl(field, sort_order))
{
    // Nothing to do.
}

BlockSort::Pimpl
BlockSort::getPimpl() const
{
    return _impl;
}

bool
BlockSort::isSorted() const
{
    return bool(_impl);
}

EnumWrapper<BlockSort::Field::Value>
BlockSort::getField() const
{
    if (! _impl)  { return Field::Name; }

    return _impl->getField();
}

EnumWrapper<SortOrder::Value>
BlockSort::getSortOrder() const
{
    if (! _impl)  return SortOrder::Ascending;

    return _impl->getSortOrder();
}

void
BlockSort::setSort(
        Field::Value field,
        SortOrder::Value sort_order
        )
{
    if (! _impl) {
        _impl.reset(new BlockSort::Impl(field, sort_order));
    } else {
        _impl->setSort(field, sort_order);
    }
}

} // namespace bgsched::core
} // namespace bgsched
