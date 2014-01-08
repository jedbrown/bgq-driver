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
#define PBSTART() \
    {                                               \
        CxxSockets::Message side_msg;               \
        side_sock->Receive(side_msg);               \
        assert(side_msg.str() == "START");          \
    }                                               \

// Tells the other side we're ready
#define PBREADY() \
    {                                                            \
        CxxSockets::Message side_msg("READY");                   \
        side_sock->Send(side_msg);                               \
    }                                                            \

// Waits for the other side to complete its processing and then
// sends a READY message.
#define PBDONE() \
    {                                                            \
        CxxSockets::Message side_msg;                            \
        side_sock->Receive(side_msg);                            \
        std::cout << "MSG: " << side_msg.str() << std::endl;     \
        assert(side_msg.str() == "DONE");                        \
    }                                                            \
    {                                                            \
        CxxSockets::Message side_msg("READY");                   \
        std::cout << "Sending " << side_msg.str() << std::endl;  \
        side_sock->Send(side_msg);                               \
    }                                                            \

#define PBSUCCESS() \
    {                                                            \
        CxxSockets::Message side_msg("SUCCESS");                 \
        side_sock->Send(side_msg);                               \
    }                                                            \

#define PBFAIL() \
    {                                                            \
        CxxSockets::Message side_msg("FAIL");                    \
        side_sock->Send(side_msg);                               \
    }                                                            \

#define PBWAITSTRING(x) \
    {                                                            \
        CxxSockets::Message side_msg;                            \
        side_sock->Receive(side_msg);                            \
        x = side_msg.str();                                      \
    }                                                            \
