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
#ifndef RUNJOB_MUX_CONFIG_H_
#define RUNJOB_MUX_CONFIG_H_

#include "common/AbstractOptions.h"

#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>
#include <utility/include/portConfiguration/ServerPortConfiguration.h>

namespace runjob {
namespace mux {

/*!
 * \brief program options to control mux behavior.
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

    // getters
    const std::string& getLocalSocket() const { return _socket; }   //!< Get local socket path.
    int getThreadPoolSize() const { return _threads; }     //!< Get thread pool size.
    const bgq::utility::ClientPortConfiguration& getClientPort() const { return _clientPort; }  //!< Get client port configuration.
    const bgq::utility::ServerPortConfiguration& getCommandPort() const { return _commandPort; }  //!< Get server command port configuration.

private:
    void combineProperties();

private:
    boost::program_options::options_description _options;       //!<
    bgq::utility::ClientPortConfiguration _clientPort;          //!<
    bgq::utility::ServerPortConfiguration _commandPort;         //!<
    std::string _host;                                          //!<
    uint16_t _port;                                             //!<
    std::string _socket;                                        //!<
    int _threads;                                               //!<
};

} // mux
} // runjob

#endif
