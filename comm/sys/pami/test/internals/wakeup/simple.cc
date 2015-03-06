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
 * \file test/internals/wakeup/simple.cc
 * \brief Simple standalone wakeup component software test.
 */

#include <pami.h>

#include "Global.h"
#include "components/wakeup/WakeupSemaphore.h"
#include "components/wakeup/WakeupPrepareNoop.h"

//#define NOOP_PREPARE

class MyPrepareToSleep : public PAMI::Interface::WakeupPrepare<MyPrepareToSleep>
{
  public:

    friend class PAMI::Interface::WakeupPrepare<MyPrepareToSleep>;

    inline bool prepareToSleepFunctor_impl()
    {
      fprintf (stdout, "[%d] now ready to sleep\n", task);
      return true;
    };

    pami_task_t task;
};

int main(int argc, char ** argv)
{
  if (__global.mapping.size() < 2)
  {
    fprintf (stderr, "Error. Test requires > 1 tasks.\n");
    return 1;
  }

  pami_task_t task = __global.mapping.task();
  size_t peers = 0;
  __global.mapping.nodePeers(peers);

  PAMI::Wakeup::Semaphore<> * wakeup =
    PAMI::Wakeup::Semaphore<>::generate (peers);

  fprintf (stdout, "[%d] Start test\n", task);

  if (task == 0)
  {
    int i;
    for (i=0; i<4; i++)
    {
      fprintf (stdout, "[%d] %d\n", task, i);
      sleep (1);
    }
    fprintf (stdout, "[%d] wake up task 1 ...\n", task);
    wakeup[1].wakeup();
    fprintf (stdout, "[%d] ... after wake up.\n", task);
    sleep (1);
    fprintf (stdout, "[%d] %d\n", task, i++);
  }
  else if (task == 1)
  {
    fprintf (stdout, "[%d] go to sleep ...\n", task);
#ifdef NOOP_PREPARE
    PAMI::Wakeup::Prepare::Noop prepare;
#else
    MyPrepareToSleep prepare;
    prepare.task = task;
#endif
    wakeup[1].sleep(prepare);
    fprintf (stdout, "[%d] ... now awake!\n", task);
  }

  fprintf (stdout, "[%d] Test completed\n", task);

  return 0;
}
