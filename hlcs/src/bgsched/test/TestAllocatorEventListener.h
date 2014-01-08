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
 * \file bgsched/test/TestAllocatorEventListener.h
 * \brief Class TestAllocatorEventListener definition.
 */

#ifndef TEST_ALLOCATOR_EVENT_LISTENER_H_
#define TEST_ALLOCATOR_EVENT_LISTENER_H_

#include <bgsched/allocator/AllocatorEventListener.h>

/*!
 *  \brief Allocator event listener interface.
 *
 *  Each of the methods in this class represents a different type of allocator event that can
 *  occur.
 */
class TestAllocatorEventListener : public bgsched::allocator::AllocatorEventListener
{
public:

    /*!
     *  \brief
     */
    TestAllocatorEventListener();

    /*!
     *  \brief
     */
     ~TestAllocatorEventListener();

    /*!
     *  \brief Handle a block allocate completed event.
     */
    void handleAllocate(
            const bgsched::allocator::AllocatorEventListener::AllocateCompleted& info //!< Block event information.
        );

    /*!
     *  \brief Handle a block deallocate completed event.
     */
    void handleDeallocate(
            const bgsched::allocator::AllocatorEventListener::DeallocateCompleted& info //!< Block event information.
        );

    /*!
     *  \brief Get number of deallocates that were handled.
     *
     *  \return Number of deallocates that were handled.
     */
    uint32_t getDeallocateCount();

    /*!
     *  \brief Get number of allocates that were handled.
     *
     *  \return Number of allocates that were handled.
     */
    uint32_t getAllocateCount();

private:

    uint32_t      _deallocatesProcessed;  //!< Deallocates counter
    uint32_t      _allocatesProcessed;    //!< Allocates counter

};

#endif
