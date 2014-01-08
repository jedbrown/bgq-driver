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
#ifndef RUNJOB_CLIENT_OPTIONS_PARSE_ENVIRONMENT_H
#define RUNJOB_CLIENT_OPTIONS_PARSE_ENVIRONMENT_H

#include <boost/program_options.hpp>

namespace runjob {
namespace client {
namespace options {

/*!
 * \brief Look for valid RUNJOB_* environment variables that are aliased to command line arguments.
 * \ingroup argument_parsing
 */
class ParseEnvironment
{
public:
    /*!
     * \brief ctor.
     */
    ParseEnvironment(
            boost::program_options::options_description& options,   //!< [in]
            boost::program_options::variables_map& vm               //!< [in]   
            );

private:
    std::string convert(
            const std::string& env  //!< [in]
            );

private:
    boost::program_options::options_description& _options;
};

} // options
} // client
} // runjob

#endif
