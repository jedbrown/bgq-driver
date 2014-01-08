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

#ifndef BGWS_BLUE_GENE_HPP_
#define BGWS_BLUE_GENE_HPP_


#include <bgq_util/include/Location.h>

#include <stdexcept>
#include <string>
#include <vector>


namespace bgws {
namespace blue_gene {


extern const std::vector<std::string> ALL_NB_POS;


namespace diagnostics {

extern const std::string HARDWARE_BLOCK_PREFIX;
    // When diagnostics generates a block ID from a midplane location, it uses this prefix.

namespace run_status_db {
    // The values in the diagStatus in the bgqDiagRuns table.

extern const std::string RUNNING;
extern const std::string COMPLETED;
extern const std::string FAILED;
extern const std::string CANCELED;

}

} // namespace bgws::blue_gene::diagnostics


namespace performance_monitoring {

const std::string& getFunctionBoot();

} // namespace bgws::blue_gene::performance_monitoring


namespace service_actions {


void setCwdBaremetalBin();


namespace action {

extern const std::string CLOSE;
extern const std::string END;
extern const std::string PREPARE;

} // namespace bgws::blue_gene::service_actions::action


namespace db_actions {

extern const std::string CLOSED;
extern const std::string END;
extern const std::string OPEN;
extern const std::string PREPARE;

const unsigned Count=4;

} // namespace bgws::blue_gene::service_actions::db_actions


namespace db_statuses {

extern const std::string ACTIVE;
extern const std::string ERROR;
extern const std::string OPEN;
extern const std::string CLOSED;
extern const std::string FORCED;
extern const std::string PREPARED;

const unsigned Count=6;

} // namespace bgws::blue_gene::service_actions::db_statuses


namespace hardware_type_name {

extern const std::string RACK;
extern const std::string MIDPLANE;
extern const std::string NODE_BOARD;
extern const std::string NODE_DCA;
extern const std::string IO_DRAWER;
extern const std::string BULK_POWER_MODULE;
extern const std::string CLOCK_CARD;


    //! \throws InvalidLocationError if the type of location is not valid for a service action.
const std::string& fromLocation( const bgq::util::Location& location );


class InvalidLocationError : public std::invalid_argument
{
public:
    InvalidLocationError( const std::string& location );
};


} // namespace bgws::blue_gene::service_actions::hardware_type_name


} // namespace service_actions

} } // namespace bgws::blue_gene

#endif
