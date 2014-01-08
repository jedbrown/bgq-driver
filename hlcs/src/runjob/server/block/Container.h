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
#ifndef RUNJOB_SERVER_BLOCK_CONTAINER_H
#define RUNJOB_SERVER_BLOCK_CONTAINER_H

#include "server/block/fwd.h"

#include "common/error.h"
#include "common/Uci.h"

#include "server/fwd.h"

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <string>
#include <vector>

class BGQBlockNodeConfig;
class BGQMachineXML;

namespace runjob {
namespace server {
namespace block {

/*!
 * \brief %Mapping of compute to I/O blocks.
 *
 * \see Compute
 * \see Io
 * \see Reconnect
 */
class Container
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Container> Ptr;
    
    /*!
     * \brief Block container.
     */
    typedef std::vector< boost::shared_ptr<Block> > Blocks;

    /*!
     * \brief Callback for the find method.
     */
    typedef boost::function< void( const boost::shared_ptr<Compute>& ) >FindCallback;

    /*!
     * \brief Callback for the create and remove methods.
     */
    typedef boost::function<
        void(
                error_code::rc,
                const std::string&
                )
        > ResponseCallback;

    /*!
     * \brief Callback for the get method.
     */
    typedef boost::function< void( const Blocks& blocks ) > GetCallback;

    /*!
     * \brief Load machine callback.
     */
    typedef boost::function< void( const boost::shared_ptr<BGQMachineXML>& machine, const error_code::rc ) > LoadMachineCallback;

public:
    /*!
     * \brief
     */
    static Ptr create(
            const boost::shared_ptr<Server>& server
            );

    /*!
     * \brief dtor
     */
    ~Container();

    /*!
     * \brief create a block::Compute or block::Io.
     *
     * This method queries the database for the block XML definition
     * using the provided name.  It will create an object of the
     * appropriate type based on this definition.
     *
     * If the machine XML description is not provided, it will be
     * constructed using database queries, see Reconnect for an
     * example of how this is done.
     *
     * \throws Exception on error
     */
    void create(
            const std::string& name,                            //!< [in] block name
            const boost::shared_ptr<BGQMachineXML>& machine,    //!< [in] machine description
            const ResponseCallback& callback                    //!< [in] callback
            );

    /*!
     * \brief
     */
    void initialized(
            const std::string& name,                    //!< [in] block name
            const ResponseCallback& callback            //!< [in] callback
            );

    /*!
     * \brief
     */
    void terminating(
            const std::string& name,                    //!< [in] block name
            const ResponseCallback& callback            //!< [in] callback
            );

    /*!
     * \brief remove a block from the container.
     */
    void remove(
            const std::string& name,            //!< [in] block name
            const ResponseCallback& callback    //!< [in] callback
            );

    /*!
     * \brief Find a compute block.
     */
    void find(
            const std::string& id,          //!< [in]
            const FindCallback& callback    //!< [in]
            );

    /*!
     * \brief
     */
    void get(
            const GetCallback& callback  //!< [in]
            );

    /*!
     * \brief Get the machine XML description.
     */
    void loadMachine(
            const LoadMachineCallback& callback //!< [in]
            );

private:
    Container(
            const boost::shared_ptr<Server>& server
            );

    void findImpl(
            const std::string& id,
            const FindCallback& callback
            );

    void add(
            const boost::shared_ptr<Block>& block
            );

    void createImpl(
            const boost::shared_ptr<BGQMachineXML>& machine,
            const error_code::rc error,
            const std::string& name,
            const ResponseCallback& callback
            );

    void createIo(
            const std::string& name,
            const boost::shared_ptr<BGQBlockNodeConfig>& config
            );

    void createCompute(
            const std::string& name,
            const boost::shared_ptr<BGQMachineXML>& machine,
            const boost::shared_ptr<BGQBlockNodeConfig>& config
            );

    void initializedImpl(
            const std::string& name,
            const ResponseCallback& callback
            );

    void terminatingImpl(
            const std::string& name,
            const ResponseCallback& callback
            );

    void removeImpl(
            const std::string& name,
            const ResponseCallback& callback
            );

    void getImpl(
            const GetCallback& callback
            );

private:
    /*!
     * \brief mapping of compute node locations to an IoNode
     */
    typedef std::map< Uci, boost::shared_ptr<IoNode> > IoMap;

private:
    boost::asio::io_service& _io_service;           //!< io service
    boost::asio::strand _strand;                    //!< strand to protect access to _blocks and _io containers
    const boost::weak_ptr<Server> _server;          //!<
    Blocks _blocks;                                 //!< container of compute blocks
    IoMap _io;                                      //!< container of I/O
};

} // block
} // server
} // runjob

#endif
