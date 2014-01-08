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
#ifndef RUNJOB_SERVER_COMMANDS_CIOS_LOG_LEVEL_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_CIOS_LOG_LEVEL_OPTIONS_H

#include "common/commands/LogLevel.h"
#include "common/commands/Options.h"

#include <bgq_util/include/Location.h>

#include <log4cxx/log4cxx.h>

namespace runjob {
namespace server {
namespace commands {
namespace cios_log_level {

/*!
 * \brief Program options specific to the cios_log_level command.
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

private:
    void doValidate() const;

    std::string usage() const;

    void doHandle(
            const runjob::commands::Response::Ptr&
            ) const;

    void doHelp(
            std::ostream& os
            ) const;
    
    const char* description() const {
        return "Change the logging configuration for the stdio and jobctl daemons on I/O node(s).";
    }

private:
    boost::program_options::options_description _options;   //!<
    std::string _block;
    bgq::util::Location _location;
    std::vector<std::string> _strings;
};

} // cios_log_level
} // commands
} // server
} // runjob

#endif
