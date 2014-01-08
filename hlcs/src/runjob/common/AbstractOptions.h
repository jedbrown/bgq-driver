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
#ifndef RUNJOB_COMMON_ABSTRACT_CONFIG_H_
#define RUNJOB_COMMON_ABSTRACT_CONFIG_H_

#include "common/logging.h"

#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/exception/all.hpp>
#include <boost/function.hpp>
#include <boost/program_options.hpp>
#include <boost/utility.hpp>

#include <string>
#include <utility>

namespace runjob {

/*!
 * \brief abstract interface for obtaining program options.
 * \ingroup argument_parsing
 *
 * This class is heavily based on Boost.ProgramOptions library, consult its documentation
 * for more information about how it works.
 */
class AbstractOptions : boost::noncopyable
{
public:
    /*!
     * \brief Extra parser function signature.
     */
    typedef boost::function<
        std::pair<std::string,std::string>(const std::string&)
        > ExtraParser;

public:
    /*!
     * \brief ctor.
     */
    AbstractOptions(
            int argc,                                   //!< [in] number of arguments
            char** argv,                                //!< [in] argument array
            const std::string& log_name                 //!< [in] logger name to pass to bgq::utility::LoggingProgramOptions
            );

    /*!
     * \brief dtor.
     */
    virtual ~AbstractOptions();

    /*!
     * \brief display help text.
     */
    virtual void help(
            std::ostream& stream        //!< [in]
            ) const = 0;

    /*!
     * \brief display version information.
     */
    void version(
            std::ostream& stream        //!< [in]
            ) const;

    // getters
    bgq::utility::Properties::Ptr getProperties() const { return _properties; } //!< Get properties file.
    bool getHelp() const { return _vm["help"].as<bool>(); }  //!< Get help flag.
    bool getVersion() const { return _vm["version"].as<bool>(); }    //!< get version flag.

protected:
    /*!
     * \brief Parse program options.
     *
     * The extra parser can be used to detect a syntax that is normally not recognized by
     * the Boost.ProgramOptions library. See runjob::server::commands::KillJobOptions for an example.
     *
     * \throws any exception that boost::program_options::command_line_parser can throw
     */
    void parse(
            boost::program_options::options_description& options,   //!< [in]
            ExtraParser* extra = NULL                               //!< [in]
            );

    /*!
     * \brief Notify program options parsers that parsing is complete.
     *
     * Open the properties file and configure logging.
     *
     * \throws anything that AbstractOptions::openProperties can throw
     */
    void notify();

    /*!
     * \brief Combine these options with the provided options.
     */
    void add(
            boost::program_options::options_description&    //!< [in] options to add to our options.
            );

    /*!
     * \brief Calculate the number of worker threads to use using /proc/cpuinfo.
     */
    unsigned calculateWorkerThreads();

    /*!
     * \brief Get our host name.
     */
    std::string hostname();

    /*!
     * \brief Get our user and group credentials.
     */
    std::string credentials();

protected:
    boost::program_options::variables_map _vm;                              //!< variables map for option storage
    int _argc;                                                              //!< argument count
    char** _argv;                                                           //!< argument vector
    boost::program_options::positional_options_description _positionalArgs; //!< positional args

private:
    /*!
     * \brief Open the properties file.
     *
     * throws bgq::utility::Properties::FileError
     * throws bgq::utility::Properties::DuplicateKey
     * throws bgq::utility::Properties::DuplicateSection
     * throws bgq::utility::Properties::MalformedKey
     * throws bgq::utility::Properties::MalformedSection
     * throws bgq::utility::Properties::MissingSection
     */
    void __attribute__ ((visibility("hidden"))) openProperties();

    /*!
     * \brief
     */
    void __attribute__ ((visibility("hidden"))) logLimits();

private:
    bgq::utility::Properties::Ptr _properties;                      //!<
    bgq::utility::Properties::ProgramOptions _propertiesOptions;    //!<
    bgq::utility::LoggingProgramOptions _loggingOptions;            //!<
};

} // runjob

#endif
