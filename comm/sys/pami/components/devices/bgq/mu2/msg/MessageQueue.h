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
 * \file components/devices/bgq/mu2/msg/MessageQueue.h
 * \brief ???
 */

#ifndef __components_devices_bgq_mu2_msg_MessageQueue_h__
#define __components_devices_bgq_mu2_msg_MessageQueue_h__


#include "util/queue/Queue.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class MessageQueue : public PAMI::Queue
      {
        public:

          class Element : public PAMI::Queue::Element
          {
            public:

              inline Element () {};
              inline ~Element () {};

              ///
              /// \brief Message element advance method
              ///
              /// \retval true  Message complete
              /// \retval false Message incomplete and must remain on the queue
              ///
              virtual bool advance () = 0;

          };  // class     PAMI::Device::MU::MessageQueue::Element


          inline MessageQueue () {};
          virtual ~MessageQueue () {};

          ///
          /// \brief Post a message to the end of the queue
          ///
          /// \param[in] msg Message to post
          ///
          inline void post (MU::MessageQueue::Element * msg)
          {
            enqueue (msg);
          };

          ///
          /// \brief Message queue advance method
          ///
          /// \retval true  Queue contains active messages
          /// \retval false Queue is empty
          ///
          inline bool advance ()
          {
            MU::MessageQueue::Element * msg = NULL;

            while ((msg = (MU::MessageQueue::Element *) dequeue()) != NULL)
              {
                if (msg->advance() == false)
                  {
                    push (msg);
                    return true;
                  }
              }

            return (size() > 0);
          }

      }; // class     PAMI::Device::MU::MessageQueue
    };   // namespace PAMI::Device::MU
  };     // namespace PAMI::Device
};       // namespace PAMI


#endif // __components_devices_bgq_mu2_msg_MessageQueue_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
