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
#ifndef RUNJOB_SERVER_COMMANDS_DUMP_PROCTABLE_OPTIONS_H
#define RUNJOB_SERVER_COMMANDS_DUMP_PROCTABLE_OPTIONS_H

#include "common/commands/DumpProctable.h"
#include "common/commands/Options.h"

#include "common/PositiveInteger.h"

#include <utility/include/BoolAlpha.h>

namespace runjob {
namespace server {
namespace commands {
namespace dump_proctable {

/*!
 * \brief Program options specific to the dump_proctable command.
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

    const char* description() const;

    void doValidate() const;

private:
    typedef PositiveIntegerWithZero<int> Rank;

private:
    boost::program_options::options_description _options;
    std::vector<Rank> _ranks;
    bgq::utility::BoolAlpha _showLocation;
};

} // dump_proctable
} // commands
} // server
} // runjob

#endif
