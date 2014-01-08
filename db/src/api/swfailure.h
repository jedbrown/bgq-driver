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
/* (C) Copyright IBM Corp.  2010, 2013                              */
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

#ifndef BGQ_DB_SW_FAILURE_H
#define BGQ_DB_SW_FAILURE_H

#include <db/include/api/tableapi/gensrc/DBTBlock.h>

namespace BGQDB {

/*!
 * \brief Clear a Software (F)ailure status from hardware resources within a block.
 *
 * \pre io != large
 */
void clearSoftwareFailure(
        const DBTBlock& block   //!< [in]
        );

} // BGQDB

#endif
