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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef _MMCSConsoleMonitor_H
#define _MMCSConsoleMonitor_H

#include <string>
#include "MMCSThread.h"
#include "MMCSCommandProcessor.h"

class MMCSConsolePort;
class MMCSSocketList;

/*!
 * \brief Parameters passed to MMCSConsoleListener and MMCSConsoleConnectionThread.
 */
class MMCSConsoleMonitorParms
{
public:
    /*!
     * \brief ctor.
     */
    MMCSConsoleMonitorParms(
            MMCSConsolePort* port,      //!< [in]
            MMCSCommandMap* map,        //!< [in]
            MMCSSocketList* socklist    //!< [in]
            ) :
        consolePort(port),
        commandMap(map),
        socketList(socklist),
        replyFormat(0)
    {

    }

    MMCSConsolePort* consolePort; // console port or console connection
    MMCSCommandMap* commandMap;	 // commands that can be executed
    MMCSSocketList* socketList;	 // to keep track of open sockets
    int replyFormat;		 // optional reply format, defaults to 0
};

//******************************************************************************
// MMCSConsoleListener: listen for MMCS console connections
//******************************************************************************
class MMCSConsoleListener : public MMCSThread
{
public:
    MMCSConsoleListener() : MMCSThread() {};
    void* threadStart();
};

/*!
 *  \brief MMCSConsoleConnection: execute commands from MMCS console connections.
 */
class MMCSConsoleConnectionThread : public MMCSThread
{
public:
    MMCSConsoleConnectionThread() : MMCSThread() {};
    void* threadStart();
};

#endif
