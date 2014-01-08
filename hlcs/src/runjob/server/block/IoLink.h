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
#ifndef RUNJOB_SERVER_BLOCK_IO_LINK_H
#define RUNJOB_SERVER_BLOCK_IO_LINK_H

#include "common/Uci.h"

#include "server/block/fwd.h"

#include <boost/shared_ptr.hpp>

#include <bgq_util/include/Location.h>


namespace runjob {
namespace server {
namespace block {

/*!
 * \brief Describes the two endpoints of an I/O link, the 
 * compute node and the I/O node.
 */
class IoLink
{
public:
    /*!
     * \brief ctor
     */
    IoLink(
            const boost::shared_ptr<IoNode>& io,    //!< [in] I/O node
            const std::string& compute              //!< [in] location string
          );

    /*!
     * \brief dtor.
     */
    ~IoLink();

    boost::shared_ptr<IoNode> getIo() const { return _io; }    //!< Get I/O node.
    const Uci& getCompute() const { return _compute; } //!< Get compute node location string.

private:
    boost::shared_ptr<IoNode> _io;
    Uci _compute;
};

} // block
} // server
} // runjob

#endif
