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
 * \file components/devices/bgp/collective_network/CNBarrierMsg.h
 * \brief Currently, Collective Network Barrier is not used.
 */
#ifndef __components_devices_bgp_collective_network_CNBarrierMsg_h__
#define __components_devices_bgp_collective_network_CNBarrierMsg_h__

#if NOT_YET // or ever?
#include "bgp/tree/Device.h"
#include "bgp/tree/Message.h"

namespace PAMI
{
  namespace Protocol
    {
      namespace MultiSend
        {
          class TreeBarrierMessage: public PAMI::Queueing::Tree::TreeSendMessage
            {
            public:
              ///
              /// \brief Non Blocking Tree Barrier Message
              ///
              /// \param[in]  Tree Device to send the data
              /// \param[in]  Classroute to send the data on
              /// \param[in]  DispatchID to put in the header
              ///
              TreeBarrierMessage(PAMI::Queueing::Tree::Device    &TreeQS,
                                 unsigned                         classroute,
                                 unsigned                         dispatch_id,
                                 const PAMI_Callback_t            cb_done):
                TreeSendMessage(TreeQS, cb_done, 1),
                _classroute(classroute),
                _modelPkt(classroute,
                          PAMI::Tree::COMBINE_OP_OR,
                          7,
                          dispatch_id)
                {
                }
                int advance(unsigned cycles, PAMI::Queueing::Tree::TreeMsgContext ctx);
            private:
              unsigned           _classroute;
              PAMI::Tree::Packet _modelPkt;
            };

          ///
          /// \brief Non Blocking Tree Barrier Receive Message
          ///
          /// \param[in]  Tree Device to send the data
          /// \param[in]  Callback to dispatch
          ///
          class TreeBarrierRecvMessage: public PAMI::Queueing::Tree::TreeRecvMessage
            {
            public:
              TreeBarrierRecvMessage(PAMI::Queueing::Tree::Device    &TreeQS,
                                     const PAMI_Callback_t            cb):
                TreeRecvMessage(TreeQS, cb, NULL, 1, 1, PAMI_BCAST_RECV_NOSTORE)
                {
                }
              int advance(unsigned cycles, PAMI::Queueing::Tree::TreeMsgContext ctx);
            private:
            };
        }
    }
}

#endif

///
/// \file components/devices/bgp/collective_network/CNBarrierMsg.h
/// \brief Tree barrier implementation.
///
#define __INLINE__ inline
#include <Util.h>
#include "multisend/bgp/tree/TreeBarrier.h"
#include "spi/bgp_SPI.h"

namespace PAMI
{
  namespace Protocol
  {
    namespace MultiSend
    {
      //  Barrier Code
      int TreeBarrierMessage::advance(unsigned cycles, PAMI::Queueing::Tree::TreeMsgContext ctx)
      {
        register unsigned rechcount, recdcount, injhcount, injdcount;
        CollectiveFifoStatus(VIRTUAL_CHANNEL,
                &rechcount, &recdcount,
                &injhcount, &injdcount);
        if (injhcount >= TREE_FIFO_SIZE || injdcount >= TREE_QUADS_PER_FIFO) {
                return PAMI::Queueing::Tree::Working;
        }
        CollectiveRawSendPacket0(VIRTUAL_CHANNEL,&_modelPkt._hh);
        return PAMI::Queueing::Tree::Done;
      }
      int TreeBarrierRecvMessage::advance(unsigned cycles, PAMI::Queueing::Tree::TreeMsgContext ctx)
      {
        register unsigned rechcount, recdcount, injhcount, injdcount;
        CollectiveFifoStatus(VIRTUAL_CHANNEL,
                &rechcount, &recdcount,
                &injhcount, &injdcount);
        if (rechcount == 0 || recdcount == 0) {
                return PAMI::Queueing::Tree::Working;
        }
        CollectiveRawReceivePacketNoHdrNoStore(VIRTUAL_CHANNEL);
        return PAMI::Queueing::Tree::Done;
      }
    };
  };
};
#endif // NOT_YET
#endif // __components_devices_bgp_cnbarriermsg_h__
