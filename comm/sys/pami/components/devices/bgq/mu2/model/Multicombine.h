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
/// \file components/devices/bgq/mu2/model/Multicombine.h
/// \brief Simple Multicombine protocol
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __components_devices_bgq_mu2_model_Multicombine_h__
#define __components_devices_bgq_mu2_model_Multicombine_h__

#include <map>

#include "components/devices/MulticombineModel.h"

#include "components/devices/bgq/mu2/model/AllreducePacketModel.h"
#include "components/devices/bgq/mu2/model/ReducePacketModel.h"
#include "components/memory/MemoryAllocator.h"
#include "components/devices/bgq/mu2/model/MU_Collective_OP_DT_Table.h"

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
      // \class MulticombineModel
      // \brief MU collective Multicombine interface
      // \details
      //   - active message model
      //   - uses MU memfifo
      //   - one destination task per node
      //   - does not fully support PipeWorkQueue (Multicombine_model_available_buffers_only)
      ///////////////////////////////////////////////////////////////////////////////
      template <class T_PacketModel, bool T_Msgdata_support, bool T_PWQ_support>
      class MulticombineModel : public Interface::MulticombineModel < MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>, MU::Context, 2048 /*sizeof(state_data_t)*/ >
      {

        protected:

          typedef uint8_t packet_state_t[T_PacketModel::packet_model_state_bytes];

          // Metadata passed in the (single-packet) header
          typedef struct __attribute__((__packed__))
          {
            uint32_t              connection_id;  ///< Collective connection id
            uint32_t              root;           ///< Root of the collective
            uint32_t              sndlen;         ///< Number of bytes of application data
          } header_metadata_t;

          // State (request) implementation.  Callers should use uint8_t[MU::MulticombineModel::sizeof_msg]
          typedef struct
          {
            packet_state_t          pkt;          ///< packet send state memory

            header_metadata_t       header_metadata;

            size_t                  remaining_length;
            PAMI::PipeWorkQueue   * rcvpwq;
            uint8_t               * buffer;
            pami_callback_t         cb_done;
          } state_data_t;


        public:

          /// \see PAMI::Device::Interface::MulticombineModel::~MulticombineModel
          ~MulticombineModel ()
          {
          };

          /// \brief Multicombine model constants/attributes
//      static const bool   Multicombine_model_all_sided               = false;
          static const bool   Multicombine_model_active_message          = true;
          static const bool   Multicombine_model_available_buffers_only  = true;

          static const size_t sizeof_msg                              = 2048 /*sizeof(state_data_t)*/;
          static const size_t packet_model_payload_bytes              = T_PacketModel::packet_model_payload_bytes;
          static const size_t packet_model_immediate_bytes              = T_PacketModel::packet_model_immediate_bytes; //\todo not supported yet

          static const size_t Multicombine_model_msgcount_max            = (packet_model_payload_bytes / sizeof(pami_quad_t));
          static const size_t Multicombine_model_bytes_max               = (uint32_t) - 1; // protocol_metadata_t::sndlen
          static const size_t Multicombine_model_connection_id_max       = (uint32_t) - 1; // protocol_metadata_t::connection_id \todo 64 bit?

          /// \see PAMI::Device::Interface::MulticombineModel::registermcombRecvFunction
          pami_result_t registermcombRecvFunction_impl(int                        dispatch_id,
                                                       pami_dispatch_multicast_function func,
                                                       void                      *arg);

          pami_result_t postMulticombineImmediate_impl(size_t                   client,
                                                       size_t                   context, 
                                                       pami_multicombine_t    * mcomb,
                                                       void                   * devinfo=NULL) 
          {
            return PAMI_ERROR;
          }

          /// \see PAMI::Device::Interface::MulticombineModel::postMulticombine
          pami_result_t postMulticombine_impl(uint8_t (&state)[MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>::sizeof_msg],
                                              size_t               client,
                                              size_t               context,					      
                                              pami_multicombine_t *mcomb,
                                              void                *devinfo = NULL);

        protected:
          /// \brief Map PAMI dt/op to MU op
          const uint8_t mu_op(pami_dt dt, pami_op op);

          /// \brief Map PAMI dt to a MU datatype size
          const size_t  mu_size(pami_dt dt);

          static unsigned getNextDispatch()
          {
            static unsigned _id = 0x81;
            TRACE_FN_ENTER();
            TRACE_FORMAT("%u\n", (_id + 1) & 0xFF);
            TRACE_FN_EXIT();
            return ++_id & 0xFF;
          }
          /// \brief MU dispatch function
          inline static int dispatch_data (void   * metadata,
                                           void   * payload,
                                           size_t   bytes,
                                           void   * arg,
                                           void   * cookie);
          /// \brief Process user data packets
          inline void processData   (state_data_t *state_data,
                                     uint8_t      *payload,
                                     size_t        bytes);

        private:
          MU::Context                                & _device;
          pami_context_t                               _context;
          pami_task_t                                  _task_id;

          T_PacketModel                         _data_model;

//        T_Connection                                 _connection; ///\todo ConnectionArray isn't appropriate...
          std::map<unsigned, state_data_t * >             _connection;

        public:

          /// \see PAMI::Device::Interface::MulticombineModel::MulticombineModel
          MulticombineModel (pami_client_t client, pami_context_t context, MU::Context & device, pami_result_t &status) :
              Interface::MulticombineModel < MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>, MU::Context, 2048 /*sizeof(state_data_t)*/ > (device, status),
              _device (device),
              _context(context),
              _task_id(__global.mapping.task()),
              _data_model (device)
              //        _connection (device)
          {
            TRACE_FN_ENTER();
            // ----------------------------------------------------------------
            // Compile-time assertions
            // ----------------------------------------------------------------

            // This protocol only works with reliable networks.
            COMPILE_TIME_ASSERT(T_PacketModel::reliable_packet_model == true);

            // This protocol only works with deterministic models.
            COMPILE_TIME_ASSERT(T_PacketModel::deterministic_packet_model == true);

            // Assert that the size of the packet metadata area is large
            // enough to transfer the eager match information. This is used in the
            // various postMultiPacket() calls to transfer long header and data
            // messages.
            COMPILE_TIME_ASSERT(sizeof(pami_endpoint_t) <= T_PacketModel::packet_model_multi_metadata_bytes);

            // Assert that the size of the packet payload area is large
            // enough to transfer a single virtual address. This is used in
            // the postPacket() calls to transfer the ack information.
            COMPILE_TIME_ASSERT(sizeof(void *) <= T_PacketModel::packet_model_payload_bytes);

            // ----------------------------------------------------------------
            // Compile-time assertions (end)
            // ----------------------------------------------------------------
            unsigned dispatch_id = getNextDispatch();

            status = _data_model.init (dispatch_id,
                                       dispatch_data, this,
                                       NULL, NULL);
            TRACE_FORMAT( "data model status = %d\n", status);
            TRACE_FN_EXIT();
          }
      };

      template <class T_PacketModel, bool T_Msgdata_support, bool T_PWQ_support>
      inline pami_result_t MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>::postMulticombine_impl(uint8_t (&state)[MulticombineModel::sizeof_msg],
	  size_t               client,
          size_t               context,
          pami_multicombine_t *mcomb,
          void                *devinfo)
      {
        TRACE_FN_ENTER();

        uint32_t classRoute = (uint32_t)(size_t)devinfo; // convert platform independent void* to bgq uint32_t classroute
        // MU class routes start at 0 but ResourceManager adds 1 to avoid NULL-looking device info.
        PAMI_assert(classRoute);
        classRoute -= 1; 

        TRACE_FORMAT( "connection_id %#X, class route %#X\n", mcomb->connection_id, classRoute);

        PAMI::PipeWorkQueue *pwq = (PAMI::PipeWorkQueue *)mcomb->data;
        size_t length = pwq ? pwq->bytesAvailableToConsume() : 0;

        if (T_PWQ_support == false)
          {
            // If you're sending data, it must all be ready in the pwq.
            //PAMI_assert(
          }
        else
          {
            PAMI_abortf("T_PWQ_support not supported yet\n");
          }

        TRACE_FORMAT( "<%p>:MU::MulticombineModel::postMulticombine_impl() connection_id %#X, count %zu, bytes %zu, pwq %p/%p\n",
                      this, mcomb->connection_id,
                      mcomb->count, length, pwq, pwq ? pwq->bufferToConsume() : NULL);

        state_data_t *state_data = (state_data_t*) & state;

        void* payload = NULL;

        PAMI_assert(((Topology*)mcomb->data_participants)->isRankMember(_task_id));

        Topology* resultsTopology = (Topology*)mcomb->results_participants;

        PAMI_assert(resultsTopology->size() == ((Topology*)mcomb->data_participants)->size() || resultsTopology->size() == 1); /// \todo based on T_PacketModel?

        if (resultsTopology->isRankMember(_task_id))
          {
            state_data->remaining_length = length;  // I get output data
          }
        else state_data->remaining_length = length;  // I don't get any data

        pami_task_t root = resultsTopology->index2Rank(0);

        state_data->header_metadata.connection_id = mcomb->connection_id;
        state_data->header_metadata.root          = root;
        state_data->header_metadata.sndlen        = length;

        state_data->cb_done = mcomb->cb_done;
        state_data->rcvpwq = (PAMI::PipeWorkQueue*)mcomb->results;
        state_data->buffer = state_data->rcvpwq ? (uint8_t*)state_data->rcvpwq->bufferToProduce() : NULL;

        // Set the connection state
//        PAMI_assert(_connection.get(mcomb->connection_id) == NULL);
//        _connection.set(mcomb->connection_id, (void *)state_data);
        PAMI_assert(_connection[mcomb->connection_id] == NULL);
        _connection[mcomb->connection_id] = state_data;

        if (T_PWQ_support == false)
          {
            if (length)
              {
                payload = (void*)pwq->bufferToConsume();
                pwq->consumeBytes(length);
              }

            // Post the Multicombine to the device in one or more packets
            if (length <= packet_model_payload_bytes) // one packet
              {
                _data_model.postCollectivePacket (state_data->pkt,
                                                  NULL,
                                                  NULL,
                                                  classRoute,
                                                  root,
                                                  mu_op(mcomb->dtype, mcomb->optor),
                                                  mu_size(mcomb->dtype),
                                                  &state_data->header_metadata,
                                                  sizeof(header_metadata_t),
                                                  payload,
                                                  length);
              }
            else  // > one packet of payload
              {
                _data_model.postMultiCollectivePacket (state_data->pkt,
                                                       NULL,
                                                       NULL,
                                                       classRoute,
                                                       root,
                                                       mu_op(mcomb->dtype, mcomb->optor),
                                                       mu_size(mcomb->dtype),
                                                       &state_data->header_metadata,
                                                       sizeof(header_metadata_t),
                                                       payload,
                                                       length);
              }
          } // T_PWQ_support==false

        TRACE_FORMAT( "<%p>:MU::MulticombineModel::postMulticombine_impl() connection_id %#X exit\n",
                      this, mcomb->connection_id);

        TRACE_FN_EXIT();
        return PAMI_SUCCESS;

      }; // MulticombineModel::postMulticombine_impl


      ///
      /// \brief Multicombine data (after header) dispatch.
      ///
      ///
      /// \see PAMI::Device::Interface::RecvFunction_t
      ///
      template <class T_PacketModel, bool T_Msgdata_support, bool T_PWQ_support>
      int MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>::dispatch_data   (void   * metadata,
          void   * payload,
          size_t   bytes,
          void   * arg,
          void   * cookie)
      {
        TRACE_FN_ENTER();
        TRACE_HEXDATA(metadata, 16);
        TRACE_HEXDATA(payload, 32);

        header_metadata_t * m = (header_metadata_t *)metadata;

        MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support> * model = (MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support> *) arg;

        TRACE_FORMAT( "<%p>:MulticombineModel::dispatch(),cookie = %p, root = %d, bytes = %zu/%d, connection id %u/%#X\n", arg, cookie, (m->root), bytes, m->sndlen, m->connection_id, m->connection_id);

        // Retrieve a state object
        state_data_t *state_data = model->_connection[m->connection_id]; //model->_connection.get(connection_id);
        PAMI_assert(state_data);

        model->processData(state_data, (uint8_t*)payload, bytes);

        TRACE_FN_EXIT();
        return 0;
      }; // PAMI::Device::MU::MulticombineModel::dispatch_data

      template <class T_PacketModel, bool T_Msgdata_support, bool T_PWQ_support>
      inline void MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>::processData   (state_data_t * state_data,
          uint8_t * payload,
          size_t    bytes)
      {
        TRACE_FN_ENTER();
        header_metadata_t   *  header = &state_data->header_metadata;
        TRACE_FORMAT( "<%p>:MulticombineModel::processData() state_data %p, connection id %u, payload %p, bytes %zu, remaining length %zu\n",
                      this, state_data, header->connection_id, payload, bytes, state_data->remaining_length);


        // Number of bytes left to copy into the destination buffer
        size_t nleft = state_data->remaining_length;

        // Number of bytes left to copy from this packet
        if (nleft > bytes) nleft = bytes;

        if (nleft) // copy data and update receive state_data
          {
            TRACE_FORMAT( "<%p>:MulticombineModel::processData memcpy(%p,%p,%zu)\n", this, state_data->buffer, payload, nleft);
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
            _connection.erase(header->connection_id);
            //_connection.clear(header->connection_id);

            // Invoke the receive done callback.
            if (state_data->cb_done.function)
              state_data->cb_done.function (_context,
                                            state_data->cb_done.clientdata,
                                            PAMI_SUCCESS);
          }

        TRACE_FN_EXIT();
        return ;
      }; // PAMI::Device::MU::MulticombineModel::processData

#undef TRACE
#define TRACE(x) //fprintf x

#if 0  // extra debug - trace the op/dt strings
#define TRACE_STRINGS 1
// Extra debug trace the strings
      inline static const char* _mu_op_string_(uint8_t index)
      {
        const char* string[] =
        {
          "MUHWI_COLLECTIVE_OP_CODE_AND                ", //MUHWI_COLLECTIVE_OP_CODE_AND
          "MUHWI_COLLECTIVE_OP_CODE_OR                 ", //MUHWI_COLLECTIVE_OP_CODE_OR
          "MUHWI_COLLECTIVE_OP_CODE_XOR                ", //MUHWI_COLLECTIVE_OP_CODE_XOR
          "INVALID                                     ", // invalid
          "MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD       ", //MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD
          "MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_MIN       ", //MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_MIN
          "MUHWI_COLLECTIVE_OP_CODE_UNSIGNED MAX       ", //MUHWI_COLLECTIVE_OP_CODE_UNSIGNED MAX
          "INVALID                                     ", // invalid
          "MUHWI_COLLECTIVE_OP_CODE_SIGNED_ADD         ", //MUHWI_COLLECTIVE_OP_CODE_SIGNED_ADD
          "MUHWI_COLLECTIVE_OP_CODE_SIGNED_MIN         ", //MUHWI_COLLECTIVE_OP_CODE_SIGNED_MIN
          "MUHWI_COLLECTIVE_OP_CODE_SIGNED_MAX         ", //MUHWI_COLLECTIVE_OP_CODE_SIGNED_MAX
          "INVALID                                     ", // invalid
          "MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD ", //MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD
          "MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_MIN ", //MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_MIN
          "MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_MAX ", //MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_MAX
          "INVALID                                     "  // invalid
        };
        return string[index];
      }
      inline static const char* _dt_string_(pami_dt index)
      {
        const char* string[PAMI_DT_COUNT] =
        {
         "PAMI_BYTE              " ,
         "PAMI_SIGNED_CHAR       " ,
         "PAMI_SIGNED_SHORT      " ,
         "PAMI_SIGNED_INT        " ,
         "PAMI_SIGNED_LONG       " ,
         "PAMI_SIGNED_LONG_LONG  " ,
         "PAMI_UNSIGNED_CHAR     " ,
         "PAMI_UNSIGNED_SHORT    " ,
         "PAMI_UNSIGNED_INT      " ,
         "PAMI_UNSIGNED_LONG     " ,
         "PAMI_UNSIGNED_LONG_LONG" ,
         "PAMI_FLOAT             " ,
         "PAMI_DOUBLE            " ,
         "PAMI_LONG_DOUBLE       " ,
         "PAMI_LOGICAL1          " ,
         "PAMI_LOGICAL2          " ,
         "PAMI_LOGICAL4          " ,
         "PAMI_LOGICAL8          " ,
         "PAMI_SINGLE_COMPLEX    " ,
         "PAMI_DOUBLE_COMPLEX    " ,
         "PAMI_LOC_2INT          " ,
         "PAMI_LOC_2FLOAT        " ,
         "PAMI_LOC_2DOUBLE       " ,
         "PAMI_LOC_SHORT_INT     " ,
         "PAMI_LOC_FLOAT_INT     " ,
         "PAMI_LOC_DOUBLE_INT    " ,
         "PAMI_LOC_LONG_INT      " ,
         "PAMI_LOC_LONGDOUBLE_INT"
        };
        return string[index];
      }
      inline static const char* _op_string_(pami_op index)
      {
        const char* string[PAMI_OP_COUNT] =
        {
          "PAMI_COPY  ",        // PAMI_COPY
          "PAMI_NOOP  ",        // PAMI_NOOP
          "PAMI_MAX   ",        // PAMI_MAX
          "PAMI_MIN   ",        // PAMI_MIN
          "PAMI_SUM   ",        // PAMI_SUM
          "PAMI_PROD  ",        // PAMI_PROD
          "PAMI_LAND  ",        // PAMI_LAND
          "PAMI_LOR   ",        // PAMI_LOR
          "PAMI_LXOR  ",        // PAMI_LXOR
          "PAMI_BAND  ",        // PAMI_BAND
          "PAMI_BOR   ",        // PAMI_BOR
          "PAMI_BXOR  ",        // PAMI_BXOR
          "PAMI_MAXLOC",        // PAMI_MAXLOC
          "PAMI_MINLOC",        // PAMI_MINLOC
        };
        return string[index];
      }
#define mu_op_string(x) _mu_op_string_(x)
#define op_string(x)    _op_string_(x)
#define dt_string(x)    _dt_string_(x)
#undef TRACE
#define TRACE(x) //fprintf x
#else
#define mu_op_string(x) "untraced"
#define op_string(x) "untraced"
#define dt_string(x) "untraced"
#endif

      template <class T_PacketModel, bool T_Msgdata_support, bool T_PWQ_support>
      inline const uint8_t   MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>::mu_op(pami_dt dt, pami_op op)
      {
#if ASSERT_LEVEL > 0
        // This table validates our assumptions about the layout of pami_op
        const pami_op op_check[PAMI_OP_COUNT] =
        {
          PAMI_COPY   ,
          PAMI_NOOP   ,
          PAMI_MAX    ,
          PAMI_MIN    ,
          PAMI_SUM    ,
          PAMI_PROD   ,
          PAMI_LAND   ,
          PAMI_LOR    ,
          PAMI_LXOR   ,
          PAMI_BAND   ,
          PAMI_BOR    ,
          PAMI_BXOR   ,
          PAMI_MAXLOC ,
          PAMI_MINLOC
        };
#endif

        TRACE((stderr, "Multicombine::mu_op(%d, %d) = %#X, %s->%s\n", dt, op, mu_collective_op_table[dt][op], op_string(op), mu_op_string((mu_collective_op_table[dt][op]) >> 4)));
        PAMI_assert_debugf(op_check[op] == op, "op_check[op] %u == op %u\n", op_check[op], op);
        return(mu_collective_op_table[dt][op]);
      }

      template <class T_PacketModel, bool T_Msgdata_support, bool T_PWQ_support>
      inline const size_t    MulticombineModel<T_PacketModel, T_Msgdata_support, T_PWQ_support>::mu_size(pami_dt dt)
      {
#if ASSERT_LEVEL > 0
        const pami_dt dt_check[PAMI_DT_COUNT] =
        {
        // This table validates our assumptions about the layout of pami_dt
          PAMI_BYTE               ,

          PAMI_SIGNED_CHAR        ,
          PAMI_SIGNED_SHORT       ,
          PAMI_SIGNED_INT         ,
          PAMI_SIGNED_LONG        ,
          PAMI_SIGNED_LONG_LONG   ,

          PAMI_UNSIGNED_CHAR      ,
          PAMI_UNSIGNED_SHORT     ,
          PAMI_UNSIGNED_INT       ,
          PAMI_UNSIGNED_LONG      ,
          PAMI_UNSIGNED_LONG_LONG ,

          PAMI_FLOAT              ,
          PAMI_DOUBLE             ,
          PAMI_LONG_DOUBLE        ,

          PAMI_LOGICAL1           ,
          PAMI_LOGICAL2           ,
          PAMI_LOGICAL4           ,
          PAMI_LOGICAL8           ,

          PAMI_SINGLE_COMPLEX     ,
          PAMI_DOUBLE_COMPLEX     ,

          PAMI_LOC_2INT           ,
          PAMI_LOC_2FLOAT         ,
          PAMI_LOC_2DOUBLE        ,
          PAMI_LOC_SHORT_INT      ,
          PAMI_LOC_FLOAT_INT      ,
          PAMI_LOC_DOUBLE_INT     ,
          PAMI_LOC_LONG_INT       ,
          PAMI_LOC_LONGDOUBLE_INT
        };
#endif

        TRACE((stderr, "Multicombine::mu_size(%d) = %zu %s\n", dt, mu_collective_size_table[dt], dt_string(dt)));
        PAMI_assert_debugf(dt_check[dt] == dt, "dt_check[dt] %u == dt %u\n", dt_check[dt], dt);
        return(mu_collective_size_table[dt]);
      }
#undef TRACE
    };
  };
};

#undef DO_TRACE_ENTEREXIT
#undef DO_TRACE_DEBUG


#endif // __components_devices_bgq_mu2_model_Multicombine_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
