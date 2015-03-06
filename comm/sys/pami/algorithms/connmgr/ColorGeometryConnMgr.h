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
 * \file algorithms/connmgr/ColorGeometryConnMgr.h
 * \brief ???
 */

#ifndef __algorithms_connmgr_ColorGeometryConnMgr_h__
#define __algorithms_connmgr_ColorGeometryConnMgr_h__

#include "algorithms/connmgr/ConnectionManager.h"
namespace CCMI
{
  namespace ConnectionManager
  {
    class ColorGeometryConnMgr : public ConnectionManager<ColorGeometryConnMgr>
    {
      public:
        enum
        {
          color0 = 0x00000000, color1 = 0x00010000, color2 = 0x00020000, color3 = 0x00030000, color4 = 0x00040000, color5 = 0x00050000
        };


        // the conn parm is what the connection id is, since multiple color executors
        //   will still only have one connection manager this parm will be the last
        //   color.
        /* This class is really just a place holder for future extensions.  */
        ColorGeometryConnMgr (int conn = 0)
            : ConnectionManager<ColorGeometryConnMgr>()
        {
          setNumConnections_impl (conn == 0 ? 1 : conn );
        }

        inline void setNumConnections_impl (size_t sz)
        {
          _numConnections = sz;
        }

        ///
        /// \brief return the connection id given a set of inputs
        /// \param comm the communicator id of the collective
        /// \param root the root of the collective operation
        /// \param color the dimension of the collective operation
        inline unsigned getConnectionId_impl (unsigned comm, unsigned root,
                                              unsigned color, unsigned phase, unsigned dst = (unsigned) - 1)
        {
          (void)root;(void)phase;(void)dst;
          PAMI_assert(_numConnections != 0);

          unsigned connid = 0;

          if (color == 0 || // this is for legacy operation, we expect 1-based colors
              color == 1)
            connid = comm | color0;
          else if (color == 2)
            connid = comm | color1;
          else if (color == 3)
            connid = comm | color2;
          else if (color == 4)
            connid = comm | color3;
          else if (color == 5)
            connid = comm | color4;
          else if (color == 6)
            connid = comm | color5;
          else
            PAMI_assert(color <= 6);

          return connid;
        }

        inline unsigned getRecvConnectionId_impl (unsigned comm, unsigned root,
                                                  unsigned src, unsigned phase, unsigned color)
        {
          (void)src;
          return getConnectionId_impl (comm, root, color, phase);
        }

        virtual int getNumConnections_impl ()
        {
          return _numConnections;
        }

      private:
        size_t    _numConnections;
    };
  };
};

#endif
