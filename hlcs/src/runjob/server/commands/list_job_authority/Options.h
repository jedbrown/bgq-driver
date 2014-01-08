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
#ifndef RUNJOB_SERVER_COMMANDS_LIST_JOB_AUTHORITY_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_LIST_JOB_AUTHORITY_OPTIONS_H

#include "common/commands/JobAcl.h"
#include "common/commands/Options.h"

#include <vector>

namespace runjob {
namespace server {
namespace commands {
namespace list_job_authority {

/*!
 * \brief Program options specific to the job_acl command.
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

    /*!
     * \copydoc runjob::commands::Options::doHandle
     */
    void doHandle(
            const runjob::commands::Response::Ptr&   //!< [in]
            ) const;

private:
    void doHelp(
            std::ostream& os
            ) const;

    void doValidate() const;

    const char* description() const;

private:
    boost::program_options::options_description _options;   //!<
};

} // list_job_authority
} // commands
} // server
} // runjob

#endif
