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
/// \file components/devices/bgq/mu2/model/Multicast.h
/// \brief Simple multicast protocol
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __components_devices_bgq_mu2_model_Multicast_h__
#define __components_devices_bgq_mu2_model_Multicast_h__

#include <map>

#include "components/devices/MulticastModel.h"

#include "components/devices/bgq/mu2/model/BroadcastPacketModel.h"
#include "components/memory/MemoryAllocator.h"

#include "util/ccmi_debug.h"
#include "util/ccmi_util.h"
#include "util/trace.h"

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
  namespace Device
  {
    namespace MU
    {

      ///////////////////////////////////////////////////////////////////////////////
      // \class MulticastModel
      // \brief MU collective Multicast interface
      // \details
      //   - all-sided or active message model
      //   - uses MU memfifo
      //   - one destination task per node
      //   - does not fully support PipeWorkQueue (multicast_model_available_buffers_only)
      ///////////////////////////////////////////////////////////////////////////////
      template <bool T_Allsided, bool T_Msgdata_support = false, bool T_PWQ_support = false>
      class MulticastModel : public Interface::AMMulticastModel < MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>, MU::Context, 4096 /*sizeof(state_data_t)*/ >
      {

      protected:

        typedef uint8_t packet_state_t[BroadcastPacketModel::packet_model_state_bytes];

        // Metadata passed in the (single-packet) header
        typedef struct __attribute__((__packed__))
        {
          uint32_t              connection_id;  ///< Collective connection id
          uint32_t              root;           ///< Root of the collective
          uint32_t              sndlen;         ///< Number of bytes of application data
          uint32_t              msgcount;       ///< Count of msgdata quads
        } header_metadata_t;

        // Metadata passed in the (multi-packet) data packet(s)
        typedef struct __attribute__((__packed__))
        {
          uint32_t              connection_id;  ///< Collective connection id
        } data_metadata_t;

        // State (request) implementation.  Callers should use uint8_t[MU::MulticastModel::sizeof_msg]
        typedef struct
        {
          packet_state_t          pkt[2];          ///< packet send state memory

          header_metadata_t       header_metadata;
          data_metadata_t         data_metadata;

          size_t                  remaining_length;
          PAMI::PipeWorkQueue   * rcvpwq;
          uint8_t               * buffer;
          pami_callback_t         cb_done;
          struct                  iovec iov[2];
        } state_data_t;


      public:

        /// \see PAMI::Device::Interface::MulticastModel::~MulticastModel
        ~MulticastModel ()
        {
        };

        /// \brief Multicast model constants/attributes
        static const bool   multicast_model_all_sided               = T_Allsided;
        static const bool   multicast_model_active_message          = !T_Allsided;
        static const bool   multicast_model_available_buffers_only  = !T_PWQ_support;

        static const size_t sizeof_msg                              = 4096 /*sizeof(state_data_t)*/;
        static const size_t packet_model_payload_bytes              = BroadcastPacketModel::packet_model_payload_bytes;
        static const size_t packet_model_immediate_bytes            = BroadcastPacketModel::packet_model_immediate_bytes;
        /* \todo use packet_model_payload_bytes or packet_model_immediate_bytes in code? */

        static const size_t multicast_model_msgcount_max            = (packet_model_payload_bytes  / sizeof(pami_quad_t));
        static const size_t multicast_model_bytes_max               = (uint32_t) - 1; // protocol_metadata_t::sndlen
        static const size_t multicast_model_connection_id_max       = (uint32_t) - 1; // protocol_metadata_t::connection_id \todo 64 bit?

        /// \see PAMI::Device::Interface::MulticastModel::registerMcastRecvFunction
        pami_result_t registerMcastRecvFunction_impl(int                        dispatch_id,
                                                     pami_dispatch_multicast_function func,
                                                     void                      *arg);

        pami_result_t postMulticastImmediate_impl(size_t                client,
						  size_t                context, 
						  pami_multicast_t    * mcast,
						  void                * devinfo=NULL) 
        {
	  return PAMI_ERROR;
	}
        /// \see PAMI::Device::Interface::MulticastModel::postMulticast
        pami_result_t postMulticast_impl(uint8_t (&state)[MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>::sizeof_msg], 
                                         size_t            client,
					 size_t            context,
                                         pami_multicast_t *mcast,
                                         void             *devinfo = NULL);

      protected:
        /// \brief MU dispatch function
        inline static int dispatch_header (void   * metadata,
                                           void   * payload,
                                           size_t   bytes,
                                           void   * arg,
                                           void   * cookie);
        inline static int dispatch_data (void   * metadata,
                                         void   * payload,
                                         size_t   bytes,
                                         void   * arg,
                                         void   * cookie);
        /// \brief Process user data packets
        inline void processData   (state_data_t *state_data,
                                   uint8_t      *payload,
                                   size_t        bytes);

        inline state_data_t * allocateState ()
        {
          return(state_data_t *) _allocator.allocateObject ();
        }

        inline void freeState (state_data_t * object)
        {
          _allocator.returnObject ((void *) object);
        }


      private:
        MU::Context                                & _device;
        pami_context_t                               _context;
        pami_task_t                                  _task_id;

        MemoryAllocator < sizeof(state_data_t), 16 > _allocator;

        BroadcastPacketModel                         _header_model;
        BroadcastPacketModel                         _data_model;

        pami_dispatch_multicast_function             _dispatch_function;
        void                                       * _dispatch_arg;

//        T_Connection                                 _connection; ///\todo ConnectionArray isn't appropriate...
        std::map<unsigned, state_data_t *>             _connection;

      public:

        /// \see PAMI::Device::Interface::MulticastModel::MulticastModel
        MulticastModel (pami_client_t  client, pami_context_t context, MU::Context & device, pami_result_t &status) :
        Interface::AMMulticastModel < MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>, MU::Context, 4096 /*sizeof(state_data_t)*/ > (device, status),
        _device (device),
        _context(context),
        _task_id(__global.mapping.task()),
        _header_model (device),
        _data_model (device),
        _dispatch_function(NULL),
        _dispatch_arg(NULL)
        //        _connection (device)
        {
          TRACE_FN_ENTER();
          TRACE_FORMAT( "this %p", this);
          // ----------------------------------------------------------------
          // Compile-time assertions
          // ----------------------------------------------------------------

          // This protocol only works with reliable networks.
          COMPILE_TIME_ASSERT(BroadcastPacketModel::reliable_packet_model == true);

          // This protocol only works with deterministic models.
          COMPILE_TIME_ASSERT(BroadcastPacketModel::deterministic_packet_model == true);

          // ----------------------------------------------------------------
          // Compile-time assertions (end)
          // ----------------------------------------------------------------
          
//        if(T_Allsided)
//        {
//          int                         dispatch_id = 514;  /// \todo constant for the max dispatch set?
//          while(registerMcastRecvFunction_impl(--dispatch_id,(pami_dispatch_multicast_function)NULL,NULL) != PAMI_SUCCESS)
//            TRACE_FORMAT( "dispatch_id %u", dispatch_id);
//
//        }
          status = PAMI_SUCCESS;
          TRACE_FN_EXIT();
        }
      };

      template <bool T_Allsided, bool T_Msgdata_support, bool T_PWQ_support>
      inline pami_result_t MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>::postMulticast_impl(uint8_t (&state)[MulticastModel::sizeof_msg],
                                                                                                            size_t            client,
                                                                                                            size_t            context, 
                                                                                                            pami_multicast_t *mcast,
                                                                                                            void             *devinfo)
      {
        TRACE_FN_ENTER();

        uint32_t classRoute = (uint32_t)(size_t)devinfo; // convert platform independent void* to bgq uint32_t classroute
        // MU class routes start at 0 but ResourceManager adds 1 to avoid NULL-looking device info.
        PAMI_assert(classRoute);
        classRoute -= 1; 

        TRACE_FORMAT( "connection_id %#X, class route %#X", mcast->connection_id, classRoute);

        // Get the source data buffer/length and validate (assert) inputs
        size_t length = mcast->bytes;

        Topology* srcTopology = (Topology*) mcast->src_participants;
        bool isSrc = srcTopology && srcTopology->isRankMember(_task_id);
        PAMI::PipeWorkQueue *pwq = isSrc ? (PAMI::PipeWorkQueue *)mcast->src : (PAMI::PipeWorkQueue *)mcast->dst;
        size_t bytesAvailable = pwq == NULL? 0 : (isSrc ? pwq->bytesAvailableToConsume() : pwq->bytesAvailableToProduce());
        uint8_t * buffer = pwq == NULL? NULL : (isSrc ? (uint8_t *)pwq->bufferToConsume() : (uint8_t *)pwq->bufferToProduce());

        if (T_PWQ_support == false)
        {
          // If you're sending data, it must all be ready in the pwq.
          PAMI_assertf(bytesAvailable == length, "bytesAvailable %zu == %zu length",bytesAvailable,length);
        }
        else
        {
          PAMI_abortf("T_PWQ_support not supported yet\n");
        }

        if (T_Msgdata_support == false)
        {
          // No msgdata expected
          PAMI_assert(mcast->msgcount == 0);
        }

        TRACE_FORMAT( "dispatch %zu, connection_id %#X, msgcount %d/%p, bytes %zu/%p/%p",
                      mcast->dispatch, mcast->connection_id,
                      mcast->msgcount, mcast->msginfo,
                      mcast->bytes, pwq, buffer);

        state_data_t *state_data = (state_data_t*) & state;

        void* payload = NULL;

        state_data->header_metadata.connection_id = mcast->connection_id;
        state_data->header_metadata.root          = _task_id;
        state_data->header_metadata.sndlen        = length;
        state_data->header_metadata.msgcount      = mcast->msgcount;

        state_data->data_metadata.connection_id   = mcast->connection_id;

        state_data->cb_done = mcast->cb_done;

        if (isSrc)
        {
          /// \todo allow root to receive data too? unclear multicast semantics...
          state_data->remaining_length = 0;  // root doesn't copy any data
          state_data->rcvpwq = NULL;         
          state_data->buffer = NULL;         
        }
        else
        {
          state_data->remaining_length = bytesAvailable;
          state_data->rcvpwq = pwq;   
          state_data->buffer = buffer;
        }
        // Set the connection state
//        PAMI_assert(_connection.get(mcast->connection_id) == NULL);
//        _connection.set(mcast->connection_id, (void *)state_data);
        PAMI_assert(_connection[mcast->connection_id] == NULL);
        _connection[mcast->connection_id] = state_data;

        if (T_PWQ_support == false)
        {
          if (length)
          {
            payload = (void*)buffer;
            if (isSrc)
              pwq->consumeBytes(length);
          }

          if (T_Msgdata_support == false)
          {
            // Post the multicast to the device in one or more packets
            if (length <= packet_model_payload_bytes ) // one packet
            {
              TRACE_HEXDATA(&state_data->header_metadata,sizeof(header_metadata_t));
              TRACE_HEXDATA(payload,  length);
              _header_model.postCollectivePacket (state_data->pkt[0],
                                                  NULL,
                                                  NULL,
                                                  classRoute,
                                                  &state_data->header_metadata,
                                                  sizeof(header_metadata_t),
                                                  isSrc? payload:NULL,
                                                  length);
            }
            else  // > one packet of payload
            {
              TRACE_HEXDATA(&state_data->header_metadata,sizeof(header_metadata_t));
              TRACE_HEXDATA(payload,  packet_model_payload_bytes);
              _header_model.postCollectivePacket (state_data->pkt[0],
                                                  NULL,
                                                  NULL,
                                                  classRoute,
                                                  &state_data->header_metadata,
                                                  sizeof(header_metadata_t),
                                                  isSrc? payload:NULL,
                                                  packet_model_payload_bytes );
              _data_model.postMultiCollectivePacket (state_data->pkt[1],
                                                     NULL,
                                                     NULL,
                                                     classRoute,
                                                     &state_data->data_metadata,
                                                     sizeof(data_metadata_t),
                                                     isSrc? ((char*)payload) + packet_model_payload_bytes :NULL,
                                                     length - packet_model_payload_bytes );
            }
          } // T_Msgdata_support==false
          else //T_Msgdata_support==true
          {
/*
            // Get the msginfo buffer/length and validate (assert) inputs
            void* msgdata = (void*)mcast->msginfo;

            PAMI_assert(multicast_model_msgcount_max >= mcast->msgcount);

            size_t msglength = mcast->msgcount * sizeof(pami_quad_t);
            size_t total = length + msglength;
            // Post the multicast to the device in one or more packets
            if (total <= packet_model_payload_bytes ) // one packet
            {
              // pack msginfo and payload into one (single) packet

              state_data->iov[0].iov_base = msgdata;
              state_data->iov[0].iov_len  = msglength;
              state_data->iov[1].iov_base = payload;
              state_data->iov[1].iov_len  = length;

              _header_model.postCollectivePacket (state_data->pkt[0],
                                                  NULL,
                                                  NULL,
                                                  classRoute,
                                                  &state_data->header_metadata,
                                                  sizeof(header_metadata_t),
                                                  isSrc?state_data->iov:(iovec*)NULL);
            }
            else // > one packet
            {
              // first packet contains msgdata
              state_data->iov[0].iov_base = msgdata;
              state_data->iov[0].iov_len  = msglength;
              state_data->iov[1].iov_base = payload;
              state_data->iov[1].iov_len  = MIN(length, (packet_model_payload_bytes  - msglength));
              _header_model.postCollectivePacket (state_data->pkt[0],
                                                  NULL,
                                                  NULL,
                                                  classRoute,
                                                  &state_data->header_metadata,
                                                  sizeof(header_metadata_t),
                                                  isSrc?state_data->iov:(iovec*)NULL);

              _data_model.postMultiCollectivePacket (state_data->pkt[1],
                                                     NULL,
                                                     NULL,
                                                     classRoute,
                                                     &state_data->data_metadata,
                                                     sizeof(data_metadata_t),
                                                     isSrc?(char*)payload + state_data->iov[1].iov_len:NULL,
                                                     length - state_data->iov[1].iov_len);
            }
*/
          } // T_Msgdata_support==true
        } // T_PWQ_support==false

        TRACE_FORMAT( "dispatch %zu, connection_id %#X exit",
                      mcast->dispatch, mcast->connection_id);

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;

      }; // MulticastModel::postMulticast_impl


      ///
      /// \brief Multicast header dispatch.
      ///
      ///
      /// \see PAMI::Device::Interface::RecvFunction_t
      ///
      template <bool T_Allsided, bool T_Msgdata_support, bool T_PWQ_support>
      int MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>::dispatch_header (void   * metadata,
                                                                                         void   * payload,
                                                                                         size_t   bytes,
                                                                                         void   * arg,
                                                                                         void   * cookie)
      {
        TRACE_FN_ENTER();
        TRACE_HEXDATA(metadata, 16);
        TRACE_HEXDATA(payload, 32);

        header_metadata_t * m = (header_metadata_t *)metadata;
        char * p;
        char * msg = p = (char*) payload;

        MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support> * model = (MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support> *) arg;
        TRACE_FORMAT( "dispatch model %p, task id %u", model, model->_task_id);

        size_t data_length = m->sndlen;

        if (T_Msgdata_support == true)
        {
          // Adjust the payload past the msgdata
          size_t msglength = m->msgcount * sizeof(pami_quad_t);
          data_length -= msglength;
          bytes       -= msglength;
          p += msglength;
        }

        // Allocate/retrieve a state object!
        state_data_t *state_data;

        if (T_Allsided) // all participants should have a saved state.
        {
          state_data = model->_connection[m->connection_id]; //model->_connection.get(m->connection_id);
          TRACE_FORMAT( "state_data %p, m->connection_id %u",state_data,m->connection_id);
          PAMI_assert(state_data);
        }
        else // active message, only the root has a state object, non-root will dispatch.
        {
          // The root already has a state object, retrieve it
          if (m->root == model->_task_id)
          {
            state_data = model->_connection[m->connection_id]; //model->_connection.get(m->connection_id);
            TRACE_FORMAT( "state_data %p, m->connection_id %u",state_data,m->connection_id);
            PAMI_assert(state_data);
          }
          else //allocate one
          {
            state_data = model->allocateState ();
            TRACE_FORMAT( "state_data %p, m->connection_id %u",state_data,m->connection_id);
            state_data->header_metadata.connection_id   = m->connection_id;
            state_data->header_metadata.msgcount        = m->msgcount;
            state_data->header_metadata.root            = m->root;
            state_data->header_metadata.sndlen          = m->sndlen;

            // Set the connection state
            //          PAMI_assert(model->_connection.get(connection_id) == NULL);
            //          model->_connection.set(connection_id, (void *)state_data);
            PAMI_assert(model->_connection[m->connection_id] == NULL);
            model->_connection[m->connection_id] = state_data;
            TRACE_FORMAT( "dispatch state_data %p, model->_connection[m->connection_id %u] %p",state_data,m->connection_id,model->_connection[m->connection_id]);

            state_data->rcvpwq = NULL;
            state_data->cb_done.function = NULL;
            state_data->cb_done.clientdata = NULL;

            // Invoke the registered dispatch function.
            model->_dispatch_function(model->_context,                  // context
                                      (pami_quad_t*)msg,                // Msgdata
                                      m->msgcount,                      // Count of msgdata
                                      m->connection_id,                 // Connection ID of data
                                      m->root,                          // Sending task/root
                                      m->sndlen,                        // Length of data sent
                                      model->_dispatch_arg,                    // Opaque dispatch arg
                                      &state_data->remaining_length,          // [out] Length of data to receive
                                      (pami_pipeworkqueue_t**) &state_data->rcvpwq,// [out] Where to put recv data
                                      &state_data->cb_done                   // [out] Completion callback to invoke when data received
                                     );
            state_data->buffer = state_data->rcvpwq ? (uint8_t*)state_data->rcvpwq->bufferToProduce() : NULL;

            TRACE_FORMAT( "after dispatch remaining_length %zu, pwq %p", state_data->remaining_length, state_data->rcvpwq);

          }
        }

        TRACE_FORMAT( "cookie = %p, root = %d, bytes = %zu remaining = %zd, sndlen = %d, connection id %u/%#X", cookie, (m->root), bytes,  state_data->remaining_length, m->sndlen, m->connection_id, m->connection_id);

        if (T_PWQ_support == false)
        {
          // If you're receiving data, it must all be ready in the pwq.
          PAMI_assertf(((state_data->rcvpwq == NULL) && (state_data->remaining_length == 0)) || (state_data->rcvpwq && (state_data->rcvpwq->bytesAvailableToProduce() >= state_data->remaining_length)), "state_data->rcvpwq->bytesAvailableToProduce() %zd,state_data->remaining_length %zd\n", state_data->rcvpwq ? state_data->rcvpwq->bytesAvailableToProduce() : -1, state_data->remaining_length);
        }

        model->processData(state_data, (uint8_t*) payload, bytes);

        TRACE_FN_EXIT();
        return 0;
      }; // PAMI::Device::MU::MulticastModel::dispatch_header

      ///
      /// \brief Multicast data (after header) dispatch.
      ///
      ///
      /// \see PAMI::Device::Interface::RecvFunction_t
      ///
      template <bool T_Allsided, bool T_Msgdata_support, bool T_PWQ_support>
      int MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>::dispatch_data   (void   * metadata,
                                                                                         void   * payload,
                                                                                         size_t   bytes,
                                                                                         void   * arg,
                                                                                         void   * cookie)
      {
        TRACE_FN_ENTER();
        TRACE_HEXDATA(metadata, 16);
        TRACE_HEXDATA(payload, 32);

        data_metadata_t * m = (data_metadata_t *)metadata;

        MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support> * model = (MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support> *) arg;

        TRACE_FORMAT( "cookie = %p, bytes = %zu, connection id %u/%#X", cookie, bytes, m->connection_id, m->connection_id);

        // Retrieve a state object
        state_data_t *state_data = model->_connection[m->connection_id]; //model->_connection.get(connection_id);
        PAMI_assert(state_data);

        model->processData(state_data, (uint8_t*) payload, bytes);

        TRACE_FN_EXIT();
        return 0;
      }; // PAMI::Device::MU::MulticastModel::dispatch_data

      template <bool T_Allsided, bool T_Msgdata_support, bool T_PWQ_support>
      inline void MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>::processData   (state_data_t * state_data,
                                                                                               uint8_t * payload,
                                                                                               size_t    bytes)
      {
        TRACE_FN_ENTER();
        header_metadata_t   *  header = &state_data->header_metadata;
        TRACE_FORMAT( "state_data %p, connection id %u, payload %p, bytes %zu, remaining length %zu, sndlen %u",
                      state_data, header->connection_id, payload, bytes, state_data->remaining_length, header->sndlen);
        TRACE_HEXDATA(header, sizeof(*header));
        TRACE_HEXDATA(payload, bytes);


        // Number of bytes left to copy into the destination buffer
        size_t nleft = state_data->remaining_length;

        // Number of bytes left to copy from this packet
        if (nleft > bytes) nleft = bytes;

        if (nleft) // copy data and update receive state_data
        {
          TRACE_FORMAT( "memcpy(%p,%p,%zu)", state_data->buffer, payload, nleft);
          memcpy (state_data->buffer, payload, nleft);

          // Update the receive state_data
          state_data->buffer += nleft;
          state_data->remaining_length -= nleft;
          state_data->rcvpwq->produceBytes(nleft);
        }
        else ;  /// toss unwanted data?

        // Decrement the original bytes sent by the bytes just received...
        // this is different from expected length because the dispatch might
        // not receive all data sent (tossing some).
        header->sndlen -= MIN(header->sndlen, (uint32_t)bytes);

        if (header->sndlen == 0)
        {
          TRACE_FORMAT( "done cb_done function %p, clientdata %p", state_data->cb_done.function, state_data->cb_done.clientdata);
          _connection.erase(header->connection_id);
          //_connection.clear(header->connection_id);

          // Invoke the receive done callback.
          if (state_data->cb_done.function)
            state_data->cb_done.function (_context,
                                          state_data->cb_done.clientdata,
                                          PAMI_SUCCESS);

          if (_task_id != header->root) freeState(state_data);
        }

        TRACE_FN_EXIT();
        return ;
      }; // PAMI::Device::MU::MulticastModel::processData

      template <bool T_Allsided, bool T_Msgdata_support, bool T_PWQ_support>
      pami_result_t MulticastModel<T_Allsided, T_Msgdata_support, T_PWQ_support>::registerMcastRecvFunction_impl(int                         dispatch_id,
                                                                                                                 pami_dispatch_multicast_function func,
                                                                                                                 void                       *arg)
      {
        TRACE_FN_ENTER();
        pami_result_t status = PAMI_SUCCESS;
        TRACE_FORMAT( "id %d, func %p, arg %p", dispatch_id, func, arg);
        // Even allsided MU gets dispatched (to dispatch_data()/dispatch_header()).  It just doesn't use _dispatch_function().
        {

          _dispatch_function = func;
          _dispatch_arg = arg;

          TRACE_FORMAT( "register data model dispatch %d", dispatch_id);
          status = _data_model.init (dispatch_id,
                                     dispatch_data, this,
                                     NULL, NULL);
          TRACE_FORMAT( "data model status = %d", status);

          if (status == PAMI_SUCCESS)
          {
            TRACE_FORMAT( "register header  model dispatch %d", dispatch_id);
            status = _header_model.init (dispatch_id,
                                         dispatch_header, this,
                                         NULL, NULL);
            TRACE_FORMAT( "header model status = %d", status);
          }

        }
        TRACE_FN_EXIT();
        return status;
      };
    };
  };
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG

#endif // __components_devices_bgq_mu2_model_Multicast_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
