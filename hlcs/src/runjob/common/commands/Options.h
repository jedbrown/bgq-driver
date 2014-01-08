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
#ifndef RUNJOB_SERVER_COMMANDS_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_OPTIONS_H

#include "common/AbstractOptions.h"
#include "common/PositiveInteger.h"

#include <hlcs/include/runjob/commands/Request.h>
#include <hlcs/include/runjob/commands/Response.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

namespace runjob {
namespace commands {

/*!
 * \brief this class holds options common across all commands.
 * \ingroup argument_parsing
 */
class Options : public AbstractOptions
{
public:
    /*!
     * \brief Job ID type.
     */
    typedef PositiveInteger<int64_t> Job;

    /*!
     * \brief
     */
    typedef PositiveInteger<int32_t> Timeout;

public:
    /*!
     * \brief dtor.
     */
    virtual ~Options();

    /*!
     * \brief Validate program options.
     */
    void validate() const;

    /*!
     * \brief handle a response message.
     */
    error::rc handle(
            const runjob::commands::Response::Ptr& response       //!< [in]
            ) const;

    /*!
     * \brief Display help text.
     */
    void help(
            std::ostream& os    //!< [in]
            ) const;

    runjob::commands::Request::Ptr getRequest() const { return _request; }    //!< Get request message.
    const bgq::utility::ClientPortConfiguration& getPort() const { return _portConfiguration; } //!< Get client port configuration.
    const Timeout&  getTimeout() const { return _timeout; }  //!< Get timeout.
    const Job& getJobId() const { return _job; } //!< Get job ID.

protected:
    /*!
     * \brief ctor
     */
    Options(
            const std::string& service,                 //!< [in]
            const std::string& log,                     //!< [in]
            runjob::commands::Message::Tag::Type tag,   //!< [in]
            unsigned int argc,                          //!< [in]
            char** argv                                 //!< [in]
            );

    /*!
     * \brief Add help information to the specified stream.
     */
    virtual void doHelp(
            std::ostream& os    //!< [in]
            ) const;

    /*!
     * \brief Handle a response message.
     */
    virtual void doHandle(
            const runjob::commands::Response::Ptr& response       //!< [in]
            ) const = 0;

    /*!
     * \brief Validate program options.
     */
    virtual void doValidate() const { return; }

    /*!
     * \brief Get usage text.
     */
    virtual std::string usage() const;

    /*!
     * \brief Get description text.
     */
    virtual const char* description() const = 0;

    /*!
     * \brief Add our options to the options parameter.
     *
     * This also parses and notifies.
     */
    void add(
            const std::string& section_name,                                            //!< [in]
            boost::program_options::options_description& options,                       //!< [in]
            AbstractOptions::ExtraParser* extra = NULL                                  //!< [in]
            );

protected:
    Job _job;

private:
    const runjob::commands::Message::Tag::Type _tag;            //!<
    boost::program_options::options_description _options;       //!<
    bgq::utility::ClientPortConfiguration _portConfiguration;   //!<
    runjob::commands::Request::Ptr _request;                    //!< request message
    Timeout _timeout;                                           //!<
};

} // commands
} // runjob

#endif
