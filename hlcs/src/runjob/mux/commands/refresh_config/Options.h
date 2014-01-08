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
#ifndef RUNJOB_MUX_COMMANDS_REFRESH_CONFIG_OPTIONS_H
#define RUNJOB_MUX_COMMANDS_REFRESH_CONFIG_OPTIONS_H

#include "common/commands/Options.h"

#include <hlcs/include/runjob/commands/RefreshConfig.h>

namespace runjob {
namespace mux {
namespace commands {
namespace refresh_config {

/*!
 * \brief Program options specific to the runjob_mux_refresh_config command.
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
            ) const
    {
        return;
    }

    /*!
     * \copydoc runjob::commands::Options::doValidate
     */
    void doValidate() const;

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
    const char* description() const { return "refresh the Blue Gene runjob mux configuration."; }

private:
    boost::program_options::options_description _options;   //!<
    std::string _file;
};

} // refresh_config
} // commands
} // mux
} // runjob

#endif
