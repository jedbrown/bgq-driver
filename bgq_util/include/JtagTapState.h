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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef __JTAGTAPSTATE_H__
#define __JTAGTAPSTATE_H__
// 
// Jtag tap states external to the fpga mvserver platform.
//

struct JtagTapState {
    enum {
        TS_TLR              = 0,       
        TS_RTI              = 1,       
        TS_DR_SCAN          = 2,
        TS_CAPTURE_DR       = 3,
        TS_SHIFT_DR         = 4,
        TS_EXIT_DR          = 5,
        TS_PAUSE_DR         = 6,
        TS_EXIT2_DR         = 7,
        TS_UPDATE_DR        = 8,
        TS_IR_SCAN          = 9,
        TS_CAPTURE_IR       = 0xa,
        TS_SHIFT_IR         = 0xb,
        TS_EXIT_IR          = 0xc,
        TS_PAUSE_IR         = 0xd,
        TS_EXIT2_IR         = 0xe,
        TS_UPDATE_IR        = 0xf,
        TS_MAX_STATE        = 0xf, 
        TS_SIZE             = 0x10,
        TS_UNKNOWN          = ~0
    };
    
};
#endif


