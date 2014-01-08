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
 * \file bgsched/allocator/AllocatorEventListenerImpl.h
 * \brief AllocatorEventListener::Impl class definition.
 */

#ifndef ALLOCATOR_EVENT_LISTENER_IMPL_H_
#define ALLOCATOR_EVENT_LISTENER_IMPL_H_

#include "../realtime/ClientEventListenerImpl.h"

#include <bgsched/allocator/AllocatorEventListener.h>

#include <bgsched/Block.h>
#include <bgsched/types.h>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 * \brief Represents an allocator event listener block change event.
 */
class AllocatorEventListener::BlockEvent::Impl
{
public:

    /*!
     * \brief
     */
    Impl(
            const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& chg,
            const std::string& error_msg
            );

    /*!
     * \brief Get the event compute block name.
     *
     * \return Event compute block name.
     */
    const std::string& getBlockName() const;

    /*!
     * \brief Get the event compute block status.
     *
     * \return Event compute block status.
     */
    Block::Status getStatus() const;

    /*!
     * \brief Get the event status sequence ID.
     *
     * \return Event status sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get event success indicator.
     *
     * \return Event success indicator.
     */
    bool successful() const;

    /*!
     * \brief Get error message (if any).
     *
     * \return Event error message (if any).
     */
    const std::string& getErrorMessage() const;

protected:

    std::string   _block_name;  //!< Event block name
    Block::Status _status;      //!< Event block status
    SequenceId    _seq_id;      //!< Event block sequence id
    bool          _successful;  //!< Event success indicator
    std::string   _error_msg;   //!< Event error message if unsuccessful

};

/*!
 *  \brief Compute block allocated event information.
 */
class AllocatorEventListener::AllocateCompleted::Impl : public AllocatorEventListener::BlockEvent::Impl
{

public:

    /*!
     * \brief
     */
    Impl(
            const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& chg,
            const std::string& error_msg);

};

/*!
 *  \brief Compute block deallocated event information.
 */
class AllocatorEventListener::DeallocateCompleted::Impl : public AllocatorEventListener::BlockEvent::Impl
{

public:

    /*!
     * \brief
     */
    Impl(
            const realtime::ClientEventListener::BlockStateChangedEventInfo::Impl& chg,
            const std::string& error_msg);

};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
