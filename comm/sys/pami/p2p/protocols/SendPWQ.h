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
/// \file p2p/protocols/SendPWQ.h
/// \brief Defines a protocol extension for sends using a PWQ
///
#ifndef __p2p_protocols_SendPWQ_h__
#define __p2p_protocols_SendPWQ_h__

#include <pami.h> // \todo remove? PAMI_Context_post()

#include "p2p/protocols/Send.h" // assumed basis of T_Protocol

#include "util/trace.h"

#include "util/ccmi_debug.h"

 #undef DO_TRACE_ENTEREXIT
 #undef DO_TRACE_DEBUG
#ifdef CCMI_TRACE_ALL
 #define DO_TRACE_ENTEREXIT 1
 #define DO_TRACE_DEBUG     1
#else
 #define DO_TRACE_ENTEREXIT 0
 #define DO_TRACE_DEBUG     0
#endif

#define TMPBUF_SIZE 16*1024
#define PWQ_HDR_SZ  128

#define CPY_USR_HDR(dst,src,sze)       \
{                                      \
unsigned * us = (unsigned *) src;      \
unsigned * ud = (unsigned *) dst;      \
if(sze == 32){                         \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;                  \
*ud = *us; ud++;us++;}                 \
else{                                  \
memcpy(dst,src,sze);}}


namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {
      ///
      /// \brief Template class to extend point-to-point send with PWQ support.
      ///
      /// Send side only.  Receive/dispatch still uses the protocol's
      /// flat buffer dispatch.
      ///
      /// T_Protocol is assumed to be some send protocol:
      /// \see PAMI::Protocol::Send::Send
      ///
      /// This class is NOT constructed, it is an extension of an existing
      /// protocol that is used by casting the protocol to this class.
      ///
      /// This was done because protocol constructor and factory generates
      /// are not standard so SendPWQ can't overload them.
      ///
      /// Restrictions:
      /// Because it is NOT constructed, it may not define member data or
      /// new virtual member functions in this class
      ///
      /// \todo Pass in a generic work device for posting instead of
      /// using the c function - PAMI_Context_post()?  Hard to do without
      /// member data. Could be passed on each call and stored in sendpwq_t?
      ///
      /// Example of usage:
      ///
      /// // Define a protocol
      /// typedef Protocol::Send::...<> MyProtocol;
      ///
      /// // Define a SendPWQ over this protocol
      /// typedef PAMI::Protocol::Send::SendPWQ < MyProtocol > MySendPWQ;
      ///
      /// // Generate the protocol
      /// MySendPWQ *protocol = (MySendPWQ*) MySendPWQ::generate(...);
      ///
      /// NOTE: generate will call MyProtocol::generate and return MyProtocol*,
      /// so it IS  necessary to cast it to MySendPWQ*.  This constructs
      /// MyProtocol but does not actually call any MySendPWQ constructor,
      /// thus the restrictions note above.
      ///
      /// You may then call member functions on MySendPWQ or MyProtocol
      ///
      /// protocol->immediatePWQ();  // Call new PWQ immediate on MySendPWQ.
      ///
      /// protocol->immediate();    // Call regular p2p immediate send.
      ///
      /// protocol->otherProtocolFunction(); // Call other protocol functions.
      ///
      static unsigned pwqMsgID    = 0;
      static unsigned immSendSize = 0;


      template < class T_Protocol >
      class SendPWQ : public T_Protocol
      {
      public:
        typedef struct
        {
          pami_work_t               work[2];//SSS: Pipelining the work posting by switching between two work structures
          enum {SIMPLE,IMMEDIATE}   type;
          union
          {
            pami_send_immediate_t       immediate;
            pami_send_t                 simple;
          }                         send;
          PAMI::PipeWorkQueue      *pwq;
          SendPWQ                  *pthis;
          PAMI::Topology            dst_participants;
          pami_client_t             client;
          size_t                    contextid;
          unsigned                  connection_id; //This is set by multicast/m2m.. It is redundant here but needed instead of extracting from multicast header
          pami_send_event_t         userEvents;
          int                       allocatedHdr;
          size_t                    totalSndln;
          char                     *tmpbuf;
          int                       work_posted;
          int                       work_counter;//SSS: Used for switching between work structures.
          unsigned                  participants;//SSS: We use this in mcast cases to determine if we finished sending to everyone.
        } sendpwq_t;

        typedef struct
        {
          unsigned     connectionId;                        //A connection id used by the recv side to match messages
          unsigned     pwqMsgId;                            //A sequence number for a pwq message
          unsigned     pwqSeqNo;                            //A sequence number to handle out of order packets for a message.  Hi order bit is 'contig' flag.
          char         usrHdr[PWQ_HDR_SZ - (sizeof(unsigned)*3)];   //A blob to hold user header.
        } sendpwqHdr_t;


        typedef struct
        {
          PAMI::MemoryAllocator < sizeof(sendpwqHdr_t) + sizeof(sendpwq_t), 16 > _hsAllocator; // Hdr and State Allocator
          PAMI::MemoryAllocator < sizeof(sendpwqHdr_t)                    , 16 > _hoAllocator; // Hdr only Allocator
        } sendpwqAllocators_t;

        static void setImmSendSize(unsigned size)
        {
          immSendSize = size;
        }

        ///
        /// \brief Async work function. Try to resend the data in the pwq if it's ready
        ///
        /// \param[in]  context   Work context
        /// \param[in]  cookie    Original send state to resend.
        ///
        static void sendPWQChunkDone ( pami_context_t   context,
                                  void          *  cookie,
                                  pami_result_t    result )
        {
          TRACE_FN_ENTER();
          sendpwq_t *state = (sendpwq_t*)cookie;

          //SSS: We only call the next iteration when we are done with all participants..
          //     Otherwise, we return to the caller (simplePWQ) to continue the loop for sending
          if(state->participants) state->participants--;
          if(!state->participants)
          {
            pami_send_t * parameters = &state->send.simple;

            sendpwqHdr_t * sendpwqHdr = (sendpwqHdr_t *)parameters->send.header.iov_base;
            sendpwqHdr->pwqSeqNo++;
            if(!state->totalSndln)
            {
              pami_send_event_t     events    = state->userEvents;
              sendpwqAllocators_t * allocator = (sendpwqAllocators_t *)state->pthis->getPWQAllocator();
              TRACE_FORMAT( "<%p>DONE context %p, cookie %p, user fn %p/%p/%p",state->pthis,context, cookie, events.local_fn,events.remote_fn, events.cookie);
              PAMI_assert(events.remote_fn == NULL); // Doesn't seem like we handle this.

              size_t size = state->dst_participants.size();
              if(state->tmpbuf)
                 free(state->tmpbuf);
              //SSS: No need to check on state->allocatedHdr anymore. If I reach here, header is allocated.
              //     Instead, I check on size. If size then it is an mcast and I only allocated a hdr, not hdr+state.
              //if(state->allocatedHdr)
                 //free(state->send.simple.send.header.iov_base);
              if(size)
                allocator->_hoAllocator.returnObject(state->send.simple.send.header.iov_base);
              else
                allocator->_hsAllocator.returnObject(state->send.simple.send.header.iov_base);


              //SSS: Don't need to free state anymore. I free it with header.
              //if(!size)free(state);//SSS: This is the case of sendPWQ creating the state.

              if(size)
                for(size_t i = 0; i < size; i++)
                  events.local_fn(context, events.cookie, result);
              else
                  events.local_fn(context, events.cookie, result);
            }
            else //Call send again.. Don't post to device.. Let send do that if it needs to
            {
              result = state->pthis->simplePWQ(state, context);
            }
          }
          TRACE_FN_EXIT();
        }

        ///
        /// \brief Async work function. Try to resend the data in the pwq if it's ready
        ///
        /// \param[in]  context   Work context
        /// \param[in]  cookie    Original send state to resend.
        ///
        static pami_result_t sendPWQ_work_function(pami_context_t context, void *cookie)
        {
          pami_result_t result = PAMI_EAGAIN;
          TRACE_FN_ENTER();
          sendpwq_t * state = (sendpwq_t*)cookie;

          TRACE_FORMAT( "<%p> context %p, cookie %p",state->pthis,context, cookie);

          result = state->pthis->simplePWQ(state, context);
          return result;

          TRACE_FORMAT( "<%p> context %p, cookie %p",state->pthis,context, cookie);
          TRACE_FN_EXIT();

          return PAMI_SUCCESS;
        }
        ///
        /// \brief Do an immediate send. This function is called from simplePWQ.
        /// It is only called when data is ready in the pwq and the size is less
        /// than the size for the immediate send.
        ///
        /// Since the function is called with ready data, no context is required.
        ///
        /// \param[in]  pami_send_immediate_t      *immediate
        ///
        pami_result_t immediatePWQ(pami_send_immediate_t* immediate)
        {
          TRACE_FN_ENTER();
          pami_result_t result = PAMI_SUCCESS;
          pami_send_immediate_t * parameters = immediate;


          TRACE_FORMAT( "<%p> length %zd, payload %p ",this,
                        (size_t)parameters->data.iov_len, 
                        parameters->data.iov_base);

          result = this->immediate (parameters);
          TRACE_FORMAT( "<%p> result %u", this, result);

          TRACE_FN_EXIT();

          return result;
        }

        ///
        /// \brief Start a new simple P2P send with PWQ.
        ///
        /// \param[in]  context    Send context
        /// \param[in]  dest       Destination endpoint
        /// \param[in]  header_len Send header length in bytes.
        /// \param[in]  header     A pointer to header data
        /// \param[in]  bytes      Send data length in bytes.
        /// \param[in]  pwq        A pipe work queue to use for the data buffer
        /// \param[in]  events     Send completion events/cookie structure
        /// \param[in]  dispatch   Dispatch id
        ///
        pami_result_t simplePWQ (
          pami_context_t       context,
          pami_endpoint_t      dest,
          unsigned             connection_id,
          size_t               header_length,
          void                *header,
          size_t               bytes,
          PAMI::PipeWorkQueue *pwq,
          pami_send_event_t   *events,
          size_t               dispatch)
        {
          TRACE_FN_ENTER();
          pami_result_t result = PAMI_EAGAIN;
          PAMI_assert(header_length <= (PWQ_HDR_SZ-(sizeof(unsigned)*3)));//SSS: Making sure the sendpwqHdr will be enough for usrHdr.

          TRACE_FORMAT( "<%p> context %p, pwq %p, bytes %zu, dest %zu",this, context, pwq, bytes, (size_t)dest);
          size_t length = pwq? pwq->bytesAvailableToConsume() : 0;
          void* payload = pwq?(void*)pwq->bufferToConsume(): NULL;
          TRACE_FORMAT( "<%p> length %zd, payload %p",this, length,payload);

          // send it now if there is enough data in the pwq

          if (length >= bytes)
          {
            unsigned pwqHdrLen = header_length + sizeof(unsigned)*3;
            if(length+pwqHdrLen <= immSendSize)
            {
              sendpwqHdr_t sendpwqHdr;
              sendpwqHdr.connectionId = connection_id;
              sendpwqHdr.pwqMsgId     = pwqMsgID++;
              sendpwqHdr.pwqSeqNo     = 0x10000000; // contig flag

              //Do hdr copy function
              CPY_USR_HDR(sendpwqHdr.usrHdr, header, header_length);

              pami_send_immediate_t send;
              send.hints                       = (pami_send_hint_t){0};

              send.dest                        = dest;
              send.dispatch                    = dispatch;
              send.header.iov_base             = &sendpwqHdr;
              send.header.iov_len              = pwqHdrLen;
              send.data.iov_base               = payload;
              send.data.iov_len                = length;
              result = this->immediatePWQ(&send);
              if(events->local_fn) events->local_fn(context, events->cookie, result);
              return result;
            }
            sendpwqHdr_t * sendpwqHdr;
            PAMI_assert(this->getPWQAllocator() != NULL);
            sendpwqAllocators_t * allocator = (sendpwqAllocators_t *)this->getPWQAllocator();
            sendpwqHdr = (sendpwqHdr_t *)allocator->_hsAllocator.allocateObject();
            //posix_memalign((void**)&sendpwqHdr, 128, sizeof(sendpwqHdr_t) + sizeof(sendpwq_t));
            sendpwqHdr->connectionId = connection_id;
            sendpwqHdr->pwqMsgId     = pwqMsgID++;
            sendpwqHdr->pwqSeqNo     = 0x10000000; // contig flag
            CPY_USR_HDR(sendpwqHdr->usrHdr, header, header_length);

            sendpwq_t   *state = (sendpwq_t*)((char*)sendpwqHdr + sizeof(sendpwqHdr_t));

            pami_send_t s;
            memset(&s.send.hints, 0, sizeof(s.send.hints));
            s.send.dest = dest;
            s.events.cookie         = state;
            s.events.local_fn       = sendPWQChunkDone;
            s.events.remote_fn      = NULL;
            s.send.dispatch         = dispatch;
            s.send.header.iov_base  = sendpwqHdr;
            s.send.header.iov_len   = pwqHdrLen;
            s.send.data.iov_base    = payload;
            s.send.data.iov_len     = length;

            PAMI::Topology tmpTopo;
            // The ONLY state fields sendPWQChunkDone uses:
            state->pthis            = this;
            state->userEvents       = *events;
            state->allocatedHdr     = 1;
            state->send.simple      = s;
            state->participants     = 0;
            state->totalSndln       = 0;//SSS: 1 - we don't need totalSndln here anymore 2 - It is needed in sendPWQChunkDone to free data
            state->dst_participants = tmpTopo;
            state->tmpbuf           = NULL;

            TRACE_HEXDATA(sendpwqHdr, sizeof(sendpwqHdr));
            TRACE_FORMAT( "<%p> send(%u(%p))", this, s.send.dest, context);
            //fprintf(stderr, "CCMI:: SHORT send to EP%d\n", dest);
            result =  this->simple(&s);
            TRACE_FORMAT( "<%p> result %u", this, result);
            TRACE_FN_EXIT();
            return result;
          }
          else if(pwq)/* Non contig data case or pwq doesn't have enough bytes*/
          {
            sendpwqHdr_t * sendpwqHdr;
            PAMI_assert(this->getPWQAllocator() != NULL);
            sendpwqAllocators_t * allocator = (sendpwqAllocators_t *)this->getPWQAllocator();
            sendpwqHdr = (sendpwqHdr_t *)allocator->_hsAllocator.allocateObject();
            //posix_memalign((void**)&sendpwqHdr, 128, sizeof(sendpwqHdr_t) + sizeof(sendpwq_t));

            sendpwqHdr->connectionId = connection_id;
            sendpwqHdr->pwqMsgId     = pwqMsgID++;
            sendpwqHdr->pwqSeqNo     = 0x0;
            CPY_USR_HDR(sendpwqHdr->usrHdr, header, header_length);

            char *tmpbuf = NULL;
            size_t tmpbuf_size = bytes<TMPBUF_SIZE?bytes:TMPBUF_SIZE;
            size_t tmpbuf_cursor = 0;
            size_t bytesAvail = 0;
            posix_memalign((void**)&tmpbuf, 128, tmpbuf_size);
            pami_send_t  s;
            sendpwq_t   *state = (sendpwq_t*)((char*)sendpwqHdr + sizeof(sendpwqHdr_t));

            PAMI::Topology tmpTopo;

            memset(&s.send.hints, 0, sizeof(s.send.hints));
            s.send.dest             = dest;
            s.events.cookie         = state;
            s.events.local_fn       = sendPWQChunkDone;
            s.events.remote_fn      = NULL;
            s.send.dispatch         = dispatch;
            s.send.header.iov_base  = sendpwqHdr;
            s.send.header.iov_len   = sizeof(sendpwqHdr_t);
            s.send.data.iov_base    = tmpbuf;

            state->send.simple      = s;
            state->pwq              = pwq;
            state->pthis            = this;
            state->dst_participants = tmpTopo;
            state->client           = NULL;
            state->contextid        = -1;
            state->connection_id    = -1;//Not needed here.. we already allocated header with connection id passed in
            state->userEvents       = *events;
            state->tmpbuf           = tmpbuf;
            state->type             = sendpwq_t::SIMPLE;
            state->allocatedHdr     = 1;
            state->totalSndln       = bytes;
            state->work_posted      = 0;
            state->work_counter     = 0;
            state->participants     = 0;

            while((bytesAvail = pwq->bytesAvailableToConsume()))
            {
              if((tmpbuf_cursor + bytesAvail) <=  tmpbuf_size)
              {
                memcpy(tmpbuf + tmpbuf_cursor, pwq->bufferToConsume(), bytesAvail);
                tmpbuf_cursor += bytesAvail;
                pwq->consumeBytes(bytesAvail);
              }
              else
              {
                s.send.data.iov_len    = tmpbuf_cursor;
                state->totalSndln     -= tmpbuf_cursor;

                TRACE_FORMAT( "<%p> send(%u(%p))", this, s.send.dest, context);
                result =  this->simple(&s);
                TRACE_FORMAT( "<%p> result %u", this, result);
                return result;
              }
            }
            //SSS: bytesAvailableToConsume may return 0 because PWQ not ready not because we are done
            s.send.data.iov_len = tmpbuf_cursor;
            state->totalSndln  -= tmpbuf_cursor;

            TRACE_FORMAT( "<%p> send(%u(%p))", this, s.send.dest, context);
            if(tmpbuf_cursor > 0)
            {
              result =  this->simple(&s); //SSS: bytes has to be > 0 if we already reached here
            }
            else
            {
              state->work_posted = 1;
              PAMI_Context_post (context,(pami_work_t*)&state->work[(state->work_counter++)%2], sendPWQ_work_function, (void *) state);
            }
            TRACE_FORMAT( "<%p> result %u", this, result);
          }
          // \todo not enough data to send yet, ...
          TRACE_FORMAT( "<%p> result %u", this, result);
          TRACE_FN_EXIT();
          return result; // PAMI_EAGAIN
        }
        ///
        /// \brief Start a new simple send message with PWQ.  If there is no
        /// data ready, post an async work function to retry later.
        ///
        /// \param[in]  sendpwq_t      *state     state storage and parameters:
        /// 
        ///             pami_work_t               work; 
        ///             enum {SIMPLE,IMMEDIATE}   type;
        ///             union
        ///             {
        ///               pami_send_immediate_t       immediate;
        ///               pami_send_t                 simple;
        ///             }                         send;
        ///             PAMI::PipeWorkQueue      *pwq;
        ///             SendPWQ                  *pthis;
        ///             PAMI::Topology            dst_participants;
        ///             pami_client_t             client;
        ///             size_t                    contextid;
        ///             size_t                    clientid;
        /// 
        /// \param[in]  pami_context_t  context
        ///
        pami_result_t simplePWQ (sendpwq_t* state, pami_context_t context)
        {
          TRACE_FN_ENTER();
          pami_result_t result = PAMI_EAGAIN;
          pami_send_t * parameters = &state->send.simple;

          PAMI::PipeWorkQueue * pwq = state->pwq;

          TRACE_FORMAT( "<%p> state %p, context %p, parameters %p, pwq %p, ndest %zu, connection id %d",this, state, context, parameters, pwq, state->dst_participants.size(),state->connection_id);
          size_t length = pwq? pwq->bytesAvailableToConsume() : 0;
          void* payload = pwq?(void*)pwq->bufferToConsume(): NULL;
          TRACE_FORMAT( "<%p> length %zd/%zd, payload %p  data[%.2u..%.2u]",this, length,(size_t)parameters->send.data.iov_len, payload, payload?*(char*)payload:-1,payload?*(char*)((char*)payload+length-1):-1);
          // send it now if there is enough data in the pwq

          if (length >= state->totalSndln)
          {
            //SSS: I have to check on state->connection_id here to make sure
            //     this is a mcast case. There may be a situation where device
            //     was not ready before and we are sending a remaining chunk of
            //     of a non contig message whose size is lessa than immSendSize.
            unsigned pwqHdrLen = parameters->send.header.iov_len + sizeof(unsigned)*3;
            if(length+pwqHdrLen <= immSendSize && state->connection_id != -1U)
            {
              sendpwqHdr_t sendpwqHdr;
              sendpwqHdr.connectionId = state->connection_id;
              sendpwqHdr.pwqMsgId     = pwqMsgID++;
              sendpwqHdr.pwqSeqNo     = 0x10000000; // contig flag

              //Do hdr copy function
              CPY_USR_HDR(sendpwqHdr.usrHdr, parameters->send.header.iov_base, parameters->send.header.iov_len);
              parameters->send.header.iov_base = &sendpwqHdr;
              parameters->send.header.iov_len  = pwqHdrLen;
              parameters->send.data.iov_base   = payload;
              parameters->send.data.iov_len    = state->totalSndln; //length;

              size_t size = state->dst_participants.size();
              //SSS: No need to check on size here. If we reach here, then it is a mcast.
              for (unsigned i = 0; i < size; ++i)
              {
                parameters->send.dest = state->dst_participants.index2Endpoint(i);

                TRACE_FORMAT( "<%p> immediate(%u(%zu))", this, parameters->send.dest, state->contextid);
                result = this->immediatePWQ(&parameters->send);
                TRACE_FORMAT( "<%p> result %u", this, result);

              }
              for(size_t i = 0; i < size; i++)
                state->userEvents.local_fn(context, state->userEvents.cookie, result);
              TRACE_FN_EXIT();
              return result;
            }
            sendpwqHdr_t *sendpwqHdr;

            TRACE_FORMAT( "<%p> state %p, context %p, parameters %p, pwq %p, ndest %zu",this, state, context, parameters, pwq, state->dst_participants.size());
            TRACE_FORMAT( "<%p> length %zd/%zd, payload %p  data[%.2u..%.2u]",this, length,state->totalSndln, payload, *(char*)payload,*(char*)((char*)payload+length-1));
            parameters->send.data.iov_base = payload;
            parameters->send.data.iov_len = state->totalSndln; //length;
            state->work_posted = 0;//SSS: I only post when I don't have enough to send in PWQ.. if I am sending now, then I should dequeue from device
            //SSS: Check to see whether this is a mcast or simplePWQ. mcast will require a hdr but simplePWQ already has one
            if(state->connection_id != -1U)//mcast case
            {
               PAMI_assert(this->getPWQAllocator() != NULL);
               sendpwqAllocators_t * allocator = (sendpwqAllocators_t *)this->getPWQAllocator();
               sendpwqHdr = (sendpwqHdr_t *)allocator->_hoAllocator.allocateObject();
               //posix_memalign((void**)&sendpwqHdr, 128, sizeof(sendpwqHdr_t));
               PAMI_assert(state->allocatedHdr!=1);
               state->allocatedHdr      = 1;
               state->participants      = state->dst_participants.size();
               state->pthis             = this;
               sendpwqHdr->connectionId = state->connection_id;
               sendpwqHdr->pwqMsgId     = pwqMsgID++;
               sendpwqHdr->pwqSeqNo     = 0x10000000; // contig flag
               PAMI_assert(parameters->send.header.iov_len <= (PWQ_HDR_SZ-(sizeof(unsigned)*3)));//SSS: Making sure the sendpwqHdr will be enough for usrHdr.
               CPY_USR_HDR(sendpwqHdr->usrHdr, parameters->send.header.iov_base, parameters->send.header.iov_len);
               parameters->send.header.iov_base = sendpwqHdr;
               parameters->send.header.iov_len  = pwqHdrLen;
               parameters->events.cookie        = state;
               parameters->events.local_fn      = sendPWQChunkDone;
               parameters->events.remote_fn     = NULL;
               TRACE_HEXDATA(&sendpwqHdr, sizeof(sendpwqHdr));
            }
            size_t size = state->dst_participants.size();
            TRACE_HEXDATA(parameters->send.header.iov_base,parameters->send.header.iov_len);
            TRACE_FORMAT("ndst %zu, sendpwqHdr.connectionId %d,sendpwqHdr.pwqMsgId %u,sendpwqHdr.pwqSeqNo %#X", size, ((sendpwqHdr_t*)parameters->send.header.iov_base)->connectionId,((sendpwqHdr_t*)parameters->send.header.iov_base)->pwqMsgId,((sendpwqHdr_t*)parameters->send.header.iov_base)->pwqSeqNo);
            if(size)//Size is 0 for point to point ...
            {
              state->totalSndln = 0;
              for (unsigned i = 0; i < size; ++i)
              {
                parameters->send.dest = state->dst_participants.index2Endpoint(i);

                TRACE_FORMAT( "<%p> send(%u(%zu))", this, parameters->send.dest, state->contextid);
                result =  this->simple (parameters);
                TRACE_FORMAT( "<%p> result %u", this, result);

              }
            }
            else
            {
              state->totalSndln = 0;
              result =  this->simple (parameters);
            }
            TRACE_FN_EXIT();
            return result;


          }
          else if(pwq)
          {

            //SSS: Check to see if this is an mcast or simplePWQ to allocate hdr
            sendpwqHdr_t * sendpwqHdr;

            if(state->connection_id != -1U)
            {
              PAMI_assert(this->getPWQAllocator() != NULL);
              sendpwqAllocators_t * allocator = (sendpwqAllocators_t *)this->getPWQAllocator();
              sendpwqHdr = (sendpwqHdr_t *)allocator->_hoAllocator.allocateObject();
              //posix_memalign((void**)&sendpwqHdr, 128, sizeof(sendpwqHdr_t));
              PAMI_assert(state->allocatedHdr!=1);
              sendpwqHdr->connectionId                = state->connection_id;
              sendpwqHdr->pwqMsgId                    = pwqMsgID++;
              sendpwqHdr->pwqSeqNo                    = 0x0;
              PAMI_assert(parameters->send.header.iov_len <= (PWQ_HDR_SZ-(sizeof(unsigned)*3)));//SSS: Making sure the sendpwqHdr will be enough for usrHdr.
              CPY_USR_HDR(sendpwqHdr->usrHdr, parameters->send.header.iov_base, parameters->send.header.iov_len);
              state->allocatedHdr                     = 1;
              state->send.simple.send.header.iov_len  = sizeof(sendpwqHdr_t);
              state->send.simple.send.header.iov_base = sendpwqHdr;
              state->connection_id                    = -1;//SSS: to stop allocating again
              state->pthis                            = this;
            }
            else
            {
              sendpwqHdr = (sendpwqHdr_t *)state->send.simple.send.header.iov_base;
            }
            TRACE_HEXDATA(sendpwqHdr, sizeof(sendpwqHdr));
            TRACE_HEXDATA(state->send.simple.send.header.iov_base,state->send.simple.send.header.iov_len);
            assert(state->send.simple.send.header.iov_len);

            char *tmpbuf = NULL;
            size_t tmpbuf_size = state->totalSndln<TMPBUF_SIZE?state->totalSndln:TMPBUF_SIZE;
            size_t tmpbuf_cursor = 0;
            size_t bytesAvail = 0;
            if(!state->tmpbuf)
            {
              posix_memalign((void**)&tmpbuf, 128, tmpbuf_size);
              state->tmpbuf = tmpbuf;
            }
            else
              tmpbuf = state->tmpbuf;

            state->send.simple.events.cookie        = state;
            state->send.simple.events.local_fn      = sendPWQChunkDone;
            state->send.simple.events.remote_fn     = NULL;
            state->send.simple.send.data.iov_base   = tmpbuf;

            state->type                             = sendpwq_t::SIMPLE;

            size_t size         = state->dst_participants.size();
            state->participants = size;

            while((bytesAvail = pwq->bytesAvailableToConsume()))
            {
              if((tmpbuf_cursor + bytesAvail) <=  tmpbuf_size)
              {
                memcpy(tmpbuf + tmpbuf_cursor, pwq->bufferToConsume(), bytesAvail);
                tmpbuf_cursor += bytesAvail;
                pwq->consumeBytes(bytesAvail);
              }
              else /*SSS: Set that send is not yet done in the state*/
              {
                state->send.simple.send.data.iov_len    = tmpbuf_cursor;
                state->totalSndln -= tmpbuf_cursor;
                state->work_posted = 0;//SSS: I only post when I don't have enough to send in PWQ.. if I am sending now, then I should dequeue from device
                TRACE_FORMAT("ndst %zu, sendpwqHdr.connectionId %d,sendpwqHdr.pwqMsgId %u,sendpwqHdr.pwqSeqNo %#X", size, sendpwqHdr->connectionId,sendpwqHdr->pwqMsgId,sendpwqHdr->pwqSeqNo);
                if(size)
                {
                  for (unsigned i = 0; i < size; ++i)
                  {
                    state->send.simple.send.dest = state->dst_participants.index2Endpoint(i);
                    TRACE_FORMAT( "<%p> send(%u(%zu))", this, state->send.simple.send.dest, state->contextid);
                    result =  this->simple (&state->send.simple);
                    TRACE_FORMAT( "<%p> result %u", this, result);

                  }
                }
                else
                {
                  result =  this->simple (&state->send.simple);
                }
                TRACE_FN_EXIT();
                return result;//SSS: I can't return PAMI_EAGAIN here even if I am not done. If advance is called before senddone I may overwrite tmpbuf.
              }
            }
            //SSS: bytesAvailableToConsume may return 0 because PWQ not ready not because we are done
            state->send.simple.send.data.iov_len = tmpbuf_cursor;
            state->totalSndln -= tmpbuf_cursor;
            TRACE_FORMAT("ndst %zu, sendpwqHdr.connectionId %d,sendpwqHdr.pwqMsgId %u,sendpwqHdr.pwqSeqNo %#X", size, sendpwqHdr->connectionId,sendpwqHdr->pwqMsgId,sendpwqHdr->pwqSeqNo);
            if(size && tmpbuf_cursor > 0)
            {
              state->work_posted = 0;//SSS: I only post when I don't have enough to send in PWQ.. if I am sending now, then I should dequeue from device
              for (unsigned i = 0; i < size; ++i)
              {
                state->send.simple.send.dest = state->dst_participants.index2Endpoint(i);
                TRACE_FORMAT( "<%p> send(%u(%zu))", this, state->send.simple.send.dest, state->contextid);
                result =  this->simple (&state->send.simple);//SSS: Only send if PWQ had something in it and was ready
                TRACE_FORMAT( "<%p> result %u", this, result);

              }
            }
            else
            {
              if(tmpbuf_cursor > 0)
              {
                state->work_posted = 0;//SSS: I only post when I don't have enough to send in PWQ.. if I am sending now, then I should dequeue from device
                TRACE_FORMAT( "<%p> send(%u(%zu))", this, state->send.simple.send.dest, state->contextid);
                result =  this->simple (&state->send.simple);//SSS: Only send if PWQ had something in it and was ready
                TRACE_FORMAT( "<%p> result %u", this, result);
              }
            }
            if(tmpbuf_cursor > 0)
            {
              TRACE_FN_EXIT();
              return result;//SSS: Only send if PWQ had something in it and was ready.. otherwise post to device
            }
          }
          // not enough data to send yet, post it to the context work queue for later processing
          TRACE_FORMAT( "<%p> queue it on context %p",this, context);
          state->type = sendpwq_t::SIMPLE;
          state->pthis = this;

          /// \todo Pass in a generic/work device so we can directly post
          if(!state->work_posted)
          {
            state->work_posted = 1;
            PAMI_Context_post (context,(pami_work_t*)&state->work[(state->work_counter++)%2], sendPWQ_work_function, (void *) state);
          }
          TRACE_FN_EXIT();
          return result;//SSS: returning PAMI_EAGAIN here will guarantee I remain on generic device queue if I am already on it
//        circular header dependencies if I try to use Context
//          PAMI::Context * ctx = (PAMI::Context *) context;
//          return ctx->post(state, sendPWQ_work_function, (void *) state);
        }

      }; // PAMI::Protocol::SendPWQ class

      template < class T_Protocol >
      class SendWrapperPWQ : public T_Protocol, public SendPWQ<PAMI::Protocol::Send::Send>
      {
      };

    }; // PAMI::Protocol::Send namespace
  };   // PAMI::Protocol namespace
};     // PAMI namespace
 #undef DO_TRACE_ENTEREXIT
 #undef DO_TRACE_DEBUG

#endif // __pami_p2p_protocols_SendPWQ_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
