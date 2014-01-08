/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef _CNK_CNVERBS_H
#define _CNK_CNVERBS_H

// Includes
#include "atomicops.h"
#include <cnk/include/Config.h>
#include <cnk/include/Verbs.h>
#include <mudm/include/mudm.h>

//! Opaque device context for external interfaces.
struct cnv_context
{
   void *mudmcontext;        //!< Pointer to mdum context.
};

//! Internal protection domain.
struct cnverbs_pd
{
   struct cnv_pd *pd;        //!< Pointer to user object.
   Lock_Atomic_t usecnt;     //!< Number of active references to object.
};

//! Internal memory region.
struct cnverbs_mr
{
   struct cnv_mr *mr;        //!< Pointer to user object.
   uint64_t base_va;         //!< Base virtual address of contiguous range.
   uint64_t base_pa;         //!< Base physical address of contiguous range.
   size_t base_sz;           //!< Size in bytes of contiguous range.
   uint32_t rkey;            //!< Remote key (used for fast lookups).
   Lock_Atomic_t usecnt;     //!< Number of active references to object.
};

//! State of work completion.
enum cnverbs_wc_state
{
   CNVERBS_WC_INIT,                         //!< Initialized (not in use).
   CNVERBS_WC_POSTED_SEND,                  //!< Send operation posted.
   CNVERBS_WC_POSTED_RECV,                  //!< Receive operation posted.
   CNVERBS_WC_RECV_INPROGRESS,              //!< Receive operation using DMA is in progress.
   CNVERBS_WC_READY,                        //!< Operation is completed.
};

//! Internal work completion.
struct cnverbs_wc
{
   struct cnv_cq *cq;                       //!< Pointer to external completion queue.
   struct cnv_qp *qp;                       //!< Pointer to external queue pair.
   enum cnverbs_wc_state state;             //!< Current state of work completion.
   uint32_t proc_id;                        //!< Processor id of thread that posted work completion. 
   uint32_t sequence_id;                    //!< sequence_id to match if qp is set to sequence_id
   struct cnv_wc wc;                        //!< External work completion (can be partially filled in).
   int num_recv_sge;                        //!< Number of scatter/gather elements in list.
   struct cnv_sge recv_sge[CNV_MAX_SGE];    //!< List of sge for posted recv.
   struct cnverbs_wc * next;                //!< For linked list characteristic processing 
};
  
//! Internal completion queue.
struct cnverbs_cq
{
   struct cnv_cq *cq;                       //!< Pointer to external completion queue.
   struct cnverbs_wc wc_list[CNV_MAX_WC];   //!< List of work completions.
   Lock_Atomic_t usecnt;                    //!< Number of active references to object.
   Lock_Atomic_t num_wc;                    //!< Number of posted work completions.
   Lock_Atomic_t wc_lock;                   //!< Lock to synchronize access to list of work completions.
   uint32_t      cq_character;              //!< flags modifying cq operation characteristics   
   struct cnverbs_wc * free_wc_list;        //!< free list for post_send or post_receive work requests
   struct cnverbs_wc * send_wc_list;        //!< send work requests in progress
   struct cnverbs_wc * receive_wc_list;     //!< receive work requests in progress
   struct cnverbs_wc * completed_wc_list;   //!< completed work requests for completion queue handling
   struct cnverbs_wc * send_wc_tail;        //!< send work requests in progress tail
   struct cnverbs_wc * receive_wc_tail;     //!< receive work requests in progress tail
   struct cnverbs_wc * completed_wc_tail;   //!< completed work requests for completion queue handling tail
   
};

//! Internal queue pair.
struct cnverbs_qp
{
   struct cnv_qp *qp;                       //!< Pointer to external queue pair.
   void *conn_context;                      //!< Pointer to mudm connect context.
   int conn_qp_num;                         //!< Number of connected queue pair.
   uint16_t psn;                            //!< Packet sequence number.
   uint16_t port_num;                       //!< Port number to identify queue pair. 
   Lock_Atomic_t connected;                 //!< True when queue pair is connected.
   Lock_Atomic_t usecnt;                    //!< Number of active references to object.
};

//! \brief  Initialize verbs.
//! \return 0 when successful, errno when unsuccessful.

int cnverbs_init(void);

//! \brief  Terminate verbs.
//! \return 0 when successful, errno when unsuccessful.

int cnverbs_term(void);

//! \brief  Handle a non-data message with connect or disconnect operations.
//! \param  conn_context Pointer to connection context (unused).
//! \param  header Pointer to message header.
//! \param  callback_context Pointer to context specified on mudm_init() which is always NULL.

int cnverbs_recv_conn(void *conn_context, struct ionet_header *header, void *callback_context);

//! \brief  Handle connect reply message received by mudm.
//! \param  msg Pointer to connect reply message.
//! \return 

int cnverbs_connect_reply(struct ionet_connect *msg);

//! \brief  Handle a message with inbound data.
//! \param  data Pointer to message data.
//! \param  callback_context Pointer to context specified on mudm_init() which is always NULL.
//! \return 0 when successful, -1 when error.

int cnverbs_recv(char *data, void *callback_context);

//! \brief  Handle packet data immediate message received by mudm.
//! \param  msg Pointer to packet data immediate message.
//! \param  qp_handle Queue pair with data to receive. 
//! \return 0 when successful, -1 when error.

int cnverbs_recv_immed(struct ionet_send_imm *msg, uint32_t qp_handle);

//! \brief  Handle packet data remote message received by mudm.
//! \param  msg Pointer to send remote message.
//! \param  qp_handle Queue pair with data to receive. 
//! \return 0 when successful, -1 when error.

int cnverbs_recv_remote(struct ionet_send_remote *msg, uint32_t qp_handle);

//! \brief  Handle rdma read message received by mudm.
//! \param  msg Pointer to send remote message.
//! \param  qp_handle Queue pair with data to receive. 
//! \return 0 when successful, -1 when error.

int cnverbs_rdma_read(struct ionet_send_remote *msg, uint32_t qp_handle);

//! \brief  Handle rdma write message received by mudm.
//! \param  msg Pointer to send remote message.
//! \param  qp_handle Queue pair with data to receive. 
//! \return 0 when successful, -1 when error.

int cnverbs_rdma_write(struct ionet_send_remote *msg, uint32_t qp_handle);

//! \brief  Handle completion of previously started operations.
//! \param  requestID Array of pointers to internal work completion entries for completed operations.
//! \param  status Array of status values for completed operations.
//! \param  callback_context Pointer to context specified on mudm_init() which is always NULL.
//! \param  error_return Array of return codes for each completed operation.
//! \param  num_ops Number of completed operations (all arrays are the same size).
//! \return 0 when successful, -1 when error.

int cnverbs_status(void* requestID[], uint32_t status[], void* callback_context, uint32_t error_return[], uint32_t num_ops);

//! \brief  Find a memory region by its rkey.
//! \param  rkey Remote key of memory region.
//! \return Pointer to memory region if found, or NULL when not found.

struct cnverbs_mr *cnverbs_find_mr_by_rkey(uint32_t rkey);

//! \brief  Find a free work completion from the specified completion queue.
//! \param  cqe Pointer to internal completion queue.
//! \return Pointer to work completion if found, or NULL when not found.

struct cnverbs_wc *cnverbs_find_free_wc(struct cnverbs_cq *cqe);

//! \brief  Find a work completion with a posted receive from the specified completion queue and queue pair.
//! \param  cqe Pointer to internal completion queue.
//! \param  qp Pointer to queue pair.
//! \return Pointer to work completion if found, or NULL when not found.

struct cnverbs_wc *cnverbs_find_recv_wc(struct cnverbs_cq *cqe, struct cnv_qp *qp);

//! \brief  Find a work completion with a posted receive that matches the specified queue pair and processor id.
//! \param  wc_list List of work completions in the internal completion queue.
//! \param  qp Pointer to queue pair.
//! \param  proc_id Processor id of hardware thread.
//! \return Pointer to work completion if found, or NULL when not found.

struct cnverbs_wc *cnverbs_find_recv_wc_by_id(struct cnverbs_wc *wc_list, struct cnv_qp *qp, uint32_t proc_id);

struct cnverbs_wc *cnverbs_find_recv_wc_by_seqid(struct cnverbs_wc *wc_list, struct cnv_qp *qp, uint32_t sequenceID);

int cnv_post_send_no_comp(struct cnv_qp *qp, struct cnv_send_wr *wr_list);

int cnv_poll_cq_for_single_recv(struct cnv_cq *cq, uint32_t proc_id);

//! \brief  Dump a buffer in hexadecimal format.
//! \param  identifier String to identify buffer.
//! \param  ptr Pointer to buffer.
//! \param  len Length of buffer.
//! \return Nothing.

void dumphex(const char *identifier, void *ptr, int len);

#endif // _CNK_CNVERBS_H
