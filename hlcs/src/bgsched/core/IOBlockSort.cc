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

#include <bgsched/core/IOBlockSort.h>

#include "bgsched/core/IOBlockSortImpl.h"

namespace bgsched {
namespace core {

const IOBlockSort IOBlockSort::AnyOrder = IOBlockSort();

IOBlockSort::IOBlockSort()
{
    // Nothing to do.
}

IOBlockSort::IOBlockSort(
        Field::Value field,
        SortOrder::Value sort_order
    ) :
        _impl(new Impl(field, sort_order))
{
    // Nothing to do.
}

IOBlockSort::Pimpl
IOBlockSort::getPimpl() const
{
    return _impl;
}

bool
IOBlockSort::isSorted() const
{
    return bool(_impl);
}

EnumWrapper<IOBlockSort::Field::Value>
IOBlockSort::getField() const
{
    if (! _impl)  { return Field::Name; }

    return _impl->getField();
}

EnumWrapper<SortOrder::Value>
IOBlockSort::getSortOrder() const
{
    if (! _impl)  return SortOrder::Ascending;

    return _impl->getSortOrder();
}

void
IOBlockSort::setSort(
        Field::Value field,
        SortOrder::Value sort_order
        )
{
    if (! _impl) {
        _impl.reset(new IOBlockSort::Impl(field, sort_order));
    } else {
        _impl->setSort(field, sort_order);
    }
}

} // namespace bgsched::core
} // namespace bgsched
