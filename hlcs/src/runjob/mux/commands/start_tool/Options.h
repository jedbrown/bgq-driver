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
#ifndef RUNJOB_MUX_COMMANDS_START_TOOL_OPTIONS_H
#define RUNJOB_MUX_COMMANDS_START_TOOL_OPTIONS_H

#include "mux/commands/start_tool/Debugger.h"

#include "common/message/Proctable.h"
#include "common/message/StartTool.h"
#include "common/tool/Daemon.h"
#include "common/AbstractOptions.h"
#include "common/Message.h"
#include "common/PositiveInteger.h"

#include <utility/include/BoolAlpha.h>

#include <sys/types.h>

#include <boost/asio.hpp>

#include <string>
#include <vector>

namespace runjob {
namespace mux {
namespace commands {
namespace start_tool {

/*!
 * \brief Program options specific to the start_tool command.
 */
class Options : public AbstractOptions
{
public:
    /*!
     * \brief Job ID type.
     */
    typedef PositiveInteger<int64_t> Job;

public:
    /*!
     * \brief ctor.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
            );

    /*!
     * \brief
     */
    void help(
            std::ostream& os        //!< [in]
            ) const;

    /*!
     * \brief
     */
    void validate() const;

    /*!
     * \brief
     */
    void handle(
            boost::asio::local::stream_protocol::socket& socket //!< [in]
            );

    /*!
     * \brief
     */
    const tool::Daemon& toolDaemon() const { return _tool; }

    /*!
     * \brief
     */
    pid_t pid() const { return _pid; }

    /*!
     * \brief
     */
    const std::string& socket() const { return _socket; }

private:
    void result(
            const Message::Ptr& message
            );

    void proctable(
            const message::Proctable::Ptr& result
            );

private:
    pid_t _pid;
    Job _job;
    std::string _socket;
    boost::program_options::options_description _options;
    tool::Daemon _tool;
    bgq::utility::BoolAlpha _interactive;
    Debugger _debugger;
};

} // start_tool
} // commands
} // mux
} // runjob

#endif
