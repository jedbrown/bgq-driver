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
 * \file components/devices/bgq/mu2/model/DmaModel.h
 * \brief ???
 */
#ifndef __components_devices_bgq_mu2_model_DmaModel_h__
#define __components_devices_bgq_mu2_model_DmaModel_h__

#include "components/devices/bgq/mu2/model/DmaModelBase.h"

namespace PAMI
{
  namespace Device
  {
    namespace MU
    {
      class DmaModel : public MU::DmaModelBase<DmaModel,sizeof(MUSPI_DescriptorBase)>
      {
        public :

          static const bool dma_model_fence_supported = false;

          /// \see PAMI::Device::Interface::DmaModel::DmaModel
          inline DmaModel (MU::Context & device, pami_result_t & status) :
              MU::DmaModelBase<DmaModel,sizeof(MUSPI_DescriptorBase)> (device, status)
          {};

          /// \see PAMI::Device::Interface::DmaModel::~DmaModel
          inline ~DmaModel () {};

      }; // PAMI::Device::MU::DmaModel class
    };   // PAMI::Device::MU namespace
  };     // PAMI::Device namespace
};       // PAMI namespace

#endif // __components_devices_bgq_mu2_DmaModel_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
