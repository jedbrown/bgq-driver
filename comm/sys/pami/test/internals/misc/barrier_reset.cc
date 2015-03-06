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
/// \file test/internals/misc/barrier_reset.cc
/// \brief ???
///

#include <stdio.h>
#include <pami.h>

#include "test/internals/multisend/multisync.h"

#include "components/devices/MultisyncModel.h"
#include "components/devices/FactoryInterface.h"
#include "components/devices/generic/Device.h"

#include "util/common.h"

#define BARRIER1_NAME	"local_barriered_shmemzero()"
#define BARRIER1_MM	(&__global.mm)
#define BARRIER1_ISLOCAL	1

class ShmemZeroDevice {
public:
	class Factory : public PAMI::Device::Interface::FactoryInterface<Factory,ShmemZeroDevice,PAMI::Device::Generic::Device> {
	public:
		static inline ShmemZeroDevice *generate_impl(size_t client, size_t num_ctx, PAMI::Memory::MemoryManager & mm, PAMI::Device::Generic::Device *devices);
		static inline pami_result_t init_impl(ShmemZeroDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices);
		static inline size_t advance_impl(ShmemZeroDevice *devs, size_t client, size_t context);
		static inline ShmemZeroDevice & getDevice_impl(ShmemZeroDevice *devs, size_t client, size_t context);

	};
	PAMI::Memory::MemoryManager *_mm;
	pami_context_t _ctx;
};

ShmemZeroDevice __dev;

inline ShmemZeroDevice *ShmemZeroDevice::Factory::generate_impl(size_t clientid, size_t nctx, PAMI::Memory::MemoryManager &mm, PAMI::Device::Generic::Device *devices) {
	return &__dev;
}

inline pami_result_t ShmemZeroDevice::Factory::init_impl(ShmemZeroDevice *devs, size_t client, size_t contextId, pami_client_t clt, pami_context_t ctx, PAMI::Memory::MemoryManager *mm, PAMI::Device::Generic::Device *devices) {
	__dev._mm = mm;
	__dev._ctx = ctx;
	return PAMI_SUCCESS;
}
inline size_t ShmemZeroDevice::Factory::advance_impl(ShmemZeroDevice *devs, size_t client, size_t context) {
	return 0;
}
inline ShmemZeroDevice &ShmemZeroDevice::Factory::getDevice_impl(ShmemZeroDevice *devs, size_t client, size_t context) {
	return __dev;
}

template <int T_Size>
class ShmemZeroModel {
public:
	static const size_t sizeof_msg = 8;
	static const size_t sizeof_shm = T_Size;

	ShmemZeroModel(ShmemZeroDevice &dev, pami_result_t &status) {
		_shmem = NULL;
		char mmkey[PAMI::Memory::MMKEYSIZE];
		sprintf(mmkey, "/ShmemZeroModel-%zd", sizeof_shm);
		status = dev._mm->memalign(&_shmem, sizeof(void *), sizeof_shm, mmkey);
		PAMI_assertf(_shmem, "Failed to allocate %zu bytes Shmem\n", sizeof_shm);
	}

	inline pami_result_t postMultisync(uint8_t (&state)[sizeof_msg],
					   size_t                 client,
					   size_t                 context, 
						pami_multisync_t *msync) {
		pami_task_t me = __global.mapping.task();
		PAMI::Topology *topo = (PAMI::Topology *)msync->participants;
		local_barriered_shmemzero(_shmem, sizeof_shm, topo->size(),
						(topo->index2Rank(0) == me));
		if (msync->cb_done.function) {
			msync->cb_done.function(__dev._ctx, msync->cb_done.clientdata, PAMI_SUCCESS);
		}
		return PAMI_SUCCESS;
	}

	void *_shmem;
};

#define MEMMGR_SIZE	(128*1024)

int main(int argc, char ** argv) {
        pami_context_t context;
        pami_task_t task_id;
        size_t num_tasks;

#if 0
        pami_client_t client;
        pami_result_t status = PAMI_ERROR;
        status = PAMI_Client_create("multisync test", &client, NULL, 0);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to initialize pami client. result = %d\n", status);
                return 1;
        }

        { size_t _n = 1; status = PAMI_Context_createv(client, NULL, 0, &context, _n); }
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable to create pami context. result = %d\n", status);
                return 1;
        }

        pami_configuration_t configuration;

        configuration.name = PAMI_CLIENT_TASK_ID;
        status = PAMI_Client_query(client, &configuration,1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
        task_id = configuration.value.intval;
        //fprintf(stderr, "My task id = %zu\n", task_id);

        configuration.name = PAMI_CLIENT_NUM_TASKS;
        status = PAMI_Client_query(client, &configuration,1);
        if (status != PAMI_SUCCESS) {
                fprintf (stderr, "Error. Unable query configuration (%d). result = %d\n", configuration.name, status);
                return 1;
        }
        num_tasks = configuration.value.intval;
#else
        task_id = __global.mapping.task();
        num_tasks = __global.mapping.size();
        context = (pami_context_t)1; // context must not be NULL
#endif
        if (task_id == 0) fprintf(stderr, "Number of tasks = %zu\n", num_tasks);


// END standard setup
// ------------------------------------------------------------------------

        // Register some multisyncs, C++ style
#ifdef BARRIER1_NAME
	DO_BARRIER_TEST(BARRIER1_NAME "<128>", ShmemZeroModel<128>, ShmemZeroDevice, BARRIER1_ISLOCAL,
		BARRIER1_MM, task_id, num_tasks, context);
	DO_BARRIER_TEST(BARRIER1_NAME "<4096>", ShmemZeroModel<4096>, ShmemZeroDevice, BARRIER1_ISLOCAL,
		BARRIER1_MM, task_id, num_tasks, context);
	DO_BARRIER_TEST(BARRIER1_NAME "<16>", ShmemZeroModel<16>, ShmemZeroDevice, BARRIER1_ISLOCAL,
		BARRIER1_MM, task_id, num_tasks, context);
#endif // BARRIER1_NAME;

// ------------------------------------------------------------------------
#if 0
        status = PAMI_Client_destroy(&client);
        if (status != PAMI_SUCCESS) {
                fprintf(stderr, "Error. Unable to finalize pami client. result = %d\n", status);
                return 1;
        }
#endif

        return 0;
}
