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

#ifndef __algorithms_connmgr_ColorMapConnMgr_h__
#define __algorithms_connmgr_ColorMapConnMgr_h__

#include "ConnectionManager.h"

namespace CCMI
{
  namespace ConnectionManager
  {
    class ColorMapConnMgr : public ConnectionManager<ColorMapConnMgr>
    {
      static const unsigned MaxColors = 12;
      uint8_t    _colormap[MaxColors];
    public:
      
      /// Default Constructor
      ColorMapConnMgr () : ConnectionManager<ColorMapConnMgr> ()
      {
      }

      /// Constructor
      ColorMapConnMgr (unsigned nconn) : ConnectionManager<ColorMapConnMgr> ()
      {
      }

      void reset() {
	memset(_colormap, 0, sizeof(_colormap));
      }

      inline void setConnections (unsigned       * colors, 
				  unsigned       * connids, 
				  unsigned         nconn) 
      {
	for (uint i = 0; i < nconn; ++i) {
	  //fprintf(stderr, "Mapping color %d to conn %d\n", colors[i], connids[i]);
	  CCMI_assert(colors[i] < MaxColors);
	  _colormap[colors[i]] = (uint8_t)connids[i];
	}
      }

      inline void setNumConnections_impl (size_t sz)
	{
	}
      
      ///
      /// \brief return the connection id given a set of inputs
      /// \param comm the communicator id of the collective
      /// \param root the root of the collective operation
      /// \param color the dimension of the collective operation
      inline unsigned getConnectionId_impl (unsigned comm, unsigned root,
                                            unsigned color, unsigned phase, unsigned dst=(unsigned)-1)
	{
	  CCMI_assert(color < MaxColors);
	  return _colormap[color];
	}
      
      inline unsigned getRecvConnectionId_impl (unsigned comm, unsigned root,
						unsigned src, unsigned phase, unsigned color)
	{
	  CCMI_assert(color < MaxColors);
	  return _colormap[color];
	}
      
      virtual int getNumConnections_impl ()
      {
	return MaxColors;
      }
    };
  };
};

#endif
