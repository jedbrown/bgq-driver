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
/// \file p2p/protocols/send/datagram/DTimer.h
/// \brief Datagram timer
///
/// The DTimer class defined in this file defines a timer which works
/// with the Generic device progress function
///
#ifndef __p2p_protocols_send_datagram_DTimer_h__
#define __p2p_protocols_send_datagram_DTimer_h__

#include "components/devices/misc/ProgressFunctionMsg.h"

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI {
namespace Protocol {
namespace Send {
class DTimer {
public:
        DTimer() :
                _start_time(0), _interval(0), _timeout_cb(NULL), _timeout_cookie(NULL),
                                _max_timeouts(0), _max_cb(NULL), _max_cookie(NULL),
                                _running(false), _closed(false) {
        }

        pami_result_t start(PAMI::Device::ProgressFunctionMdl progfmodel,
                        unsigned long long interval, pami_event_function timeout_cb, void * timeout_cookie,
                        unsigned long max_timeouts, pami_event_function max_cb, void * max_cookie) {
                TRACE_ERR((stderr,"DTimer(%p) start() ... Starting timer with interval = %llu \n", this, interval));
                PAMI_ProgressFunc_t progf;

                _start_time = 0;
                _interval = interval;
                _timeout_cb = timeout_cb;
                _timeout_cookie = timeout_cookie;
                _max_timeouts = max_timeouts;
                _max_timeouts_left = max_timeouts;
                _max_cb = max_cb;
                _max_cookie = max_cookie;
                _running = true;
                _closed = false;

                progf.request = &pmsgbuf;
                progf.clientdata = this;
#warning fix this progress fnction stuff
                //progf.client = clientid;
                //progf.context = contextid;
                progf.func = advance_timer_cb;  // pami_work_function
                progf.cb_done = (pami_callback_t) {NULL};
                bool rc = progfmodel.postWork(&progf);
                if (!rc) {
                        TRACE_ERR((stderr,"DTimer(%p) advance_timer() ... Failed to generateMessage on progress function\n",this));
                        return PAMI_ERROR;
                }
                return PAMI_SUCCESS;
        }

        static pami_result_t advance_timer_cb(pami_context_t context, void * me) {
                DTimer *dt = (DTimer *)me;
                return dt->advance_timer(context);
        }

        pami_result_t advance_timer(pami_context_t context) {
                if (_running) {
                        // If initial time not set, set it
                        if (_start_time == 0) {
                                _start_time = (unsigned long long)__global.time.time();TRACE_ERR((stderr,
                                                                "	DTimer(%p) advance_timer().. Starting Timer at tick %llu, waiting until %llu\n",
                                                                this, _start_time, _start_time + _interval));
                        }

                        // See if timed out
                        unsigned long long t1 = (unsigned long long)__global.time.time();
                        if (t1 - _start_time >= _interval) {
                                // Timed out
                                TRACE_ERR((stderr,
                                                                "	DTimer(%p) advance_timer().. Timeout at tick %llu (%ld calls)\n",
                                                                this, t1, _max_timeouts));
                                _start_time = 0; //reset timer
                                --_max_timeouts; //decrement number of timeouts left
                                if (_max_timeouts == 0) {
                                        // out of timeouts
                                        TRACE_ERR((stderr, "   DTimer(%p) advance_timer() ...  Maxed Out\n", this));
                                        if (_max_cb) {
                                                _max_cb(context, _max_cookie, PAMI_ERROR);
                                        }
                                        return PAMI_ERROR;
                                } else {
                                        // normal timeout processing
                                        TRACE_ERR((stderr, "   DTimer(%p) advance_timer() ...  Normal timeout processing\n", this));
                                        if (_timeout_cb) {
                                                _timeout_cb(context, _timeout_cookie, PAMI_EAGAIN);
                                        }
                                }
                        }
                        return PAMI_EAGAIN;
                }
                if (_closed) return PAMI_SUCCESS;  // Remove from advance
                return PAMI_EAGAIN; // Keep advancing, but don't do anything
        }

        void reset() {
                if (_closed) abort(); // can't reset
                _start_time = 0;
                _max_timeouts_left = _max_timeouts;
                _running = true;
        }

        void stop() {
                _running = false;
        }

        void close() {
                _closed = true;
        }

protected:
private:
        char pmsgbuf[PAMI::Device::ProgressFunctionMdl::sizeof_msg];

        unsigned long long _start_time; ///starting time
        unsigned long long _interval; ///define interval
        pami_event_function _timeout_cb;
        void * _timeout_cookie;
        unsigned long _max_timeouts; ///time out condition
        unsigned long _max_timeouts_left;
        pami_event_function _max_cb;
        void * _max_cookie;
        bool _running; ///running flag
        bool _closed;  ///closed flag


}; // PAMI::Protocol::Send::DTimer class
}
; // PAMI::Protocol::Send namespace
}
; // PAMI::Protocol namespace
}
; // PAMI namespace

#undef TRACE_ERR
#endif // __pami_p2p_protocol_send_datagram_DTimer_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
