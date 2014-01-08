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

#ifndef RUNJOB_SERVER_JOB_VALIDATE_MAPPING_FILE_H
#define RUNJOB_SERVER_JOB_VALIDATE_MAPPING_FILE_H

#include "common/Coordinates.h"
#include "common/fwd.h"

#include <db/include/api/job/types.h>

#include <spi/include/kernel/location.h>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief
 */
class ValidateMappingFile
{
public:
    /*!
     * \brief ctor.
     */
    ValidateMappingFile(
            BGQDB::job::Id id,          //!< [in]
            const JobInfo& info,        //!< [in]
            const BG_JobCoords_t& size  //!< [in] job size
            );

private:
    void analyzeLine( 
            unsigned number,
            std::string& line
            );

private:
    const BG_JobCoords_t _size;
};

} // job
} // server
} // runjob

#endif
