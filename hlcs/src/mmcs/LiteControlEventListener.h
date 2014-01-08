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

#ifndef __LITECONTROLEVENTLISTENER
#define __LITECONTROLEVENTLISTENER

// Handles RAS events from mcServer that when there is no specific
// ControlEventListener and this is the mmcs-lite environment

#include "MMCSCommand_lite.h"
#include <vector>

class MMCSCommandReply;

class LiteControlEventListener: public LiteBlockController
{
    friend class MailboxMonitor;
    friend class MailboxListener;
public:
    static	LiteControlEventListener* getLiteControlEventListener();
    virtual	~LiteControlEventListener();
    void        disconnect();
protected:
    LiteControlEventListener();
private:
    // singleton instance
    static	LiteControlEventListener* _liteControlEventListener;

    // the following is to prevent accidental bitwise copying of instances of this class
    LiteControlEventListener(const LiteControlEventListener&);
    LiteControlEventListener& operator=(const LiteControlEventListener&);

};

#endif
