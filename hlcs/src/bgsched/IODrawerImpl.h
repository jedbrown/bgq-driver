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
 * \file bgsched/IODrawerImpl.h
 * \brief IODrawer::Impl class definition.
 */

#ifndef BGSCHED_IODRAWER_IMPL_H_
#define BGSCHED_IODRAWER_IMPL_H_

#include <bgsched/IODrawer.h>

#include "bgsched/HardwareImpl.h"

#include <db/include/api/cxxdb/fwd.h>

#include <vector>

namespace bgsched {

/*!
 * \brief Represents an I/O drawer.
 *
 * \note This class was added in V1R2M0.
 * \ingroup V1R2
 *
 */
class IODrawer::Impl : public Hardware::Impl
{
public:

    /*!
     * \brief Create an I/O drawer from the database.
     */
    static IODrawer::Pimpl createFromDatabase(
            const cxxdb::Columns& IODrawer_cols  //!< [in] Database I/O drawer columns
    );

    /*!
     * \brief Copy ctor.
     *
     * This creates a copy of an I/O drawer.
     */
    Impl(
            const IODrawer::Pimpl fromIODrawer  //!< [in] From I/O drawer
    );

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
    const std::vector<IONode::Pimpl>& getIONodes();

    /*!
     * \brief Add I/O node to I/O drawer.
     */
    void addIONode(
            const IONode::Pimpl ionode      //!< [in] Pointer to I/O node to be added
            );

protected:

    /*!
     * \brief Construct an I/O drawer from the database.
     */
    Impl(
            const cxxdb::Columns& IODrawer_cols //!< [in] Database I/O drawer columns
    );

protected:

    uint32_t                   _availableIONodeCount; //!< Count of I/O nodes with "Available" state
    std::vector<IONode::Pimpl> _IONodes;              //!< I/O node container
};

} // namespace bgsched

#endif
