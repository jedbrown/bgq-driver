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

#ifndef BGWS_BLUE_GENE_DIAGNOSTICS_RUN_OPTIONS_HPP_
#define BGWS_BLUE_GENE_DIAGNOSTICS_RUN_OPTIONS_HPP_


#include <string>
#include <vector>


namespace bgws {
namespace blue_gene {
namespace diagnostics {


class RunOptions
{
public:

    typedef std::vector<std::string> Args;


    void setUserName( const std::string& user_name )  { _user_name = user_name; }
    const std::string& getUserName() const  { return _user_name; }

    void setArgs( const Args& args )  { _args = args; }
    const Args& getArgs() const  { return _args; }


private:

    std::string _user_name;
    Args _args;
};


} } } // namespace bgws::blue_gene::diagnostics

#endif
