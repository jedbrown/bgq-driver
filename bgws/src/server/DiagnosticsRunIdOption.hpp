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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#ifndef BGWS_DIAGNOSTICS_RUN_ID_OPTION_HPP_
#define BGWS_DIAGNOSTICS_RUN_ID_OPTION_HPP_


#include "blue_gene/diagnostics/types.hpp"

#include <boost/optional.hpp>

#include <iosfwd>


namespace bgws {


class DiagnosticsRunIdOption
{
public:

    typedef boost::optional<blue_gene::diagnostics::RunId> RunIdOpt;


    void set( blue_gene::diagnostics::RunId run_id )  { _run_id_opt = run_id; }

    const RunIdOpt& getOpt() const  { return _run_id_opt; }


private:

    RunIdOpt _run_id_opt;

};


std::istream& operator>>( std::istream& is, DiagnosticsRunIdOption& run_id_option_out );


} // namespace bgws

#endif
