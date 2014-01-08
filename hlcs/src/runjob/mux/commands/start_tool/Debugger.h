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
#ifndef RUNJOB_MUX_COMMANDS_START_TOOL_DEBUGGER_H
#define RUNJOB_MUX_COMMANDS_START_TOOL_DEBUGGER_H

#include "common/message/Proctable.h"

#include "common/tool/Daemon.h"

/*!
 * \brief structure for MPI Debug support.
 */
typedef struct {
    const char* host_name;         /*!<  Something to pass to inet_addr */
    const char* executable_name;   /*!<  The name of the image */
    int    pid;                    /*!<  The pid of the process */
} MPIR_PROCDESC;

extern "C" void MPIR_Breakpoint();

namespace runjob {
namespace mux {
namespace commands {
namespace start_tool {

/*!
 * \brief
 */
class Debugger
{
public:
    /*!
     * \brief
     */
    enum class State
    {
        Idle = 0,
        Spawned = 1,
        Aborting = 2
    };

public:
    /*!
     * \brief ctor.
     */
    Debugger();

    /*!
     * \brief dtor.
     */
    ~Debugger();

    /*!
     * \brief
     */
    tool::Daemon tool() const;

    /*!
     * \brief
     */
    bool attached() const;

    /*!
     * \brief
     */
    void fillProctable(
            const std::string& exe,                     //!< [in]
            const message::Proctable::Ptr& response     //!< [in]
            ) const;

private:
    void parseArguments(
            tool::Daemon& tool
            ) const;
};

} // start_tool
} // commands
} // mux
} // runjob

#endif
