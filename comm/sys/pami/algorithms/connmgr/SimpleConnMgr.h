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
 * \file algorithms/connmgr/SimpleConnMgr.h
 * \brief ???
 */

#ifndef __algorithms_connmgr_SimpleConnMgr_h__
#define __algorithms_connmgr_SimpleConnMgr_h__

#include "algorithms/connmgr/ConnectionManager.h"

namespace CCMI
{
  namespace ConnectionManager
  {
    class SimpleConnMgr : public ConnectionManager<SimpleConnMgr>
    {
      protected:
        int   _connid;
        int   _nconn;
      public:
        // the conn parm is what the connection id is, since multiple color executors
        //   will still only have one connection manager this parm will be the last
        //   color.
        /* This class is really just a place holder for future extensions.  */
        SimpleConnMgr (int conn = 0)
            : ConnectionManager<SimpleConnMgr>()
            , _connid(conn)
        {
          this->setNumConnections (1);
        }
        ///
        /// \brief Virtual destructors make compilers happy.
        ///
        virtual ~SimpleConnMgr() {};

        ///
        /// \brief return the connection id given a set of inputs
        /// \param comm the communicator id of the collective
        /// \param root the root of the collective operation
        /// \param color the dimension of the collective operation
        virtual unsigned getConnectionId_impl (unsigned comm, unsigned root,
                                               unsigned color, unsigned phase, unsigned dst = (unsigned) - 1)
        {
          (void)comm;(void)root;(void)color;(void)phase;(void)dst;
          return _connid;
        }

        virtual unsigned getRecvConnectionId_impl (unsigned comm, unsigned root,
                                                   unsigned src, unsigned phase, unsigned color)
        {
          (void)comm;(void)root;(void)color;(void)phase;(void)src;
          return _connid;
        }

        virtual void setNumConnections_impl(int nconn)
        {
          _nconn = nconn;
        }

        virtual int getNumConnections_impl ()
        {
          return _nconn;
        }

    };
  };
};

#endif
