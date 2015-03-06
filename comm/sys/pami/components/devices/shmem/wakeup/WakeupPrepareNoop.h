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
///
/// \file components/devices/shmem/wakeup/WakeupPrepareNoop.h
/// \brief Empty "noop" implementation of the wakeup prepare interface
///
#ifndef __components_devices_shmem_wakeup_WakeupPrepareNoop_h__
#define __components_devices_shmem_wakeup_WakeupPrepareNoop_h__

#include "components/wakeup/WakeupInterface.h"

namespace PAMI
{
  namespace Wakeup
  {
    namespace Prepare
    {
      class Noop : public Interface::WakeupPrepare <Noop>
      {
        public:
          friend class Interface::WakeupPrepare <Noop>;
          inline bool prepareToSleepFunctor_impl() { return true; };
      }; // class PAMI::Wakeup::Prepare::Noop
    }; // namespace PAMI::Wakeup::Prepare
  }; // namespace PAMI::Wakeup
}; // namespace PAMI

#endif // __components_wakeup_WakeupPrepareNoop_h__
