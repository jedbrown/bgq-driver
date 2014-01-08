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

#ifndef _MMCSCOMMANDPROCESSOR_STATUS
#define _MMCSCOMMANDPROCESSOR_STATUS

/*!
 * \brief status returned by MMCSCommandProcessor::execute()
 *
 * 0   command was executed, check reply for command status
 * 1   command was not executed because it was not found
 * 2   command was not executed because of an error, check reply for error message
 *
 */

namespace MMCSCommandProcessorStatus {
    enum procstat { CMD_EXECUTED, CMD_NOT_FOUND, CMD_INVALID, CMD_EXTERNAL };
};

#endif
