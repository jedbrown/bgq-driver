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
 * \file components/devices/mpi/mpimulticombinemodel.h
 * \brief ???
 */

#ifndef __components_devices_mpi_mpimulticombinemodel_h__
#define __components_devices_mpi_mpimulticombinemodel_h__

#include <pami.h>
#include "components/devices/MulticombineModel.h"

namespace PAMI
{
    namespace Device
    {
        template <class T_Device, class T_Message>
        class MPIMulticombineModel :
        public Interface::MulticombineModel<MPIMulticombineModel<T_Device, T_Message>,T_Device,sizeof(T_Message)>
        {
        public:
          static const size_t multicombine_model_state_bytes = sizeof(T_Message);
          static const size_t sizeof_msg                     = sizeof(T_Message);


          MPIMulticombineModel (T_Device & device, pami_result_t &status) :
              Interface::MulticombineModel < MPIMulticombineModel<T_Device, T_Message>,T_Device,sizeof(T_Message) > (device, status)
                {};


          inline pami_result_t postMulticombine_impl (uint8_t (&state)[multicombine_model_state_bytes],
						      size_t           client,
						      size_t           context, 
                                                      pami_multicombine_t *mcombine,
                                                      void *devinfo)
                {

              return PAMI_SUCCESS;
                }
        };
    };
};
#endif // __components_devices_mpi_mpimulticombinemodel_h__
