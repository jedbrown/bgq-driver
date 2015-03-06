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
/// \file p2p/protocols/send/datagram/Datagram.h
/// \brief Datagram send protocol for reliable devices.
///
/// The Datagram class defined in this file uses C++ templates and
/// the device "message" interface - which also uses C++ templates.
///
/// C++ templates require all source code to be #include'd from a header file.
///
#ifndef __p2p_protocols_send_datagram_Datagram_h__
#define __p2p_protocols_send_datagram_Datagram_h__

#include "p2p/protocols/Send.h"
#include "p2p/protocols/send/datagram/DTimer.h"

#include "components/connection/ConnectionArray.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

#define WINDOW_SIZE 8
#define STD_RATE_RECV 10000
#define STD_RATE_SEND 10000

namespace PAMI {
namespace Protocol {
namespace Send {

///
/// \brief Datagram simple send protocol class for reliable network devices.
///
/// \tparam T_Model   Template packet model class
/// \tparam T_Device  Template packet device class
///
/// \see PAMI::Device::Interface::PacketModel
/// \see PAMI::Device::Interface::PacketDevice
///
template < class T_Model, class T_Device, bool T_LongHeader = true, class T_Connection = ConnectionArray<T_Device> > class Datagram :
        public PAMI::Protocol::Send::Send {
public:
        typedef Datagram < T_Model, T_Device, T_LongHeader, T_Connection> DatagramProtocol;

        template <class T_Allocator>
        static DatagramProtocol * generate (size_t                     dispatch,
                                            pami_dispatch_callback_function dispatch_fn,
                                            void                     * cookie,
                                            T_Device                 & device,
                                            T_Allocator              & allocator,
                                            pami_result_t             & result)
        {
          TRACE_ERR((stderr, ">> Datagram::generate()\n"));
          COMPILE_TIME_ASSERT(sizeof(Datagram) <= T_Allocator::objsize);

          Datagram * datagram = (Datagram *) allocator.allocateObject ();
          new ((void *)datagram) Datagram (dispatch, dispatch_fn, cookie, device, result);
          if (result != PAMI_SUCCESS)
          {
            allocator.returnObject (datagram);
            datagram = NULL;
          }

          TRACE_ERR((stderr, "<< Datagram::generate(), datagram = %p, result = %d\n", datagram, result));
          return datagram;
        }



        // ----------------------------------------------------------------------
        // STRUCTURES
        // ----------------------------------------------------------------------

        // COMMMON
        //forward declaration
        class send_state_t;
        struct recv_state_t;

        typedef uint8_t pkt_t[T_Model::packet_model_state_bytes];

        // SUPPORTING SEND IMMEDIATE

        /// \brief Sender-side message copy structure for immediate sends.
        ///
        /// Used to store the message until it is sent
        ///
        typedef struct {
                uint8_t header[T_Model::packet_model_metadata_bytes];
                uint8_t data[T_Model::packet_model_payload_bytes];
                pkt_t pkt;
        } msg_copy_t;

        // SUPPORTING SEND SIMPLE

        // rts information   32 bytes
        struct rts_info_t {
                send_state_t *va_send; ///virtual address sender
                uint32_t msg_id; ///message identifier
                pami_task_t from_taskid; ///task originating message
                pami_task_t dest_taskid; ///task receiving message
                size_t from_contextid; ///context id on task originating message
                size_t dest_contextid; ///context id on task receiving message
                size_t wsize; ///window size fo ack
                size_t wrate; ///ack rate
                size_t bytes; ///total bytes to send
                size_t mbytes; ///total bytes application metadata
        };

        // rts_ack header (metadata)
        struct __attribute__ ((__packed__)) header_ack_t {
                send_state_t *va_send; ///virtual address receiver
                uint32_t msg_id; ///message identifier
                recv_state_t *va_recv; ///virtual address receiver
                size_t wsize; ///window size fo ack
                size_t wrate; ///ack rate
        };

        //data header (metadata)
        struct __attribute__ ((__packed__)) header_metadata_t {
                recv_state_t *va_recv; ///virtual address receiver
                uint32_t msg_id; ///message identifier
                size_t seqno; ///packet sequence number
                size_t bsend; ///bytes send
        };

        // This allows storage for the iovecs to be retained
        // until the message is complete and still take advantage
        // of any iovec size specializations of the Model API
        union vecs_t {
                struct iovec d1[1];
                struct iovec d2[2];
        };

        //receiver status
        struct recv_state_t {
                size_t pkgnum; /// Number of pkg to receive from the origin task.
                header_ack_t ack; /// ack info
                pkt_t pkt; /// Message Object
                DatagramProtocol *datagram; /// Pointer to protocol
                pami_recv_t info; /// Application receive information.
                size_t mbytes; /// Message info bytes
                pami_task_t from_taskid; ///origin task
                size_t from_contextid; ///origin context id
                //char *             msgbuff;                                     /// RTS data buffer
                size_t rpkg; /// received packages

                size_t fbytes; /// short package bytes
                size_t rate; /// rate timers
                size_t bytes; /// bytes to receive

                char pmsgbuf[2][PAMI::Device::ProgressFunctionMdl::sizeof_msg]; ///timers' buffer
                DTimer timer1; ///timer 1
                DTimer timer0; ///timer 2
                size_t wmaxseq; ///Max sequence id in control mechanism (CM)
                size_t wminseq; ///Min sequence id in CM
                size_t *lost_list; ///Lost list array
                size_t *rcv_list; ///Received  list array
                size_t nlost; ///number of lost packages
                size_t last_nlost; /// last number of lost elements
                bool fseq; ///Flag used to record minimun wminseq
                vecs_t vecs;
        };

        //package structure
        struct package_t {
                header_metadata_t header; ///header
                pkt_t pkt; ///packet to send
        };

        //window structure
        struct window_t {
                send_state_t *va_send; ///virtual address sender
                package_t pkg[WINDOW_SIZE]; ///Array of packages
        };

        //send_state_class
        class send_state_t : public Queue::Element {
public:

                size_t pkgnum; /// Number of pkg to send from the origin task.
                recv_state_t *va_recv; /// Receiver Virtual Address
                uint32_t msg_id; /// Message identifier
                char *send_buffer; /// Send Buffer address
                pkt_t pkt; /// packet
                rts_info_t rts; /// RTS struct info

                header_metadata_t header; /// aux header

                pami_event_function cb_data; /// Callback to execute when data have been sent
                pami_event_function cb_rts; /// Callback to execute when rts have been sent
                void *pf; /// Pointer to receiver parameters
                void *msginfo; /// Message info
                pami_event_function local_fn; /// Local Completion callback.
                pami_event_function remote_fn; /// Remote Completion callback.
                void *cookie; /// Cookie

                size_t rate; /// used to setup timers rate
                size_t pkgsend; /// number of packages send
                size_t last_seqno; /// last sequence number saved
                size_t *lost_list; /// lost list array
                size_t nlost; /// number of lost elements
                size_t fbytes; /// short_data bytes, first package

                char pmsgbuf[2][PAMI::Device::ProgressFunctionMdl::sizeof_msg]; /// timers' buffer
                DTimer timer1; ///timer 1
                DTimer timer0; ///timer 2
                DatagramProtocol *datagram; ///pointer to protocol

                window_t window[2]; ///define 2 windows
                vecs_t vecs;
        };

        // ----------------------------------------------------------------------
        // CONSTRUCTOR
        // ----------------------------------------------------------------------

        ///
        /// \brief Datagram send protocol constructor.
        ///
        /// \param[in]  dispatch     Dispatch identifier
        /// \param[in]  dispatch_fn  Dispatch callback function
        /// \param[in]  cookie       Opaque application dispatch data
        /// \param[in]  device       Device that implements the message interface
        /// \param[in]  origin_task  Origin task identifier
        /// \param[in]  context      Communication context
        /// \param[out] status       Constructor status
        ///
        Datagram(size_t dispatch, pami_dispatch_callback_function dispatch_fn,
                        void *cookie, T_Device & device, pami_result_t & status) :
                                PAMI::Protocol::Send::Send(),
                                _rts_model(device),
                                _rts_ack_model(device),
                                _ack_model(device),
                                _data_model(device),
                                _short_data_model(device),
                                _device(device),
                                _from_taskid(__global.mapping.task()),
                                _from_contextid(device.getContextOffset()),
                                _context(device.getContext()),
                                _dispatch_fn(dispatch_fn),
                                _cookie(cookie),
                                _connection(device),
                                _cont(0) {
                // ----------------------------------------------------------------
                // Compile-time assertions
                // ----------------------------------------------------------------

                // Assert that the size of the packet payload area is large
                // enough to transfer a single virtual address. This is used in
                // the postPacket() calls to transfer the ack information.
                COMPILE_TIME_ASSERT(sizeof(void *)
                                <= T_Model::packet_model_payload_bytes);

                //allocate memory
		pami_result_t prc;
		prc = __global.heap_mm->memalign((void **)&_queue, 0,
						sizeof(*_queue) * _device.peers());
		PAMI_assertf(prc == PAMI_SUCCESS, "alloc of _queue failed");

                //Initializing queue
                for (size_t i = 0; i < _device.peers(); i++) {
                        new(&_queue[i]) Queue();
                }
                status = _rts_model.init(dispatch, dispatch_rts_direct, this,
                                dispatch_rts_read, this);TRACE_ERR((stderr, "Datagram() _rts [1] status = %d\n", status));
                if (status != PAMI_SUCCESS)
                        abort();

                status = _rts_ack_model.init(dispatch, dispatch_rts_ack_direct, this,
                                dispatch_rts_ack_read, this);TRACE_ERR((stderr, "Datagram() _rts_ack [2] status = %d\n",
                                                status));
                if (status != PAMI_SUCCESS)
                        abort();

                status = _ack_model.init(dispatch, dispatch_ack_direct, this,
                                dispatch_ack_read, this);TRACE_ERR((stderr, "Datagram() _ack [3] status = %d\n", status));
                if (status != PAMI_SUCCESS)
                        abort();

                status = _data_model.init(dispatch, dispatch_data_direct, this,
                                dispatch_data_read, this);TRACE_ERR((stderr, "Datagram() _data [4] status = %d\n",
                                                status));
                if (status != PAMI_SUCCESS)
                        abort();

                status = _short_data_model.init(dispatch, dispatch_short_data_direct,
                                this, dispatch_short_data_read, this);TRACE_ERR((stderr,
                                                "Datagram() _short_data [5] status = %d\n", status));
                if (status != PAMI_SUCCESS)
                        abort();
        }
        ;

        // ----------------------------------------------------------------------
        // DESTRUCTOR
        // ----------------------------------------------------------------------
        virtual ~ Datagram() {
        }
        ;

        // ----------------------------------------------------------------------
        // SEND IMMEDIATE SUPPORT
        // ----------------------------------------------------------------------
        /// \brief Immediate implementation
        ///  This is implemented to copy the message into local
        ///  storage and then call the simple interface with a callback to
        ///  a routine that cleans up the message storage
        ///  Note: there is not attempt to call immediate on the device
        pami_result_t immediate(pami_send_immediate_t * parameters) {
                TRACE_ERR((stderr, ">> Datagram::immediate()\n"));
                pami_result_t result;
                pami_send_t simple_parm;
                // Create storage to save message until sent (have to malloc since unbounded)
		pami_result_t prc;
		prc = __global.heap_mm->memalign((void **)&msg_copy, 0,
								sizeof(*msg_copy));
		PAMI_assertf(prc == PAMI_SUCCESS, "alloc of msg_copy failed");
                memcpy(msg_copy->header, parameters->header.iov_base,
                                parameters->header.iov_len);
                memcpy(msg_copy->data, parameters->data.iov_base,
                                parameters->data.iov_len);

                // Setup parameter structure for simple
                simple_parm.events.cookie = msg_copy;
                simple_parm.events.local_fn = cb_immediate_complete;
                simple_parm.events.remote_fn = NULL;
                simple_parm.send.header.iov_base = msg_copy->header;
                simple_parm.send.header.iov_len = parameters->header.iov_len;
                simple_parm.send.data.iov_base = msg_copy->data;
                simple_parm.send.data.iov_len = parameters->data.iov_len;
                simple_parm.send.dest = parameters->dest;

                // Call the impl directly to avoid the 2nd virtual call
                result = Datagram::simple_impl(&simple_parm);TRACE_ERR((stderr, "<< Datagram::immediate()  returned = %d \n",
                                                result));
                return result;
        }
        ;

        // ----------------------------------------------------------------------
        // SEND SIMPLE PUBLIC METHOD SUPPORT
        // ----------------------------------------------------------------------

        ///
        /// \brief Start a new simple send operation.
        ///
        /// \see PAMI::Protocol::Send::simple
        ///
        virtual pami_result_t simple(pami_send_t * parameters) {
                // This allows immediate to use this without having to call a virtual function
                return simple_impl(parameters);
        }
        ;

protected:
        // ----------------------------------------------------------------------
        // SEND IMMEDIATE CALLBACKS
        // ----------------------------------------------------------------------

        /// \brief Callback for immediate completion to clean up message copy
        static void cb_immediate_complete(pami_context_t context,
                        void *cookie, pami_result_t result) {
                TRACE_ERR((stderr, ">> Datagram::cb_immediate_complete() \n"));
                // Send finished so get rid of copy of message
                msg_copy_t *msg_copy = (msg_copy_t *) cookie;
                __global.heap_mm->free(msg_copy);TRACE_ERR((stderr, "<< Datagram::cb_immediate_complete() \n"));
        }
        ;

        // ----------------------------------------------------------------------
        // UTILITY FUNCTIONS
        // ----------------------------------------------------------------------


        inline send_state_t *allocateSendState() {
                return (send_state_t *) _send_allocator.allocateObject();
        }
        inline void freeSendState(send_state_t * object) {
                _send_allocator.returnObject((void *) object);
        }

        inline recv_state_t *allocateRecvState() {
                return (recv_state_t *) _recv_allocator.allocateObject();
        }

        inline void freeRecvState(recv_state_t * object) {
                _recv_allocator.returnObject((void *) object);
        }
#if 0
        inline void setConnection(pami_task_t task, void *arg) {
                size_t peer = _device.task2peer(task);
                TRACE_ERR((stderr,
                                                ">> Datagram::setConnection(%zu, %p) .. _connection[%zu] = %p\n",
                                                task, arg, peer, _connection[peer]));
                PAMI_assert(_connection[peer] == NULL);
                _connection[peer] = arg;TRACE_ERR((stderr, "<< Datagram::setConnection(%zu, %p)\n",
                                                task, arg));
        }

        inline void *getConnection(pami_task_t task) {
                size_t peer = _device.task2peer(task);
                TRACE_ERR((stderr,
                                                ">> Datagram::getConnection(%zu) .. _connection[%zu] = %p\n",
                                                task, peer, _connection[peer]));
                PAMI_assert(_connection[peer] != NULL);TRACE_ERR((stderr,
                                                "<< Datagram::getConnection(%zu) .. _connection[%zu] = %p\n",
                                                task, peer, _connection[peer]));
                return _connection[peer];
        }

        inline void clearConnection(pami_task_t task) {
                size_t peer = _device.task2peer(task);
                TRACE_ERR((stderr,
                                                ">> Datagram::clearConnection(%zu) .. _connection[%zu] = %p\n",
                                                task, peer, _connection[peer]));
                _connection[peer] = NULL;TRACE_ERR((stderr,
                                                "<< Datagram::clearConnection(%zu) .. _connection[%zu] = %p\n",
                                                task, peer, _connection[peer]));
        }
#endif
        // \brief Send a packet ... pack as needed.
        //
        static inline int send_packet(T_Model & model, pkt_t & msg,
                        pami_event_function callback, void *cookie, pami_task_t targetTask, size_t targetContextId,
                        vecs_t & vecs, void *part1, size_t numPart1, void *part2,
                        size_t numPart2) {
                if (numPart1 <= T_Model::packet_model_metadata_bytes) { // part 1 will fit in the metadata
                        TRACE_ERR((stderr,
                                                        "Datagram::send_packet() .. part1 fits in metadata\n"));
                        vecs.d1[0].iov_base = (void *) part2;
                        vecs.d1[0].iov_len = numPart2;
                        model.postPacket(msg, // T_Message to send
                                        callback, // Callback to execute when done
                                        cookie, // Cookie -- if no cb, why do we care?
                                        targetTask, // Task to send to
                                        targetContextId, // Context id to send to
                                        part1, // Part 1 in metadata
                                        numPart1, // Size of Part 1
                                        vecs.d1); // Message info
                } else { // part1 will NOT fit in the metadata
                        TRACE_ERR((stderr,
                                                        "Datagram::send_packet() .. part1 does NOT fit in metadata\n"));
                        vecs.d2[0].iov_base = (void *) part1;
                        vecs.d2[0].iov_len = numPart1;
                        vecs.d2[1].iov_base = (void *) part2;
                        vecs.d2[1].iov_len = numPart2;
                        model.postPacket(msg, // T_Message to send
                                        callback, // Callback to execute when done
                                        cookie, // Cookie -- if no cb, why do we care?
                                        targetTask, // Task to send to
                                        targetContextId, // Context id to send to
                                        NULL, // No metadata (Part 1 didn't fit)
                                        0, //
                                        vecs.d2); // Message info
                }
                return PAMI_SUCCESS;
        }

        // \brief Receive a packet ... unpack as needed.
        static inline int rcv_packet(void *metadata, unsigned int numMeta,
                        void *payload, void *&part1, void *&part2) {
                if (numMeta <= T_Model::packet_model_metadata_bytes) { // part 1 was in the metadata
                        TRACE_ERR((stderr,
                                                        "Datagram::rcv_packet() .. part1 fits in metadata\n"));
                        part1 = metadata;
                        part2 = payload;
                } else { // part1 was NOT in the metadata
                        TRACE_ERR((stderr,
                                                        "Datagram::rcv_packet() .. part1 does NOT fit in metadata\n"));
                        part1 = payload;
                        part2 = (void *) ((uintptr_t) payload + numMeta);
                }
                return PAMI_SUCCESS;
        }

        /// \brief Send an rts message -- utility to ensure consistency
        static inline void send_rts(send_state_t * send) {
                send_packet(send->datagram->_rts_model, // Model to send packet on
                                send->pkt, // T_Message to send
                                NULL, // Callback
                                NULL, // Cookie
                                send->rts.dest_taskid, // Target task
                                send->rts.dest_contextid,
                                send->vecs, (void *) &send->rts, // header
                                sizeof(rts_info_t), // size of header
                                (void *) send->msginfo, // payload
                                send->rts.mbytes); // size of playload
        }
        ;

        /// \brief Send an rts ack message -- utility to ensure consistency
        static inline void send_rts_ack(recv_state_t * rcv,
                        pami_event_function callback, void *cookie) {
                send_packet(rcv->datagram->_rts_ack_model, // Model to send packet on
                                rcv->pkt, // T_Message to send
                                callback, // Callback
                                cookie, // Cookie
                                rcv->from_taskid, // Target task
                                rcv->from_contextid,
                                rcv->vecs, &rcv->ack, // header
                                sizeof(header_ack_t), // size of header
                                NULL, // payload
                                0); // size of playload
        }
        ;

        /// \brief Send an ack message -- utility to ensure consistency
        static inline void send_ack(recv_state_t * rcv,
                        pami_event_function callback, void *cookie) {

                if ( rcv->nlost != 0 ) {
                send_packet(rcv->datagram->_ack_model, // Model to send packet on
                                rcv->pkt, // T_Message to send
                                callback, // Callback
                                cookie, // Cookie
                                rcv->from_taskid, // Target task
                                rcv->from_contextid, // Target context
                                rcv->vecs, &rcv->ack, // header
                                sizeof(header_ack_t), // size of header
                                (void *) rcv->lost_list, // payload
                                4 * rcv->nlost); // size of playload
                } else {
                  // have to always send at least one empty entry in lost list
                  rcv->lost_list[0]=0;
                send_packet(rcv->datagram->_ack_model, // Model to send packet on
                                rcv->pkt, // T_Message to send
                                callback, // Callback
                                cookie, // Cookie
                                rcv->from_taskid, // Target task
                                rcv->from_contextid, // Target context
                                rcv->vecs, &rcv->ack, // header
                                sizeof(header_ack_t), // size of header
                                (void *) rcv->lost_list, // payload
                                4 ); // size of playload ... one empty entry

                }
        }
        ;

        static inline void process_next_send(
                        DatagramProtocol *datagram) {
#warning this queue needs to  be endpoint enabled
                //Remove Head from queue
                datagram->_queue[datagram->_from_taskid].popHead();

                //Check queue is not empty
                if (!datagram->_queue[datagram->_from_taskid].isEmpty()) {

                        //recover next object to send
                        send_state_t
                                        *send =
                                                        (send_state_t *) (datagram->_queue[datagram->_from_taskid].peekHead());

                        TRACE_ERR((stderr,
                                                        "Datagram::process_next_send() .. Sending queue request\n"));
                        send_rts(send);

                        //reset timer
#warning fix prog func timer
                        send->timer0.start(datagram->_progfmodel, STD_RATE_SEND,
                                        resend_rts, send, 10, send_complete, send);
                }
        }
        ;

private:

        // ----------------------------------------------------------------------
        // CLASS ATTRIBUTES
        // ----------------------------------------------------------------------
        MemoryAllocator < sizeof(send_state_t), 16> _send_allocator;
        MemoryAllocator < sizeof(recv_state_t), 16> _recv_allocator;

        T_Model _rts_model;
        T_Model _rts_ack_model;
        T_Model _ack_model;
        T_Model _data_model;
        T_Model _short_data_model;
        T_Device & _device;
        pami_task_t _from_taskid;
        size_t     _from_contextid;
        pami_context_t _context;
        pami_dispatch_callback_function _dispatch_fn;
        void *_cookie;
        T_Connection _connection;

        static Queue *_queue; ///queue send requests
        //static Queue *_recvqueue;   ///not used
        // static Queue *_lostqueue;  ///not used

        PAMI::Device::ProgressFunctionMdl _progfmodel; ///used to setup timers

        send_state_t *_lastva_send; //used in CM

        size_t _cont;

        // ----------------------------------------------------------------------
        // SEND SIMPLE INTERNAL SUPPORT
        // ----------------------------------------------------------------------

        /// \brief Simple implementation
        pami_result_t simple_impl(pami_send_t * parameters) {
                TRACE_ERR((stderr, ">> Datagram::simple_impl()\n"));TRACE_ERR((stderr,
                                                "*** Datagram::simple_imp(): msginfo_bytes = %d  , bytes =%d \n",
                                                parameters->send.header.iov_len,
                                                parameters->send.data.iov_len));
                //TRACE_ERR((stderr," T_Model::packet_model_metadata_bytes = %d\n T_Model::packet_model_payload_bytes = %d \n sizeof(rts_info_t) = %d\n sizeof(cts_info_t) = %d\n sizeof(send_state_t) = %d\n sizeof(recv_state_t) = %d\n sizeof(header_metadata_t) = %d\n sizeof(header_rts_ack_t) = %d\n",T_Model::packet_model_metadata_bytes,T_Model::packet_model_payload_bytes , sizeof(_info_t),sizeof(cts_info_t), sizeof(send_state_t),sizeof(recv_state_t),sizeof(header_metadata_t),sizeof(header_rts_ack_t)));
                TRACE_ERR((stderr,
                                                " T_Model::packet_model_metadata_bytes = %d\n T_Model::packet_model_payload_bytes = %d \n sizeof(header_metadata_t) = %d \n , sizeof(header_ack_t)= %d \n, sizeof(send_state_t) = %d \n, sizeof(recv_state_t) = %d ***\n ",
                                                T_Model::packet_model_metadata_bytes,
                                                T_Model::packet_model_payload_bytes,
                                                sizeof(header_metadata_t), sizeof(header_ack_t),
                                                sizeof(send_state_t), sizeof(recv_state_t)));

                // Allocate memory to maintain the state of the send.
                send_state_t *send = allocateSendState();

                // Save data in send_state_t
                send->cookie = parameters->events.cookie; // Save cookie
                send->local_fn = parameters->events.local_fn; // Save Callback for when local done
                send->remote_fn = parameters->events.remote_fn; // Save Callback for when remote done
                send->datagram = this; // Save pointer to protocol

                send->send_buffer = (char *) parameters->send.data.iov_base; // Sender buffer address

                send->rts.from_taskid = _from_taskid; // Origin Task
                send->rts.from_contextid = _from_contextid; // Origin Context
                send->rts.bytes = parameters->send.data.iov_len; // Total bytes to send
                send->rts.va_send = send; // Virtual Address sender

                // Target task and context identifier (offset)
                PAMI_ENDPOINT_INFO(parameters->send.dest,send->rts.dest_taskid,send->rts.dest_contextid);

                send->rts.mbytes = parameters->send.header.iov_len; // Metadata Number of  bytes
                send->rts.wrate = STD_RATE_SEND; // Initial value for window rate
                send->rts.wsize = 2; // Window size
                send->msginfo = parameters->send.header.iov_base; // Message info
                send->pkgsend = 0; // Initialize pkgsend
                send->cb_data = cb_data_send; // Register Callback for when sent
                // send->cb_rts  =  cb_rts_send;                    // Register Callback for after rts sent

                send->header.seqno = 0; // Initialize sequence number
                send->header.bsend = 0; // Initialize bytes to send
                send->window[0].va_send = send; // Save sender virtual address on window[0]
                send->window[1].va_send = send; // Save sender virtual address on window[1]
                new (&send->timer0) DTimer();
                new (&send->timer1) DTimer();

                // Want to put the check of queue and insertion as close to each other as possible
                bool q_was_empty = _queue[_from_taskid].isEmpty();
                _queue[_from_taskid].pushTail(send);

                TRACE_ERR((stderr,
                                                "Datagram::simple_impl(): Info sender  cookie=%p , send =%p , Sender task  = %d, msinfo= %p , msgbytes = %d , bytes = %d \n",
                                                parameters->events.cookie, send,
                                                parameters->send.task,
                                                parameters->send.header.iov_base,
                                                parameters->send.header.iov_len,
                                                parameters->send.data.iov_len));

                //Send if queue is empty
                if (q_was_empty) {
                        TRACE_ERR((stderr,"Datagram::simple_impl(): Sending RTS\n"));

                        send_rts(send);
                        send->timer0.start(_progfmodel, STD_RATE_SEND,
                                        resend_rts, send, 10, send_complete, send);

                }

                TRACE_ERR((stderr, "<< Datagram::simple_impl()\n"));
                return PAMI_SUCCESS;
        }
        ;

        // ----------------------------------------------------------------------
        // RESEND callbacks for timeouts
        // ----------------------------------------------------------------------

        /// \brief Resend the rts when there is a timeout
        static void resend_rts(pami_context_t context, void * cookie,
                        pami_result_t result) {
                TRACE_ERR((stderr,"Datagram::resend_rts()\n"));
                send_rts( (send_state_t *)cookie );
        }

        /// \brief Resend the rts ack when there is a timeout
        static void resend_rts_ack(pami_context_t context, void * cookie,
                        pami_result_t result) {
                TRACE_ERR((stderr,"Datagram::resend_rts_ack()\n"));
                send_rts_ack( (recv_state_t *)cookie, NULL, NULL );
        }

        /// \brief Resend the ack when there is a timeout
        static void resend_ack(pami_context_t context, void * cookie,
                        pami_result_t result) {
                TRACE_ERR((stderr,"Datagram::resend_ack()\n"));
                recv_state_t * rcv = (recv_state_t *)cookie;
                send_ack(rcv, NULL, NULL);
        }

        // ----------------------------------------------------------------------
        // Completion callbacks
        // ----------------------------------------------------------------------
        ///
        /// \brief Local receive completion event callback.
        ///
        /// This callback will invoke the application local receive
        /// completion callback and free the receive state object
        /// memory.
        ///
        static void receive_complete(pami_context_t context, void *cookie,
                        pami_result_t result) {
                TRACE_ERR((stderr, "Datagram::receive_complete() >> \n"));

                recv_state_t *rcv = (recv_state_t *) cookie;

                pami_event_function local_fn = rcv->info.local_fn;
                void *fn_cookie = rcv->info.cookie;

                DatagramProtocol *datagram =
                                (DatagramProtocol *)rcv->datagram;

                //free memory
                datagram->freeRecvState(rcv);

                //invoke local_fn
                if (local_fn)
                        local_fn(datagram->_context, fn_cookie, result);

                TRACE_ERR((stderr, "Datagram::receive_complete() << \n"));
                return;
        }
        ;

        ///
        /// \brief Local send completion event callback.
        ///
        /// This callback will invoke the application local completion
        /// callback function and, if notification of remote receive
        /// completion is not required, free the send state memory.
        ///
        static void send_complete(pami_context_t context, void *cookie,
                        pami_result_t result) {
                TRACE_ERR((stderr, "Datagram::send_complete() >> \n"));
                send_state_t *send = (send_state_t *) cookie;

                DatagramProtocol *datagram =
                                (DatagramProtocol *)send->datagram;

                pami_event_function local_fn = send->local_fn;
                pami_event_function remote_fn = send->remote_fn;
                void *fn_cookie = send->cookie;

                //free VA memory
                datagram->freeSendState(send);

                //invoke local_fn
                if (local_fn != NULL) {
                        local_fn(datagram->_context, fn_cookie, result);
                }
                //invoke remote_fn
                if (remote_fn != NULL) {
                        remote_fn(datagram->_context, fn_cookie, result);
                }

                process_next_send(datagram);

                TRACE_ERR((stderr, "Datagram::send_complete() << \n"));
                return;
        }
        ;

        // ----------------------------------------------------------------------
        // DISPATCH DIRECT CALLBACKS
        // ----------------------------------------------------------------------
        ///

        /// \brief Dispatch rts direct
        ///
        static int dispatch_rts_direct(void *metadata, void *payload, size_t bytes,
                        void *recv_func_parm, void *cookie) {
                TRACE_ERR((stderr, ">> Datagram::dispatch_rts_direct()\n"));

                rts_info_t *rts;
                void *msginfo;
                rcv_packet(metadata, sizeof(rts_info_t), payload, (void *&) rts,
                                msginfo);
                DatagramProtocol *datagram =
                                (DatagramProtocol *)recv_func_parm;

                TRACE_ERR((stderr,
                                                ">> Datagram::dispatch_rts_direct() rts(%d) received , rts->va_send = %p  datagram->_lastva_send=%p \n",
                                                ++datagram->_cont, rts->va_send,
                                                datagram->_lastva_send));
                // TODO NEED TO FIX THIS
                if (datagram->_lastva_send == rts->va_send) {
                        std::cout
                                        << "XXXXXXX Datagram::dispatch_rts_direct(): don't need to create a new receive structure XXXXXX"
                                        << std::endl;
                        // TODO This should rts_ack
                        return 0;
                } else {
                        datagram->_cont = 0; ///counter for reference

                        datagram->_lastva_send = rts->va_send; ///update  va_send

                        // Allocate memory to maintain the state of the rcv.
                        recv_state_t *rcv = datagram->allocateRecvState();

                        //save pointer to protocol
                        rcv->datagram = datagram;

                        //Save data in recv_state_t
                        rcv->ack.va_send = rts->va_send; ///Virtual address sender
                        rcv->ack.va_recv = rcv; ///Virual Address Receiver
                        rcv->ack.wsize = rts->wsize; ///Save window size
                        rcv->ack.wrate = rts->wrate; ///Save window rate

                        rcv->bytes = rts->bytes; ///Total Bytes to send
                        rcv->from_taskid = rts->from_taskid; ///Origin Task
                        rcv->from_contextid = rts->from_contextid; ///Origin Context Id
                        rcv->mbytes = rts->mbytes; ///Metadata application bytes
                        rcv->rpkg = 0; ///Metadata application bytes

                        rcv->nlost = 0; ///Number of lost packages
                        rcv->last_nlost = 0; ///Last number of lost packages
                        rcv->fseq = true; ///Flag to true
                        rcv->wmaxseq = 0; ///Min seqno
                        rcv->wminseq = 0; ///Max seqno

                        //TODO need to implement longheader in the future  ????
                        //rcv->msgbuff = (char *) malloc (sizeof (char*)*rts->mbytes);  ///Allocate buffer for Metadata using __global.heap_mm!
                        //rcv->msgbytes = 0;                                            ///Initalized received bytes

                        // TODO Why two arrays?
                        ///initialize Array with zeros
			pami_result_t prc;
			prc = __global.heap_mm->memalign(
					(void **)&rcv->lost_list, 0,
					sizeof(*rcv->lost_list) * rts->bytes);
			PAMI_assertf(prc == PAMI_SUCCESS, "alloc of rcv->lost_list failed");
			pami_result_t prc;
			prc = __global.heap_mm->memalign(
					(void **)&rcv->rcv_list, 0,
					sizeof(*rcv->rcv_list) * rts->bytes);
			PAMI_assertf(prc == PAMI_SUCCESS, "alloc of rcv->rcv_list failed");

                        for (size_t i = 0; i < rts->bytes; i++)
                                rcv->lost_list[i] = 0;

                        for (size_t i = 0; i < rts->bytes; i++)
                                rcv->rcv_list[i] = 0;
                        /// end initialize array

                        if (sizeof(header_metadata_t)
                                        <= T_Model::packet_model_metadata_bytes) {

                                //Determine short_data package bytes, first one to receive
                                if ((rts->bytes % (T_Model::packet_model_payload_bytes) == 0)) {
                                        rcv->fbytes = T_Model::packet_model_payload_bytes;
                                        rcv->pkgnum = rts->bytes
                                                        / T_Model::packet_model_payload_bytes;
                                } else {
                                        rcv->fbytes = (rts->bytes
                                                        % (T_Model::packet_model_payload_bytes));
                                        rcv->pkgnum = rts->bytes
                                                        / T_Model::packet_model_payload_bytes + 1;
                                }
                        } else {
                                //Determine short_data package bytes, first one to receive
                                if ((rts->bytes % (T_Model::packet_model_payload_bytes
                                                - sizeof(header_metadata_t)) == 0)) {
                                        rcv->fbytes = T_Model::packet_model_payload_bytes
                                                        - sizeof(header_metadata_t);
                                        rcv->pkgnum = rts->bytes
                                                        / (T_Model::packet_model_payload_bytes
                                                                        - sizeof(header_metadata_t));
                                } else {
                                        rcv->fbytes = (rts->bytes
                                                        % (T_Model::packet_model_payload_bytes
                                                                        - sizeof(header_metadata_t)));
                                        rcv->pkgnum = rts->bytes
                                                        / (T_Model::packet_model_payload_bytes
                                                                        - sizeof(header_metadata_t)) + 1;
                                }
                        }

                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_rts_direct() ..  _context=%p , -contextid =%d , cookie=%p ,Sender = %d, msinfo= %p , msgbytes = %d , bytes = %d, Recv_inof addr = %p \n",
                                                        datagram->_client, datagram->_contextid,
                                                        datagram->_cookie, rts->from_task, msginfo,
                                                        rts->mbytes, rts->bytes,
                                                        (void *) (rcv->info.data.simple.addr)));

                        // Invoke the registered dispatch function.
                        datagram->_dispatch_fn.p2p(datagram->_context, // Communication context
                                        datagram->_cookie, // Dispatch cookie
                                        msginfo, // Application metadata
                                        rts->mbytes, // Metadata bytes
                                        NULL, // No payload data
                                        rts->bytes, // Number of msg bytes
                                        (pami_recv_t *) & (rcv->info)); //Recv_state struct

                        // Only handle simple receives .. until the non-contiguous support
                        // is available
                        PAMI_assert(rcv->info.kind == PAMI_AM_KIND_SIMPLE);

                        if (rts->bytes == 0) // Move this special case to another dispatch funtion to improve latency in the common case.
                        {
                                TRACE_ERR((stderr,
                                                                "   Datagram::dispatch_rts_direct .. Application Metadata send in only one package, 0 bytes received\n"));

                                // No data packets will follow the rts so send the rts ack and when
                                // it completes do receive completion processing
                                send_rts_ack(rcv, receive_complete, (void *) rcv);
                                rcv->datagram->_lastva_send = NULL;

                                return 0;
                        }

                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_rts_direct() rts_ack was send for 0 byte case\n"));

                        //Post rts_ack package
                        send_rts_ack(rcv, NULL, NULL);

                        //Start timer (initial value = 0, max number of cycles = 5, rate = 10000 , receiver, function send_rts_ack, timer0)
#warning fix timer stuff
                        rcv->timer0.start(rcv->datagram->_progfmodel, STD_RATE_RECV,
                                        resend_rts_ack, rcv, 5, receive_complete, rcv);

                        TRACE_ERR((stderr,
                                                        ">> Datagram::dispatch_rts_direct() rts_ack was sent\n"));

                        return 0;
                }
        }
        ;

        ///
        /// \brief Process rts ack direct
        ///
        static int dispatch_rts_ack_direct(void *metadata, void *payload,
                        size_t bytes, void *recv_func_parm, void *cookie) {

                TRACE_ERR((stderr, ">> Datagram::dispatch_rts_ack()\n"));

                //Pointer to Protocol object
                DatagramProtocol *datagram =
                                (DatagramProtocol *)recv_func_parm;

                header_ack_t *ack;
                void *dummy;
                rcv_packet(metadata, sizeof(header_ack_t), payload, (void *&) ack,
                                dummy);

                ack->va_send->timer0.stop();

                // Special case of 0 byte send
                if (ack->va_send->rts.bytes == 0) {

                        TRACE_ERR((stderr, "   Datagram::rts_ack() ..  0 bytes condition \n"));

                        ack->va_send->timer0.close();

                        //call terminate function
                        send_complete(datagram->_context, (void *) ack->va_send, PAMI_SUCCESS);

                        return 0;
                }

                // Actually data to send
                TRACE_ERR((stderr,
                                                "   Datagram::process_rts_ack() .. va_recv = %p , to_send = %d , payload_bytes = %d, pkgnum= %d , total of bytes=%d\n",
                                                ack->va_recv, ack->va_send->header.bsend,
                                                T_Model::packet_model_payload_bytes,
                                                ack->va_send->pkgnum, ack->va_send->rts.bytes));

                ack->va_send->header.va_recv = ack->va_recv; ///Virtual address reciver
                ack->va_send->header.seqno = 0; ///Initialize seqno to zero
                ack->va_send->pf = recv_func_parm; ///Save pointer to recv_func_param

                if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes) {
                        //Determine short_data package and total of packages to send
                        if ((ack->va_send->rts.bytes
                                        % (T_Model::packet_model_payload_bytes) == 0)) {
                                ack->va_send->fbytes = T_Model::packet_model_payload_bytes;
                                ack->va_send->pkgnum = ack->va_send->rts.bytes
                                                / T_Model::packet_model_payload_bytes;TRACE_ERR((stderr,
                                                                " x Datagram::process_rts_ack() first one\n"));
                        } else {
                                ack->va_send->fbytes = (ack->va_send->rts.bytes
                                                % (T_Model::packet_model_payload_bytes));
                                ack->va_send->pkgnum = ack->va_send->rts.bytes
                                                / T_Model::packet_model_payload_bytes + 1;TRACE_ERR((stderr,
                                                                " x Datagram::process_rts_ack() second one\n"));
                        }
                } else {
                        TRACE_ERR((stderr,
                                                        " x Datagram::process_rts_ack() third one\n"));

                        //Determine short_data package and total of packages to send
                        if ((ack->va_send->rts.bytes % (T_Model::packet_model_payload_bytes
                                        - sizeof(header_metadata_t)) == 0)) {
                                ack->va_send->fbytes = T_Model::packet_model_payload_bytes
                                                - sizeof(header_metadata_t);
                                ack->va_send->pkgnum = ack->va_send->rts.bytes
                                                / (T_Model::packet_model_payload_bytes
                                                                - sizeof(header_metadata_t));
                        } else {
                                ack->va_send->fbytes = (ack->va_send->rts.bytes
                                                % (T_Model::packet_model_payload_bytes
                                                                - sizeof(header_metadata_t)));
                                ack->va_send->pkgnum = ack->va_send->rts.bytes
                                                / (T_Model::packet_model_payload_bytes
                                                                - sizeof(header_metadata_t)) + 1;
                        }

                }

                ack->va_send->header.bsend = ack->va_send->fbytes; //update header
                ++ack->va_send->pkgsend; //update sent packages number

                TRACE_ERR((stderr,
                                                "   Datagram::process_rts_ack() ..  Sending data\n"));

                send_packet(datagram->_short_data_model, // Model to send packet on
                                ack->va_send->pkt, // T_Message to send
                                ack->va_send->cb_data, // Callback -- initialized to cb_data_send
                                (void *) ack->va_send, // Cookie
                                ack->va_send->rts.dest_taskid, // Target task
                                ack->va_send->rts.dest_contextid, // Target task
                                ack->va_send->vecs, &ack->va_send->header, // header
                                sizeof(header_metadata_t), // size of header
                                ack->va_send->send_buffer, // payload
                                ack->va_send->header.bsend); // size of playload

                //Start timer (initial value = 0, max number of cycles = 10, rate = 1200 , sender, function send_data, timer1)
#warning fix prog func timer stuff
                ack->va_send->timer1.start(datagram->_progfmodel,
                                STD_RATE_SEND, resend_data, ack->va_send, 10, send_complete,
                                ack->va_send);
        return 0;
        }
        ;

        // ----------------------------------------------------------------------
        // POST PACKET CALLBACKS and Utilities
        // ----------------------------------------------------------------------

        /// \brief Callback invoked after send a data_send packet.
        ///
        static void cb_data_send(pami_context_t context, void *cookie,
                        pami_result_t result) {

                TRACE_ERR((stderr, ">> Datagram::cb_data_send()\n"));

                //Pointer to send state
                send_state_t *send = (send_state_t *) cookie;

                //Pointer to Protocol
                DatagramProtocol *datagram =
                                (DatagramProtocol *)send->pf;

                TRACE_ERR((stderr,
                                                "   Datagram::cb_data_send() .. data total bytes= %d  , send->pkgnum = %d ,  send->rts.window = %d, pkgsend = %d , offset=%d , bsend=%d \n",
                                                send->rts.bytes, send->pkgnum, send->rts.wsize,
                                                send->pkgsend, send->header.seqno,
                                                T_Model::packet_model_payload_bytes));
                //TRACE_ERR((stderr,"   Datagram::Callback_data_send() .. cookie= %p , send->header->va_recv = %p ,send->cb_data = %p\n" ,cookie,send->header.va_recv, send->cb_data));

                TRACE_ERR((stderr,
                                                "   Datagram::cb_data_send() .. window= %p , iolen =%d, wsize = %d\n",
                                                &send->window[0], 2, send->rts.wsize));

                ///function to send window
                send->datagram->send_window((window_t *) &send->window[0], 2, send->rts.wsize);

                TRACE_ERR((stderr,
                                                "   Datagram::cb_data_send() ..  Stop Sending, send->pkgsend =%d,  send->rts.window= %d , send->header.seqno = %d \n",
                                                send->pkgsend, send->rts.wsize, send->header.seqno));

                return;
        }

        ///
        /// \brief function to send window
        /// TODO changed static to inline ... OK?
        inline int send_window(window_t * window, size_t iolen, size_t wsize) {

                size_t i = 0;

                for (i = 0; i < WINDOW_SIZE - 1; i++) {
                        //sleep(1);

                        //Check if data to send
                        if ((window->va_send->pkgsend == wsize)
                                        || ((window->va_send->pkgnum - 1)
                                                        <= window->va_send->header.seqno)) {
                                window->va_send->pkgsend = 0;
                                window->va_send->last_seqno = window->va_send->header.seqno + 1;TRACE_ERR((stderr,
                                                                ">>   Datagram::send_window() .. all data was sent\n"));

                                return 0;
                        }

                        TRACE_ERR((stderr,
                                                        "   Datagram::send_window() .. window= %p , iolen =%d, wsize = %d, window->va_send->header.seqno =%d , window->va_send->pkgsend = %d , window->va_send->pkgnum =%d \n",
                                                        window, iolen, wsize, window->va_send->header.seqno,
                                                        window->va_send->pkgsend, window->va_send->pkgnum));

                        if (sizeof(header_metadata_t)
                                        <= T_Model::packet_model_metadata_bytes) {
                                window->va_send->header.bsend
                                                = T_Model::packet_model_payload_bytes; ///Update bytes to window->va_send
                        } else {
                                window->va_send->header.bsend
                                                = (T_Model::packet_model_payload_bytes
                                                                - sizeof(header_metadata_t)); ///Update bytes to window->va_send
                        }

                        ++window->va_send->header.seqno; //increment seqno

                        ++window->va_send->pkgsend; //increment pksens

                        //TRACE_ERR((stderr,"   AdaptiveSimple::Callback_data_window->va_send() .. cookie= %p , window->va_send->cts->va_window->va_send = %p ,window->va_send->cb_data = %p\n" ,cookie,window->va_send->rts.va_window->va_send, window->va_send->cb_data));

                        //save header info inside package
                        window->pkg[i].header.va_recv = window->va_send->header.va_recv;
                        window->pkg[i].header.seqno = window->va_send->header.seqno;
                        window->pkg[i].header.bsend = window->va_send->header.bsend;

                        TRACE_ERR((stderr,
                                                        "   Datagram::send_window() .. window pkt = %p , pktno=%d\n",
                                                        &window->pkg[i].pkt, i));

                        //send data
                        send_packet(window->va_send->datagram->_data_model, // Model to send packet on
                                        window->pkg[i].pkt, // T_Message to send
                                        NULL, // Callback
                                        (void *) NULL, // Cookie
                                        window->va_send->rts.dest_taskid, // Target task
                                        window->va_send->rts.dest_contextid, // Target context
                                        window->va_send->vecs, (void *) &window->pkg[i].header, // header
                                        sizeof(header_metadata_t), // size of header
                                        (void *) (window->va_send->send_buffer
                                                        + ((window->va_send->header.seqno - 1)
                                                                        * T_Model::packet_model_payload_bytes
                                                                        + window->va_send->fbytes)), // payload
                                        (size_t) window->pkg[i].header.bsend); // size of playload

                        TRACE_ERR((stderr,
                                                        "   Datagram::send_window() pkt(%d).. data total bytes= %d  ,window = %p , seqno = %d , winodw.seqno=%d , bsend=%d \n",
                                                        i, window->va_send->rts.bytes, window,
                                                        window->va_send->header.seqno,
                                                        window->pkg[i].header.seqno,
                                                        window->pkg[i].header.bsend));

                }
                //call callback

                if ((window->va_send->pkgsend == wsize) || ((window->va_send->pkgnum
                                - 1) <= window->va_send->header.seqno)) {
                        window->va_send->pkgsend = 0;
                        window->va_send->last_seqno = window->va_send->header.seqno + 1;TRACE_ERR((stderr,
                                                        ">>   Datagram::send_window() .. all data was sent\n"));

                        return 0;
                }

                TRACE_ERR((stderr,
                                                "   Datagram::send_window() .. window= %p , iolen =%d, wsize = %d, window->va_send->header.seqno =%d , window->va_send->pkgsend = %d , window->va_send->pkgnum =%d \n",
                                                window, iolen, wsize, window->va_send->header.seqno,
                                                window->va_send->pkgsend, window->va_send->pkgnum));

                // update_header(window);

                if (sizeof(header_metadata_t) <= T_Model::packet_model_metadata_bytes) {
                        window->va_send->header.bsend = T_Model::packet_model_payload_bytes; ///Update bytes to window->va_send
                } else {
                        window->va_send->header.bsend
                                        = (T_Model::packet_model_payload_bytes
                                                        - sizeof(header_metadata_t)); ///Update bytes to window->va_send
                }

                ++window->va_send->header.seqno;

                ++window->va_send->pkgsend;

                window->pkg[i].header.va_recv = window->va_send->header.va_recv;
                window->pkg[i].header.seqno = window->va_send->header.seqno;
                window->pkg[i].header.bsend = window->va_send->header.bsend;

                //TRACE_ERR((stderr, "   AdaptiveSimple::Callback_data_window->va_send() .. after window->va_send->fblock= %s\n", (window->va_send->fblock)?"true":"false"));

                TRACE_ERR((stderr,
                                                "   Datagram::send_window() .. window pkt = %p , pktno=%d\n",
                                                &window->pkg[i].pkt, i));
                //pkt_t * dummy = (pkt_t *)malloc(sizeof(pkt_t)); using __global.heap_mm!
                send_packet(window->va_send->datagram->_data_model, // Model to send packet on
                                window->pkg[i].pkt, // T_Message to send
                                NULL, // Callback
                                (void *) NULL, // Cookie
                                window->va_send->rts.dest_taskid, // Target task
                                window->va_send->rts.dest_contextid, // Target context
                                window->va_send->vecs, (void *) &window->pkg[i].header, // header
                                sizeof(header_metadata_t), // size of header
                                (void *) (window->va_send->send_buffer
                                                + ((window->va_send->header.seqno - 1)
                                                                * T_Model::packet_model_payload_bytes
                                                                + window->va_send->fbytes)), // payload
                                (size_t) window->pkg[i].header.bsend); // size of playload

                //Check if data to send

                return 0;
        }

        /// \brief resend data due to time out
        static void resend_data(pami_context_t ctx, void *cd, pami_result_t result ) {

                TRACE_ERR((stderr,
                                                "	Datagram::Timer send_data() .. Sending data, protocol header_info_t fits in the packet metadata, application metadata fits in a single packet payload\n"));

                send_state_t * send = (send_state_t *)cd;
                //Post First Data package
                if (send->header.seqno == 0) { // TODO why wouldn't we always send it? (hmm may have had ack)

                        TRACE_ERR((stderr,
                                                        "	Datagram::Timer send_data() .. data seqno=0 resend short data\n"));

                        send_packet(send->datagram->_short_data_model, // Model to send packet on
                                        send->pkt, // T_Message to send
                                        send->cb_data, // Callback
                                        (void *) send, // Cookie
                                        send->rts.dest_taskid, // Target task
                                        send->rts.dest_contextid, // Target context
                                        send->vecs, &send->header, // header
                                        sizeof(header_metadata_t), // size of header
                                        &send->send_buffer, // payload
                                        send->header.bsend); // size of playload

                } else if (send->nlost> 0) {

                        TRACE_ERR((stderr,
                                                        "	Datagram::Timer send_data() .. sending lost list"));

                        //send lost list type=true (sender)
                        send_lost_list((send_state_t *) send);

                        TRACE_ERR((stderr,
                                                        "	Datagram::Timer send_data() .. data (%d) was sent\n",
                                                        ++send->datagram->_cont));
                } else

                        TRACE_ERR((stderr,
                                                        "	Datagram::Timer send_data() .. Nothing (%d) was sent\n",
                                                        ++send->datagram->_cont));
        }
        ;

        /// \brief resend lost list sender
        static int send_lost_list(send_state_t * send) {

                struct iovec v[2];
                size_t i = 0;
                TRACE_ERR((stderr,
                                                "	Datagram::send_lost_list() .. nlost =%d \n",
                                                send->nlost));

                for (i = 0; i < send->nlost - 1; i++) {

                        TRACE_ERR((stderr,
                                                        "	Datagram::send_lost_list() .. data lost =%d was sent\n",
                                                        send->lost_list[i]));

                        send->header.seqno = send->lost_list[i];
                        if (send->lost_list[i] == 0) {
                                v[1].iov_base = (void *) (send->send_buffer);
                                v[1].iov_len = (size_t) send->fbytes;
                                send->header.bsend = send->fbytes;
                        } else {
                                v[1].iov_base = (void *) (send->send_buffer
                                                + ((send->lost_list[i] - 1)
                                                                * T_Model::packet_model_payload_bytes
                                                                + send->fbytes));
                                v[1].iov_len = (size_t) send->header.bsend;
                                send->header.bsend = T_Model::packet_model_payload_bytes;
                        }

                        send_packet(send->datagram->_data_model, // Model to send packet on
                                        send->pkt, // T_Message to send
                                        NULL, // Callback
                                        (void *) send, // Cookie
                                        send->rts.dest_taskid, // Target task
                                        send->rts.dest_contextid, // Target context
                                        send->vecs, &send->header, // header
                                        sizeof(header_metadata_t), // size of header
                                        v[1].iov_base, // payload
                                        v[1].iov_len); // size of playload

                }

                TRACE_ERR((stderr,
                                                "	Datagram::send_lost_list() .. data lost package =%d was sent\n",
                                                send->lost_list[i]));
                send->header.seqno = send->lost_list[i];
                if (send->lost_list[i] == 0) {
                        v[1].iov_base = (void *) (send->send_buffer);
                        v[1].iov_len = (size_t) send->fbytes;
                        send->header.bsend = send->fbytes;
                } else {
                        v[1].iov_base = (void *) (send->send_buffer + ((send->lost_list[i]
                                        - 1) * T_Model::packet_model_payload_bytes + send->fbytes));
                        v[1].iov_len = (size_t) send->header.bsend;
                        send->header.bsend = T_Model::packet_model_payload_bytes;
                }

                send_packet(send->datagram->_data_model, // Model to send packet on
                                send->pkt, // T_Message to send
                                send->cb_data, // Callback
                                (void *) send, // Cookie
                                send->rts.dest_taskid, // Target task
                                send->rts.dest_contextid, // Target context
                                send->vecs, &send->header, // header
                                sizeof(header_metadata_t), // size of header
                                v[1].iov_base, // payload
                                v[1].iov_len); // size of playload

                send->nlost = 0;
                send->header.seqno = send->last_seqno;

                return 0;
        }

        ///
        /// \brief Direct send short_data   packet dispatch.
        ///
        static int dispatch_short_data_direct(void *metadata, void *payload,
                        size_t bytes, void *recv_func_parm, void *cookie) {
                TRACE_ERR((stderr, ">> Datagram::dispatch_short_data()\n"));
                DatagramProtocol *datagram =
                                (DatagramProtocol *)recv_func_parm;

                header_metadata_t *header;
                void *msginfo;
                rcv_packet(metadata, sizeof(header_metadata_t), payload,
                                (void *&) header, msginfo);
                memcpy((char *) (header->va_recv->info.data.simple.addr)
                                + header->seqno, (msginfo), header->va_recv->fbytes);  // TODO why add seqno?

                header->va_recv->timer0.close(); // Know that rts ack arrived
                header->va_recv->rcv_list[header->seqno] = header->seqno; //saved seqno in received list

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_short_data() .. buffer address =%p ,  bytes for receiving =%d, offset = %d, bsend = %d\n",
                                                header->va_recv->info.data.simple.addr,
                                                header->va_recv->info.data.simple.bytes,
                                                header->seqno, header->va_recv->fbytes));

                //Update total of bytes received
                header->va_recv->info.data.simple.bytes -= header->bsend;

                //update total of received packages
                ++header->va_recv->rpkg;

                //Start timer (initial value = 0, max number of cycles = 20, rate = 1200 , receiver, function send_ack, timer1)
#warning fix prog func timer
                header->va_recv->timer1.start(datagram->_progfmodel,
                                STD_RATE_RECV, resend_ack, header->va_recv, 20,
                                receive_complete, header->va_recv);

                header->va_recv->lost_list[0] = 0; //set lost list to zero

                //are all bytes received?
                if ((header->va_recv->info.data.simple.bytes == 0)
                                && (header->va_recv->pkgnum == 1)) {

                        header->va_recv->ack.wrate = 0; //window rate to zero = end condition

                        // send ack and once it is dispatched complete the receive
                        send_ack(header->va_recv, receive_complete, header->va_recv);

                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_short_data() ..  all data Received \n"));

                        header->va_recv->timer0.close();
                        header->va_recv->timer1.close();

                        header->va_recv->datagram->_lastva_send = NULL; //set to NULL

                }

                return 0;
        };

        ///
        /// \brief Direct send data   packet dispatch.
        ///
        static int dispatch_data_direct(void *metadata, void *payload,
                        size_t bytes, void *recv_func_parm, void *cookie) {
                TRACE_ERR((stderr, ">> Datagram::dispatch_data_direct()\n"));

                DatagramProtocol *datagram =
                                (DatagramProtocol *)recv_func_parm;

                //Pointer to metadata
                header_metadata_t *header;
                void *msginfo;
                rcv_packet(metadata, sizeof(header_metadata_t), payload,
                                (void *&) header, msginfo);
                memcpy((char *) (header->va_recv->info.data.simple.addr)
                                + ((header->seqno - 1) * header->bsend
                                                + header->va_recv->fbytes), (msginfo), header->bsend);

                header->va_recv->timer1.stop();

                //Save maximun received seqno
                if (header->seqno> header->va_recv->wmaxseq)
                        header->va_recv->wmaxseq = header->seqno;

                //save received seqno
                header->va_recv->rcv_list[header->seqno] = header->seqno;

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_data_direct() ..  wminseq =%d, wmaxseq=%d, rcv_list[%d]=%d  header->seqno = %d \n",
                                                header->va_recv->wminseq, header->va_recv->wmaxseq,
                                                header->seqno,
                                                header->va_recv->rcv_list[header->seqno],
                                                header->seqno));

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_data_direct() .. header_metadata_t coming inside metadata\n"));

                //Update total of bytes received
                header->va_recv->info.data.simple.bytes -= header->bsend;

                //update total of received packages
                ++header->va_recv->rpkg;

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_data_direct() .. buffer address =%p ,  bytes for receiving =%d, offset = %d, bsend = %d\n",
                                                header->va_recv->info.data.simple.addr,
                                                header->va_recv->info.data.simple.bytes,
                                                header->seqno, header->bsend));

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_data_direct() Window size = %d, rpkg = %d , pkgnum = %d .  Protocol header_info_t fits in the packet metadata, pami_task_t  fits in the message metadata\n",
                                                header->va_recv->ack.wsize, header->va_recv->rpkg,
                                                header->va_recv->pkgnum));

                //are all bytes received?
                if ((header->va_recv->info.data.simple.bytes == 0)
                                && (header->va_recv->wmaxseq + 1 == header->va_recv->pkgnum)) {
                        header->va_recv->lost_list[0] = 0; //nothing lost
                        header->va_recv->nlost = 0;
                        header->va_recv->ack.wrate = 0; //end condition

                        //post ack package
                        send_ack(header->va_recv, NULL, NULL);

                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_data_direct() ..  all data Received \n"));

                        header->va_recv->timer0.close();
                        header->va_recv->timer1.close();

                        header->va_recv->datagram->_lastva_send = NULL;

                        //call terminate function
                        receive_complete(datagram->_context, (void *) header->va_recv,
                                        PAMI_SUCCESS);

                        return 0;

                } else {

                        //call control function
                        control_ack((header_metadata_t *) header);
                }
                // reset timer ResetRxTimer(0, 10, header->va_recv->ack.wrate, header->va_recv,	1);
                header->va_recv->timer1.reset();

                return 0;
        };

        //Control mechanism
        static int control_ack(header_metadata_t * header) {

                if (header->va_recv->rpkg == header->va_recv->ack.wsize) {

                        header->va_recv->rpkg = 0; //reset for next window

                        ///Implement control mechanism
                        //Check from winseq to maxseq
                        for (size_t i = header->va_recv->wminseq; i
                                        < header->va_recv->wmaxseq; i++) {

                                //TRACE_ERR((stderr,"   Datagram::control_ack() ..  wminseq =%d, wmaxseq=%d, rcv_list[%d]=%d  i = %d \n",header->va_recv->wminseq, header->va_recv->wmaxseq, i, header->va_recv->rcv_list[i], i ));

                                if (header->va_recv->rcv_list[i] == i) {
                                        if (header->va_recv->fseq)
                                                header->va_recv->wminseq++; //update minimun seqno received OK

                                } else {
                                        header->va_recv->fseq = false; //stop saving min seqno
                                        header->va_recv->lost_list[header->va_recv->nlost++] = i; //update lost list
                                }
                        }
                        header->va_recv->fseq = true; //reset flag

                        //Update wrate and wsize
                        if (header->va_recv->last_nlost < header->va_recv->nlost) {
                                header->va_recv->ack.wsize = header->va_recv->ack.wsize / 2; //decrement window size
                                header->va_recv->ack.wrate = header->va_recv->ack.wrate / 2; //decrement window rate
                        } else {
                                header->va_recv->ack.wsize = header->va_recv->ack.wsize * 2; //increment window size
                                header->va_recv->ack.wrate = header->va_recv->ack.wrate * 2; //increment window rate
                        }

                        header->va_recv->last_nlost = header->va_recv->nlost; //save nlost

                        TRACE_ERR((stderr,
                                                        "   Datagram::control_ack()  Window size = %d , wminseq = %d , wmaxseq = %d , nlost = %d ",
                                                        header->va_recv->ack.wsize,
                                                        header->va_recv->wminseq, header->va_recv->wmaxseq,
                                                        header->va_recv->nlost));

                        if ((header->va_recv->wmaxseq + 1 == header->va_recv->pkgnum)
                                        && (header->va_recv->nlost == 0)) {
                                header->va_recv->ack.wrate = 0; //end condition

                                //post ack package
                                send_ack(header->va_recv, NULL, NULL);

                                TRACE_ERR((stderr,
                                                                "   Datagram::control_ack() ..  all data Received \n"));

                                header->va_recv->timer0.close();
                                header->va_recv->timer1.close();

                                header->va_recv->datagram->_lastva_send = NULL;

                                //call terminate function
                                receive_complete(header->va_recv->datagram->_context,
                                                (void *) header->va_recv, PAMI_SUCCESS);

                        } else {

                                send_ack(header->va_recv, NULL, NULL);

                                TRACE_ERR((stderr,
                                                                ">>   Datagram::control_ack() Packed was received and ack was sent\n"));

                        }
                        ///End control Mechanism

                }
                return 0;
        }

        static int dispatch_ack_direct(void *metadata, void *payload, size_t bytes,
                        void *recv_func_parm, void *cookie) {

                TRACE_ERR((stderr, ">> Datagram::dispatch_ack_direct()\n"));

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_ack_direct() .. header_info coming inside metadata\n"));

                //ack is inside metadata
                header_ack_t *ack;
                size_t *lost;
                rcv_packet(metadata, sizeof(header_ack_t), payload, (void *&) ack,
                                (void *&) lost);

                //Pointer to Protocol object
                DatagramProtocol *datagram =
                                (DatagramProtocol *)recv_func_parm;

                ack->va_send->timer1.stop();

                //update window size and rate
                ack->va_send->rts.wsize = ack->wsize;
                ack->va_send->rts.wrate = ack->wrate;

                TRACE_ERR((stderr,
                                                "   Datagram::dispatch_ack_direct() .. lost_list =%d ,bytes =%d \n",
                                                lost[0], bytes));

                //lost package?
                if (lost[0] == 0) {

                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_ack_direct() .. lost_list EMPTY = %d  seqno =%d\n",
                                                        ack->va_send->pkgnum, ack->va_send->header.seqno));

                        if (ack->wrate == 0) {
                                //END CONDITION

                                ack->va_send->timer0.close();
                                ack->va_send->timer1.close();

                                TRACE_ERR((stderr,
                                                                ">>   Datagram::dispatch_ack_direct() wrate==0 .. all data was sent\n"));

                                send_complete(datagram->_context, (void *) ack->va_send,
                                                PAMI_SUCCESS);

                                return 0;
                        } else {
                                //send more data
                                TRACE_ERR((stderr,
                                                                ">>   Datagram::dispatch_ack_direct() wrate!=0 .. more data to send\n"));

                                if (ack->va_send->pkgnum - 1> ack->va_send->header.seqno) {
                                        ack->va_send->pkgsend = 0;
                                        cb_data_send(datagram->_context, (void *) ack->va_send,
                                                        PAMI_SUCCESS);
                                }

                        }

                } else {
                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_ack_direct() .. lost_list is NOT EMPTY\n"));
                        ack->va_send->lost_list = lost;
                        ack->va_send->nlost = bytes >> 2;

                        TRACE_ERR((stderr,
                                                        "   Datagram::dispatch_ack_direct() .. lost_list[0] =%d ,nlost =%d \n",
                                                        lost[0], bytes >> 2));

                        // send lost list  (sender)
                        send_lost_list((send_state_t *) ack->va_send);

                        TRACE_ERR((stderr,
                                                        "	Datagram::dispatch_ack_direct() .. last_seqno =%d , header.seqno = %d\n",
                                                        ack->va_send->last_seqno,
                                                        ack->va_send->header.seqno));

                }

                //reset timer ResetTxTimer(0, 10, ack->va_send->rts.wrate, ack->va_send, 1);
                ack->va_send->timer1.reset();

                return 0;
        };

        // ----------------------------------------------------------------------
        // SEND SIMPLE DISPATCH READ -- NOT CURRENTLY SUPPORTED
        // ----------------------------------------------------------------------

        ///
        /// \brief Read-access  send rts packet callback.
        ///

        static int dispatch_rts_read(void *metadata, void *payload, size_t bytes,
                        void *recv_func_parm, void *cookie) {
#if 0
                TRACE_ERR((stderr,
                                                "(%zu) DatagramFactory::dispatch_rts_read() .. \n"));

                DatagramFactory < T_Model, T_Device, T_LongHeader> *pf =
                (DatagramFactory < T_Model, T_Device,
                                T_LongHeader> *)recv_func_parm;

                // This packet device DOES NOT provide the data buffer(s) for the
                // message and the data must be read on to the stack before the
                // recv callback is invoked.

                uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
                void *p = (void *) &stackData[0];
                pf->getDevice()->readData(channel, (char *) p, bytes);

                dispatch_rts_direct(channel, metadata, p, bytes, recv_func_parm);
#else
                PAMI_abort();
#endif
                return 0;
        }
        ;

        //
        /// \brief Read-access  send ack packet callback.
        ///

        static int dispatch_ack_read(void *metadata, void *payload, size_t bytes,
                        void *recv_func_parm, void *cookie) {
#if 0
                TRACE_ERR((stderr,
                                                "(%zu) DatagramFactory::dispatch_rts_read() .. \n"));

                DatagramFactory < T_Model, T_Device, T_LongHeader> *pf =
                (DatagramFactory < T_Model, T_Device,
                                T_LongHeader> *)recv_func_parm;

                // This packet device DOES NOT provide the data buffer(s) for the
                // message and the data must be read on to the stack before the
                // recv callback is invoked.

                uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
                void *p = (void *) &stackData[0];
                pf->getDevice()->readData(channel, (char *) p, bytes);

                dispatch_ack_direct(channel, metadata, p, bytes, recv_func_parm);
#else
                PAMI_abort();
#endif
                return 0;
        }
        ;

        ///
        /// \brief Read-access  send rts_ack packet callback.
        ///

        static int dispatch_rts_ack_read(void *metadata, void *payload,
                        size_t bytes, void *recv_func_parm, void *cookie) {
#if 0
                TRACE_ERR((stderr,
                                                "(%zu) DatagramFactory::dispatch_rts_ack_read() .. \n"));

                DatagramFactory < T_Model, T_Device, T_LongHeader> *pf =
                (DatagramFactory < T_Model, T_Device,
                                T_LongHeader> *)recv_func_parm;

                // This packet device DOES NOT provide the data buffer(s) for the
                // message and the data must be read on to the stack before the
                // recv callback is invoked.

                uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
                void *p = (void *) &stackData[0];
                pf->getDevice()->readData(channel, (char *) p, bytes);

                dispatch_rts_ack_direct(channel, metadata, p, bytes,
                                recv_func_parm);
#else
                PAMI_abort();
#endif
                return 0;
        }
        ;

        ///
        /// \brief Read-access  send rts_data packet callback.
        ///

        static int dispatch_rts_data_read(void *metadata, void *payload,
                        size_t bytes, void *recv_func_parm, void *cookie) {
#if 0
                TRACE_ERR((stderr,
                                                "(%zu) DatagramFactory::dispatch_rts_data_read() .. \n"));

                DatagramFactory < T_Model, T_Device, T_LongHeader> *pf =
                (DatagramFactory < T_Model, T_Device,
                                T_LongHeader> *)recv_func_parm;

                // This packet device DOES NOT provide the data buffer(s) for the
                // message and the data must be read on to the stack before the
                // recv callback is invoked.

                uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
                void *p = (void *) &stackData[0];
                pf->getDevice()->readData(channel, (char *) p, bytes);

                dispatch_rts_data_direct(channel, metadata, p, bytes,
                                recv_func_parm);
#else
                PAMI_abort();
#endif
                return 0;
        }
        ;

        ///
        /// \brief Read-access  send data packet callback.
        ///

        static int dispatch_data_read(void *metadata, void *payload, size_t bytes,
                        void *recv_func_parm, void *cookie) {
#if 0
                TRACE_ERR((stderr,
                                                "DatagramFactory::dispatch_data_read() .. \n"));

                DatagramFactory < T_Model, T_Device, T_LongHeader> *pf =
                (DatagramFactory < T_Model, T_Device,
                                T_LongHeader> *)recv_func_parm;

                // This packet device DOES NOT provide the data buffer(s) for the
                // message and the data must be read on to the stack before the
                // recv callback is invoked.

                uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
                void *p = (void *) &stackData[0];
                pf->getDevice()->readData(channel, (char *) p, bytes);

                dispatch_data_read(channel, metadata, p, bytes, recv_func_parm);
#else
                PAMI_abort();
#endif
                return 0;
        }
        ;

        ///
        /// \brief Read-access  send data packet callback.
        ///

        static int dispatch_short_data_read(void *metadata, void *payload,
                        size_t bytes, void *recv_func_parm, void *cookie) {
#if 0
                TRACE_ERR((stderr,
                                                "DatagramFactory::dispatch_data_read() .. \n"));

                DatagramFactory < T_Model, T_Device, T_LongHeader> *pf =
                (DatagramFactory < T_Model, T_Device,
                                T_LongHeader> *)recv_func_parm;

                // This packet device DOES NOT provide the data buffer(s) for the
                // message and the data must be read on to the stack before the
                // recv callback is invoked.

                uint8_t stackData[pf->getDevice()->getPacketPayloadSize()];
                void *p = (void *) &stackData[0];
                pf->getDevice()->readData(channel, (char *) p, bytes);

                dispatch_short_data_read(channel, metadata, p, bytes,
                                recv_func_parm);
#else
                PAMI_abort();
#endif
                return 0;
        };
        }; // PAMI::Protocol::Send::Datagram class
        // Initialize queue
        template < class T_Model, class T_Device, bool T_LongHeader, class T_Connection> Queue* Datagram < T_Model, T_Device, T_LongHeader, T_Connection>::_queue = NULL;

}
; // PAMI::Protocol::Send namespace
}
; // PAMI::Protocol namespace
}
; // PAMI namespace

#undef TRACE_ERR
#endif				// __pami_p2p_protocol_send_datagram_datagram_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
