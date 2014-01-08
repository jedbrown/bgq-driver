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

/*!
 * \file ReconnectBlocks.h
 * \brief
 */

#ifndef RECONNECT_BLOCKS_H
#define RECONNECT_BLOCKS_H

#include <control/include/mcServer/MCServerRef.h>

#include <string>

class MMCSServerParms;
class MMCSCommandReply;
class MMCSCommandProcessor;

typedef boost::shared_ptr<MCServerRef> MCRefPtr;

enum MCServerMessageType { FAILOVER_MSG, DIE_MSG, BRINGUP_MSG, NO_MSG };

/*!
 * \brief
 */
void reconnectBlocks(
        MMCSServerParms *mmcsParms,                 //!< [in]
        MMCSCommandReply& reply,                    //!< [in]
        MMCSCommandProcessor* commandProcessor      //!< [in]
        );

void freeBlock(
        const std::string& block,       //!< [in]
        const std::string& user,        //!< [in]
        MCRefPtr& mcServer        //!< [in]
        );

void deleteTargetSet(
        const std::string& block,       //!< [in]
        const std::string& user,        //!< [in]
        MCRefPtr& mcServer        //!< [in]
        );

#endif
