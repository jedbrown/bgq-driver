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

#ifndef LITECONTROLEVENTLISTENER
#define LITECONTROLEVENTLISTENER

// Handles RAS events from mcServer that when there is no specific
// ControlEventListener and this is the mmcs-lite environment

#include "BlockController.h"

#include <vector>


namespace mmcs_client { class CommandReply; }


namespace mmcs {
namespace lite {


class LiteControlEventListener : public lite::BlockController
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


} } // namespace mmcs::lite

#endif
