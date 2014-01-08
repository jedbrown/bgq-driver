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
 * \file bgsched/IOLinkImpl.h
 * \brief IOLink::Impl class definition.
 */

#ifndef BGSCHED_IOLINK_IMPL_H_
#define BGSCHED_IOLINK_IMPL_H_

#include <bgsched/IOLink.h>
#include <bgsched/types.h>

#include "bgsched/HardwareImpl.h"

#include <utility/include/XMLEntity.h>

namespace bgsched {

/*!
 * \brief Represents an I/O link on a midplane node board that connects to an I/O node
 */
class IOLink::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::XMLParseError - if error parsing XML
     */
    Impl(
            const std::string& location,    //!< [in] I/O link location
            const XMLEntity* XMLEntityPtr   //!< [in] I/O link XML description
            );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of an I/O link.
     */
    Impl(
            const IOLink::Pimpl fromIOLink    //!< [in] From I/O link
            );

    /*!
     * \brief Get I/O node destination location.
     *
     * \return I/O node destination location.
     */
    const std::string& getDestinationLocation() const;

    /*!
     * \brief Dump object to stream.
     */
    void dump(
            std::ostream& os   //!< [in/out] Stream to write to
            ) const;

    /*!
     * \brief Get the hardware state of the I/O node that the I/O link connects to.
     *
     * \return Connected I/O node hardware state.
     */
    Hardware::State getIONodeState( ) const;

protected:

    std::string         _destination;  //!< Destination I/O node location
    Hardware::State     _IONodeState;  //!< I/O node hardware state

};

} // namespace bgsched

#endif
