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


#include "BgEcc.h"

// for historical reasons, define these locally so as not to break builds or get name conflicts
#define _BN_ecc32_correct(b)    ((1<<(31-(b))))
#define _BN_ecc64_correct(b)    ((1ULL<<(63-(b))))



unsigned BgEcc::ecc64(uint64_t v)
{
    static uint64_t gen_mat[]=
    {
	0xff0000e8423c0f99ll,
	0x99ff0000e8423c0fll,
	0x0f99ff0000e8423cll,
	0x3c0f99ff0000e842ll,
	0x423c0f99ff0000e8ll,
	0xe8423c0f99ff0000ll,
	0x00e8423c0f99ff00ll,
	0x0000e8423c0f99ffll
    };
    unsigned ecc=0;
    uint64_t* m=gen_mat;
    for (int i=8;i;--i,++m)
    {
        uint64_t tl= v & *m;
        uint32_t t=((uint32_t)tl)^((uint32_t)(tl>>32));
        t^=t>>16;
        t^=t>>8;
        t^=t>>4;
        t^=t>>2;
        t^=t>>1;
        ecc<<=1;
        ecc|=(~t)&1;
    }
    return ecc;
}


unsigned BgEcc::ecc72(const ArbitraryLengthNumber &v)
{
    unsigned ecc0 = ecc64(v.getUint64(0,64-1));
    static uint8_t gen_mat[]=
    {
	0x9b,
	0xcd,
	0xe6,
	0x73,
	0xb9,
	0xdc,
	0x6e,
	0x37
    };
    unsigned ecc1=0;
    uint8_t* m=gen_mat;
    uint8_t x = v.getUint32(64,64+8-1);
    for (int i=8;i;--i,++m)
    {
        uint8_t t= x & *m;
        t^=t>>4;
        t^=t>>2;
        t^=t>>1;
        ecc1<<=1;
        ecc1|=(~t)&1;
    }
    return(ecc0 ^ ((~ecc1) & 0xFF));
 
}


/* --------------------------------------------------------
From Martin Ohmacht
6/6/2008

the matrix is:

--                  1111111111222222222233
--        01234567890123456789012345678901
------------------------------------------
-- e(0) | 11110000000010010101001000111110
-- e(1) | 11111111100000001000100100010001
-- e(2) | 10001111011110000000010010101000
-- e(3) | 01001000111111111100000001000101
-- e(4) | 00100100010001111011110000000010
-- e(5) | 00010010101001000111111111100000
-- e(6) | 00000001000100100010001111011111
------------------------------------------
-- e(0) | 0xf009523e
-- e(1) | 0xff808911
-- e(2) | 0x8f7804a8
-- e(3) | 0x48ffc045
-- e(4) | 0x2447bc02
-- e(5) | 0x12a47fe0
-- e(6) | 0x011223df

the transposed version is below (hex):

70
68
64
62
38
34
32
31
2a
1c
1a
19
58
0e
0d
4c
2c
4a
07
46
26
16
43
23
13
0b
52
61
51
49
45
29

If the syndrome matches any of the 32 elements above, you have a single 
bit error in the data field at the the position defined by the position 
of the match in the list.
If the syndrome has a single bit set, you have a single bit error in the ECC. 
If no bit is set, you have no error. In all other cases, you have a multi-bit error.

An ECC generating method would be


------------------------------------------- */
uint32_t  BgEcc::ecc32(uint32_t v)
{
    static uint32_t gen_mat[]=
    {
        0xf009523e,
        0xff808911,
        0x8f7804a8,
        0x48ffc045,
        0x2447bc02,
        0x12a47fe0,
        0x011223df
    };
    unsigned ecc=0;
    uint32_t* m=gen_mat;
    for (int i=7;i;--i,++m)
    {
        uint32_t t= v & *m;
        t^=t>>16;
        t^=t>>8;
        t^=t>>4;
        t^=t>>2;
        t^=t>>1;
        ecc<<=1;
        ecc|=(~t)&1;
    }
    return ecc;
}



uint32_t  BgEcc::ecc32_correct(uint32_t v, uint32_t ecc, uint32_t *rc  )
{
  // rc = 0 if no error, 1 if single bit corrected error, 2 if multibit error
  *rc = 0;
  unsigned syn = ecc32_syndrome(v,ecc);
  if ( syn == 0) return v;

  if ( ( syn == 0x01) || (syn == 0x02) || (syn == 0x04) || ( syn == 0x08) ||
       ( syn == 0x10) || (syn == 0x20) || (syn == 0x40) )
  {
    // single bit error in the ecc
    *rc = 1;
    return v;
  }
  
  static uint32_t syn_check[] =
  {
    0x70,0x68,0x64,0x62,0x38,0x34,0x32,0x31,
    0x2a,0x1c,0x1a,0x19,0x58,0x0e,0x0d,0x4c,
    0x2c,0x4a,0x07,0x46,0x26,0x16,0x43,0x23,
    0x13,0x0b,0x52,0x61,0x51,0x49,0x45,0x29
  };

  uint32_t i;
  uint32_t corrected_v;
  for ( i=0;i< 32;i++)
  {
    if ( syn == syn_check[i])
    {
      // single bit error in bit i
      *rc=1;
      corrected_v = ( v & ~_BN_ecc32_correct(i)) | (  ( ~v) & _BN_ecc32_correct(i) );
      return corrected_v;
    }
    
  }
  
  // double bit error
  *rc =2;
  return v;
  
  
  
}


uint64_t  BgEcc::ecc64_correct(uint64_t v, uint32_t ecc, uint32_t *rc  )
{
  // rc = 0 if no error, 1 if single bit corrected error, 2 if multibit error
  *rc = 0;
  uint32_t syn = ecc64_syndrome(v,ecc);
  if ( syn == 0) return v;

  if ( ( syn == 0x01) || (syn == 0x02) || (syn == 0x04) || ( syn == 0x08) ||
       ( syn == 0x10) || (syn == 0x20) || (syn == 0x40) || ( syn == 0x80) )
  {
    // single bit error in the ecc
    *rc = 1;
    return v;
  }
  // syn_check obtained as transpose of matrix in ddx/share/vhdl/sh_ecc64gen.vhdl
  // equivalent to computing ecc64(0), then computing syndromes of ecc64(_BN(i))
  static uint32_t syn_check[] =
  {
    0xc4, 0x8c, 0x94, 0xd0, 0xf4, 0xb0, 0xa8, 0xe0, 0x62, 0x46, 0x4a, 0x68, 0x7a, 0x58, 0x54, 0x70,
    0x31, 0x23, 0x25, 0x34, 0x3d, 0x2c, 0x2a, 0x38, 0x98, 0x91, 0x92, 0x1a, 0x9e, 0x16, 0x15, 0x1c,
    0x4c, 0xc8, 0x49, 0x0d, 0x4f, 0x0b, 0x8a, 0x0e, 0x26, 0x64, 0xa4, 0x86, 0xa7, 0x85, 0x45, 0x07,
    0x13, 0x32, 0x52, 0x43, 0xd3, 0xc2, 0xa2, 0x83, 0x89, 0x19, 0x29, 0xa1, 0xe9, 0x61, 0x51, 0xc1
  };

  uint32_t i;
  uint64_t corrected_v;
  for ( i=0;i< 64;i++)
  {
    if ( syn == syn_check[i])
    {
      // single bit error in bit i
      *rc=1;
      corrected_v = ( v & ~_BN_ecc64_correct(i)) | (  ( ~v) & _BN_ecc64_correct(i) );
      return corrected_v;
    }
    
  }
  
  // double bit error
  *rc =2;
  return v;
  

  
}



uint32_t  BgEcc::ecc64_correct(uint64_t v,   // original 64 bit data
			       uint32_t ecc, // original  ecc
			       uint64_t & corrected_v,  // corrected data
			       uint32_t & corrected_ecc) // corrected ecc 
{
  // rc = 0 if no error, 1 if single bit corrected error, 2 if multibit error
  // return the corrected data in corrected_v and corrected_ecc

  uint32_t   rc = 0;
  corrected_ecc = ecc;
  corrected_v = ecc64_correct(v,ecc,&rc);
  if( rc == 0) return rc;

  if ( ( rc == 1) && (corrected_v != v) )
  {
    // the error is in the data, the ecc is ok
    return 1;
  }
  
  if ( ( rc == 1) && (corrected_v == v) )
  {
    // the error is in the ecc, the data is ok
    corrected_ecc = ecc64(v);
    return 1;
  }
  
  // there is a UE, do not correct
  corrected_v = v;
  corrected_ecc = ecc;
  return rc;
  
}









uint32_t BgEcc::ecc65(uint64_t v, uint32_t bit64)
{
  
  // this is used on the interface of the L2 to the ddr arbiter
  unsigned ecc = ecc64(v);
  if ( bit64) ecc  ^= 0xc7;
  return ecc;
  
}

uint32_t  BgEcc::ecc65_correct(uint64_t v, uint32_t bit64, uint32_t ecc, 
			       uint64_t & corrected_v, uint32_t & corrected_bit64,uint32_t & corrected_ecc )
{
  // rc = 0 if no error, 1 if single bit corrected error, 2 if multibit error
  uint32_t rc = 0;
  corrected_v = v;
  corrected_bit64 = bit64;
  corrected_ecc = ecc;
  uint32_t syn = ecc65_syndrome(v,bit64,ecc);
  if ( syn == 0) return rc;

  if ( ( syn == 0x01) || (syn == 0x02) || (syn == 0x04) || ( syn == 0x08) ||
       ( syn == 0x10) || (syn == 0x20) || (syn == 0x40) || ( syn == 0x80) )
  {
    // single bit error in the ecc
    rc = 1;
    corrected_ecc = ecc65(v,bit64);
    return rc;
    
  }
// syn_check obtained as transpose of matrix in ddx/share/vhdl/sh_ecc64gen.vhdl
// equivalent to computing ecc65(0,0), then computing syndromes with 1 bit flipped
static uint32_t syn_check[] = 
{
 0xc4, 0x8c, 0x94, 0xd0, 0xf4, 0xb0, 0xa8, 0xe0, 0x62, 0x46, 0x4a, 0x68, 0x7a, 0x58, 0x54, 0x70,
 0x31, 0x23, 0x25, 0x34, 0x3d, 0x2c, 0x2a, 0x38, 0x98, 0x91, 0x92, 0x1a, 0x9e, 0x16, 0x15, 0x1c,
 0x4c, 0xc8, 0x49, 0x0d, 0x4f, 0x0b, 0x8a, 0x0e, 0x26, 0x64, 0xa4, 0x86, 0xa7, 0x85, 0x45, 0x07,
 0x13, 0x32, 0x52, 0x43, 0xd3, 0xc2, 0xa2, 0x83, 0x89, 0x19, 0x29, 0xa1, 0xe9, 0x61, 0x51, 0xc1,
 0xc7
};


  uint32_t i;

  for ( i=0;i< 64;i++)
  {
    if ( syn == syn_check[i])
    {
      // single bit error in bit i
      rc=1;
      if ( i< 64) corrected_v = ( v & ~_BN_ecc64_correct(i)) | (  ( ~v) & _BN_ecc64_correct(i) );
      return rc;
    }
    
  }

  if ( syn == syn_check[64] )
  {
    rc = 1;
    if (bit64 == 0) corrected_bit64 =1;
    else corrected_bit64 =0;
    return rc;
  }
  

  return 2;
  
 
}





  
