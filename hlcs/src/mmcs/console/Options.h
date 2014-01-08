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

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace mmcs {
namespace console {

/*!
 * \brief Parse command line program Options.
 */
class Options
{
public:
    /*!
     * \brief ctor.
     *
     * \throws anything that boost::program_options::command_line_parser::run() throws.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
           );

    boost::shared_ptr<const bgq::utility::ClientPortConfiguration> getPortConfigurationPtr() const  { return _portConfig_ptr; }

private:
    /*!
     * \brief Open properties file.
     */
    void openProperties();

    void setupLoggingDefaults();
private:
    unsigned int                                              _argc;
    char**                                                    _argv;
    boost::program_options::variables_map                     _vm;
    boost::program_options::options_description               _options;
    bgq::utility::Properties::ProgramOptions                  _propertiesOptions;
    bgq::utility::LoggingProgramOptions                       _loggingOptions;
    boost::shared_ptr<bgq::utility::ClientPortConfiguration>  _portConfig_ptr;
};

} } // namespace mmcs::console

#endif
