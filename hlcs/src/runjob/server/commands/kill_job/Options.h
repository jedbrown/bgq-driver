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
#ifndef RUNJOB_SERVER_COMMANDS_KILL_JOB_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_KILL_JOB_OPTIONS_H

#include "common/commands/Options.h"

#include "common/PositiveInteger.h"

#include <hlcs/include/runjob/commands/KillJob.h>

#include <boost/tuple/tuple.hpp>

#include <vector>

namespace runjob {
namespace server {
namespace commands {
namespace kill_job {

/*!
 * \brief Program options specific to the kill_job command.
 */
class Options: public runjob::commands::Options
{
public:
    /*!
     * \brief Tuple of signal name and number.
     */
    typedef boost::tuple<std::string,int> Signal;

    /*!
     * \brief Container of Signal objects.
     */
    typedef std::vector<Signal> Signals;

    /*!
     * \brief
     */
    typedef PositiveInteger<int32_t> Timeout;

public:
    /*!
     * \brief List of supported signals.
     */
    static const Signals signals;

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
     * \brief Get signal.
     */
    const std::string& getSignal() const { return _signal; }

    /*!
     * \brief Get list option.
     */
    bool getList() const { return _vm["list"].as<bool>(); }

    /*!
     * \brief
     */
    const Timeout& getTimeout() const { return _timeout; }

    /*!
     * \brief
     */
    pid_t getPid() const { return _pid; }

private:
    void doValidate() const;

    void doHelp(
            std::ostream& os
            ) const;

    const char* description() const;
    
    int convertSignal(
            const std::string& signal   //!< [in]
            ) const;

    std::pair<std::string,std::string> signalParser(
            const std::string& arg  //!< [in]
            );

    std::string getDetails() const;

private:
    pid_t _pid;
    std::string _hostname;
    boost::program_options::options_description _options;   //!<
    std::string _signal;
    Timeout _timeout;
};

} // kill_job
} // commands
} // server
} // runjob

#endif
