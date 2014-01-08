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

#ifndef __DDR_ECC_H__
#define __DDR_ECC_H__

#include <stdint.h>

class DdrEcc
{
public:
    /*!
    //     Take 64 bytes of bgq data and compute the ecc values to insert into bgq memory
    */
    static void EncodeBgq(unsigned char data[64], unsigned char checks_out[2][4]);

protected:
    enum {NSLICES = 8 };
    enum {DATABITS = 512};

    static void encode_prism(unsigned char data[64], unsigned char byte65, unsigned char ap, unsigned char checks[8]) ;
    static void encode_bgq(unsigned char data[64], unsigned char byte65, unsigned char ap, unsigned char checks[8]) ;
    static unsigned char* encode_byte_bgq(unsigned char byte, unsigned char checks[NSLICES], unsigned char *pcheckmat) ;
    static void encode_1bit(unsigned char checks[NSLICES], unsigned char *pcheckmat);
    static unsigned char* encode_byte(unsigned char byte, unsigned char checks[NSLICES], unsigned char *pcheckmat) ;
    static void replace_checkbits(unsigned char checks[]);
    static unsigned shiftbitx2(unsigned char abyte);

    static unsigned char data_checkmat[DATABITS*NSLICES];
    static unsigned char byte65_checkmat[8*NSLICES];
    static unsigned char byte65_checkmat_bgq[8*NSLICES];
    static unsigned char ap10_checkmat[NSLICES];
    static unsigned char ap01_checkmat[NSLICES];

private:


};


#endif

