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
 * \file bgsched/IONode.h
 * \brief IONode class definition.
 */

#ifndef BGSCHED_IONODE_H_
#define BGSCHED_IONODE_H_

#include <bgsched/Hardware.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace bgsched {

/*!
 * \brief Represents an I/O node in an I/O drawer.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 *
 */
class IONode : public Hardware
{
public:

    typedef boost::shared_ptr<IONode> Ptr;               //!< Pointer type.
    typedef boost::shared_ptr<const IONode> ConstPtr;    //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                       //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;             //!< Collection of const pointers.

    /*!
     * \brief Get I/O node sequence ID.
     *
     * \return I/O node sequence ID.
     */
    SequenceId getSequenceId() const;

    /*!
     * \brief Get the I/O node "in use" indicator.
     *
     * \return I/O node "in use" indicator.
     */
    bool isInUse() const;

    /*!
     * \brief Get the "in use" I/O block name.
     *
     * \return "In use" I/O block name. Returns empty string if I/O node is not is use.
     */
    const std::string& getIOBlockName() const;

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
    explicit IONode(
            Pimpl impl    //!< [in] Pointer to implementation
            );
};

} // namespace bgsched

#endif
