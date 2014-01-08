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
#ifndef RUNJOB_SERVER_CONFIG_H_
#define RUNJOB_SERVER_CONFIG_H_

#include "common/AbstractOptions.h"

#include "server/Reconnect.h"

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <utility/include/BoolAlpha.h>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace runjob {
namespace server {

/*!
 * \brief program options to control server behavior
 * \ingroup argument_parsing
 */
class Options : public AbstractOptions
{
public:
    /*!
     * \brief ctor.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Options();

    /*!
     * \brief
     */
    void help(
            std::ostream& os        //!< [in]
            ) const;

    const bgq::utility::ServerPortConfiguration& getCommandPort() const { return _commandPort; }    //!< Get the server port configuration for command connections.
    const bgq::utility::ServerPortConfiguration& getMuxPort() const { return _muxPort; }    //!< Get the server port configuration for mux connections.
    bool getSim() const { return _sim._value; }    //!< Get the simulation flag.
    const Reconnect& reconnect() const { return _reconnect; }   //!< Get reconnect.
    int getThreadPoolSize() const { return _threads; }   //!< Get the thread pool size.

private:
    void combineProperties();

    void setupSimulationEnvironment();

private:
    po::options_description _options;                   //!<
    bgq::utility::ServerPortConfiguration _commandPort; //!<
    bgq::utility::ServerPortConfiguration _muxPort;     //!<
    bgq::utility::BoolAlpha _sim;                       //!<
    int _threads;                                       //!<
    Reconnect _reconnect;                               //!<
};

} // server
} // runjob

#endif
