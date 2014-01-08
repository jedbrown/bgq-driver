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

// Includes
#include "Kernel.h"
#include <cnk/include/Verbs.h>
#include "cnverbs.h"
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

struct Msg_hdr
{
   uint8_t  service;              //!< Service to process message.
   uint8_t  version;              //!< Protocol version number.
   uint16_t type;                 //!< Content of message.
   uint32_t rank;                 //!< Rank message is associated with.
   uint32_t sequenceId;           //!< Correlate requests and acknowledgements.
   uint32_t returnCode;           //!< Result of previous request.
   uint32_t errorCode;            //!< Error detail (typically errno value).
   uint32_t length;               //!< Amount of data in message (including this header).
   uint64_t jobId;                //!< Job message is associated with.
};

// Constant to convert from external number to internal handle.
#define CNV_HANDLE_ADJUST (1)

// Starting port number (all ports are ephemeral).
#define CNV_BASE_PORT (2000)

// List of protection domains.
struct cnverbs_pd pdlist[CNV_MAX_PD];

// List of memory regions.
struct cnverbs_mr mrlist[CNV_MAX_MR];

// List of completion queues.
struct cnverbs_cq cqlist[CNV_MAX_CQ];

// List of queue pairs.
struct cnverbs_qp qplist[CNV_MAX_QP];

// Opaque context for external interfaces.
struct cnv_context cnk_context;

uint64_t CQ_NUM_WC[CNV_MAX_CQ] K_ATOMIC;

int cnverbs_init(void)
{
   // Initialize all of the internal objects.
   uint32_t index = 0;
   for ( ; index < CNV_MAX_PD; ++index) {
      memset(&(pdlist[index]), 0, sizeof(struct cnverbs_pd));
   }

   for (index = 0; index < CNV_MAX_MR; ++index) {
      memset(&(mrlist[index]), 0, sizeof(struct cnverbs_mr));
   }

   for (index = 0; index < CNV_MAX_CQ; ++index) {
      memset(&(cqlist[index]), 0, sizeof(struct cnverbs_cq));
   }

   for (index = 0; index < CNV_MAX_QP; ++index) {
      memset(&(qplist[index]), 0, sizeof(struct cnverbs_qp));
   }

   cnk_context.mudmcontext = NodeState.MUDM;

   TRACE( TRACE_Verbs, ("(I) cnverbs_init(): compute node verbs are initialized\n") );
   return 0;
}

int cnverbs_term(void)
{
   TRACE( TRACE_Verbs, ("(I) cnverbs_term(): compute node verbs are terminated\n") );
   return 0;
}

int cnverbs_recv_conn(void *conn_context, struct ionet_header *header, void *callback_context)
{
   int rc = 0;
   switch (header->type) {
      case MUDM_CONN_REPLY:
         rc = cnverbs_connect_reply((struct ionet_connect *)header);
         break;

      case MUDM_DISCONNECTED:
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDISCON, 0, 0, 0, 0);
         printf("(E) cnverbs_recv_conn: Received a disconnect request and it was ignored.  Most likely an error occurred on the I/O node.\n");
         rc = -1;
         break;

      case MUDM_CONN_REQUEST:
         //! \todo Send a RAS event -- CNK will never support inbound connections.
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVCONREQ, 0, 0, 0, 0);
         printf("(E) cnverbs_recv_conn: Received a connection request and it was ignored.  Most likely the node's personality is not configured correctly.\n");
         rc = -1;
         break;

      default:
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVCONREQ, header->type, 0, 0, 0);
         printf("(E) cnverbs_recv_conn(): message type %x is not supported and was ignored\n", header->type);
         rc = -1;
         break;
   }
   return rc;
}

int cnverbs_connect_reply(struct ionet_connect *msg)
{
// dumphex("connect reply", msg, sizeof(struct ionet_connect));

   // Find the queue pair for this connect reply.
   uint32_t handle = msg->ionet_hdr.dest_qpn - CNV_HANDLE_ADJUST;
   if (handle >= CNV_MAX_QP) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADQPN, msg->ionet_hdr.dest_qpn, handle, 0, 0);
      printf("(E) cnverbs_connect_reply(): local qpn %u (handle %u) in message is invalid\n", msg->ionet_hdr.dest_qpn, handle);
      return -1;
   }

   // Make sure the queue pair is in use.
   if (fetch(&(qplist[handle].usecnt)) == 0) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVQPNUSE, msg->ionet_hdr.dest_qpn, handle, 0, 0);
      printf("(E) cnverbs_connect_reply(): local qpn %u (handle %u) is not in use\n", msg->ionet_hdr.dest_qpn, handle);
      return -1;
   }

   // Make sure connect request was successful.
   if (msg->status != 0) {
      //! \todo Send a FATAL RAS event?
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVCONREJ, msg->ionet_hdr.dest_qpn, handle, msg->status, 0);
      printf("(E) cnverbs_connect_reply(): connect request from qpn %u (handle %u) was rejected with status %u\n", msg->ionet_hdr.dest_qpn, handle, msg->status);
      return -1;
   }

   // Update queue pair to complete connect.
   struct cnv_qp *qp = qplist[handle].qp;
   qp->state = CNV_QPS_RTS;
   fetch_and_add(&(qplist[handle].connected), 1);
   qplist[handle].conn_qp_num = msg->ionet_hdr.source_qpn;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVCONACC, msg->ionet_hdr.dest_qpn, handle, msg->status, 0);
   TRACE( TRACE_Verbs, ("(I) cnverbs_connect_reply(): connect completed for queue pair %u (handle %u) to remote queue pair %u\n",
                        qp->qp_num, handle, msg->ionet_hdr.source_qpn) );
// dumphex("queue pair after connect", qp, sizeof(struct cnv_qp));
   return 0;
}

int cnverbs_recv(char *data, void *callback_context)
{
   struct ionet_header *header = (struct ionet_header *)data;

   // Find the queue pair for this receive.
   uint32_t qp_handle = header->dest_qpn - CNV_HANDLE_ADJUST;
   if (qp_handle >= CNV_MAX_QP) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADQPN, header->dest_qpn, qp_handle, 0, 0);
      TRACE( TRACE_Verbs, ("(E) cnverbs_recv_immed(): dest qpn %u (handle %u) in recv immediate packet is invalid\n", header->dest_qpn, qp_handle) );
      return -1;
   }

   // Validate packet sequence number.
   if (header->sequence_number == qplist[qp_handle].psn) {
      qplist[qp_handle].psn++;
   }
   else {      
      printf("(E) cnverbs_recv_immed(): sequence number %u in packet does not match expected value %u for qpn %u (handle %u)\n",
             header->sequence_number, qplist[qp_handle].psn, header->dest_qpn, qp_handle);
      //qplist[qp_handle].psn = header->sequence_number + 1;
      if (qplist[qp_handle].conn_qp_num != header->source_qpn){
         printf("(E) cnverbs_recv_immed(): Source QP number %u in packet does not match expected value %u for qpn %u (handle %u)\n",
             header->source_qpn, qplist[qp_handle].conn_qp_num, header->dest_qpn, qp_handle);
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADSQP, header->source_qpn, qplist[qp_handle].conn_qp_num, header->dest_qpn, qp_handle);
         return -ENOTCONN;
      }
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADPSN, header->sequence_number, qplist[qp_handle].psn, header->dest_qpn, qp_handle);
      return -EILSEQ;
   }

   int rc = 0;
   switch (header->type) {
      case MUDM_PKT_DATA_IMM:
         rc = cnverbs_recv_immed((struct ionet_send_imm *)data, qp_handle);
         break;

      case MUDM_PKT_DATA_REMOTE:
         rc = cnverbs_recv_remote((struct ionet_send_remote *)data, qp_handle);
         break;

      case MUDM_RDMA_READ:
         rc = cnverbs_rdma_read((struct ionet_send_remote *)data, qp_handle);
         break;

      case MUDM_RDMA_WRITE:
         rc = cnverbs_rdma_write((struct ionet_send_remote *)data, qp_handle);
         break;

      default:
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVCONREQ, header->type, 0, 0, 0);
         printf("(E) cnkmudm_recv(): message type %x is not supported\n", header->type);
         rc = -1;
         break;
   }
   return rc;
}

int cnverbs_recv_immed(struct ionet_send_imm *msg, uint32_t qp_handle)
{
// dumphex("recv immed", msg, sizeof(struct ionet_send_imm));
   // qp_handle is bad

   // Make sure there is a receive posted for this queue pair.
   if( (qplist[qp_handle].qp == NULL) ||  (  (uint64_t)qplist[qp_handle].qp > (uint64_t)&__KERNEL_END) )
   {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_BADQP_VAL, (uint64_t)qplist[qp_handle].qp,(uint64_t) &__KERNEL_END, qp_handle, msg->ionet_hdr.dest_qpn);
      uint64_t * data = (uint64_t *)msg->payload;
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADIMM, data[0],data[1],data[2],data[3]);
      Kernel_WriteFlightLog(FLIGHTLOG,FL_UNKMSGRCV, data[0],data[1],data[2],data[3]);
      return -1;
   }

   uint32_t cq_handle = qplist[qp_handle].qp->recv_cq->handle;
   struct cnverbs_cq *cqe = &(cqlist[cq_handle]);
   Kernel_Lock(&(cqe->wc_lock));
   struct cnverbs_wc *wce = NULL;
   if ( cqe->cq_character ==CNVERBS_SEQUENCEID_CQ_CHAR){
      if (msg->ionet_hdr.payload_length >= sizeof(struct Msg_hdr)){
       struct Msg_hdr *header = (struct Msg_hdr *) msg->payload;
       wce = cnverbs_find_recv_wc_by_seqid(cqe->wc_list, qplist[qp_handle].qp, header->sequenceId);
     }
   }
   else if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){
      if (cqe->receive_wc_list !=NULL){
         wce = cqe->receive_wc_list;
         cqe->receive_wc_list = wce->next;
         wce->next=NULL;
         if (cqe->receive_wc_list==NULL)cqe->receive_wc_tail=NULL;
      }
      else wce=NULL;
   }

   else { 
     
     wce = cnverbs_find_recv_wc(cqe, qplist[qp_handle].qp);
   }
   if (wce == NULL) {
      Kernel_Unlock(&(cqe->wc_lock));
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVNORECV, msg->ionet_hdr.dest_qpn, qp_handle, 0, 0);
      TRACE( TRACE_Verbs, ("(E) cnverbs_recv_immed(): recv is not posted for qpn %u (handle %u)\n", msg->ionet_hdr.dest_qpn, qp_handle) );
      uint64_t * data = (uint64_t *)msg->payload;
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADIMM, data[0],data[1],data[2],data[3]);
      TRACE( TRACE_Verbs, ("(E) incoming starting data 0x%lx 0x%lx 0x%lx 0x%lx \n", data[0],data[1],data[2],data[3]) );
      if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){
         //! \todo TODO NEED TO MARK QP in ERROR STATE........., AND SEND disconnect to IO node
         return 0;  //failure should not bring down compute node , return 0 
      }
      return -1;
   }

   // Copy data from immediate packet to specified address.
   void *dest = (void *)(wce->recv_sge[0].addr);
   memcpy(dest, msg->payload, msg->ionet_hdr.payload_length);
   TRACE( TRACE_Verbs, ("(I) cnverbs_recv_immed(): copied %u bytes to address %p\n", msg->ionet_hdr.payload_length, dest) );
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVSELWCE, (uint64_t)wce, cq_handle, wce->proc_id, (uint64_t)dest);

   // Update work completion.
   enum cnverbs_wc_state prev_state = wce->state;
   wce->wc.status = CNV_WC_SUCCESS;
   wce->wc.byte_len = msg->ionet_hdr.payload_length;
   wce->wc.src_qp = msg->ionet_hdr.source_qpn;
   wce->state = CNVERBS_WC_READY;
   if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){//put onto end of completion list
     if (cqe->completed_wc_tail){
        cqe->completed_wc_tail->next=wce;
        cqe->completed_wc_tail=wce;
     }
     else {
        cqe->completed_wc_tail=wce;
        cqe->completed_wc_list=wce;
     }
   }

   // Update completion queue.
   ppc_msync();
   uint64_t prev_num_wc = L2_AtomicLoadIncrement(&CQ_NUM_WC[cq_handle]);
//   uint32_t prev_num_wc = fetch_and_add(&(cqlist[cq_handle].num_wc), 1);
   Kernel_Unlock(&(cqe->wc_lock));
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVADDWCE, cq_handle, prev_num_wc, wce->wc.status, prev_state);
   TRACE( TRACE_Verbs, ("(I) cnverbs_recv_immed(): posted completion queue %u (num_wc %ld)\n", cq_handle, prev_num_wc));
   
   return 0;
}

int cnverbs_recv_remote(struct ionet_send_remote *msg, uint32_t qp_handle)
{
// dumphex("recv remote", msg, sizeof(struct ionet_send_imm));

   // Make sure there is a receive posted for this queue pair.
   if( (qplist[qp_handle].qp == NULL) ||  (  (uint64_t)qplist[qp_handle].qp > (uint64_t)&__KERNEL_END) )
   {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_BADQP_VAL, (uint64_t)qplist[qp_handle].qp,(uint64_t) &__KERNEL_END, qp_handle, msg->ionet_hdr.dest_qpn);
      return -1;
   }
   uint32_t cq_handle = qplist[qp_handle].qp->recv_cq->handle;
   struct cnverbs_cq *cqe = &(cqlist[cq_handle]);
   struct cnverbs_wc *wce;
   Kernel_Lock(&(cqe->wc_lock));
   if ( cqe->cq_character ==CNVERBS_SEQUENCEID_CQ_CHAR){
      wce = cnverbs_find_recv_wc_by_seqid(cqe->wc_list, qplist[qp_handle].qp, 
                                          msg->payload.immediate_data);
   }
   else if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){
      if (cqe->receive_wc_list !=NULL){
         wce = cqe->receive_wc_list;
         cqe->receive_wc_list = wce->next;
         wce->next=NULL;
      }
      else wce=NULL;
   }
   else if ((msg->payload.flags == MUDM_IMMEDIATE_DATA) && (msg->payload.immediate_data & CNV_DIRECTED_RECV)) {
       wce = cnverbs_find_recv_wc_by_id(cqe->wc_list, qplist[qp_handle].qp, (msg->payload.immediate_data & ~CNV_DIRECTED_RECV));
   }
   else {
      wce = cnverbs_find_recv_wc(cqe, qplist[qp_handle].qp);
   }
   if (wce == NULL) {      
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVNORECV, msg->ionet_hdr.dest_qpn, qp_handle, 0,0);
      Kernel_Unlock(&(cqe->wc_lock));
      uint64_t * data = (uint64_t *)&msg->payload.remote_vaddr;
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADRMT, data[0],data[1],data[2],data[3]);
      TRACE( TRACE_Verbs, ("(E) cnverbs_recv_remote(): recv is not posted for qpn %u (handle %u)\n", msg->ionet_hdr.dest_qpn, qp_handle) );
      if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){
         //! \todo TODO NEED TO MARK QP in ERROR STATE........., AND SEND disconnect to IO node
         return 0;  //failure should not bring down compute node , return 0 
      }
      return -1;
   }
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVSELWCE, (uint64_t)wce, cq_handle, wce->proc_id, wce->recv_sge[0].addr);

   // Save immediate data if available.
   wce->wc.wc_flags = 0;
   if (msg->payload.flags & MUDM_IMMEDIATE_DATA) {
      wce->wc.imm_data = msg->payload.immediate_data;
      wce->wc.wc_flags |= CNV_WC_FLAGS_IMM;
   }

   // Convert external sge to mudm sge.
   uint32_t index = 0;
   struct mudm_sgl psge[MUDM_MAX_SGE];
   for ( ; index < wce->num_recv_sge; ++index) {
      uint32_t lkey = wce->recv_sge[index].lkey - CNV_HANDLE_ADJUST; //! \todo Validate lkey?  Make sure length doesn't exceed memory region?
      psge[index].physicalAddr = (wce->recv_sge[index].addr - mrlist[lkey].base_va) + mrlist[lkey].base_pa;
      psge[index].memlength = wce->recv_sge[index].length;
//    TRACE( TRACE_Verbs, ("(I) cnverbs_recv_remote(): local sge %u addr=%lu length=%u\n", index, psge[index].physicalAddr, psge[index].memlength) );
   }

   // Start read from remote node.
   if (qplist[qp_handle].conn_context == NULL) return -1;
   int rc = mudm_rdma_read(qplist[qp_handle].conn_context, wce, (void *)msg->payload.RequestID, msg->payload.rdma_object,
                           msg->payload.data_length, psge, msg->payload.SGE);

   // Read operation completed successfully so post to completion queue.
   if (rc == 0) {
      enum cnverbs_wc_state prev_state = wce->state;
      wce->wc.status = CNV_WC_SUCCESS;
      wce->state = CNVERBS_WC_READY;
      struct cnv_cq *cq = wce->cq;
      ppc_msync();
      uint64_t prev_num_wc = L2_AtomicLoadIncrement(&CQ_NUM_WC[cq_handle]);
      //uint32_t prev_num_wc = fetch_and_add(&(cqlist[cq->handle].num_wc), 1);
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVADDWCE, cq_handle, prev_num_wc, wce->wc.status, prev_state);
      TRACE( TRACE_Verbs, ("(I) cnverbs_recv_remote(): posted completion queue %u (num_wc %ld)\n", cq->handle, prev_num_wc) );
      if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){//put onto end of completion list
         if (cqe->completed_wc_tail) cqe->completed_wc_tail->next=wce;
         else                        cqe->completed_wc_list = wce;
         cqe->completed_wc_tail = wce;
      }
   }

   // Wait for read operation to complete.
   else if (rc == -EINPROGRESS) {
      wce->state = CNVERBS_WC_RECV_INPROGRESS;
      if ( cqe->cq_character ==CNVERBS_WC_LINKEDLIST_CQ_CHAR){//put onto end of completion list
         if (cqe->completed_wc_tail) cqe->completed_wc_tail->next=wce;
         else                        cqe->completed_wc_list = wce;
         cqe->completed_wc_tail=wce;
      }
   }

   // Unexpected error.
   else {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVRDMARD, rc, msg->ionet_hdr.dest_qpn, qp_handle, 0);
      TRACE( TRACE_Verbs, ("(E) cnverbs_recv_remote(): mudm_rdma_read failed with rc %d for qpn %u (handle %u)\n", rc, msg->ionet_hdr.dest_qpn, qp_handle) );
   }

   Kernel_Unlock(&(cqe->wc_lock));

   return 0;
}

int cnverbs_rdma_read(struct ionet_send_remote *msg, uint32_t qp_handle)
{
// dumphex("rdma read msg", msg, sizeof(struct ionet_send_remote));

   // Find the memory region that describes the memory on this node.
   struct cnverbs_mr *cnv_mr = cnverbs_find_mr_by_rkey(msg->payload.rkey);
   if (cnv_mr == NULL) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADMR_, msg->payload.rkey, 0, 0, 0);
      printf("(E) cnverbs_rdma_read(): memory region with rkey %u was not found\n", msg->payload.rkey);
      return -1;
   }

   // Make sure the memory region is big enough for the requested length.
   if (cnv_mr->mr->length < msg->payload.data_length) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVMRSIZE, msg->payload.rkey, cnv_mr->mr->length, msg->payload.data_length, 0);
      printf("(E) cnverbs_rdma_read(): memory region with rkey %u and length %lu is too small for request of %u bytes\n",
             msg->payload.rkey, cnv_mr->mr->length, msg->payload.data_length);
      return -1;
   }

   // Build SGE with physical address for memory on this node.
   struct mudm_sgl psge[MUDM_MAX_SGE];
   psge[0].physicalAddr = (msg->payload.remote_vaddr - cnv_mr->base_va) + cnv_mr->base_pa;
   psge[0].memlength = msg->payload.data_length;
   TRACE( TRACE_Verbs, ("(I) cnverbs_rdma_read(): ready to start rdma read for memory region with rkey %u at vaddr 0x%lx (paddr=0x%lx) for length=%d\n",
                        msg->payload.rkey, msg->payload.remote_vaddr, (uint64_t)psge[0].physicalAddr, psge[0].memlength) );

   // Start read from remote node.
   if (qplist[qp_handle].conn_context == NULL) return -1;
   int rc = mudm_rdma_read(qplist[qp_handle].conn_context, NULL, (void *)msg->payload.RequestID, msg->payload.rdma_object,
                           msg->payload.data_length, psge, msg->payload.SGE);

   TRACE( TRACE_Verbs, ("(I) cnverbs_rdma_read: mudm_rdma_read started for dest qpn %u (handle %u), rc=%d\n",
                        msg->ionet_hdr.dest_qpn, qp_handle, rc) );

   return 0;
}

int cnverbs_rdma_write(struct ionet_send_remote *msg, uint32_t qp_handle)
{
// dumphex("rdma write msg", msg, sizeof(struct ionet_send_remote));

   // Find the memory region that describes the memory on this node.
   struct cnverbs_mr *cnv_mr = cnverbs_find_mr_by_rkey(msg->payload.rkey);
   if (cnv_mr == NULL) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADMR_, msg->payload.rkey, 0, 0, 0);
      printf("(E) cnverbs_rdma_write(): rkey %u was not found\n", msg->payload.rkey);
      return -1;
   }

   // Make sure the memory region is big enough for the requested length.
   if (cnv_mr->mr->length < msg->payload.data_length) {
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVMRSIZE, msg->payload.rkey, cnv_mr->mr->length, msg->payload.data_length, 0);
      printf("(E) cnverbs_rdma_read(): memory region with rkey %u and length %lu is too small for request of %u bytes\n",
             msg->payload.rkey, cnv_mr->mr->length, msg->payload.data_length);
      return -1;
   }

   // Build SGE with physical address for memory on this node.
   struct mudm_sgl psge[MUDM_MAX_SGE];
   psge[0].physicalAddr = (msg->payload.remote_vaddr - cnv_mr->base_va) + cnv_mr->base_pa;
   psge[0].memlength = msg->payload.data_length;
   TRACE( TRACE_Verbs, ("(I) cnverbs_rdma_write(): ready to start rdma write for memory region with rky %u at vaddr=0x%lx (paddr=0x%lx) for length=%d\n",
                        msg->payload.rkey, msg->payload.remote_vaddr, (uint64_t)psge[0].physicalAddr, psge[0].memlength) );

   // Start write from remote node.
   if (qplist[qp_handle].conn_context == NULL) return -1;
   int rc = mudm_rdma_write(qplist[qp_handle].conn_context, (void *)msg->payload.RequestID, msg->payload.rdma_object,
                            msg->payload.data_length, psge, msg->payload.SGE);

   TRACE( TRACE_Verbs, ("(I) cnverbs_rdma_write: mudm_rdma_write started for dest qpn %u (handle %u), rc=%d\n",
                        msg->ionet_hdr.dest_qpn, qp_handle, rc) );

   return 0;
}

int cnverbs_status(void *requestID[], uint32_t status[], void *callback_context, uint32_t error_return[], uint32_t num_ops)
{
   // Run the list of completed operations.
   int rc = 0;
   uint32_t index;
   for (index = 0; index < num_ops; ++index) {
      // Make sure there is an outstanding operation for this work completion.
      struct cnverbs_wc *wce = (struct cnverbs_wc *)requestID[index];
      if (wce==NULL) continue;
      if ((wce->state == CNVERBS_WC_INIT) || (wce->state == CNVERBS_WC_READY)) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVBADWCE, (uint64_t)wce, wce->state, 0, 0);
         printf("(E) cnverbs_status(): work completion %p with state %u does not have outstanding operation\n", wce, wce->state);
         error_return[index] = 1;
         rc = -1;
         continue;
      }

      // Finish filling out work completion.
      enum cnverbs_wc_state prev_state = wce->state;
      if (status[index] == 0) {
         wce->wc.status = CNV_WC_SUCCESS;
      }
      else {
         wce->wc.status = CNV_WC_GENERAL_ERR;
      }
      wce->state = CNVERBS_WC_READY;

      // Post to completion queue.
      struct cnv_cq *cq = wce->cq;
      if (cq == NULL) {
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVNULLCQ, (uint64_t)wce, (uint64_t)wce->qp, wce->state, 0);
         printf("(E) cnverbs_status(): cq pointer is null in work completion %p, qp %p, state %u\n", wce, wce->qp, wce->state);
         error_return[index] = 1;
         rc = -1;
         continue;
      }
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVGOTWCE, (uint64_t)wce, cq->handle, wce->proc_id, index);
      // uint32_t prev_num_wc = fetch_and_add(&(cqlist[cq->handle].num_wc), 1);
      
      ppc_msync();
      uint64_t prev_num_wc = L2_AtomicLoadIncrement(&CQ_NUM_WC[cq->handle]);
      Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVADDWCE, cq->handle, prev_num_wc, wce->wc.status, prev_state);
      error_return[index] = 0;

      TRACE( TRACE_Verbs, ("(I) cnverbs_status(): posted completion queue %u (num_wc %ld)\n", cq->handle, prev_num_wc) );
   }

   TRACE( TRACE_Verbs, ("(I) cnverbs_status(): processed %u operations, rc %d\n", num_ops, rc) );
   return rc;
}

struct cnverbs_mr *cnverbs_find_mr_by_rkey(uint32_t rkey)
{
   // Search the list of memory regions for the memory region with the specified rkey.
   int handle;
   for (handle = 0; handle < CNV_MAX_MR; ++handle) {
      if (mrlist[handle].rkey == rkey) {
         return &(mrlist[handle]);
      }
   }

   return NULL;
}

struct cnverbs_wc *cnverbs_find_free_wc(struct cnverbs_cq *cqe)
{
   // Search the list of work completions in the completion queue.
   int index;
   for (index = 0; index < CNV_MAX_WC; ++index) {
      if (cqe->wc_list[index].state == CNVERBS_WC_INIT) {
         return &(cqe->wc_list[index]);
      }
   }

   return NULL;
}

struct cnverbs_wc *cnverbs_find_recv_wc(struct cnverbs_cq *cqe, struct cnv_qp *qp)
{
   // Search the list of work completions in the completion queue.
   int index;
   for (index = 0; index < CNV_MAX_WC; ++index) {
      if ((cqe->wc_list[index].state == CNVERBS_WC_POSTED_RECV) &&
          (cqe->wc_list[index].qp == qp)) {
         return &(cqe->wc_list[index]);
      }
   }

   return NULL;
}

struct cnverbs_wc *cnverbs_find_recv_wc_by_id(struct cnverbs_wc *wc_list, struct cnv_qp *qp, uint32_t proc_id)
{
   int index;
   for (index = 0; index < CNV_MAX_WC; ++index) {
      if ((wc_list[index].state == CNVERBS_WC_POSTED_RECV) &&
          (wc_list[index].qp == qp) &&
          ((wc_list[index].proc_id & 0xff) == proc_id)) {
         return &(wc_list[index]);
      }
   }

   return NULL;
}

struct cnverbs_wc *cnverbs_find_recv_wc_by_seqid(struct cnverbs_wc *wc_list, struct cnv_qp *qp, uint32_t sequenceID)
{
   int index;
   for (index = 0; index < CNV_MAX_WC; ++index) {
      if ((wc_list[index].state == CNVERBS_WC_POSTED_RECV) &&
          (wc_list[index].qp == qp) &&
          (wc_list[index].sequence_id == sequenceID) ){
         return &(wc_list[index]);
      }
   }

   return NULL;
}

int cnverbs_depost_by_id(struct cnv_qp *qp, uint32_t proc_id)
{
    struct cnv_cq *cq = qp->send_cq;
    struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

    int index;
    Kernel_Lock(&(cqe->wc_lock));
    for(index=0; index<CNV_MAX_WC; index++)
    {
        if((cqe->wc_list[index].state == CNVERBS_WC_POSTED_RECV) &&
           (cqe->wc_list[index].qp == qp) &&
           ((cqe->wc_list[index].proc_id & 0xff) == proc_id))
        {
            memset(&cqe->wc_list[index], 0, sizeof(struct cnverbs_wc));
            cqe->wc_list[index].state = CNVERBS_WC_INIT;
        }
    }
    Kernel_Unlock(&(cqe->wc_lock));
    return 0;
}

int cnv_open_dev(struct cnv_context **context)
{
   *context = &cnk_context;
   return 0;
}

int cnv_alloc_pd(struct cnv_pd *pd, struct cnv_context *context)
{
   // Check for valid pointers.
   if ((pd == NULL) || (context == NULL)) {
      return EFAULT;
   }

   // Look for an available protection domain.
   uint32_t handle = 0;
   for ( ; handle < CNV_MAX_PD; ++handle) {
      if (fetch(&(pdlist[handle].usecnt)) == 0) {
         break;
      }
   }

   // All of the protection domains are in use.
   if (handle == CNV_MAX_PD) {
      printf("(E) cnv_alloc_pd(): all protection domains are in use\n");
      return EAGAIN;
   }

   // Mark protection domain as in use.
   fetch_and_add(&(pdlist[handle].usecnt), 1);
   pdlist[handle].pd = pd;
   
   // Set all fields in protection domain structure.
   pd->context = context;
   pd->handle = handle;

// dumphex("protection domain", pd, sizeof(struct cnv_pd));
   TRACE( TRACE_Verbs, ("(I) cnv_alloc_pd(): allocated protection domain %u\n", pd->handle) );
   return 0;
}

int cnv_dealloc_pd(struct cnv_pd *pd)
{
   // Check for valid pointer.
   if (pd == NULL) {
      return EFAULT;
   }

   // Make sure handle is valid.
   uint32_t handle = pd->handle;
   if (handle >= CNV_MAX_PD) {
      TRACE( TRACE_Verbs, ("(E) cnv_dealloc_pd(): protection domain handle %u is invalid\n", handle) );
      return EINVAL;
   }

   // Make sure all other references to protection domain are released.
   if (fetch(&(pdlist[handle].usecnt)) != 1) {
      TRACE( TRACE_Verbs, ("(E) cnv_dealloc_pd(): protection domain %u still has %u references\n", handle, fetch(&(pdlist[handle].usecnt))) );
      return EBUSY;
   }

   // Mark protection domain as available.
   fetch_and_sub(&(pdlist[handle].usecnt), 1);
   pdlist[handle].pd = NULL;

   // Clear protection domain structure.
   memset(pd, 0, sizeof(struct cnv_pd));

   TRACE( TRACE_Verbs, ("(I) cnv_dealloc_pd(): deallocated protection domain %u\n", handle) );
   return 0;
}

int cnv_reg_mr(struct cnv_mr *mr, struct cnv_pd *pd, void *addr, size_t length, enum cnv_access_flags flags)
{
   // Check for valid pointers.
   if ((mr == NULL) || (pd == NULL) || (addr == NULL)) {
      return EFAULT;
   }

   // Look for an available memory region.
   uint32_t handle = 0;
   for ( ; handle < CNV_MAX_MR; ++handle) {
      if (fetch(&(mrlist[handle].usecnt)) == 0) {
         // Try to obtain this memory region by incrementing use count.
         uint32_t prev_usecnt = fetch_and_add(&(mrlist[handle].usecnt), 1);
         if (prev_usecnt == 0) { // Successfully obtained this memory region.
            break;
         }
         fetch_and_sub(&(mrlist[handle].usecnt), 1);
      }
   }

   // All of the memory regions are in use.
   if (handle == CNV_MAX_MR) {
      TRACE( TRACE_Verbs, ("(E) cnv_reg_mr(): all memory regions are in use\n") );
      return EAGAIN;
   }

   // Increment use count on protection domain.
   fetch_and_add(&(pdlist[pd->handle].usecnt), 1);

   // Find base physical address of the contiguous range containing memory region.
   mrlist[handle].mr = mr;
   mrlist[handle].rkey = handle + CNV_HANDLE_ADJUST;
   vmm_TranslateAddress(addr, &(mrlist[handle].base_va), &(mrlist[handle].base_pa), &(mrlist[handle].base_sz));
   TRACE( TRACE_Verbs, ("(I) cnv_reg_mr(): addr=0x%lx base_va=0x%lx base_pa=0x%lx base_sz=%lu\n",
                        (uint64_t)addr, mrlist[handle].base_va, mrlist[handle].base_pa, mrlist[handle].base_sz) );

   // Make sure the memory region fits in the contiguous range.
// pasize = base_sz - (va-base_va);

   // Set all fields in user's memory region structure.
   mr->context = pd->context;
   mr->pd = pd;
   mr->addr = addr;
   mr->length = length;
   mr->handle = handle;
   mr->lkey = mr->rkey = mrlist[handle].rkey;

   TRACE( TRACE_Verbs, ("(I) cnv_reg_mr(): registered memory region with lkey %u and rkey %u (handle %u)\n", mr->lkey, mr->rkey, handle) );
   return 0;
}

int cnv_dereg_mr(struct cnv_mr *mr)
{
   // Check for valid pointers.
   if ((mr == NULL) || (mr->pd == NULL)) {
      return EFAULT;
   }

   // Make sure handle is valid.
   uint32_t handle = mr->handle;
   if (handle >= CNV_MAX_MR) {
      TRACE( TRACE_Verbs, ("(E) cnv_dereg_mr(): memory region handle %u is invalid\n", handle) );
      return EINVAL;
   }

   // Make sure all other references to memory region are released.
   if (fetch(&(mrlist[handle].usecnt)) != 1) {
      TRACE( TRACE_Verbs, ("(I) cnv_dereg_mr(): memory region with lkey %u and rkey %u (handle %u) is still in use\n", mr->lkey, mr->rkey, handle) );
      return EBUSY;
   }

   // Decrement use count on protection domain.
   if (mr->pd->handle >= CNV_MAX_PD) {
      TRACE( TRACE_Verbs, ("(E) cnv_dereg_mr(): protection domain handle %u is invalid\n", mr->pd->handle) );
      return EINVAL;
   }
   fetch_and_sub(&(pdlist[mr->pd->handle].usecnt), 1);

   // Reset internal memory region structure.
   mrlist[handle].mr = NULL;
   mrlist[handle].base_va = 0;
   mrlist[handle].base_pa = 0;
   mrlist[handle].base_sz = 0;
   mrlist[handle].rkey = 0;

   // Clear user's memory region structure.
   memset(mr, 0, sizeof(struct cnv_mr));

   // Mark memory region as available.
   fetch_and_sub(&(mrlist[handle].usecnt), 1);

   TRACE( TRACE_Verbs, ("(I) cnv_dereg_mr(): deregistered memory region with handle %u\n", handle) );
   return 0;
}


int init_wc_linked_list(struct cnv_cq *cq){
     uint32_t handle = cq->handle;
     int i = 0;
     cqlist[handle].free_wc_list = NULL;
     cqlist[handle].send_wc_list = NULL;
     cqlist[handle].receive_wc_list = NULL;
     cqlist[handle].completed_wc_list = NULL;
     cqlist[handle].send_wc_tail = NULL;
     cqlist[handle].receive_wc_tail = NULL;
     cqlist[handle].completed_wc_tail = NULL;
     struct cnverbs_wc * wc = cqlist[handle].wc_list;
     for (i=0;i<(CNV_MAX_WC -1);i++){
        (wc+i)->next = (wc+i+1);
     }
     cqlist[handle].wc_list[CNV_MAX_WC-1].next = NULL;
     cqlist[handle].free_wc_list = wc;
    return 0;
}

int cnv_modify_cq_character(struct cnv_cq *cq, uint32_t character){
     uint32_t handle = cq->handle;
     cqlist[handle].cq_character=character;
     if (character == CNVERBS_WC_LINKEDLIST_CQ_CHAR){
       init_wc_linked_list(cq);
     }
     return 0;
}

int cnv_create_cq(struct cnv_cq *cq, struct cnv_context *context, int cqe)
{
   // Check for valid pointers.
   if ((cq == NULL) || (context == NULL)) {
      return EFAULT;
   }

   // Make sure number of elements in completion queue does not exceed maximum.
   if (cqe > CNV_MAX_WC) {
      return EINVAL;
   }

   // Look for an available completion queue.
   uint32_t handle = 0;
   for ( ; handle < CNV_MAX_CQ; ++handle) {
      if (fetch(&(cqlist[handle].usecnt)) == 0) {
         break;
      }
   }

   // All of the completon queues are in use.
   if (handle == CNV_MAX_CQ) {
      TRACE( TRACE_Verbs, ("(E) cnv_create_cq(): all completion queues are in use\n") );
      return EAGAIN;
   }

   // Mark completion queue as in use.
   fetch_and_add(&(cqlist[handle].usecnt), 1);
   cqlist[handle].cq = cq;
   cqlist[handle].cq_character = CNVERBS_DEFAULT_CQ_CHAR;
   cqlist[handle].free_wc_list = NULL;
   cqlist[handle].send_wc_list = NULL;
   cqlist[handle].receive_wc_list = NULL;
   cqlist[handle].completed_wc_list = NULL;
   cqlist[handle].send_wc_tail = NULL;
   cqlist[handle].receive_wc_tail = NULL;
   cqlist[handle].completed_wc_tail = NULL;

   // Set all fields in completion queue structure.
   cq->context = context;
   cq->handle = handle;
   cq->cqe = cqe;

   TRACE( TRACE_Verbs, ("(I) cnv_create_cq(): created completion queue %u\n", cq->handle) );
   return 0;
}

int cnv_destroy_cq(struct cnv_cq *cq)
{
   // Check for valid pointer.
   if (cq == NULL) {
      return EFAULT;
   }

   // Make sure handle is valid.
   uint32_t handle = cq->handle;
   if (handle >= CNV_MAX_CQ) {
      TRACE( TRACE_Verbs, ("(I) cnv_destroy_cq(): completion queue handle %u is invalid\n", handle) );
      return EINVAL;
   }

   // Make sure all other references to completion queue are released.
   if (fetch(&(cqlist[handle].usecnt)) != 1) {
      TRACE( TRACE_Verbs, ("(E) cnv_destroy_cq(): completion queue %u is still in use\n", handle) );
      return EBUSY;
   }

   // Mark completion queue as available.
   fetch_and_sub(&(cqlist[handle].usecnt), 1);
   cqlist[handle].cq = NULL;

   // Clear completion queue structure.
   memset(cq, 0, sizeof(struct cnv_cq));

   TRACE( TRACE_Verbs, ("(I) cnv_destroy_cq(): destroyed completion queue %u\n", handle) );
   return 0;
}

int cnv_create_qp(struct cnv_qp *qp, struct cnv_pd *pd, struct cnv_qp_init_attr *attr)
{
   // Check for valid pointers.
   if ((qp == NULL) || (pd == NULL) || (attr == NULL)) {
      return EFAULT;
   }

   // Look for an available queue pair.
   uint32_t handle = 0;
   for ( ; handle < CNV_MAX_QP; ++handle) {
      if (fetch(&(qplist[handle].usecnt)) == 0) {
         break;
      }
   }

   // All of the completon queues are in use.
   if (handle == CNV_MAX_QP) {
      TRACE( TRACE_Verbs, ("(E) cnv_create_qp(): all queue pairs are in use\n") );
      return EBUSY;
   }

   // Mark completion queue as in use.
   fetch_and_add(&(qplist[handle].usecnt), 1);
   qplist[handle].qp = qp;
   qplist[handle].conn_qp_num = 0;
   qplist[handle].psn = 0;
   qplist[handle].port_num = CNV_BASE_PORT + handle;

   // Increment use count on protection domain.
   fetch_and_add(&(pdlist[pd->handle].usecnt), 1);

   // Set all fields in queue pair structure.
   qp->context = pd->context;
   qp->pd = pd;
   qp->send_cq = attr->send_cq;
   qp->recv_cq = attr->recv_cq;
   qp->handle = handle;
   qp->qp_num = handle + CNV_HANDLE_ADJUST;
   qp->state = CNV_QPS_INIT;

// dumphex("queue pair", qp, sizeof(struct cnv_qp));
   TRACE( TRACE_Verbs, ("(I) cnv_create_qp(): created queue pair %u (handle %u)\n", qp->qp_num, handle) );
   return 0;
}

int cnv_recycle_qp(struct cnv_qp *qp)
{
   // Check for valid pointers.
   if ((qp == NULL) || (qp->pd == NULL)) {
      return EFAULT;
   }

   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      TRACE( TRACE_Verbs, ("(E) cnv_destroy_qp(): queue pair handle %u is invalid\n", handle) );
      return EINVAL;
   }
   
   // Disconnect from the I/O node if connected
   if (qplist[qp->handle].conn_context){
       mudm_disconnect(qplist[qp->handle].conn_context);
       qplist[handle].conn_context = NULL;
   }
   
   fetch_and_and(&(qplist[handle].connected), 0);
   qplist[handle].psn = 0;
   qp->state = CNV_QPS_INIT; 

   //reset work requests and completions on the the unified queue array
   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

   Kernel_Lock(&(cqe->wc_lock));
   memset(cqe->wc_list, 0, CNV_MAX_WC * sizeof(struct cnverbs_wc) );
   init_wc_linked_list(cq);

   Kernel_Unlock(&(cqe->wc_lock));

   return 0;
}
int cnv_destroy_qp(struct cnv_qp *qp)
{
   // Check for valid pointers.
   if ((qp == NULL) || (qp->pd == NULL)) {
      return EFAULT;
   }

   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      TRACE( TRACE_Verbs, ("(E) cnv_destroy_qp(): queue pair handle %u is invalid\n", handle) );
      return EINVAL;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_SQD) {
      TRACE( TRACE_Verbs, ("(E) cnv_destroy_qp(): queue pair state %u is invalid for queue pair %u\n", qp->state, qp->qp_num) );
      return EINVAL;
   }

   // Make sure all other references to queue pair are released.
   if (fetch(&(qplist[handle].usecnt)) != 1) {
      TRACE( TRACE_Verbs, ("(E) cnv_destroy_qp(): queue pair %u is still in use\n", handle) );
      return EBUSY;
   }

   // Disconnect from the I/O node.
   if (qplist[qp->handle].conn_context){
       mudm_disconnect(qplist[qp->handle].conn_context);
#if 0
       qplist[handle].conn_context = NULL;
#endif
   }

//race conditions exist where freeing or zeroing or setting to NULL storage causes exceptions
//For example, if the job processing hardware thread 66 is processing a signal and the shutdown thread 65 is running this code, a valid pointer 
//will become NULL in 66 and result in an exception and RAS.
#if 0
   // Decrement use count on protection domain.
   if (qp->pd->handle >= CNV_MAX_PD) {
      TRACE( TRACE_Verbs, ("(E) cnv_destroy_qp(): protection domain handle %u is invalid\n", qp->pd->handle) );
      return EINVAL;
   }
   fetch_and_sub(&(pdlist[qp->pd->handle].usecnt), 1);

   // Mark queue pair as available.
   fetch_and_sub(&(qplist[handle].usecnt), 1);
   fetch_and_and(&(qplist[handle].connected), 0);
   qplist[handle].qp = NULL;
   qplist[handle].conn_qp_num = 0;
   qplist[handle].psn = 0;
   qplist[handle].port_num = 0;

   // Clear queue pair structure.
   memset(qp, 0,  sizeof(struct cnv_qp));
   qp->state = CNV_QPS_RESET;
#endif

   TRACE( TRACE_Verbs, ("(I) cnv_destroy_qp(): destroyed queue pair %u\n", handle) );
   return 0;
}

//! \todo Should there be a source_addr parameter?

int cnv_connect(struct cnv_qp *qp, struct sockaddr *remote_addr)
{
   int rc;
   uint64_t endtime;
   struct cnv_context *context = qp->context;  // fix
   struct ionet_connect conn_request; // This parameter is deprecated.

   // Check for valid pointers.
   if ((qp == NULL) || (remote_addr == NULL)) {
      return EFAULT;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_INIT) {
      TRACE( TRACE_Verbs, ("(E) cnv_connect(): queue pair state %u is invalid for queue pair %u\n", qp->state, qp->qp_num) );
      return EINVAL;
   }

   // Initialize the connect request.
   Personality_t *pers = GetPersonality();
   memset(&conn_request, 0, sizeof(conn_request));
   conn_request.ionet_hdr.source_qpn = qp->qp_num;
   conn_request.source_port = qplist[qp->handle].port_num;
   memcpy(&(conn_request.source_IP_addr), NodeState.torusIpAddress.octet, sizeof(conn_request.source_IP_addr));
   if (remote_addr->sa_family == AF_INET) {
      struct sockaddr_in *dest = (struct sockaddr_in *)remote_addr;
      conn_request.dest_port = dest->sin_port;
      memcpy(&(conn_request.dest_IP_addr), &(dest->sin_addr), sizeof(struct in_addr)); 
   }
   else {
      TRACE( TRACE_Verbs, ("(E) cnv_connect(): remote address family %d is invalid\n", remote_addr->sa_family) );
      return EINVAL;
   }
// dumphex("connect request", &conn_request, sizeof(struct ionet_connect));

   TRACE( TRACE_Verbs, ("(I) cnv_connect(): bridge compute node is at coordinates %d %d %d %d %d\n", pers->Network_Config.cnBridge_A,
                        pers->Network_Config.cnBridge_B, pers->Network_Config.cnBridge_C, pers->Network_Config.cnBridge_D, pers->Network_Config.cnBridge_E) );
   TRACE( TRACE_Verbs, ("(I) cnv_connect(): my compute node is at coordinates %d %d %d %d %d\n", pers->Network_Config.Acoord,
                        pers->Network_Config.Bcoord, pers->Network_Config.Ccoord, pers->Network_Config.Dcoord, pers->Network_Config.Ecoord) );

   // Connect to the I/O node.
   TRACE( TRACE_Verbs, ("(I) cnv_connect(): connecting from port %u to port %u on queue pair %u\n",
                        conn_request.source_port, conn_request.dest_port, qp->qp_num) );
   rc = mudm_connect(context->mudmcontext, &(qplist[qp->handle].conn_context), MUDM_CN2IO_NODE, &conn_request, NULL, 0);
   if (rc != 0) {
      printf("(E) cnv_connect(): mudm_connect failed with rc=%d\n", rc);
      return rc;
   }
   
   endtime = GetTimeBase() + 1000000ull * CONFIG_MUDMCONNECTTIMEOUT * GetPersonality()->Kernel_Config.FreqMHz;
   // Spin until connect is completed.
   while ((fetch(&(qplist[qp->handle].connected)) == 0) && (GetTimeBase() < endtime)) {
      Delay(1000);
   }
   if(GetTimeBase() > endtime)
       rc = ETIMEDOUT;
   
   TRACE( TRACE_Verbs, ("(I) cnv_connect(): queue pair %u (handle %u) is connected\n", qp->qp_num, qp->handle) );
   return rc;
}

int cnv_disconnect(struct cnv_qp *qp)
{
   // Check for valid pointers.
   if (qp == NULL) {
      return EFAULT;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      TRACE( TRACE_Verbs, ("(E) cnv_disconnect(): queue pair state %u is invalid\n", qp->state) );
      return EINVAL;
   }

   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      TRACE( TRACE_Verbs, ("(E) cnv_disconnect(): queue pair handle %u is invalid\n", handle) );
      return EINVAL;
   }

   // Drain the send queue.
   qp->state = CNV_QPS_SQD;

   //! \todo What else should be done here?

   TRACE( TRACE_Verbs, ("(I) cnv_disconnect(): queue pair %u (handle %u) is disconnected\n", qp->qp_num, qp->handle) );
   return 0;
}
int cnv_post_send(struct cnv_qp *qp, struct cnv_send_wr *wr_list, struct cnv_send_wr **bad_wr){
  return cnv_post_send_seqID(qp,wr_list,bad_wr,0);
}

int cnv_post_send_seqID(struct cnv_qp *qp, struct cnv_send_wr *wr_list, struct cnv_send_wr **bad_wr,uint32_t seqID)
{
   // Check for valid pointers.
   if ((qp == NULL) || (wr_list == NULL) || (bad_wr == NULL)) {
      return EFAULT;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_send(): queue pair state %u is invalid\n", qp->state) );
      *bad_wr = wr_list;
      return EINVAL;
   }

   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_send(): queue pair handle %u is invalid\n", handle) );
      *bad_wr = wr_list;
      return EINVAL;
   }

   // Structure describing send.
   struct ionet_send_remote_payload payload;
   int rc = 0;

   // This is very simple.  Only handle one work request.
   struct cnv_send_wr *wr = wr_list;
   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

#if 0
   // The only operations that are supported are CNV_WR_SEND and CNV_SEND_WITH_IMM.
   if ((wr->opcode != CNV_WR_SEND) && (wr->opcode != CNV_WR_SEND_WITH_IMM)) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_send(): opcode %u is invalid\n", wr->opcode) );
      *bad_wr = wr;
      return EINVAL;
   }
#endif 

   // Lock the list of work completions in the internal completion queue.
   Kernel_Lock(&(cqe->wc_lock));

   // Find a free work completion from the list.
   struct cnverbs_wc *wce = cnverbs_find_free_wc(cqe);

   // Start filling out work completion.
   wce->wc.opcode = CNV_WC_SEND;
   wce->wc.vendor_err = 0;
   wce->wc.qp_num = qp->qp_num;
   wce->wc.src_qp = qplist[handle].conn_qp_num;
   wce->cq = cq;
   wce->qp = qp;
   wce->state = CNVERBS_WC_POSTED_SEND;
   wce->proc_id = ProcessorID();
   wce->num_recv_sge = 0;

   // Unlock the list of work completions in the internal completion queue.
   Kernel_Unlock(&(cqe->wc_lock));

   // Convert external sge with virtual addresses to mudm sge with physical addresses
   uint32_t index = 0;
   payload.RequestID = (uint64_t)wce;
   payload.sgl_num = wr->num_sge;
   payload.data_length = 0;
   payload.flags = 0;
   if (wr->opcode == CNV_WR_SEND_WITH_IMM) {
      payload.immediate_data = wr->imm_data;
      payload.flags |= MUDM_IMMEDIATE_DATA;
   }
   else {
      payload.immediate_data = 0;
   }
   payload.reserved = 0;
   for ( ; index < wr->num_sge; ++index) {
      uint32_t lkey = wr->sg_list[index].lkey - CNV_HANDLE_ADJUST; //! \todo Validate lkey?  Make sure length doesn't exceed memory region?
      payload.SGE[index].physicalAddr = (wr->sg_list[index].addr - mrlist[lkey].base_va) + mrlist[lkey].base_pa;
      payload.SGE[index].memlength = wr->sg_list[index].length;
      payload.data_length += wr->sg_list[index].length;
      TRACE( TRACE_Verbs, ("(I) cnv_post_send(): SGE %u: vaddr=0x%lx paddr=0x%lx length=%d\n",
                           index, wr->sg_list[index].addr, (uint64_t)payload.SGE[index].physicalAddr, payload.SGE[index].memlength) );
   }
   

   if (wr->opcode == CNV_WR_RDMA_WRITE){
      payload.remote_vaddr = wr->remote_addr; /* remote virtual Address                     */
      payload.rkey = wr->rkey;  //remote key 
      wce->wc.byte_len = payload.data_length;
      rc = mudm_send_message_remote(qplist[handle].conn_context, MUDM_RDMA_READ, &payload);
   }
   // If possible, send the data in an immediate packet.
   else if ((wr->num_sge == 1) && (wr->sg_list[0].length <= MUDM_MAX_PAYLOAD_SIZE) && (wr->opcode == CNV_WR_SEND)) {
      wce->wc.byte_len = payload.SGE[0].memlength;
      uint64_t physicalAddr = payload.SGE[0].physicalAddr; 
      rc = mudm_send_pkt(qplist[handle].conn_context, wce, MUDM_PKT_DATA_IMM, (void *)physicalAddr, payload.SGE[0].memlength);
   }
   
   else {
      wce->wc.byte_len = payload.data_length;
      rc = mudm_send_message_remote(qplist[handle].conn_context, MUDM_PKT_DATA_REMOTE, &payload);
   }
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVPSTSND, qp->qp_num, (uint64_t)wce, cq->handle, rc);
   
   // Handle the work completion based on the return code from the MUDM operation.
   switch (rc) {
      case 0:
      {
         // Complete the work request right now.
         enum cnverbs_wc_state prev_state = wce->state;
         wce->wc.status = CNV_WC_SUCCESS;
         wce->state = CNVERBS_WC_READY;
         
         ppc_msync();
         // Post to completion queue.
         uint64_t prev_num_wc = L2_AtomicLoadIncrement(&CQ_NUM_WC[cq->handle]);
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVADDWCE, cq->handle, prev_num_wc, wce->wc.status, prev_state);
         TRACE( TRACE_Verbs, ("(I) cnv_post_send(): posted completion queue %u (num_wc %ld)\n", cq->handle, prev_num_wc));
         break;
      }

      case -EINPROGRESS:
         TRACE( TRACE_Verbs, ("(I) cnv_post_send(): operation is in progress ...\n") );
         break;

      case -EBUSY:
         // For now log this special.
         printf("(E) cnv_post_send(): CNV_WR_SEND failed because MUDM was busy for queue pair %u\n", qp->qp_num);
         *bad_wr = wr;
         return rc;

      default:
         TRACE( TRACE_Verbs, ("(E) cnv_post_send(): CNV_WR_SEND failed to post send queue for queue pair %u, rc=%d\n", qp->qp_num, rc) );
         *bad_wr = wr;
         return rc;
   }

   // Only process one work request.
   if (wr->next != NULL) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_send(): more than %d work requests in list\n", CNV_MAX_WR) );
      *bad_wr = wr->next;
      return EINVAL;
   }

   TRACE( TRACE_Verbs, ("(I) cnv_post_send(): posted send to queue pair %u using completion queue %u\n", qp->qp_num, cq->handle) );
   return 0;
}

int cnv_get_completions_linked_list(struct cnv_qp *qp, unsigned long int num_entries, Kernel_RDMAWorkCompletion_t* WorkCompletionList){

   // Check for valid pointers.
   if ((qp == NULL) || (WorkCompletionList == NULL)) {
      return -EFAULT;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      return -EINVAL;
   }

   if (num_entries == 0) return 0;

   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);
   
   ppc_msync();

   // Lock the list of work completions in the internal completion queue.
   Kernel_Lock(&(cqe->wc_lock));
   if (cqe->completed_wc_list == NULL){
     // Unlock the list of work completions in the internal completion queue.
     Kernel_Unlock(&(cqe->wc_lock));
     return 0;
   }
    Kernel_RDMAWorkCompletion_t* curRWC = WorkCompletionList;
    struct cnverbs_wc * cur_wc = cqe->completed_wc_list;
    struct cnverbs_wc * first_wc = cqe->completed_wc_list;
    if (cur_wc->state != CNVERBS_WC_READY){
     Kernel_Unlock(&(cqe->wc_lock));
     return 0;
    }
    int i = 0;
    int num_completions = 0;
    for (i=0;i<num_entries;i++){
      struct cnv_wc * wc_user = &cur_wc->wc;
      curRWC->buf = (void *)wc_user->wr_id;
      curRWC->len = wc_user->byte_len;
      curRWC->opcode = wc_user->opcode;
      curRWC->status = wc_user->status;
      curRWC->flags  = wc_user->wc_flags;
      curRWC->reserved = 0;
      cur_wc->state = CNVERBS_WC_INIT; //previous state was READY
      curRWC++;
      num_completions++;
      if (cur_wc ->next == NULL) break;
      if (cur_wc->next->state != CNVERBS_WC_READY) break; 
      cur_wc = cur_wc ->next;
    }
    if (num_completions){
      cqe->completed_wc_list = cur_wc ->next;
      cur_wc ->next =  cqe->free_wc_list;
      cqe->free_wc_list = first_wc;
      if (cqe->completed_wc_list==NULL)cqe->completed_wc_tail=NULL;
    }
    Kernel_Unlock(&(cqe->wc_lock));
    return num_completions;
}
#if 0
typedef
struct Kernel_RDMAWorkCompletion {
  void*    buf;    /* buffer address of data for completed operation */
  uint32_t len;    /* length of data for completed operation         */
  uint32_t opcode; /* opcode receive=1  send=2                       */
  uint32_t status; /* =0, successful, >0 errno                       */
  uint32_t flags;    /* reserved for future use                      */
  uint64_t reserved; /* reserved for future use                      */
} Kernel_RDMAWorkCompletion_t;

//! Work completion.
struct cnv_wc
{
   uint64_t             wr_id;         //!< User defined work request id.
   enum cnv_wc_status   status;        //!< Status of completed work request.
   enum cnv_wc_opcode   opcode;        //!< Operation type of completed work request.
   uint32_t             vendor_err;    //!< Vendor error syndrome value. 
   uint32_t             byte_len;      //!< Number of bytes transferred.
   uint32_t             imm_data;      //!< Immediate data value.
   uint32_t             qp_num;        //!< Local queue pair number of completed work request.
   uint32_t             src_qp;        //!< Source (remote) queue pair number of completed work request.
   enum cnv_wc_flags    wc_flags;      //!< Flags for the completed work request.
};
#endif 


int cnv_post_send_linked_list(struct cnv_qp *qp, struct cnv_send_wr *wr_list)
{
   // Check for valid pointers.
   if ((qp == NULL) || (wr_list == NULL)) {
      return EFAULT;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      return EINVAL;
   }

   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      return EINVAL;
   }

   // Structure describing send.
   struct ionet_send_remote_payload payload;
   int rc = 0;

   // This is very simple.  Only handle one work request.
   struct cnv_send_wr *wr = wr_list;
   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

   // Lock the list of work completions in the internal completion queue.
   Kernel_Lock(&(cqe->wc_lock));

   // Find a free work completion from the linked list
   struct cnverbs_wc *wce = cqe->free_wc_list;
   if (wce==NULL){
      Kernel_Unlock(&(cqe->wc_lock));
      return ENOMEM;
   }
   cqe->free_wc_list = wce->next;
   wce->next = NULL;

   // Start filling out work completion.
   wce->wc.opcode = CNV_WC_SEND;
   wce->wc.vendor_err = 0;
   wce->wc.qp_num = qp->qp_num;
   wce->wc.src_qp = qplist[handle].conn_qp_num;
   wce->cq = cq;
   wce->qp = qp;
   wce->state = CNVERBS_WC_POSTED_SEND;
   wce->proc_id = ProcessorID();
   wce->num_recv_sge = 0;

   // Unlock the list of work completions in the internal completion queue.
   Kernel_Unlock(&(cqe->wc_lock));

   // Convert external sge with virtual addresses to mudm sge with physical addresses
   uint32_t index = 0;
   payload.RequestID = (uint64_t)wce;
   payload.sgl_num = wr->num_sge;
   payload.data_length = 0;
   payload.flags = 0;
   payload.immediate_data = 0;
   payload.reserved = 0;

   for ( ; index < wr->num_sge; ++index) {
      uint32_t lkey = wr->sg_list[index].lkey - CNV_HANDLE_ADJUST; //! \todo Validate lkey?  Make sure length doesn't exceed memory region?
      payload.SGE[index].physicalAddr = (wr->sg_list[index].addr - mrlist[lkey].base_va) + mrlist[lkey].base_pa;
      payload.SGE[index].memlength = wr->sg_list[index].length;
      payload.data_length += wr->sg_list[index].length;
      TRACE( TRACE_Verbs, ("(I) cnv_post_send(): SGE %u: vaddr=0x%lx paddr=0x%lx length=%d\n",
                           index, wr->sg_list[index].addr, (uint64_t)payload.SGE[index].physicalAddr, payload.SGE[index].memlength) );
   }
   

   if ((wr->num_sge == 1) && (wr->sg_list[0].length <= MUDM_MAX_PAYLOAD_SIZE) && (wr->opcode == CNV_WR_SEND)) {
      wce->wc.byte_len = payload.SGE[0].memlength;
      uint64_t physicalAddr = payload.SGE[0].physicalAddr; 
      rc = mudm_send_pkt(qplist[handle].conn_context, wce, MUDM_PKT_DATA_IMM, (void *)physicalAddr, payload.SGE[0].memlength);
   }
   
   else {
      wce->wc.byte_len = payload.data_length;
      rc = mudm_send_message_remote(qplist[handle].conn_context, MUDM_PKT_DATA_REMOTE, &payload);
   }
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVPSTSND, qp->qp_num, (uint64_t)wce, cq->handle, rc);
   
   // Handle the work completion based on the return code from the MUDM operation.
   switch (rc) {
      case 0:
      {
         // Complete the work request right now.
         enum cnverbs_wc_state prev_state = wce->state;
         wce->wc.status = CNV_WC_SUCCESS;
         wce->state = CNVERBS_WC_READY;

         // Post to completion queue.  
         if (cqe->completed_wc_tail)
             cqe->completed_wc_tail->next = wce;
         else
             cqe->completed_wc_list = wce;
         cqe->completed_wc_tail = wce;
         
         ppc_msync();
        
         uint64_t prev_num_wc = L2_AtomicLoadIncrement(&CQ_NUM_WC[cq->handle]);
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVADDWCE, cq->handle, prev_num_wc, wce->wc.status, prev_state);
         break;
      }

      case -EINPROGRESS:
         // Post to completion queue.  
         if (cqe->completed_wc_tail)
            cqe->completed_wc_tail->next = wce;
            cqe->completed_wc_list       = wce;
         cqe->completed_wc_tail = wce;
         
         ppc_msync();
         break;
      case -EBUSY:
         // on send queue to send later.  
         //! \todo TODO Flightlog here, add code to handle when completions do happen
         if (cqe->receive_wc_tail){
            cqe->receive_wc_tail->next = wce;
         }
         cqe->receive_wc_tail = wce;
         break; 
      default:
         return rc;
   }

   return 0;
}

int cnv_post_send_no_comp(struct cnv_qp *qp, struct cnv_send_wr *wr_list)
{
   // Check for valid pointers.
   if ((qp == NULL) || (wr_list == NULL)) {
      return EFAULT;
   }
   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      return EINVAL;
   }
   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      return EINVAL;
   }

   // Structure describing send.
   struct ionet_send_remote_payload payload;
   int rc = 0;

   // This is very simple.  Only handle one work request.
   struct cnv_send_wr *wr = wr_list;

   // ignore using completion queue,  Kernel_Lock(&(cqe->wc_lock));

   // Convert external sge with virtual addresses to mudm sge with physical addresses
   uint32_t index = 0;
   payload.RequestID = 0;  //no status callback
   payload.sgl_num = wr->num_sge;
   payload.data_length = 0;
   payload.flags = 0;
   if (wr->opcode == CNV_WR_SEND_WITH_IMM) {
      payload.immediate_data = wr->imm_data;
      payload.flags |= MUDM_IMMEDIATE_DATA;
   }
   else {
      payload.immediate_data = 0;
   }
   payload.reserved = 0;
   for ( ; index < wr->num_sge; ++index) {
      uint32_t lkey = wr->sg_list[index].lkey - CNV_HANDLE_ADJUST; //! \todo Validate lkey?  Make sure length doesn't exceed memory region?
      payload.SGE[index].physicalAddr = (wr->sg_list[index].addr - mrlist[lkey].base_va) + mrlist[lkey].base_pa;
      payload.SGE[index].memlength = wr->sg_list[index].length;
      payload.data_length += wr->sg_list[index].length;
      TRACE( TRACE_Verbs, ("(I) cnv_post_send(): SGE %u: vaddr=0x%lx paddr=0x%lx length=%d\n",
                           index, wr->sg_list[index].addr, (uint64_t)payload.SGE[index].physicalAddr, payload.SGE[index].memlength) );
   }
   
   if ((wr->num_sge == 1) && (wr->sg_list[0].length <= MUDM_MAX_PAYLOAD_SIZE) && (wr->opcode == CNV_WR_SEND)) {
      uint64_t physicalAddr = payload.SGE[0].physicalAddr; 
      rc = mudm_send_pkt(qplist[handle].conn_context, NULL, MUDM_PKT_DATA_IMM, (void *)physicalAddr, payload.SGE[0].memlength);
   }
   
   else {
      rc = mudm_send_message_remote(qplist[handle].conn_context, MUDM_PKT_DATA_REMOTE, &payload);
   }
   
   if (rc== (-EINPROGRESS) ) return 0;
   return rc;
}

int cnv_post_recv(struct cnv_qp *qp, struct cnv_recv_wr *wr_list, struct cnv_recv_wr **bad_wr)
{
  return cnv_post_recv_proc(qp, wr_list, bad_wr, ProcessorID());
}

int cnv_post_recv_proc(struct cnv_qp *qp, struct cnv_recv_wr *wr_list, struct cnv_recv_wr **bad_wr, uint32_t proc_id)
{
   // Check for valid pointers.
   if ((qp == NULL) || (wr_list == NULL) || (bad_wr == NULL)) {
      return EFAULT;
   }

   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_recv(): queue pair state %u is invalid\n", qp->state) );
      *bad_wr = wr_list;
      return EINVAL;
   }

   // Make sure handle is valid.
   uint32_t handle = qp->handle;
   if (handle >= CNV_MAX_QP) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_recv(): queue pair handle %u is invalid\n", handle) );
      *bad_wr = wr_list;
      return EINVAL;
   }

   // This is very simple.  Only handle one work request.
   struct cnv_recv_wr *wr = wr_list;
   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

   // Make sure number of elements in sge list is valid.
   if (wr->num_sge > CNV_MAX_SGE) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_recv(): too many elements in sge list (%d > %d)\n", wr->num_sge, CNV_MAX_SGE) );
      return EINVAL;
   }

   // Lock the list of work completions in the internal completion queue.
   Kernel_Lock(&(cqe->wc_lock));

   // Find a free work completion from the list.
   struct cnverbs_wc *wce = cnverbs_find_free_wc(cqe);

   // Setup work completion.
   wce->wc.wr_id = wr->wr_id;
   wce->wc.opcode = CNV_WC_RECV;
   wce->wc.vendor_err = 0;
   wce->wc.qp_num = qp->qp_num;
   wce->cq = cq;
   wce->qp = qp;
   wce->state = CNVERBS_WC_POSTED_RECV;
   wce->proc_id = proc_id;
   wce->sequence_id = 0;

   // Unlock the list of work completions in the internal completion queue.
   Kernel_Unlock(&(cqe->wc_lock));

   // Copy scatter/gather element list to internal work completion.
   memcpy(&(wce->recv_sge), wr->sg_list, (sizeof(struct cnv_sge) * wr->num_sge));
   wce->num_recv_sge = wr->num_sge;
   int index;
   for (index = 0; index < wce->num_recv_sge; ++index) {
      TRACE( TRACE_Verbs, ("(I) cnv_post_recv(): SGE %u: vaddr=0x%lx length=%d\n",
                           index, wce->recv_sge[index].addr, wce->recv_sge[index].length) );
   }
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVPSTRCV, qp->qp_num, (uint64_t)wce, cq->handle, wr->wr_id);

   // Only process one work request.
   if (wr->next != NULL) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_recv(): more than %d work requests in list\n", CNV_MAX_WR) );
      *bad_wr = wr->next;
      return EINVAL;
   }

   TRACE( TRACE_Verbs, ("(I) cnv_post_recv(): posted recv from work request at address 0x%p for queue pair %u\n", wr, qp->qp_num) );
   return 0;
}


int cnv_post_recv_seqID(struct cnv_qp *qp, struct cnv_recv_wr *wr_list, struct cnv_recv_wr **bad_wr, uint32_t seq_ID)
{
   // assume pointers are nonNULL
   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      *bad_wr = wr_list;
      return EINVAL;
   }

   // This is very simple.  Only handle one work request.
   struct cnv_recv_wr *wr = wr_list;
   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

   // Lock the list of work completions in the internal completion queue.
   Kernel_Lock(&(cqe->wc_lock));

   // Find a free work completion from the list.
   struct cnverbs_wc *wce = cnverbs_find_free_wc(cqe);

   // Setup work completion.
   wce->wc.wr_id = wr->wr_id;
   wce->wc.opcode = CNV_WC_RECV;
   wce->wc.vendor_err = 0;
   wce->wc.qp_num = qp->qp_num;
   wce->cq = cq;
   wce->qp = qp;
   wce->state = CNVERBS_WC_POSTED_RECV;
   wce->proc_id = ProcessorID();
   wce->sequence_id = seq_ID;

   // Unlock the list of work completions in the internal completion queue.
   Kernel_Unlock(&(cqe->wc_lock));

   // Copy scatter/gather element list to internal work completion.
   memcpy(&(wce->recv_sge), wr->sg_list, (sizeof(struct cnv_sge) * wr->num_sge));
   wce->num_recv_sge = wr->num_sge;
   int index;
   for (index = 0; index < wce->num_recv_sge; ++index) {
      TRACE( TRACE_Verbs, ("(I) cnv_post_recv(): SGE %u: vaddr=0x%lx length=%d\n",
                           index, wce->recv_sge[index].addr, wce->recv_sge[index].length) );
   }
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVPSTRCV, qp->qp_num, (uint64_t)wce, cq->handle, wr->wr_id);

   // Only process one work request.
   if (wr->next != NULL) {
      TRACE( TRACE_Verbs, ("(E) cnv_post_recv(): more than %d work requests in list\n", CNV_MAX_WR) );
      *bad_wr = wr->next;
      return EINVAL;
   }

   TRACE( TRACE_Verbs, ("(I) cnv_post_recv(): posted recv from work request at address 0x%p for queue pair %u\n", wr, qp->qp_num) );
   return 0;
}

int cnv_post_recv_linked_list(struct cnv_qp *qp, struct cnv_recv_wr *wr_list)
{
   // assume pointers are nonNULL
   // Make sure queue pair is in correct state.
   if (qp->state != CNV_QPS_RTS) {
      return EINVAL;
   }

   // This is very simple.  Only handle one work request.
   struct cnv_recv_wr *wr = wr_list;
   struct cnv_cq *cq = qp->send_cq;
   struct cnverbs_cq *cqe = &(cqlist[cq->handle]);

   // Lock the list of work completions in the internal completion queue.
   Kernel_Lock(&(cqe->wc_lock));

   // Find a free work completion from the linked list
   struct cnverbs_wc *wce = cqe->free_wc_list;
   if (wce==NULL){
      Kernel_Unlock(&(cqe->wc_lock));
      return ENOMEM;
   }
   cqe->free_wc_list = wce->next;
   wce->next = NULL;
   if (cqe->receive_wc_tail){
      cqe->receive_wc_tail->next = wce;
      cqe->receive_wc_tail = wce;
   }
   else {
     cqe->receive_wc_tail = wce;
     cqe->receive_wc_list = wce;
   }
   

   // Setup work completion.
   wce->wc.wr_id = wr->wr_id;
   wce->wc.opcode = CNV_WC_RECV;
   wce->wc.vendor_err = 0;
   wce->wc.qp_num = qp->qp_num;
   wce->cq = cq;
   wce->qp = qp;
   wce->state = CNVERBS_WC_POSTED_RECV;
   wce->proc_id = ProcessorID();
   wce->sequence_id = 0;

   // Unlock the list of work completions in the internal completion queue.
   Kernel_Unlock(&(cqe->wc_lock));

   // Copy scatter/gather element list to internal work completion.
   memcpy(&(wce->recv_sge), wr->sg_list, (sizeof(struct cnv_sge) * wr->num_sge));
   wce->num_recv_sge = wr->num_sge;

   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVPSTRCV, qp->qp_num, (uint64_t)wce, cq->handle, wr->wr_id);

   return 0;
}

int cnv_poll_cq(struct cnv_cq *cq, int num_entries, struct cnv_wc *wc, int *num_returned, uint32_t proc_id)
{
   // Check for valid pointers.
   if ((cq == NULL) || (wc == NULL) || (num_returned == NULL)) {
      return EFAULT;
   }
   
   // Default to no work completions returned.
   *num_returned = 0;

   // Make sure there is at least one entry in the list.
   if (num_entries <= 0) {
      return EINVAL;
   }

   // Make sure handle is valid.
   uint32_t handle = cq->handle;
   if (handle >= CNV_MAX_CQ) {
      TRACE( TRACE_Verbs, ("(E) cnv_poll_cq(): completion queue handle %u is invalid\n", handle) );
      return EINVAL;
   }

   // Spin until a work completion is ready.
   struct cnverbs_cq *cqe = &(cqlist[handle]);
   uint64_t ready;
   while((ready = L2_AtomicLoad(&CQ_NUM_WC[handle])) == 0)
   {
      Delay(100);
   }
   
   Kernel_Lock(&(cqe->wc_lock));

   // Find the work completions that are ready and copy them to caller's storage.
   uint32_t index = 0;
   while (index < CNV_MAX_WC && ready > *num_returned) {
      struct cnverbs_wc *wce = &(cqe->wc_list[index]);
      
      if ((wce->state == CNVERBS_WC_READY) && (proc_id == CNV_ANY_PROC_ID || proc_id == wce->proc_id)) {
         ppc_msync();  //  msync needed here to ensure consistent wc/storage prior to memcpy.
         memcpy(&(wc[*num_returned]), &(wce->wc), sizeof(struct cnv_wc));
         *num_returned += 1;
         uint64_t prev_num_wc = L2_AtomicLoadDecrement(&CQ_NUM_WC[handle]);
         Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVRMVWCE, (uint64_t)wce, proc_id, cq->handle, prev_num_wc-1);
         memset(wce, '\0', sizeof(struct cnverbs_wc));
         wce->state = CNVERBS_WC_INIT;
      }
      ++index;
   }      
   
   Kernel_Unlock(&(cqe->wc_lock));

   if (*num_returned > 0) {
       Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVPOLLCQ, *num_returned, index, handle, L2_AtomicLoad(&CQ_NUM_WC[handle]));
   }
   TRACE( TRACE_Verbs, ("(I) cnv_poll_cq(): %d work completions are ready for completion queue %u (num_wc %ld)\n",
                        *num_returned, handle, L2_AtomicLoad(&CQ_NUM_WC[handle])));
   return 0;
}

int cnv_poll_cq_for_single_recv(struct cnv_cq *cq, uint32_t proc_id)
{

   // Make sure handle is valid.
   uint32_t handle = cq->handle;

   // Spin until a work completion is ready.
   struct cnverbs_cq *cqe = &(cqlist[handle]);
   uint64_t ready;
   uint32_t index = 0;
   struct cnverbs_wc *wce = NULL;
   while (index < CNV_MAX_WC ) {
      wce = &(cqe->wc_list[index]);     
      if ( (wce->state != CNVERBS_WC_INIT) && (proc_id == wce->proc_id) ) {
         break;
      }
      ++index;
   }  
   while((ready = L2_AtomicLoad(&CQ_NUM_WC[handle])) == 0)
   { }
   
   Kernel_Lock(&(cqe->wc_lock));

   // Find the work completions that are ready and copy them to caller's storage.

   ppc_msync();  //  msync needed here to ensure consistent wc/storage prior to memcpy.
   uint64_t prev_num_wc = L2_AtomicLoadDecrement(&CQ_NUM_WC[handle]);
   Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVRMVWCE, (uint64_t)wce, proc_id, cq->handle, prev_num_wc-1);
   memset(wce, '\0', sizeof(struct cnverbs_wc));
   wce->state = CNVERBS_WC_INIT;

   Kernel_Unlock(&(cqe->wc_lock));


   return 0;
}

#define BYTES_PER_LINE 32

void dumphex(const char *identifier, void *ptr, int len)
{
   char remainder[BYTES_PER_LINE];
   uint32_t *data = (uint32_t *)ptr;
   uint32_t offset = 0;
   printf("%s (at address 0x%p for 0x%x (%d) bytes)\n", identifier, data, len, len);
   while (len >= BYTES_PER_LINE) {
      printf("%04x: %08x %08x %08x %08x  %08x %08x %08x %08x\n", offset, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
      len -= BYTES_PER_LINE;
      offset += BYTES_PER_LINE;
      data += BYTES_PER_LINE / sizeof(*data);
   }
   if (len > 0) {
      memset(remainder, 0, sizeof(remainder));
      memcpy(remainder, data, len);
      data = (uint32_t *)remainder;
      printf("%04x: %08x %08x %08x %08x  %08x %08x %08x %08x\n", offset, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
   }

   return;
}

