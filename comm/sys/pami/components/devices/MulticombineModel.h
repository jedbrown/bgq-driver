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
 * \file components/devices/MulticombineModel.h
 * \brief ???
 */

#ifndef __components_devices_MulticombineModel_h__
#define __components_devices_MulticombineModel_h__

#include <sys/uio.h>

#include <pami.h>
#include "util/common.h"
#include "common/MultisendInterface.h"

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
   *       Multicombine Personalized reduction
   ******************************************************************************/
  /**
   * \brief structure defining interface to Multicombine
   *
   * The recv callback, and associated metadata parameters, are not valid for all
   * multicombines. Depending on the kind of multicombine being registered, it may
   * require that the recv callback be either NULL or valid. If the recv callback
   * is NULL then the metadata parameters should also be NULL (0) when inoking
   * the multicombine.
   *
   * data and results parameters may not always be required, depending on role (and other?).
   * For example, if a call the a multicombine specifies a single role of, say, "injection",
   * then the results parameters are not needed. Details of this are specified by the
   * type of multicombine being registered/used.
   */
  typedef struct
  {
    //size_t                client;               /**< client to operate within */
    //size_t                context;              /**< primary context to operate within */
    pami_callback_t       cb_done;              /**< User's completion callback */
    unsigned              connection_id;        /**< A connection is a distinct stream of
                                                   traffic. The connection id identifies the
                                                   connection */
    unsigned              roles;                /**< bitmap of roles to perform */
    pami_pipeworkqueue_t *data;                 /**< Data source */
    pami_topology_t      *data_participants;    /**< Tasks contributing data */
    pami_pipeworkqueue_t *results;              /**< Results destination */
    pami_topology_t      *results_participants; /**< Tasks receiving results */
    pami_op               optor;                /**< Operation to perform on data */
    pami_dt               dtype;                /**< Datatype of elements */
    size_t                count;                /**< Number of elements */
  } pami_multicombine_t;



namespace PAMI
{
    namespace Device
  {
        namespace Interface
        {
            ///
            /// \todo Need A LOT MORE documentation on this interface and its use
            /// \param T_Model   Multicombine model template class
            ///
            /// \see Multicombine::Model
            ///
            template <class T_Model,class T_Device, unsigned T_StateBytes>
            class MulticombineModel
            {
            public:
                /// \param[in] device                Multicombine device reference
                MulticombineModel (T_Device &device, pami_result_t &status) {
                        COMPILE_TIME_ASSERT(T_Model::sizeof_msg <= T_StateBytes);
                        status = PAMI_SUCCESS;
                };
                ~MulticombineModel () {};
                inline pami_result_t postMulticombine (uint8_t (&state)[T_StateBytes],
						       size_t               client,
						       size_t               context,
                                                       pami_multicombine_t *mcomb,
                                                       void                *devinfo=NULL);

		inline pami_result_t postMulticombineImmediate (size_t               client,
								size_t               context,
								pami_multicombine_t *mcomb,
								void                *devinfo=NULL);
            }; // class MulticombineModel

            template <class T_Model,class T_Device, unsigned T_StateBytes>
            pami_result_t MulticombineModel<T_Model,T_Device, T_StateBytes>::postMulticombine(uint8_t (&state)[T_StateBytes],
											      size_t               client,
											      size_t               context,
                                                                                              pami_multicombine_t *mcomb,
                                                                                              void                *devinfo)
            {
              return static_cast<T_Model*>(this)->postMulticombine_impl(state, client, context, mcomb, devinfo);
            }

	    template <class T_Model,class T_Device, unsigned T_StateBytes>
            pami_result_t MulticombineModel<T_Model,T_Device, T_StateBytes>::postMulticombineImmediate
	      (size_t               client,
	       size_t               context,
	       pami_multicombine_t *mcomb,
	       void                *devinfo)
            {
              return static_cast<T_Model*>(this)->postMulticombineImmediate_impl(client, context, mcomb, devinfo);
            }

        }; // namespace Interface
    }; // namespace Device
}; // namespace PAMI
#endif // __components_devices_MulticombineModel_h__
