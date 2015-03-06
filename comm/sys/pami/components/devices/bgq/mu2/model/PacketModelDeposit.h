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
 * \file components/devices/bgq/mu2/model/PacketModelDeposit.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_PacketModelDeposit_h__
#define __components_devices_bgq_mu2_model_PacketModelDeposit_h__

#include "components/devices/bgq/mu2/model/PacketModel.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      typedef enum {
        UNDETERMINED = 0,
        POSITIVE,
        MINUS
      } direction_t;

      template <direction_t T_DIRECTION=UNDETERMINED>
      class PacketModelDeposit : public MU::PacketModel
      {
        public :

          /// \see PAMI::Device::Interface::PacketModel::PacketModel
          /// \param[in] hint_p  true = positive direction, false = minus direction
          inline PacketModelDeposit (MU::Context & context) :
              MU::PacketModel (context)
          {
#if 0//__bgq__
//          PAMI_assert(T_DIRECTION != UNDETERMINED);
            PAMI_assert((T_DIRECTION == MINUS) || (T_DIRECTION == POSITIVE)); // In case we add directions?

            uint8_t hintsABCD = T_DIRECTION == POSITIVE ? 
              MUHWI_PACKET_HINT_AP|MUHWI_PACKET_HINT_BP|MUHWI_PACKET_HINT_CP|MUHWI_PACKET_HINT_DP :
              MUHWI_PACKET_HINT_AM|MUHWI_PACKET_HINT_BM|MUHWI_PACKET_HINT_CM|MUHWI_PACKET_HINT_DM;

            uint8_t hintsE = T_DIRECTION == POSITIVE ? MUHWI_PACKET_HINT_EP : MUHWI_PACKET_HINT_EM;
            _singlepkt.setHints(hintsABCD,hintsE);
            _multipkt.setHints(hintsABCD,hintsE);
#endif
            _singlepkt.setDeposit(MUHWI_PACKET_DEPOSIT);

            _multipkt.setDeposit(MUHWI_PACKET_DEPOSIT);
          };

          /// \see PAMI::Device::Interface::PacketModel::~PacketModel
          inline ~PacketModelDeposit () {};

      }; // PAMI::Device::MU::PacketModelDeposit class
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#endif // __components_devices_bgq_mu2_PacketModelDeposit_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
