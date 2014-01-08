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

#ifndef BGWS_TYPES_HPP_
#define BGWS_TYPES_HPP_


/*! \file
 * \brief Contains forward declarations.
 */


#include <boost/shared_ptr.hpp>


namespace bgws {

    class BgwsServer;
    class BlueGene;
    class BlockingOperationsThreadPool;
    class CheckUserAdminExecutor;
    class PwauthExecutor;
    class ResponderFactory;
    class ServerStats;

    class Session;
    typedef boost::shared_ptr<Session> SessionPtr;

    class Sessions;
    class UserInfo;

} // namespace bgws


#endif
