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
 * \file components/devices/bgp/collective_network/CNAllreduce.h
 * \brief Default collective network allreduce interface.
 */
#ifndef __components_devices_bgp_collective_network_CNAllreduce_h__
#define __components_devices_bgp_collective_network_CNAllreduce_h__

#include <pami.h>
#include "util/common.h"
#include "components/devices/MulticombineModel.h"
#include "components/devices/bgp/collective_network/CNPacket.h"
#include "components/devices/workqueue/WorkQueue.h"
#include "math/bgp/collective_network/pami_bg_math.h"
#include "math/FloatUtil.h"

extern int pami_dt_shift[PAMI_DT_COUNT];

typedef coremath1 preprocess;
typedef coremath vnmprocess;
typedef coremath1 postprocess;

/**
 * \brief Function to set up function pointers for collective processing
 *
 * Based on datatype and operator, chooses pre- and post-processing routines
 * (when Collective Network hardware is used), virtual node mode routine (to
 * implement operator on datatype), hardware header operator code (for
 * Collective Network hardware), and size of operands in datatype.
 *
 * Note, the hardware header operator code may not be equivalent to 'op'
 * as pre- and post-processing may dictate.
 *
 * \param[in]	dt	Datatype of operands
 * \param[in]	op	Operator
 * \param[out]	pre	Pre-processing routine required for CN
 * \param[out]	post	Post-processing routine required for CN
 * \param[out]	vnm	Implementation of operator on datatype, for VNM
 * \param[out]	hhfunc	Hardware code for operator used on CN
 * \param[out]	opsize	Size of operands in datatype
 * \return	Zero on success, -1 if dt/op pair is not supported.
 */
static inline int allreduce_setup(pami_dt	dt,
                                pami_op		op,
                                preprocess	*pre,
                                postprocess	*post,
                                unsigned	*hhfunc,
                                unsigned	*opshift) {
        *pre =NULL;
        *post=NULL;
        *opshift = pami_dt_shift[dt];
        switch(op) {
        case PAMI_SUM:
                *hhfunc =  PAMI::Device::BGP::COMBINE_OP_ADD;
                break;
        case PAMI_MAX:
        case PAMI_MAXLOC:
                *hhfunc =  PAMI::Device::BGP::COMBINE_OP_MAX;
                break;
        case PAMI_MIN:
        case PAMI_MINLOC:
                *hhfunc =  PAMI::Device::BGP::COMBINE_OP_MAX;
                break;
        case PAMI_BAND:
        case PAMI_LAND:
                *hhfunc =  PAMI::Device::BGP::COMBINE_OP_AND;
                break;
        case PAMI_BOR:
        case PAMI_LOR:
                *hhfunc =  PAMI::Device::BGP::COMBINE_OP_OR;
                break;
        case PAMI_BXOR:
        case PAMI_LXOR:
                *hhfunc =  PAMI::Device::BGP::COMBINE_OP_XOR;
                break;
        default:
                return -1;
        }
        *pre = PAMI_PRE_OP_FUNCS(dt,op,1);
        if (*pre == (void *)PAMI_UNIMPL) {
                return -1;
        }
        *post = PAMI_POST_OP_FUNCS(dt,op,1);
        return 0;
}

namespace PAMI {
namespace Device {
namespace BGP {

extern void postprocSum1PDouble(void *out, void *in, int c);
extern void preprocSum1PDouble(void *out, void *in, int c);
extern void preprocSum1PFloat(void *out, void *in, int c);
extern void postprocSum1PFloat(void *out, void *in, int c);

/**
 * \brief Object to hold info about a Collective Network Allreduce
 *
 * Creates an object that contains all pertinent information
 * about an Allreduce on the Collective Network, based on datatype
 * and operator.
 */
class CNAllreduceSetup {
        static CNAllreduceSetup CNAllreduceSetupCache[PAMI_OP_COUNT][PAMI_DT_COUNT];
public:

        CNAllreduceSetup() {}
        CNAllreduceSetup(pami_dt dt, pami_op op)
                {

                int rc = allreduce_setup(dt, op, &_pre, &_post,
                                        &_hhfunc, &_logopsize);
                if (rc != 0)
                        _hhfunc = PAMI::Device::BGP::COMBINE_OP_NONE;

                _dbl_sum = (dt == PAMI_DOUBLE && op == PAMI_SUM);
                //_flt_sum = (dt == PAMI_FLOAT && op == PAMI_SUM);
                if (_dbl_sum) {
                        _logopsize = 8; // 2^8 == 256 == BGPCN_PKT_SIZE
                //} else if (_flt_sum) {
                        // TBD...
                }
                _logbytemult = _logopsize - pami_dt_shift[dt];
                _opsize = (1 << _logopsize);
        }
        static void initCNAS();
        static CNAllreduceSetup &getCNAS(pami_dt dt, pami_op op) {
                return CNAllreduceSetupCache[op][dt];
        }
        preprocess         _pre;
        postprocess        _post;
        //vnmprocess         _vnm;
        unsigned           _hhfunc;
        unsigned           _opsize;
        unsigned           _logopsize;
        unsigned           _logbytemult;
        bool		   _dbl_sum;
        //bool		   _flt_sum;
};	// class CNAllreduceSetup

};      // namespace BGP
};      // namespace Device
};	// PAMI

#endif // __components_devices_bgp_cnallreduce_h__
