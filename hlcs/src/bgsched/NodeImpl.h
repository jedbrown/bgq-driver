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
 * \file bgsched/NodeImpl.h
 * \brief Node::Impl class definition.
 */

#ifndef BGSCHED_NODE_IMPL_H_
#define BGSCHED_NODE_IMPL_H_

#include <bgsched/Node.h>

#include "bgsched/HardwareImpl.h"

#include <utility/include/XMLEntity.h>

#include <string>

namespace bgsched {

/*!
 * \brief Represents a node on a node board.
 */
class Node::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief
     *
     * \throws bgsched::InternalException with value:
     * - bgsched::InternalErrors::XMLParseError - if error parsing XML
     */
    Impl(
            const std::string& location,     //!< [in] Node location
            const XMLEntity* XMLEntityPtr    //!< [in] Node XML description
        );

    /*!
     * \brief
     */
    Impl(
            const std::string& location,     //!< [in] Node location
            const Hardware::State state,     //!< [in] Hardware state
            const SequenceId sequenceId      //!< [in] Sequence Id
        );

};

} // namespace bgsched

#endif
