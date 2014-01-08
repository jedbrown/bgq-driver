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
#ifndef RUNJOB_CLIENT_OPTIONS_PARSER_H
#define RUNJOB_CLIENT_OPTIONS_PARSER_H

#include "client/options/fwd.h"
#include "client/options/Job.h"

#include "common/tool/Daemon.h"

#include "common/AbstractOptions.h"
#include "common/JobInfo.h"

#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>

namespace runjob {
namespace client {
namespace options {

/*!
 * \brief Main driver for program option parsing and valication.
 * \ingroup argument_parsing
 *
 * This class is heavily based on the boost::program_options library.
 */
class Parser : public AbstractOptions
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Parser> Ptr;

public:
    /*!
     * \brief ctor.
     */
    Parser(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
           );

    /*!
     * \brief dtor.
     */
    ~Parser();
    
    /*!
     * \copydoc AbstractOptions::help
     */
    void help(
            std::ostream& stream    //!< [in]
            ) const;

    const JobInfo& getJobInfo() const { return _info; } //!< Get JobInfo.
    const tool::Daemon& getTool() const { return _tool; } //!< Get tool daemon.
    std::string getSocket() const;  //!< Get socket path.
    options::Job::Timeout::Type getTimeout() const;    //!< Get timeout value.
    const Label& getLabel() const;    //!< Get label.
    bool getRaise() const;    //!< Get raise.
    boost::posix_time::time_duration getDuration() const;   //!< Get the time duration it took to parse options.

private:
    void findMpmdSentinel();
    void mpmdSyntax();

private:
    typedef boost::shared_ptr<Description> DescriptionPtr;
    typedef std::vector<DescriptionPtr> Descriptions;
    
private:
    JobInfo _info;
    tool::Daemon _tool;
    Descriptions _descriptions;
    boost::program_options::options_description _abstractOptions;
    boost::posix_time::ptime _startTime;
    unsigned _mpmdArgc;
};

} // options
} // clienmt
} // runjob

#endif
