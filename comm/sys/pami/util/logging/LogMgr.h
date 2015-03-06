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
 * \file util/logging/LogMgr.h
 * \brief ???
 */

#ifndef __util_logging_LogMgr_h__
#define __util_logging_LogMgr_h__

#include  <stdio.h>
#include  <stdlib.h>
#include  "algorithms/ccmi.h"

namespace CCMI
{
  namespace Logging
  {

#define  NTIMERS            16
#define  ENABLE_PROFILING   0

    class LogMgr
    {
    public:
      inline LogMgr ()
      {
        reset ();
      }

      inline void reset ()
      {
#if ENABLE_PROFILING
        for(int count = 0; count < NTIMERS; count ++)
        {
          _start [count] = 0;
          _count [count] = 0;
          _sum [count] = 0;
        }

        _curIndex = 0;
#endif
      }

      inline unsigned registerEvent (const char *event_name)
      {
#if ENABLE_PROFILING
        CCMI_assert (_curIndex < NTIMERS);

        strncpy(_logHandle[_curIndex], event_name, 256);
        _curIndex ++;
        return _curIndex - 1;
#else

        return 0;
#endif
      }

      inline void startCounter (int id)
      {
#if ENABLE_PROFILING
        _start[id] =  (unsigned long long) CCMI_GetTimebase();
#endif
      }

      inline void stopCounter (int id, int n=1)
      {
#if ENABLE_PROFILING
        _sum[id] += (unsigned)( (unsigned long long) CCMI_GetTimebase() -_start[id]);
        _count[id] += n;
#endif
      }
#if 0
      void   dumpTimers(FILE *fp, CollectiveMapping *map)
      {
#if ENABLE_PROFILING
        for(int idx = 0; idx < NTIMERS; idx ++)
        {
          if(_count [idx] > 0)
            fprintf (fp,
                     "%d: Event %s has overhead of %d cycles over %d calls\n",
                     map->rank(),
                     _logHandle[idx], (unsigned)(_sum[idx]/_count[idx]),
                     _count[idx]);
        }
#endif
      }
#endif
      static void setLogMgr (LogMgr *l)
      {
        _staticLogMgr = l;
      }
      static LogMgr *getLogMgr ()
      {
        return _staticLogMgr;
      }

    private:
      char                    _logHandle [NTIMERS][256];
      unsigned long long      _start [NTIMERS];
      unsigned long long      _sum   [NTIMERS];
      unsigned                _count [NTIMERS];
      unsigned                _curIndex;


      static LogMgr *_staticLogMgr;
    };
  };
};

#endif
