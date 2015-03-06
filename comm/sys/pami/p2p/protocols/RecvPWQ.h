/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* ---------------------------------------------------------------- */
/* (C)Copyright IBM Corp.  2009, 2010                               */
/* IBM CPL License                                                  */
/* ---------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
///
/// \file p2p/protocols/RecvPWQ.h
/// \brief Defines a protocol ext. for sends using a PWQ (Recv side)
///
#ifndef __p2p_protocols_RecvPWQ_h__
#define __p2p_protocols_RecvPWQ_h__

#include <pami.h> // \todo remove? PAMI_Context_post()

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


namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {
      static char recv_trace_once = DO_TRACE_ENTEREXIT;
      ///
      /// \brief Class to extend point-to-point recv with PWQ support.
      ///
      /// Receive/dispatch no longer uses the protocol's flat buffer
      /// dispatch.
      ///
      /// T_Protocol is assumed to be some send protocol:
      /// \see PAMI::Protocol::Send::Send
      ///
      /// \todo Pass in a generic work device for posting instead of
      /// using the c function - PAMI_Context_post()?  This needs to be
      /// changed when SendPWQ is changed to do the same.
      ///
      /// In this class, we define a PWQMessage which is used as packet holder.
      /// PWQMessages are only created to hold packets in the case of non-contig
      /// data or pwq not ready. If pwq is ready, then data is processed immediately.
      ///
      /// Message internally handles out of sequence packets (for PERCS like network).
      ///
      /// Static functions defined here are to be used to process recv pwq.
      ///
      typedef std::map<pami_endpoint_t, std::map<size_t, MatchQueue<unsigned long long> *> *> mymap_t;

      class PWQMessage : public MatchQueueElem<unsigned long long>
      {

        public:
          class IncomingPacket: public Queue::Element
          {
            public:
             int       _size;   //Size of _data. This should be updated when producing into pwq.
             int       _offset; //Pointing to the offset in _data. Should be 0 when first creating packet
             unsigned  _seqNo;  //Sequence number for packet for out of order messaging
             char     *_data;   //This will be allocated based on chunk size recvd.
          };
          pami_work_t             _work;
          Queue                   _packetQueue;
          PipeWorkQueue          *_recvPWQ;
          size_t                  _totalRecvln; //Total expected message size.
          pami_callback_t         _cb_done;
          int                     _work_posted;
          void                   *_myMap;
          void                   *_myAllocator;
          void                   *_dispatch_allocator;
          void                   *_queue_allocator;
          size_t                  _dispatch_id; //dispatch_id that this message belongs to
          pami_endpoint_t         _origin;      //Who sent me this message
          unsigned                _seqNo;       //PWQ sequence number for out of order handling
/*        typedef struct
        {
          pami_work_t               work;
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
          size_t                    clientid;
          pami_send_event_t         userEvents;
          int                       senddone;
          size_t                    totalSndln;
          char                     *tmpbuf;
          int                       work_posted;
        } sendpwq_t;
*/
          PWQMessage(PipeWorkQueue    * recvPWQ,
                     size_t             totalRecvln,
                     pami_callback_t    cb_done,
                     void             * myMap,
                     void             * myAllocator,
                     void             * dispatch_allocator,
                     void             * queue_allocator,
                     size_t             dispatch_id,
                     pami_endpoint_t    origin,
                     unsigned long long searchKey):
              MatchQueueElem<unsigned long long>(searchKey),
              _recvPWQ(recvPWQ),
              _totalRecvln(totalRecvln),
              _cb_done(cb_done),
              _work_posted(0),
              _myMap(myMap),
              _myAllocator(myAllocator),
              _dispatch_allocator(dispatch_allocator),
              _queue_allocator(queue_allocator),
              _dispatch_id(dispatch_id),
              _origin(origin),
              _seqNo(0)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("origin %u, _totalRecvln %zu",_origin,_totalRecvln);
            TRACE_FN_EXIT();
          }

        private:
          PAMI::MemoryAllocator<sizeof(PWQMessage::IncomingPacket),16> _pkt_allocator;

        public:
          static void cleanupMsg(pami_context_t context, PWQMessage * msg)
          {
            if(msg->_myAllocator)//SSS: Message is not already cleanedup
            {
              //get ranks map, dispatch id map and my match queue
              mymap_t                                                                               * rankMap       = (mymap_t *)msg->_myMap;
              std::map<size_t, MatchQueue<unsigned long long> *>                                    * dispatchMap   = NULL;
              MatchQueue<unsigned long long>                                                        * msgQueue      = NULL;
              PAMI::MemoryAllocator<sizeof(std::map<size_t, MatchQueue<unsigned long long> *>), 16> * dispatchAllocator = NULL;
              PAMI::MemoryAllocator<sizeof(MatchQueue<unsigned long long>), 16>                     * matchqueueAllocator = NULL;

              if(rankMap)
                dispatchMap   = (*rankMap)[msg->_origin];

              if(dispatchMap)
                msgQueue      = (*dispatchMap)[msg->_dispatch_id];

              //get memory allocators for cleanup work.
              PAMI::MemoryAllocator<sizeof(PWQMessage),16>         * msgAllocator  =
                   (PAMI::MemoryAllocator<sizeof(PWQMessage),16> *) msg->_myAllocator;
              if(dispatchMap) dispatchAllocator =
                   (PAMI::MemoryAllocator<sizeof(std::map<size_t, MatchQueue<unsigned long long> *>), 16> *)	msg->_dispatch_allocator;
              if(msgQueue) matchqueueAllocator =
                   (PAMI::MemoryAllocator<sizeof(MatchQueue<unsigned long long>), 16> *) msg->_queue_allocator;
              //copy done function
              pami_callback_t cb_done = msg->_cb_done;

              //remove from queue and do queue/map cleanup
              if(msgQueue)msgQueue->deleteElem(msg);
              if(msgQueue && !msgQueue->size())
              {
                dispatchMap->erase(msg->_dispatch_id);
                msgQueue->~MatchQueue<unsigned long long>();
                matchqueueAllocator->returnObject(msgQueue);
              }
              if(dispatchMap && !dispatchMap->size())
              {
                rankMap->erase(msg->_origin);
                dispatchMap->~map<size_t, MatchQueue<unsigned long long> *>();
                dispatchAllocator->returnObject(dispatchMap);
              }
              //call destructor
              msg->~PWQMessage();
              //let allocator return msg
              msgAllocator->returnObject(msg);
              //Cleanup message.. I don't want to clear everything here in case this message is still posted on device.
              //                  In this case, I want to know that it is completed totalRecvln == 0 to remove from device.
              msg->_myAllocator = NULL;
              //call done function
              if (cb_done.function)
               (cb_done.function)(context, cb_done.clientdata, PAMI_SUCCESS);
            }
          }

          ///
          /// \brief Async work function. Try to recieve the data into the pwq if it's ready
          ///
          /// \param[in]  context   Work context
          /// \param[in]  cookie    Message holding all data and recvPWQ
          ///
          static pami_result_t recvPWQ_work_function(pami_context_t context, void *cookie)
          {
            pami_result_t result = PAMI_EAGAIN;
            if(recv_trace_once) TRACE_FN_ENTER();
            PWQMessage * msg = (PWQMessage*)cookie;

            if(recv_trace_once) TRACE_FORMAT( "<%p> context %p, cookie %p",msg,context, cookie);

            result = recvPWQ( context, cookie );
            if(msg->_totalRecvln == 0)
            {
              if(!recv_trace_once) TRACE_FN_ENTER(); recv_trace_once = DO_TRACE_ENTEREXIT;
              cleanupMsg( context, msg );
              result = PAMI_SUCCESS;
            }
            if(recv_trace_once) TRACE_FN_EXIT();
            return result;
          }

          //Wrapper function for recvPWQ
          static void RecvPWQ(pami_context_t context, void * cookie, pami_result_t result)
          {
            pami_result_t res = PAMI_EAGAIN;
            PWQMessage * msg = (PWQMessage*)cookie;
            res = recvPWQ( context, cookie );
            if(msg->_totalRecvln == 0)
            {
              cleanupMsg( context, msg );
            }
          }


          static pami_result_t recvPWQ( pami_context_t context, void * cookie )
          {
            PWQMessage * msg = (PWQMessage*) cookie;
            PipeWorkQueue * recvPWQ = msg->_recvPWQ;

            int bytesAvail;
            int bytesToCopy;
            if((bytesAvail = recvPWQ->bytesAvailableToProduce()) > 0) //PWQ is ready to produce bytes
            {
              PWQMessage::IncomingPacket *ipacket = (PWQMessage::IncomingPacket *)msg->_packetQueue.peek();
              while(ipacket && ipacket->_seqNo != msg->_seqNo)
              {
                ipacket = (PWQMessage::IncomingPacket *)msg->_packetQueue.next(ipacket);
              }
              if(ipacket != NULL)/*It may be a message that was half processed (a packet consumed and
                                   waiting for another packet that is not here yet) */
              {
                PAMI_assertf(msg->_totalRecvln >= (size_t)ipacket->_size,"msg->_totalRecvln(%zu) >= (size_t)ipacket->_size(%zu)",msg->_totalRecvln,(size_t)ipacket->_size);
                while(bytesAvail && msg->_totalRecvln && ipacket->_size)
                {
                  bytesToCopy = ipacket->_size >= bytesAvail ? bytesAvail:ipacket->_size;
                  memcpy(recvPWQ->bufferToProduce(), ipacket->_data + ipacket->_offset, bytesToCopy);
                  ipacket->_size    -= bytesToCopy;
                  ipacket->_offset  += bytesToCopy;
                  msg->_totalRecvln -= bytesToCopy;
                  recvPWQ->produceBytes(bytesToCopy);
                  if(!ipacket->_size)
                  {
                    msg->dequeuePacket();
                    ipacket = (PWQMessage::IncomingPacket *)msg->_packetQueue.peek();
                    if(ipacket == NULL && msg->_totalRecvln)
                    {
                      if(!msg->_work_posted)
                      {
                        //SSS: No need to post here..Once data comes in, recvPWQ will be called by NI
                        return PAMI_SUCCESS;
                      }
                      else
                      {
                        return PAMI_EAGAIN;
                      }
                    }
                  }
                  bytesAvail = recvPWQ->bytesAvailableToProduce();
                }
                if(!msg->_totalRecvln)
                {
                   msg->_work_posted = 0;
                   return PAMI_SUCCESS;
                }
                else//Message not done but pwq not ready
                {
                  if(!msg->_work_posted)
                  {
                    msg->_work_posted = 1;
                    PAMI_Context_post (context,(pami_work_t*)&msg->_work, recvPWQ_work_function, (void *) msg);
                    return PAMI_SUCCESS;
                  }
                  else
                  {
                    return PAMI_EAGAIN;
                  }
                }
              }
              else//Didn't get the data yet
              {
                if(!msg->_work_posted)
                {
                  if(msg->_totalRecvln)//SSS: Only post if I need to
                  {
                    msg->_work_posted = 1;
                    PAMI_Context_post (context,(pami_work_t*)&msg->_work, recvPWQ_work_function, (void *) msg);
                  }
                  return PAMI_SUCCESS;
                }
                else
                {
                  return PAMI_EAGAIN;
                }
              }
            }
            else //PWQ is not ready, post msg to generic device if not already posted
            {
              if(!msg->_work_posted)
              {
                if(msg->_totalRecvln)//SSS: Only post if I need to
                {
                  msg->_work_posted = 1;
                  PAMI_Context_post (context,(pami_work_t*)&msg->_work, recvPWQ_work_function, (void *) msg);
                }
                return PAMI_SUCCESS;
              }
              else
              {
                return PAMI_EAGAIN;
              }
            }

          }

          //enqueuePacket allocates a packet object and enqueues it to the packet queue.
          //The data* assumes that data is already alloacted. On the dequeue, on the
          //other hand, the packet allocation is returned and the data* is freed.
          void enqueuePacket(int size, int offset, unsigned seqNo, char * data)
          {
            TRACE_FN_ENTER();
            TRACE_FORMAT("size %d,offset %d,seqNo %u,data %p",size,offset,seqNo,data);
            PWQMessage::IncomingPacket *ipacket = (PWQMessage::IncomingPacket *)this->_pkt_allocator.allocateObject();
            ipacket->_size    = size;
            ipacket->_offset  = offset;
            ipacket->_data    = data;
            ipacket->_seqNo   = seqNo;
            this->_packetQueue.enqueue(ipacket);
            TRACE_FN_EXIT();
          }

          void dequeuePacket()
          {
            PWQMessage::IncomingPacket *ipacket = (IncomingPacket*)_packetQueue.dequeue();
            free(ipacket->_data);
            this->_pkt_allocator.returnObject(ipacket);
            this->_seqNo++;
          }
      };// Class PWQMessage
    }; // PAMI::Protocol::Send namespace
  };   // PAMI::Protocol namespace
};     // PAMI namespace

 #undef DO_TRACE_ENTEREXIT
 #undef DO_TRACE_DEBUG

#endif // __pami_p2p_protocols_RecvPWQ_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
