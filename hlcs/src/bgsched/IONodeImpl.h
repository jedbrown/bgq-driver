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

/*!
 * \file bgsched/IONodeImpl.h
 * \brief IONode::Impl class definition.
 */

#ifndef BGSCHED_IONODE_IMPL_H_
#define BGSCHED_IONODE_IMPL_H_

#include <bgsched/IONode.h>

#include "bgsched/HardwareImpl.h"

#include <string>

namespace bgsched {

/*!
 * \brief Represents an I/O node in an I/O drawer.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 */
class IONode::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     */
    Impl(
            const std::string& location,     //!< [in] I/O node location
            const Hardware::State state,     //!< [in] Hardware state
            const SequenceId sequenceId,     //!< [in] Sequence Id
            const bool inUse,                //!< [in] "In use" indicator
            const std::string& ioblock       //!< [in] I/O block using I/O node
    );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of an I/O node.
     */
    Impl(
            const IONode::Pimpl fromIONode  //!< [in] From I/O node
    );

    /*!
     * \brief I/O node "in use" indicator.
     *
     * \return true if I/O node is "in use".
     */
    bool isInUse() const;

    /* \brief Get the "in use" I/O block name.
     *
     * \return "In use" I/O block name.
     */
    const std::string& getIOBlockName() const;

protected:

    bool                  _inUse;                 //!< I/O node "in use" indicator
    std::string           _IOBlockName;           //!< "In use" I/O block

};

} // namespace bgsched

#endif
