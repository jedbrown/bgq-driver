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
#ifndef RUNJOB_MUX_COMMANDS_STATUS_OPTIONS_H
#define RUNJOB_MUX_COMMANDS_STATUS_OPTIONS_H

#include "common/commands/MuxStatus.h"
#include "common/commands/Options.h"

#include <boost/asio.hpp>

namespace runjob {
namespace mux {
namespace commands {
namespace status {

/*!
 * \brief Program options specific to the runjob_mux_status command.
 */
class Options: public runjob::commands::Options
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
     * \copydoc runjob::commands::Options::doHandle
     */
    void doHandle(
            const runjob::commands::Response::Ptr&   //!< [in]
            ) const;

private:
    /*!
     * \brief
     */
    void doHelp(
            std::ostream& os
            ) const;
   
    /*!
     * \brief
     */
    const char* description() const { return "display status information from the Blue Gene runjob mux."; }

    /*!
     * \brief
     */
    void displayConnections(
            const runjob::commands::response::MuxStatus::Ptr&   //!< [in]
            ) const;

    /*!
     * \brief
     */
    void displayClients(
            const runjob::commands::response::MuxStatus::Ptr&   //!< [in]
            ) const;

private:
    boost::program_options::options_description _options;   //!<
    boost::asio::io_service _io_service;                    //!<
    mutable boost::asio::ip::tcp::resolver _resolver;       //!<

};

} // status
} // commands
} // mux
} // runjob

#endif
