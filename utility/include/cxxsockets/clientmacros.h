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
#include <fstream>
#define PBSTART() \
    {                                                            \
        CxxSockets::Message side_msg("START");                   \
        side_sock->Send(side_msg);                               \
    }                                                            \
    {                                                            \
        CxxSockets::Message side_msg;                            \
        side_sock->Receive(side_msg);                            \
        assert(side_msg.str() == "READY");                       \
    }                                                            \

#define PBDONE() \
    {                                                            \
        CxxSockets::Message side_msg("DONE");                    \
        side_sock->Send(side_msg);                               \
    }                                                            \
    {                                                            \
        CxxSockets::Message side_msg;                            \
        side_sock->Receive(side_msg);                            \
        assert(side_msg.str() == "READY");                       \
        ++completed;                                             \
    }                                                            \

#define PBRESULT() \
    {                                                                   \
        CxxSockets::Message side_msg;                                   \
        side_sock->Receive(side_msg);                                   \
        if(side_msg.str() != "SUCCESS") {                               \
            ++failed;                                                   \
        }                                                               \
        assert(side_msg.str() == "SUCCESS");                            \
    }                                                                   \

#define PBSENDSTRING(x) \
    {                                                            \
        CxxSockets::Message side_msg(x);                         \
        side_sock->Send(side_msg);                               \
    }                                                            \

#define PBREADYWAIT() \
    {                                                            \
        CxxSockets::Message side_msg;                            \
        side_sock->Receive(side_msg);                            \
        assert(side_msg.str() == "READY");                       \
    }                                                            \
