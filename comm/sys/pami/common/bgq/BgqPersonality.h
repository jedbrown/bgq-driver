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
#ifndef __common_bgq_BgqPersonality_h__
#define __common_bgq_BgqPersonality_h__
///
///  \file common/bgq/BgqPersonality.h
///  \brief Blue Gene/Q Personality Object
///
///  This object currently provides the following features:
///  - Access to coordinate information
///  - Access to partition size information
///  - Access to operating modes (vnm, etc)
///  - Access to pset information
///  - Ability to dump the entire personality
///
///  Definitions:
///  - Personality is an object that returns machine specific hardware info
///

#undef MU_CR_DEBUG

#define PERS_SIZE 1024

#include <sys/types.h>
#include <util/common.h>

#include <unistd.h>
#include <stdlib.h>

#include <firmware/include/personality.h>
#include <hwi/include/common/uci.h>
#include <kernel/location.h>
#include <kernel/process.h>

#undef TRACE_ERR
#define TRACE_ERR(x) //fprintf x

#ifdef ENABLE_MAMBO_WORKAROUNDS
#include <hwi/include/bqc/nd_500_dcr.h>
#endif

namespace PAMI
{
  class BgqJobPersonality : private Personality_t
  {
    public:

      BgqJobPersonality ()
      {

        Personality_t * p = this;

#ifdef ENABLE_MAMBO_WORKAROUNDS

        Kernel_GetPersonality(p, sizeof(Personality_t));

        _is_mambo = false; // Indicates whether mambo is being used

        uint64_t p1 = Kernel_Config.NodeConfig & PERS_ENABLE_Mambo;

        if(p1) _is_mambo = true;

        TRACE_ERR((stderr, "BGQPersonality Kernel_Config.NodeConfig %#llX\n", (unsigned long long)(Kernel_Config.NodeConfig)));
        TRACE_ERR((stderr, "BGQPersonality Kernel_Config.TraceConfig %#llX\n", (unsigned long long)(Kernel_Config.TraceConfig)));
        TRACE_ERR((stderr, "BGQPersonality Network_Config.MuFlags %#llX\n", (unsigned long long)(Network_Config.MuFlags)));
        TRACE_ERR((stderr, "BGQPersonality Network_Config.NetFlags %#llX\n", (unsigned long long)(Network_Config.NetFlags)));

        TRACE_ERR((stderr, "BGQPersonality is mambo enabled: %s\n", _is_mambo ? "true" : "false"));

        TRACE_ERR((stderr, "Network_Config Coord A = %#x\n", Network_Config.Acoord));
        TRACE_ERR((stderr, "Network_Config Coord B = %#x\n", Network_Config.Bcoord));
        TRACE_ERR((stderr, "Network_Config Coord C = %#x\n", Network_Config.Ccoord));
        TRACE_ERR((stderr, "Network_Config Coord D = %#x\n", Network_Config.Dcoord));
        TRACE_ERR((stderr, "Network_Config Coord E = %#x\n", Network_Config.Ecoord));

        TRACE_ERR((stderr, "Network_Config Nodes A = %#x\n", Network_Config.Anodes));
        TRACE_ERR((stderr, "Network_Config Nodes B = %#x\n", Network_Config.Bnodes));
        TRACE_ERR((stderr, "Network_Config Nodes C = %#x\n", Network_Config.Cnodes));
        TRACE_ERR((stderr, "Network_Config Nodes D = %#x\n", Network_Config.Dnodes));
        TRACE_ERR((stderr, "Network_Config Nodes E = %#x\n", Network_Config.Enodes));

        if (_is_mambo)
          fprintf(stderr, "Running mambo circumventions\n");

#else // no mambo workarounds..

        if (Kernel_GetPersonality(p, sizeof(Personality_t)) != 0)
          PAMI_abortf("%s<%d>\n", __FILE__, __LINE__);

#endif

        _tSize = Kernel_ProcessCount();

        // Set the t coord
        _tCoord =  Kernel_MyTcoord();

        TRACE_ERR((stderr, "BGQPersonality() tid %zu, t %zu, core %zu, thread %zu, tSize %zu\n", tid(),  tCoord(),  core(), thread(), tSize()));

        _blkTorus[0] = (bool) (ND_ENABLE_TORUS_DIM_A & Network_Config.NetFlags);
        _blkTorus[1] = (bool) (ND_ENABLE_TORUS_DIM_B & Network_Config.NetFlags);
        _blkTorus[2] = (bool) (ND_ENABLE_TORUS_DIM_C & Network_Config.NetFlags);
        _blkTorus[3] = (bool) (ND_ENABLE_TORUS_DIM_D & Network_Config.NetFlags);
        _blkTorus[4] = (bool) (ND_ENABLE_TORUS_DIM_E & Network_Config.NetFlags);
	_blkTorus[5] = true; // correct?

        TRACE_ERR((stderr, "BGQPersonality() _blkTorus[0] %d, _blkTorus[1] %d, _blkTorus[2] %d, _blkTorus[3] %d, _blkTorus[4] %d\n", _blkTorus[0], _blkTorus[1], _blkTorus[2], _blkTorus[3], _blkTorus[4]));

  init_coord(_blkLL, 0,0,0,0,0,0 );
	init_coord(_blkUR,
		Network_Config.Anodes - 1,
		Network_Config.Bnodes - 1,
		Network_Config.Cnodes - 1,
		Network_Config.Dnodes - 1,
		Network_Config.Enodes - 1,
		_tSize - 1
	);
	init_coord(_blkCoord,
		Network_Config.Acoord,
		Network_Config.Bcoord,
		Network_Config.Ccoord,
		Network_Config.Dcoord,
		Network_Config.Ecoord,
		_tCoord
	);

	BG_JobCoords_t subblk;
	uint32_t krc;
	krc = Kernel_JobCoords(&subblk);
	PAMI_assertf(krc == 0, "Kernel_JobCoords failed");
	_isSubBlockJob = subblk.isSubBlock;
	if (_isSubBlockJob) {
		PAMI_assertf(subblk.shape.core == 16, "Sub-node jobs not supported");
		init_coord(_jobLL,
			subblk.corner.a,
			subblk.corner.b,
			subblk.corner.c,
			subblk.corner.d,
			subblk.corner.e,
			0
		);
		init_coord(_jobUR,
			subblk.corner.a + subblk.shape.a - 1,
			subblk.corner.b + subblk.shape.b - 1,
			subblk.corner.c + subblk.shape.c - 1,
			subblk.corner.d + subblk.shape.d - 1,
			subblk.corner.e + subblk.shape.e - 1,
			_tSize - 1
		);
		// don't (yet) know how many actual dimensions to use, so use all.
		// any undefined values will be ignored later.
		int x;
		for (x = 0; x < PAMI_MAX_DIMS; ++x) {
			_jobTorus[x] = (_blkTorus[x] &&
				_jobLL.u.n_torus.coords[x] == _blkLL.u.n_torus.coords[x] &&
				_jobUR.u.n_torus.coords[x] == _blkUR.u.n_torus.coords[x]);
		}
		init_coord(_jobCoord,
			Network_Config.Acoord - _jobLL.u.n_torus.coords[0],
			Network_Config.Bcoord - _jobLL.u.n_torus.coords[1],
			Network_Config.Ccoord - _jobLL.u.n_torus.coords[2],
			Network_Config.Dcoord - _jobLL.u.n_torus.coords[3],
			Network_Config.Ecoord - _jobLL.u.n_torus.coords[4],
			_tCoord
		);
		init_coord(_jobSize,
			subblk.shape.a,
			subblk.shape.b,
			subblk.shape.c,
			subblk.shape.d,
			subblk.shape.e,
			_tSize
		);
	} else {
		_jobLL = _blkLL;
		_jobUR = _blkUR;
		memcpy(&_jobTorus, &_blkTorus, sizeof(_jobTorus));
		_jobCoord = _blkCoord;
		init_coord(_jobSize,
			Network_Config.Anodes,
			Network_Config.Bnodes,
			Network_Config.Cnodes,
			Network_Config.Dnodes,
			Network_Config.Enodes,
			_tSize
		);
	}
#ifdef MU_CR_DEBUG
fprintf(stderr, "Block (%zd,%zd,%zd,%zd,%zd):(%zd,%zd,%zd,%zd,%zd) [%d,%d,%d,%d,%d]\n",
_blkLL.u.n_torus.coords[0],
_blkLL.u.n_torus.coords[1],
_blkLL.u.n_torus.coords[2],
_blkLL.u.n_torus.coords[3],
_blkLL.u.n_torus.coords[4],
_blkUR.u.n_torus.coords[0],
_blkUR.u.n_torus.coords[1],
_blkUR.u.n_torus.coords[2],
_blkUR.u.n_torus.coords[3],
_blkUR.u.n_torus.coords[4],
_blkTorus[0],
_blkTorus[1],
_blkTorus[2],
_blkTorus[3],
_blkTorus[4]);
fprintf(stderr, "Job   (%zd,%zd,%zd,%zd,%zd):(%zd,%zd,%zd,%zd,%zd) [%d,%d,%d,%d,%d]\n",
_jobLL.u.n_torus.coords[0],
_jobLL.u.n_torus.coords[1],
_jobLL.u.n_torus.coords[2],
_jobLL.u.n_torus.coords[3],
_jobLL.u.n_torus.coords[4],
_jobUR.u.n_torus.coords[0],
_jobUR.u.n_torus.coords[1],
_jobUR.u.n_torus.coords[2],
_jobUR.u.n_torus.coords[3],
_jobUR.u.n_torus.coords[4],
_jobTorus[0],
_jobTorus[1],
_jobTorus[2],
_jobTorus[3],
_jobTorus[4]);
#endif // MU_CR_DEBUG
      };

      void location (char location[])
      {
        bg_uci_toString (Kernel_Config.UCI, location);
      };

      void dumpPersonality ()
      {
      };

      bool isSubBlockJob() { return _isSubBlockJob; }

      pami_result_t jobRectangle(pami_coord_t &ll, pami_coord_t &ur) {
	memcpy(&ll, &_jobLL, sizeof(ll));
	memcpy(&ur, &_jobUR, sizeof(ur));
	return PAMI_SUCCESS;
      }

      pami_result_t jobCoord(pami_coord_t &me) {
	me = _jobCoord;
	return PAMI_SUCCESS;
      }

      pami_result_t jobTorus(bool torus[PAMI_MAX_DIMS]) {
	memcpy(&torus, &_jobTorus, sizeof(torus));
	return PAMI_SUCCESS;
      }

      pami_result_t blockRectangle(pami_coord_t &ll, pami_coord_t &ur) {
	memcpy(&ll, &_blkLL, sizeof(ll));
	memcpy(&ur, &_blkUR, sizeof(ur));
	return PAMI_SUCCESS;
      }

      pami_result_t blockCoord(pami_coord_t &me) {
	me = _blkCoord;
	return PAMI_SUCCESS;
      }

      pami_result_t blockTorus(bool torus[PAMI_MAX_DIMS]) {
	memcpy(&torus, &_blkTorus, sizeof(torus));
	return PAMI_SUCCESS;
      }

      ///
      /// \brief Retrieves the 'A' coordinate of the node
      /// relative to the lower-left corner of the job.
      ///
      size_t aCoord() const
      {
        return _jobCoord.u.n_torus.coords[0];
      }

      ///
      /// \brief Retrieves the 'B' coordinate of the node
      /// relative to the lower-left corner of the job.
      ///
      size_t bCoord() const
      {
        return _jobCoord.u.n_torus.coords[1];
      }

      ///
      /// \brief Retrieves the 'C' coordinate of the node
      /// relative to the lower-left corner of the job.
      ///
      size_t cCoord() const
      {
        return _jobCoord.u.n_torus.coords[2];
      }

      ///
      /// \brief Retrieves the 'D' coordinate of the node
      /// relative to the lower-left corner of the job.
      ///
      size_t dCoord() const
      {
        return _jobCoord.u.n_torus.coords[3];
      }

      ///
      /// \brief Retrieves the 'E' coordinate of the node
      /// relative to the lower-left corner of the job.
      ///
      size_t eCoord() const
      {
        return _jobCoord.u.n_torus.coords[4];
      }

      ///
      /// \brief Retrieves the core id of the node
      ///
      ///
      size_t core() const
      {
        return Kernel_PhysicalProcessorID();
      }

      ///
      /// \brief Retrieves the 'T' coordinate of the node
      ///
      /// a.k.a. the thread id on the core starting at 0 and
      /// incrementing to tSize.  Not the same as hwThread!
      ///
      size_t tCoord() const
      {
        return _tCoord;
      }

      ///
      /// \brief Retrieves the 'hardware thread id' on the core
      ///
      size_t thread() const
      {
        return Kernel_ProcessorID();
      }

      ///
      /// \brief Retrieves the 'thread id' on the node
      ///
      size_t tid() const
      {
        return Kernel_PhysicalHWThreadID();
      }

      ///
      /// \brief Retrieves the size of the 'A' dimension in job.
      /// \note  Does not consider the mapping.
      ///
      size_t aSize()  const
      {
        return _jobSize.u.n_torus.coords[0];
      }

      ///
      /// \brief Retrieves the size of the 'B' dimension in job.
      /// \note  Does not consider the mapping.
      ///
      size_t bSize()  const
      {
        return _jobSize.u.n_torus.coords[1];
      }

      ///
      /// \brief Retrieves the size of the 'C' dimension in job.
      /// \note  Does not consider the mapping.
      ///
      size_t cSize()  const
      {
        return _jobSize.u.n_torus.coords[2];
      }

      ///
      /// \brief Retrieves the size of the 'D' dimension in job.
      /// \note  Does not consider the mapping.
      ///
      size_t dSize()  const
      {
        return _jobSize.u.n_torus.coords[3];
      }

      ///
      /// \brief Retrieves the size of the 'E' dimension in job.
      /// \note  Does not consider the mapping.
      ///
      size_t eSize()  const
      {
        return _jobSize.u.n_torus.coords[4];
      }

      ///
      /// \brief Retrieves the size of the 'T' dimension.
      ///
      /// The 'T' coordinate identifies the process
      /// starting at 0 and incrementing to BG_PROCESSESPERNODE.
      ///
      size_t tSize()  const
      {
        return _tSize;
      }

      ///
      /// \brief Retrieves the torus vs mesh status of the 'A' dimension in job.
      ///
      bool isTorusA()  const
      {
        return _jobTorus[0];
      }

      ///
      /// \brief Retrieves the torus vs mesh status of the 'B' dimension in job.
      ///
      bool isTorusB()  const
      {
        return _jobTorus[1];
      }

      ///
      /// \brief Retrieves the torus vs mesh status of the 'C' dimension in job.
      ///
      bool isTorusC()  const
      {
        return _jobTorus[2];
      }

      ///
      /// \brief Retrieves the torus vs mesh status of the 'D' dimension in job.
      ///
      bool isTorusD()  const
      {
        return _jobTorus[3];
      }

      ///
      /// \brief Retrieves the torus vs mesh status of the 'E' dimension in job.
      ///
      bool isTorusE()  const
      {
        return _jobTorus[4];
      }

      ///
      /// \brief Retrieves the max number of cores, not the number active
      ///
      size_t maxCores() const
      {
        return NUM_CORES;
      }

      ///
      /// \brief Retrieves the max number of threads, not the number active
      ///
      size_t maxThreads() const
      {
        return NUM_SMT;
      }

      ///
      /// \brief Get the size of NODE memory
      /// \return _node_ memory size in MiB
      ///
      size_t  memSize()  const
      {
        return DDR_Config.DDRSizeMB;
      }

      ///
      /// \brief Gets the clock speed in MEGA-Hz
      /// \return MHz
      ///
      size_t  clockMHz()  const
      {
        return Kernel_Config.FreqMHz;
      }

    protected:

      void init_coord (pami_coord_t & coord,
                       size_t a,
                       size_t b,
                       size_t c,
                       size_t d,
                       size_t e,
                       size_t t)
      {
        coord.network = PAMI_N_TORUS_NETWORK;
        coord.u.n_torus.coords[0] = a;
        coord.u.n_torus.coords[1] = b;
        coord.u.n_torus.coords[2] = c;
        coord.u.n_torus.coords[3] = d;
        coord.u.n_torus.coords[4] = e;
        coord.u.n_torus.coords[5] = t;
      }

      size_t _tCoord;
      size_t _tSize;

      bool _isSubBlockJob;
      bool _blkTorus[PAMI_MAX_DIMS];
      bool _jobTorus[PAMI_MAX_DIMS];
      pami_coord_t _blkLL;
      pami_coord_t _blkUR;
      pami_coord_t _blkCoord;
      pami_coord_t _jobLL;
      pami_coord_t _jobUR;
      pami_coord_t _jobCoord;
      pami_coord_t _jobSize;
#ifdef ENABLE_MAMBO_WORKAROUNDS
    public:
      bool   _is_mambo; // Indicates whether mambo is being used
    protected:
#endif

  };  // class BgqJobPersonality
};  // namespace PAMI


#endif // __pami_components_sysdep_bgq_bgqpersonnality_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
