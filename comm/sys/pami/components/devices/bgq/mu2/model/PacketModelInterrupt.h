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
 * \file components/devices/bgq/mu2/model/PacketModelInterrupt.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_PacketModelInterrupt_h__
#define __components_devices_bgq_mu2_model_PacketModelInterrupt_h__

#include "components/devices/bgq/mu2/model/PacketModel.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class PacketModelInterrupt : public MU::PacketModel
      {
        public :

          /// \see PAMI::Device::Interface::PacketModel::PacketModel
          inline PacketModelInterrupt (MU::Context & context) :
              MU::PacketModel (context)
          {
            _singlepkt.setInterrupt(MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL);
            _multipkt.setInterrupt(MUHWI_DESCRIPTOR_INTERRUPT_ON_PACKET_ARRIVAL);
          };

          /// \see PAMI::Device::Interface::PacketModel::~PacketModel
          inline ~PacketModelInterrupt () {};

      }; // PAMI::Device::MU::PacketModelInterrupt class
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#endif // __components_devices_bgq_mu2_PacketModelInterrupt_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
