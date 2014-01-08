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
 * \file bgsched/Node.h
 * \brief Node class definition.
 */

#ifndef BGSCHED_NODE_H_
#define BGSCHED_NODE_H_

#include <bgsched/Hardware.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace bgsched {

/*!
 * \brief Represents a node on a node board.
 */
class Node : public Hardware
{
public:

    typedef boost::shared_ptr<Node> Ptr;                 //!< Pointer type.
    typedef boost::shared_ptr<const Node> ConstPtr;      //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Get node sequence ID.
     *
     * \return Node sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     *
     */
    explicit Node(
            Pimpl impl    //!< [in] Pointer to implementation
            );
};

} // namespace bgsched

#endif
