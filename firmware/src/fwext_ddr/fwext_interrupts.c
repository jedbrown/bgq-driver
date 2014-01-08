/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */



#include <firmware/include/fwext/fwext.h>
//#include <firmware/include/personality.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/BIC.h>
#include <hwi/include/bqc/BIC_inlines.h>

FwExt_Regs_t FWExt_InterruptState[NUM_HW_THREADS][3] ALIGN_L2_CACHE; // Storage for 68 threads + 3 interrupt priorities




//typedef void (*FWExt_InterruptHandler_t)(FwExt_Regs_t* context, uint64_t code);

int fwext_PUEA_Handler(FwExt_Regs_t* context, uint64_t code);
void Kernel_Crash(int);

void fwext_default_GEAHandler(FwExt_Regs_t* context, uint64_t code) {

  int i;

  volatile uint64_t* mapped_status = &(GEA_DCR_PRIV_PTR->gea_mapped_interrupt_status0_0);

  for ( i = 0; i < 16; i++, mapped_status += 3 ) {
    if ( ( mapped_status[0] != 0 ) || ( mapped_status[1] != 0 ) || ( mapped_status[2] != 0 ) ) {
      printf("GEA Mapped Status [%d] %016lX-%016lX-%016lX\n", i, mapped_status[0], mapped_status[1], mapped_status[2] );
    }
  }
}

typedef struct _FWExt_PUEA_Data {
  int a;
  FwExt_InterruptHandler_t intHandler;
} FWExt_PUEA_Data_t;

static FWExt_PUEA_Data_t PUEA_Data[32] = {
  { 0, 0 }, // lane  0 (L2C[0])
  { 0, 0 }, // lane  1 (L2C[1])
  { 0, 0 }, // lane  2 (L2C[2])
  { 0, 0 }, // lane  3 (L2C[3])
  { 0, 0 }, // lane  4
  { 0, 0 }, // lane  5
  { 0, 0 }, // lane  6 (WAC  )
  { 0, 0 }, // lane  7 (MU[0])
  { 0, 0 }, // lane  8 (MU[1])
  { 0, 0 }, // lane  9 (MU[2])
  { 0, 0 }, // lane 10 (MU[3])
  { 0, fwext_default_GEAHandler }, // lane 11 (GEA[0])
  { 0, fwext_default_GEAHandler }, // lane 12 (GEA[1])
  { 0, fwext_default_GEAHandler }, // lane 13 (GEA[2])
  { 0, fwext_default_GEAHandler }, // lane 14 (GEA[3])
  { 0, fwext_default_GEAHandler }, // lane 15 (GEA[4])
  { 0, fwext_default_GEAHandler }, // lane 16 (GEA[5])  
  { 0, fwext_default_GEAHandler }, // lane 17 (GEA[6])
  { 0, fwext_default_GEAHandler }, // lane 18 (GEA[7])
  { 0, fwext_default_GEAHandler }, // lane 19 (GEA[8])
  { 0, fwext_default_GEAHandler }, // lane 20 (GEA[9])
  { 0, fwext_default_GEAHandler }, // lane 21 (GEA[10])
  { 0, fwext_default_GEAHandler }, // lane 22 (GEA[11])
  { 0, fwext_default_GEAHandler }, // lane 23 (GEA[12])
  { 0, fwext_default_GEAHandler }, // lane 24 (GEA[13])
  { 0, fwext_default_GEAHandler }, // lane 25 (GEA[14])
  { 0, fwext_default_GEAHandler }, // lane 26 (GEA[15])
  { 0, 0 }, // lane 27 (L1P[0])
  { 0, 0 }, // lane 28 (L1P[1])
  { 0, 0 }, // lane 29 (L1P[2])
  { 0, 0 }, // lane 30 (L1P[3])
  { 0, 0 }, // lane 31 (L1P[4])
};



void fwext_setInterruptHandler( FwExt_InterruptHandler_t handler ) {
  OPD_Entry_t* opd = (OPD_Entry_t*)handler;
  mtspr(SPRN_SPRG4, opd->Entry_Addr);
}

void fwext_takeMachineCheckHandlerFromFirmware( void ) {
  extern void* _fwext_Vec_MCHK;
  mtspr( SPRN_IVPR,  &_fwext_Vec_MCHK);
}

void fwext_DefaultInterruptHandler(FwExt_Regs_t* context, uint64_t code) {

  static char* INTERRUPT_CODES[] = {
    "Machine Check",
    "Critical Interrupt",
    "Debug",
    "Data Storage Exception",  
    "Instruction Storage Exception",
    "External Interrupt",
    "Alignment Interrupt",
    "Program Interrupt",
    "Floating Point Unavailable",
    "System Call",
    "APU",
    "Decrementer",
    "Fixed Interval Timer",
    "Watchdog Timer",
    "Data TLB Miss",
    "Instruction TLB Miss",
    "Vector Unavailable",
    "?",
    "?",
    "?",
    "Processor Doorbell",
    "Processor Doorbell Critical",
    "Guest Doorbell",
    "Guest Doorbell Critical",
    "Hypervisor System Call",
    "Hypervisor Privilege",
    "LRAT Error"
  };

  unsigned n = code>>5;

  printf(
        "Interrupt.  Code=%lX (%s)  IP=%lx  LR=%lx  MSR=%lx DEAR=%lx ESR=%lx MCSR=%lx Timestamp=%lx\n", 
        (unsigned long)code, 
        (n < sizeof(INTERRUPT_CODES)/sizeof(char*)) ? INTERRUPT_CODES[n] : "?",  
        context->ip, 
        context->lr, 
        context->msr, 
        mfspr(SPRN_DEAR), 
        mfspr(SPRN_ESR),
        mfspr(SPRN_MCSR),
        GetTimeBase());


  switch (code) {

  case IVO_MCHK :
  case IVO_CI   :
  case IVO_EI   :
    {
      fwext_PUEA_Handler(context,code);
      break;
    }

    // NOTE: all other interrupts are fatal at this time:

  default : 
    {
      Kernel_Crash(code);
      break;
    }
  }
}




#define PUEA_STATUS_L2C_MASK _B4(33,-1)
#define PUEA_STATUS_MU_MASK  _B4(40,-1)
#define PUEA_STATUS_GEA_MASK _B16(56,-1)
#define PUEA_STATUS_L1P_MASK _B5(61,-1)
#define PUEA_STATUS_C2C_MASK _B2(63,-1)
#define PUEA_STATUS_WAC_MASK _B1(36,-1)

#define PUEA_STATUS_L2C_BASE  30
#define PUEA_STATUS_MU_BASE   37
#define PUEA_STATUS_GEA_BASE  41
#define PUEA_STATUS_L1P_BASE  57


int fwext_FindFirstOneBit(uint64_t data) {

  uint64_t mask = 1ul << 63;
  int i;

  for (i = 0; mask; i++, mask >>= 1) {
    if ( ( data & mask ) != 0 ) {
      return i;
    }
  }
  return -1;

}

/**
 * @brief Converts the PUEA status word into one of the 32 PUEA lanes (or -1
 *        if no bit is detected).
 */
int fwext_PueaStatus2Lane(uint64_t status) {

  if ( ( status & PUEA_STATUS_L2C_MASK ) != 0 ) {
    return fwext_FindFirstOneBit(status) - PUEA_STATUS_L2C_BASE + BIC_MAP_L2C_LANE(0);
  }
  else if ( ( status & PUEA_STATUS_MU_MASK ) != 0 ) {
    return fwext_FindFirstOneBit(status) - PUEA_STATUS_MU_BASE + BIC_MAP_MU_LANE(0);
  }
  else if ( ( status & PUEA_STATUS_GEA_MASK ) != 0 ) {
    return fwext_FindFirstOneBit(status) - PUEA_STATUS_GEA_BASE + BIC_MAP_GEA_LANE(0);
  }
  else if ( ( status & PUEA_STATUS_L1P_MASK ) != 0 ) {
    return fwext_FindFirstOneBit(status) - PUEA_STATUS_L1P_BASE + BIC_MAP_L1P_LANE(0);
  }
  else if ( (status & PUEA_STATUS_WAC_MASK ) != 0 ) {
    return BIC_MAP_WAC_LANE;
  }
  else {
    return -1;
  }
}


int fwext_PUEA_Handler(FwExt_Regs_t* context, uint64_t code) {

#if 0
  BIC_Device* bic = ((BIC_Device *)BIC_DEVICE_BASE_ADDRESS);
  printf("(D) EI : %016llX-%016llX-%016llX-%016llX\n", bic->_ext_int_summary[0],bic->_ext_int_summary[1],bic->_ext_int_summary[2],bic->_ext_int_summary[3]);
  printf("(D) CI : %016llX-%016llX-%016llX-%016llX\n", bic->_crit_int_summary[0],bic->_crit_int_summary[1],bic->_crit_int_summary[2],bic->_crit_int_summary[3]);
  printf("(D) MC : %016llX-%016llX-%016llX-%016llX\n", bic->_mach_int_summary[0],bic->_mach_int_summary[1],bic->_mach_int_summary[2],bic->_mach_int_summary[3]);
#endif

  uint64_t summary = 0;

  switch (code) {
  case IVO_MCHK : summary = BIC_ReadMachineCheckIntSummary(ProcessorThreadID());    break;
  case IVO_CI   : summary = BIC_ReadCriticalIntSummary(ProcessorThreadID());        break;
  case IVO_EI   : summary = BIC_ReadExternalIntSummary(ProcessorThreadID());        break;
  default :
    {
      printf("(E) Illegal interrupt code (code=0x%lX) [%s:%d]\n", code, __func__, __LINE__);
      Kernel_Crash(__LINE__);
      return -1; // keep BEAM quiet
    }
  }

  int lane = fwext_PueaStatus2Lane(summary);

  printf("(I) PUEA summary : summary=%lX (code=%lX) lane=%d\n", summary, code, lane);

  if ((lane < 0) || (lane >= 32)) {
     Kernel_Crash(__LINE__);
     return -1; // keep BEAM quiet
  }

  if ( PUEA_Data[lane].intHandler != 0 ) {
    (*PUEA_Data[lane].intHandler)( context, code );
    return 0;
  }

  return -1;
}


/**
 * @brief Sets the interrupt handler for the specified PUEA lane.  The handler is identical
 *        for all threads on all cores.
 * @param[in] lane specifies one of the 32 interrupt lanes in the PUEA interrupt
 *        map.  This is typically macro-tized via the BIC_MAP_*_LANE(n) macros.
 * @param[in] handler provides the interrupt handler function.  If NULL is passed, the default
 *       handler is used.
 * @example fwext_setPUEAInterruptHandler( BIC_MAP_MU_LANE(1), 4, myInterruptHandler );
 */

void fwext_setPUEAInterruptHandler( unsigned lane, FwExt_InterruptHandler_t handler ) {
  PUEA_Data[lane].intHandler = handler;
}

int fwext_mapPUEAInterruptLane( unsigned lane, unsigned threadMask, unsigned interruptType ) {

  unsigned t;
  
  BIC_REGISTER mask = 0x3 << ( 62 - (lane*2) );
  BIC_REGISTER typE = (BIC_REGISTER)interruptType << (62 - (lane*2));

  for (t = 0; t < 4; t++) {
    if ( ( threadMask & (1<<t) ) != 0 ) {
      BIC_REGISTER map  = BIC_ReadInterruptMap(t);  
      map = (map & ~mask) | typE;
      BIC_WriteInterruptMap(t, map);
    }
  }

  return 0;
}
