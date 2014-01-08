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
#ifndef RUNJOB_MUX_PLUGIN_H
#define RUNJOB_MUX_PLUGIN_H

#include "mux/fwd.h"

#include <hlcs/include/bgsched/runjob/Plugin.h>

#include <utility/include/PluginHandle.h>

#include <boost/asio/strand.hpp>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace mux {

/*!
 * \brief wrapper around a plugin.
 *
 * this class is a wrapper around a similarly named class that
 * does most of the work.  The purpose here is to extract the
 * plugin path from bg.properties.
 */
class Plugin : public boost::enable_shared_from_this<Plugin>
{
public:
    /*!
     * \brief Handle type.
     */
    typedef bgq::utility::PluginHandle<bgsched::runjob::Plugin> Handle;

    /*!
     * \brief Handle pointer type.
     */
    typedef boost::shared_ptr<Handle> Ptr;

    /*!
     * \brief Handle pointer type.
     */
    typedef boost::weak_ptr<Handle> WeakPtr;

    /*!
     * \brief Callback type to get handle.
     */
    typedef boost::function< void (const WeakPtr&) > GetCallback;
public:
    /*!
     * \brief ctor.
     */
    Plugin(
            const boost::shared_ptr<Multiplexer>& mux  //!< [in]
          );

    /*!
     * \brief Start the plugin.
     */
    void start();

    /*!
     * \brief get the handle.
     */
    void get(
            const GetCallback& callback    //!< [in]
            );

private:
    void startImpl();

    void getImpl(
            const GetCallback& callback
            );

    int getFlags() const;

private:
    boost::asio::strand _strand;
    const Options& _options;
    Ptr _handle;
};

} // mux
} // runjob

#endif
