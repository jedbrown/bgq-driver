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
#ifndef RUNJOB_SERVER_BLOCK_IO_NODE_H
#define RUNJOB_SERVER_BLOCK_IO_NODE_H

#include "common/Uci.h"

#include "server/cios/fwd.h"

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

#include <iosfwd>
#include <string>

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief container for both control and data connections to a single I/O node.
 */
class IoNode : private boost::noncopyable
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<IoNode> Ptr;

public:
    /*!
     * \brief ctor
     */
    IoNode(
            const std::string& block,       //!< [in] block name
            const std::string& location     //!< [in] location string
          );

    /*!
     * \brief
     */
    void initialized(
            const boost::shared_ptr<cios::Connection>& control,   //!< [in] control connection
            const boost::shared_ptr<cios::Connection>& data       //!< [in] data connection
            );

    /*!
     * \brief dtor.
     */
    ~IoNode();

    const Uci&  getLocation() const { return _location; } //!< Get location string.
    const std::string& getBlock() const { return _block; } //!< Get block name.
    boost::shared_ptr<cios::Connection> getControl() const { return _control.lock(); } //!< Get control connection.
    boost::shared_ptr<cios::Connection> getData() const { return _data.lock(); } //!< Get data connection.

private:
    const std::string _block;
    const Uci _location;
    boost::weak_ptr<cios::Connection> _control;
    boost::weak_ptr<cios::Connection> _data;
};

} // block
} // server
} // runjob

#endif
