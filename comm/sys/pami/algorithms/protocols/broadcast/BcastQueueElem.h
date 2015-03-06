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
 * \file algorithms/protocols/broadcast/BcastQueueElem.h
 * \brief ???
 */

#ifndef __algorithms_protocols_broadcast_BcastQueueElem_h__
#define __algorithms_protocols_broadcast_BcastQueueElem_h__

#include "util/queue/Queue.h"
#include "algorithms/composite/Composite.h"
#include "util/ccmi_util.h"

namespace CCMI
{
  namespace Adaptor
  {
    namespace Broadcast
    {

      class BcastQueueElem : public PAMI::MatchQueueElem<>
      {
        protected:
          //matchq
          unsigned            _bytes;  ///Bytes in the broadcast
          PAMI_Callback_t     _cb_done;///Application completion callback

          char              * _rcvbuf;  ///buffer to receive bcast
          char              * _appbuf;  ///App buffer which will be
          ///different from rcvbuf for an
          ///unexpected bcast

          bool                _isFinished;  ///Bcast completed locally?
          CCMI::Executor::Composite   * _composite;  ///Executor Composite associated with this queue elem

        public:

          ///
          /// \brief Default constructor
          ///
          BcastQueueElem (CCMI::Executor::Composite *c = NULL, unsigned root = -1) : PAMI::MatchQueueElem<> (root),
              _isFinished (false),
              _composite (c)
          {
          }

          void initUnexpMsg (unsigned bytes, char *unexpbuf)
          {
            _bytes    = bytes;
            _rcvbuf   = unexpbuf;

            _appbuf = NULL;  //Will be set by the application later
            _cb_done.function = NULL;
            _cb_done.clientdata = NULL;

            CCMI_assert(bytes > 0);//SSS ???
          }

          void initPostMsg (unsigned bytes, char *rcvbuf, PAMI_Callback_t &cb)
          {
            _bytes    = bytes;
            _cb_done  = cb;
            _rcvbuf   = rcvbuf;
            _appbuf   = NULL;
          }

          ///
          /// \brief Call this function when the bcast was received
          ///        unexpected and the application wants to provide the final target
          ///        buffer
          void  setPosted (unsigned bytes, char *buf,
                           pami_callback_t &cb_done)
          {
            CCMI_assert(bytes >= _bytes);
            _appbuf = buf;
            _cb_done = cb_done;
          }

          CCMI::Executor::Composite *composite ()
          {
            return _composite;
          }

          PAMI_Callback_t  &callback ()
          {
            return _cb_done;
          }

          unsigned bytes ()
          {
            return _bytes;
          }
          unsigned root  ()
          {
            return key();
          }

          char *rcvbuf ()
          {
            return _rcvbuf;
          }

          bool  isFinished ()
          {
            return _isFinished;
          }
          void  setFinished ()
          {
            _isFinished = true;
          }

          bool  isPosted ()
          {
            return(_appbuf != NULL);
          }

          void completeUnexpected ()
          {
            CCMI_assert(isPosted() && isFinished());
            memcpy (_appbuf, _rcvbuf, _bytes);

            if (_cb_done.function)
              _cb_done.function(NULL, _cb_done.clientdata, PAMI_SUCCESS);
          }

          void completePosted ()
          {
            if (_cb_done.function)
              _cb_done.function(NULL, _cb_done.clientdata, PAMI_SUCCESS);
          }

      } __attribute__((__aligned__(16))); //- BcastQueueElem
    };  //- Broadcast
  };  //- Adaptor
};  //- CCMI

#endif
