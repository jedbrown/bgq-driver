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
 * \file bgsched/runjob/Node.h
 * \brief definition and implementation of bgsched::runjob::Node class.
 */

#ifndef BGSCHED_RUNJOB_NODE_H
#define BGSCHED_RUNJOB_NODE_H

#include <bgsched/runjob/Coordinates.h>

#include <string>

namespace bgsched {
namespace runjob {

/*!
 * \brief Node information.
 */
class Node
{
public:
    /*!
     * \brief Get the node's location string.
     */
    const std::string& location() const;

    /*!
     * \brief Get the node's torus coordinates.
     *
     * \note the coordinates are relative to the block.
     */
    const Coordinates& coordinates() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Ctor.
     */
    explicit Node(
            const Pimpl& impl      //!< [in] Pointer to implementation
            );

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif

