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
 * \file bgsched/IOLink.h
 * \brief I/O link class definition.
 */

#ifndef BGSCHED_IOLINK_H_
#define BGSCHED_IOLINK_H_

#include <bgsched/Hardware.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace bgsched {

/*!
 * \brief Represents an I/O link on a midplane node board that connects to an I/O node.
 */
class IOLink : public Hardware
{
public:

    typedef boost::shared_ptr<IOLink> Ptr;               //!< Pointer type.
    typedef boost::shared_ptr<const IOLink> ConstPtr;    //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Get I/O node destination location
     *
     * \return I/O node destination location.
     */
    const std::string& getDestinationLocation() const;

    /*!
     * \brief Get I/O link sequence ID.
     *
     * \return I/O link sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the hardware state of the I/O node that the I/O link connects to.
     *
     * \return Connected I/O node hardware state.
     */
    EnumWrapper<Hardware::State> getIONodeState() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief
     */
    explicit IOLink(
            Pimpl impl     //!< [in] Pointer to implementation
            );
};

} // namespace bgsched

#endif
