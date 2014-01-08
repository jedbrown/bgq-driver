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
#ifndef RUNJOB_SERVER_COMMANDS_GRANT_JOB_AUTHORITY_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_GRANT_JOB_AUTHORITY_OPTIONS_H

#include "common/commands/Options.h"

#include "common/MaximumLengthString.h"

#include <db/include/api/tableapi/gensrc/DBTJobsecurity.h>
#include <hlcs/include/security/Types.h>

#include <vector>

namespace runjob {
namespace server {
namespace commands {
namespace grant_job_authority {

/*!
 * \brief Program options specific to the grant_job_authority command.
 */
class Options: public runjob::commands::Options
{
public:
    /*!
     * \brief ctor.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
            );

private:
    void doHelp(
            std::ostream& os
            ) const;

    void doValidate() const;

    void doHandle(
            const runjob::commands::Response::Ptr&
            ) const { return; }

    const char* description() const;

private:
    boost::program_options::options_description _options;   //!<
    runjob::MaximumLengthString<BGQDB::DBTJobsecurity::AUTHID_SIZE> _user;
    hlcs::security::Action::Type _action;
};

} // grant_job_authority
} // commands
} // server
} // runjob

#endif
