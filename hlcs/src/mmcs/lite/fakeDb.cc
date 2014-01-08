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

#ifndef _FAKEDB_H_
#include "fakeDb.h"
#endif

BGQDB::STATUS BGQDB::getBlockStatus(
                                   const std::string& block,           //!< [in] block ID
                                   BGQDB::BLOCK_STATUS& currentState    //!< [out]
                                   ) {
    currentState = BGQDB::FREE;
    return BGQDB::OK;
}

BGQDB::STATUS BGQDB::queryMissing(
                                  const std::string& block,                   //!< [in] block ID
                                  std::vector<std::string>& missing,          //!< [out]
                                  BGQDB::DIAGS_MODE diags   //!< [in]
                                  ) {
    return BGQDB::OK;
}

BGQDB::STATUS BGQDB::queryError(
                                  const std::string& block,                   //!< [in] block ID
                                  std::vector<std::string>& missing           //!< [out]
                                  ) {
    return BGQDB::OK;
}
