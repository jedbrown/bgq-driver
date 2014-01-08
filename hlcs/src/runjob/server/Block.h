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
#ifndef RUNJOB_SERVER_BLOCK_H
#define RUNJOB_SERVER_BLOCK_H

#include "server/fwd.h"

#include <string>

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <stdint.h>

namespace runjob {
namespace server {

/*!
 * \brief Abstract representation of a compute or I/O block.
 */
class Block
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<Block> Ptr;

public:
    /*!
     * \brief ctor.
     *
     * \pre !name.empty()
     * \pre block != NULL
     */
    Block(
            const std::string& name,                                //!< [in] block name
            const boost::shared_ptr<BGQBlockNodeConfig>& block,     //!< [in] block configuration
            const boost::shared_ptr<Server>& server                 //!< [in] server object
         );

    /*!
     * \brief dtor.
     */
    virtual ~Block() = 0;

    /*!
     * \brief
     */
    virtual void initialized() { return; }

    const std::string& name() const { return _name; }    //!< Get block name.

    boost::shared_ptr<const BGQBlockNodeConfig> config() const { return _config; }  //!< Get configuration.

    unsigned id() const { return _id; } //!< Get block creation ID.

    unsigned a() const;  //!< Get size of A dimension.
    unsigned b() const;  //!< Get size of B dimension.
    unsigned c() const;  //!< Get size of C dimension.
    unsigned d() const;  //!< Get size of D dimension.
    unsigned e() const;  //!< Get size of E dimension.

protected:
    const std::string _name;                                //!< name
    const unsigned _id;                                     //!< creation ID
    const boost::shared_ptr<BGQBlockNodeConfig> _config;    //!< block description
    const boost::weak_ptr<Server> _server;                  //!< server object
};

} // server
} // runjob

#endif
