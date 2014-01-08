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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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

#include "Kernel.h"

#define EMON_DEFINE_GLOBALS
#include <spi/include/kernel/envmon.h>

uint64_t sc_GETRAWENVMONPOWERDATA( SYSCALL_FCN_ARGS )
{
    uint32_t mode = (uint32_t) r3;
    uint8_t *fpga_version_p = (uint8_t *) r4;
    uint64_t *raw_v_data = (uint64_t *) r5;
    uint64_t *raw_i_data = (uint64_t *) r6;
    uint64_t rc;

    if ((fpga_version_p != NULL) &&
	!VMM_IsAppAddress(fpga_version_p, sizeof(uint8_t)))
    {
        return CNK_RC_SPI(EFAULT);
    }

    if ((raw_v_data != NULL) &&
	!VMM_IsAppAddress(raw_v_data, 14 * sizeof(uint64_t)))
    {
        return CNK_RC_SPI(EFAULT);
    }

    if ((raw_i_data != NULL) &&
	!VMM_IsAppAddress(raw_i_data, 14 * sizeof(uint64_t)))
    {
        return CNK_RC_SPI(EFAULT);
    }

    rc = Kernel_GetRawEnvmonPowerData(mode, fpga_version_p,
				      raw_v_data, raw_i_data);

    return CNK_RC_SPI(rc);
}

uint64_t sc_SETPOWERCONSUMPTIONPARAM( SYSCALL_FCN_ARGS )
{
    uint64_t param = r3;
    uint64_t value = r4;

    if (IsSubNodeJob()) // Power control is not supported for subnode jobs
    {
	return CNK_RC_SPI(ENOSYS);
    }

    uint64_t rc = Kernel_SetPowerConsumptionParam(param, value);
    return CNK_RC_SPI(rc);

    // NOTE:  Power consumption controls will be restored to their default
    //        values at job termination in PowerManagement_Shutdown().
}

uint64_t sc_EXECENVMON2COMMAND( SYSCALL_FCN_ARGS )
{
    uint64_t command = r3;
    if (IsSubNodeJob()) // Power control is not supported for subnode jobs                                                       
    {                                                                                                                            
        return CNK_RC_SPI(ENOSYS);                                                                                                 
    }
    
    uint64_t rc = Kernel_ExecEnvmon2Command(command);
    return CNK_RC_SPI(rc);
}
