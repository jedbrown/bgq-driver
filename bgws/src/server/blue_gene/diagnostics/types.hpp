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


#ifndef BGWS_BLUE_GENE_DIAGNOSTICS_TYPES_HPP_
#define BGWS_BLUE_GENE_DIAGNOSTICS_TYPES_HPP_


#include <boost/cstdint.hpp>
#include <boost/function.hpp>

#include <string>


namespace bgws {
namespace blue_gene {
namespace diagnostics {


class Run;
class Runs;


typedef boost::int64_t RunId;

typedef boost::function<void ( RunId run_id, const std::string& error_msg )> RunIdCallback;


} } } // namespace bgws::blue_gene::diagnostics

#endif
