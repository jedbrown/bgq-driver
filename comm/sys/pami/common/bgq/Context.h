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
 * \file common/bgq/Context.h
 * \brief PAMI Blue Gene\Q specific context implementation.
 */
#ifndef __common_bgq_Context_h__
#define __common_bgq_Context_h__


#include <stdlib.h>
#include <string.h>
#include <new>

#include <pami.h>
#include "common/ContextInterface.h"
#include "common/default/Dispatch.h"

#include "components/devices/generic/Device.h"
#include "components/devices/misc/ProgressFunctionMsg.h"
#include "components/devices/misc/AtomicBarrierMsg.h"
#include "components/devices/misc/AtomicMutexMsg.h"
#include "components/atomic/native/NativeCounter.h"
#include "components/atomic/bgq/L2Counter.h"
#include "components/atomic/counter/CounterMutex.h"
#include "components/atomic/indirect/IndirectCounter.h"
#include "components/atomic/indirect/IndirectMutex.h"

#include "api/extension/c/async_progress/ProgressExtension.h"

#include "components/memory/MemoryAllocator.h"

#include "components/memory/MemoryManager.h"
#include "Memregion.h"

#include "p2p/protocols/RGet.h"
#include "p2p/protocols/rget/GetRdma.h"
#include "p2p/protocols/rput/PutRdma.h"
#include "p2p/protocols/get/GetOverSend.h"
#include "p2p/protocols/put/PutOverSend.h"
#include "p2p/protocols/rmw/RmwOverSend.h"
#include "p2p/protocols/send/eager/Eager.h"
#include "p2p/protocols/send/composite/Composite.h"
#include "p2p/protocols/fence/deterministic/DeterministicFence.h"

#include "TypeDefs.h"
#include "algorithms/geometry/BGQMultiRegistration.h"

#include "algorithms/geometry/P2PCCMIRegistration.h"

#include "util/trace.h"

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG
#define DO_TRACE_ENTEREXIT 0
#define DO_TRACE_DEBUG     0
#define TRACE_FORMAT_DETAILED(...) //TRACE_FORMAT(__VA_ARGS__)
#include "algorithms/geometry/GeometryOptimizer.h"

namespace PAMI
{

  typedef Mutex::Indirect<Mutex::Counter<Counter::BGQ::L2> >  ContextLock;

  typedef CollRegistration::P2P::CCMIRegistration < BGQGeometry,
  ProtocolAllocator,
  CCMI::Adaptor::P2PBarrier::BinomialBarrier,
  CCMI::Adaptor::P2PBarrier::BinomialBarrierFactory > CCMIRegistration;

  typedef CollRegistration::P2P::CCMIRegistration < BGQGeometry,
  ProtocolAllocator,
  CCMI::Adaptor::P2PBarrier::BinomialBarrierKey2,
  CCMI::Adaptor::P2PBarrier::BinomialBarrierFactoryKey2,
  0 > CCMIRegistrationKey2;

  typedef CollRegistration::BGQMultiRegistration < BGQGeometry,
  ShmemDevice,
  AllSidedShmemNI,
  MUDevice,
  MUGlobalNI,
  MUDputNI,
  MUShmemAxialDputNI,
  MidProtocolAllocator,
  BigProtocolAllocator > BGQRegistration;

  // Shmem NI class that overrides the metadata name
  template<class T_Parent>
  class ShmemNI_metadata : public T_Parent
  {
    char name[48];
    const char* niName;
  public:
    ShmemNI_metadata(pami_client_t client, pami_context_t context, size_t context_id, size_t client_id) :
    T_Parent(client, context, context_id, client_id),
    niName("SHMEM:-")
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p>",this);
      TRACE_FN_EXIT();
    }
    ///
    /// \brief NI hook to override metadata for collective
    ///
    void metadata(pami_metadata_t *m, pami_xfer_type_t t)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p> %s type %u",this, m->name, t);
      // Following assumes that the string is "XXX:NAME:DEVICE:DEVICE" !
      // We'll replace DEVICE:DEVICE with our override string or leave
      // it alone if our assumption is obviously wrong (strstr() fails)
      char* s = strstr(m->name, ":"); // Find  :NAME:DEVICE:DEVICE

      if(s)
      {
        s++;
        s = strstr(s, ":"); // Find  :DEVICE:DEVICE

        if(s)
        {
          s++; // Find  DEVICE:DEVICE

          if(sizeof(name) >= (s - m->name) + strlen(niName) + 1) // Is it reasonable?
          {
            //PAMI_assertf(sizeof(name) >= (s-m->name)+strlen(niName)+1,"%zu >= %zu",sizeof(name),(s-m->name)+strlen(niName)+1);
            memcpy(name, m->name, (s - m->name));
            name[(s-m->name)] = 0x00;
            strcat(name, niName);
            m->name = name;
          }
        }
      }

      TRACE_FORMAT_DETAILED("<%p> %s",this, m->name);
      TRACE_FN_EXIT();
    };
  };

  // Shmem NI factory for ShmemNI_metadata<ShmemNI_AM>/ShmemNI_metadata<ShmemNI_AS>
  // which will override analyze to only enable on shmem-only geometries
  template <class T_NIAM, class T_NIAS>
  class ShmemNIFactoryT : public NativeInterfaceCommon::NativeInterfaceFactory <MidProtocolAllocator,  ShmemNI_metadata<T_NIAM>,  ShmemNI_metadata<T_NIAS>, ShmemEager, ShmemDevice>
  {
  public:
    ShmemNIFactoryT( pami_client_t       client,
                     pami_context_t      context,
                     size_t              clientid,
                     size_t              contextid,
                     ShmemDevice          & device,
                     MidProtocolAllocator       & allocator) :
    NativeInterfaceCommon::NativeInterfaceFactory<MidProtocolAllocator,  ShmemNI_metadata<T_NIAM>,  ShmemNI_metadata<T_NIAS>, ShmemEager, ShmemDevice>( client,
                                                                                                                                                        context,
                                                                                                                                                        clientid,
                                                                                                                                                        contextid,
                                                                                                                                                        device,
                                                                                                                                                        allocator)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p>",this);
      TRACE_FN_EXIT();
    };

    pami_result_t  analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag)
    {
      TRACE_FN_ENTER();
      *flag = 0;
      PAMI::Topology *global = (PAMI::Topology *) topology;
      PAMI::Topology local;
      global->subTopologyLocalToMe(&local);
      pami_result_t result = PAMI_UNIMPL; // not supported

      if(local.size() == global->size()) result = PAMI_SUCCESS; // all local/shmem - ok

      TRACE_FORMAT_DETAILED("<%p> result %u/%u",this, result, *flag);
      TRACE_FN_EXIT();
      return result;
    };
  };

  typedef ShmemNIFactoryT<ShmemNI_AM, ShmemNI_AS> ShmemNIFactory;
  typedef ShmemNIFactoryT<ShmemNI_AM_AMC, ShmemNI_AS_AMC> ShmemNIFactory_AMC;

  // MU NI class that overrides the metadata name and possibly the range (templatized)
  template < class T_Parent, class T_Allocator, int T_Range_Hi = 0 >
  class MUNI_metadata : public T_Parent
  {
  protected:
    char name[48];
    const char* niName;
  public:
    MUNI_metadata(MUDevice &device, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id) :
    T_Parent(device, allocator, client, context, context_id, client_id, dispatch_id)
    {
      TRACE_FN_ENTER();

      if(T_Range_Hi)
      {
        niName = "-:ShortMU";
      }
      else niName = "-:MU";

      TRACE_FORMAT_DETAILED("<%p>%s,%d",this, niName, T_Range_Hi);
      TRACE_FN_EXIT();
    }

    MUNI_metadata(pami_client_t client, pami_context_t context, size_t context_id, size_t client_id) :
    T_Parent(client, context, context_id, client_id)
    {
      TRACE_FN_ENTER();

      if(T_Range_Hi)
      {
        niName = "-:ShortMU";
      }
      else niName = "-:MU";

      TRACE_FORMAT_DETAILED("<%p>%s,%d",this, niName, T_Range_Hi);
      TRACE_FN_EXIT();
    }
    ///
    /// \brief NI hook to override metadata for collective
    ///
    void metadata(pami_metadata_t *m, pami_xfer_type_t t)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p> %s type %u",this, m->name, t);
      // Following assumes that the string is "XXX:NAME:DEVICE:DEVICE" !
      // We'll replace DEVICE:DEVICE with our override string or leave
      // it alone if our assumption is obviously wrong (strstr() fails)
      char* s = strstr(m->name, ":"); // Find  :NAME:DEVICE:DEVICE

      if(s)
      {
        s++;
        s = strstr(s, ":"); // Find  :DEVICE:DEVICE

        if(s)
        {
          s++; // Find  DEVICE:DEVICE

          if(sizeof(name) >= (s - m->name) + strlen(niName) + 1) // Is it reasonable?
          {
            //PAMI_assertf(sizeof(name) >= (s-m->name)+strlen(niName)+1,"%zu >= %zu",sizeof(name),(s-m->name)+strlen(niName)+1);
            memcpy(name, m->name, (s - m->name));
            name[(s-m->name)] = 0x00;
            strcat(name, niName);
            m->name = name;
          }
        }
      }

      TRACE_FORMAT_DETAILED("<%p> %s",this, m->name);

      if(T_Range_Hi)
      {
        /* Short binomial has metadata */
        s = strstr(m->name, "I0:Binomial:-:ShortMU");

        if(s != NULL)
        {
          m->check_correct.values.contigsflags = 1;
          m->check_correct.values.contigrflags = 1;
        }

        // These only work for very small msg/number of ranks
        if((s != NULL) &&
           ((t == PAMI_XFER_ALLGATHER       ) ||
            (t == PAMI_XFER_ALLGATHERV      ) ||
            (t == PAMI_XFER_ALLGATHERV_INT  ) ||
            (t == PAMI_XFER_SCATTER         ) ||
            (t == PAMI_XFER_SCATTERV        ) ||
            (t == PAMI_XFER_SCATTERV_INT    ) ||
            (t == PAMI_XFER_GATHER          ) ||
            (t == PAMI_XFER_GATHERV         ) ||
            (t == PAMI_XFER_GATHERV_INT     )
           ))
        {
          name[s-m->name] = 'X';   //Make experimental
          /// \todo calculate range_hi based on geometry size?
          /// but for now, disable it by invalid range
          m->check_correct.values.rangeminmax = 1;
          m->range_lo                         = (size_t) - 1;
          m->range_hi                         = 0;
        }
        else
        {
          m->check_correct.values.rangeminmax = 1;
          m->range_hi                         = T_Range_Hi;
        }
      }

      TRACE_FORMAT_DETAILED("<%p> %s, rangeminmax %u, range_hi %zu",this, m->name,
                   m->check_correct.values.rangeminmax, m->range_hi);
      TRACE_FN_EXIT();
    };
  };

  // MU NI class that overrides the metadata name and possibly the range (templatized)
  template<class T_Parent, class T_Allocator>
  class MUNI_2Device_metadata : public T_Parent
  {
  protected:
    char name[48];
    const char* niName;
  public:
    MUNI_2Device_metadata(MUDevice &device, ShmemDevice &device2, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id) :
    T_Parent(device, device2, allocator, client, context, context_id, client_id, dispatch_id)
    {
      TRACE_FN_ENTER();
      niName = "SHMEM:MU";
      TRACE_FORMAT_DETAILED("<%p>%s",this, niName);
      TRACE_FN_EXIT();
    }

    MUNI_2Device_metadata(MUDevice &device, T_Allocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id) :
    T_Parent(device, allocator, client, context, context_id, client_id, dispatch_id)
    {
      TRACE_FN_ENTER();
      niName = "SHMEM:MU";
      TRACE_FORMAT_DETAILED("<%p>%s",this, niName);
      TRACE_FN_EXIT();
    }

    ///
    /// \brief NI hook to override metadata for collective
    ///
    void metadata(pami_metadata_t *m, pami_xfer_type_t t)
    {
      TRACE_FN_ENTER();
      //printf("<%p> %s type %u cur niName %s",this,m->name,t, niName);
      char *s = strstr(m->name, "P2P");
      PAMI_assertf(sizeof(name) >= (s - m->name) + strlen(niName) + 1, "%zu >= %zu", sizeof(name), (s - m->name) + strlen(niName) + 1);
      memcpy(name, m->name, (s - m->name));
      name[(s-m->name)] = 0x00;
      strcat(name, niName);
      s = strstr(m->name, "X0:Ring");

      if(s != NULL)
      {
        name[s-m->name] = 'I';   //Make optimized algorithms I0:
        m->check_correct.values.rangeminmax = 1;
        m->range_lo                         = 64 * 1024; //Bandwidth protocol (arbitrarily limit to >64k)
      }

      m->name = name;
      TRACE_FORMAT_DETAILED("<%p> %s",this, m->name);

      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;

      TRACE_FN_EXIT();
    }
  };

  // MU NI factory for MUNI_metadata<MUNI_AM>/MUNI_metadata<MUNI_AS>
  typedef NativeInterfaceCommon::NativeInterfaceFactory <BigProtocolAllocator,  MUNI_metadata<MUNI_AM, BigProtocolAllocator>,  MUNI_metadata<MUNI_AS, MidProtocolAllocator>, MUEager, MUDevice> MUNIFactory;
  typedef NativeInterfaceCommon::NativeInterfaceFactory <BigProtocolAllocator,  MUNI_metadata<MUNI_AM_AMC, BigProtocolAllocator>,  MUNI_metadata<MUNI_AS_AMC, MidProtocolAllocator>, MUEager, MUDevice> MUNIFactory_AMC;

  // MU NI factory for MUNI_metadata<MUAMMulticastNI,(512-16)> which will
  // override analyze to set query-required for (short) range metadata
  class MUAMMulticastFactory : public BGQNativeInterfaceFactory < BigProtocolAllocator, MUNI_metadata < MUAMMulticastNI, BigProtocolAllocator, (512 - 16) > , MUDevice, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE >
  {
  public:
    MUAMMulticastFactory( pami_client_t       client,
                          pami_context_t      context,
                          size_t              clientid,
                          size_t              contextid,
                          MUDevice          & device,
                          BigProtocolAllocator       & allocator) :
    BGQNativeInterfaceFactory < BigProtocolAllocator, MUNI_metadata < MUAMMulticastNI, BigProtocolAllocator, (512 - 16) > , MUDevice, CCMI::Interfaces::NativeInterfaceFactory::MULTICAST, CCMI::Interfaces::NativeInterfaceFactory::ACTIVE_MESSAGE >
    ( client,
      context,
      clientid,
      contextid,
      device,
      allocator)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p>",this);
      TRACE_FN_EXIT();
    };
    pami_result_t  analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag)
    {
      TRACE_FN_ENTER();
      *flag = 0;
      TRACE_FORMAT_DETAILED("<%p> result %u/%u",this, PAMI_OTHER, *flag);
      TRACE_FN_EXIT();
      return PAMI_OTHER;// query required (short metadata)
    };
  };

  // MU NI class that overrides the metadata name and possibly the range (templatized)
  class MUDputNI_metadata : public MUNI_metadata<MUDputNI, BigProtocolAllocator>
  {
  public:
    MUDputNI_metadata(MUDevice &device, BigProtocolAllocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id) :
    MUNI_metadata<MUDputNI, BigProtocolAllocator>(device, allocator, client, context, context_id, client_id, dispatch_id)
    {
      this->niName = "-:MUDput";
    }

    void metadata(pami_metadata_t *m, pami_xfer_type_t t)
    {
      MUNI_metadata<MUDputNI, BigProtocolAllocator>::metadata(m, t);
      char* s = strstr(m->name, "X0:Ring");

      if(s != NULL)
      {
        name[s-m->name] = 'I';   //Make optimized algorithms I0:
        m->check_correct.values.rangeminmax = 1;
        m->range_lo                         = 64 * 1024; //Bandwidth protocol (arbitrarily limit to >64k)
      }

      m->check_correct.values.contigsflags = 1;
      m->check_correct.values.contigrflags = 1;
    }
  };

  // MUShmemDputNI class that overrides the metadata name and possibly the range (templatized)
  class MUShmemDputNI_metadata : public MUNI_2Device_metadata<MUShmemDputNI, BigProtocolAllocator>
  {
  public:
    MUShmemDputNI_metadata(MUDevice &device, ShmemDevice &device2, BigProtocolAllocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id) :
    MUNI_2Device_metadata<MUShmemDputNI, BigProtocolAllocator>(device, device2, allocator, client, context, context_id, client_id, dispatch_id)
    {
      this->niName = "Shmem:MUDput";
    }

    MUShmemDputNI_metadata(MUDevice &device, BigProtocolAllocator &allocator, pami_client_t client, pami_context_t context, size_t context_id, size_t client_id, int *dispatch_id) :
    MUNI_2Device_metadata<MUShmemDputNI, BigProtocolAllocator>(device, allocator, client, context, context_id, client_id, dispatch_id)
    {
      this->niName = "Shmem:MUDput";
    }
  };

  // MU NI factory for MUNI_metadata<MUNI_AM>/MUNI_metadata<MUNI_AS> which will
  // override analyze to set querynnn-required for non-contig
  class MUDputNIFactory : public BGQNativeInterfaceFactory < BigProtocolAllocator,
  MUDputNI_metadata,
  MUDevice,
  CCMI::Interfaces::NativeInterfaceFactory::MULTICAST,
  CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 16 >
  {
  public:
    MUDputNIFactory( pami_client_t       client,
                     pami_context_t      context,
                     size_t              clientid,
                     size_t              contextid,
                     MUDevice          & device,
                     BigProtocolAllocator       & allocator) :
    BGQNativeInterfaceFactory < BigProtocolAllocator,
    MUDputNI_metadata,
    MUDevice,
    CCMI::Interfaces::NativeInterfaceFactory::MULTICAST,
    CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 16 >
    ( client,
      context,
      clientid,
      contextid,
      device,
      allocator)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p>",this);
      TRACE_FN_EXIT();
    };
    pami_result_t  analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag)
    {
      TRACE_FN_ENTER();
      *flag = 0;
      TRACE_FORMAT_DETAILED("<%p> result %u/%u",this, PAMI_OTHER, *flag);
      TRACE_FN_EXIT();
      return PAMI_OTHER;// query required (non-contig)
    };
  };


  // MUShmemDput NI factory which will
  // override analyze to set query-required for non-contig
  class MUShmemDputNIFactory : public BGQNativeInterfaceFactory2Device < BigProtocolAllocator,
  MUShmemDputNI_metadata,
  MUDevice,
  ShmemDevice,
  CCMI::Interfaces::NativeInterfaceFactory::MULTICAST,
  CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 16 >
  {
  public:
    MUShmemDputNIFactory( pami_client_t          client,
                          pami_context_t         context,
                          size_t                 clientid,
                          size_t                 contextid,
                          MUDevice             & device1,
                          ShmemDevice          & device2,
                          BigProtocolAllocator & allocator):
    BGQNativeInterfaceFactory2Device < BigProtocolAllocator,
    MUShmemDputNI_metadata,
    MUDevice,
    ShmemDevice,
    CCMI::Interfaces::NativeInterfaceFactory::MULTICAST,
    CCMI::Interfaces::NativeInterfaceFactory::ALLSIDED, 16 >
    ( client,
      context,
      clientid,
      contextid,
      device1,
      device2,
      allocator)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p>",this);
      TRACE_FN_EXIT();
    };
    pami_result_t  analyze(size_t context_id, pami_topology_t *topology, int phase, int* flag)
    {
      TRACE_FN_ENTER();
      *flag = 0;
      TRACE_FORMAT_DETAILED("<%p> result %u/%u",this, PAMI_OTHER, *flag);
      TRACE_FN_EXIT();
      return PAMI_OTHER;// query required (non-contig)
    };
  };


  // Composite (MU/SHMEM) NI class that overrides the metadata name
  template<class T_Parent>
  class CompositeNI_metadata : public T_Parent
  {
    char name[48];
    const char* niName;
  public:
    CompositeNI_metadata(pami_client_t client, pami_context_t context, size_t context_id, size_t client_id) :
    T_Parent(client, context, context_id, client_id),
    niName("SHMEM:MU")
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p>",this);
      TRACE_FN_EXIT();
    }
    ///
    /// \brief NI hook to override metadata for collective
    ///
    void metadata(pami_metadata_t *m, pami_xfer_type_t t)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT_DETAILED("<%p> %s type %u",this, m->name, t);
      // Following assumes that the string is "XXX:NAME:DEVICE:DEVICE" !
      // We'll replace DEVICE:DEVICE with our override string or leave
      // it alone if our assumption is obviously wrong (strstr() fails)
      char* s = strstr(m->name, ":"); // Find  :NAME:DEVICE:DEVICE

      if(s)
      {
        s++;
        s = strstr(s, ":"); // Find  :DEVICE:DEVICE

        if(s)
        {
          s++; // Find  DEVICE:DEVICE

          if(sizeof(name) >= (s - m->name) + strlen(niName) + 1) // Is it reasonable?
          {
            //PAMI_assertf(sizeof(name) >= (s-m->name)+strlen(niName)+1,"%zu >= %zu",sizeof(name),(s-m->name)+strlen(niName)+1);
            memcpy(name, m->name, (s - m->name));
            name[(s-m->name)] = 0x00;
            strcat(name, niName);
            m->name = name;
          }
        }
      }

      TRACE_FORMAT_DETAILED("<%p> %s",this, m->name);
      TRACE_FN_EXIT();
    };
  };




  // Composite (MU/SHMEM) NI factory for CompositeNI_metadata
  typedef NativeInterfaceCommon::NativeInterfaceFactory2Device <MidProtocolAllocator, CompositeNI_metadata<CompositeNI_AM>, CompositeNI_metadata<CompositeNI_AS>, ShmemEager, ShmemDevice, MUEager, MUDevice> CompositeNIFactory;
  typedef NativeInterfaceCommon::NativeInterfaceFactory2Device <MidProtocolAllocator, CompositeNI_metadata<CompositeNI_AM_AMC>, CompositeNI_metadata<CompositeNI_AS_AMC>, ShmemEager, ShmemDevice, MUEager, MUDevice> CompositeNIFactory_AMC;

  /**
   * \brief Class containing all devices used on this platform.
   *
   * This container object governs creation (allocation of device objects),
   * initialization of device objects, and advance of work. Note, typically
   * the devices advance routine is very short - or empty - since it only
   * is checking for received messages (if the device even has reception).
   *
   * The generic device is present in all platforms. This is how context_post
   * works as well as how many (most/all) devices enqueue work.
   */
  class PlatformDeviceList
  {
  public:
    PlatformDeviceList():
    _generics(NULL),
    _shmem(NULL),
    _progfunc(NULL),
    _atombarr(NULL),
#if 0
    _localallreduce(NULL),
    _localbcast(NULL),
    _localreduce(NULL),
#endif
    _mu(NULL),
    _atmmtx(NULL)
    {
    }

    inline size_t maxContexts (size_t clientid, Memory::MemoryManager &mm)
    {
      size_t count, num = (size_t) - 1;

      // Determine how many devices can be constructed using the memory manager
      size_t memory_required = 0;

      if(__global.useshmem())
      {
        memory_required = ShmemDevice::Factory::getInstanceMemoryRequired (clientid, mm);
      }

      if(__global.useMU())
      {
        memory_required += Device::MU::Factory::getInstanceMemoryRequired (clientid, mm);
      }

      if(memory_required > 0)
      {
        count  = (mm.size() / memory_required);
        num = MIN(num, count);
      }

      // Determine how many devices can be constructed using device-specific resources.
      if(__global.useshmem())
      {
        count = ShmemDevice::Factory::getInstanceMaximum (clientid, mm);
        num = MIN(num, count);
      }

      if(__global.useMU())
      {
        count = Device::MU::Factory::getInstanceMaximum (clientid, mm);
        num = MIN(num, count);
      }
#if 0 // debugging code
      PAMI_assert_alwaysf((num < MAX_CONTEXTS), "numContexts %zu\n",num);
#endif
      return MIN(num,MAX_CONTEXTS);
    };

    /**
     * \brief initialize this platform device list
     *
     * This creates arrays (at least 1 element) for each device used in this platform.
     * Note, in some cases there may be only one device instance for the entire
     * process (all clients), but any handling of that (mutexing, etc) is hidden.
     *
     * Device arrays are semi-opaque (we don't know how many
     * elements each has).
     *
     * \param[in] clientid     Client ID (index)
     * \param[in] contextid    Context ID (index)
     */
    inline pami_result_t generate(size_t clientid, size_t num_ctx, Memory::MemoryManager &mm)
    {
      // these calls create (allocate and construct) each element.
      // We don't know how these relate to contexts, they are semi-opaque.
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p>",this);
      TRACE_STRING("device init: generic");
      _generics = PAMI::Device::Generic::Device::Factory::generate(clientid, num_ctx, mm, NULL);

      if(__global.useshmem())
      {
        TRACE_STRING("device init: shmem");
        _shmem = ShmemDevice::Factory::generate(clientid, num_ctx, mm, _generics);
#if 0
        TRACE_STRING("device init: local allreduce wq");
        _localallreduce = PAMI::Device::LocalAllreduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        TRACE_STRING("device init: local bcast wq");
        _localbcast = PAMI::Device::LocalBcastWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
        TRACE_STRING("device init: local reduce wq");
        _localreduce = PAMI::Device::LocalReduceWQDevice::Factory::generate(clientid, num_ctx, mm, _generics);
#endif
      }

      TRACE_STRING("device init: progress function");
      _progfunc = PAMI::Device::ProgressFunctionDev::Factory::generate(clientid, num_ctx, mm, _generics);
      TRACE_STRING("device init: atomic barrier");
      _atombarr = PAMI::Device::AtomicBarrierDev::Factory::generate(clientid, num_ctx, mm, _generics);

      if(__global.useMU())
      {
        TRACE_STRING("device init: MU");
        _mu = Device::MU::Factory::generate(clientid, num_ctx, mm, _generics);
      }

      _atmmtx = PAMI::Device::AtomicMutexDev::Factory::generate(clientid, num_ctx, mm, _generics);
      PAMI_assertf(_atmmtx == _generics, "AtomicMutexDev must be a NillSubDevice");
      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    /**
     * \brief initialize devices for specific context
     *
     * Called once per context, after context object is initialized.
     * Devices must handle having init() called multiple times, using
     * clientid and contextid to ensure initialization happens to the correct
     * instance and minimizing redundant initialization. When each is called,
     * the 'this' pointer actually points to the array - each device knows whether
     * that is truly an array and how many elements it contains.
     *
     * \param[in] clientid     Client ID (index)
     * \param[in] num_ctx      Number of contexts in this client
     * \param[in] ctx          Context opaque entity
     * \param[in] contextid    Context ID (index)
     */
    inline pami_result_t init(size_t clientid, size_t contextid, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm)
    {
      PAMI::Device::Generic::Device::Factory::init(_generics, clientid, contextid, clt, ctx, mm, _generics);

      if(__global.useshmem())
      {
        ShmemDevice::Factory::init(_shmem, clientid, contextid, clt, ctx, mm, _generics);
#if 0
        PAMI::Device::LocalAllreduceWQDevice::Factory::init(_localallreduce, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalBcastWQDevice::Factory::init(_localbcast, clientid, contextid, clt, ctx, mm, _generics);
        PAMI::Device::LocalReduceWQDevice::Factory::init(_localreduce, clientid, contextid, clt, ctx, mm, _generics);
#endif
      }

      PAMI::Device::ProgressFunctionDev::Factory::init(_progfunc, clientid, contextid, clt, ctx, mm , _generics);
      PAMI::Device::AtomicBarrierDev::Factory::init(_atombarr, clientid, contextid, clt, ctx, mm, _generics);

      if(__global.useMU())
      {
        Device::MU::Factory::init(_mu, clientid, contextid, clt, ctx, mm, _generics);
      }

      PAMI::Device::AtomicMutexDev::Factory::init(_atmmtx, clientid, contextid, clt, ctx, mm, _generics);
      return PAMI_SUCCESS;
    }

    /**
     * \brief advance all devices
     *
     * since device arrays are semi-opaque (we don't know how many
     * elements each has) we call a more-general interface here.
     *
     * \param[in] clientid     Client ID (index)
     * \param[in] contextid    Context ID (index)
     */
    inline size_t advance(size_t clientid, size_t contextid)
    {
      size_t events = 0;
      //unsigned long long t1 = PAMI_Wtimebase();
      events += PAMI::Device::Generic::Device::Factory::advance(_generics, clientid, contextid);

      if(__global.useshmem())
      {
        events += ShmemDevice::Factory::advance(_shmem, clientid, contextid);
#if 0
        events += PAMI::Device::LocalAllreduceWQDevice::Factory::advance(_localallreduce, clientid, contextid);
        events += PAMI::Device::LocalBcastWQDevice::Factory::advance(_localbcast, clientid, contextid);
        events += PAMI::Device::LocalReduceWQDevice::Factory::advance(_localreduce, clientid, contextid);
#endif
      }

      events += PAMI::Device::ProgressFunctionDev::Factory::advance(_progfunc, clientid, contextid);
      events += PAMI::Device::AtomicBarrierDev::Factory::advance(_atombarr, clientid, contextid);

      if(__global.useMU())
        events += Device::MU::Factory::advance(_mu, clientid, contextid);

      //unsigned long long t2 = PAMI_Wtimebase() -t1;
      //printf("overhead:%lld\n", t2);


      events += Device::AtomicMutexDev::Factory::advance(_atmmtx, clientid, contextid);
      return events;
    }

    PAMI::Device::Generic::Device *_generics; // need better name...
    ShmemDevice *_shmem; //compile-time always needs the devices since runtime is where the check is made to use them

    PAMI::Device::ProgressFunctionDev *_progfunc;
    PAMI::Device::AtomicBarrierDev *_atombarr;
#if 0
    PAMI::Device::LocalAllreduceWQDevice *_localallreduce;
    PAMI::Device::LocalBcastWQDevice *_localbcast;
    PAMI::Device::LocalReduceWQDevice *_localreduce;
#endif
    MUDevice *_mu;
    PAMI::Device::AtomicMutexDev *_atmmtx;
  }; // class PlatformDeviceList

  class Context : public Interface::Context<PAMI::Context>
  {
  public:
    typedef struct 
    { 
      int  skip_to_id;
    } shared_data_t;
    inline Context (pami_client_t client, size_t clientid, size_t id, size_t num,
                    PlatformDeviceList *devices,
                    PAMI::Memory::MemoryManager * pmm, size_t bytes,
                    BGQGeometry *world_geometry,
                    std::map<unsigned, pami_geometry_t> *geometry_map,
                    shared_data_t *shared_data) :
    Interface::Context<PAMI::Context> (client, id),
    _client (client),
    _context ((pami_context_t)this),
    _clientid (clientid),
    _contextid (id),
    _geometry_map(geometry_map),
    _rmw (&_no_rmw),
    _lock(),
    _multi_registration(NULL),
    _ccmi_registration(NULL),
    _ccmi_registration_shmem(NULL),
    _ccmi_registration_mu(NULL),
    _ccmi_registration_muam(NULL),
    _ccmi_registration_mudput(NULL),
    _status(PAMI_SUCCESS),
    _shmemMcastModel(NULL),
    _shmemMsyncModel(NULL),
    _shmemMcombModel(NULL),
    _shmem_native_interface(NULL),
    _devices(devices),
    _pgas_mu_registration(NULL),
    _pgas_lite_mu_registration(NULL),
    _pgas_shmem_registration(NULL),
    _pgas_lite_shmem_registration(NULL),
    _pgas_composite_registration(NULL),
    _pgas_lite_composite_registration(NULL),
    _dummy_disable(true),
    _dummy_disabled(true),
    _no_rmw (),
    _dispatch (_context)
    {
      TRACE_FN_ENTER();
      /* The world geometry is passed in for our use, but it's only 'sliced' for use on context 0 so don't
         actually store it on other contexts */
      if(id == 0)
        _world_geometry = world_geometry;
      else
        _world_geometry = NULL;
      _async_suspend = NULL;
      _async_resume = NULL;
      _async_cookie = NULL;
      char mmkey[PAMI::Memory::MMKEYSIZE];
      char *mms;
      mms = mmkey + sprintf(mmkey, "/pami-clt%zd-ctx%zd", clientid, id);\
      // World geometry is context 0 only, so getkey's must be on context 0.
      void *val = world_geometry->getFirstKey(PAMI::Geometry::CKEY_NONCONTIG_DISABLE);
      TRACE_FORMAT(" CKEY_NONCONTIG_DISABLE %p",val);
      if(!val)      // If noncontig is not disabled
        _world_noncontig_support = true;
      else
        _world_noncontig_support = false;
      val = world_geometry->getFirstKey(PAMI::Geometry::CKEY_MEMORY_OPTIMIZE);
      TRACE_FORMAT(" CKEY_MEMORY_OPTIMIZE %p",val);
      _world_memory_optimized = (size_t)val;
      TRACE_FORMAT("<%p>, clientid %zu, contextid %zu, dispatch.id %u, noncontig %u, memory optimize %zu",this, _clientid, _contextid, _dispatch.id, _world_noncontig_support, _world_memory_optimized);
      // ----------------------------------------------------------------
      // Compile-time assertions
      // ----------------------------------------------------------------

      // Make sure the memory allocator is large enough for all
      // protocol classes.
//        COMPILE_TIME_ASSERT(sizeof(MUGlobalNI) <= ProtocolAllocator::objsize);
      // ----------------------------------------------------------------
      // Compile-time assertions
      // ----------------------------------------------------------------

      strcpy(mms, "-mm");
      _mm.init(pmm, bytes, 16, 16, 0, mmkey);
      _self = PAMI_ENDPOINT_INIT(_clientid, __global.mapping.task(), _contextid);

      //strcpy(mms, "-lock");
      _lock.init(__global._wuRegion_mm, NULL);

      _devices->init(_clientid, _contextid, _client, _context, &_mm);

      pami_endpoint_t self = PAMI_ENDPOINT_INIT(_clientid, __global.mapping.task(), _contextid);

      Protocol::Get::GetRdma <Device::MU::DmaModelMemoryFifoCompletion, MUDevice> * rget_mu = NULL;
      Protocol::Put::PutRdma <Device::MU::DmaModelMemoryFifoCompletion, MUDevice> * rput_mu = NULL;

      Protocol::Get::GetOverSend<Device::MU::PacketModel> * get_mu = NULL;
      Protocol::Put::PutOverSend<Device::MU::PacketModel> * put_mu = NULL;

      Protocol::Rmw::RmwOverSend<Device::MU::PacketModel> * rmw_mu = NULL;

      Protocol::Get::GetRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice> * rget_shmem = NULL;
      Protocol::Put::PutRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice> * rput_shmem = NULL;

      Protocol::Get::GetOverSend<ShmemPacketModel> * get_shmem = NULL;
      Protocol::Put::PutOverSend<ShmemPacketModel> * put_shmem = NULL;

      Protocol::Rmw::RmwOverSend<ShmemPacketModel> * rmw_shmem = NULL;

      ///////////////////////////////////////////////////////////////
      // Setup rget/rput protocols
      ///////////////////////////////////////////////////////////////

      typedef Protocol::Fence::DeterministicFence <
      Device::MU::PacketModel,
      Device::MU::DmaModelMemoryFifoCompletion >
      MuFence;
      MuFence * mu_fence = NULL;

      if(__global.useMU())
      {
        mu_fence = MuFence::generate(devices->_mu[_contextid], devices->_mu[_contextid], _request);
        mu_fence->initialize (_dispatch.id--, self, _context);

        // Initialize mu rget and mu rput protocols.
        pami_result_t result = PAMI_ERROR;

        rget_mu = Protocol::Get::GetRdma <Device::MU::DmaModelMemoryFifoCompletion, MUDevice>::
        generate (_devices->_mu[_contextid], _context, _request, result);

        if(result != PAMI_SUCCESS) rget_mu = NULL;

        rput_mu = Protocol::Put::PutRdma <Device::MU::DmaModelMemoryFifoCompletion, MUDevice>::
        generate (_devices->_mu[_contextid], _context, __global.heap_mm, result);

        if(result != PAMI_SUCCESS) rput_mu = NULL;

        get_mu = Protocol::Get::GetOverSend<Device::MU::PacketModel>::generate(_devices->_mu[_contextid], __global.heap_mm);
        put_mu = Protocol::Put::PutOverSend<Device::MU::PacketModel>::generate(_devices->_mu[_contextid], __global.heap_mm);
        rmw_mu = Protocol::Rmw::RmwOverSend<Device::MU::PacketModel>::generate(_devices->_mu[_contextid], __global.heap_mm);
      }

      TRACE_FORMAT("<%p>, dispatch.id %u",this,  _dispatch.id);
#if 0
      // ----------------------------------------------------------------
      // Initialize the memory region get protocol(s)
      // ----------------------------------------------------------------
      {
        pami_result_t result = PAMI_ERROR;

        if(__global.useMU() && __global.useshmem())
        {
          Protocol::Get::GetRdma <Device::MU::MUDmaModel<false>, MUDevice> * getmu =
          Protocol::Get::GetRdma <Device::MU::MUDmaModel<false>, MUDevice>::
          generate (_devices->_mu[_contextid], _request, result);
//        if (result != PAMI_SUCCESS)
//          getmu = Protocol::Get::NoRGet::generate (_context, _request);

          Protocol::Get::GetRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice> * getshmem =
          Protocol::Get::GetRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _request, result);
//        if (result != PAMI_SUCCESS)
//          getshmem = Protocol::Get::NoRGet::generate (_context, _request);

          //_rget = (Protocol::Get::RGet *) Protocol::Get::CompositeRGet::
          //_rget = Protocol::Get::CompositeRGet::
          //generate (_request, getshmem, getmu, result);
          _rget = (Protocol::Get::RGet *) Protocol::Get::Factory::
                  generate (getshmem, getmu, _request, result);

          Protocol::Put::PutRdma <Device::MU::MUDmaModel<false>, MUDevice> * putmu =
          Protocol::Put::PutRdma <Device::MU::MUDmaModel<false>, MUDevice>::
          generate (_devices->_mu[_contextid], _request, result);

          Protocol::Put::PutRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice> * putshmem =
          Protocol::Put::PutRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _request, result);

          _rput = (Protocol::Put::RPut *) Protocol::Put::Factory::
                  generate (putshmem, putmu, _request, result);

        }
        else if(__global.useshmem())
        {
          _rget = Protocol::Get::GetRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _request, result);
//        if (result != PAMI_SUCCESS)
//          _rget = Protocol::Get::NoGet::generate (_context, _request);

          _rput = Protocol::Put::PutRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _request, result);
//        if (result != PAMI_SUCCESS)
//          _rget = Protocol::Get::NoGet::generate (_context, _request);
        }
        else
        {
          _rget = Protocol::Get::GetRdma <Device::MU::MUDmaModel<false>, MUDevice>::
          generate (_devices->_mu[_contextid], _request, result);

          _rput = Protocol::Put::PutRdma <Device::MU::MUDmaModel<false>, MUDevice>::
          generate (_devices->_mu[_contextid], _request, result);
        }
      }
#endif


      typedef Protocol::Fence::DeterministicFence <
      ShmemPacketModel,
      Device::Shmem::DmaModel<ShmemDevice, true> >
      ShmemFence;
      ShmemFence * shmem_fence = NULL;

      if(__global.useshmem())
      {
        // Can't construct these models on single process nodes (no shmem)
        if(__global.topology_local.size() > 1)
        {
          shmem_fence = ShmemFence::generate(devices->_shmem[_contextid], devices->_shmem[_contextid], _request);
          shmem_fence->initialize (_dispatch.id--, self, _context);

          // Initialize shmem rget and shmem rput protocols.
          pami_result_t result = PAMI_ERROR;

          rget_shmem = Protocol::Get::GetRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _context, _request, result);

          if(result != PAMI_SUCCESS) rget_shmem = NULL;

          rput_shmem = Protocol::Put::PutRdma <Device::Shmem::DmaModel<ShmemDevice, false>, ShmemDevice>::
          generate (_devices->_shmem[_contextid], _context, __global.heap_mm, result);

          if(result != PAMI_SUCCESS) rput_shmem = NULL;

          get_shmem = Protocol::Get::GetOverSend<ShmemPacketModel>::generate(_devices->_shmem[_contextid], __global.heap_mm);
          put_shmem = Protocol::Put::PutOverSend<ShmemPacketModel>::generate(_devices->_shmem[_contextid], __global.heap_mm);
          rmw_shmem = Protocol::Rmw::RmwOverSend<ShmemPacketModel>::generate(_devices->_shmem[_contextid], __global.heap_mm);
        }
        else
        {
          // We need to decrement id if some nodes > 1 PPN and this
          // node == 1 PPN or else the dispatch id gets out of sync.
          if(__global.mapping.tSize() > 1)
            _dispatch.id--;

          TRACE_STRING("topology does not support shmem");
        }
      }

      TRACE_FORMAT("<%p>, dispatch.id %u",this,  _dispatch.id);

      // Complete rget and rput protocol initialization
      if(((rget_mu != NULL) && (rget_shmem != NULL)) &&
         ((rput_mu != NULL) && (rput_shmem != NULL)))
      {
        // Create "composite" protocols
        pami_result_t result;

        result = PAMI_ERROR;
        Protocol::Get::RGet * rget = NULL;
        rget = Protocol::Get::Factory::generate (rget_shmem, rget_mu, _request, result);

        if(result == PAMI_SUCCESS)
          _dispatch.init (rget);

        result = PAMI_ERROR;
        Protocol::Put::RPut * rput = NULL;
        rput = Protocol::Put::Factory::generate (rput_shmem, rput_mu, _request, result);

        if(result == PAMI_SUCCESS)
          _dispatch.init (rput);

        result = PAMI_ERROR;
        Protocol::Fence::Fence * fence = NULL;
        fence = Protocol::Fence::generate (shmem_fence, mu_fence, _request, result);

        if(result == PAMI_SUCCESS)
          _dispatch.init (fence);

        // For now, just use mu-based get and put protocols.
        get_mu->initialize (_dispatch.id--, self, _context);
        _dispatch.init (get_mu);

        put_mu->initialize (_dispatch.id--, self, _context);
        _dispatch.init (put_mu);

        rmw_mu->initialize (_dispatch.id--, self, _context);
        _rmw = rmw_mu;
      }
      else if(((rget_mu != NULL) && (rget_shmem == NULL)) &&
              ((rput_mu != NULL) && (rput_shmem == NULL)))
      {
        _dispatch.init (mu_fence);
        _dispatch.init (rget_mu);
        _dispatch.init (rput_mu);

        get_mu->initialize (_dispatch.id--, self, _context);
        _dispatch.init (get_mu);

        put_mu->initialize (_dispatch.id--, self, _context);
        _dispatch.init (put_mu);

        rmw_mu->initialize (_dispatch.id--, self, _context);
        _rmw = rmw_mu;
      }
      else if(((rget_mu == NULL) && (rget_shmem != NULL)) &&
              ((rput_mu == NULL) && (rput_shmem != NULL)))
      {
        _dispatch.init (shmem_fence);
        _dispatch.init (rget_shmem);
        _dispatch.init (rput_shmem);

        get_shmem->initialize (_dispatch.id--, self, _context);
        _dispatch.init (get_shmem);

        put_shmem->initialize (_dispatch.id--, self, _context);
        _dispatch.init (put_shmem);

        rmw_shmem->initialize (_dispatch.id--, self, _context);
        _rmw = rmw_shmem;
      }

      TRACE_FORMAT("<%p>, dispatch.id %u",this,  _dispatch.id);
      ///////////////////////////////////////////////////////////////
      // Construct shmem native interface
      ///////////////////////////////////////////////////////////////

      if ((_clientid == 0)
          && (_contextid == 0)
         )
      {
        // Can't construct these models on single process nodes (no shmem)
        if((__global.useshmem()) && (__global.topology_local.size() > 1))
        {
          TRACE_FORMAT("<%p> construct shmem native interface and models",this);
          _shmemMcastModel         = (ShmemMcstModel*)_shmemMcastModel_storage;
          _shmemMcombModel         = (ShmemMcombModel*)_shmemMcombModel_storage;
          _shmemMsyncModel         = (ShmemMsyncModel*)_shmemMsyncModel_storage;

          // if _shmemMsyncModel_storage is in heap...
          // PAMI_assert(Barrier_Model::checkCtorMm(__global.heap_mm);
          new (_shmemMsyncModel_storage)        ShmemMsyncModel(PAMI::Device::AtomicBarrierDev::Factory::getDevice(_devices->_atombarr, _clientid, _contextid), _status);

          //new (_shmemMcastModel_storage)        ShmemMcstModel(PAMI::Device::Generic::Device::Factory::getDevice(_devices->_generics,  _clientid, _contextid),_status);
          new (_shmemMcastModel_storage)        ShmemMcstModel(ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), _status);
          new (_shmemMcombModel_storage)        ShmemMcombModel(ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), _status);
#if 0
          new (_shmemMcastModel_storage)        ShmemMcstModel(PAMI::Device::LocalBcastWQDevice::Factory::getDevice(_devices->_localbcast, _clientid, _contextid), _status);
          new (_shmemMcombModel_storage)        ShmemMcombModel(PAMI::Device::LocalAllreduceWQDevice::Factory::getDevice(_devices->_localreduce, _clientid, _contextid), _status);
#endif

          _shmem_native_interface  = (AllSidedShmemNI*)_shmem_native_interface_storage;
          new (_shmem_native_interface_storage) AllSidedShmemNI(_shmemMcastModel, _shmemMsyncModel, _shmemMcombModel, _big_protocol, client, (pami_context_t)this, id, clientid, &_dispatch.id);
        }
      }

      TRACE_FORMAT("<%p>, dispatch.id %u",this,  _dispatch.id);

      ///////////////////////////////////////////////////////////////
      // Register collectives
      ///////////////////////////////////////////////////////////////
      TRACE_FORMAT("<%p> Register collectives(%p,%p,%zu,%zu",this, client, this, id, clientid);

#ifndef PAMI_ENABLE_COLLECTIVE_MULTICONTEXT
      if(_contextid == 0)
#endif
      {
        if(_world_noncontig_support)
        {
          // Register one PGAS (mu+shmem, mu, or shmem)
          if((__global.useMU()) && (__global.useshmem()))
          {
            _pgas_composite_registration = new(_pgas_composite_registration_storage) Composite_PGASCollreg(_client,
                                                                                                           (pami_context_t)this,
                                                                                                           _clientid,
                                                                                                           _contextid,
                                                                                                           _mid_protocol,
                                                                                                           Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid),
                                                                                                           _devices->_shmem[_contextid],
                                                                                                           NULL,
                                                                                                           &_dispatch.id,
                                                                                                           _geometry_map,
                                                                                                           true);
            _pgas_composite_registration->setGenericDevice(&_devices->_generics[_contextid]);
          }
          else if(__global.useMU())
          {
            _pgas_mu_registration = new(_pgas_mu_registration_storage) MU_PGASCollreg(_client, (pami_context_t)this, _clientid, _contextid, _mid_protocol, Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid), ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), NULL, &_dispatch.id, _geometry_map);
            _pgas_mu_registration->setGenericDevice(&_devices->_generics[_contextid]);
          }
          else if((__global.useshmem()) && (__global.topology_local.size() > 1))
          {
            _pgas_shmem_registration = new(_pgas_shmem_registration_storage) Shmem_PGASCollreg(_client, (pami_context_t)this, _clientid, _contextid, _mid_protocol, ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), NULL, & _dispatch.id, _geometry_map);
            _pgas_shmem_registration->setGenericDevice(&_devices->_generics[_contextid]);
          }
        }
        else
        {
          // Register one PGAS (mu+shmem, mu, or shmem)
          if((__global.useMU()) && (__global.useshmem()))
          {
            _pgas_lite_composite_registration = new(_pgas_composite_registration_storage) Composite_PGASLiteCollreg(_client,
                                                                                                           (pami_context_t)this,
                                                                                                           _clientid,
                                                                                                           _contextid,
                                                                                                           _mid_protocol,
                                                                                                           Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid),
                                                                                                           _devices->_shmem[_contextid],
                                                                                                           NULL,
                                                                                                           &_dispatch.id,
                                                                                                           _geometry_map,
                                                                                                           true);
            _pgas_lite_composite_registration->setGenericDevice(&_devices->_generics[_contextid]);
          }
          else if(__global.useMU())
          {
            _pgas_lite_mu_registration = new(_pgas_mu_registration_storage) MU_PGASLiteCollreg(_client, (pami_context_t)this, _clientid, _contextid, _mid_protocol, Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid), ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), NULL, &_dispatch.id, _geometry_map);
            _pgas_lite_mu_registration->setGenericDevice(&_devices->_generics[_contextid]);
          }
          else if((__global.useshmem()) && (__global.topology_local.size() > 1))
          {
            _pgas_lite_shmem_registration = new(_pgas_shmem_registration_storage) Shmem_PGASLiteCollreg(_client, (pami_context_t)this, _clientid, _contextid, _mid_protocol, ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid), NULL, & _dispatch.id, _geometry_map);
            _pgas_lite_shmem_registration->setGenericDevice(&_devices->_generics[_contextid]);
          }
        }
      }

      TRACE_FORMAT("<%p>, dispatch.id %u",this,  _dispatch.id);

      // The multi registration will use shmem/mu if they are ctor'd above.
      // Even if we use PAMI_ENABLE_COLLECTIVE_MULTICONTEXT, these are context 0 only so no ifndef here.
      if ((_clientid == 0)
          && (_contextid == 0)
          )
      {
        _multi_registration       =  new ((BGQRegistration*) _multi_registration_storage)
                                     BGQRegistration(_shmem_native_interface,
                                                     ShmemDevice::Factory::getDevice(_devices->_shmem, _clientid, _contextid),
                                                     PAMI::Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid),
                                                     client,
                                                     (pami_context_t)this,
                                                     id,
                                                     clientid,
                                                     &_dispatch.id,
                                                     _geometry_map,
                                                     _mid_protocol,
                                                     _big_protocol);
        uint64_t inval = (uint64_t) - 1;
        _multi_registration->receive_global (_contextid, _world_geometry, &inval, 1);
        // Because we only register these on context 0, dispatch id's can get out of sync across contexts.
        // So record our dispatch id after registering MU optimized collectives so that other contexts
        //  can skip to that id without doing the registration.
        shared_data->skip_to_id = _dispatch.id; 
      }
      else // skip to the same dispatch id as context 0 used after registering MU optimized collectives
      {  
        if(_contextid == 0) shared_data->skip_to_id = _dispatch.id; // !clientid == 0 must still set the skip id.
        else _dispatch.id = shared_data->skip_to_id;
      }

      TRACE_FORMAT("<%p>, dispatch.id %u",this,  _dispatch.id);

#ifndef PAMI_ENABLE_COLLECTIVE_MULTICONTEXT
      if(_contextid == 0)
#endif
      {
        if((__global.useMU()) && (__global.useshmem()))
        {
          COMPILE_TIME_ASSERT(sizeof(CompositeNIFactory) <= ProtocolAllocator::objsize);
          COMPILE_TIME_ASSERT(sizeof(CompositeNIFactory_AMC) <= ProtocolAllocator::objsize);
          TRACE_FORMAT("<%p>Allocator:  sizeof(CompositeNIFactory) %zu, ProtocolAllocator::objsize %zu",this, sizeof(CompositeNIFactory), ProtocolAllocator::objsize);
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory_amc = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          TRACE_FORMAT("<%p>Composite CCMI NI %p, %p, registration %p",this, ni_factory, ni_factory_amc, _ccmi_registration_storage);
          new (ni_factory) CompositeNIFactory (_client, _context, _clientid, _contextid, _devices->_shmem[_contextid], _devices->_mu[_contextid], _mid_protocol);
          new (ni_factory_amc) CompositeNIFactory_AMC (_client, _context, _clientid, _contextid, _devices->_shmem[_contextid], _devices->_mu[_contextid], _mid_protocol);
          _ccmi_registration =  new((CCMIRegistration*)_ccmi_registration_storage)
                                CCMIRegistration(_client, _context,  _clientid, _contextid,
                                                 _protocol,
                                                 __global.topology_global.size(),
                                                 __global.topology_local.size(),
                                                 &_dispatch.id,
                                                 _geometry_map,
                                                 ni_factory,
                                                 ni_factory_amc);
        }
        else if(__global.useMU())
        {
          COMPILE_TIME_ASSERT(sizeof(MUNIFactory) <= ProtocolAllocator::objsize);
          COMPILE_TIME_ASSERT(sizeof(MUNIFactory_AMC) <= ProtocolAllocator::objsize);
          TRACE_FORMAT("<%p>Allocator:  sizeof(MUNIFactory) %zu, ProtocolAllocator::objsize %zu",this, sizeof(MUNIFactory), ProtocolAllocator::objsize);
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory_mu = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory_mu_amc = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          TRACE_FORMAT("<%p>MU CCMI NI %p, %p, registration %p, _clientid %zu, _contextid %zu",this, ni_factory_mu, ni_factory_mu_amc, _ccmi_registration_mu_storage, _clientid, _contextid);
          new (ni_factory_mu) MUNIFactory (_client, _context, _clientid, _contextid, _devices->_mu[_contextid], _big_protocol);
          new (ni_factory_mu_amc) MUNIFactory_AMC (_client, _context, _clientid, _contextid, _devices->_mu[_contextid], _big_protocol);
          _ccmi_registration_mu =  new((CCMIRegistration*)_ccmi_registration_mu_storage)
                                   CCMIRegistration(_client, _context, _clientid, _contextid,
                                                    _protocol,
                                                    __global.topology_global.size(),
                                                    __global.topology_local.size(),
                                                    &_dispatch.id,
                                                    _geometry_map,
                                                    ni_factory_mu,
                                                    ni_factory_mu_amc);
        }
        else if(__global.useshmem())
        {
          //use shmem
          COMPILE_TIME_ASSERT(sizeof(ShmemNIFactory) <= ProtocolAllocator::objsize);
          COMPILE_TIME_ASSERT(sizeof(ShmemNIFactory_AMC) <= ProtocolAllocator::objsize);
          TRACE_FORMAT("<%p>Allocator:  sizeof(ShmemNIFactory) %zu, ProtocolAllocator::objsize %zu",this, sizeof(ShmemNIFactory), ProtocolAllocator::objsize);
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory_shmem = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory_shmem_amc = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          TRACE_FORMAT("<%p>Shmem CCMI NI %p, %p, registration %p",this, ni_factory_shmem, ni_factory_shmem_amc, _ccmi_registration_shmem_storage);
          new (ni_factory_shmem) ShmemNIFactory (_client, _context, _clientid, _contextid, _devices->_shmem[_contextid], _mid_protocol);
          new (ni_factory_shmem_amc) ShmemNIFactory_AMC (_client, _context, _clientid, _contextid, _devices->_shmem[_contextid], _mid_protocol);
          _ccmi_registration_shmem =  new((CCMIRegistration*)_ccmi_registration_shmem_storage)
                                      CCMIRegistration(_client, _context, _clientid, _contextid,
                                                       _protocol,
                                                       __global.topology_global.size(),
                                                       __global.topology_local.size(),
                                                       &_dispatch.id,
                                                       _geometry_map,
                                                       ni_factory_shmem,
                                                       ni_factory_shmem_amc);
        }
      }
      if ((_clientid == 0)
          && (_contextid == 0)
         )
      {
        if(__global.useMU())
        {
          COMPILE_TIME_ASSERT(sizeof(MUAMMulticastFactory) <= ProtocolAllocator::objsize);
          TRACE_FORMAT("<%p>Allocator:  sizeof(MUAMMulticastFactory) %zu, ProtocolAllocator::objsize %zu",this, sizeof(MUAMMulticastFactory), ProtocolAllocator::objsize);
          CCMI::Interfaces::NativeInterfaceFactory *ni_factory_muam = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
          TRACE_FORMAT("<%p>MU AM CCMI NI %p, registration %p",this, ni_factory_muam,  _ccmi_registration_muam_storage);
          new (ni_factory_muam) MUAMMulticastFactory (_client, _context, _clientid, _contextid, _devices->_mu[_contextid], _big_protocol);

          _ccmi_registration_muam =  new((CCMIRegistrationKey2*)_ccmi_registration_muam_storage)
                                     CCMIRegistrationKey2(_client, _context, _clientid, _contextid,
                                                          _protocol,
                                                          __global.topology_global.size(),
                                                          __global.topology_local.size(),
                                                          &_dispatch.id,
                                                          _geometry_map,
                                                          ni_factory_muam,
                                                          NULL);

          PAMI_assert(__global.topology_local.size() == __global.local_size());

          if(__global.mapping.tSize() < 64) /// \todo Not enough resources for MU Dput at 64 PPN, maybe fix this.
          {
            if(__global.mapping.tSize() == 1 || !__global.useshmem())
            {
              COMPILE_TIME_ASSERT(sizeof(MUDputNIFactory) <= ProtocolAllocator::objsize);
              CCMI::Interfaces::NativeInterfaceFactory *ni_factory_mudp = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
              new (ni_factory_mudp) MUDputNIFactory (_client, _context, _clientid, _contextid, _devices->_mu[_contextid], _big_protocol);

              _ccmi_registration_mudput =  new((CCMIRegistrationKey2*)_ccmi_registration_mudput_storage)
                                           CCMIRegistrationKey2(_client, _context,  _clientid, _contextid,
                                                                _protocol,
                                                                __global.topology_global.size(),
                                                                __global.topology_local.size(),
                                                                &_dispatch.id,
                                                                _geometry_map,
                                                                ni_factory_mudp,
                                                                NULL);
            }
            else
            {
              COMPILE_TIME_ASSERT(sizeof(MUShmemDputNIFactory) <= ProtocolAllocator::objsize);
              CCMI::Interfaces::NativeInterfaceFactory *ni_factory_mudp = (CCMI::Interfaces::NativeInterfaceFactory *) _protocol.allocateObject();
              new (ni_factory_mudp) MUShmemDputNIFactory (_client, _context, _clientid, _contextid, _devices->_mu[_contextid], _devices->_shmem[_contextid], _big_protocol);

              _ccmi_registration_mudput =  new((CCMIRegistrationKey2*)_ccmi_registration_mudput_storage)
                                           CCMIRegistrationKey2(_client, _context,  _clientid, _contextid,
                                                                _protocol,
                                                                __global.topology_global.size(),
                                                                __global.topology_local.size(),
                                                                &_dispatch.id,
                                                                _geometry_map,
                                                                ni_factory_mudp,
                                                                NULL);
            }
          }
        }
      }

      if (_contextid == 0)
      {
        if(_world_noncontig_support)
        {  
          // Can only use shmem pgas if the geometry is all local tasks, so check the topology
          if(_pgas_shmem_registration && ((PAMI::Topology*)_world_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->isLocal()) _pgas_shmem_registration->analyze(_contextid, _world_geometry, 0);
  
          // Can always use MU if it's available
          if(_pgas_mu_registration) _pgas_mu_registration->analyze(_contextid, _world_geometry, 0);
  
          // Can always use composite if it's available
          if(_pgas_composite_registration) _pgas_composite_registration->analyze(_contextid, _world_geometry, 0);
        }
        else
        {  
          // Can only use shmem pgas if the geometry is all local tasks, so check the topology
          if(_pgas_lite_shmem_registration && ((PAMI::Topology*)_world_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->isLocal()) _pgas_lite_shmem_registration->analyze(_contextid, _world_geometry, 0);
  
          // Can always use MU if it's available
          if(_pgas_lite_mu_registration) _pgas_lite_mu_registration->analyze(_contextid, _world_geometry, 0);
  
          // Can always use composite if it's available
          if(_pgas_lite_composite_registration) _pgas_lite_composite_registration->analyze(_contextid, _world_geometry, 0);
        }
        /* Order matters a little here.  The last one to set the UE barrier wins.   */

        if(_ccmi_registration_mudput)
        {
          _ccmi_registration_mudput->analyze(_contextid, _world_geometry, 0);
        }

        if(_ccmi_registration_shmem)
        {
          _ccmi_registration_shmem->analyze(_contextid, _world_geometry, 0);
        }
  
        if(_ccmi_registration_mu)
        {
          _ccmi_registration_mu->analyze(_contextid, _world_geometry, 0);
        }
  
        if(_ccmi_registration)
        {
          _ccmi_registration->analyze(_contextid, _world_geometry, 0);
        }
  
        if(_ccmi_registration_muam)
        {
          _ccmi_registration_muam->analyze(_contextid, _world_geometry, 0);
        }
  
        if(_multi_registration) // && (((PAMI::Topology*)_world_geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->size() != 1))
        {
          _multi_registration->analyze(_contextid, _world_geometry, 0);
  
          // for now, this is the only registration that has a phase 1...
          // We know that _world_geometry is always "optimized" at create time.
          _multi_registration->analyze(_contextid, _world_geometry, 1);
        }
      }

      TRACE_FN_EXIT();
    }

    inline ~Context()
    {
    }

    inline pami_client_t getClient_impl ()
    {
      return _client;
    }

    inline size_t getId_impl ()
    {
      return _contextid;
    }

    inline pami_result_t destroy_impl ()
    {
        TRACE_FN_ENTER();
        pami_result_t rc;
        rc = PAMI_SUCCESS;

        // Invoke the MU device destroy()
        if ( __global.useMU() )
        {
          rc = Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid).destroy();
        }

        TRACE_FORMAT("<%p>rc = %d",this, rc);
        TRACE_FN_EXIT();
        return rc;
    }

    inline pami_result_t post_impl (pami_work_t *state, pami_work_function work_fn, void * cookie)
    {
      PAMI::Device::Generic::GenericThread *work;
      COMPILE_TIME_ASSERT(sizeof(*state) >= sizeof(*work));
      work = new (state) PAMI::Device::Generic::GenericThread(work_fn, cookie);
      _devices->_generics[_contextid].postThread(work);
      return PAMI_SUCCESS;
    }

    inline size_t advance_impl (size_t maximum, pami_result_t & result)
    {
//          result = PAMI_EAGAIN;
      result = PAMI_SUCCESS;
      size_t events = 0;
      unsigned i;

      for(i = 0; i < maximum && events == 0; i++)
      {
        events += _devices->advance(_clientid, _contextid);
      }

      //if (events > 0) result = PAMI_SUCCESS;

      return events;
    }

    inline pami_result_t lock_impl ()
    {
      _lock.acquire ();
      return PAMI_SUCCESS;
    }

    inline pami_result_t trylock_impl ()
    {
      if(_lock.tryAcquire ())
      {
        return PAMI_SUCCESS;
      }

      return PAMI_EAGAIN;
    }

    inline pami_result_t unlock_impl ()
    {
      _lock.release ();
      return PAMI_SUCCESS;
    }

    inline pami_result_t send_impl (pami_send_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.send (parameters);

      TRACE_FORMAT("<%p>rc = %d",this,rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t send_impl (pami_send_immediate_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.send (parameters);

      TRACE_FORMAT("<%p>rc = %d",this,rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t send_impl (pami_send_typed_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.send (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t put_impl (pami_put_simple_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.put (parameters);

      TRACE_FORMAT("<%p>rc = %d",this,rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t put_typed (pami_put_typed_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.put (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t get_impl (pami_get_simple_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.get (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t get_typed (pami_get_typed_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.get (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t rmw (pami_rmw_t * parameters)
    {
      return _rmw->start (parameters);
    }

    inline pami_result_t memregion_create_impl (void             * address,
                                                size_t             bytes_in,
                                                size_t           * bytes_out,
                                                pami_memregion_t * memregion)
    {
      new ((void *) memregion) Memregion ();
      Memregion * mr = (Memregion *) memregion;
      return mr->createMemregion (bytes_out, bytes_in, address, 0);
    }

    inline pami_result_t memregion_destroy_impl (pami_memregion_t * memregion)
    {
      // Memory regions do not need to be deregistered on BG/Q so this
      // interface is implemented as a noop.
      return PAMI_SUCCESS;
    }

    inline pami_result_t rput_impl (pami_rput_simple_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.rput (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t rput_typed (pami_rput_typed_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.rput (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t rget_impl (pami_rget_simple_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.rget (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t rget_typed (pami_rget_typed_t * parameters)
    {
      TRACE_FN_ENTER();

      pami_result_t rc = _dispatch.rget (parameters);

      TRACE_FORMAT("<%p>rc = %d",this, rc);
      TRACE_FN_EXIT();
      return rc;
    }

    inline pami_result_t purge_totask (pami_endpoint_t *dest, size_t count)
    {
      return PAMI_SUCCESS;
    }

    inline pami_result_t resume_totask (pami_endpoint_t *dest, size_t count)
    {
      return PAMI_SUCCESS;
    }

    inline pami_result_t fence_begin ()
    {
      _dispatch.fence (true);
      return PAMI_SUCCESS;
    }

    inline pami_result_t fence_end ()
    {
      _dispatch.fence (false);
      return PAMI_SUCCESS;
    }

    inline pami_result_t fence_all (pami_event_function   done_fn,
                                    void               * cookie)
    {
      return _dispatch.fence (done_fn, cookie);
    }

    inline pami_result_t fence_endpoint (pami_event_function   done_fn,
                                         void                * cookie,
                                         pami_endpoint_t       endpoint)
    {
      return _dispatch.fence (done_fn, cookie, endpoint);
    }


    inline pami_result_t collective_impl (pami_xfer_t * parameters)
    {
      TRACE_FN_ENTER();
      std::map<size_t, Geometry::Algorithm<BGQGeometry> > *algo =
      (std::map<size_t, Geometry::Algorithm<BGQGeometry> > *)parameters->algorithm;
      TRACE_FORMAT("<%p>algorithm %p, context %p/%zu",this, algo, this, _contextid);
      TRACE_FN_EXIT();
      return(*algo)[_contextid].generate(parameters);
    }

    inline pami_result_t amcollective_dispatch_impl (pami_algorithm_t            algorithm,
                                                     size_t                     dispatch,
                                                     pami_dispatch_callback_function fn,
                                                     void                     * cookie,
                                                     pami_collective_hint_t      options)
    {
      TRACE_FN_ENTER();
      std::map<size_t, Geometry::Algorithm<BGQGeometry> > *algo =
      (std::map<size_t, Geometry::Algorithm<BGQGeometry> > *)algorithm;
      TRACE_FORMAT("<%p>algorithm %p, context %p/%zu, dispatch %zu, cookie %p, options %#X",this, algo, this, _contextid, dispatch, cookie, *(unsigned*)&options);
      TRACE_FN_EXIT();
      return(*algo)[0].dispatch_set(_contextid, dispatch, fn, cookie, options);
    }

    inline pami_result_t dispatch_impl (size_t                          id,
                                        pami_dispatch_callback_function fn,
                                        void                          * cookie,
                                        pami_dispatch_hint_t            options)
    {
      TRACE_FN_ENTER();
      pami_result_t result = PAMI_ERROR;
      //TRACE_FORMAT(" _dispatch[%zu] = %p, options = %#X", id, _dispatch[id], *(unsigned*)&options);

      // Return an error for invalid / unimplemented 'hard' hints.
      if(
        options.use_rdma              == PAMI_HINT_ENABLE  ||
        (options.use_shmem            == PAMI_HINT_ENABLE  && !__global.useshmem()) ||
        (options.use_shmem            == PAMI_HINT_DISABLE && !__global.useMU())    ||
        false)
      {
        TRACE_FN_EXIT();
        return PAMI_ERROR;
      }

      pami_endpoint_t self = PAMI_ENDPOINT_INIT(_clientid, __global.mapping.task(), _contextid);

      using namespace Protocol::Send;


      if((options.use_shmem == PAMI_HINT_DISABLE) || (__global.useMU() && !__global.useshmem()))
      {
        Protocol::Send::Send * send =
        Eager <Device::MU::PacketModel>::generate (id, fn.p2p, cookie,
                                                   _devices->_mu[_contextid],
                                                   self, _context, options,
                                                   __global.heap_mm, result);
        _dispatch.set (id, send);
      }
      else if((options.use_shmem == PAMI_HINT_ENABLE) || (!__global.useMU() && __global.useshmem()))
      {
        Protocol::Send::Send * send =
        Eager <ShmemPacketModel>::generate (id, fn.p2p, cookie,
                                            _devices->_shmem[_contextid],
                                            self, _context, options,
                                            __global.heap_mm, result);
        _dispatch.set (id, send);
      }
      else
      {
        Protocol::Send::Send * send =
        Eager <ShmemPacketModel, Device::MU::PacketModel>::generate (id, fn.p2p, cookie,
                                                                     _devices->_shmem[_contextid],
                                                                     _devices->_mu[_contextid],
                                                                     self, _context, options,
                                                                     __global.heap_mm, result);
        _dispatch.set (id, send);
      }

      TRACE_FORMAT("<%p>result = %d",this, result);
      TRACE_FN_EXIT();
      return result;
    }

    /// \brief Dummy work function to keep context advance returning "busy"
    ///
    /// \see pami_work_function
    ///
    static pami_result_t _dummy_work_fn(pami_context_t ctx, void *cookie)
    {
      Context *thus = (Context *)cookie;

      if(!thus->_dummy_disable)
      {
        return PAMI_EAGAIN;
      }
      else
      {
        thus->_dummy_disabled = true;
        // Note: dequeue happens sometime after return...
        // So all participants must lock the context to
        // ensure an atomic view of the above variables.
        return PAMI_SUCCESS;
      }
    }

    /// \brief BGQ-only method to return the BGQ core to which a context has best affinity
    ///
    /// \return	BGQ Core ID that is best for context
    ///
    inline uint32_t coreAffinity()
    {
      // might be other affinities to consider, in the future.
      // could be cached in context, instead of calling MU RM (etc) every time
      // When the MU is active, go with its affinity.
      // Otherwise, compute an affinity.
      if( __global.useMU() )
      {
        return Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid).affinity();
      }
      else
      {
        return(NUM_CORES - 1) - (_contextid % (Kernel_ProcessorCount() / NUM_SMT));
      }
    }

    /// \brief BGQ-only method to fix any problems with hardware affinity to core/thread
    ///
    /// This method is (initially) to address a problem with MU interrupts having
    /// hardware affinity to a core. This means that if a commthread is advancing
    /// a context from a "foriegn" core it cannot depend on the WakeUp Unit to
    /// wake up a waitimpl instruction and so we have to effectively disable the
    /// wait. This is done by posting a dummy (persistent) function on the generic
    /// device which will cause the return value of advance to always be "1", which
    /// will prevent the commthread from going into wait.
    ///
    /// Caller holds the context lock.
    ///
    /// \param[in] acquire	True if acquiring the context,
    ///			false means the context is being released.
    ///
    inline void cleanupAffinity(bool acquire)
    {
      TRACE_FN_ENTER();
#if 1
      bool affinity = (coreAffinity() == Kernel_ProcessorCoreID());
      TRACE_FORMAT("<%p>acquire=%d, affinity=%d, coreAffinity=%u, Kernel_ProcessorCoreID=%u, ContextID=%zu",this, acquire, affinity, coreAffinity(), Kernel_ProcessorCoreID(), _contextid);
#else
      bool affinity = !__global.useMU(); // if no MU, affinity anywhere
#endif
      bool enqueue = (acquire && !affinity);
      bool cancel = (enqueue && _dummy_disable && !_dummy_disabled);
      bool dequeue = (!acquire);

      // If we are using the MU, tell the MU device to set up interrupts properly
      // depending on whether we are acquiring the context or releasing it.
      if( __global.useMU() )
      {
        Device::MU::Factory::getDevice(_devices->_mu, _clientid, _contextid).setInterrupts(acquire);
      }

      if(cancel)
      {
        _dummy_disable = _dummy_disabled = false;
      }
      else if(enqueue && _dummy_disabled)
      {
        _dummy_disable = _dummy_disabled = false;
        PAMI::Device::Generic::GenericThread *work = new (&_dummy_work)
                                                     PAMI::Device::Generic::GenericThread(_dummy_work_fn, this);
        _devices->_generics[_contextid].postThread(work);
      }
      else if(dequeue)
      {
        _dummy_disable = true;
      }

      TRACE_FN_EXIT();
    }

    inline pami_result_t registerWithOptimizer (Geometry::GeometryOptimizer<BGQGeometry>  *go)
    {
      TRACE_FN_ENTER();

      PAMI_assertf(_contextid == 0, "No support for context %zu optimized collectives",_contextid);

      // Even if we use PAMI_ENABLE_COLLECTIVE_MULTICONTEXT, these are context 0 only so no ifndef here.
      if((_clientid == 0)
         && (_contextid == 0)
        )
      {
        TRACE_FORMAT("<%p>id %zu, geometry %p",this,_contextid, go->geometry());
        uint64_t  reduce_result[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        int n_multi = 0;
        TRACE_FORMAT("<%p>id %zu, geometry %p, register %p, %d",this, _contextid, go->geometry(),reduce_result,n_multi);
        _multi_registration->register_local (_contextid, go->geometry(), reduce_result, n_multi);
        TRACE_FORMAT("<%p>id %zu, geometry %p, register %p, %d",this,_contextid, go->geometry(),reduce_result,n_multi);
        go->registerWithOptimizer (_contextid, reduce_result, n_multi, receive_global, this );
      }

      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }


    static void receive_global( size_t           context_id,
                                void           * cookie,
                                uint64_t       * reduce_result,
                                size_t           n,
                                BGQGeometry    * geometry,
                                pami_result_t    result )
    {
      TRACE_FN_ENTER();
      Context *context = (Context *) cookie;
      TRACE_FORMAT("<%p>context id %zu, geometry %p",context, context_id, geometry);
      context->_multi_registration->receive_global(context_id, geometry, reduce_result, n);
      TRACE_FN_EXIT();
    }

    inline pami_result_t analyze(size_t         context_id, /* may be PAMI_ALL_CONTEXTS */
                                 BGQGeometry    *geometry,
                                 int phase = 0)
    {
      TRACE_FN_ENTER();
      TRACE_FORMAT("<%p>analyze id %zu, registration %p, phase %d, size %zu",this, _contextid, geometry, phase,
                   ((PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->size());

      // We choose not to enable pgas on 1 task geommetries
      if(((PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->size() != 1)
      {
        // If client non-contig requested (PGAS was registered), 
        if(_world_noncontig_support) 
        {
          // Can only use shmem pgas if the geometry is all local tasks, so check the topology
          if(_pgas_shmem_registration && 
             ((PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->isLocal())
            _pgas_shmem_registration->analyze(_contextid, geometry, phase);
    
          // Can always use MU if it's available
          if(phase == 0 && _pgas_mu_registration)
            _pgas_mu_registration->analyze(_contextid, geometry, phase);
    
          // Can always use composite if it's available
          if(_pgas_composite_registration)
            _pgas_composite_registration->analyze(_contextid, geometry, phase);
        }
        else /* Use PGAS Lite */
        {
    
          // Can only use shmem pgas if the geometry is all local tasks, so check the topology
          if(_pgas_lite_shmem_registration && 
             ((PAMI::Topology*)geometry->getTopology(PAMI::Geometry::DEFAULT_TOPOLOGY_INDEX))->isLocal())
            _pgas_lite_shmem_registration->analyze(_contextid, geometry, phase);
    
          // Can always use MU if it's available
          if(phase == 0 && _pgas_lite_mu_registration)
            _pgas_lite_mu_registration->analyze(_contextid, geometry, phase);
    
          // Can always use composite if it's available
          if(_pgas_lite_composite_registration)
            _pgas_lite_composite_registration->analyze(_contextid, geometry, phase);
        }
      }

      /* Order matters a little here.  The last one to set the UE barrier wins.   */

      if(_ccmi_registration_mudput && (context_id != PAMI_ALL_CONTEXTS)) 
      {
        _ccmi_registration_mudput->analyze(_contextid, geometry, phase);
      }

      if(_ccmi_registration_shmem)
      {
        _ccmi_registration_shmem->analyze(_contextid, geometry, phase);
      }

      if(_ccmi_registration_mu)
      {
        _ccmi_registration_mu->analyze(_contextid, geometry, phase);
      }

      if(_ccmi_registration)
      {
        _ccmi_registration->analyze(_contextid, geometry, phase);
      }

      if(_ccmi_registration_muam && (context_id != PAMI_ALL_CONTEXTS))
      {
        _ccmi_registration_muam->analyze(_contextid, geometry, phase);
      }

      if(_multi_registration && (context_id != PAMI_ALL_CONTEXTS))
      {  
        _multi_registration->analyze(_contextid, geometry, phase);
      }

      TRACE_FN_EXIT();
      return PAMI_SUCCESS;
    }

    inline pami_result_t dispatch_query_impl(size_t                dispatch,
                                             pami_configuration_t  configuration[],
                                             size_t                num_configs)
    {
      return _dispatch.query (dispatch, configuration, num_configs);
    }

    inline pami_result_t dispatch_update_impl(size_t                dispatch,
                                              pami_configuration_t  configuration[],
                                              size_t                num_configs)
    {
      return _dispatch.update (dispatch, configuration, num_configs);
    }

    inline pami_result_t query_impl(pami_configuration_t  configuration[],
                                    size_t                num_configs)
    {
      pami_result_t result = PAMI_SUCCESS;
      size_t i;

      for(i = 0; i < num_configs; i++)
      {
        switch(configuration[i].name)
        {
        case PAMI_CONTEXT_DISPATCH_ID_MAX:
          configuration[i].value.intval = _dispatch.id;
          break;

        default:
          result = PAMI_INVAL;
        }
      }

      return result;
    }

    inline pami_result_t update_impl(pami_configuration_t  configuration[],
                                     size_t                num_configs)
    {
      return PAMI_INVAL;
    }

    inline void registerUnexpBarrier_impl (unsigned     comm,
                                           pami_quad_t &info,
                                           unsigned     peer,
                                           unsigned     algorithm)
    {
      Geometry::UnexpBarrierQueueElement *ueb =
      (Geometry::UnexpBarrierQueueElement *) _ueb_allocator.allocateObject();
      new (ueb) Geometry::UnexpBarrierQueueElement (comm, _contextid, info, peer, algorithm);
      _ueb_queue.pushTail(ueb);
    }

    inline size_t getClientId ()
    {
      return _clientid;
    }

    inline pami_result_t registerAsync(
                                      PAMI::ProgressExtension::pamix_async_function progress,
                                      PAMI::ProgressExtension::pamix_async_function suspend,
                                      PAMI::ProgressExtension::pamix_async_function resume,
                                      void *cookie)
    {
      _async_suspend = suspend;
      _async_resume = resume;
      _async_cookie = cookie;
      return PAMI_SUCCESS;
    }

    inline pami_result_t getAsyncRegs(
                                     PAMI::ProgressExtension::pamix_async_function *progress,
                                     PAMI::ProgressExtension::pamix_async_function *suspend,
                                     PAMI::ProgressExtension::pamix_async_function *resume,
                                     void **cookie)
    {
      *progress = NULL;
      *suspend = _async_suspend;
      *resume = _async_resume;
      *cookie = _async_cookie;
      return PAMI_SUCCESS;
    }
  private:

    pami_client_t                _client;
    pami_context_t               _context;
    size_t                       _clientid;
    size_t                       _contextid;
    PAMI::ProgressExtension::pamix_async_function _async_suspend;
    PAMI::ProgressExtension::pamix_async_function _async_resume;
    void                        *_async_cookie;
    std::map<unsigned, pami_geometry_t> *_geometry_map;
    pami_endpoint_t              _self;

    PAMI::Memory::GenMemoryManager  _mm;

    Protocol::Rmw::Rmw          *_rmw;
    MemoryAllocator<1024, 64, 16> _request;
    ContextLock                  _lock;
    BGQRegistration             *_multi_registration;
    CCMIRegistration            *_ccmi_registration;
    CCMIRegistration            *_ccmi_registration_shmem;
    CCMIRegistration            *_ccmi_registration_mu;
    CCMIRegistrationKey2        *_ccmi_registration_muam;
    CCMIRegistrationKey2        *_ccmi_registration_mudput;
    BGQGeometry                 *_world_geometry; /* only valid on context 0, client 0 */
    pami_result_t                _status;
    ShmemMcstModel              *_shmemMcastModel;
    ShmemMsyncModel             *_shmemMsyncModel;
    ShmemMcombModel             *_shmemMcombModel;
    AllSidedShmemNI             *_shmem_native_interface;
    uint8_t                      _ccmi_registration_storage[sizeof(CCMIRegistration)];
    uint8_t                      _ccmi_registration_shmem_storage[sizeof(CCMIRegistration)];
    uint8_t                      _ccmi_registration_mu_storage[sizeof(CCMIRegistration)];
    uint8_t                      _ccmi_registration_muam_storage[sizeof(CCMIRegistrationKey2)];
    uint8_t                      _ccmi_registration_mudput_storage[sizeof(CCMIRegistrationKey2)];
    uint8_t                      _multi_registration_storage[sizeof(BGQRegistration)];
    uint8_t                      _shmemMcastModel_storage[sizeof(ShmemMcstModel)];
    uint8_t                      _shmemMsyncModel_storage[sizeof(ShmemMsyncModel)];
    uint8_t                      _shmemMcombModel_storage[sizeof(ShmemMcombModel)];
    uint8_t                      _shmem_native_interface_storage[sizeof(AllSidedShmemNI)];
    ProtocolAllocator            _protocol;
    BigProtocolAllocator         _big_protocol;
    MidProtocolAllocator         _mid_protocol;
    PlatformDeviceList          *_devices;
    MU_PGASCollreg              *_pgas_mu_registration;
    MU_PGASLiteCollreg          *_pgas_lite_mu_registration;
    uint8_t                      _pgas_mu_registration_storage[MAX(sizeof(MU_PGASCollreg),sizeof(MU_PGASLiteCollreg))];
    Shmem_PGASCollreg           *_pgas_shmem_registration;
    Shmem_PGASLiteCollreg       *_pgas_lite_shmem_registration;
    uint8_t                      _pgas_shmem_registration_storage[MAX(sizeof(Shmem_PGASCollreg),sizeof(Shmem_PGASLiteCollreg))];
    Composite_PGASCollreg       *_pgas_composite_registration;
    Composite_PGASLiteCollreg   *_pgas_lite_composite_registration;
    uint8_t                      _pgas_composite_registration_storage[MAX(sizeof(Composite_PGASCollreg),sizeof(Composite_PGASLiteCollreg))];

    bool                         _dummy_disable;
    bool                         _dummy_disabled;

    // PAMI_CLIENT_NONCONTIG and PAMI_CLIENT_MEMORY_OPTIMIZE values
    bool                         _world_noncontig_support;
    size_t                       _world_memory_optimized;

    PAMI::Device::Generic::GenericThread _dummy_work;

    Protocol::Rmw::Error        _no_rmw;
    Dispatch<256>               _dispatch;
    public:
    MemoryAllocator <sizeof(PAMI::Geometry::UnexpBarrierQueueElement), 16> _ueb_allocator;
    MatchQueue<>                                                           _ueb_queue;
  }; // end PAMI::Context
}; // end namespace PAMI

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_context_bgq_bgqcontext_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
