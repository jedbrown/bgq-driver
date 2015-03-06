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

#ifndef RUNJOB_SERVER_JOB_COPY_MAPPING_FILE_H
#define RUNJOB_SERVER_JOB_COPY_MAPPING_FILE_H

#include "common/fwd.h"

#include <db/include/api/job/types.h>
#include <utility/include/Properties.h>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class CopyMappingFile
{
public:
    static const std::string KeyName;

public:
    /*!
     * \brief ctor.
     */
    CopyMappingFile(
            BGQDB::job::Id id,                                      //!< [in]
            const JobInfo& info,                                    //!< [in]
            const bgq::utility::Properties::ConstPtr& properties    //!< [in]
            );

    /*!
     * \brief
     */
    std::string result() const { return _copiedFilename; }

private:
    void impl(
            const Mapping& mapping
            );

private:
    const BGQDB::job::Id _id;
    std::string _copiedFilename;
};

} // job
} // server
} // runjob

#endif
