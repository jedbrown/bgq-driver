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


#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/bqc/testint_dcr.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/testint_inlines.h>

#include <hwi/include/bqc/dc_arbiter_dcr.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>
#include <hwi/include/bqc/l2_dcr.h>

size_t BgGetRealLockIndex( size_t virtual_index )
{	
    if (virtual_index >= FWEXT_LOCK_COUNT)
    {
	printf("FWEXT lock virtual index %ld too large\n", virtual_index);
	Terminate(1000); // terminate with failure
    }
    return BeDRAM_LOCKNUM_FWEXT_FIRST + virtual_index;
}

void BgBarrier( size_t index_value, uint64_t targetValue )
{
   size_t realIndex = BgGetRealLockIndex(index_value);
   // barrier...
   BeDRAM_WriteAdd(realIndex,1); // this threads contribution.
   //int dl=15;
 
   uint64_t ullDramBarrierValue = BeDRAM_ReadRaw(realIndex);
   while (ullDramBarrierValue < targetValue)
   {  // we need a backoff to make sure we don't lock up the core...
      #if 0
      printf("@@@ BgBarrier - ullDramBarrierValue = %llu, targetValue = %llu \n", 
         (unsigned long long)ullDramBarrierValue, (unsigned long long)targetValue);
      #endif
      //_Delay_dbnz(dl);                    // obsolete backoff
      //dl=(dl|(dl<<1))&0xff;
      
      ullDramBarrierValue = BeDRAM_ReadRaw(realIndex);
   }
}

unsigned int BgGetNumThreads(void)
{
    static volatile uint64_t NumThreads = 0;
    uint64_t v;

    // Reading the thread_active DCR's from lots of threads is hideously
    // expensive.  We read them just once and cache the value.

    v = NumThreads;

    if (v == 0)
    {
	// NumThreads hasn't been computed yet.  See if we should compute it.
	size_t doneIndex = BgGetRealLockIndex(FWEXT_LOCKNUM_NUM_THRD_DONE);
	if (BeDRAM_ReadIncSat(doneIndex) == 0)
	{
	    // We won.  Compute NumThreads.
	    v = popcnt64(TESTINT_DCR_PRIV_PTR->thread_active0_rb) +
		    popcnt64(TESTINT_DCR_PRIV_PTR->thread_active1_rb);
	    if ((v <= 0) || (v > 68))
	    {
		Terminate(1001);  // Die right away. Should never be zero.
	    }
	    NumThreads = v;  // Update NumThreads, releasing any threads
			     // waiting below.
	}
	else
	{
	    // Someone else is computing NumThreads.  Wait for it.
	    if (((uint64_t) &NumThreads) >= 
				(VA_MINADDR_BEDRAM | VA_PRIVILEGEDOFFSET))
	    {
		// We're a bedram extension.  We can't just poll on NumThreads
		// without livelocking the core, so poll via the bedram
		// atomic space.
		do {
		    v = BeDRAM_AtomicRead(&NumThreads);
		} while (v == 0);
	    }
	    else
	    {
		// We're in ddr.  Just poll.  NumThreads will be cached in
		// L1 until it's final value is stored.
		do {
		    v = NumThreads;
		} while (v == 0);
	    }
	}
    }

    return v;
}

void Bgmemset(void* addr, char val, int size)
{
	 int i;
	 for (i = 0; i < size; i++) ((char*)addr)[i] = val;
}

void test_detach(void)
{
    size_t cntIndex = BgGetRealLockIndex(FWEXT_LOCKNUM_TEST_EXIT_CNT);
    int cnt;

    cnt = BeDRAM_ReadIncWrap(cntIndex) + 1;

    if (cnt == BgGetNumThreads())
    {
	puts("TEST_SUCCESS\n");
	Terminate(0);
	// NOTREACHED
    }
}

void __NORETURN test_exit(int status)
{
    if (status == 0)
    {

	// If the LLB check is enabled, we cannot simply detach.  We need
	// to run code on thread 0 of every core .... so we will politely
	// hang here until all threads have exited.

	if ( ( fwext_getenv("BG_LLBCHECK") != 0 ) && ( ! TI_isDD1() ) ) {

	    int cnt = 0;
	
	    while ( cnt < BgGetNumThreads() ) {
		fwext_udelay(1000);
		cnt = BeDRAM_Read(FWEXT_LOCKNUM_TEST_EXIT_CNT);
	    }

	    if ( ProcessorThreadID() == 0 ) {
		extern void fwext_check_llb_firs();
		fwext_check_llb_firs();
	    }
	}

	test_detach();
    }
    else
    {
	// Try to print and terminate only for the first failure, but it
	// doesn't matter if a race causes us to do this more than one.
	static int test_already_failed = 0;
	if (!test_already_failed)
	{
	    test_already_failed = 1;
	    size_t cntIndex = BgGetRealLockIndex(FWEXT_LOCKNUM_TEST_EXIT_CNT);
	    printf("TEST_FAILURE: thread %d, status %d, "
		   "exited thread count %ld.\n",
		   ProcessorID(), status, BeDRAM_Read(cntIndex));
	    Terminate(status);
	    // NOTREACHED
	}
    }

    _Bg_Sleep_Forever();
    // NOTREACHED
}

void Bg_init_mc_ddr()
{

    /*
    13.9.1 Recommended Initialisation Sequence
    1.Apply power.  RESET# is low (active) via external pull-down resistor on 
      each channel (off-chip drivers will be disabled).  
    2.(Per DIMM socket) Read eeproms on DIMMs (via I2C) to find out what’s there.
    Compute required configuration and values, see Computations on page 331.
    3.Configure Prism PLL for target DDR3 clock speed (2X clock).  Enable PLL 
      to drive MC clock grids (one enable per MC).  Note: These PLL controls are 
      in Pervasive somewhere, not in MC or IOM.  Access to MCA registers is not 
      possible until the clock grid is running.  
    4.(Per MC) Load MCS registers to configure Pbus side of MC:
      Mandatory to load:
         MCFGP based on total number of channels in use and total memory on them
         MCFGC0 with organisation info for DIMM(s) on channel 0 of this MC
         MCFGC1 with organisation info for DIMM(s) on channel 1 of this MC
      Optional (the power-on defaults should work but may not be optimal):
        •MEPSILON pbus settings
        •MCBCFG pbus dataflow settings
        •MCCLMSK command list usage tuning
        •MCTL behavious settings
        •MCMODE behaviour settings
    5.(Per MC) Load MCA registers to configure dram side of MC:
      Mandatory to load:
        •MCADTAL overall latencies for writes (WDLAT) and reads (RDLAT)
        •MCTCNTL timing (TRRD TFAW TRC W_AS R_AS W_WD W_RS W_RD R_RD R_WS R_WD)
      Optional (the power-on defaults should work but may not be optimal):
        •MCAFIR select which errors should be masked and which enabled
        •MCAMISC enable opportunistic page mode and set its timings
        •MCAODT to configure sdram chips’ on-die-termination
    6.(Per Channel) Load IOM registers to configure ddr3 phy:
      Mandatory to load:
        •MASTER_CNTL_CONFIG_0 sets x4 or x8 chips, for each rank
        •RD_CAL_GLOBAL_DLY dqs gating and fifo settings
        •FIFO_CNTL to configure read data fifo to deliver data at correct cycle
      Optional (the power-on defaults should work but may not be optimal):
        •VREF_CNTL - external reference voltage tuning
        •INT_VREF - internal reference voltage tuning
        •IO_IMPEDANCE sets prism driver impedances
        •MASTER_CNTL_CONFIG_1 driver enable timing
        •MASTER_CNTL_ERR_MASK selects which phy errors cause an interrupt
        •ADR_n_p_DELAY defines ADR16 driver delays that may be required prior to Mode Register access.
    7.(Per Channel) Take DIMM out of reset:
        Write MCAPOS to enable command bus drivers to drive command signals to their inactive state, and drive RESET# active (low).
        Poll MCAPOS until “force” bits off (request completed).
        Insure at least 200 us has elapsed since valid power has been applied (ddr3 sdram requirement).
        Write MCAPOS to set RESET# high (inactive) while still enabling the command bus drivers.
        Poll MCAPOS until “force” bits off (request completed).
    8.(Per Channel) Enable sdram clock inputs:
        Wait at least 500 us (ddr3 sdram requirement)
        Write MCAPOS to set CKE high (active) while still enabling the command bus and RESET# high.
        Poll MCAPOS until “force” bits off (request completed).
        Wait for at least 300 ns (tXPR)
    9.(Per Rank) Perform initial delay line calibration operation:
        Write Rank number to MCAPOS.
        Write the IOM Calibration Control register (CAL_CNTL) to Issue the CPC 
          sequence, firmware may issue these two requests as seperate operations or 
          as a sequence by enabling both bits together for hardware to work through.  
          CPC Data 
          CPC Addr
        As each operation completes, the relevant Issue bit will be cleared by 
        hardware to indicate to firmware that the request is complete.  
    10.(Per Channel) Load sdram mode registers:
        (See Mode Register Values on page 335 for details of what values to write)
        Write MR2_pattern to MCAPOS, to load Mode register 2
        Poll MCAPOS until “force” bits off (request completed).
        Write MR3_pattern to MCAPOS, to load Mode register 3
        Poll MCAPOS until “force” bits off (request completed).
        Write MR1_pattern to MCAPOS, to load Mode register 1 and enable DLL
        Poll MCAPOS until “force” bits off (request completed).
        Write MR0_pattern to MCAPOS, to load Mode register 0 and reset DLL
        Poll MCAPOS until “force” bits off (request completed).
    11.(Per Rank) Perform Initial calibration operations:
        Write MR3_CAL_pattern to MCAPOS, to enable the multi-purpose register (MPR), for all ranks
        Poll MCAPOS until “force” bits off (request completed).
        Write the rank number to MCAPOS.
        Write the IOM Calibration Control register to Issue the desired Initial 
        calibration sequence, firmware may issue these requests as seperate 
        operations or as a sequence.  The sequence should be as follows: 
            •Read Calibration (DQS set to Fixed 90 degree shift)
            •Deskew
            •Read Eye
        As each operation completes, the relevant Issue bit will be cleared by hardware.
        Poll for completion.
        Write MR3_pattern to MCAPOS, to enable normal data flow from memory.
        Poll MCAPOS until “force” bits off (request completed).
    12.(Per Rank) Perform IOM controlled Write Leveling (Fine search):
        Write MR1_WL1_pattern to MCAPOS, to enable write leveling and disable output buffers for all ranks
        Poll MCAPOS until “force” bits off (request completed).
        Write MR1_WL2_pattern to MCAPOS, to enable output buffers for rank to be write leveled.
        Poll MCAPOS until “force” bits off (request completed).
        Write the rank number to MCAPOS.
        Write the IOM Calibration Control register (CAL_CNTL) to Issue a write leveling request:
            •Write Leveling
        Poll Write leveling Issue request bit for completion.
        Write MR1_WL3_pattern to MCAPOS, to disable write leveling and enable output buffers for all ranks.
        Poll MCAPOS until “force” bits off (request completed).
    13.(Per Rank) Perform firmware controlled Write Leveling (Coarse search).
        TBD
        [Ensure Initial calibration has completed.  Firmware will write a known 
        pattern into memory at a specified location, read back, if miscompares 
        increment delay on failing bytes via request to IOM register and repeat 
        write/readback sequence.  Repeat sequence with inverse pattern.] 
    14.(Per Rank) Perform Final calibration operations:
        Write MR3_CAL_pattern to MCAPOS, to enable the multi-purpose register (MPR), for all ranks
        Poll MCAPOS until “force” bits off (request completed).
        Write the rank number to MCAPOS.
        Write the IOM Calibration Control register (CAL_CNTL) to Issue the Final calibration sequence:
            •Deskew (optimisation)
            •CPC Data
            •CPC Addr
        As each operation completes, the relevant Issue bit will be cleared by hardware.
        Poll for completion.
        Write MR3_pattern to MCAPOS, to enable normal data flow from memory.
        Poll MCAPOS until“force” bits off (request completed).
    15.(Per MC) Periodic operations should be defined within the IOM Calibration Control register.
    16.(Per MC) Write MCZMRINT register to configure/enable periodic ZQ cal, MEMCAL, and refresh.
    17.(Per MC) Optionally perform memory test using hardware (MCMT* registers)
    18.(Per Prism) Optionally, perform memory test using firmware.
    19.  (Per MC) Optionally, load markstore registers MRKSTDTA0-MRKSTDTA15 
        with details of bad chips (per rank) found during memory test (or 
        otherwise known to be suspect), 
    20.Write good ECC to all locations in memory. May be done by:
        •(Per MC) Issuing "Fast Init" Maintenance Operation, or
        •Full-cacheline writes by firmware to every cacheline
    21.(Per MC) Write MCMACA/MCMEA/MCSCTL/MCMCT registers to enable scrubbing
    22.Memory is now available for normal use.
    */


    struct SCOM_adr_val_t{unsigned a; unsigned op; uint64_t v;};

    static struct SCOM_adr_val_t avp[] = 
    {
        #define DDRINIT(stepname, opcode, address, value) { address, opcode, value },
        #include <hwi/include/bqc/ddr_init.h>
        #undef DDRINIT
    };
    int i;
 


    // dork the dcr arbiter timeout value
    DCRWritePriv(DC_ARBITER_DCR(ARB_CTRL),0x0000000006010007 );

    for (i=0; i<sizeof(avp)/sizeof(avp[0]);++i)
    {
        
        if (avp[i].op == DDROP_DELAY)
        {
            uint64_t t = GetTimeBase() + avp[i].v;
            while (GetTimeBase() < t)
                ;
            continue;
        }

        DCRWritePriv(avp[i].a, avp[i].v);
        DCRWritePriv(avp[i].a + (DR_ARB_DCR__B_base-DR_ARB_DCR__A_base), avp[i].v);
        //printf("0x%08x: 0x%016llx\n", avp[i].a, avp[i].v);
    }

}


void fwext_check_llb_firs() {

    unsigned targetSlave = BGQCFAM_CoreToSlave(ProcessorCoreID());
    uint64_t fir0 = BgDcrPcbRead (BGQ_TI_SCOM_CFAM_A2SCOM_FIR0(targetSlave));
    uint64_t llbits = _BN(56) | _BN(57) | _BN(58) | _BN(59);

    if ( fir0 & llbits ) {

        uint64_t fir1 = BgDcrPcbRead (BGQ_TI_SCOM_CFAM_A2SCOM_FIR1(targetSlave));
        uint64_t pccr0 = BgDcrPcbRead (BGQ_TI_SCOM_CFAM_A2SCOM_PCCR0(targetSlave));
        uint64_t llcount = _G4(pccr0, 51);
	int overflow = ( _GN(fir1, 32) != 0 );

	printf("(E) fir0 = %llx, fir1 = %llx, pccr0 = %llx\n"
	       "(E) live lock buster count %c %lld\n", 
	       (unsigned long long) fir0, 
	       (unsigned long long) fir1, 
	       (unsigned long long) pccr0,
	       ( ( overflow ) ? '>' : '=' ),
	       ( ( overflow ) ? 16ull : llcount )
	    );
    }
}

