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
 * \file bgsched/IODrawer.h
 * \brief IODrawer class definition.
 */

#ifndef BGSCHED_IODRAWER_H_
#define BGSCHED_IODRAWER_H_

#include <bgsched/Hardware.h>
#include <bgsched/IONode.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace bgsched {

/*!
 * \brief Represents an I/O drawer.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 *
 */
class IODrawer : public Hardware
{
public:

    static const uint32_t IONodeCount = 8;               //!< Number of I/O nodes in an I/O drawer

    typedef boost::shared_ptr<IODrawer> Ptr;             //!< Pointer type.
    typedef boost::shared_ptr<const IODrawer> ConstPtr;  //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Get I/O drawer sequence ID.
     *
     * \return I/O drawer sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the number of available I/O nodes.
     *
     * \return Number of available I/O nodes.
     */
    uint32_t getAvailableIONodeCount() const;

    /*!
     * \brief Get the list of I/O nodes for the I/O drawer.
     *
     * \return List of I/O nodes.
     */
    IONode::ConstPtrs getIONodes() const;

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
    explicit IODrawer(
            Pimpl impl    //!< [in] Pointer to implementation
    );
};

} // namespace bgsched

#endif
