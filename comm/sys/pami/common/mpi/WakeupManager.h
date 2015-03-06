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
/// \file common/mpi/WakeupManager.h
/// \brief PAMI wakeup manager implementation.
///
#ifndef __common_mpi_WakeupManager_h__
#define __common_mpi_WakeupManager_h__

#include "common/WakeupManagerInterface.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#undef USE_WAKEUP_VECTORS

namespace PAMI {

        class WakeupManager : public Interface::WakeupManager<PAMI::WakeupManager> {
                // everything must fit in a (void *)
                #define SEMID_BITS	((sizeof(void *) / 2) * 8)
                #define SEMNO_BITS	SEMID_BITS

                #define MAX_SEMID	((1UL << SEMID_BITS) - 1)
                #define MAX_SEMNO	((1UL << SEMNO_BITS) - 1)
        public:
                inline WakeupManager() :
                Interface::WakeupManager<PAMI::WakeupManager>(),
                _semKey(IPC_PRIVATE),
                _semSet(0),
                _semNum(0)
                {
                }

                inline pami_result_t init_impl(int num, int setKey) {
                        if (_semKey != IPC_PRIVATE) {
                                return PAMI_ERROR;
                        }
                        if (num > (int)MAX_SEMNO) {
                                return PAMI_ERROR;
                        }
                        key_t key = (key_t)setKey;
                        PAMI_assertf(key != IPC_PRIVATE, "WakeupManager called with illegal key value %d\n", key);
                        int rc = semget(key, num, IPC_CREAT);
                        if (rc < 0) {
                                return PAMI_ERROR;
                        }
                        if (rc > (int)MAX_SEMID) {
                                return PAMI_ERROR;
                        }
                        _semSet = rc;
                        _semKey = key;
                        _semNum = num;
                        return PAMI_SUCCESS;
                }

                inline void *getWakeupVec_impl(int num) {
                        if (num >= _semNum || num < 0) {
                                return NULL;
                        }
                        size_t z = ((num + 1UL) << SEMNO_BITS) | _semSet;
                        return (void *)z;
                }

                inline pami_result_t wakeup_impl(void *v) {
                        struct sembuf op;
                        size_t z = (size_t)v;
                        op.sem_num = ((z >> SEMNO_BITS) & MAX_SEMNO) - 1;
                        op.sem_op = 1;	// "up" the semaphore
                        op.sem_flg = IPC_NOWAIT; // just in case...
                        // PAMI_assert(_semSet == (z & MAX_SEMID));
                        int err = semop(_semSet, &op, 1);
                        if (err < 0) {
                                return PAMI_ERROR;
                        }
                        return PAMI_SUCCESS;
                }

                inline pami_result_t clear_impl(void *v) {
                        // union semun a;
                        int a;
                        size_t z = (size_t)v;
                        int sem_num = ((z >> SEMNO_BITS) & MAX_SEMNO) - 1;
                        // a.val = 0;
                        a = 0;
                        // PAMI_assert(_semSet == (z & MAX_SEMID));
                        int err = semctl(_semSet, sem_num, SETVAL, &a);
                        if (err < 0) {
                                return PAMI_ERROR;
                        }
                        return PAMI_SUCCESS;
                }

                inline pami_result_t sleep_impl(void *v) {
                        struct sembuf op;
                        size_t z = (size_t)v;
                        op.sem_num = ((z >> SEMNO_BITS) & MAX_SEMNO) - 1;
                        op.sem_op = -1; // 'down' the semaphore
                        op.sem_flg = 0;
                        // PAMI_assert(_semSet == (z & MAX_SEMID));
                        int err = semop(_semSet, &op, 1);
                        if (err < 0) {
                                return PAMI_ERROR;
                        }
                        clear_impl(v);
                        return PAMI_SUCCESS;
                }

                inline pami_result_t trySleep_impl(void *v) {
                        struct sembuf op;
                        size_t z = (size_t)v;
                        op.sem_num = ((z >> SEMNO_BITS) & MAX_SEMNO) - 1;
                        op.sem_op = -1; // 'down' the semaphore
                        op.sem_flg = IPC_NOWAIT;
                        // PAMI_assert(_semSet == (z & MAX_SEMID));
                        int err = semop(_semSet, &op, 1);
                        if (err == EAGAIN) {
                                return PAMI_EAGAIN;
                        }
                        if (err < 0) {
                                return PAMI_ERROR;
                        }
                        clear_impl(v);
                        return PAMI_SUCCESS;
                }

                inline pami_result_t poll_impl(void *v) {
                        size_t z = (size_t)v;
                        int s = ((z >> SEMNO_BITS) & MAX_SEMNO) - 1;
                        // PAMI_assert(_semSet == (z & MAX_SEMID));
                        int err = semctl(_semSet, s, GETVAL);
                        if (err < 0) {
                                return PAMI_ERROR;
                        }
                        if ((short)err <= 0) {
                                return PAMI_EAGAIN;
                        }
                        return PAMI_SUCCESS;
                }

        private:
                key_t _semKey;
                int _semSet;
                int _semNum;

        }; // class WakeupManager
}; // namespace PAMI

#endif // __common_mpi_WakeupManager_h__
