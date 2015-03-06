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
 * \file components/devices/bgp/collective_network/CNDevice.h
 * \brief Collective Network Device interface
 */
#ifndef __components_devices_bgp_collective_network_CNDevice_h__
#define __components_devices_bgp_collective_network_CNDevice_h__

#include "components/devices/bgp/collective_network/CNAllreduce.h"
#include "components/devices/bgp/collective_network/CollectiveNetworkLib.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/util/SubDeviceSuppt.h"

/**
 * \page env_vars Environment Variables
 *
 * - PAMI_THREADED_CN -
 *	Bitmask indicating whether Send (1) and/or Recv (2) should
 *	use Comm (helper) Threads.  Note, Comm
 *	threads may not be used in all cases, it will depend on
 *	factors such as run mode, message size, partition size,
 *	data operand, etc.
 *      Possible values:
 *      - 0 - Neither Send nor Recv will use Comm Threads.
 *      - 1 - Only Send will use Comm Threads.
 *      - 2 - Only Recv will use Comm Threads.
 *      - 3 - Both Send and Recv will use Comm Threads.
 *      - Default is 3.
 *
 * - PAMI_PERSISTENT_ADVANCE -
 *	Number of cycles to persist in the advance loop waiting for
 *	a (the first) receive packet to arrive.
 *      - Default is a value computed from the partition size
 *        (Collective network depth).
 *
 * - PAMI_PERSIST_MAX -
 *	Upper limit on the number of cycles to persist in advance.
 *	This is only used when PAMI_PERSISTENT_ADVANCE is computed.
 *	- Default is 5000 cycles.
 *
 * - PAMI_PERSIST_MIN -
 *	Lower limit on the number of cycles to persist in advance.
 *	This is only used when PAMI_PERSISTENT_ADVANCE is computed.
 *	- Default is 1000 cycles.
 *
 * - PAMI_CN_DBLSUM_THRESH -
 *	Number of doubles at which to start using the 2-Pass algorithm.
 *      Special values:
 *      - -1 (minus 1) - Effectively disables the 2-Pass algorithm.
 *      - Default is 2 doubles.
 *
 * - PAMI_CN_HELPER_THRESH -
 *	Number of bytes (message size) at which to start using a
 *	helper thread. Ideally this value would be computed based
 *	on network depth and comm thread start-up time.
 *	- Default 16384 bytes.
 *
 * - PAMI_CN_VN_DEEP -
 *	Boolean indicating whether to use the "Deep" protocol
 *	for receiving a message in virtual node mode. Currently
 *	not used.
 *      Possible values:
 *      - 0 (false) - The "Deep" protocol is not used.
 *      - 1 (true)  - The "Deep" protocol is used.
 *      - Default is 1.
 */
extern int PAMI_THREADED_CN;
extern unsigned PAMI_PERSISTENT_ADVANCE;
extern unsigned PAMI_PERSIST_MAX;
extern unsigned PAMI_PERSIST_MIN;
extern unsigned PAMI_CN_DBLSUM_THRESH;
extern unsigned PAMI_CN_HELPER_THRESH;
extern int PAMI_CN_VN_DEEP;

// This is here to avoid recursive includes
extern "C" size_t _g_num_active_nodes;

namespace PAMI {
namespace Device {
namespace BGP {

class CNDevice : public PAMI::Device::Generic::CommonQueueSubDevice {
public:
        /**
         * \brief  A Collective Network device constructor
         *
         * \param[in] sd	SysDep object
         */
        CNDevice() :
        PAMI::Device::Generic::CommonQueueSubDevice(),
        _threadRoles(0)
        {
        }

        virtual ~CNDevice() {}

        /// \note This is required to make "C" programs link successfully with virtual destructors
        inline void operator delete(void * p) { PAMI_abort(); }

        /**
         * \brief Tree Device Initialization
         *
         * Typically called once, after construction.
         * Sets up various device parameters for use during
         * operation. This may include measuring the send-recv
         * timings for optimizing those parameters.
         *
         * All environment variables are sampled at this point.
         */
        pami_result_t init(PAMI::Memory::MemoryManager *mm, size_t client, size_t contextId, pami_context_t ctx);

        inline int getMaxThreads() { return _threadRoles; }

private:
        unsigned _threadRoles;
        unsigned __cn_times[2]; // measured cn depth, local and global
}; // class CNDevice

}; // namespace BGP
}; // namespace Device
}; // namespace PAMI

#endif /* __components_devices_bgp_cndevice_h__ */
