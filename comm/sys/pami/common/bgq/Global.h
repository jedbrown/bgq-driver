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
#ifndef __common_bgq_Global_h__
#define __common_bgq_Global_h__
///
/// \file common/bgq/Global.h
/// \brief Blue Gene/Q Global Object
///
/// This global object is constructed before main() and is a container class
/// for all other classes that need to be constructed and initialized before
/// the application starts.
///

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "Platform.h"
#include "util/common.h"
#include "components/memory/heap/HeapMemoryManager.h"
#include "components/memory/shmem/SharedMemoryManager.h"
#include "common/GlobalInterface.h"
#include "common/bgq/BgqPersonality.h"
#include "common/bgq/BgqMapCache.h"
#include "Mapping.h"
#include "Topology.h"
#include "common/bgq/L2AtomicFactory.h"
#include <agents/include/comm/commagent.h>

#ifndef PAMI_MAX_NUM_CLIENTS
/** \todo PAMI_MAX_NUM_CLIENTS needs to be setup by pami.h */
  #define PAMI_MAX_NUM_CLIENTS    4
#endif // !PAMI_MAX_NUM_CLIENTS

// This enables compilation of the comm-thread code, but it will
// not be used unless something calls PAMI_Client_add_commthread_context().
// See test/BG/bgq/commthreads/single-context.c for example use.
// However, if desired, compilation may be disabled by changing
// the following to "#undef".
#define USE_COMMTHREADS // define/undef

#ifdef USE_COMMTHREADS
  #include "components/devices/bgq/commthread/CommThreadFactory.h"
#endif // USE_COMMTHREADS

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

#ifndef abs_x
  #define abs_x(x) ((x^(x>>31)) - (x>>31))
#endif

///
/// \brief dump hex data to stderr
///  \param s : an informational text string to print
///  \param b : the (integer) buffer to dump
///  \param n : number of integers to dump from the buffer
#undef DUMP_HEXDATA
#define DUMP_HEXDATA(s,b,n) //globalDumpHexData(s,b,n)
void globalDumpHexData(const char * pstring, const uint32_t *buffer, size_t n_ints);

////////////////////////////////////////////////////////////////////////////////
/// \env{pami,PAMI_GLOBAL_SHMEMSIZE}
/// Number of bytes allocated from shared memory for global information
/// such as the mapcache.
/// May use the 'K' and 'k' suffix as a 1024 multiplier, or the
/// 'M' and 'm' suffix as a 1024*1024 multiplier.
///
/// \default 4M
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
/// \env{pami,PAMI_A2A_PACING_WINDOW}
/// Number of simultaneous send operations to start on Alltoall(v) collectives.
/// Only when these complete will more send operations be started.  This 
/// reduces the resource usage on large geometries.
///
/// \default 1024
////////////////////////////////////////////////////////////////////////////

namespace PAMI
{
  class Global : public Interface::Global<PAMI::Global>
  {
  public:

    inline Global () :
    personality (),
    mapping(personality),
    l2atomicFactory(),
    _mapcache (),
    _shmemThreshold(96*1024) // arbitrary threshold for number of nodes where shmem is handled differently: 96 racks
    {
      pami_coord_t ll, ur;
      pami_task_t min = 0, max = 0;
      const char   * shmemfile = "/pami-global-shmem";
      size_t   bytes;
      size_t   pagesize  = 4096;
      char *envopts;
      envopts = getenv("PAMI_SHMEM_NNODE_THRESHOLD");
      if(envopts) _shmemThreshold = atoi(envopts);
      
      envopts = getenv("PAMI_A2A_PACING_WINDOW");
      if(envopts) _a2a_pacing_window = atoi(envopts);
      else _a2a_pacing_window = 1024;

      envopts = getenv("PAMI_DEVICE_1");
      _useshmem = (personality.tSize() > 1);
      _useMU = true;

      time.init(personality.clockMHz());

      heap_mm = new ((PAMI::Memory::MemoryManager*)_heap_mm) PAMI::Memory::HeapMemoryManager();
      PAMI::Memory::MemoryManager::heap_mm = heap_mm;
      if(personality.tSize() == 1)
      {
        // There is no shared memory, so don't try. Fake using heap.
        shared_mm = heap_mm;
      }
      else
      {
        shared_mm = new (_shared_mm) PAMI::Memory::SharedMemoryManager(0, heap_mm);
      }
      PAMI::Memory::MemoryManager::shared_mm = shared_mm;
      PAMI::Memory::MemoryManager::shm_mm = &mm;  // not initialized yet!

      /// \todo #80 #99 Remove this when the DMA supports >1 context.
      /// Hopefully this is temporary. We should always include all
      /// the devices and have run time checks. But with no MU on
      /// the FPGA, this is required now, or you have to build
      /// multiple libraries.
      /// \note 4/13/2010: Enabling *just* MU does not work. Both or
      /// shmem are the only viable options.
      if(envopts != NULL)
      {
        if(strncasecmp(envopts, "B", 1) == 0) // use BOTH
        {
          _useshmem = true;
          _useMU = true;
        }
        else if(strncasecmp(envopts, "S", 1) == 0) // SHMEM only
        {
          _useshmem = true;
          _useMU = false;
        }
        else if(strncasecmp(envopts, "M", 1) == 0) // MU only
        {
          _useshmem = false;
          _useMU = true;
        }
        else
        {
          fprintf(stderr, "Invalid device option %s\n", envopts);
        }

        if(_useshmem) TRACE_ERR((stderr, "Using shmem device\n"));

        if(_useMU) TRACE_ERR((stderr, "Using MU device\n"));
      }

      size_t ppn = Kernel_ProcessCount();
      size_t nn = personality.aSize() * personality.bSize() *
                  personality.cSize() * personality.dSize() * personality.eSize();
      // circular dependencies prevent using
      // __MUGlobal.getMuRM().getPerProcessMaxPamiResources();
      size_t num_ctx = 256 / ppn; // Why not platform MAX_CONTEXTS?
      size_t master_rank_list_sz = nn * sizeof(pami_task_t);
      if (nn > _shmemThreshold) master_rank_list_sz = 0; // don't put it in shmem if past shmem threshold
      bytes = (56*1024) + (nn * ppn * 8) + master_rank_list_sz;
      if(ppn > 1)
      {
        bytes += num_ctx * (12*1024) + ppn * (8*1024);
      }

      char *env = getenv("PAMI_GLOBAL_SHMEMSIZE");
      if(env)
      {
        char *s = NULL;
        bytes = strtoull(env, &s, 0);
        if(*s == 'm' || *s == 'M') bytes *= 1024 * 1024;
        else if(*s == 'k' || *s == 'K') bytes *= 1024;
      }
      // Round up to the page size
      size_t size = ((bytes + pagesize - 1) & ~(pagesize - 1));

      TRACE_ERR((stderr, "Global() .. size = %zu\n", size));
      pami_result_t rc = mm.init(shared_mm, size, 1, 1, 0, shmemfile);
      PAMI_assert_alwaysf(rc == PAMI_SUCCESS, "Failed to create shm mm %zd bytes \"%s\"",
                          size, shmemfile);
      (void)initializeMapCache(personality, &mm, ll, ur, min, max,
                               ((mm.attrs() & PAMI::Memory::PAMI_MM_NODESCOPE) != 0)); //shared initialization

      mapping.init(_mapcache, personality);
      PAMI::Topology::static_init(&mapping);
      size_t rectsize = 1;

      for(unsigned d = 0; d < mapping.globalDims(); ++d)
      {
        rectsize *= (ur.u.n_torus.coords[d] - ll.u.n_torus.coords[d] + 1);
      }

      TRACE_ERR((stderr,  "Global() mapping.size %zu, rectsize %zu,mapping.globalDims %zu, min %u, max %u\n", mapping.size(), rectsize, mapping.globalDims(), min, max));

      if(mapping.size() == rectsize)
      {
        new (&topology_global) PAMI::Topology(&ll, &ur);
      }
      else if(mapping.size() == max - min + 1)
      {
        new (&topology_global) PAMI::Topology(min, max);
      }
      else
      {
        PAMI_abortf("failed to build global-world topology %zu::%zu(%zu) / %d..%d", mapping.size(), rectsize, mapping.globalDims(), min, max); //hack
      }

      topology_global.subTopologyLocalToMe(&topology_local);
      PAMI_assert_alwaysf(topology_local.size() >= 1, "Failed to create valid (non-zero) local topology\n");

      l2atomicFactory.init(shared_mm, heap_mm, &mapping, &topology_local);

      TRACE_ERR((stderr, "Global() <<\n"));

      return;
    };



    inline ~Global ()
    {
    }

    inline bgq_mapcache_t * getMapCache ()
    {
      return &_mapcache;
    };

    inline size_t size ()
    {
      return _mapcache.size;
    }

    inline size_t local_size () //hack
    {
      return _mapcache.local_size;
    }
    inline bool useshmem()
    {
      return _useshmem;
    }
    inline bool useMU()
    {
      return _useMU;
    }
    /// \todo temp function while MU2 isn't complete
    inline bool useMU(bool flag)
    {
      bool oldFlag = _useMU;
      _useMU = flag;
      return oldFlag;
    }


    /// \brief Get Comm Agent Control Structure
    ///
    /// \retval  struct  The comm agent control structure used on
    ///                  functions that work with the comm agent.
    ///
    inline CommAgent_Control_t getCommAgentControlStruct()
    {
      return _commAgentControl;
    }


    /// \brief Is Comm Agent Running
    ///
    /// \retval  true  Comm Agent is running.
    ///          false Comm Agent is not running.
    ///
    inline bool isCommAgentRunning()
    {
      return _isCommAgentRunning;
    }

    /// \brief Get Flexability Metric Routing Options
    ///
    /// Returns a 2 element array containing the routing options when
    /// the flexability metric between 2 nodes is within the flexability metric
    /// range, and outside of it.
    /// 
    /// The routing options are values 0,1,2,3, or 4 for the 4 dynamic
    /// routing zones (0,1,2,3) and deterministic routing (4).
    ///
    inline unsigned int *getFlexabilityMetricRoutingOptions ()
    {
      return _flexabilityMetricRangeRouting;
    }

    /// \brief Get Small Routing Size
    ///
    /// When the message size is <= this value, the small routing value
    /// determines the routing for the message.
    ///
    /// \ref getSmallRouting()
    ///
    inline size_t getSmallRoutingSize()
    {
      return _smallRoutingSize;
    }

    /// \brief Get Small Routing
    ///
    /// When the message size <= small routing size, this routing value
    /// is used to route the message.
    ///
    /// \ref getSmallRoutingSize()
    ///
    inline unsigned int getSmallRouting()
    {
      return _smallRouting;
    }

    /// \brief Get Rget Pacing Size
    ///
    inline size_t getRgetPacingSize()
    {
      return _rgetPacingSize;
    }

  private:

    inline size_t initializeMapCache (BgqJobPersonality  & personality,
                                      PAMI::Memory::MemoryManager *mm,
                                      pami_coord_t &ll, pami_coord_t &ur, pami_task_t &min, pami_task_t &max, bool shared);

  public:

    BgqJobPersonality       personality;
    PAMI::Mapping         mapping;
    PAMI::Atomic::BGQ::L2AtomicFactory l2atomicFactory;
    PAMI::Memory::GenMemoryManager mm;
    PAMI::Memory::MemoryManager *_wuRegion_mm; // this process
    // This must be GenMemoryManager so that array arith works
    PAMI::Memory::GenMemoryManager *_wuRegion_mms; // all processes on node

  private:



    ////////////////////////////////////////////////////////////////////////////////
    /// \envs{pami,rgetpacing,Remote Get Pacing}
    /// PAMI remote get pacing interacts with the communication agent to
    /// complete the entire pacing operation.  The communication agent can be
    /// individually configured which will affect the behavior of the PAMI
    /// remote get functions.

    ////////////////////////////////////////////////////////////////////////////////
    /// \env{rgetpacing,PAMI_RGETPACING}
    /// Specifies whether or not to consider messages for pacing.
    ///
    /// A pacing request will be posted to the communication agent to
    /// complete the pacing operation only if the message satisfies the PAMI
    /// pacing criteria. The communication agent may, or may not, pace
    /// the message depending on the communication agent configuration.
    ///
    /// - 0 means that no messages will be paced.
    /// - 1 means that messages will be considered for pacing.
    /// \default 0 if the block size is 1 rack (1024 nodes) or smaller.
    ///          1 if the block size is larger than 1 rack.
    ///
    /// \see \ref COMMAGENT_RGETPACING
    ///
    /// \env{rgetpacing,PAMI_RGETPACINGHOPS}
    /// Messages between nodes that are more than this
    /// many hops apart on the network will be considered for pacing.
    ///
    /// \default 4
    ///
    /// \env{rgetpacing,PAMI_RGETPACINGDIMS}
    /// Messages between nodes whose coordinates differ
    /// in more than this many dimensions in ABCD are considered for pacing.  
    /// For example, node A has ABCD coordinates (0,0,0,0) and node B has 
    /// (3,2,1,0).  They differ in 3 dimensions (A, B, and C).  Specifying 2 
    /// means that messages between these nodes will be considered for pacing.
    ///
    /// \default 1
    ///
    /// \env{rgetpacing,PAMI_RGETPACINGSIZE}
    /// Messages exceeding this size in bytes are considered for pacing.
    ///
    /// \default 65536
    ////////////////////////////////////////////////////////////////////////////////


    /// \brief Initialize Comm Agent
    ///
    /// Initialize the comm agent.  The comm agent is loaded (by default) by CNK.
    /// It runs in a thread on the 17th core.  This initialization will wait for
    /// it to start.  If it does not start within a timeout period, a message
    /// will be printed and messaging will run without it.
    ///
    inline void initializeCommAgent( size_t aCoord, size_t bCoord, size_t cCoord, size_t dCoord, size_t eCoord, size_t tCoord );


    /// \brief Initialize Remote Get Pacing
    ///
    /// Fetch remote get pacing env vars
    /// 
    /// \param[in]   blockSize      Number of nodes in the block
    /// \param[out]  doRgetPacing   Indicates whether rget pacing should be done at all.
    /// \param[out]  hops           If the distance between our node and the dest node
    ///                             is greater than or equal to "hops", rgets may be
    ///                             eligible for pacing.
    /// \param[out]  dims           If our node and the dest coords differ in "dims" 
    ///                             dimensions, rgets may be eliglble for pacing.
    /// \param[out]  size           Rget Pacing Size threshold, above which messages
    ///                             are considered for pacing.
    ///
    /// \see paceRgets()
    ///
    inline void initializeRgetPacing ( size_t blockSize,
                                       bool   &doRgetPacing,
                                       size_t &hops,
                                       size_t &dims,
                                       size_t &size);

    /// \brief Determine Whether to Pace Remote Gets
    ///
    /// Our node's ABCD coords are compared with the dest node's abcd coords
    /// to determine if remote get pacing should be considered between these
    /// two nodes.  The E coord is not considered.  
    ///
    /// - If the distance between our node and the dest node is greater than
    ///   or equal to "hops", rgets may be eligible for pacing.
    /// - If our node and the dest coords differ in "dims" dimensions or
    ///   greater, rgets may be eliglble for pacing.
    ///
    /// \retval  true  Remote gets should be considered for pacing.
    /// \retval  false Remote gets should not be considered for pacing.
    ///
    inline bool paceRgets( bool   doRgetPacing,
                           size_t hops,
                           size_t dims,
                           size_t aSource,
                           size_t bSource,
                           size_t cSource,
                           size_t dSource,
                           size_t aDest,
                           size_t bDest,
                           size_t cDest,
                           size_t dDest,
                           size_t aSize,
                           size_t bSize,
                           size_t cSize,
                           size_t dSize );

    /// \brief Calculate Number of Hops Along a Line, Assuming a Torus
    inline size_t getHops ( size_t source, size_t dest, size_t dimSize )
    {
      ssize_t signedHops;
      size_t  unsignedHops;
      size_t  dimSizeHalved;

      dimSizeHalved = dimSize >> 1;
      signedHops = (ssize_t)dest - (ssize_t)source;
      unsignedHops = abs_x ( signedHops );
      if( unsignedHops > dimSizeHalved ) unsignedHops = dimSize - unsignedHops;

      return unsignedHops;
    }

    /// \brief Initialize Values Associated with the Flexability Metric.
    /// 
    /// This processes the PAMI_ROUTING env var and stores the result.
    ///
    inline void initializeFlexabilityMetric ( BgqJobPersonality  & personality );


    /// \brief Determine The Routing Between Two Nodes Using Flexability Metric
    ///
    /// Our node's ABCD coords are compared with the dest node's abcd coords
    /// to compute the flexability metric.  Then, the flexabilty metric thresholds
    /// are applied to determine which of two types of routing to use for
    /// rendezvous messages between these two nodes.
    ///
    /// \retval  0  Use routing option 0.
    /// \retval  1  Use routing option 1.
    ///
    /// The actual routing option for 0 or 1 is determined by the PAMI_ROUTING
    /// env var (or its defaults).
    ///
    inline unsigned int getFlexability ( size_t srcAcoord,
                                         size_t srcBcoord,
                                         size_t srcCcoord,
                                         size_t srcDcoord,
                                         size_t destAcoord,
                                         size_t destBcoord,
                                         size_t destCcoord,
                                         size_t destDcoord,
                                         size_t Asize,
                                         size_t Bsize,
                                         size_t Csize,
                                         size_t Dsize );

    bgq_mapcache_t   _mapcache;
    size_t           _size;
    bool _useshmem;
    bool _useMU;
    bool _isCommAgentRunning;
    CommAgent_Control_t _commAgentControl;          // Comm Agent control struct.
    size_t       _rgetPacingSize;                   // Message size threshold for rget pacing.
    size_t       _smallRoutingSize;                 // Small routing size.
    unsigned int _smallRouting;                     // Routing to use when message <= _smallRoutingSize.
    float        _flexabilityMetricRange[2];        // Flexability Metric low/high range values.
    unsigned int _flexabilityMetricRangeRouting[2]; // Routing (0,1,2,3,4) when in and out of range.
    size_t       _shmemThreshold;                   // Some threshold for number of nodes where we change shmem usage
  public:
    unsigned _a2a_pacing_window;
  }; // PAMI::Global
};     // PAMI

#ifdef USE_COMMTHREADS
extern PAMI::Device::CommThread::Factory __commThreads;
#endif // USE_COMMTHREADS

void PAMI::Global::initializeCommAgent( size_t aCoord, size_t bCoord, size_t cCoord, size_t dCoord, size_t eCoord, size_t tCoord )
{
  int rc;

  _isCommAgentRunning = false;

  // Try to init the comm agent.
  // If it is not there, this will return with rc=2 (ENOENT).
  rc = CommAgent_Init ( &_commAgentControl );

  if( rc == 0 )
  {
    // Verify version number.
    CommAgent_State_t version;
    version = CommAgent_GetVersion ( _commAgentControl );
    PAMI_assert_alwaysf(version >= COMM_AGENT_STATE_INITIALIZED_VERSION_2, "The Messaging App Agent (Comm Agent) has version %d, but version %d or later is required.  Upgrade to the latest agent.\n",version,COMM_AGENT_STATE_INITIALIZED_VERSION_2);
    _isCommAgentRunning = true;
  }
  else
  {
    // If the rc is ENOMEM, it is most likely due to there being no shared memory.
    // If the agent isn't there, the rc will be ENOENT.
    // Continue onward without the agent.  
    if( ( rc == ENOENT ) || ( rc == ENOMEM ) )
    {
      // Only print in 1 process.
      if( (aCoord + bCoord + cCoord + dCoord + eCoord + tCoord) == 0 )
        printf("Warning:  The Messaging App Agent (Comm Agent) is not running.  It may have been disabled (BG_APPAGENTCOMM=DISABLE), or there may be no shared memory available, or redirecting the mmcs console output for the block (redirect_block on) and re-running may show error messages.  Messaging will continue to run, but has the following limitations:  1) Remote Get Pacing is not available (potentially causing network congestion, reducing performance), and 2) One-sided-put-fence operations will abort.\n");
    }
    else PAMI_assert_alwaysf(rc==0, "Messaging App Agent (Comm Agent) failed to initialize, rc=%d\n",rc);
  }
}

void PAMI::Global::initializeRgetPacing ( size_t blockSize,
                                          bool   &doRgetPacing,
                                          size_t &hops,
                                          size_t &dims,
                                          size_t &size )
{
  char *s;
  unsigned long v;

  // Set the default values.
  // Default the pacing flag based on the block size...more than 1 rack 
  // defaults to pacing=ON...less than or equal to 1 rack defaults to
  // pacing=OFF.  If PAMI_RGETPACING is specified, then the
  // value of that flag determines whether or not pacing is ON.
  doRgetPacing = (blockSize > 1024) ? true : false;
  hops         = 4;
  dims         = 1;
  size         = 65536;

  s = getenv( "PAMI_RGETPACING" );
  if( s )
  {
    v = strtoul( s, 0, 10 );
    if( v == 0 ) doRgetPacing = false;
    else doRgetPacing = true;
  }

  if( _isCommAgentRunning == false ) doRgetPacing = false;

  s = getenv( "PAMI_RGETPACINGHOPS" );
  if( s )
  {
    v = strtoul( s, 0, 10 );
    hops = (size_t)v;
  }

  s = getenv( "PAMI_RGETPACINGDIMS" );
  if( s )
  {
    v = strtoul( s, 0, 10 );
    dims = (size_t)v;
  }

  s = getenv( "PAMI_RGETPACINGSIZE" );
  if( s )
  {
    v = strtoul( s, 0, 10 );
    size = (size_t)v;
  }
  TRACE_ERR((stderr,"InitializeRgetPacing: doRgetPacing=%d, hops=%zu, dims=%zu, size=%zu\n",doRgetPacing,hops,dims,size));
}


bool PAMI::Global::paceRgets( bool   doRgetPacing,
                              size_t hops,
                              size_t dims,
                              size_t aSource, // source coords
                              size_t bSource,
                              size_t cSource,
                              size_t dSource,
                              size_t aDest,      // dest coords
                              size_t bDest,
                              size_t cDest,
                              size_t dDest,
                              size_t aSize,
                              size_t bSize,
                              size_t cSize,
                              size_t dSize )
{
  TRACE_ERR((stderr,"paceRgets: myCoords=(%zu,%zu,%zu,%zu), Dest=(%zu,%zu,%zu,%zu)\n",aSource,bSource,cSource,dSource,aDest,bDest,cDest,dDest));
  // If the caller does not want rget pacing, return false.
  if( doRgetPacing == false ) return false;

  // If the number of hops between our node and the dest node exceeds "hops", return true.
  size_t actualHops;
  actualHops = 0;
  actualHops += getHops ( aSource, aDest, aSize );
  actualHops += getHops ( bSource, bDest, bSize );
  actualHops += getHops ( cSource, cDest, cSize );
  actualHops += getHops ( dSource, dDest, dSize );
  TRACE_ERR((stderr,"paceRgets: actualHops=%zu\n",actualHops));
  if( actualHops > hops ) return true;

  // If the number of dimensions in ABCD that differ between our node and the dest node
  // exceeds "dims", return true.
  size_t actualDims;
  actualDims = 0;
  if( aSource != aDest ) actualDims++;
  if( bSource != bDest ) actualDims++;
  if( cSource != cDest ) actualDims++;
  if( dSource != dDest ) actualDims++;
  TRACE_ERR((stderr,"paceRgets: actualDims=%zu\n",actualDims));
  if( actualDims > dims ) return true;

  return false;
}


////////////////////////////////////////////////////////////////////////////////
/// \envs{pami,routing,Message Routing}
/// Point-to-Point messages that are transferred using the rendezvous protocol
/// may be routed deterministically or dynamically.  If dynamically routed,
/// they may use one of 4 zone routing techniques.  PAMI defaults the routing
/// but it may be overridden using environment variables.
/// \see \ref PAMID_EAGER
/// \see \ref PAMI_ROUTING

////////////////////////////////////////////////////////////////////////////////
/// \env{routing,PAMI_ROUTING}
/// Specifies PAMI network routing options to be used for point-to-point
/// messages that are large enough to use the rendezvous protocol (larger than
/// the eager limit (PAMID_EAGER) ).
///
/// The complete syntax is
/// <tt> PAMI_ROUTING=[size][,[small][,[low:high][,[in][,out]]]] </tt>
///
/// When the source and destination nodes are on a network line (their ABCDE
/// coordinates differ in at most 1 dimension), deterministic routing is always
/// used.  PAMI_ROUTING does not override this.
///
/// When the message size is less than or equal to <tt>size</tt>, PAMI uses the
/// <tt>small</tt> network routing (see below for possible values).
///
/// When the message size is larger than <tt>size</tt>, PAMI uses the
/// "flexability metric" to determine the network routing as follows:
/// <tt>low:high</tt> is the "flexability metric" range.  The flexability metric
/// guages the routing flexability between a given source node and destination
/// node.  Values for "low" and "high" are each floating point numbers ranging
/// from 0.0 through 4.0.  "low" must be less than or equal to "high".  PAMI
/// computes the flexability metric between the source node and destination node
/// of a messaging transfer as the sum of the flexability of dimensions A, B, C,
/// and D between those nodes.  The flexability of a particular dimension is
/// the ratio of the number of hops between the source and the destination in
/// that dimension and the size of that dimension, and can range from 0.0
/// through 1.0.
///
/// <tt>in</tt> is the network routing to be used for a message transfer
/// between two nodes when their flexability metric is between "low" and
/// "high", and <tt>out</tt> is the network routing otherwise.  
///
/// The values for <tt>in</tt>, <tt>out</tt>, and <tt>small</tt>  may each be
/// one of the following values:
/// - 0: Dynamic routing zone 0.
/// - 1: Dynamic routing zone 1.
/// - 2: Dynamic routing zone 2.
/// - 3: Dynamic routing zone 3.
/// - 4: Deterministic routing.
///
/// \verbatim
/// Default values based upon system size:
///
/// BLOCK SIZE           Size   Small   Flexability Metric Range   Routing when   Routing when 
///                                     low:high                   "in" Range     "out" of Range
/// ------------------   -----  -----   ------------------------   ------------   --------------
///  32 <= Nodes <   64  65536    4         1.5 - 3.5                    3              3
///  64 <= Nodes <  128  65536    4         1.5 - 3.5                    3              3
/// 128 <= Nodes <  256  65536    4         1.5 - 3.5                    3              3
/// 256 <= Nodes <  512  65536    4         1.5 - 3.5                    3              3
/// 512 <= Nodes < 1024  65536    4         1.5 - 3.5                    3              3
///   1 <= Racks <  2    65536    4         1.5 - 3.5                    2              0
///   2 <= Racks <  4    65536    4         1.5 - 3.5                    3              0
///   4 <= Racks <  8    65536    4         1.5 - 3.5                    3              0
///   8 <= Racks < 16    65536    4         1.5 - 2.5                    3              0
///  16 <= Racks < 32    65536    4         1.3 - 3.0                    3              0
///  32 <= Racks < 48    65536    4         1.3 - 3.0                    3              0
///  48 <= Racks < 64    65536    4         1.3 - 3.0                    3              0
///  64 <= Racks < 80    65536    4        0.75 - 3.0                    3              0
///  80 <= Racks < 96    65536    4        0.75 - 3.0                    3              0
///  96 <= Racks         65536    4        0.75 - 3.0                    3              0
/// \endverbatim
////////////////////////////////////////////////////////////////////////////////

/// \brief Initialize Values Associated with the Flexability Metric.
/// 
/// This processes the PAMI_ROUTING env var and stores the result.
///
void PAMI::Global::initializeFlexabilityMetric ( BgqJobPersonality  & personality )
{
  float defaultFlexabilityMetricRange[15][2] = { { 1.5, 3.5},   /*    32    */
    { 1.5, 3.5},   /*    64    */
    { 1.5, 3.5},   /*   128    */
    { 1.5, 3.5},   /*   256    */
    { 1.5, 3.5},   /*   512    */
    { 1.5, 3.5},   /*  1 rack  */
    { 1.5, 3.5},   /*  2 racks */
    { 1.5, 3.5},   /*  4 racks */
    { 1.5, 2.5},   /*  8 racks */
    { 1.3, 3.0},   /* 16 racks */
    { 1.3, 3.0},   /* 32 racks */
    { 1.3, 3.0},   /* 48 racks */
    { 0.75, 3.0},  /* 64 racks */
    { 0.75, 3.0},  /* 80 racks */
    { 0.75, 3.0}};/* 96 racks */
  unsigned int defaultRouting[15][2] = { { 3, 3},   /*    32    */
    { 3, 3},   /*    64    */
    { 3, 3},   /*   128    */
    { 3, 3},   /*   256    */
    { 3, 3},   /*   512    */
    { 2, 0},   /*  1 rack  */
    { 3, 0},   /*  2 racks */
    { 3, 0},   /*  4 racks */
    { 3, 0},   /*  8 racks */
    { 3, 0},   /* 16 racks */
    { 3, 0},   /* 32 racks */
    { 3, 0},   /* 48 racks */
    { 3, 0},   /* 64 racks */
    { 3, 0},   /* 80 racks */
    { 3, 0}}; /* 96 racks */
  size_t defaultSize[15] = { 65536,  /*    32    */
    65536,  /*    64    */
    65536,  /*   128    */
    65536,  /*   256    */
    65536,  /*   512    */
    65536,  /*  1 rack  */
    65536,  /*  2 racks */
    65536,  /*  4 racks */
    65536,  /*  8 racks */
    65536,  /* 16 racks */
    65536,  /* 32 racks */
    65536,  /* 48 racks */
    65536,  /* 64 racks */
    65536,  /* 80 racks */
    65536}; /* 96 racks */
  unsigned int defaultSmall[15] = { 4,  /*    32    */
    4,  /*    64    */
    4,  /*   128    */
    4,  /*   256    */
    4,  /*   512    */
    4,  /*  1 rack  */
    4,  /*  2 racks */
    4,  /*  4 racks */
    4,  /*  8 racks */
    4,  /* 16 racks */
    4,  /* 32 racks */
    4,  /* 48 racks */
    4,  /* 64 racks */
    4,  /* 80 racks */
    4}; /* 96 racks */

  // Compute block size
  size_t blockSize;
  blockSize =
  personality.aSize() *
  personality.bSize() *
  personality.cSize() *
  personality.dSize() *
  personality.eSize();

  // Establish defaults based on system size.
  size_t index;
  index = 14; /* Set for largest system size */
  if( blockSize < 96*1024 ) index = 13;
  if( blockSize < 80*1024 ) index = 12;
  if( blockSize < 64*1024 ) index = 11;
  if( blockSize < 48*1024 ) index = 10;
  if( blockSize < 32*1024 ) index = 9;
  if( blockSize < 16*1024 ) index = 8;
  if( blockSize <  8*1024 ) index = 7;
  if( blockSize <  4*1024 ) index = 6;
  if( blockSize <  2*1024 ) index = 5;
  if( blockSize <  1*1024 ) index = 4;
  if( blockSize <  512    ) index = 3;
  if( blockSize <  256    ) index = 2;
  if( blockSize <  128    ) index = 1;
  if( blockSize <   64    ) index = 0;
  _smallRoutingSize = defaultSize[index]; // Small routing size value.
  _smallRouting     = defaultSmall[index];// Small routing when message <= _smallRoutingSize.
  _flexabilityMetricRange[0] = defaultFlexabilityMetricRange[index][0]; // Flexability Metric low  range value.
  _flexabilityMetricRange[1] = defaultFlexabilityMetricRange[index][1]; // Flexability Metric high range value.
  _flexabilityMetricRangeRouting[0] = defaultRouting[index][0]; // Routing (0,1,2,3,4) when in range.
  _flexabilityMetricRangeRouting[1] = defaultRouting[index][1]; // Routing (0,1,2,3,4) when in range.

  TRACE_ERR((stderr,"Default small routing size = %zu, small routing = %u, flexability range = %g - %g, routing = %u - %u\n",_smallRoutingSize,_smallRouting,_flexabilityMetricRange[0],_flexabilityMetricRange[1],_flexabilityMetricRangeRouting[0],_flexabilityMetricRangeRouting[1]));

  // Get the PAMI_ROUTING env var.
  char *envVar;
  envVar = getenv ( "PAMI_ROUTING" );

  // If the PAMI_ROUTING env var is not specified, return with default values set.
  if( envVar == NULL )
  {
    return;
  }

  // Find out how long the env var string is, so we can make a copy of it
  unsigned int envVarLen = 0;
  while( envVar[envVarLen++] != '\0' );

  // Allocate space for a copy of the colon and comma-delimited list of values
  pami_result_t prc;
  char *envVarCopy;
  prc = heap_mm->memalign((void **)&envVarCopy, 0, envVarLen);
  PAMI_assertf(prc == PAMI_SUCCESS, "alloc of PAMI_ROUTINE env var copy failed");

  // Copy the env var string into our writeable copy
  unsigned int i;
  for( i=0; i<envVarLen; i++ )
    envVarCopy[i] = envVar[i];

  char *currentChar = envVarCopy;

  // Parse the env var:  unsigned long,unsigned,range,unsigned,unsigned

  // First, handle the Small Routing Size.
  char *v;           // "v" for "value"
  unsigned int vLen; // Length of value
  v = currentChar;   // Point to first character of the "size" value.
  vLen = 0;
  while( ( *currentChar != ','  ) &&
         ( *currentChar != '\0' ) )
  {
    currentChar++;
    vLen++;
  }

  // If there is no "size" value...
  if( vLen == 0 )
  {
    // If remainder of env var is empty, return with defaults set.
    if( *currentChar == '\0' )
    {
      return;
    }

    // "size" value is empty and we hit a comma.  Use defaults for "size" routing value.
    currentChar++;
  }
  else
  {
    // "size" routing value is not empty.  Extract it.
    *currentChar = '\0';      // Null terminate the value string.
    errno=0;
    _smallRoutingSize = strtoul( v, NULL, 10 );
    PAMI_assert_alwaysf( (errno==0) && (_smallRoutingSize >= 0),"Incorrect PAMI_ROUTING env var");

    TRACE_ERR((stderr,"Small Routing Size overridden to %zu\n",_smallRoutingSize));

    currentChar++;
  }

  // Next, handle the Small Routing Value.
  v = currentChar;   // Point to first character of the "small" value.
  vLen = 0;
  while( ( *currentChar != ','  ) &&
         ( *currentChar != '\0' ) )
  {
    currentChar++;
    vLen++;
  }

  // If there is no "small" value...
  if( vLen == 0 )
  {
    // If remainder of env var is empty, return with defaults set.
    if( *currentChar == '\0' )
    {
      return;
    }

    // "small" value is empty and we hit a comma.  Use defaults for "small" routing value.
    currentChar++;
  }
  else
  {
    // "small" routing value is not empty.  Extract it.
    *currentChar = '\0';      // Null terminate the value string.
    errno=0;
    _smallRouting = strtoul( v, NULL, 10 );
    PAMI_assert_alwaysf( (errno==0) && (_smallRouting >= 0) && (_smallRouting <= 4),"Incorrect PAMI_ROUTING env var");

    TRACE_ERR((stderr,"Small Routing overridden to %u\n",_smallRouting));

    currentChar++;
  }

  //
  // Next, handle the Flexability Metric range in this format: float:float
  v = currentChar;   // Point to first character of the range
  vLen = 0;
  while( ( *currentChar != ','  ) &&
         ( *currentChar != ':'  ) &&
         ( *currentChar != '\0' ) )
  {
    currentChar++;
    vLen++;
  }

  // If there is no range value...
  if( vLen == 0 )
  {
    // If entire env var is empty, return with defaults set.
    if( *currentChar == '\0' )
    {
      return;
    }

    // If start of range is empty, report error.
    PAMI_assert_alwaysf( *currentChar != ':',"Incorrect PAMI_ROUTING env var");

    // Range is empty and we hit a comma.  Use defaults for range.
    currentChar++;
  }
  else
  {
    // Range is not empty.  Extract the start of the range.
    *currentChar = '\0';      // Null terminate the value string.
    errno=0;
    _flexabilityMetricRange[0] = strtof( v, NULL );
    PAMI_assert_alwaysf( (errno==0) && (_flexabilityMetricRange[0] >= 0.0) && (_flexabilityMetricRange[0] <= 4.0),"Incorrect PAMI_ROUTING env var");

    TRACE_ERR((stderr,"Flexability start of range overridden to %g\n",_flexabilityMetricRange[0]));

    // Get the value that ends the range.
    currentChar++;
    v = currentChar;   // Point to first character of the second range value
    vLen = 0;
    while( ( *currentChar != ','  ) &&
           ( *currentChar != '\0' ) )
    {
      currentChar++;
      vLen++;
    }
    PAMI_assert_alwaysf( vLen > 0,"Incorrect PAMI_ROUTING env var");

    // 2nd half of Range is not empty.  Extract the end of the range.
    *currentChar = '\0';      // Null terminate the value string.
    errno=0;
    _flexabilityMetricRange[1] = strtof( v, NULL );
    PAMI_assert_alwaysf( (errno==0) && (_flexabilityMetricRange[1] >= 0.0) && (_flexabilityMetricRange[1] <= 4.0) && (_flexabilityMetricRange[0] <= _flexabilityMetricRange[1]),"Incorrect PAMI_ROUTING env var");

    TRACE_ERR((stderr,"Flexability end of range overridden to %g\n",_flexabilityMetricRange[1]));

    currentChar++;
  }

  // Next, handle the "in" routing value.
  v = currentChar;   // Point to first character of the "in" routing value.
  vLen = 0;
  while( ( *currentChar != ','  ) &&
         ( *currentChar != '\0' ) )
  {
    currentChar++;
    vLen++;
  }

  // If there is no "in" value...
  if( vLen == 0 )
  {
    // If remainder of env var is empty, return with defaults set.
    if( *currentChar == '\0' )
    {
      return;
    }

    // "in" routing value is empty and we hit a comma.  Use defaults for "in" routing value.
    currentChar++;
  }
  else
  {
    // "in" routing value is not empty.  Extract it.
    *currentChar = '\0';      // Null terminate the value string.
    errno=0;
    _flexabilityMetricRangeRouting[0] = strtoul( v, NULL, 10 );
    PAMI_assert_alwaysf( (errno==0) && (_flexabilityMetricRangeRouting[0] >= 0) && (_flexabilityMetricRangeRouting[0] <=4),"Incorrect PAMI_ROUTING env var");

    TRACE_ERR((stderr,"Flexability Routing[0] overridden to %u\n",_flexabilityMetricRangeRouting[0]));

    currentChar++;
  }

  // Next, handle the "out" routing value.
  v = currentChar;   // Point to first character of the "out" routing value.
  vLen = 0;
  while( ( *currentChar != ','  ) &&
         ( *currentChar != '\0' ) )
  {
    currentChar++;
    vLen++;
  }

  // If there is no "out" value...
  if( vLen == 0 )
  {
    // If remainder of env var is empty, return with defaults set.
    if( ( *currentChar == '\0' ) || (*currentChar == ',') )
    {
      return;
    }
  }
  else
  {
    // "out" routing value is not empty.  Extract it.
    *currentChar = '\0';      // Null terminate the value string.
    errno=0;
    _flexabilityMetricRangeRouting[1] = strtoul( v, NULL, 10 );
    PAMI_assert_alwaysf( (errno==0) && (_flexabilityMetricRangeRouting[1] >= 0) && (_flexabilityMetricRangeRouting[1] <=4),"Incorrect PAMI_ROUTING env var");

    TRACE_ERR((stderr,"Flexability Routing[1] overridden to %u\n",_flexabilityMetricRangeRouting[1]));
  }
}


/// \brief Determine The Routing Between Two Nodes Using Flexability Metric
///
/// Our node's ABCD coords are compared with the dest node's abcd coords
/// to compute the flexability metric.  Then, the flexabilty metric thresholds
/// are applied to determine which of two types of routing to use for
/// rendezvous messages between these two nodes.
///
/// \retval  0  Use routing option 0.
/// \retval  1  Use routing option 1.
///
/// The actual routing option for 0 or 1 is determined by the PAMI_ROUTING
/// env var (or its defaults).
///
/// The flexibility metric (for a torus in each dimension) is calculated as follows:
///
/// flex = 0;
/// for each dimension i, except E {
///     flex += dist(src, dest) / max_dist(i);
/// }
/// if (flex > Tlow and flex < Thigh) { use routing option 0} else { use routing option 1}
///
/// Tlow and Thigh correspond to _flexabilityMetricRange[0] and _flexabilityMetricRange[1].
///
unsigned int PAMI::Global::getFlexability ( size_t srcAcoord,
                                            size_t srcBcoord,
                                            size_t srcCcoord,
                                            size_t srcDcoord,
                                            size_t destAcoord,
                                            size_t destBcoord,
                                            size_t destCcoord,
                                            size_t destDcoord,
                                            size_t Asize,
                                            size_t Bsize,
                                            size_t Csize,
                                            size_t Dsize )
{
  float flex, Adist, Bdist, Cdist, Ddist, Amax, Bmax, Cmax, Dmax;

  flex = 0;

  // Compute for A dimension
  if ( Asize > 1 )
  {
    Adist = (float) getHops ( srcAcoord, destAcoord, Asize );
    Amax  = (float) (Asize >> 1);
    flex += Adist / Amax;
  }
  // Compute for B dimension
  if ( Bsize > 1 )
  {
    Bdist = (float) getHops ( srcBcoord, destBcoord, Bsize );
    Bmax  = (float) (Bsize >> 1);
    flex += Bdist / Bmax;
  }
  // Compute for C dimension
  if ( Csize > 1 )
  {
    Cdist = (float) getHops ( srcCcoord, destCcoord, Csize );
    Cmax  = (float) (Csize >> 1);
    flex += Cdist / Cmax;
  }
  // Compute for D dimension
  if ( Dsize > 1 )
  {
    Ddist = (float) getHops ( srcDcoord, destDcoord, Dsize );
    Dmax  = (float) (Dsize >> 1);
    flex += Ddist / Dmax;
  }

  TRACE_ERR((stderr,"Flexability metric between %zu,%zu,%zu,%zu and %zu,%zu,%zu,%zu, Adist=%g, Amax=%g, Aflex=%g,, Bdist=%g, Bmax=%g, Bflex=%g, Cdist=%g, Cmax=%g, Cflex=%g, Ddist=%g, Dmax=%g, Dflex=%g, totalFlex=%g\n",srcAcoord,srcBcoord,srcCcoord,srcDcoord,destAcoord,destBcoord,destCcoord,destDcoord,Adist,Amax,Adist/Amax,Bdist,Bmax,Bdist/Bmax,Cdist,Cmax,Cdist/Cmax,Ddist,Dmax,Ddist/Dmax,flex));

  if( ( flex > _flexabilityMetricRange[0] ) &&
      ( flex < _flexabilityMetricRange[1] ) )
    return 0;
  else
    return 1;
}


// If 'mm' is NULL, compute total memory needed for mapcache and return (doing nothing else).
size_t PAMI::Global::initializeMapCache (BgqJobPersonality  & personality,
                                         PAMI::Memory::MemoryManager *mm,
                                         pami_coord_t &ll, pami_coord_t &ur, pami_task_t &min, pami_task_t &max, bool shared)
{
  pami_result_t res;
  bgq_mapcache_t  * mapcache = &_mapcache;

  TRACE_ERR( (stderr, "Global::initializeMapCache() >> mm = %p, mapcache = %p\n", mm, mapcache));
  // This structure anchors pointers to the map cache and rank cache.
  // It is created in the static portion of shared memory in this
  // constructor, but exists there only for the duration of this
  // constructor.  It communicates mapping initialization information
  // to the other tasks running on this physical node.
  typedef struct cacheAnchors
  {
    struct
    {
      uint64_t enter;
      uint64_t exit;
    } atomic;
    volatile size_t numActiveRanksLocal; // Number of ranks on our physical node.
    volatile size_t numActiveRanksGlobal;// Number of ranks in the partition.
    volatile size_t numActiveNodesGlobal;// Number of nodes in the partition.
    volatile size_t idxMasterRanks;      // Number of master ranks found (t==0) 
    volatile unsigned isMasterRanksShmem; // Master ranks list is in shmem
    volatile pami_task_t maxRank;       // Largest valid rank
    volatile pami_task_t minRank;       // Smallest valid rank
    volatile pami_coord_t activeLLCorner;
    volatile pami_coord_t activeURCorner;
    volatile size_t       lowestTCoordOnMyNode;
    pami_task_t * volatile master_ranks; // A list of master ranks (first rank (t=0) on each node) 
  } cacheAnchors_t;

  //size_t myRank;



  size_t aCoord = personality.aCoord ();
  size_t bCoord = personality.bCoord ();
  size_t cCoord = personality.cCoord ();
  size_t dCoord = personality.dCoord ();
  size_t eCoord = personality.eCoord ();
  size_t tCoord = personality.tCoord ();

  size_t aSize  = personality.aSize ();
  size_t bSize  = personality.bSize ();
  size_t cSize  = personality.cSize ();
  size_t dSize  = personality.dSize ();
  size_t eSize  = personality.eSize ();
  size_t tSize  = personality.tSize ();

  initializeCommAgent( aCoord, bCoord, cCoord, dCoord, eCoord, tCoord );
  initializeFlexabilityMetric ( personality );

  TRACE_ERR( (stderr, "Global::initializeMapCache() .. myCoords{%zu %zu %zu %zu %zu %zu} size{%zu %zu %zu %zu %zu %zu}\n", aCoord, bCoord, cCoord, dCoord, eCoord, tCoord, aSize, bSize, cSize, dSize, eSize, tSize));

  // Calculate the number of potential tasks in this partition.
  size_t fullSize = aSize * bSize * cSize * dSize * eSize * tSize;

  // Calculate the number of potential tasks on a node in this partition.
  size_t peerSize = tSize;

  if(!mm)
  {
    size_t mapsize = sizeof(cacheAnchors_t) +
                     fullSize * sizeof(*mapcache->torus.task2coords) +
                     fullSize * sizeof(*mapcache->torus.coords2task) +
                     peerSize * sizeof(*mapcache->node.local2peer) +
                     peerSize * sizeof(*mapcache->node.peer2task);
    TRACE_ERR( (stderr, "Global::initializeMapCache() << mapsize = %zu\n", mapsize));
    return mapsize;
  }

  volatile cacheAnchors_t *cacheAnchorsPtr;
  res = mm->memalign((void **)&cacheAnchorsPtr, 16, sizeof(*cacheAnchorsPtr),
                     "/pami-global-cacheAnchorsPtr");
  PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to get memory for cacheAnchorsPtr");

  TRACE_ERR( (stderr, "Global::initializeMapCache() .. mapcache = %p, size = %zu, cacheAnchorsPtr = %p, sizeof(cacheAnchors_t) = %zu, fullSize = %zu, peerSize = %zu\n", mapcache, mm->size(), cacheAnchorsPtr, sizeof(cacheAnchors_t), fullSize, peerSize));

  if(shared)
  {
    DUMP_HEXDATA("Shared memory", (const uint32_t *)cacheAnchorsPtr, 16);
  }

  // Notify all other tasks on the node that this task has entered the
  // map cache initialization function.  If the value returned is zero
  // then this task is the first one in and is designated the "master".
  // All other tasks will wait until the master completes the
  // initialization.
  uint64_t participant = Fetch_and_Add((uint64_t *) & (cacheAnchorsPtr->atomic.enter), 1); /// \todo this isn't working on mambo

  //myRank = personality.rank();

  TRACE_ERR( (stderr, "Global::initializeMapCache() .. participant=%ld\n", participant));

  res = mm->memalign((void **)&mapcache->torus.task2coords, 16, fullSize * sizeof(*mapcache->torus.task2coords),
                     "/pami-global-task2coords");
  PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to get memory for task2coords");
  res = mm->memalign((void **)&mapcache->torus.coords2task, 16, fullSize * sizeof(*mapcache->torus.coords2task),
                     "/pami-global-coords2task");
  PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to get memory for coords2task");
  TRACE_ERR( (stderr, "Global::initializeMapCache() .. mapcache->torus.task2coords = %p mapcache->torus.coords2task = %p\n", mapcache->torus.task2coords, mapcache->torus.coords2task));
  TRACE_ERR( (stderr, "Global::initializeMapCache() .. mapcache->node.local2peer = %p mapcache->torus.coords2task = %p\n", mapcache->node.local2peer, mapcache->torus.coords2task));

  res = mm->memalign((void **)&mapcache->node.local2peer, 16, peerSize * sizeof(*mapcache->node.local2peer),
                     "/pami-global-local2peer");
  PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to get memory for local2peer");
  res = mm->memalign((void **)&mapcache->node.peer2task, 16, peerSize * sizeof(*mapcache->node.peer2task),
                     "/pami-global-peer2task");
  PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to get memory for peer2task");
  TRACE_ERR( (stderr, "Global::initializeMapCache() .. mapcache->node.local2peer = %p mapcache->node.peer2task = %p\n", mapcache->node.local2peer, mapcache->node.peer2task));

  pami_task_t max_rank = 0, min_rank = (pami_task_t) - 1;
  pami_coord_t _ll, _ur;

  /* Initialize for rget pacing analysis as the mapcache is being constructed below.
   * The number of nodes in the block (first parameter) is used to determine whether
   * or not to pace.
   */
  bool   doRgetPacing;
  size_t rgetPacingHops;
  size_t rgetPacingDims;
  initializeRgetPacing( aSize * bSize * cSize * dSize * eSize,
                        doRgetPacing, rgetPacingHops, rgetPacingDims, _rgetPacingSize );

  // If we are the master (participant 0), then initialize the caches.
  // Then, set the cache pointers into the shared memory area for the other
  // ranks on this node to see, and wait for them to see it.
  pami_task_t *master_ranks; /* A list of master ranks (first rank (t=0) on each node) */

  if((participant == 0) || !shared)
  {
    char *envopts;
    envopts = getenv("PAMI_MASTER_RANK_LIST_IN_HEAP");
    if(envopts) cacheAnchorsPtr->isMasterRanksShmem = 0;
    else cacheAnchorsPtr->isMasterRanksShmem = 1;
    // Allocate storage for an array to be used in the loop below to track
    // the number of physical nodes in the partition.  The loop goes through
    // each rank, gets that rank's physical node coordinates, and sets a bit
    // in the array corresponding to that node, indicating that there is
    // a rank on that node.  The loop monitors the 0 to 1 transition of a
    // bit, and increments numActiveNodesGlobal when it sees the first
    // rank on the node.  After the loop, the storage for the array is
    // freed.
    uint64_t numNodes   = aSize * bSize * cSize * dSize * eSize;
    // Calculate number of array slots needed...
    uint64_t narraySize = (numNodes + 63) >> 6; // Divide by 64 bits.
    uint64_t *narray;
    res = heap_mm->memalign((void **)&narray, 0, narraySize * sizeof(uint64_t));
    PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to alloc narray");
    memset(narray, 0, narraySize*sizeof(uint64_t));

    // Initialize the task and peer mappings to -1 (== "not mapped")
    memset (mapcache->torus.coords2task, (uint32_t) - 1, sizeof(uint32_t) * fullSize);
    memset (mapcache->node.peer2task, (uint8_t) - 1, sizeof(size_t) * peerSize);

    size_t a = 0;
    size_t b = 0;
    size_t c = 0;
    size_t d = 0;
    size_t e = 0;
    size_t t = 0;

    /* Fill in the _mapcache array in a single syscall.
     * It is indexed by rank, dimensioned to be the full size of the
     * partition (ignoring -np), and filled in with the xyzt
     * coordinates of each rank packed into a single 4 byte int.
     * Non-active ranks (-np) have x, y, z, and t equal to 255, such
     * that the entire 4 byte int is -1.
     */
    int rc = 0;

    uint64_t numentries;

    rc = Kernel_RanksToCoords(fullSize * sizeof(*mapcache->torus.task2coords), (BG_CoordinateMapping_t *)mapcache->torus.task2coords, &numentries);
    TRACE_ERR( (stderr, "Global::initializeMapCache() .. fullSize = %zu, numentries %zu\n", fullSize, numentries));

    //BG_CoordinateMapping_t* map = (BG_CoordinateMapping_t *)mapcache->torus.task2coords;

    //for (uint64_t x = 0; x < numentries; x++) fprintf(stderr, "Global::initializeMapCache() index=%zu: a=%d, b=%d, c=%d, d=%d, e=%d, t=%d\n", x, map[x].a, map[x].b, map[x].c, map[x].d, map[x].e, map[x].t);

    // If the syscall works, obtain info from the returned _mapcache.
    if(rc == 0)
    {
      /* Obtain the following information from the _mapcache:
       * 1. Number of active ranks in the partition.
       * 2. Number of active compute nodes in the partition.
       * 3. _rankcache (the reverse of _mapcache).  It is indexed by
       *    coordinates and contains the rank.
       * 4. Number of active ranks on each compute node.
       * 5. Lowest T coordinate on our node.
       */
      size_t i;

      _ll.network = _ur.network = PAMI_N_TORUS_NETWORK;
      _ll.u.n_torus.coords[0] = _ur.u.n_torus.coords[0] = personality.aCoord();
      _ll.u.n_torus.coords[1] = _ur.u.n_torus.coords[1] = personality.bCoord();
      _ll.u.n_torus.coords[2] = _ur.u.n_torus.coords[2] = personality.cCoord();
      _ll.u.n_torus.coords[3] = _ur.u.n_torus.coords[3] = personality.dCoord();
      _ll.u.n_torus.coords[4] = _ur.u.n_torus.coords[4] = personality.eCoord();
      _ll.u.n_torus.coords[5] = _ur.u.n_torus.coords[5] = tCoord;

      size_t numActiveRanksGlobal = numentries;
      size_t numActiveNodesGlobal = 0;
      size_t lowestTCoordOnMyNode = 999999999;

      max_rank = numentries - 1;
      min_rank = 0;
      res = heap_mm->memalign((void **)&narray, 0, narraySize * sizeof(uint64_t));
      PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to alloc narray");

      if(numNodes > numentries) numNodes = numentries; /* estimated max number of nodes */
      if(numNodes > _shmemThreshold) cacheAnchorsPtr->isMasterRanksShmem = 0; // Exceed some threshold for BG_SHMEMSIZE?  Use heap
      size_t idxMasterRanks = 0;
      if(cacheAnchorsPtr->isMasterRanksShmem)
        res = mm->memalign((void **)&master_ranks, 16, numNodes * sizeof(pami_task_t),"/pami-global-masterranks");/* Shared master list */
      else
        res = heap_mm->memalign((void **)&master_ranks, 16, numNodes * sizeof(pami_task_t));/* My master list */
      PAMI_assert_alwaysf(res == PAMI_SUCCESS, "Failed to allocate temporary master rank array");

      for(i = 0; i < numentries; i++)
      {
        a = mapcache->torus.task2coords[i].mapped.a;
        b = mapcache->torus.task2coords[i].mapped.b;
        c = mapcache->torus.task2coords[i].mapped.c;
        d = mapcache->torus.task2coords[i].mapped.d;
        e = mapcache->torus.task2coords[i].mapped.e;
        t = mapcache->torus.task2coords[i].mapped.t;
        TRACE_ERR( (stderr, "Global::initializeMapCache() task = %zu, estimated task = %zu, coords{%zu,%zu,%zu,%zu,%zu,%zu}\n", i, ESTIMATED_TASK(a, b, c, d, e, t, aSize, bSize, cSize, dSize, eSize, tSize), a, b, c, d, e, t));
        if(t==0)
        {
            master_ranks[idxMasterRanks++] = i;
        }

        // If this task is local to our node
        // 1. Track the lowest T coord on our node
        // 2. Track whether this task is local to our node.  This is stored in
        //    the "reserved" bit of the coords structure.
        //
        if( (a==aCoord) && (b==bCoord) && (c==cCoord) && (d==dCoord) && (e==eCoord) )
        {
          if( t < lowestTCoordOnMyNode )
            lowestTCoordOnMyNode = t;

          mapcache->torus.task2coords[i].mapped.reserved = 1;
        }
        else
        {
          // The task is not local to our node.

          // Determine whether remote gets to this task should be paced.
          // If so, set the 2nd highest bit of the D coordinate.
          if( paceRgets( doRgetPacing, rgetPacingHops, rgetPacingDims, 
                         aCoord, bCoord, cCoord, dCoord, a, b, c, d, aSize, bSize, cSize, dSize ) )
          {
            TRACE_ERR((stderr,"Global::initializeMapCache(): Pacing rgets is true to this dest\n"));
            mapcache->torus.task2coords[i].raw |= 0x00000400;
          }

          // Determine the routing to use between our task and the dest task.
          unsigned int routing;
          routing = getFlexability ( aCoord,bCoord,cCoord,dCoord,
                                     a,b,c,d,
                                     aSize,bSize,cSize,dSize );
          if( routing )
            mapcache->torus.task2coords[i].raw |= 0x00010000;
          TRACE_ERR((stderr,"Routing based on flex = %u\n",routing));
        }

        // Set the bit corresponding to the physical node of this rank,
        // indicating that we have found a rank on that node.
        // Increment numActiveNodesGlobal when the bit goes from 0 to 1.
        uint64_t tmpIndex      = ESTIMATED_TASK(a, b, c, d, e, 0, aSize, bSize, cSize, dSize, eSize, tSize) / tSize;
        uint64_t narrayIndex   = tmpIndex >> 6;     // Divide by 64 to get narray index.
        uint64_t bitNumber     = tmpIndex & (64 - 1); // Mask off high bits to get bit number.
        uint64_t bitNumberMask = _BN(bitNumber);

        if((narray[narrayIndex] & bitNumberMask) == 0)
        {
          numActiveNodesGlobal++;
          narray[narrayIndex] |= bitNumberMask;
          TRACE_ERR( (stderr, "Global::initializeMapCache() bitNumberMask = %#.16lX, narray[%#.16lX]=%#.16lX\n", bitNumberMask, narrayIndex, narray[narrayIndex]));
        }

        uint32_t addr_hash = ESTIMATED_TASK(a, b, c, d, e, t, aSize, bSize, cSize, dSize, eSize, tSize);
        mapcache->torus.coords2task[addr_hash] = i;
        TRACE_ERR((stderr, "Global::initializeMapCache() .. coords2task[%d]=%#lX\n", addr_hash, i));


        if(a < _ll.u.n_torus.coords[0]) _ll.u.n_torus.coords[0] = a;

        if(b < _ll.u.n_torus.coords[1]) _ll.u.n_torus.coords[1] = b;

        if(c < _ll.u.n_torus.coords[2]) _ll.u.n_torus.coords[2] = c;

        if(d < _ll.u.n_torus.coords[3]) _ll.u.n_torus.coords[3] = d;

        if(e < _ll.u.n_torus.coords[4]) _ll.u.n_torus.coords[4] = e;

        if(t < _ll.u.n_torus.coords[5]) _ll.u.n_torus.coords[5] = t;

        if(a > _ur.u.n_torus.coords[0]) _ur.u.n_torus.coords[0] = a;

        if(b > _ur.u.n_torus.coords[1]) _ur.u.n_torus.coords[1] = b;

        if(c > _ur.u.n_torus.coords[2]) _ur.u.n_torus.coords[2] = c;

        if(d > _ur.u.n_torus.coords[3]) _ur.u.n_torus.coords[3] = d;

        if(e > _ur.u.n_torus.coords[4]) _ur.u.n_torus.coords[4] = e;

        if(t > _ur.u.n_torus.coords[5]) _ur.u.n_torus.coords[5] = t;

      }
      PAMI_assertf( numActiveNodesGlobal >= idxMasterRanks, "numActiveNodesGlobal %zu, idxMasterRanks %zu",numActiveNodesGlobal, idxMasterRanks);

      // Record information in the common cacheAnchors structure.
      cacheAnchorsPtr->numActiveRanksGlobal = numActiveRanksGlobal;
      cacheAnchorsPtr->numActiveNodesGlobal = numActiveNodesGlobal;
      cacheAnchorsPtr->lowestTCoordOnMyNode = lowestTCoordOnMyNode;
      cacheAnchorsPtr->idxMasterRanks       = idxMasterRanks; 
      cacheAnchorsPtr->master_ranks         = master_ranks;

      heap_mm->free(narray);
      narray = NULL;
      cacheAnchorsPtr->maxRank = max_rank;
      cacheAnchorsPtr->minRank = min_rank;
      memcpy((void *)&cacheAnchorsPtr->activeLLCorner, &_ll, sizeof(_ll));
      memcpy((void *)&cacheAnchorsPtr->activeURCorner, &_ur, sizeof(_ur));
      TRACE_ERR((stderr, "Global::initializeMapCache() numActiveRanksGlobal %zu,numActiveNodesGlobal %zu,idxMasterRanks %zu, max_rank %u, min_rank %u, lowestTCoordOnMyNode %zu\n", numActiveRanksGlobal, numActiveNodesGlobal, idxMasterRanks, max_rank, min_rank, lowestTCoordOnMyNode ));
    }
    else PAMI_abortf("Kernel_RanksToCoords(%zd, %p, %p) rc = %d\n", fullSize * sizeof(*mapcache->torus.task2coords), mapcache->torus.task2coords, &numentries, rc);

    // Initialize the node task2peer and peer2task caches.
    uint32_t hash;
    size_t peer = 0;
    size_t numRanks = 0;

    for(t = 0; t < tSize; t++)
    {
      hash = ESTIMATED_TASK(aCoord, bCoord, cCoord, dCoord, eCoord, t, 
                            aSize, bSize, cSize, dSize, eSize, tSize);

      // Extract the task for this t.
      // If there is actually a task here, process it, otherwise ignore it.
      // This can happen if --np is less than the block size.

      uint32_t task = mapcache->torus.coords2task[hash];
      if( task != (uint32_t) -1 )
      {
        mapcache->node.peer2task[peer] = task;

        hash = ESTIMATED_TASK(0, 0, 0, 0, 0, t, 1, 1, 1, 1, 1, tSize);
        mapcache->node.local2peer[hash] = peer++;

        numRanks++; // increment local variable

        TRACE_ERR((stderr, "Global::initializeMapCache() .. t=%zu, peer2task[%zu]=%zu, local2peer[%d]=%zu\n", t, peer - 1, mapcache->node.peer2task[peer-1], hash, peer - 1));
      }
      else
        TRACE_ERR((stderr, "Global::initializeMapCache() .. No task at t=%zu, hash=%u, coords2task=0x%08x\n",t,hash,task));
    }

    cacheAnchorsPtr->numActiveRanksLocal = numRanks; // update global from local variable

    mbar();  // Ensure that stores to memory are in the memory.

    // Notify the other processes on this node that the master has
    // completed the initialization.
    Fetch_and_Add ((uint64_t *)&(cacheAnchorsPtr->atomic.exit), 1);

    memcpy((void *)&ll, &_ll, sizeof(ll));
    memcpy((void *)&ur, &_ur, sizeof(ur));

  } // End: Allocate an initialize the map and rank caches.
  else
  { // We are not the master t on our physical node.  Wait for the master t to
    // initialize the caches.  Then grab what we need
    while(cacheAnchorsPtr->atomic.exit == 0);
    if(!cacheAnchorsPtr->isMasterRanksShmem) // Did the master NOT build it in shmem?
    { // Then create our own list of master ranks in heap
      size_t i, t = 0, idxMasterRanks = 0;
      uint64_t numNodes   = cacheAnchorsPtr->idxMasterRanks;
      uint64_t numentries = cacheAnchorsPtr->numActiveRanksGlobal;
      res = heap_mm->memalign((void **)&master_ranks, 16, numNodes * sizeof(pami_task_t));/* My master list */
      for(i = 0; i < numentries; i++)
      {
        t = mapcache->torus.task2coords[i].mapped.t;
        TRACE_ERR( (stderr, "Global::initializeMapCache() task = %zu, estimated task = %zu, coords{%zu,%zu,%zu,%zu,%zu,%zu}\n", i, ESTIMATED_TASK(a, b, c, d, e, t, aSize, bSize, cSize, dSize, eSize, tSize), a, b, c, d, e, t));
        if(t==0)
        {
          master_ranks[idxMasterRanks++] = i;
        }
      }
      PAMI_assertf( idxMasterRanks == cacheAnchorsPtr->idxMasterRanks, "idxMasterRanks %zu, cached idxMasterRanks %zu",idxMasterRanks,cacheAnchorsPtr->idxMasterRanks);
    }
    else // Get it from shmem
       master_ranks = cacheAnchorsPtr->master_ranks;


    max_rank = cacheAnchorsPtr->maxRank;
    min_rank = cacheAnchorsPtr->minRank;

    memcpy(&ll, (void *)&cacheAnchorsPtr->activeLLCorner, sizeof(ll));
    memcpy(&ur, (void *)&cacheAnchorsPtr->activeURCorner, sizeof(ur));

  }

  new (&topology_master) Topology(master_ranks, cacheAnchorsPtr->idxMasterRanks, 0, 1);
  
  min = min_rank;
  max = max_rank;

  mapcache->size = cacheAnchorsPtr->numActiveRanksGlobal;
  mapcache->local_size = cacheAnchorsPtr->numActiveRanksLocal;
  mapcache->lowestTCoordOnMyNode = cacheAnchorsPtr->lowestTCoordOnMyNode;
  mapcache->numActiveNodesGlobal= cacheAnchorsPtr->numActiveNodesGlobal;

  TRACE_ERR( (stderr, "Global::initializeMapCache() .. size: %zu local_size:%zu lowestTCoordOnMyNode %zu\n", mapcache->size, mapcache->local_size,mapcache->lowestTCoordOnMyNode ));

  return 0;
};

inline void globalDumpHexData(const char * pstring, const uint32_t *buffer, size_t n_ints)
{
  fprintf(stderr, "dumphex:%s:%p/%p:%zu:\n\n", pstring, &buffer, buffer, sizeof(unsigned)*n_ints);
  unsigned nChunks = n_ints / 8;

  if(!buffer || !n_ints) return;

  for(unsigned i = 0; i < nChunks; i++)
  {
    fprintf(stderr,
            "<%p>: %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X \n",
            buffer + (i*8),
            *(buffer + (i*8) + 0),
            *(buffer + (i*8) + 1),
            *(buffer + (i*8) + 2),
            *(buffer + (i*8) + 3),
            *(buffer + (i*8) + 4),
            *(buffer + (i*8) + 5),
            *(buffer + (i*8) + 6),
            *(buffer + (i*8) + 7)
           );
  }

  if(n_ints % 8)
  {
    unsigned lastChunk = nChunks * 8;
    fprintf(stderr,
            "<%p>: %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X \n",
            buffer + lastChunk,
            lastChunk + 0 < n_ints ? *(buffer + lastChunk + 0) : 0xDEADDEAD,
            lastChunk + 1 < n_ints ? *(buffer + lastChunk + 1) : 0xDEADDEAD,
            lastChunk + 2 < n_ints ? *(buffer + lastChunk + 2) : 0xDEADDEAD,
            lastChunk + 3 < n_ints ? *(buffer + lastChunk + 3) : 0xDEADDEAD,
            lastChunk + 4 < n_ints ? *(buffer + lastChunk + 4) : 0xDEADDEAD,
            lastChunk + 5 < n_ints ? *(buffer + lastChunk + 5) : 0xDEADDEAD,
            lastChunk + 6 < n_ints ? *(buffer + lastChunk + 6) : 0xDEADDEAD,
            lastChunk + 7 < n_ints ? *(buffer + lastChunk + 7) : 0xDEADDEAD
           );
    lastChunk = 0; // gets rid of an annoying warning when not tracing the buffer
  }
}



extern PAMI::Global __global;
#undef TRACE_ERR
#endif // __pami_components_sysdep_bgq_bgqglobal_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
