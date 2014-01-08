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


#ifndef LOGMERGE_HPP_
#define LOGMERGE_HPP_


#include "Configuration.hpp"

#include <iosfwd>


namespace log_merge {


class FilenameMapper;


class LogMerge
{
public:


    static void validatePaths( const Paths& paths );


    LogMerge();

    void setOutput( std::ostream& os )  { _os_p = &os; }

    void setConfiguration( const Configuration& configuration )  { _configuration = configuration; }

    void run();


private:

    Configuration _configuration;
    std::ostream *_os_p;


    void _runImmediate( const FilenameMapper& filename_mapper );

    void _runLive( const FilenameMapper& filename_mapper );
};

} // namespace log_merge

#endif
