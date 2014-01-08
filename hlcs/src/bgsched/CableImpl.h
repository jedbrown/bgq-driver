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
 * \file bgsched/CableImpl.h
 * \brief Cable::Impl class definition.
 */

#ifndef BGSCHED_CABLE_IMPL_H_
#define BGSCHED_CABLE_IMPL_H_

#include <bgsched/Cable.h>
#include <bgsched/Switch.h>
#include <bgsched/types.h>

#include "bgsched/HardwareImpl.h"

#include <bgq_util/include/Location.h>

#include <string>

class XMLEntity;

namespace bgsched {

/*!
 * \brief Represents a cable connection between two switches.
 */
class Cable::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::XMLParseError - if error parsing XML
     */
    Impl(
            const std::string& location,    //!< [in] Cable location
            const XMLEntity* XMLEntityPtr   //!< [in] Cable XML description
            );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of a cable.
     */
    Impl(
            const Cable::Pimpl fromCable    //!< [in] From cable
            );

    /*!
     * \brief Get cable destination location.
     *
     * \return Cable destination location.
     */
    const std::string& getDestinationLocation() const;

    /*!
     * \brief Dump object to stream.
     */
    void dump(
            std::ostream& os  //!< [in/out] Stream to write to
            ) const;

protected:

    bgq::util::Location _destination;   //!< Cable destination location
};

} // namespace bgsched

#endif
