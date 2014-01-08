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
 * \file bgsched/allocator/AllocatorEventListener.h
 * \brief Class AllocatorEventListener definition.
 */

#ifndef BGSCHED_ALLOCATOR_ALLOCATOR_EVENT_LISTENER_H_
#define BGSCHED_ALLOCATOR_ALLOCATOR_EVENT_LISTENER_H_

#include <bgsched/Block.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <string>

namespace bgsched {
namespace allocator {

/*!
 *  \brief Allocator event listener interface.
 *
 *  Each of the methods in this class represents a different type of allocator event that can
 *  occur. The application should create a subclass of AllocatorEventListener and override the
 *  methods for the events that it wants to handle.
 *
 *  The default implementation of the handle methods just logs a message indicating that the
 *  event was ignored.
 */
class AllocatorEventListener
{
public:
    /*!
     * \brief Base information about a compute block event.
     */
    class BlockEvent
    {
    public:

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
         * \brief Implementation type.
         */
        class Impl;

        /*!
         * \brief Pointer to implementation type.
         */
        typedef boost::shared_ptr<Impl> Pimpl;

    protected:

        /*!
         * \brief
         */
        BlockEvent(
                Pimpl impl  //!< [in] Pointer to implementation
        );

        Pimpl _impl;

    };

    /*!
     *  \brief Compute block allocated event information.
     */
    class AllocateCompleted : public BlockEvent
    {
    public:

        /*!
         * \brief Implementation type.
         */
        class Impl;

        /*!
         * \brief Pointer to implementation type.
         */
        typedef boost::shared_ptr<Impl> Pimpl;

        /*!
         * \brief
         */
        AllocateCompleted(
                const Pimpl impl  //!< [in] Pointer to implementation
        );
    };

    /*!
     *  \brief Compute block deallocated event information.
     */
    class DeallocateCompleted : public BlockEvent
    {
    public:

        /*!
         * \brief Implementation type.
         */
        class Impl;

        /*!
         * \brief Pointer to implementation type.
         */
        typedef boost::shared_ptr<Impl> Pimpl;

        /*!
         * \brief
         */
        DeallocateCompleted(
                const Pimpl impl  //!< [in] Pointer to implementation
        );
    };

    /*!
     *  \brief Handle a compute block allocate completed event.
     */
    virtual void handleAllocate(
            const AllocateCompleted& info //!< [in] Compute block event information
        );

    /*!
     *  \brief Handle a block deallocate completed event.
     */
    virtual void handleDeallocate(
            const DeallocateCompleted& info //!< [in] Compute block event information
        );

    /*!
     *  \brief Virtual destructor.
     */
    virtual ~AllocatorEventListener();
};

} // namespace bgsched::allocator
} // namespace bgsched

#endif
