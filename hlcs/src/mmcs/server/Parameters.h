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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

#ifndef MMCS_SERVER_PARAMETERS_H_
#define MMCS_SERVER_PARAMETERS_H_


#include <cstdlib>
#include <string>
#include <vector>


namespace mmcs {
namespace server {


/*!
 * /class Parameters
 * /brief Class for parsing command line arguments for mmcs_server
 * In addition to parsing and setting the command line values in the Parameters object, appropriate values are
 * set in the Properties object.
 */
class Parameters
{
public:
    Parameters(int argc, char** argv);
    int         _argc;                          // original command line argc
    char**      _argv ;                         // original command line argv
    std::vector<std::string> _bringup_options;  // hardware bring-up options
    bool        _listeners_set;

private:
    /*!
     * \brief display usage information and exit.
     */
    void usage(
            int exit_status = EXIT_FAILURE  //!< [in] value passed to exit(3)
            );
};

} } // namespace mmcs::server

#endif
