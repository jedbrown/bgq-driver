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
 * \file bgsched/Cable.h
 * \brief Cable class Definition.
 */

#ifndef BGSCHED_CABLE_H_
#define BGSCHED_CABLE_H_

#include <bgsched/Hardware.h>
#include <bgsched/types.h>

#include <boost/shared_ptr.hpp>

#include <string>

namespace bgsched {

/*!
 * \brief Represents a cable connection between two switches.
 */
class Cable : public Hardware
{
public:

    typedef boost::shared_ptr<Cable> Ptr;                  //!< Pointer type.
    typedef boost::shared_ptr<const Cable> ConstPtr;       //!< Const pointer type.

    typedef std::vector<Ptr> Ptrs;                         //!< Collection of pointers.
    typedef std::vector<ConstPtr> ConstPtrs;               //!< Collection of const pointers.

    /*!
     * \brief Get cable destination location.
     *
     * \return Cable destination location.
     */
    const std::string& getDestinationLocation() const;

    /*!
     * \brief Get cable sequence ID.
     *
     * \return Cable sequence ID.
     */
    SequenceId getSequenceId() const;

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
    explicit Cable(
            Pimpl impl     //!< [in] Pointer to implementation
            );
};

} // namespace bgsched

#endif
