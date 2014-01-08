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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_SERVER_BOOT_FAILURE_H
#define MMCS_SERVER_BOOT_FAILURE_H

#include "types.h"

#include <bgq_util/include/Location.h>
#include <ras/include/RasEventImpl.h>

#include <boost/utility.hpp>

#include <map>

namespace mmcs {
namespace server {

/*!
 * \brief Analyze why a block failed to boot and produce a helpful RAS event
 * and useful details in the mmcs_server log.
 *
 * This class attempts to coalesce compute node locations that fail to
 * initialize into their common (midplane, node board, I/O node) parts
 * that may be the root cause.
 *
 * \note noncopyable because the destructor inserts a RAS event
 */
class BootFailure : private boost::noncopyable
{
public:
    static const unsigned ComputeCardsOnMidplane;

public:
    /*!
     * \brief ctor
     */
    BootFailure(
            const BlockPtr& block,
            const std::string& message
            );

    /*!
     * \brief dtor.
     */
    ~BootFailure();

    /*!
     * \brief
     */
    void timeout();

private:
    void gatherDetails(
            const std::string& node
            );

    bool logComputeBlockDetails() const;

private:
    typedef std::map<std::string, unsigned> NodeFailure;

private:
    RasEventImpl _event;
    const BlockPtr _block;
    NodeFailure _io;
    NodeFailure _boards;
    NodeFailure _midplanes;
};

} } // namespace mmcs::server

#endif
