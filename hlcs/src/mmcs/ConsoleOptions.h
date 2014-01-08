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

#ifndef MMCS_CONSOLE_OPTIONS_H
#define MMCS_CONSOLE_OPTIONS_H

#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/program_options.hpp>

#include <string>

/*!
 * \brief Parse command line program Options.
 */
class ConsoleOptions
{
public:
    /*!
     * \brief ctor.
     *
     * \throws anything that boost::program_options::command_line_parser::run() throws.
     */
    ConsoleOptions(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
           );

    /*!
     * \brief Get retry string.
     */
    const std::string& getRetry() const { return _retry; }

private:
    /*!
     * \brief Open properties file.
     */
    void openProperties();

    void setupLoggingDefaults();
private:
    unsigned int _argc;                                             //!<
    char** _argv;                                                   //!<
    std::string _retry;                                             //!<
    boost::program_options::variables_map _vm;                      //!<
    boost::program_options::options_description _options;           //!<
    bgq::utility::Properties::ProgramOptions _propertiesOptions;    //!<
    bgq::utility::LoggingProgramOptions _loggingOptions;            //!<
};

#endif
