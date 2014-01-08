/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#ifndef  _KERNEL_ENVMON_H_
#define  _KERNEL_ENVMON_H_

enum {
  EMON_DCA_DATID_DCA0_IMON1           = 0,
  EMON_DCA_DATID_DCA0_VO_INTBUSP1     = 1,
  EMON_DCA_DATID_DCA1_IMON1           = 2,
  EMON_DCA_DATID_DCA1_VO_INTBUSP1     = 3,
  EMON_DCA_DATID_TOTAL_IMON1          = 4,
  EMON_DCA_DATID_DCA0_IMON2           = 5,
  EMON_DCA_DATID_DCA0_VO_INTBUSP2     = 6,
  EMON_DCA_DATID_DCA1_IMON2           = 7,
  EMON_DCA_DATID_DCA1_VO_INTBUSP2     = 8,
  EMON_DCA_DATID_TOTAL_IMON2          = 9,
  EMON_DCA_DATID_DCA0_IMON3           = 10,
  EMON_DCA_DATID_DCA0_VO_INTBUSP3     = 11,
  EMON_DCA_DATID_DCA1_IMON3           = 12,
  EMON_DCA_DATID_DCA1_VO_INTBUSP3     = 13,
  EMON_DCA_DATID_TOTAL_IMON3          = 14,
  EMON_DCA_DATID_DCA0_IMON4           = 15,
  EMON_DCA_DATID_DCA0_VO_INTBUSP4     = 16,
  EMON_DCA_DATID_DCA1_IMON4           = 17,
  EMON_DCA_DATID_DCA1_VO_INTBUSP4     = 18,
  EMON_DCA_DATID_TOTAL_IMON4          = 19,
  EMON_DCA_DATID_DCA0_IMON6           = 20,
  EMON_DCA_DATID_DCA0_VO_INTBUSP6     = 21,
  EMON_DCA_DATID_DCA1_IMON6           = 22,
  EMON_DCA_DATID_DCA1_VO_INTBUSP6     = 23,
  EMON_DCA_DATID_TOTAL_IMON6          = 24,
  EMON_DCA_DATID_DCA0_IMON8           = 25,
  EMON_DCA_DATID_DCA0_VO_INTBUSP8     = 26,
  EMON_DCA_DATID_DCA1_IMON8           = 27,
  EMON_DCA_DATID_DCA1_VO_INTBUSP8     = 28,
  EMON_DCA_DATID_TOTAL_IMON8          = 29,
  EMON_DCA_DATID_TOTAL_POWER_H        = 30, // obsolete, just for avoiding compilation errors
  EMON_DCA_DATID_TOTAL_POWER_L        = 31, // obsolete, just for avoiding compilation errors
  EMON_DCA_DATID_NUM_OLD              = 32, // for OLD FPGA

  EMON_DCA_DATID_DCA0_IMON7           = 30,
  EMON_DCA_DATID_DCA0_VBUF7           = 31,
  EMON_DCA_DATID_DCA1_IMON7           = 32,
  EMON_DCA_DATID_DCA1_VBUF7           = 33,
  EMON_DCA_DATID_TOTAL_IMON7          = 34,
  EMON_DCA_DATID_NUM                  = 35,
};

// N= 0..11
#define EMON_DCA_DATID_IMONX(N)      ( ((N)>>1)*5 + ((N)&1)*2)
#define EMON_DCA_DATID_VO_INTBUSPX(N) ( ((N)>>1)*5 + ((N)&1)*2 + 1)

#define EMON_DCA_DATID_TO_DCAID(N) (((N)%5) >> 1)

#define EMON_DCA_N_DOMAINS 7
#define EMON_DCA_N_DOMAINS_OLD 6

#define EMON_DCA_USE_VOINTBUS  1

#define USE_VOINTBUS (emon_mode & EMON_DCA_USE_VOINTBUS)

enum {
  EMON_DCA_FV_CTRLONRX0, // initial version 0 : control info appears in Rx0
  EMON_DCA_FV_CTRLONRX5, // version 1 : control info appears in Rx5
  EMON_DCA_FV_LONG,      // version 2 : long format and domain 7 support
  EMON_DCA_FV_INVALID    // neither of them
};

extern uint8_t fpga_version;
extern int     emon_mode;

#ifdef EMON_DEFINE_GLOBALS
uint8_t fpga_version = EMON_DCA_FV_INVALID;
int     emon_mode = 0;
#endif

/*!
 * \brief Retrieve raw power-consumption data for the current nodeboard.
 *
 *  \warning THIS INTERFACE IS EXPERIMENTAL AND UNSUPPORTED.
 *
 * \param[in] mode - 0 or EMON_DCA_USE_VOINTBUS
 * \param[out] fpga_version_p - NULL or location where fpga version number
 *                              is to be stored
 * \param[out] raw_v_data - NULL or address of a 14-dword vector where raw
 *                          voltage information is to be stored
 * \param[out] raw_a_data - NULL or address of a 14-dword vector where raw
 *                          amperage information is to be stored
 *
 *
 * \return Error indication
 * \retval  0 success
 * \retval  EFAULT if any of the output pointers are invalid
 * \retval  ENODEV if the fpga isn't recognized
 * \retval  EIO if emon errors occur
 *
 */

__INLINE__
uint64_t Kernel_GetRawEnvmonPowerData(int mode, uint8_t *fpga_version_p,
				      uint64_t raw_v_data[14],
				      uint64_t raw_a_data[14]);

/*!
 * \brief Set power-consumption control parameters.
 *
 * \warning THIS INTERFACE IS EXPERIMENTAL AND UNSUPPORTED.
 *
 * This interface can affect performance and power consumption for the
 * whole node, so calling it from a sub-node CNK job is disallowed.
 *
 * \param[in] param - what control parameter to modify.  Possibilities are:
 *                        DDR_IDLES_PER_ACCESS
 *                        (only one control so far)
 *
 * \param[in] value - value to which param is to be set.  Possibilities depend
 *                    on the value of param:
 *                        DDR_IDLES_PER_ACCESS: 0 <= value <= 126
 *
 * \return Error indication
 * \retval  0 success
 * \retval  ENOSYS if the interface is called from a sub-node job
 * \retval  EINVAL if any of the input parameters are invalid
 *
 */

#define DDR_IDLES_PER_ACCESS 1
__INLINE__
uint64_t Kernel_SetPowerConsumptionParam(uint64_t param, uint64_t value);


/*!
 * \brief Executes Envmon2 FPGA&DCA interface command
 *
 *  THIS INTERFACE IS EXPERIMENTAL AND UNSUPPORTED.
 *
 * \param[in] command      The Envmon2 command to be executed
 *
 * \return Error indication
 * \retval  0 success
 * \retval  EIO if emon errors occur
 *
 */

__INLINE__
uint64_t Kernel_ExecEnvmon2Command(uint64_t command);


/**
 * \brief Include implementations of the above functions.
 */
#include "envmon_impl.h"

#endif /* _KERNEL_ENVMON_H_ */
