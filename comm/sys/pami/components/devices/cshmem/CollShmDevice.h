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
 * \file components/devices/cshmem/CollShmDevice.h
 * \brief ???
 */

#ifndef __components_devices_cshmem_CollShmDevice_h__
#define __components_devices_cshmem_CollShmDevice_h__

#include "math/math_coremath.h"
#include "Global.h"
#include "Platform.h"
#include "Memory.h"
#include "components/devices/util/SubDeviceSuppt.h"
#include "components/devices/generic/AdvanceThread.h"
#include "components/devices/MultisyncModel.h"
#include "components/devices/MulticombineModel.h"
#include "components/devices/MulticastModel.h"
#include "components/devices/FactoryInterface.h"
#include "components/devices/cshmem/CollSharedMemoryManager.h"
#include "common/default/PipeWorkQueue.h"
#include "algorithms/protocols/allreduce/ReduceFunctions.h"

#undef TRACE_ERR
#define TRACE_ERR(x)  //fprintf x

#undef TRACE_DBG
#define TRACE_DBG(x)  //fprintf x

#undef PAMI_ASSERT
#define PAMI_ASSERT(x) PAMI_assert(x)

#define IMMEDIATE_CHANNEL_DATA_SIZE (CACHEBLOCKSZ-sizeof(window_control_t)-2*sizeof(char*))

namespace PAMI
{
  namespace Device
  {
    namespace CollShm
    {

//      static const int POLLCOUNT      = 1073741824;
//      static const int INNERPOLLCOUNT = 1073741824;
      static const int POLLCOUNT      = 2;
      static const int INNERPOLLCOUNT = 5;

      typedef union
      {
        pami_multicast_t mcast;
        pami_multisync_t msync;
        pami_multicombine_t mcombine;
      } collshm_multi_t;

      typedef enum
      {
        Generic = 0,
        MultiCast,
        MultiSync,
        MultiCombine
      } collshm_msgtype_t;

      template <typename T_Multi>
      struct message_type
      {
        static const unsigned msgtype = Generic;
      };
      template <> struct message_type<pami_multicast_t> { static const collshm_msgtype_t msgtype = MultiCast; };
      template <> struct message_type<pami_multisync_t> { static const collshm_msgtype_t msgtype = MultiSync; };
      template <> struct message_type<pami_multicombine_t> { static const collshm_msgtype_t msgtype = MultiCombine; };

///
/// \brief BaseCollShmMessage provides the getMsgType method inherited by
///        all collective shm messages in the device message queue.
///
      class BaseCollShmMessage : public PAMI::Device::Generic::GenericMessage
      {
        public :
          inline BaseCollShmMessage(GenericDeviceMessageQueue *device,
                                    pami_callback_t            cb,
                                    size_t                     client,
                                    size_t                     context):
            PAMI::Device::Generic::GenericMessage (device, cb, client, context)
          {}
          inline collshm_msgtype_t getMsgType() {return _msgtype;}
        protected :
          collshm_msgtype_t _msgtype;
      };

      template <typename T_Multi, class T_CollShmDevice>
      class CollShmMessage : public BaseCollShmMessage
      {
        public:
          inline CollShmMessage(GenericDeviceMessageQueue *device,
                                size_t                     client,
                                size_t                     context,
                                T_Multi                   *multi):
            BaseCollShmMessage (device, multi->cb_done, client, context),
            _multi (multi)
          {
            TRACE_DBG((stderr, "<%p>CollShmMessage::\n", this));
            _msgtype =  message_type< T_Multi >::msgtype;
          }

          inline pami_context_t postNext(bool devQueued)
          {
            T_CollShmDevice * qs = static_cast<T_CollShmDevice *>(getQS());
            return qs->postNextMsg();
          }
          inline T_Multi *getMulti() { return _multi; }
        private:
          T_Multi *_multi;
      };

///
/// \brief CollShmDevice class is message queue on a geometry's local topology. The messags and work threads
///        will be posted on generic devices. The CollShmDevice maintains a number of threads, each representing
///        work posted on one collective shared memory channel. There is a one-to-one relationship
///        between an active posted message, a thread and a collective shared memory channel
///        Template parameters T_NumSyncs and T_SyncCount define the circular queue usage of the threads
///        The array of threads are divided in to T_NumSyncs consecutive groups, each consisting of T_SyncCount
///        threads. Each group of threads has a corresponding barrier counters in shared memory. Whenever a
///        task completes the whole group of threads locally, it checks in the corresponding barrier. The
///        group of threads can be reused when the barrier completes. Both T_NumSyncs and T_SyncCount must be
///        power of two.
///
      template < class    T_Atomic,
                 class    T_MemoryManager,
                 unsigned T_NumSyncs,
                 unsigned T_SyncCount >
      class CollShmDevice : public GenericDeviceMessageQueue
      {
        public:
          static const unsigned _numsyncs    = T_NumSyncs;
          static const unsigned _synccounts  = T_SyncCount;
          static const unsigned _numchannels = T_NumSyncs * T_SyncCount;
          class CollShmWindow
          {
            public:
              typedef  enum
              {
                NODATA    = 0,
                IMMEDIATE,
                COPYINOUT,
                XMEMATT
              } window_content_t;

              typedef struct window_control_t
              {
                volatile window_content_t content : 8; // type of data
                volatile unsigned char    sync_flag;   // for barrier
                volatile unsigned char    avail_flag;  // 0 indicates no data, non-zero indicates data ready and reuse stage.
                volatile unsigned char    flags;       // flags reserved for active message collective,
                T_Atomic                  cmpl_cntr;   // for producer only, updated by
                // consumers through atomic operations
              } window_control_t;

              typedef struct xmem_descriptor_t
              {
#if defined(__64BIT__) && !defined(_LAPI_LINUX)
                volatile _css_mem_hndl_t hndl;
#else
                volatile int             hndl;
#endif
                volatile char            *src;
              } xmem_descriptor_t;

              typedef union window_data_t
              {
                xmem_descriptor_t xmem_data;
                char immediate_data[IMMEDIATE_CHANNEL_DATA_SIZE];
              } window_data_t;

              CollShmWindow():
                _buf_offset(-1UL),
                _len(0)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow()\n", this));
                clearCtrl();
                prepData();
              }

              inline void combineData(void *dst, void *src, size_t len, int flag, pami_op op, pami_dt dt)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::combineData()\n", this));

                if (flag)
                  {
                    coremath func;
                    unsigned sizeOfType;
                    int dtshift    = pami_dt_shift[dt];
                    void * buf[]   = { dst, src };
                    unsigned count = len >> dtshift;
                    CCMI::Adaptor::Allreduce::getReduceFunction(dt, op, sizeOfType, func);


                    TRACE_DBG((stderr, "Reducing Data dt=%d op=%p typesz=%d func=%p %p\n",
                               dt, op, sizeOfType, func, Core_int16_max));
                    func (dst, buf, 2, count);
                  }
                else
                  {
                    if(dst)
                      memcpy (dst, src, len);
                  }
              }
              inline void clearCtrl()
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::clearCtrl()\n", this));
                *((long long *)&_ctrl) = 0x0LL;
              }
              inline void freeBuf(T_MemoryManager *csmm)
              {
                if(_buf_offset != csmm->shm_null_offset())
                {
                  size_t *buf = (size_t*)csmm->offset_to_addr(_buf_offset);
                  *buf        = csmm->shm_null_offset();
                  csmm->returnDataBuffer((typename T_MemoryManager::shm_data_buf_t *)buf);
                  _buf_offset = csmm->shm_null_offset();
                }
              }
              inline void prepData() { }
              inline window_content_t getContent() { return _ctrl.content; }
              inline void setContent(window_content_t content)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::setContent() %u\n", this, content));
                Memory::sync(); // lwsync();
                _ctrl.content = content;
              }
              inline int getSyncflag() { return _ctrl.sync_flag; }
              inline void setSyncflag(int sync_flag) { _ctrl.sync_flag = sync_flag; }
              inline size_t getCmpl() { return _ctrl.cmpl_cntr.fetch(); }
              inline void setCmpl()
              {
//                PAMI_ASSERT(_ctrl.cmpl_cntr.fetch() >= 1);
                _ctrl.cmpl_cntr.fetch_and_sub(1);
              }
              inline pami_result_t setAvail(unsigned avail_value, unsigned cmpl_value)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::setAvail() avail_value %u,cmpl_value %u\n",
                           this,
                           avail_value, cmpl_value));
//                PAMI_ASSERT(_ctrl.cmpl_cntr.fetch() == 0);
                _ctrl.cmpl_cntr.fetch_and_add(cmpl_value);
                Memory::sync();
//                PAMI_ASSERT(_ctrl.avail_flag != avail_value);
                _ctrl.avail_flag = avail_value;

                if (_ctrl.cmpl_cntr.fetch() == 1)
                  {
                    _ctrl.cmpl_cntr.set(0);

                    if (_ctrl.content == XMEMATT) returnBuffer(NULL);

                    TRACE_DBG((stderr, "<%p>CollShmWindow::setAvail() PAMI_SUCCESS\n", this));
                    return PAMI_SUCCESS;
                  }

                TRACE_DBG((stderr, "<%p>CollShmWindow::setAvail() PAMI_EAGAIN\n", this));
                return PAMI_EAGAIN;
              }

              ///
              /// \brief Check the availibilty of the data or buffer
              ///
              inline pami_result_t isAvail(unsigned value)
              {
                if (_ctrl.avail_flag != value)
                  {
                    TRACE_DBG((stderr, "<%p>CollShmWindow::isAvail() value %u cmpflag=%u PAMI_EAGAIN\n", this, value, _ctrl.avail_flag));
                    return PAMI_EAGAIN;
                  }

                Memory::sync<Memory::instruction>(); //isync();
                TRACE_DBG((stderr, "<%p>CollShmWindow::isAvail() value %u cmpflag=%u PAMI_SUCCESS\n", this, value, _ctrl.avail_flag));
                return PAMI_SUCCESS;
              }

              ///
              /// \brief Producer prepares data or buffer for sharing with consumers
              ///        PipeWorkQueue version
              ///
              /// \param src    source pipeworkqueue
              /// \param length length of data to produce into the window
              /// \parma _csmm  pointer to memeory manager in case a shared memory buffer is needed
              ///
              /// \return length of data produced into the window
              ///         -1 indicates there is no data buffer
              ///          associated with the window and no memory
              ///          manager specified, or no memory available
            inline size_t produceData(PAMI::PipeWorkQueue &src, size_t length, T_MemoryManager *csmm, bool do_consume=true)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::produceData() src %p/%p, trying length %zu :avail=%lu\n",
                           this, &src, src.bufferToConsume(), length, src.bytesAvailableToConsume()));

                //SSS: This if statement would disable non-contig data
                //if (src.bytesAvailableToConsume() < MIN(length, COLLSHM_BUFSZ) ) return 0;
                size_t reqbytes = MIN (length, src.bytesAvailableToConsume());

                if (reqbytes < IMMEDIATE_CHANNEL_DATA_SIZE)
                  {
                    _len = reqbytes;

                    memcpy (_data.immediate_data, src.bufferToConsume(), _len);
                    if(do_consume)
                      src.consumeBytes(_len);
                    setContent(IMMEDIATE);
                  }
                else if (reqbytes <= XMEM_THRESH)
                  {
                    _len = MIN(reqbytes, COLLSHM_BUFSZ);
                    void* buf = csmm->offset_to_addr(_buf_offset);
                    if (buf == csmm->shm_null_ptr())
                      {
                        buf         = csmm->getDataBuffer(1);
                        _buf_offset = csmm->addr_to_offset(buf);
                        if (buf == NULL)
                        {
                          _buf_offset = csmm->shm_null_offset();
                          return -1;
                        }
                      }
                    memcpy (buf, src.bufferToConsume(), _len);
                    if(do_consume)
                      src.consumeBytes(_len);
                    setContent(COPYINOUT);
                  }
                else
                  {
#if  defined(__64BIT__) && !defined(_LAPI_LINUX)
                    _css_shmem_reg_info_t  reg;
                    // PAMI_ASSERT( XMEM );
                    reg.command  = CSS_SHMEM_REG;
                    reg.pointer  = (long long)src.bufferToConsume();
                    reg.len      = (long long)length;
                    reg.hndl_out = ZCMEM_HNDL_NULL;
                    PAMI_ASSERT(! _css_shmem_register((zcmem_t)&reg));
                    _len = reqbytes;
                    _data.xmem_data.src    = (volatile char *)src.bufferToConsume();
                    _data.xmem_data.hndl   = reg.hndl_out;
                    setContent(XMEMATT);
#else
                    PAMI_ASSERT(0);
#endif
                  }

                return _len;
              }

              ///
              /// \brief Producer prepares data or buffer for sharing with consumers
              ///        Flat buffer version, src is a pipeworkqueue
              ///
              inline size_t produceData(char *src, size_t length, T_MemoryManager *csmm)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::produceData() src %p, length %zu\n", this, src, length));
                if (length < IMMEDIATE_CHANNEL_DATA_SIZE)
                  {
                    _len = length;
                    memcpy (_data.immediate_data, src, _len);
                    setContent(IMMEDIATE);
                  }
                else if (length <= XMEM_THRESH)
                  {
                    _len = MIN(length, COLLSHM_BUFSZ);

                    void* buf = csmm->offset_to_addr(_buf_offset);
                    if (buf == csmm->shm_null_ptr())
                      {
                        buf         = (char *)csmm->getDataBuffer(1);
                        _buf_offset = csmm->addr_to_offset(buf);
                        if (buf == NULL)
                        {
                          _buf_offset = csmm->shm_null_offset();
                          return -1;
                        }
                      }

                    memcpy (buf, src, _len);
                    setContent(COPYINOUT);
                  }
                else
                  {
#if  defined(__64BIT__) && !defined(_LAPI_LINUX)
                    _css_shmem_reg_info_t  reg;
                    //PAMI_ASSERT( XMEM );
                    reg.command  = CSS_SHMEM_REG;
                    reg.pointer  = (long long)src;
                    reg.len      = (long long)length;
                    reg.hndl_out = ZCMEM_HNDL_NULL;
                    PAMI_ASSERT(! _css_shmem_register((zcmem_t)&reg));
                    _len = length;
                    _data.xmem_data.src    = (volatile char *)src;
                    _data.xmem_data.hndl   = reg.hndl_out;
                    setContent(XMEMATT);
#else
                    PAMI_ASSERT(0);
#endif
                  }

                return _len;
              }

              ///
              /// \brief Consumer consumes data, PipeWorkQueue version, destination is a pipeworkqueue
              ///
              inline size_t consumeData(PAMI::PipeWorkQueue &dest, size_t length, int combine_flag, pami_op op, pami_dt dt,T_MemoryManager *csmm)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::consumeData() dest %p/%p, length %zu\n", this, &dest, dest.bufferToConsume(), length));
                if (dest.bytesAvailableToProduce() < MIN(length, _len)) return 0;

                size_t reqbytes = MIN (length, dest.bytesAvailableToProduce());
                size_t len      = MIN (reqbytes, _len);
#if  defined(__64BIT__) && !defined(_LAPI_LINUX)
                char *src;
#endif
                TRACE_DBG((stderr, "Using Control Structure this=%p %p\n", this, &_ctrl));
                switch (_ctrl.content)
                  {
                    case IMMEDIATE:
                      combineData(dest.bufferToProduce(), _data.immediate_data, len, combine_flag, op, dt);
                      break;
                    case COPYINOUT:
                    {
                      void* buf = csmm->offset_to_addr(_buf_offset);
                      combineData(dest.bufferToProduce(), buf, len, combine_flag, op, dt);
                    }
                      break;
#if  defined(__64BIT__) && !defined(_LAPI_LINUX)
                    case XMEMATT:
                      // PAMI_ASSERT( XMEM );
                      PAMI_ASSERT( _data.xmem_data.hndl != ZCMEM_HNDL_NULL);
                      _css_shmem_att_info_t  att;
                      att.command  = CSS_SHMEM_ATT;
                      att.hndl_att = _data.xmem_data.hndl;
                      att.req_ptr  = (long long)_data.xmem_data.src;
                      att.req_len = (long long)len;
                      att.offset   = (long long) 0;
                      PAMI_ASSERT(! _css_shmem_attach((zcmem_t)&att));
                      PAMI_ASSERT(att.llen_avail == att.req_len);
                      src = (char *) (att.pointer + att.latt_offset);
                      combineData(dest.bufferToProduce(), src, len, combine_flag, op, dt);
                      att.command = CSS_SHMEM_DET;
                      att.hndl_det = att.hndl_att;
                      PAMI_ASSERT(! _css_shmem_attach((zcmem_t)&att) );
                      break;
#endif
                    default:
                      fprintf(stderr, "<%p>CollShmWindow::consumeData() value of content is %d\n", this, _ctrl.content);
                      PAMI_ASSERT(0);
                  }

                dest.produceBytes(len);
                return len;
              }

              ///
              /// \brief Consumer consumes data, destination is a flat buffer
              ///
              inline size_t consumeData(char *dest, size_t length, int combine_flag, pami_op op, pami_dt dt,T_MemoryManager *csmm)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::consumeData() dest %p, length %zu\n", this, dest, length));
                size_t len      = MIN (length, _len);
#if  defined(__64BIT__) && !defined(_LAPI_LINUX)
                char *src;
#endif
                int y = _ctrl.content;
                switch (y)
                  {
                    case IMMEDIATE:
                      combineData(dest, _data.immediate_data, len, combine_flag, op, dt);
                      break;
                    case COPYINOUT:
                    {
                      void* buf = csmm->offset_to_addr(_buf_offset);
                      combineData(dest, buf, len, combine_flag, op, dt);
                    }
                      break;
#if defined(__64BIT__) && !defined(_LAPI_LINUX)
                    case XMEMATT:
                      // PAMI_ASSERT( XMEM );
                      PAMI_ASSERT( _data.xmem_data.hndl != ZCMEM_HNDL_NULL);
                      _css_shmem_att_info_t  att;
                      att.command  = CSS_SHMEM_ATT;
                      att.hndl_att = _data.xmem_data.hndl;
                      att.req_ptr  = (long long)_data.xmem_data.src;
                      att.req_len = (long long)len;
                      att.offset   = (long long) 0;
                      PAMI_ASSERT(! _css_shmem_attach((zcmem_t)&att));
                      PAMI_ASSERT(att.llen_avail == att.req_len);
                      src = (char *) (att.pointer + att.latt_offset);
                      combineData(dest, src, len, combine_flag, op, dt);
                      att.command = CSS_SHMEM_DET;
                      att.hndl_det = att.hndl_att;
                      PAMI_ASSERT(! _css_shmem_attach((zcmem_t)&att) );
                      break;
#endif
                    default:
                      TRACE_DBG((stderr, "<%p>CollShmWindow::consumeData() valus of content is %d\n", this, _ctrl.content));
                      PAMI_assertf(0, "<%p>switch %d, dest %p, length %zu/%zu, combine_flag %d, op %u, dt %u y=%d\n",
                                   this, _ctrl.content, dest, length, len, combine_flag, op, dt, y);
                  }

                return len;
              }

              ///
              /// \brief Get data buffer, can be used to directly manipulate data from multiple windows
              ///
              inline char *getBuffer(T_MemoryManager *csmm)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::getBuffer()\n", this));
                char *buf = (char*)csmm->shm_null_ptr();

                switch (_ctrl.content)
                  {
                    case IMMEDIATE:
                      buf = &(_data.immediate_data[0]);
                      break;
                    case COPYINOUT:
                    {
                      char* tbuf = (char*)csmm->offset_to_addr(_buf_offset);
                      if (tbuf == csmm->shm_null_ptr())
                        {
                          tbuf        = (char *)csmm->getDataBuffer(1);
                          PAMI_ASSERT(tbuf);
                        }
                      buf         = tbuf;
                      _buf_offset = csmm->addr_to_offset(buf);
                    }
                      break;
#if defined(__64BIT__) && !defined(_LAPI_LINUX)
                    case XMEMATT:
                      // PAMI_ASSERT( XMEM );
                      PAMI_ASSERT( _data.xmem_data.hndl != ZCMEM_HNDL_NULL);
                      _css_shmem_att_info_t  att;
                      att.command  = CSS_SHMEM_ATT;
                      att.hndl_att = _data.xmem_data.hndl;
                      att.req_ptr  = (long long)_data.xmem_data.src;
                      att.req_len  = (long long)_len;
                      att.offset   = (long long) 0;
                      PAMI_ASSERT(! _css_shmem_attach((zcmem_t)&att));
                      PAMI_ASSERT(att.llen_avail == att.req_len);
                      buf = (char *) (att.pointer + att.latt_offset);
                      break;
#endif
                    default:
                      PAMI_ASSERT(0);
                  }

                return buf;
              }            
              ///
              /// \brief Return data buffer, only really needed to detach buffer in Xmem attach case
              ///
              inline void returnBuffer(char *buf)
              {
                TRACE_DBG((stderr, "<%p>CollShmWindow::returnBuffer()\n", this));
#if defined(__64BIT__) && !defined(_LAPI_LINUX)
                PAMI_ASSERT (_ctrl.content == XMEMATT);
                PAMI_ASSERT( _data.xmem_data.hndl != ZCMEM_HNDL_NULL);

                _css_shmem_reg_info_t  reg;
                reg.command  = CSS_SHMEM_REL;
                reg.hndl_in  = _data.xmem_data.hndl;
                PAMI_ASSERT(! _css_shmem_register((zcmem_t)&reg));
#endif
              }
            inline size_t getBufOffset()
              {  return _buf_offset; }
            inline void   setBufOffset(size_t set)
              {  _buf_offset = set;}
            protected:
              window_control_t _ctrl;
              size_t           _buf_offset;
              volatile size_t  _len;
              window_data_t    _data;
          } __attribute__((__aligned__(CACHEBLOCKSZ) ));  // class CollShmWindow


          class CollShmThread : public PAMI::Device::Generic::GenericAdvanceThread
          {
            public:

              typedef enum
              {
                NOACTION = 0,
                CSOSYNC,
                READFROM,
                SHAREWITH,
                EXCHANGE,
                OPDONE,
              } collshm_action_t;

              typedef uint64_t collshm_mask_t;

              typedef enum
              {
                NOROLE = 0,
                PARENT,
                CHILD,
                BOTH
              } collshm_role_t;

              static pami_result_t advanceThread(pami_context_t context, void *thr)
              {
                CollShmThread *t = (CollShmThread *) thr;
                return t->_advanceThread(context);
              }

              // children in the returned array are in ascending order
              static void getchildren_knary(uint8_t  rank,
                                            uint8_t  k,
                                            uint8_t  tasks,
                                            uint8_t *children,
                                            uint8_t *numchildren,
                                            uint8_t *parent)
              {
                int i;
                PAMI_ASSERT(k);

                if (rank * k + 1 > tasks)
                  *numchildren = 0;
                else
                  *numchildren = MIN((tasks - (rank * k + 1)), k);

                for (i = 0 ; i < *numchildren; i++)
                  {
                    children[i] = rank * k + 1 + i;
                  }
                *parent = (uint8_t) (rank + k - 1) / k - 1;
              }

              CollShmThread() : PAMI::Device::Generic::GenericAdvanceThread()
              {}

              CollShmThread(unsigned idx, CollShmDevice *device) :
                PAMI::Device::Generic::GenericAdvanceThread(),
                _idx(idx),
                _step(0),
                _action(NOACTION),
                _partners(0ULL),
                _sync_flag(0),
                _target_cntr(0),
                _root((unsigned char) - 1),
                _arank(device->getRank()),
                _nranks(device->getSize()),
                _role(NOROLE),
                _device(device)
              {
                setAdv(advanceThread);
              }
              inline void resetThread()
              {
                _root        = -1;
                _step        = 0;
                _action      = NOACTION;
                _partners    = 0x0ULL;
                _sync_flag   = 0;
                _target_cntr = 0;
              }
              inline collshm_action_t getAction() { return _action; }
              inline void setAction(collshm_action_t action) { _action = action; }
              inline unsigned int getIdx() { return _idx; }
              inline pami_result_t _advanceThread(pami_context_t context)
              {
                pami_result_t rc = PAMI_SUCCESS;
                BaseCollShmMessage *msg = (BaseCollShmMessage *)getMsg();
                CollShmWindow  *window;
                int pollcnt = INNERPOLLCOUNT;

                switch (_action)
                  {
                    case NOACTION: // just started
                      break;
                    case CSOSYNC:

                      while (_partners && pollcnt--)
                        {
                          for (collshm_mask_t partner = 0x1ULL, i = 0ULL; i < _nranks; ++i, partner <<= 1ULL)
                            {
                              if (_partners & partner)
                                {
                                  window = _device->getWindow(0, i, _idx);

                                  if (window->getSyncflag() == _sync_flag)
                                    {
                                      _partners ^= partner;
                                      pollcnt = INNERPOLLCOUNT;
                                    }
                                  else
                                    break;
                                }
                            }
                        }

                      if (_partners) return PAMI_EAGAIN;
                      break;
                    case SHAREWITH:
                      window = _device->getWindow(0, _arank, _idx);

                      if (window->getCmpl() != 1) return PAMI_EAGAIN;
                      window->setCmpl();

                      if (window->getContent() == CollShmWindow::XMEMATT) window->returnBuffer(NULL);

                      break;
                    case READFROM:

                      for (collshm_mask_t partner = 0ULL; partner < _nranks; ++partner)
                        {
                          if (_partners & (0x1ULL << partner))
                            {
                              window = _device->getWindow(0, partner, _idx);

                              if (window->isAvail(_target_cntr) == PAMI_SUCCESS)
                                _partners = _partners & (~(0x1ULL << partner)) ;
                              else
                                return PAMI_EAGAIN;
                            }
                        }

                      break;
                    case EXCHANGE: // not implemented yet
                    default:
                      PAMI_ASSERT(0);
                      break;
                  }

                TRACE_DBG((stderr, "<%p>CollShmThread::_advanceThread() msg %p\n", this, msg));
                if (msg)
                  {
                    switch (msg->getMsgType())
                      {
                        case MultiCast:
                          rc = progressMulticast(static_cast< CollShmMessage<pami_multicast_t, CollShmDevice> *>(msg));
                          break;
                        case MultiSync:
                          rc = progressMultisync(static_cast< CollShmMessage<pami_multisync_t, CollShmDevice> *>(msg));
                          break;
                        case MultiCombine:
                          rc = progressMulticombine(static_cast< CollShmMessage<pami_multicombine_t, CollShmDevice> *>(msg));
                          TRACE_DBG((stderr, "<%p>Progress Multicombine CollShmThread::_advanceThread() msg %p rc=%d\n", this, msg, rc));
                          break;
                        default:
                          PAMI_ASSERT(0);
                          break;
                      }
                  }

                if (rc == PAMI_SUCCESS) _device->setThreadAvail(getIdx());
                return rc;
              }
              inline void _setPartners()
              {
                PAMI_ASSERT(_nchildren <= PAMI_MAX_PROC_PER_NODE);
                _partners        = 0x0ULL;
                for (int j = 0; j < _nchildren; ++j)
                  _partners = _partners | (0x1ULL << ((_children[j] + _root) % _nranks ));
                TRACE_DBG((stderr, "SetPartners:  %p\n", _partners));
              }
              inline void _setRole()
              {
                if (_parent != (unsigned char) - 1 && _nchildren > 0)
                  _role = BOTH;
                else if (_parent != (unsigned char) - 1)
                  _role = CHILD;
                else if (_nchildren > 0)
                  _role = PARENT;
                else
                  PAMI_ASSERT(0);
              }
              inline void initThread( collshm_msgtype_t msgtype )
              {
                unsigned char k = _nranks - 1;
                size_t root;
                PAMI::Topology *topo;
                pami_multicast_t *mcast;
                pami_multicombine_t *mcombine;
                resetThread();
                
                TRACE_DBG((stderr, "<%p>CollShmThread::initThread() msgtype %u\n", this, msgtype));

                switch (msgtype)
                  {
                    case MultiCast:
                    {
                      mcast  = (pami_multicast_t *) static_cast<CollShmMessage<pami_multicast_t, CollShmDevice> *>(_msg)->getMulti();
                      topo   = (PAMI::Topology *)mcast->src_participants;
                      root   = _device->getTopo()->endpoint2Index(topo->index2Endpoint(0));
                      _len   = mcast->bytes;
                      _wlen  = 0;
                      // Todo:  non-flat n-ary trees give incorrect results
                      // for bcast > COLLSHM_BUFSZ
                      if(_len <= COLLSHM_BUFSZ)
                        k      = 2;
                      TRACE_DBG((stderr, "CollShmThread::initThread() MultiCast topo->index2Endpoint(0) %d\n", (int)topo->index2Endpoint(0)));
                    } 
                      break;
                    case MultiCombine:
                      mcombine = (pami_multicombine_t *) static_cast<CollShmMessage<pami_multicombine_t, CollShmDevice> *>(_msg)->getMulti();
                      topo     = (PAMI::Topology *)mcombine->results_participants;
                      root     = _device->getTopo()->endpoint2Index(topo->index2Endpoint(0));
                      _sync_flag = 0;
                      _len     = mcombine->count << pami_dt_shift[mcombine->dtype] ;
                      _wlen    = 0;
                      k        = 2; // binary for better latency
                      TRACE_DBG((stderr, "CollShmThread::initThread, Multicombine() topo=%p topo->index2Endpoint(0) %d size=%ld:\n",
                                 mcombine->results_participants,
                                 (int)topo->index2Endpoint(0),
                                 (int)topo->size()));
                      break;
                    case MultiSync:
                      TRACE_DBG((stderr, "CollShmThread::initThread() MultiSync\n"));
                      _sync_flag    = 1;
                      root          = 0;
                      k             = 2;  // binary for better latency
                      break;
                    default:
                      root = 0; // avoid uninitialized warnings
                      PAMI_ASSERT(0);
                      break;
                  }

                _rrank           = (_arank + _nranks - root) % _nranks;
                uint8_t rootcast = (uint8_t)root;
                TRACE_DBG((stderr, "rootcast = %d, _root = %d, arank=%d nranks=%d rrank=%d\n",
                           (int)rootcast, (int)_root, _arank, _nranks, _rrank));
                if (rootcast != _root)
                  {
                    _root = rootcast;
                    getchildren_knary(_rrank, MIN(k, MAX(1, _nranks - 1)), _nranks, &_children[0], &(_nchildren), &(_parent));
                  }

                _setRole();
              }

              inline pami_result_t progressMultisync( CollShmMessage<pami_multisync_t, CollShmDevice> *msg)
              {
                TRACE_DBG((stderr, "<%p>CollShmThread::progressMultisync() msg %p\n", this, msg));
                // pami_result_t rc;
                // PAMI_ASSERT(_sync_flag == 0);
                // if (_sync_flag != _target_cntr)  rc = _device->sync(_idx, _sync_flag);
                // return rc;
                int pollcnt = INNERPOLLCOUNT;
                CollShmWindow *window;

                if (_action == NOACTION)
                  {
                    _setPartners();
                    _action = CSOSYNC;
                    collshm_mask_t partner = 0x1ULL;
                    int i = 0;

                    while (_partners && pollcnt--)
                      {
                        for (; i < _nranks; ++i, partner <<= 1ULL)
                          {
                            if (_partners & partner)
                              {
                                window = _device->getWindow(0, i, _idx);

                                if (window->getSyncflag() == _sync_flag)
                                  {
                                    _partners ^= partner;
                                    pollcnt = INNERPOLLCOUNT;
                                  }
                                else
                                  break;
                              }
                          }
                      }

                    if (_partners)
                    {
                      TRACE_DBG((stderr, "EAGAIN partners=%lp\n", _partners));
                      return PAMI_EAGAIN;
                    }
                  }

                if (!_step)
                  {
                    Memory::sync(); // lwsync();
                    window = _device->getWindow(0, _arank, _idx);
                    TRACE_DBG((stderr, "window addr = %p\n", window));
                    TRACE_DBG((stderr, "window addr = %p, sync flag=%d\n", window, _sync_flag));
                    window->setSyncflag(_sync_flag);
                    _step = 1;
                  }

                pollcnt = INNERPOLLCOUNT;
                window = _device->getWindow(0, 0, _idx);

                while(window->getSyncflag() != _sync_flag && (pollcnt--));

                if (window->getSyncflag() != _sync_flag)
                {
                  TRACE_DBG((stderr, "EAGAIN _sync_flag=%d\n", _sync_flag));
                  return PAMI_EAGAIN;
                }
                else
                  {
                    Memory::sync<Memory::instruction>(); //isync();
                    msg->setStatus(PAMI::Device::Done);
                    setMsg(NULL);
                    TRACE_DBG((stderr, "shm_sync, _sync_flag =%d\n", _sync_flag));
                    return PAMI_SUCCESS;
                  }
              }

              ///
              /// \brief Further progress on multicombine
              ///
              inline pami_result_t progressMulticombine( CollShmMessage<pami_multicombine_t, CollShmDevice> *msg)
              {
                TRACE_DBG((stderr, "<%p>progressMulticombine msg enter %p, idx %d %lu\n", this, msg, _idx, _len));
                pami_result_t rc = PAMI_SUCCESS;
                pami_multicombine_t *mcombine = msg->getMulti();
                CollShmWindow *window = _device->getWindow(0, _arank, _idx);
                int i, prank;

                while (_len != 0)
                {
                    if (_role == CHILD)
                      {
                        TRACE_DBG((stderr, "<%p>CHILD  progressMulticombine msg %p\n", this));
                        _wlen = window->produceData(*(PAMI::PipeWorkQueue *)mcombine->data, _len, _device->getSysdep());

                        if (_wlen == 0)
                          {
                            _action = NOACTION;
                            TRACE_DBG((stderr, "<%p>CHILD, NOACTION  progressMulticombine msg %p, idx %d EAGAIN 1\n", this, msg, _idx));
                            return PAMI_EAGAIN;
                          }

                        PAMI_ASSERT(_wlen != -1ULL);
                        _len  -= _wlen;
                        ++_step ;
                        rc = window->setAvail(_step, 2);
                        if (rc != PAMI_SUCCESS)
                          {
                            TRACE_DBG((stderr, "<%p>CHILD, SHAREWITH progressMulticombine msg %p, idx %d EAGAIN 2\n", this, msg, _idx));
                            _action = SHAREWITH;
                            break;
                          }
                      }
                    else
                      {
                        // PARENT or BOTH
                        TRACE_DBG((stderr, "<%p>PARENT or BOTH  progressMulticombine msg %p\n", this));

                        if (_action == READFROM)
                          {
                            if (_wlen > 0)
                              {
                                for (i = 0; i < _nchildren; ++i)
                                  {
                                    prank = (_children[i] + _root) % _nranks;
                                    CollShmWindow *pwindow = _device->getWindow(0, prank, _idx);
                                    size_t len = pwindow->consumeData(window->getBuffer(_device->getSysdep()), _wlen, 1,
                                                                      mcombine->optor, mcombine->dtype,
                                                                      _device->getSysdep());
                                    (void)len;
                                    PAMI_ASSERT(len == _wlen);
                                    pwindow->setCmpl();
                                  }

                                _len  -= _wlen;
                              }

                            if (_role == BOTH)
                              {
                                _action = SHAREWITH;
                                rc = window->setAvail(_step, 2);

                                if (rc != PAMI_SUCCESS)
                                  {
                                    TRACE_DBG((stderr, "<%p>progressMulticombine msg %p, idx %d EAGAIN 3\n", this, msg, _idx));
                                    break;
                                  }
                              }
                            else
                              {
                                // PARENT
                                size_t len = window->consumeData(*(PAMI::PipeWorkQueue *)mcombine->results, _wlen,0,
                                                                 PAMI_OP_COUNT, PAMI_DT_COUNT,
                                                                 _device->getSysdep());

                                if (len == 0)
                                  {
                                    _wlen = len;
                                    TRACE_DBG((stderr, "<%p>progressMulticombine msg %p, idx %d EAGAIN 4\n", this, msg, _idx));
                                    return PAMI_EAGAIN;
                                  }
                              }

                            if (_len == 0) break;
                          }

                        _wlen = window->produceData(*(PAMI::PipeWorkQueue *)mcombine->data, _len, _device->getSysdep());

                        if (_wlen == 0)
                          {
                            _action = NOACTION;
                            TRACE_DBG((stderr, "<%p>progressMulticombine msg %p, idx %d EAGAIN 5\n", this, msg, _idx));
                            return PAMI_EAGAIN;
                          }

                        PAMI_ASSERT(_wlen != -1ULL);

                        ++_step ;
                        _action = READFROM;
                        _setPartners();
                        collshm_mask_t partner = 0x1ULL;
                        for (int i = 0 ; i < _nranks; ++i, partner <<= 1ULL)
                          {
                            TRACE_DBG((stderr, "<%p>progressMulticombine msg %p, idx %d: _partners=%lX partner=%lX\n",
                                       this, msg, _idx,
                                       _partners,
                                       partner));

                            if (_partners & partner)
                              {
                                CollShmWindow *pwindow = _device->getWindow(0, i, _idx);
                                TRACE_DBG((stderr, "<%p>Calling isAvail %p HERE1\n", this, msg));

                                if (pwindow->isAvail(_step) == PAMI_SUCCESS)
                                  _partners ^= partner;
                                else
                                  {
                                    _target_cntr = _step;
                                    TRACE_DBG((stderr, "<%p>progressMulticombine msg %p, idx %d EAGAIN 6 _partners=%lX partner=%lX\n",
                                               this, msg, _idx,
                                               _partners,
                                               partner));
                                    return PAMI_EAGAIN;
                                  }
                              }
                          }
                      }
                  }

                TRACE_DBG((stderr, "<%p>progressMulticombine msg %p, _len=%lu rc=%p wlen=%lu\n", this, msg, _len, rc, _wlen));

                if (_len == 0)
                  {
                    if (rc == PAMI_SUCCESS || _wlen <= XMEM_THRESH)
                      {
                        msg->setStatus(PAMI::Device::Done);
                        setMsg(NULL);
                      }
                  }
                return rc;
              }

              ///
              /// \brief Further progress on multicast, with pipelining support
              ///
              inline pami_result_t progressMulticast( CollShmMessage<pami_multicast_t, CollShmDevice> *msg)
              {
                TRACE_DBG((stderr, "<%p>CollShmThread::progressMulticast() msg=%p _arank=%d _idx=%d\n", this, msg, _arank, _idx));
                pami_result_t rc = PAMI_SUCCESS;
                pami_multicast_t *mcast = msg->getMulti();
                CollShmWindow *window = _device->getWindow(0, _arank, _idx);
                int prank;

                while (_len != 0)
                  {
                    if (_role == PARENT)
                      {
                        TRACE_DBG((stderr, "<%p>CSM::PARENT(), produceData msg=%p _role=%d _arank=%d _idx=%d window=%p mcast=%p, mc->src=%p\n",
                                   this, msg, _role, _arank, _idx, window, mcast, mcast->src));
                        _wlen = window->produceData(*(PAMI::PipeWorkQueue *)mcast->src, _len, _device->getSysdep());
                        TRACE_DBG((stderr, "<%p>CSM::PARENT() produceData Done\n"));

                        if (_wlen == 0)
                          {
                            _action = NOACTION;
                            return PAMI_EAGAIN;
                          }

                        PAMI_ASSERT(_wlen != -1ULL);
                        _len  -= _wlen;
                        ++_step ;
                        rc = window->setAvail(_step, _nchildren + 1);
                        if (rc != PAMI_SUCCESS)
                          {
                            _action = SHAREWITH;
                            break;
                          }
                      }
                    else
                      {
                        // CHILD or BOTH
                        prank = (_parent + _root) % _nranks;
                        CollShmWindow *pwindow = _device->getWindow(0, prank, _idx);

                        if (_action == READFROM)
                          {
                            _wlen = pwindow->consumeData(*(PAMI::PipeWorkQueue *)mcast->dst, _len, 0,
                                                         PAMI_OP_COUNT, PAMI_DT_COUNT,
                                                         _device->getSysdep());

                            if (_wlen == 0)return PAMI_EAGAIN;

                            pwindow->setCmpl();
                            _len  -= _wlen;

                            if (_role == BOTH)
                              {
                                /// \todo Resolve SSS's conflict here with CJA's fix
                                //SSS: We need to set do_consume to true for correctness in non-contig data cases.
                                //     If do_consume is false, only first chunk is consumed repeatedly.
                                size_t len = window->produceData(*(PAMI::PipeWorkQueue *)mcast->dst, _wlen, _device->getSysdep(),false);(void)len;

                                PAMI_ASSERT(len == _wlen);
                                _action = SHAREWITH;
                                rc = window->setAvail(_step, _nchildren + 1);

                                if (rc != PAMI_SUCCESS) break;
                              }

                            if (_len == 0) break;
                          }

                        ++_step ;
                        _action = READFROM;

                        if (pwindow->isAvail(_step) != PAMI_SUCCESS)
                          {
                            _target_cntr   = _step;
                            _partners      = 1ULL << prank;
                            return PAMI_EAGAIN;
                          }
                      }
                  }
                if (_len == 0)
                  {
                    if (rc == PAMI_SUCCESS || _wlen <= XMEM_THRESH)
                      {
                        msg->setStatus(PAMI::Device::Done);
                        setMsg(NULL);
                      }
                  }

                return rc;
              }
            protected:
              unsigned char  _idx;
              unsigned       _step;
              collshm_action_t _action;
              collshm_mask_t _partners;
              unsigned char  _sync_flag;
              unsigned char  _target_cntr;
              size_t         _len;
              size_t         _wlen;
              uint8_t        _root;
              uint8_t        _rrank;               // relative on node rank
              uint8_t        _arank;               // absolute on node rank
              uint8_t        _nranks;              // total number of on node ranks for the topology
              uint8_t        _parent;              // these needs to be optimized
              uint8_t        _nchildren;
              uint8_t        _children[PAMI_MAX_PROC_PER_NODE];// enough to support flat tree of max procs per node
              collshm_role_t _role;
              CollShmDevice  *_device;
          }; // CollShmThread

          typedef struct collshm_wgroup_t
          {
            size_t            next_offset;
            unsigned          context_id : 16;
            unsigned          num_tasks  : 8;
            unsigned          task_rank  : 8;
            T_Atomic          barrier[sizeof(int)][_numsyncs]; // only the first two rows are used
            // char           pad[CACHEBLOCKSZ-(((_numsyncs*sizeof(int)+1)*sizeof(int)+sizeof(void *))%CACHEBLOCKSZ)];
            CollShmWindow     windows[_numchannels];
          } collshm_wgroup_t __attribute__ ((__aligned__ (CACHEBLOCKSZ)));

          static pami_result_t advanceQueue(pami_context_t context, void *dev)
          {
            CollShmDevice *d = (CollShmDevice *)dev;
            
            return d->_advanceQueue(context);
          }

          CollShmDevice(PAMI::Device::Generic::Device *devices,
                        unsigned                       gid,
                        pami_endpoint_t                my_endpoint,
                        PAMI::Topology                *topo,
                        T_MemoryManager               *csmm,
                        void                          *in_str) :
            GenericDeviceMessageQueue(),
            _topo(topo),
            _csmm(csmm),
            _generics(devices),
            _gid(gid),
            _ntasks(topo->size()),
            _tid(topo->endpoint2Index(my_endpoint)),
            _syncbits(0),
            _head(_numchannels),
            _tail(0),
            _round(0)
          {
            int num = _synccounts;

            while (num >>= 1) ++_syncbits;
            TRACE_DBG((stderr, "syncbits = %d\n", _syncbits));

            void * str = (char*)csmm->getCollShmAddr() + (size_t)in_str;
            PAMI_ASSERT(str != csmm->shm_null_ptr());
            COMPILE_TIME_ASSERT(T_MemoryManager::_windowsz>=sizeof(collshm_wgroup_t));

            collshm_wgroup_t *ctlstr = (collshm_wgroup_t *)str;

            for (unsigned i = 0;  i < _numchannels; ++i)
              {
                new (&_threads[i]) CollShmThread(i, this);
              }

            new (&_threadm) PAMI::Device::Generic::GenericAdvanceThread();
            _threadm.setStatus(PAMI::Device::Idle);
            _threadm.setFunc(advanceQueue, this);

            // initialize increments to 1 for both rounds
            for (int i = 0; i < 2; ++i)
              {
                _increments[i] = 1;

                for (unsigned j = 0; j < _numsyncs; ++j)
                  _completions[i][j] = 0;
              }

            // initialize shm channels
            for (unsigned i = 0;  i < _ntasks; ++i)
              {
                TRACE_DBG((stderr, "%d: ctlstr is %p\n", i, ctlstr));
                PAMI_assert(ctlstr != csmm->shm_null_ptr());
                _wgroups[i] = ctlstr;
                ctlstr      = (collshm_wgroup_t*)((intptr_t)csmm->getCollShmAddr() + ctlstr->next_offset);
                csmm->ctl_str_check(_wgroups[i]);
                if (_tid == 0)
                  {
                    if(!_wgroups[i])
                      fprintf(stderr, "Error:  _wgroups[%d] is NULL\n", i);
                    
                    PAMI_assert(_wgroups[i]);
                    memset((char*)_wgroups[i]+sizeof(size_t), 0, sizeof(_wgroups[i])-sizeof(size_t));
                    Memory::sync();
                    _wgroups[i]->context_id = _gid;
                    _wgroups[i]->num_tasks  = _ntasks;
                    _wgroups[i]->task_rank  = i;

                    for(unsigned j = 0; j < _numchannels; j++)
                    {
                      memset(&_wgroups[i]->windows[j], 0, sizeof(_wgroups[i]->windows[j]));
                      _wgroups[i]->windows[j].setBufOffset(csmm->shm_null_offset());
                    }
                    
                    for (int j = 0; j < 2; ++j)
                      for (unsigned k = 0; k < _numsyncs; ++k)
                          _wgroups[i]->barrier[j][k].set(0);
                  }

              }
            Memory::sync();
          }

          ~CollShmDevice()
          {
            if (_tid == 0)
            {
              for (unsigned i = 0;  i < _ntasks; ++i)
              {
                for(unsigned j = 0; j < _numchannels; j++)
                  _wgroups[i]->windows[j].freeBuf(_csmm);
                _wgroups[i]=(collshm_wgroup_t*)_csmm->shm_null_ptr();
                
              }
            }
          }

        
          inline PAMI::Device::Generic::Device *getGenerics()
          {
            return _generics;
          }
          inline GenericDeviceMessageQueue * getQS()
          {
            return this;
          }

          ///
          /// \brief Advance the head index
          ///
          /// \return number of advances made, non-zero means there is
          ///         definitely more available channel/thread in the device
          ///
          inline unsigned _advanceHead()
          {
            // BITWISE OPTIMIZATION (disabled)
            //int      head      = (_head >> _syncbits) & ( _numsyncs - 1);
            //bool     cur_round = head < (_tail >> _syncbits);
            //unsigned round = cur_round ? _round : ((_round + 1) & 0x1);
            int      head      = (_head / _synccounts) % _numsyncs;
            bool     cur_round = (unsigned)head < (_tail / _synccounts);
            unsigned round     = cur_round ? _round : (_round+1)% 2;
            int      increment  = _increments[round];
            unsigned adv   = 0;

            while (_completions[round][head] == _synccounts)
              {
                TRACE_DBG((stderr, "advanceHead() round = %d, head = %d, barrier = %zu, completion = %d wgroups %p\n",
                           round, head, _wgroups[0]->barrier[round][head].fetch(), _completions[round][head], _wgroups[0]));

                if (_wgroups[0]->barrier[round][head].fetch() == ((increment == 1) ? _ntasks : 0))
                  {
                    ++adv;
                    _completions[round][head] = 0;

                    // BITWISE OPTIMIZATION(disabled)
                    // head = (head + 1) & (_numsyncs - 1);
                    head = (head+1) % _numsyncs;

                    if (head == 0)
                      {
                        _increments[round] = - _increments[round]; // flip the increment value
                        round              = (round + 1) & 0x1;
                        increment          = _increments[round];
                        cur_round          = true;
                      }
                  }
                else
                  break;
              }

            _head += (adv * _synccounts);
            TRACE_DBG((stderr,
                       "advance head ..._head = %d, adv = %u, round = %d, head = %d, barrier = %zu, completion = %d\n, wgroups=%p",
                       _head, adv, round, head, _wgroups[0]->barrier[round][head].fetch(), _completions[round][head] ,_wgroups[0]));
            return adv;
          }

          /// \brief Check if there is available Thread in the device
          ///
          inline bool _isThreadAvailable()
          {
            return (_tail < _head || _advanceHead());
          }

          ///
          /// \brief Advance the tail index and return the next available thread
          ///
          /// \return available channel/thread index, -1 if there is no slot available
          ///
          inline int _advanceTail()
          {
            int position = -1;

            TRACE_DBG((stderr, "before advance tail, tail = %d\n", (int)_tail));

            if (_isThreadAvailable())
              {
                position = _tail++;

                if (_tail == _numchannels)
                  {
                    _tail   = 0;
                    _head   -= _numchannels;
                    _round  = (_round + 1) & 0x1;
                  }
              }

            TRACE_DBG((stderr, "after advance tail, tail =%d\n", (int)_tail));
            return position;
          }

          ///
          /// \brief monitors if the circular queue can move ahead
          ///
          inline pami_result_t _advanceQueue(pami_context_t context)
          {
            TRACE_DBG((stderr, "advanceQueue activated\n"));

            if (_isThreadAvailable())
              {
                postNextMsg();
                _threadm.setStatus(PAMI::Device::Idle); // potential race condition here !!!
                return PAMI_SUCCESS;
              }

            return PAMI_EAGAIN;
          }

          ///
          /// \brief Get number of threads/channels of the device
          ///
          /// \return Number of threads/channels in the device
          ///
          inline unsigned getNumThreads() { return _numchannels; }

          /// \brief Acessor for thread objects for this send queue
          ///        Get the next available thread from the device
          ///        The thread represents shared memory channel
          ///
          /// \return Pointer of the next available Thread
          ///         Return Null when no Thread is currently available
          ///
          inline CollShmThread *getAvailThread()
          {
            if (_isThreadAvailable())
              {
                int ind = _advanceTail();
                _threads[ind].resetThread();
                return &_threads[ind];
              }
            else
              return NULL;
          }

          ///
          /// \brief Return the channel to the device
          ///
          /// \param channel_id Index of the channel to turn
          inline void setThreadAvail(int channel_id)
          {
            _threads[channel_id].setStatus(PAMI::Device::Idle);
            // BITWISE OPTIMIZATION(disabled)
            //unsigned idx = channel_id >> _syncbits;
            unsigned idx       = channel_id / _synccounts;
            bool     cur_round = (unsigned)channel_id < _tail;
            unsigned round     = cur_round ? _round : ((_round + 1) & 0x1);

            TRACE_DBG((stderr,
                       "Operation %d completed, thread set to free, completions = %d\n",
                       channel_id, _completions[round][idx]));

            if ((++ _completions[round][idx]) < _synccounts) return;

            int      increment  = _increments[round];
            int      arrived;

            TRACE_DBG((stderr, "Setting Thread Available\n"));

            do
              {
                arrived    = _wgroups[0]->barrier[round][idx].fetch();

                // needs to recheck if it is the last one to take care of race condition
                if (arrived == (increment == 1 ? (int)(_ntasks - 1) : 1))
                  {
                    for (unsigned grp = 0; grp < _ntasks; ++grp)
                      {
                        for (unsigned w = 0; w < _synccounts; ++w)
                          (_wgroups[grp]->windows[idx * _synccounts + w]).clearCtrl();
                      }

                    Memory::sync();
                  }
              } //while(!(COLLSHM_COMPARE_AND_SWAP((atomic_p)&(_wgroups[0]->barrier[round][idx]),&arrived, arrived+increment)))
            while(!_wgroups[0]->barrier[round][idx].bool_compare_and_swap(arrived, arrived + increment)) ;
            TRACE_DBG((stderr, "Setting Thread Available Done\n"));

            Memory::sync();
            _advanceHead();
          }

          ///
          /// \brief Get peer window address
          ///
          /// \param geometry Geometry ID
          /// \param peer     Rank of the peer task
          /// \param channel  Index of the channel interested in
          ///                 For source rank based channel organization
          ///                 this can be the geometry collective
          ///                 sequence number
          /// \return pointer to the corresponding window
          inline CollShmWindow * getWindow (unsigned geometry, size_t peer, unsigned channel)
          {
            // _csmm->ctl_str_check(&(_wgroups[peer]->windows[channel]));
            return &(_wgroups[peer]->windows[channel]);
          }

          inline T_MemoryManager *getSysdep() { return _csmm; }

          /// \brief Post message to device
          ///
          /// \param[in] msg      Message to be posted
          ///
          inline void postMsg(BaseCollShmMessage *msg)
          {
            size_t x = msg->getContextId();
            PAMI::Device::Generic::Device *g = getGenerics();
            TRACE_DBG((stderr, "postMsg %p\n", msg));

            if (_isThreadAvailable())
              {
                CollShmThread *thr = getAvailThread();
                thr->setMsg(msg);
                thr->initThread(msg->getMsgType());
                thr->setStatus(PAMI::Device::Ready);
                msg->setStatus(PAMI::Device::Active);

                pami_result_t rc      = thr->_advanceThread(g[x].getContext());
                int           pollcnt = POLLCOUNT;
                bool          done    = false;
                while((rc == PAMI_EAGAIN) && pollcnt)
                {
                  pollcnt--;
                  rc = thr->_advanceThread(g[x].getContext());
                  if (msg->getStatus() == PAMI::Device::Done)
                    done = true;
                }
                if (done || (msg->getStatus() == PAMI::Device::Done))
                {
                  msg->executeCallback(g[x].getContext());
                  if (rc == PAMI_EAGAIN)
                    g[x].postThread(thr);
                  return;
                }
                
                g[x].postMsg(msg);
                g[x].postThread(thr);
                TRACE_DBG((stderr, "message posted %p\n", this));
              }
            else
              {
                msg->setStatus(PAMI::Device::Initialized);
                if (_threadm.getStatus() == PAMI::Device::Idle)
                  {
                    _threadm.setStatus(PAMI::Device::Ready);
                    g[x].postThread(&_threadm);
                  }

                TRACE_DBG((stderr, "message initalized %p\n",this));
              }

            enqueue(msg);
            return;
          }

          ///
          /// \brief Scan the posted message queue and start as many as possible
          ///
          inline pami_context_t postNextMsg()
          {
            BaseCollShmMessage *msg, *nextmsg;

            for (msg = (BaseCollShmMessage *)peek(); msg; msg = nextmsg)
              {
                TRACE_DBG((stderr, "postNextMsg %p\n", msg));

                nextmsg = (BaseCollShmMessage *)next(msg);

                if (msg->getStatus() != PAMI::Device::Initialized)continue;

                if (_isThreadAvailable())
                  {
                    size_t x = msg->getContextId();
                    PAMI::Device::Generic::Device *g = getGenerics();
                    msg->setStatus(PAMI::Device::Active);

                    CollShmThread *thr = getAvailThread();
                    thr->setMsg(msg);
                    thr->initThread(msg->getMsgType());
                    thr->setStatus(PAMI::Device::Ready);
                    pami_result_t rc      = thr->_advanceThread(g[x].getContext());
                    int           pollcnt = POLLCOUNT;
                    bool          done    = false;
                    while((rc == PAMI_EAGAIN) && pollcnt)
                    {
                      pollcnt--;
                      rc = thr->_advanceThread(g[x].getContext());
                      if (msg->getStatus() == PAMI::Device::Done)
                        done = true;
                    }
                    if(done || (msg->getStatus() == PAMI::Device::Done))
                    {
                      deleteElem(msg);
                      msg->executeCallback(g[x].getContext());
                      if (rc == PAMI_EAGAIN)
                        g[x].postThread(thr);
                      continue;
                    }
                    g[x].postMsg(msg);
                    g[x].postThread(thr);
                  }
                else
                  break;
              }

            return NULL;
          }

          inline size_t getRank() { return _tid; }
          inline size_t getSize() { return _ntasks; }
          inline PAMI::Topology *getTopo() {return _topo;}

        protected:
          PAMI::Topology                *_topo;
          T_MemoryManager               *_csmm;   ///< collective shmem memory manager
          PAMI::Device::Generic::Device *_generics; ///< generic device arrays
          CollShmThread                  _threads[_numchannels];   ///< Threads for active posted messages on device
          PAMI::Device::Generic::GenericAdvanceThread _threadm;
          // this thread is not associated with shared memory channel
          // it is a pure work item making sure enqueued message gets
          // a chance to run

          unsigned     _gid;     // id for the geometry
          unsigned     _ntasks;  // number of tasks sharing the device
          unsigned     _tid;     // task idndex in the device
          unsigned     _syncbits;
          unsigned     _head;    // logical index to the next unavailable channel
          unsigned     _tail;    // index of the oldest in-use channel
          unsigned     _round;   // wrap around count
          unsigned     _completions[2][_numsyncs]; // counters for local completions
          int          _increments [2];             // increment values for updating
          // counters of local completions
          collshm_wgroup_t *_wgroups[PAMI_MAX_PROC_PER_NODE]; // pointer to the shm channels
      }; // class CollShmDevice

//typedef PAMI::Device::Generic::NillSubDevice NillCollShmDevice;
      typedef PAMI::Device::Generic::Device NillCollShmDevice;

      template < class T_CollShmDevice,
               class T_MemoryManager >
      class CollShmModel :
        public PAMI::Device::Interface::MulticastModel < CollShmModel<T_CollShmDevice, T_MemoryManager>,
        NillCollShmDevice,
        sizeof(CollShmMessage<pami_multicast_t, T_CollShmDevice>) > ,
        public PAMI::Device::Interface::MultisyncModel < CollShmModel<T_CollShmDevice, T_MemoryManager>,
        NillCollShmDevice,
        sizeof(CollShmMessage<pami_multisync_t, T_CollShmDevice>) > ,
        public PAMI::Device::Interface::MulticombineModel < CollShmModel<T_CollShmDevice, T_MemoryManager>,
        NillCollShmDevice, sizeof(CollShmMessage<pami_multicombine_t, T_CollShmDevice>) >
      {
        public:
          static const size_t sizeof_msg              = sizeof(CollShmMessage<collshm_multi_t, T_CollShmDevice>);
          static const size_t sizeof_multicast_msg    = sizeof(CollShmMessage<pami_multicast_t, T_CollShmDevice>);
          static const size_t sizeof_multisync_msg    = sizeof(CollShmMessage<pami_multisync_t, T_CollShmDevice>);
          static const size_t sizeof_multicombine_msg = sizeof(CollShmMessage<pami_multicombine_t, T_CollShmDevice>);

          //CollShmModel(PAMI::Device::Generic::Device *device, unsigned commid, PAMI::Topology *topology, T_MemoryManager *csmm) :
          CollShmModel(PAMI::Device::Generic::Device *device, unsigned commid, pami_endpoint_t my_endpoint,
                       PAMI::Topology *topology, T_MemoryManager *csmm, void *ctrlstr) :
            PAMI::Device::Interface::MulticastModel<CollShmModel, NillCollShmDevice, sizeof_msg>(*device, _status),
            PAMI::Device::Interface::MultisyncModel<CollShmModel, NillCollShmDevice, sizeof_msg>(*device, _status),
            PAMI::Device::Interface::MulticombineModel<CollShmModel, NillCollShmDevice, sizeof_msg>(*device, _status),
            _peer(topology->endpoint2Index(my_endpoint)),
            _npeers(topology->size()),
            _csdevice(device, commid, my_endpoint, topology, csmm, ctrlstr)
          {
            TRACE_DBG((stderr, "<%p>CollShmModel()\n", this));
            _csdevice.getWindow(0, 0, 0); // just simple checking
          }

          ~CollShmModel()
            {

            }
          
          void destroy()
            {
              this->~CollShmModel();
            }

          inline pami_result_t postMulticast_impl(uint8_t (&state)[sizeof_msg],
                                                  size_t           client,
                                                  size_t           context,
                                                  pami_multicast_t *mcast,
                                                  void             *devinfo = NULL);
          inline pami_result_t postMultisync_impl(uint8_t (&state)[sizeof_msg],
                                                  size_t           client,
                                                  size_t           context,
                                                  pami_multisync_t *msync,
                                                  void             *devinfo = NULL);
          inline pami_result_t postMulticombine_impl(uint8_t (&state)[sizeof_msg],
                                                     size_t           client,
                                                     size_t           context,
                                                     pami_multicombine_t *mcombine,
                                                     void             *devinfo = NULL);
        private:
          unsigned        _peer;
          unsigned        _npeers;
          pami_result_t   _status;
          T_CollShmDevice _csdevice;

      }; // class CollShmModel

      template <class T_CollShmDevice, class T_MemoryManager>
      inline pami_result_t CollShmModel<T_CollShmDevice, T_MemoryManager> ::postMulticast_impl(uint8_t (&state)[sizeof_msg],
          size_t           client,
          size_t           context,
          pami_multicast_t *mcast,
          void             *devinfo)
      {
        CollShmMessage<pami_multicast_t, T_CollShmDevice> *msg =
          new (&state) CollShmMessage<pami_multicast_t, T_CollShmDevice> (&_csdevice, client, context, mcast);
        _csdevice.postMsg(msg);
        return PAMI_SUCCESS;
      }

      template <class T_CollShmDevice, class T_MemoryManager>
      inline pami_result_t CollShmModel<T_CollShmDevice, T_MemoryManager>::postMultisync_impl(uint8_t (&state)[sizeof_msg],
          size_t           client,
          size_t           context,
          pami_multisync_t *msync,
          void             *devinfo)
      {
        CollShmMessage<pami_multisync_t, T_CollShmDevice> *msg =
          new (&state) CollShmMessage<pami_multisync_t, T_CollShmDevice> (&_csdevice, client, context, msync);
        _csdevice.postMsg(msg);
        return PAMI_SUCCESS;
      }

      template <class T_CollShmDevice, class T_MemoryManager>
      inline pami_result_t CollShmModel<T_CollShmDevice, T_MemoryManager>::postMulticombine_impl(uint8_t (&state)[sizeof_msg],
          size_t           client,
          size_t           context,
          pami_multicombine_t *mcombine,
          void             *devinfo)
      {
        TRACE_DBG((stderr, "POSTING MULTICOMBINE\n"));
        CollShmMessage<pami_multicombine_t, T_CollShmDevice> *msg =
          new (&state) CollShmMessage<pami_multicombine_t, T_CollShmDevice> (&_csdevice, client, context, mcombine);
        _csdevice.postMsg(msg);
        return PAMI_SUCCESS;
      }
    }; // namespace CollShm
  }; // namespace Device
}; // namespace PAMI

#undef PAMI_ASSERT
#undef TRACE_ERR
#undef TRACE_DBG

#endif // __pami_components_devices_cshmem_CollShmDevice_h__
