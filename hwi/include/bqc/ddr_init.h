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
#ifndef DDROP_WRITE
#define DDROP_WRITE 1
#endif
#ifndef DDROP_DELAY
#define DDROP_DELAY 2
#endif

DDRINIT(MCFGP, DDROP_WRITE,      _DDR_MC_MCFGP(0), 
        _B10(9, 8*2))   // Configure total mem size (8GB, 2 ranks of 4GB), base addr.  size in 512MB

DDRINIT(MCFGC0, DDROP_WRITE,    _DDR_MC_MCFGC0(0), 
        // Configure channel 0
        // size in 512MB
        _B10(8, 8*2) |
        // Row/Col/Bnk 15/10/3
        _B4(12, 4) |
        // Rank 0 active
        _BN(13) |
        // Rank 1 active
        _BN(14))

DDRINIT(MCBCFG, DDROP_WRITE,    _DDR_MC_MCBCFG(0), 
        _B3(2,4))        // number of read data credits

DDRINIT(MCMISMODE, DDROP_WRITE, _DDR_MC_MCMISMODE(0), 
        _B4(7, 4)) // Configure channel 0

DDRINIT(MCECCDIS, DDROP_WRITE,  _DDR_MC_MCECCDIS(0), 
        0 /* | _BN(8) | _BN(9)*/)  // enable memory ECC (bit 8,9 = 0)

DDRINIT(MCADTAL, DDROP_WRITE,   _DDR_MC_MCADTAL(0),
        // Write latency, tbd
        _B8(7,2) |
        // Read latency, tbd
        _B8(15,8))  // disable memory ECC

DDRINIT(MCTCNTL, DDROP_WRITE, _DDR_MC_MCTCNTL(0), 
    _B5(4,4-4) |         // tRRD
    _B5(9,20-4) |        // tFAW
    _B6(15,33-4) |       // tRC
    _B6(21,33-4) |       // W_AS
    _B6(27,33-4) |       // R_AS
    _B5(32,6-4) |        // W_WD
    _B5(37,24-4) |       // W_RS
    _B5(42,4-4) |        // W_RD
    _B5(47,6-4) |        // R_RD
    _B5(52,8-4) |        // R_WS
    _B5(57,8-4))         // R_WD


DDRINIT(MCACFG0, DDROP_WRITE, _DDR_MC_MCACFG0(0), 
    _B8(7,0xc0))	// ranks 0 and 1 on dimm 0 ch 0 present


DDRINIT(MCACFG1, DDROP_WRITE, _DDR_MC_MCACFG1(0), 
    _BN(26))		// Write Queue Priority Enhanced Mode (only in DD2)


DDRINIT(MCTL, DDROP_WRITE, _DDR_MC_MCTL(0), 
    0x1800080000002000ULL)	// issue 1488


DDRINIT(_DDR_MC_MCAMISC, DDROP_WRITE, _DDR_MC_MCAMISC(0), 
    _B5(24,4-4) |	// R_RS
    _B5(29,4-4) |	// W_WS
    _B7(43,0) |		// NUM_IDLES
    _B9(52,256))	// NUM_CMDS_128_WINDOW


DDRINIT(IOM_PHYREGINDEX, DDROP_WRITE, _DDR_MC_IOM_PHYREGINDEX(0), 
    _B16(47,0xeffd) |
    _B16(63,0xff))       // IOM: configure x8 for all ranks


DDRINIT(IOM_PHYREGINDEX2, DDROP_WRITE, _DDR_MC_IOM_PHYREGINDEX(0)+0x40, 
    _B16(47,0xeffd) |
    _B16(63,0xff))       // IOM: configure x8 for all ranks


// IOM: FIFO delay
//{_DDR_MC_IOM_PHYREGINDEX(0),
//	_B16(47,0x6ffb) |
//	_B16(63,0x7f85)
//},


DDRINIT(IOM_READWRITELATENCY, DDROP_WRITE, DR_ARB_DCR(0, SCOM_IOM)+0xa, 
    _BN(44) |
    _B3(47,2) |
    _B5(55,15) |
    _B5(63,17))     // IOM: READ/write latency


DDRINIT(IOM_READWRITELATENCY2, DDROP_WRITE, DR_ARB_DCR(0, SCOM_IOM)+0xa+0x40, 
    _BN(44) |
    _B3(47,2) |
    _B5(55,15) |
    _B5(63,17))     // IOM: READ/write latency


DDRINIT(MCAPOS_EN, DDROP_WRITE, _DDR_MC_MCAPOS(0), 
    _BN(36) |
    _BN(37))    // enable driver, reset active

// wait 200us
DDRINIT(DELAY_RESET, DDROP_DELAY, 0, 200)

DDRINIT(MCAPOS_RESET, DDROP_WRITE, _DDR_MC_MCAPOS(0), 
    _BN(35) |
    _BN(36) |
    _BN(37))  // reset inactive


// clocks on
// wait 500us
DDRINIT(DELAY_CLOCKS, DDROP_DELAY, 0, 500)

DDRINIT(MCAPOS_ENABLECKE, DDROP_WRITE, _DDR_MC_MCAPOS(0),
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37))    // enable cke


DDRINIT(MR2, DDROP_WRITE, _DDR_MC_MCAPOS(0), 
    _BN(4) | // CWL=7

    _BN(17) | // MR2

    _BN(19) |
    _BN(20) |
    _BN(21) |
    _B8(30,0xff) |
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37))        // write MR2

DDRINIT(MR0_PRE_CLEAR, DDROP_WRITE, _DDR_MC_MCAPOS(0),
    _BN(4) | // CL=9
    _BN(6) | // CL=9
    _BN(9) | // WR=10
    _BN(11) | // WR=10

    _BN(19) |
    _BN(20) |
    _BN(21) |
    _B8(30,0xff) |
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37)) // write MR0

DDRINIT(MR3, DDROP_WRITE, _DDR_MC_MCAPOS(0), 
    _BN(16) | // MR3
    _BN(17) | // MR3

    _BN(19) |
    _BN(20) |
    _BN(21) |
    _B8(30,0xff) |
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37))   // write MR3


DDRINIT(MR1, DDROP_WRITE, _DDR_MC_MCAPOS(0),
    _BN(3) | // AL=CL-1

    _BN(16) | // MR1

    _BN(19) |
    _BN(20) |
    _BN(21) |
    _B8(30,0xff) |
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37))  //  write MR1


DDRINIT(MR0, DDROP_WRITE, _DDR_MC_MCAPOS(0),
    _BN(4) | // CL=9
    _BN(6) | // CL=9
    _BN(8) | // DLL reset
    _BN(9) | // WR=10
    _BN(11) | // WR=10

    _BN(19) |
    _BN(20) |
    _BN(21) |
    _B8(30,0xff) |
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37)) // write MR0


DDRINIT(ZQL, DDROP_WRITE, _DDR_MC_MCAPOS(0),
    _BN(10) | // A10 set
    _BN(21) | // WE active

    _B8(30,0xff) |
    _B4(34,0xf) |
    _BN(35) |
    _BN(36) |
    _BN(37))

DDRINIT(DQS1, DDROP_WRITE, _DDR_MC_IOM_PHYREGINDEX(0),
    0x000000007FF00002)

DDRINIT(DQS2, DDROP_WRITE, _DDR_MC_IOM_PHYREGINDEX(0) + 0x40,
        0x000000007FF00002)
