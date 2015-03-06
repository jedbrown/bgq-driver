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
 * \file test/api/p2p/send/send_concurrency.h
 * \brief Common routines and items for concurrency tests
 */

#ifndef __test_api_p2p_send_send_concurrency_h__
#define __test_api_p2p_send_send_concurrency_h__

#include "send_general.h"

int recvflags[2];	/* 2 phases are enough to keep things separate */

void send_concurrency_init(int bufsiz, size_t nranks) {
        send_general_init(bufsiz);
}

struct test_msginfo {
        int phase;	/* test phase message is for */
        unsigned _w1;	/* unused */
        unsigned _w2;	/* unused */
        unsigned _w3;	/* unused */
};

/* --------------------------------------------------------------- */

static void concurrency_senddone(pami_context_t context, void *clientdata, pami_result_t err) {
        ++sendflag;
        TRACE_ERR(stderr, "Send Done\n");
}

/* --------------------------------------------------------------- */

static void concurrency_recvdone(pami_context_t context, void *clientdata, pami_result_t err) {
        int *rflagp = (int *)clientdata;
        ++(*rflagp);
        TRACE_ERR(stderr, "Recv Done\n");
}


/* --------------------------------------------------------------- */

static void cb_recv_con(pami_context_t context, void *clientdata,
                        void *msginfo, size_t msginfolen,
                        void *pipe_addr, size_t pipe_size,
                        pami_endpoint_t origin,
pami_recv_t *recv) {
        struct test_msginfo *mi = (struct test_msginfo *)msginfo;
        int phase = mi->phase;

        if (pipe_addr != NULL) {
                memcpy(rbuf, pipe_addr, pipe_size);
                ++recvflags[phase];
        } else {
                recv->cookie = &recvflags[phase];
                recv->local_fn = concurrency_recvdone;
                recv->kind = PAMI_AM_KIND_SIMPLE;
                recv->data.simple.bytes = pipe_size;
                recv->data.simple.addr = rbuf;
        }
}

int con_recv_err = 0, con_send_err = 0;

int con_setup_netw(int net, pami_context_t context, size_t *proto) {
        /* turns out, for now, we don't need special values here, */
        /* but this is a better way of doing the registration since */
        /* the callbacks reside in this file and so should the specification */
        /* of clientdata. */
        return setup_netw(net, context, proto, cb_recv_con, NULL);
}

/* 'ranks' is NULL for global (non-local) test. */
/* Global: rank = 0..numranks-1 */
/* Local:  rank = ranks[0..numranks-1] */
/* */
int con_test(size_t dispatch_id, pami_client_t client, pami_context_t context,
                pami_task_t rank, pami_task_t *ranks, size_t numranks,
                unsigned *times, bool verify) {
        int i, niter = ITERATIONS, tt = 0;
        size_t sndlen;
        size_t targ;

        con_recv_err = con_send_err = 0;
        struct test_msginfo msginfo;

        static bool init = true;
        if (init) {
                /* Configure and register barriers */
                barrier_init();
                init = false;
        }
        pami_send_t send;
        send.send.header.iov_base = &msginfo;
        send.send.header.iov_len = sizeof(msginfo);
        send.send.data.iov_base = sbuf;
        send.send.hints = (pami_send_hint_t){0};
        send.send.dispatch = dispatch_id;
        send.events.cookie = NULL;
        send.events.local_fn = concurrency_senddone;
        send.events.remote_fn = NULL;

        curr_tag = (((rank << 8) | numranks) << 16);

        for (sndlen = MINBUFSIZE; sndlen <= BUFSIZE; sndlen = NEXT_BUFSIZE(sndlen)) {
                ++curr_tag;
                fill_buf(sbuf, sndlen, curr_tag);
                send.send.data.iov_len = sndlen;

                /* assert (recvflag == lastrecv); */
                sendflag = recvflags[0] = recvflags[1] = 0;
                barrier();
                unsigned long long t1 = PAMI_Wtimebase();
                for (i = 0; i < niter; i++) {
                        int phase = i & 1; /* phase of test, alternate 0/1 */
                        msginfo.phase = phase;
                        PAMI_Context_lock(context);
                        for (targ = 0; targ < numranks; ++targ) {
                                unsigned t = ranks ? ranks[targ] : targ;
                                if (t == rank) continue;
                                PAMI_Endpoint_create(client, t, 0, &send.send.dest);
                                PAMI_Send(context, &send);
                        }
                        while (sendflag < (int)numranks - 1) {
                                PAMI_Context_advance(context, 100);
                        }
                        while (recvflags[phase] < (int)numranks - 1) {
                                PAMI_Context_advance(context, 100);
                        }
                        PAMI_Context_unlock(context);
                        recvflags[phase] = 0;
                        sendflag = 0;
                        if (verify) {
                                ; /* TBD... */
                                /* need barrier?  how to separate diff orig buffers? */
                        }
                }
                unsigned long long t2 = PAMI_Wtimebase();
                unsigned t = (unsigned)((t2 - t1) / (numranks - 1) / niter);
                if (times) {
                        times[tt++] = t;
                }
        }

        barrier();
        if (con_recv_err || con_send_err) {
                fprintf(stderr, "%zu saw %d recv buffer errors and %d send buffer errors\n",
                        rank, con_recv_err, con_send_err);
        }
        return 0;
}

#endif
