/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file algorithms/connmgr/ConnectionManager.h
 * \brief ???
 */

#ifndef __algorithms_connmgr_ConnectionManager_h__
#define __algorithms_connmgr_ConnectionManager_h__

#include "TypeDefs.h"

namespace CCMI
{
  namespace ConnectionManager
  {

    class BaseConnectionManager
    {
      public:
        BaseConnectionManager() {}
        virtual ~BaseConnectionManager() {}

        virtual void     setNumConnections (int nconn) = 0;
        virtual int      getNumConnections() = 0;
        virtual unsigned getConnectionId (unsigned comm, unsigned root,
                                          unsigned color, unsigned phase, unsigned dst) = 0;
        virtual unsigned getRecvConnectionId (unsigned comm, unsigned root,
                                              unsigned src, unsigned phase, unsigned color) = 0;
    };

    typedef unsigned (*GetKeyFn)      (unsigned                root,
                                       unsigned                connid,
                                       PAMI_GEOMETRY_CLASS    *geometry,
                                       ConnectionManager::BaseConnectionManager **connmgr);


    template <class T_ConnectionManager>
    class ConnectionManager: public BaseConnectionManager
    {
      public:
        ConnectionManager () {}
        inline void     setNumConnections (int nconn);
        inline int      getNumConnections();
        inline unsigned getConnectionId (unsigned comm, unsigned root,
                                         unsigned color, unsigned phase, unsigned dst);
        inline unsigned getRecvConnectionId (unsigned comm, unsigned root,
                                             unsigned src, unsigned phase, unsigned color);
    }; //- ConnectionManager


    template <class T_ConnectionManager>
    inline void ConnectionManager<T_ConnectionManager>::setNumConnections(int nconn)
    {
      static_cast<T_ConnectionManager*>(this)->setNumConnections_impl(nconn);
    }

    template <class T_ConnectionManager>
    inline int      ConnectionManager<T_ConnectionManager>::getNumConnections()
    {
      return static_cast<T_ConnectionManager*>(this)->getNumConnections_impl();
    }

    template <class T_ConnectionManager>
    inline unsigned ConnectionManager<T_ConnectionManager>::getConnectionId (unsigned comm, unsigned root,
                                                                             unsigned color, unsigned phase, unsigned dst)
    {
      return static_cast<T_ConnectionManager*>(this)->getConnectionId_impl(comm,
                                                                           root,
                                                                           color,
                                                                           phase,
                                                                           dst);
    }

    template <class T_ConnectionManager>
    inline unsigned ConnectionManager<T_ConnectionManager>::getRecvConnectionId (unsigned comm, unsigned root,
                                                                                 unsigned src, unsigned phase, unsigned color)
    {
      return static_cast<T_ConnectionManager*>(this)->getRecvConnectionId_impl(comm,
                                                                               root,
                                                                               src,
                                                                               phase,
                                                                               color);
    }
  };  //- namespace ConnectionManager
};  //- CCMI

#endif
