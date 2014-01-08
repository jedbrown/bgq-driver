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

#ifndef FAKEDB_H_
#define FAKEDB_H_
#ifdef WITH_DB
#include <db/include/api/BGQDBlib.h>
#else
#include <vector>
#include <string>
#include <set>
#include <map>
#include "bgq_util/include/LocationUtil.h"
#ifdef BGQBLOCK_COMPAT_HEADER
#define BGQDB_USE_COMPACT true
#else
#define BGQDB_USE_COMPACT false
#endif
#ifndef SERIAL_NUM_LEN
#define SERIAL_NUM_LEN 19
#endif

namespace BGQDB
{
    enum DIAGS_MODE {
        NO_DIAGS = 0,
        NORMAL_DIAGS,
        SVCACTION_DIAGS
    };

    enum BLOCK_STATUS {
        INVALID_STATE = 0,
        FREE,
        ALLOCATED,
        CONFIG_ERROR,
        INITIALIZED,
        BOOTING,
        TERMINATING
    };
    /*!
     * \brief Return value enumeration for all calls offered by this library
     */
    enum STATUS {
        OK = 0,
        DB_ERROR,
        FAILED,
        INVALID_ID,
        NOT_FOUND,
        DUPLICATE,
        XML_ERROR,
        CONNECTION_ERROR,
        INVALID_ARG
    };

    STATUS queryMissing(
                        const std::string& block,                   //!< [in] block ID
                        std::vector<std::string>& missing,          //!< [out]
                        BGQDB::DIAGS_MODE diags = BGQDB::NO_DIAGS   //!< [in]
                        );
    STATUS queryError(
                        const std::string& block,                   //!< [in] block ID
                        std::vector<std::string>& missing          //!< [out]
                        );

    STATUS getBlockStatus(
                         const std::string& block,           //!< [in] block ID
                         BGQDB::BLOCK_STATUS& currentState    //!< [out]
                         );

};
#endif
#endif
