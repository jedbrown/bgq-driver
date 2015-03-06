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
 * \file components/devices/MultisyncModel.h
 * \brief ???
 */

#ifndef __components_devices_MultisyncModel_h__
#define __components_devices_MultisyncModel_h__

#include <pami.h>
#include "util/common.h"

/**
 * \brief Multisend interfaces.
 *
 * A multisend operation allows many message passing transactions to
 * be performed in the same call to amortize software overheads.  It
 * has two flavors
 *
 *   - multicast, which sends the same buffer to a list of processors
 *     and also supports depost-bit torus line broadcasts
 *
 *   - manytomany, which sends different offsets from the same buffer
 *     to a list of processors
 *
 * As reductions have a single source buffer and destination buffer,
 * we have extended the multicast call to support reductions.
 *
 * Each multisend operation carries a connection id to identify the
 * data on the reciever.
 *
 * In a multisend operation the cores are typically involved at the
 * end-points. Moreover the processor lists can be created on the fly.
 */


  /******************************************************************************
   *       Multisync Personalized synchronization/coordination
   ******************************************************************************/

  /**
   * \brief structure defining interface to Multisync
   */
  typedef struct
  {
    //size_t             client;	        /**< client to operate within */
    //size_t             context;	        /**< primary context to operate within */
    pami_callback_t     cb_done;		/**< User's completion callback */
    unsigned           connection_id;	/**< (remove?) differentiate data streams */
    unsigned           roles;		/**< bitmap of roles to perform */
    pami_topology_t    *participants;	/**< Tasks involved in synchronization */
  } pami_multisync_t;


namespace PAMI
{
  namespace Device
  {
    namespace Interface
    {
      ///
      /// \todo Need A LOT MORE documentation on this interface and its use
      /// \param T_Model   Multisync model template class
      ///
      /// \see Multisync::Model
      ///
      template <class T_Model,class T_Device,unsigned T_StateBytes>
      class MultisyncModel
      {
      public:
        /// \param[in] device                Multisync device reference
        MultisyncModel (T_Device &device, pami_result_t &status)
          {
            COMPILE_TIME_ASSERT(T_Model::sizeof_msg <= T_StateBytes);
            status = PAMI_SUCCESS;
          };
        ~MultisyncModel ()
          {

          };

        inline pami_result_t postMultisyncImmediate(size_t            client,
						    size_t            context,
						    pami_multisync_t *msync,
						    void             *devinfo = NULL);
	
        inline pami_result_t postMultisync(uint8_t (&state)[T_StateBytes],
					   size_t            client,
					   size_t            context,
                                           pami_multisync_t *msync,
                                           void             *devinfo = NULL);
      }; // class MultisyncModel

      template <class T_Model,class T_Device, unsigned T_StateBytes>
      pami_result_t MultisyncModel<T_Model,T_Device,T_StateBytes>::postMultisync(uint8_t (&state)[T_StateBytes],
										 size_t            client,
										 size_t            context,
                                                                                 pami_multisync_t *msync,
                                                                                 void             *devinfo)
      {
        return static_cast<T_Model*>(this)->postMultisync_impl(state, client, context, msync, devinfo);
      }

      template <class T_Model,class T_Device, unsigned T_StateBytes>
      pami_result_t MultisyncModel<T_Model,T_Device,T_StateBytes>::postMultisyncImmediate(size_t            client,
											  size_t            context,
											  pami_multisync_t *msync,
											  void             *devinfo)
      {
        return static_cast<T_Model*>(this)->postMultisyncImmediate_impl(client, context, msync, devinfo);
      }

    }; // namespace Interface
  }; // namespace Device
}; // namespace PAMI
#endif // __components_devices_MultisyncModel_h__
