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
 * \file components/devices/bgp/collective_network/CollectiveNetworkLib.h
 * \brief ???
 */
#ifndef __components_devices_bgp_collective_network_CollectiveNetworkLib_h__
#define __components_devices_bgp_collective_network_CollectiveNetworkLib_h__

#include "PipeWorkQueue.h"
#include "components/devices/workqueue/WorkQueue.h"
#include "math/bgp/collective_network/DblUtils.h"
#include "components/devices/bgp/collective_network/CNAllreduce.h"
#include "components/devices/util/SubDeviceSuppt.h"

// collective network hardware...
#include "spi/bgp_SPI.h"
#define VIRTUAL_CHANNEL 0
#define BGPCN_PKT_SIZE   256
#define BGPCN_FIFO_SIZE  8
#define BGPCN_QUADS_PER_PKT      (BGPCN_PKT_SIZE / sizeof(pami_quad_t))
#define BGPCN_QUADS_PER_FIFO     (BGPCN_FIFO_SIZE * BGPCN_QUADS_PER_PKT)

#ifndef __defined__tsc__
static inline unsigned long __tsc() {
        long tsc;
        asm volatile ("mfspr %0,%1" : "=r" (tsc) : "i" (SPRN_TBRL));
        return tsc;
}
#define __defined__tsc__
#endif /* ! __defined__tsc__ */

#define EXPCOUNT        8 // standard cycle rate for 2-pass double-sum
#define SHORTS_PER_PKT	(BGPCN_PKT_SIZE / sizeof(unsigned short))
#define MANT_PER_PKT	(BGPCN_PKT_SIZE / (3 * sizeof(unsigned)))
#define EXPO_PER_PKT	((BGPCN_PKT_SIZE / MANT_PER_PKT / sizeof(unsigned short)) * MANT_PER_PKT)
#define EXPO_MANT_FACTOR	(EXPO_PER_PKT / MANT_PER_PKT)

#define EXPO_WQ_SIZE	EXPCOUNT	// must be power of 2 and >= EXPCOUNT.
#define MANT_WQ_FACT	(8)		// must be power of 2 and
                                        //		>= EXPO_MANT_FACTOR.

namespace PAMI {
namespace Device {
namespace BGP {

class BaseGenericCNThread : public PAMI::Device::Generic::SimpleAdvanceThread {
public:
        BaseGenericCNThread() :
        PAMI::Device::Generic::SimpleAdvanceThread(),
        _wq(NULL),
        _cycles(0)
        {
        }
public:
        PAMI::PipeWorkQueue *_wq;
        unsigned _cycles;
}; // class BaseGenericCNThread

class BaseGenericCNMessage : public PAMI::Device::Generic::GenericMessage {
public:
        // on BG/P we only use one classroute, the global one.
        static const unsigned classroute = 3;

        BaseGenericCNMessage(GenericDeviceMessageQueue *qs,
                        size_t client,
                        size_t context,
                        PAMI::PipeWorkQueue *swq,
                        PAMI::PipeWorkQueue *rwq,
                        size_t bytes,
                        bool doStore,
                        unsigned roles,
                        pami_callback_t cb,
                        unsigned disp_id,
                        unsigned hhfunc,
                        unsigned opsize) :
        PAMI::Device::Generic::GenericMessage(qs, cb, client, context),
        _swq(swq),
        _rwq(rwq),
        _bytes(bytes),
        _roles(roles),
        _doStore(doStore),
        _modelPkt(classroute, hhfunc, (opsize >> 1) - 1, disp_id)
        {
        }

        /**
         * \brief Waits until at least one packet is ready to be processed,
         * but only if less than "cycles" time elapses.
         *
         * Ensure that at least one check is made, even if "cycles"
         * is very small (or zero).
         *
         * \param[in] thr		Context of send
         * \param[in,out] hcount	The number of headers available in FIFO
         * \param[in,out] dcount	The number of data quads in FIFO
         * \param[in] cycles		Timeout in clock cycles
         *
         * \return True if timed out (FIFO not ready)
         */
        inline bool __wait_send_fifo_to(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, unsigned cycles) {
                register unsigned rechcount, recdcount,
                        injhcount = hcount, injdcount = dcount;
                unsigned t0 = __tsc();
                while (1) {
                        CollectiveFifoStatus(VIRTUAL_CHANNEL,
                                &rechcount, &recdcount,
                                &injhcount, &injdcount);
                        if (injhcount < BGPCN_FIFO_SIZE && injdcount < BGPCN_QUADS_PER_FIFO) {
                                break;
                        }
                        if (__tsc() - t0 > cycles) {
                                return true;
                        }
                }
                hcount = injhcount;
                dcount = injdcount;
                return false;
        }

        inline void __send_null_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount) {
                while (hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO &&
                                thr->_bytesLeft > 0) {
                        CollectiveRawSendPacket0(VIRTUAL_CHANNEL, &_modelPkt._hh);
                        if (thr->_bytesLeft > BGPCN_PKT_SIZE) {
                                thr->_bytesLeft -= BGPCN_PKT_SIZE;
                        } else {
                                thr->_bytesLeft =0;
                        }
                        ++hcount;
                        dcount += BGPCN_QUADS_PER_PKT;
                }
        }

        inline void __send_whole_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *&buf, bool aligned) {
                size_t left = thr->_bytesLeft;
                while (hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO &&
                                left >= BGPCN_PKT_SIZE &&
                                avail >= BGPCN_PKT_SIZE) {
                        // 'buf' is always updated by BGPCN_PKT_SIZE (except last pkt)
                        // and so will stay aligned if it was originally aligned.
                        if (aligned) {
                                CollectiveRawSendPacket(VIRTUAL_CHANNEL, &_modelPkt._hh, buf);
                        } else {
                                CollectiveRawSendPacketUnaligned(VIRTUAL_CHANNEL, &_modelPkt._hh, buf, BGPCN_PKT_SIZE);
                        }
                        buf += BGPCN_PKT_SIZE;
                        did += BGPCN_PKT_SIZE;
                        avail -= BGPCN_PKT_SIZE;
                        left -= BGPCN_PKT_SIZE;
                        ++hcount;
                        dcount += BGPCN_QUADS_PER_PKT;
                }
                thr->_bytesLeft = left;
        }

        inline void __send_last_packet(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *buf, bool aligned) {
                size_t left = thr->_bytesLeft;
                if (hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO &&
                                left > 0 &&
                                left < BGPCN_PKT_SIZE && avail >= left) {
                        // last packet...
                        CollectiveRawSendPacketUnaligned(VIRTUAL_CHANNEL, &_modelPkt._hh, buf, left);
                        //buf += left;
                        did += left;
                        //avail -= left;
                        //thr->_bytesLeft -= left;
                        thr->_bytesLeft = 0;
                        //++hcount;
                        //dcount += BGPCN_QUADS_PER_PKT;
                }
        }

        /**
         * \brief Waits until at least one packet is ready to be processed,
         * but only if less than "cycles" time elapses.
         *
         * Assumes 'cycles' is larger than one pass through loop.
         *
         * \param[in] thr		Context of send
         * \param[in,out] hcount	The number of headers available in FIFO
         * \param[in,out] dcount	The number of data quads in FIFO
         * \param[in] cycles		Timeout in clock cycles
         *
         * \return True if timed out (FIFO not ready)
         */
        inline bool __wait_recv_fifo_to(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, unsigned cycles) {
                register unsigned rechcount = hcount, recdcount = dcount,
                                injhcount, injdcount;
                unsigned t0 = __tsc();
                while (rechcount == 0 || recdcount == 0) {
                        if (__tsc() - t0 > cycles) {
                                return true;
                        }
                        CollectiveFifoStatus(VIRTUAL_CHANNEL,
                                &rechcount, &recdcount,
                                &injhcount, &injdcount);
                }
                hcount = rechcount;
                dcount = recdcount;
                return false;
        }

        /**
         * \brief Receive as many whole packets as available, but don't store
         *
         * \param[in] thr		Context of send
         * \param[in,out] hcount	The number of headers available in FIFO
         * \param[in,out] dcount	The number of data quads in FIFO
         * \param[in,out] did		Number of bytes actually received
         */
        inline void __recv_null_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount,
                        size_t &did) {
                CollHwHdr hdr;
                size_t left = thr->_bytesLeft;
                while (hcount > 0 && dcount > 0 && left >= 0) {
                        CollectiveRawReceiveHeader(VIRTUAL_CHANNEL, &hdr);
                        --hcount;
                        CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
                        dcount -= BGPCN_QUADS_PER_PKT;
                        if (left < BGPCN_PKT_SIZE) {
                                did += left;
                                left = 0;
                        } else {
                                left -= BGPCN_PKT_SIZE;
                                did += BGPCN_PKT_SIZE;
                        }
                }
                thr->_bytesLeft = left;
        }

        /**
         * \brief Receive as many whole packets as available
         *
         * \param[in] thr		Context of send
         * \param[in,out] hcount	The number of headers available in FIFO
         * \param[in,out] dcount	The number of data quads in FIFO
         * \param[in,out] avail		Number of bytes available in output buf
         * \param[in,out] did		Number of bytes actually received
         * \param[in] buf		Where to store received bytes
         * \param[in] aligned		Is 'buf' aligned for network?
         */
        inline void __recv_whole_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *&buf, bool aligned) {
                CollHwHdr hdr;
                size_t left = thr->_bytesLeft;
                while (hcount > 0 && dcount > 0 &&
                                left >= BGPCN_PKT_SIZE &&
                                avail >= BGPCN_PKT_SIZE) {
                        CollectiveRawReceiveHeader(VIRTUAL_CHANNEL, &hdr);
                        --hcount;
                        if (_doStore) {
                                if (aligned) {
                                        CollectiveRawReceivePacketNoHdr(VIRTUAL_CHANNEL,
                                                (void *)buf);
                                } else {
                                        CollectiveRawReceivePayloadUnaligned(VIRTUAL_CHANNEL,
                                                (void *)buf, BGPCN_PKT_SIZE);
                                }
                        } else {
                                CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
                        }
                        dcount -= BGPCN_QUADS_PER_PKT;
                        left -= BGPCN_PKT_SIZE;
                        avail -= BGPCN_PKT_SIZE;
                        did += BGPCN_PKT_SIZE;
                        buf += BGPCN_PKT_SIZE;
                }
                thr->_bytesLeft = left;
        }

        /**
         * \brief Receive last, partial, packet
         *
         * Only does work if last packet was not full
         *
         * \param[in] thr		Context of send
         * \param[in,out] hcount	The number of headers available in FIFO
         * \param[in,out] dcount	The number of data quads in FIFO
         * \param[in,out] avail		Number of bytes available in output buf
         * \param[in,out] did		Number of bytes actually received
         * \param[in] buf		Where to store received bytes
         * \param[in] aligned		Is 'buf' aligned for network?
         */
        inline void __recv_last_packet(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *buf, bool aligned) {
                size_t left = thr->_bytesLeft;
                CollHwHdr hdr;
                if (hcount > 0 && dcount > 0 && left > 0 &&
                                left < BGPCN_PKT_SIZE &&
                                avail >= left) {
                        CollectiveRawReceiveHeader(VIRTUAL_CHANNEL, &hdr);
                        --hcount;
                        if (_doStore) {
                                // since we have a partial packet, we can't use
                                // aligned routines anyway, don't care about 'aligned'.
                                CollectiveRawReceivePayloadUnaligned(VIRTUAL_CHANNEL,
                                                (void *)buf, left);
                        } else {
                                CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
                        }
                        //dcount -= BGPCN_QUADS_PER_PKT;
                        //avail = 0;
                        did += left;
                        //buf += left;
                        thr->_bytesLeft = 0;
                }
        }
protected:
        PAMI::PipeWorkQueue *_swq;
        PAMI::PipeWorkQueue *_rwq;
        size_t _bytes;
        unsigned _roles;
        bool _doStore;
        PAMI::Device::BGP::CNPacket _modelPkt;
}; // class BaseGenericCNMessage

/// Collective Network Message with Pre/Post-processing of packets
class BaseGenericCNPPMessage : public BaseGenericCNMessage {
public:
        BaseGenericCNPPMessage(GenericDeviceMessageQueue *qs,
                        size_t client,
                        size_t context,
                        PAMI::PipeWorkQueue *swq,
                        PAMI::PipeWorkQueue *rwq,
                        size_t bytes,
                        bool doStore,
                        unsigned roles,
                        pami_callback_t cb,
                        unsigned disp_id,
                        PAMI::Device::BGP::CNAllreduceSetup &tas) :
        BaseGenericCNMessage(qs, client, context, swq, rwq, bytes, doStore, roles, cb, disp_id,
                        tas._hhfunc, tas._opsize),
        _allreduceSetup(tas)
        {
        }

        inline void __send_whole_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *&buf, bool aligned) {
                static char tmp[BGPCN_PKT_SIZE]__attribute__((__aligned__(16)));
                size_t inBytes = BGPCN_PKT_SIZE >> _allreduceSetup._logbytemult;
                // Since we always inject from 'tmp', it will always be aligned.
                while (hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO &&
                                thr->_bytesLeft >= BGPCN_PKT_SIZE && avail >= inBytes) {
                        _allreduceSetup._pre(tmp, buf, BGPCN_PKT_SIZE >> _allreduceSetup._logopsize);
                        CollectiveRawSendPacket(VIRTUAL_CHANNEL, &_modelPkt._hh, tmp);
                        buf += inBytes;
                        did += inBytes;
                        avail -= inBytes;
                        thr->_bytesLeft -= BGPCN_PKT_SIZE;
                        ++hcount;
                        dcount += BGPCN_QUADS_PER_PKT;
                }
        }

        inline void __send_last_packet(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *buf, bool aligned) {
                static char tmp[BGPCN_PKT_SIZE]__attribute__((__aligned__(16)));
                size_t left = thr->_bytesLeft;
                size_t inBytes = left >> _allreduceSetup._logbytemult;
                if (hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO &&
                                left > 0 && left < BGPCN_PKT_SIZE && avail >= inBytes) {
                        // last packet...
                        _allreduceSetup._pre(tmp, buf, left >> _allreduceSetup._logopsize);
                        CollectiveRawSendPacket(VIRTUAL_CHANNEL, &_modelPkt._hh, tmp);
                        //buf += inBytes;
                        did += inBytes;
                        //avail -= inBytes;
                        //thr->_bytesLeft -= left;
                        thr->_bytesLeft = 0;
                        //++hcount;
                        //dcount += BGPCN_QUADS_PER_PKT;
                }
        }

        inline void __recv_whole_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *&buf, bool aligned) {
                static char tmp[BGPCN_PKT_SIZE]__attribute__((__aligned__(16)));
                CollHwHdr hdr;
                size_t outBytes = BGPCN_PKT_SIZE >> _allreduceSetup._logbytemult;
                while (hcount > 0 && dcount > 0 &&
                                thr->_bytesLeft >= BGPCN_PKT_SIZE &&
                                avail >= outBytes) {
                        CollectiveRawReceiveHeader(VIRTUAL_CHANNEL, &hdr);
                        --hcount;
                        if (_doStore) {
                                // actual network buffer is 'tmp' which is always aligned.
                                // don't care if user buffer is aligned.
                                CollectiveRawReceivePacketNoHdr(VIRTUAL_CHANNEL, (void *)tmp);
                                _allreduceSetup._post((void *)buf, (void *)tmp,
                                        BGPCN_PKT_SIZE >> _allreduceSetup._logopsize);
                        } else {
                                CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
                        }
                        dcount -= BGPCN_QUADS_PER_PKT;
                        thr->_bytesLeft -= BGPCN_PKT_SIZE;
                        avail -= outBytes;
                        did += outBytes;
                        buf += outBytes;
                }
        }

        inline void __recv_last_packet(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount, size_t &avail,
                        size_t &did, char *buf, bool aligned) {
                static char tmp[BGPCN_PKT_SIZE]__attribute__((__aligned__(16)));
                size_t left = thr->_bytesLeft;
                CollHwHdr hdr;
                size_t outBytes = left >> _allreduceSetup._logbytemult;
                if (hcount > 0 && dcount > 0 && left > 0 &&
                                left < BGPCN_PKT_SIZE &&
                                avail >= outBytes) {
                        CollectiveRawReceiveHeader(VIRTUAL_CHANNEL, &hdr);
                        --hcount;
                        if (_doStore) {
                                // actual network buffer is 'tmp' which is always aligned.
                                // don't care if user buffer is aligned.
                                // Also, network uses only full packets, so get whole
                                // packet into 'tmp' then only use part of it.
                                CollectiveRawReceivePacketNoHdr(VIRTUAL_CHANNEL,
                                                (void *)tmp);
                                _allreduceSetup._post((void *)buf, (void *)tmp,
                                        left >> _allreduceSetup._logopsize);
                        } else {
                                CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
                        }
                        //dcount -= BGPCN_QUADS_PER_PKT;
                        //avail = 0;
                        did += outBytes;
                        //buf += outBytes;
                        thr->_bytesLeft = 0;
                }
        }
protected:
        PAMI::Device::BGP::CNAllreduceSetup &_allreduceSetup;
}; // class BaseGenericCNPPMessage

class BaseGenericCN2PMessage : public BaseGenericCNMessage {
public:
        BaseGenericCN2PMessage(GenericDeviceMessageQueue *qs,
                        size_t client,
                        size_t context,
                        PAMI::Device::WorkQueue::WorkQueue &ewq,
                        PAMI::Device::WorkQueue::WorkQueue &mwq,
                        PAMI::Device::WorkQueue::WorkQueue &xwq,
                        PAMI::PipeWorkQueue *swq,
                        PAMI::PipeWorkQueue *rwq,
                        size_t bytes,
                        bool doStore,
                        unsigned roles,
                        const pami_callback_t cb,
                        unsigned dispatch_id_e,
                        unsigned dispatch_id_m) :
        BaseGenericCNMessage(qs, client, context, swq, rwq, bytes, doStore, roles, cb, 0, 0, 0),
        _expcount(bytes>>3),
        _mancount(bytes>>3),
        _expcycle(EXPCOUNT), /// \todo Set according to collective network depth?
        _expnum(0),
        _expsent(0),
        _mansent(0),
        _exprecv(0),
        _manrecv(0),
        _expo_disp_id(dispatch_id_e),
        _mant_disp_id(dispatch_id_m),
        _ehdr(classroute,PAMI::Device::BGP::COMBINE_OP_MAX,0,dispatch_id_e), // 16-bit exponent
        _mhdr(classroute,PAMI::Device::BGP::COMBINE_OP_ADD,5,dispatch_id_m), // 96 bit mantissa
        _ewq(ewq),
        _mwq(mwq),
        _xwq(xwq)
        {
        }

        // _ewq - single producer/consumer, local only (for now).
        // _mwq - single producer/consumer, local only (for now).
        // _xwq - one producer/two consumer, shared memory
        // [xxxx] - data on collective network
        // (xx) - data/results user buffers
        //
        // Injection:
        // (in) --> _pami_core_fp64_pre1_2pass --+--> _ewq ---> [expo]
        //                                  +--> _mwq
        //
        // _ewq --+
        // _mwq --+--> _pami_core_fp64_pre2_2pass ---> [mant]
        // _xwq --+
        //
        // Reception:
        // [expo] --> _xwq
        //
        // [mant] --+--> _pami_core_fp64_post_2pass --> (out)
        // _xwq ----+
        //
        //
        //

        inline void __send_expo_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount,
                        unsigned &expRemain, size_t avail,
                        size_t &did, char *buf) {
                unsigned exptoSend = MIN(EXPO_PER_PKT, expRemain);
                unsigned srcBytes = exptoSend * sizeof(double);
                while (expRemain > 0 && _expnum < _expcycle &&
                                avail >= srcBytes &&
                                hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO) {
                        uint16_t *ebuf = (uint16_t *)_ewq.bufferToProduce(0);
                        uint32_t *mbuf = (uint32_t *)_mwq.bufferToProduce(0);
                        _pami_core_fp64_pre1_2pass(ebuf, mbuf, (double *)(buf + did), exptoSend);
//fprintf(stderr, "_pami_core_fp64_pre1_2pass {%p} %08x%08x%08x %04x %f (%d)\n", mbuf, mbuf[1], mbuf[2], mbuf[3], ebuf[0], *((double *)(buf + did)), exptoSend);
                        _ewq.produceBytes(BGPCN_PKT_SIZE, 0);
                        // jump to boundary
                        _mwq.produceBytes(MANT_WQ_FACT * BGPCN_PKT_SIZE, 0);
                        // we know we produced exactly one packet of exponents...
                            CollectiveRawSendPacket(VIRTUAL_CHANNEL, &_ehdr._hh, ebuf);
                        ++hcount;
                        dcount += BGPCN_QUADS_PER_PKT;
                            _expsent += exptoSend;
                            expRemain -= exptoSend;
                        did += srcBytes;
                        avail -= srcBytes;
                            _expnum++;
                        exptoSend = MIN(EXPO_PER_PKT, expRemain);
                        srcBytes = exptoSend * sizeof(double);
                }
        }

        inline void __send_mant_packets(BaseGenericCNThread *thr,
                        unsigned &hcount, unsigned &dcount,
                        unsigned &manRemain,
                        size_t &didm) {
                // _ewq and _mwq must have all that we need... otherwise _xwq would not.
                // also, _ewq and _xwq are in-sync, both reach end of packet at same time.
                uint16_t *ebuf = (uint16_t *)_ewq.bufferToConsume(0);
                uint16_t *xbuf = (uint16_t *)_xwq.bufferToConsume(0);
                uint32_t *mbuf = (uint32_t *)_mwq.bufferToConsume(0);
                size_t avail = _xwq.bytesAvailableToConsume(0);
                unsigned ipktx = (_xwq.getBytesConsumed(0) % BGPCN_PKT_SIZE);
                unsigned pktx = ipktx;
                unsigned ipktm = (_mwq.getBytesConsumed(0) % (MANT_WQ_FACT * BGPCN_PKT_SIZE));
                unsigned pktm = ipktm;
                unsigned mantoSend = MIN(MANT_PER_PKT, manRemain);
                unsigned expBytes = mantoSend * sizeof(uint16_t);
                unsigned srcBytes = mantoSend * sizeof(double);
                size_t did = 0;
                while (manRemain > 0 && avail >= expBytes &&
                                hcount < BGPCN_FIFO_SIZE && dcount < BGPCN_QUADS_PER_FIFO) {
//fprintf(stderr, "_pami_core_fp64_pre2_2pass PRE  {%p} %08x%08x%08x %04x {%p} %04x (%d)\n", mbuf, mbuf[1], mbuf[2], mbuf[3], ebuf[0], xbuf, xbuf[0], mantoSend);
                        _pami_core_fp64_pre2_2pass(mbuf, ebuf, xbuf, mantoSend);
//fprintf(stderr, "_pami_core_fp64_pre2_2pass POST {%p} %08x%08x%08x\n", mbuf, mbuf[1], mbuf[2], mbuf[3]);
                        CollectiveRawSendPacket(VIRTUAL_CHANNEL, &_mhdr._hh, mbuf);
                        ++hcount;
                        dcount += BGPCN_QUADS_PER_PKT;
                        _mansent += mantoSend;
                        manRemain -= mantoSend;
                        did += srcBytes;
                        // These adds will not cross a packet boundary...
                        // but might reach end of packet.
                        pktx += expBytes;
                        if (_mansent == _expsent) {
                                didm += did;
                                _expnum = 0;
                                // either end of message, or end of cycle.
                                // both force end of exponent packets
                                // Exponent WQs will never be exactly at the boundary,
                                // but mantissa WQ might. Must ensure we don't skip
                                // too far.
                                unsigned res = BGPCN_PKT_SIZE - ipktx;
                                _ewq.consumeBytes(res, 0);
                                _xwq.consumeBytes(res, 0);
                                res = (MANT_WQ_FACT * BGPCN_PKT_SIZE) - ipktm;
                                _mwq.consumeBytes(res, 0);
                                // nothing else to do until more exponents are sent.
                                return;
                        }
                        if (pktx == (EXPO_PER_PKT * sizeof(uint16_t))) {
                                // end of expo packet(s) reached
                                unsigned res = BGPCN_PKT_SIZE - ipktx;
                                _ewq.consumeBytes(res, 0);
                                _xwq.consumeBytes(res, 0);
                                res = (MANT_WQ_FACT * BGPCN_PKT_SIZE) - ipktm;
                                _mwq.consumeBytes(res, 0);
                                ipktx = pktx = 0;
                                ipktm = pktm = 0;
                                // buffers might wrap, so must get fresh values
                                ebuf = (uint16_t *)_ewq.bufferToConsume(0);
                                xbuf = (uint16_t *)_xwq.bufferToConsume(0);
                                mbuf = (uint32_t *)_mwq.bufferToConsume(0);
                                avail = _xwq.bytesAvailableToConsume(0);
                        } else {
                                mbuf = (uint32_t *)((char *)mbuf + BGPCN_PKT_SIZE);
                                pktm += BGPCN_PKT_SIZE;
                                avail -= expBytes;
                        }
                        if (manRemain && manRemain < MANT_PER_PKT) {
                                mantoSend = manRemain;
                                expBytes = mantoSend * sizeof(uint16_t);
                                srcBytes = mantoSend * sizeof(double);
                        }
                }
                if (did) {
                        // partially complete... must update WQs for next time...
                        unsigned res = (pktx - ipktx);
                        _ewq.consumeBytes(res, 0);
                        _xwq.consumeBytes(res, 0);
                        res = (pktm - ipktm);
                        _mwq.consumeBytes(res, 0);
                        didm += did;
                }
        }

        /**
         * \brief Processes exponent packets off the network.
         *
         * The "exptoRecv" param is ignored, since there is always
         * a whole packet in the FIFO and we want to keep the WQ
         * packet-aligned, so we never deal in partial packets at
         * this level. The consumers of exponents will never take
         * more than they need.
         *
         * \param[in] exptoRecv Number of exponents to receive
         */
        inline void __recv_expo_packet(BaseGenericCNThread *thr) {
                // assert _xwq.bytesAvailableToProduce(0) >= BGPCN_PKT_SIZE
                CollectiveRawReceivePacketNoHdr(VIRTUAL_CHANNEL, _xwq.bufferToProduce(0));
                _xwq.produceBytes(BGPCN_PKT_SIZE, 0);
        }

        /**
         * Processes mantissa packets off the network.
         *
         */
        inline void __recv_mant_packet(BaseGenericCNThread *thr,
                        size_t avail, size_t &did, char *buf, size_t &dstBytes) {
                static char tmp[BGPCN_PKT_SIZE]__attribute__((aligned(16)));
                uint16_t *ebuf = (uint16_t *)_xwq.bufferToConsume(1);
                unsigned pktx = (_xwq.getBytesConsumed(1) % BGPCN_PKT_SIZE);
                unsigned ipktx = pktx;
                size_t manRemain = _mancount - _manrecv;
                unsigned manToRecv = MIN(MANT_PER_PKT, manRemain);
                dstBytes = manToRecv * sizeof(double);
                unsigned edid = 0;
                // manRemain is always > 0 ?
                if (avail >= dstBytes) {
                        CollectiveRawReceivePacketNoHdr(VIRTUAL_CHANNEL, tmp);
                        uint32_t *mbuf = (uint32_t *)tmp;
                        if (_doStore) {
                                _pami_core_fp64_post_2pass((double *)buf, ebuf, mbuf, manToRecv);
//fprintf(stderr, "_pami_core_fp64_post_2pass %f %08x%08x%08x {%p} %04x (%d)\n", *((double *)buf), mbuf[1], mbuf[2], mbuf[3], ebuf, ebuf[0], manToRecv);
                        }
                        _manrecv += manToRecv;
                        manRemain -= manToRecv;
                        did += dstBytes;
                        if (manRemain == 0) {
                                // jump to next packet boundary
                                unsigned res = BGPCN_PKT_SIZE - ipktx;
                                _xwq.consumeBytes(res, 1);
                                return;
                        }
                        avail -= dstBytes;
                        unsigned expBytes = manToRecv * sizeof(uint16_t);
                        edid += expBytes;
                        pktx += expBytes;
                        if (pktx == (EXPO_PER_PKT * sizeof(uint16_t))) {
                                // jump to next packet boundary
                                unsigned res = BGPCN_PKT_SIZE - ipktx;
                                _xwq.consumeBytes(res, 1);
                                // might wrap...
                                ebuf = (uint16_t *)_xwq.bufferToConsume(1);
                                pktx = 0;
                                edid = 0;
                        }
                }
                if (edid) {
                        _xwq.consumeBytes(edid, 1);
                }
        }

protected:
        unsigned _expcount;	/**< number exponents in message */
        unsigned _mancount;	/**< number mantissas in message */

        unsigned _expcycle;	/**< number expo pkts @ switch to mantissas */
        unsigned _expnum;	/**< number of expo total */
        unsigned _expsent;	/**< number of expo sent in current cycle */
        unsigned _mansent;	/**< number mant sent */

        unsigned _exprecv;	/**< number exponents recv'd */
        unsigned _manrecv;	/**< number mantissas recv'd */

        unsigned _expo_disp_id;
        unsigned _mant_disp_id;

        PAMI::Device::BGP::CNPacket _ehdr;	/**< exponent network header model */
        PAMI::Device::BGP::CNPacket _mhdr;	/**< mantissa network header model */

        PAMI::Device::WorkQueue::WorkQueue &_ewq;
        PAMI::Device::WorkQueue::WorkQueue &_mwq;
        PAMI::Device::WorkQueue::WorkQueue &_xwq;
}; // class BaseGenericCN2PMessage

};	// namespace BGP
};	// namespace Device
};	// namespace PAMI


#endif // __components_devices_bgp_cnlib_h__
