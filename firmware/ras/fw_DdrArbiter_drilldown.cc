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

#include <sstream>
#include <iostream>
#include <vector>
#include <ras/include/RasEvent.h>
#include <hwi/include/bqc/dr_arb_dcr.h>
#include <hwi/include/bqc/ddr.h>

using namespace std;


#define GET_CHIP(mc,rank,dram) "U" << dec << ((dram + 1) / 10) << ((dram + 1) % 10) << \
                               ((rank == 0) ? "T" : (rank == 1) ? "B" : (rank == 2) ? "U" : "D") << \
                               (mc == 0 ? "R" : "L")


/* 200mar11 MJP Update for BGQ 144-bit bus */
/* C implementation of the FAST decoder vhdl */

namespace DdrDrilldown {

    unsigned char errlocs[73] = {
	/* "10000000" */ 0x80, /* "10100000" */ 0xA0, /* "10010000" */ 0x90, /* "11110000" */ 0xF0,
	/* "01100000" */ 0x60, /* "10110000" */ 0xB0, /* "11000000" */ 0xC0, /* "00100000" */ 0x20,
	/* "11100000" */ 0xE0, /* "00010000" */ 0x10, /* "01010000" */ 0x50, /* "11010000" */ 0xD0,
	/* "00001000" */ 0x08, /* "00001010" */ 0x0A, /* "00001001" */ 0x09, /* "00001111" */ 0x0F,
	/* "00000110" */ 0x06, /* "00001011" */ 0x0B, /* "00001100" */ 0x0C, /* "00000010" */ 0x02,
	/* "00001110" */ 0x0E, /* "00000001" */ 0x01, /* "00000101" */ 0x05, /* "00001101" */ 0x0D,
	/* "10011000" */ 0x98, /* "11011010" */ 0xDA, /* "10111001" */ 0xB9, /* "01111111" */ 0x7F,
	/* "11000110" */ 0xC6, /* "11111011" */ 0xFB, /* "00011100" */ 0x1C, /* "01000010" */ 0x42,
	/* "01011110" */ 0x5E, /* "00100001" */ 0x21, /* "10100101" */ 0xA5, /* "00111101" */ 0x3D,
	/* "10010001" */ 0x91, /* "11010111" */ 0xD7, /* "10110010" */ 0xB2, /* "01111000" */ 0x78,
	/* "11001010" */ 0xCA, /* "11110100" */ 0xF4, /* "00011101" */ 0x1D, /* "01000110" */ 0x46,
	/* "01011011" */ 0x5B, /* "00100011" */ 0x23, /* "10101111" */ 0xAF, /* "00111110" */ 0x3E,
	/* "00101000" */ 0x28, /* "11101010" */ 0xEA, /* "01001001" */ 0x49, /* "10011111" */ 0x9F,
	/* "11010110" */ 0xD6, /* "10001011" */ 0x8B, /* "00111100" */ 0x3C, /* "11000010" */ 0xC2,
	/* "11111110" */ 0xFE, /* "01100001" */ 0x61, /* "01110101" */ 0x75, /* "01011101" */ 0x5D,
	/* "10011010" */ 0x9A, /* "11010100" */ 0xD4, /* "10111101" */ 0xBD, /* "01110110" */ 0x76,
	/* "11001011" */ 0xCB, /* "11110011" */ 0xF3, /* "00011111" */ 0x1F, /* "01001110" */ 0x4E,
	/* "01010001" */ 0x51, /* "00100111" */ 0x27, /* "10100010" */ 0xA2, /* "00111000" */ 0x38,
	/* "11010011" */ 0xD3 };

    /* Chip kill identifiers, note that the chip number here is from left. */
    unsigned char chipmarks_inv[18] = {
	/*    Linv   L  */
        0x41 /*51*/,
        0x3A /*CB*/,
        0x7B /*9A*/,
        0x25 /*FE*/,
        0xD9 /*D6*/,
        0xFC /*28*/,
        0xA7 /*5B*/,
        0xB4 /*CA*/,
        0x13 /*91*/,
        0xED /*5E*/,
        0x8F /*C6*/,
        0x62 /*98*/,
        0x33 /*0E*/,
        0x77 /*06*/,
        0x44 /*08*/,
        0x60 /*E0*/,
        0xE0 /*60*/,
        0x80 /*80*/ };

    unsigned char cmult_u_inv(unsigned char c) {
	return 0x80*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6) )   /* prod(0) := c(0) xor c(1) */
	    + 0x40*(  ((c & 0x20)>>5)                   )   /* prod(1) := c(2)          */
	    + 0x20*(  ((c & 0x10)>>4)                   )   /* prod(2) := c(3);         */
	    + 0x10*(  ((c & 0x80)>>7)                   )   /* prod(3) := c(0);         */
	    + 0x08*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2) )   /* prod(4) := c(4) xor c(5);*/
	    + 0x04*(  ((c & 0x02)>>1)                   )   /* prod(5) := c(6);         */
	    + 0x02*(  ((c & 0x01)>>0)                   )   /* prod(6) := c(7);         */
	    + 0x01*(  ((c & 0x08)>>3)                   );  /* prod(7) := c(4);         */
    }

    unsigned char cmult_a1(unsigned char c) {
	return 0x80*(  ((c & 0x80)>>7) ^ ((c & 0x20)>>5)                   )  /* prod(0) := c(0) xor c(2)          */
	    + 0x40*(  ((c & 0x40)>>6) ^ ((c & 0x20)>>5) ^ ((c & 0x10)>>4) )  /* prod(1) := c(1) xor c(2) xor c(3) */
	    + 0x20*(  ((c & 0x80)>>7) ^ ((c & 0x20)>>5) ^ ((c & 0x10)>>4) )  /* prod(2) := c(0) xor c(2) xor c(3) */
	    + 0x10*(  ((c & 0x40)>>6) ^ ((c & 0x10)>>4)                   )  /* prod(3) := c(1) xor c(3)          */
	    + 0x08*(  ((c & 0x08)>>3) ^ ((c & 0x02)>>1)                   )  /* prod(4) := c(4) xor c(6)          */
	    + 0x04*(  ((c & 0x04)>>2) ^ ((c & 0x02)>>1) ^ ((c & 0x01)>>0) )  /* prod(5) := c(5) xor c(6) xor c(7) */
	    + 0x02*(  ((c & 0x08)>>3) ^ ((c & 0x02)>>1) ^ ((c & 0x01)>>0) )  /* prod(6) := c(4) xor c(6) xor c(7) */
	    + 0x01*(  ((c & 0x04)>>2) ^ ((c & 0x01)>>0)                   ); /* prod(7) := c(5) xor c(7)          */
    }

    unsigned char cmult_a1_2(unsigned char c) {
	return 0x80*(  ((c & 0x10)>>4)                   )  /* prod(0) := c(3)          */
	    + 0x40*(  ((c & 0x80)>>7) ^ ((c & 0x10)>>4) )  /* prod(1) := c(0) xor c(3) */
	    + 0x20*(  ((c & 0x40)>>6)                   )  /* prod(2) := c(1)          */
	    + 0x10*(  ((c & 0x20)>>5)                   )  /* prod(3) := c(2)          */
	    + 0x08*(  ((c & 0x01)>>0)                   )  /* prod(4) := c(7)          */
	    + 0x04*(  ((c & 0x08)>>3) ^ ((c & 0x01)>>0) )  /* prod(5) := c(4) xor c(7) */
	    + 0x02*(  ((c & 0x04)>>2)                   )  /* prod(6) := c(5)          */
	    + 0x01*(  ((c & 0x02)>>1)                   ); /* prod(7) := c(6)          */
    }

    unsigned char cmult_a2(unsigned char c) {
	return 0x80*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6) )  /* prod(0) := c(0) xor c(1) */
	    + 0x40*(  ((c & 0x20)>>5)                   )  /* prod(1) := c(2);         */
	    + 0x20*(  ((c & 0x10)>>4)                   )  /* prod(2) := c(3);         */
	    + 0x10*(  ((c & 0x80)>>7)                   )  /* prod(3) := c(0);         */
	    + 0x08*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2) )  /* prod(4) := c(4) xor c(5) */
	    + 0x04*(  ((c & 0x02)>>1)                   )  /* prod(5) := c(6);         */
	    + 0x02*(  ((c & 0x01)>>0)                   )  /* prod(6) := c(7);         */
	    + 0x01*(  ((c & 0x08)>>3)                   ); /* prod(7) := c(4);         */
    }

    unsigned char cmult_a2_2(unsigned char c) {
	return 0x80*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6) ^ ((c & 0x20)>>5) )  /* prod(0) := c(0) xor c(1) xor c(2) */
	    + 0x40*(  ((c & 0x10)>>4)                                     )  /* prod(1) := c(3);                  */
	    + 0x20*(  ((c & 0x80)>>7)                                     )  /* prod(2) := c(0);                  */
	    + 0x10*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6)                   )  /* prod(3) := c(0) xor c(1);         */
	    + 0x08*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2) ^ ((c & 0x02)>>1) )  /* prod(4) := c(4) xor c(5) xor c(6) */
	    + 0x04*(  ((c & 0x01)>>0)                                     )  /* prod(5) := c(7);                  */
	    + 0x02*(  ((c & 0x08)>>3)                                     )  /* prod(6) := c(4);                  */
	    + 0x01*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2)                   ); /* prod(7) := c(4) xor c(5);         */
    }

    unsigned char cmult_a3(unsigned char c) {
	return 0x80*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6) ^ ((c & 0x20)>>5) ^ ((c & 0x10)>>4) )  /* prod(0) := c(0) xor c(1) xor c(2) xor c(3) */
	    + 0x40*(  ((c & 0x80)>>7)                                                       )  /* prod(1) := c(0);                           */
	    + 0x20*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6)                                     )  /* prod(2) := c(0) xor c(1);                  */
	    + 0x10*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6) ^ ((c & 0x20)>>5)                   )  /* prod(3) := c(0) xor c(1) xor c(2);         */
	    + 0x08*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2) ^ ((c & 0x02)>>1) ^ ((c & 0x01)>>0) )  /* prod(4) := c(4) xor c(5) xor c(6) xor c(7);*/
	    + 0x04*(  ((c & 0x08)>>3)                                                       )  /* prod(5) := c(4);                           */
	    + 0x02*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2)                                     )  /* prod(6) := c(4) xor c(5);                  */
	    + 0x01*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2) ^ ((c & 0x02)>>1)                   ); /* prod(7) := c(4) xor c(5) xor c(6);         */
    }

    unsigned char cmult_a3_2(unsigned char c) {
	return 0x80*(  ((c & 0x40)>>6) ^ ((c & 0x10)>>4)                                     )  /*    prod(0) := c(1) xor c(3);                  */
	    + 0x40*(  ((c & 0x80)>>7) ^ ((c & 0x40)>>6) ^ ((c & 0x20)>>5) ^ ((c & 0x10)>>4) )  /*    prod(1) := c(0) xor c(1) xor c(2) xor c(3);*/
	    + 0x20*(  ((c & 0x40)>>6) ^ ((c & 0x20)>>5) ^ ((c & 0x10)>>4)                   )  /*    prod(2) := c(1) xor c(2) xor c(3);         */
	    + 0x10*(  ((c & 0x80)>>7) ^ ((c & 0x20)>>5) ^ ((c & 0x10)>>4)                   )  /*    prod(3) := c(0) xor c(2) xor c(3);         */
	    + 0x08*(  ((c & 0x04)>>2) ^ ((c & 0x01)>>0)                                     )  /*    prod(4) := c(5) xor c(7);                  */
	    + 0x04*(  ((c & 0x08)>>3) ^ ((c & 0x04)>>2) ^ ((c & 0x02)>>1) ^ ((c & 0x01)>>0) )  /*    prod(5) := c(4) xor c(5) xor c(6) xor c(7);*/
	    + 0x02*(  ((c & 0x04)>>2) ^ ((c & 0x02)>>1) ^ ((c & 0x01)>>0)                   )  /*    prod(6) := c(5) xor c(6) xor c(7);         */
	    + 0x01*(  ((c & 0x08)>>3) ^ ((c & 0x02)>>1) ^ ((c & 0x01)>>0)                   ); /*    prod(7) := c(4) xor c(6) xor c(7);         */
    }

/* multiplication over GF(256) */
    unsigned char mult256(unsigned char aa, unsigned char bb) {
	unsigned char a[8],b[8];
	unsigned char t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
	unsigned char t10,t11,t12,t13,t14,t15,t16,t17,t18,t19;
	unsigned char t20,t21,t22,t23,t24,t25,t26,t27,t28,t29;
	unsigned char t30,t31,t32,t33,t34,t35,t36,t37,t38,t39;
	unsigned char t40,t41,t42;

	a[0] = (aa & 0x80) >> 7;  b[0] = (bb & 0x80) >> 7;
	a[1] = (aa & 0x40) >> 6;  b[1] = (bb & 0x40) >> 6;
	a[2] = (aa & 0x20) >> 5;  b[2] = (bb & 0x20) >> 5;
	a[3] = (aa & 0x10) >> 4;  b[3] = (bb & 0x10) >> 4;
	a[4] = (aa & 0x8 ) >> 3;  b[4] = (bb & 0x8 ) >> 3;
	a[5] = (aa & 0x4 ) >> 2;  b[5] = (bb & 0x4 ) >> 2;
	a[6] = (aa & 0x2 ) >> 1;  b[6] = (bb & 0x2 ) >> 1;
	a[7] = (aa & 0x1 ) >> 0;  b[7] = (bb & 0x1 ) >> 0;

	t0  = b[0] ^ b[4];
	t1  = b[1] ^ b[5];
	t2  = b[2] ^ b[6];
	t3  = b[3] ^ b[7];
	t4  = a[0] ^ a[3];
	t5  = a[2] ^ a[3];
	t6  = a[2] ^ a[1];
	t7  = a[4] ^ a[7];
	t8  = a[6] ^ a[7];
	t9  = a[4] ^ a[5];
	t10 = a[6] ^ a[5];

	t11 = ((a[0] & b[0])^1) ^ ((a[3] & b[1])^1);
	t12 = ((a[2] & b[2])^1) ^ ((a[1] & b[3])^1);
	t13 = ((t9   & b[4])^1) ^ ((a[4] & b[5])^1);
	t14 = ((a[7] & b[6])^1) ^ ((a[6] & b[7])^1);

	t15 = ((a[1] & b[0])^1) ^ ((t4   & b[1])^1);
	t16 = ((t5   & b[2])^1) ^ ((t6   & b[3])^1);
	t17 = ((a[6] & b[4])^1) ^ ((a[5] & b[5])^1);
	t18 = ((t7   & b[6])^1) ^ ((t8   & b[7])^1);

	t19 = ((a[2] & b[0])^1) ^ ((a[1] & b[1])^1);
	t20 = ((t4   & b[2])^1) ^ ((t5   & b[3])^1);
	t21 = ((a[7] & b[4])^1) ^ ((a[6] & b[5])^1);
	t22 = ((a[5] & b[6])^1) ^ ((t7   & b[7])^1);

	t23 = ((a[3] & b[0])^1) ^ ((a[2] & b[1])^1);
	t24 = ((a[1] & b[2])^1) ^ ((t4   & b[3])^1);
	t25 = ((a[4] & b[4])^1) ^ ((a[7] & b[5])^1);
	t26 = ((a[6] & b[6])^1) ^ ((a[5] & b[7])^1);

	t27 = ((a[0] & b[4])^1) ^ ((a[3] & b[5])^1);
	t28 = ((a[2] & b[6])^1) ^ ((a[1] & b[7])^1);
	t29 = ((a[4] & t0  )^1) ^ ((a[7] & t1  )^1);
	t30 = ((a[6] & t2  )^1) ^ ((a[5] & t3  )^1);

	t31 = ((a[1] & b[4])^1) ^ ((t4  & b[5])^1);
	t32 = ((t5   & b[6])^1) ^ ((t6  & b[7])^1);
	t33 = ((a[5] & t0  )^1) ^ ((t7  & t1  )^1);
	t34 = ((t8   & t2  )^1) ^ ((t10 & t3  )^1);

	t35 = ((a[2] & b[4])^1) ^ ((a[1] & b[5])^1);
	t36 = ((t4   & b[6])^1) ^ ((t5   & b[7])^1);
	t37 = ((a[6] & t0  )^1) ^ ((a[5] & t1  )^1);
	t38 = ((t7   & t2  )^1) ^ ((t8   & t3  )^1);

	t39 = ((a[3] & b[4])^1) ^ ((a[2] & b[5])^1);
	t40 = ((a[1] & b[6])^1) ^ ((t4   & b[7])^1);
	t41 = ((a[7] & t0  )^1) ^ ((a[6] & t1  )^1);
	t42 = ((a[5] & t2  )^1) ^ ((t7   & t3  )^1);

	return  ( ((t11^t12) ^ (t13^t14) )&0x01)*0x80
	    + ( ((t15^t16) ^ (t17^t18) )&0x01)*0x40
	    + ( ((t19^t20) ^ (t21^t22) )&0x01)*0x20
	    + ( ((t23^t24) ^ (t25^t26) )&0x01)*0x10
	    + ( ((t27^t28) ^ (t29^t30) )&0x01)*0x08
	    + ( ((t31^t32) ^ (t33^t34) )&0x01)*0x04
	    + ( ((t35^t36) ^ (t37^t38) )&0x01)*0x02
	    + ( ((t39^t40) ^ (t41^t42) )&0x01)*0x01;
    }

    unsigned char square(unsigned char cc) {

	unsigned char c[8];

	c[0] = (cc & 0x80) >> 7;
	c[1] = (cc & 0x40) >> 6;
	c[2] = (cc & 0x20) >> 5;
	c[3] = (cc & 0x10) >> 4;
	c[4] = (cc & 0x08) >> 3;
	c[5] = (cc & 0x04) >> 2;
	c[6] = (cc & 0x02) >> 1;
	c[7] = (cc & 0x01) >> 0;

	return (c[0] ^ c[2] ^ c[4])*0x80
	    + (c[2] ^ c[5] ^ c[7])*0x40
	    + (c[1] ^ c[3] ^ c[7])*0x20
	    + (c[3] ^ c[4] ^ c[6])*0x10
	    + (c[4] ^ c[6]       )*0x08
	    + (c[6]              )*0x04
	    + (c[5] ^ c[7]       )*0x02
	    + (c[7]              )*0x01;
    }

/* Note the "DP" was just not(D) so leave it out here */
/* Result is 4 bits long */

    unsigned char norm256(unsigned char bb) {

	unsigned char t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
	unsigned char t10,t11,t12,t13,t14,t15,t16,t17,t18;

	unsigned char b[8];
	b[0] = (bb & 0x80) >> 7;
	b[1] = (bb & 0x40) >> 6;
	b[2] = (bb & 0x20) >> 5;
	b[3] = (bb & 0x10) >> 4;
	b[4] = (bb & 0x8 ) >> 3;
	b[5] = (bb & 0x4 ) >> 2;
	b[6] = (bb & 0x2 ) >> 1;
	b[7] = (bb & 0x1 ) >> 0;
	t0  = ( b[0] | b[4]) ^ 1;
	t1  = ( b[2] | b[7]) ^ 1;
	t2  = ( b[3] | b[7]) ^ 1;
	t3  = ( b[3] | b[4]) ^ 1;
	t4  = ( b[0] ^ b[3]) ^ 1;
	t5  =    b[0] ^   b[3];
	t6  =    b[2] ^   b[3];
	t7  =    b[4] ^   b[7];
	t8  =    b[6] ^ 1;
	t9  =    b[5] ^ 1;
	t10 =    b[1] ^ 1;

	t11   = t0 ^ (  (b[3] & b[5]) ^ 1 );
	t12   = ((b[2] & t8)^1) ^ ((b[1] & b[7])^1);

	t13   = t1 ^ ((t6 & b[6])^1);
	t14   = ((b[5] & t4)^1) ^ ((b[1] & t7)^1);

	t15   = t2 ^ ((t5 & b[6])^1);
	t16   = ((b[1] & t9)^1) ^ ((b[2] & t7)^1);

	t17   = t3 ^ ((b[2] & b[5])^1);
	t18   = ((t10 & b[6])^1) ^ ((t5 & b[7])^1);

	return  (( t11 ^ t12 )&0x01)*0x08
	    +(( t13 ^ t14 )&0x01)*0x04
	    +(( t15 ^ t16 )&0x01)*0x02
	    +(( t17 ^ t18 )&0x01)*0x01;
    }


/* Result is 7 bits long */
    unsigned char recip_norm256(unsigned char b) {

	unsigned char t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
	unsigned char t10,t11,t12,t13,t14,t15,t16,t17,t18,t19;
	unsigned char t20,t21,t22,t23,t24,t25,t26;
	unsigned char d[4],dp[4];
	unsigned char dd;

	dd = norm256(b);
	d[0]  = (dd & 0x08) >> 3;  dp[0] = (d[0]^1)&0x01;
	d[1]  = (dd & 0x04) >> 2;  dp[1] = (d[1]^1)&0x01;
	d[2]  = (dd & 0x02) >> 1;  dp[2] = (d[2]^1)&0x01;
	d[3]  = (dd & 0x01) >> 0;  dp[3] = (d[3]^1)&0x01;

	t0  = ( ((d[0] & d[1])^1) & dp[3] )^1;
	t1  = ( dp[0] &   dp[1] & dp[2])^1;
	t2  = ( dp[0] &   d[1] & d[2])^1;
	t3  = ( d[0]  &  d[1] & dp[2] & d[3])^1;

	t4  = ( ((dp[0] & dp[1])^1) & ((dp[2] | d[3])^1) )^1;
	t5  = ( dp[0] & d[2] & d[3])^1;
	t6  = ( d[0] & d[1] & dp[3])^1;
	t7  = ( dp[1] & dp[2] & d[3])^1;

	t8  = ( ((d[2] & d[3])^1) & ((dp[0] | dp[1])^1) )^1;
	t9  = (dp[0] & dp[2] & d[3])^1;
	t10 = (dp[0] & d[2] & dp[3])^1;
	t11 = (d[0]  & dp[1] & d[2] & d[3])^1;

	t12  = ( ((d[1] & d[2])^1) & dp[0] )^1;
	t13  = (d[1] & dp[2] & dp[3])^1;
	t14  = (dp[1] & d[2] & dp[3])^1;
	t15  = (d[0] & d[1] & d[2] & d[3])^1;

	t16  = (d[1]  & d[3])^1;
	t17  = (d[0]  &  dp[2]  &  dp[3])^1;
	t18  = (dp[0]  &  d[2]  &  d[3])^1;
	t19  = (dp[0]  &  d[1]  &  d[2])^1;

	t20  = ( d[0] & d[2] )^1;
	t21  = (dp[1] & d[2] & d[3])^1;
	t22  = (dp[0] & d[1] & dp[3])^1;
	t23  = (d[0]  & d[1] & d[3])^1;

	t24  = ( dp[1] & d[2] )^1;
	t25  = (dp[0]  &  d[1] & d[3])^1;
	t26  = (d[0]  &  dp[2] & d[3])^1;

	/* result is only 7 bits long!! */
	return (((t0  & t1  & t2  & t3)^1   )&0x01)*0x40
	    + (((t4  & t5  & t6  & t7)^1   )&0x01)*0x20
	    + (((t8  & t9  & t10 & t11)^1  )&0x01)*0x10
	    + (((t12 & t13 & t14 & t15)^1  )&0x01)*0x08
	    + (((t16 & t17 & t18 & t19)^1  )&0x01)*0x04
	    + (((t20 & t21 & t22 &  t23)^1 )&0x01)*0x02
	    + (((t24 & t25 & t26)^1        )&0x01)*0x01;
    }

/* returns 8 bits */
    unsigned char mult256_conjb(unsigned char aa, unsigned char bb) {

	unsigned char t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
	unsigned char t10,t11,t12,t13,t14,t15,t16,t17,t18,t19;
	unsigned char t20,t21,t22,t23,t24,t25,t26,t27,t28,t29;
	unsigned char t30,t31,t32,t33,t34,t35,t36,t37,t38,t39;
	unsigned char t40,t41,t42;
	unsigned char a[8],b[8];
	a[0] = (aa & 0x80) >> 7;  b[0] = (bb & 0x80) >> 7;
	a[1] = (aa & 0x40) >> 6;  b[1] = (bb & 0x40) >> 6;
	a[2] = (aa & 0x20) >> 5;  b[2] = (bb & 0x20) >> 5;
	a[3] = (aa & 0x10) >> 4;  b[3] = (bb & 0x10) >> 4;
	a[4] = (aa & 0x8 ) >> 3;  b[4] = (bb & 0x8 ) >> 3;
	a[5] = (aa & 0x4 ) >> 2;  b[5] = (bb & 0x4 ) >> 2;
	a[6] = (aa & 0x2 ) >> 1;  b[6] = (bb & 0x2 ) >> 1;
	a[7] = (aa & 0x1 ) >> 0;  b[7] = (bb & 0x1 ) >> 0;
	t0   = b[0] ^ b[4];
	t1   = b[1] ^ b[5];
	t2   = b[2] ^ b[6];
	t3   = b[3] ^ b[7];
	t4   = a[0] ^ a[3];
	t5   = a[2] ^ a[3];
	t6   = a[2] ^ a[1];
	t7   = a[4] ^ a[7];
	t8   = a[6] ^ a[7];
	t9   = a[4] ^ a[5];
	t10  = a[6] ^ a[5];

	t11 = ((a[0] & t0)^1)  ^ ((a[3] & t1)^1);
	t12 = ((a[2] & t2)^1)  ^ ((a[1] & t3)^1);
	t13 = ((t9   & b[4])^1) ^ ((a[4] & b[5])^1);
	t14 = ((a[7] & b[6])^1) ^ ((a[6] & b[7])^1);
	t15 = ((a[1] & t0)^1)   ^ ((t4   & t1)^1);
	t16 = ((t5   & t2)^1)   ^ ((t6   & t3)^1);
	t17 = ((a[6] & b[4])^1) ^ ((a[5] & b[5])^1);
	t18 = ((t7   & b[6])^1) ^ ((t8   & b[7])^1);
	t19 = ((a[2] & t0)^1)   ^ ((a[1] & t1)^1);
	t20 = ((t4   & t2)^1)   ^ ((t5   & t3)^1);
	t21 = ((a[7] & b[4])^1) ^ ((a[6] & b[5])^1);
	t22 = ((a[5] & b[6])^1) ^ ((t7   & b[7])^1);
	t23 = ((a[3] & t0)^1)   ^ ((a[2] & t1)^1);
	t24 = ((a[1] & t2)^1)   ^ ((t4   & t3)^1);
	t25 = ((a[4] & b[4])^1) ^ ((a[7] & b[5])^1);
	t26 = ((a[6] & b[6])^1) ^ ((a[5] & b[7])^1);
	t27 = ((a[0] & b[4])^1) ^ ((a[3] & b[5])^1);
	t28 = ((a[2] & b[6])^1) ^ ((a[1] & b[7])^1);
	t29 = ((a[4] & b[0])^1) ^ ((a[7] & b[1])^1);
	t30 = ((a[6] & b[2])^1) ^ ((a[5] & b[3])^1);
	t31 = ((a[1] & b[4])^1) ^ ((t4   & b[5])^1);
	t32 = ((t5   & b[6])^1) ^ ((t6   & b[7])^1);
	t33 = ((a[5] & b[0])^1) ^ ((t7   & b[1])^1);
	t34 = ((t8   & b[2])^1) ^ ((t10  & b[3])^1);
	t35 = ((a[2] & b[4])^1) ^ ((a[1] & b[5])^1);
	t36 = ((t4   & b[6])^1) ^ ((t5   & b[7])^1);
	t37 = ((a[6] & b[0])^1) ^ ((a[5] & b[1])^1);
	t38 = ((t7   & b[2])^1) ^ ((t8   & b[3])^1);
	t39 = ((a[3] & b[4])^1) ^ ((a[2] & b[5])^1);
	t40 = ((a[1] & b[6])^1) ^ ((t4   & b[7])^1);
	t41 = ((a[7] & b[0])^1) ^ ((a[6] & b[1])^1);
	t42 = ((a[5] & b[2])^1) ^ ((t7   & b[3])^1);

	return (( (t11 ^ t12)   ^ (t13  ^ t14) )&0x01)*0x80
	    + (( (t15 ^ t16)   ^ (t17  ^ t18) )&0x01)*0x40
	    + (( (t19 ^ t20)   ^ (t21  ^ t22) )&0x01)*0x20
	    + (( (t23 ^ t24)   ^ (t25  ^ t26) )&0x01)*0x10
	    + (( (t27 ^ t28)   ^ (t29  ^ t30) )&0x01)*0x08
	    + (( (t31 ^ t32)   ^ (t33  ^ t34) )&0x01)*0x04
	    + (( (t35 ^ t36)   ^ (t37  ^ t38) )&0x01)*0x02
	    + (( (t39 ^ t40)   ^ (t41  ^ t42) )&0x01)*0x01;
    }

/* 8 bits for aa,bb and 8 bits for result */
    unsigned char multsquare( unsigned char a, unsigned char b) {

	unsigned char a0,a1,a2,a3,a4,a5,a6,a7;
	unsigned char b0,b1,b2,b3,b4,b5,b6,b7;
	unsigned char c0,c1,c2,c3,c4,c5,c6,c7;
	unsigned char p0,p1,p2,p3,p4,p5;
	unsigned char r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11;
	unsigned char s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10;
	unsigned char s11,s12,s13,s14,s15,s16,s17,s18,s19,s20;
	unsigned char t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
	unsigned char t10,t11,t12,t13,t14,t15,t16,t17,t18,t19;
	unsigned char t20,t21,t22,t23,t24,t25,t26,t27,t28,t29;
	unsigned char t30,t31,t32,t33,t34,t35,t36,t37,t38,t39;
	unsigned char t40,t41,t42,t43,t44,t45,t46,t47;

	a0 = (a & 0x80) >> 7;   b0 = (b & 0x80) >> 7;
	a1 = (a & 0x40) >> 6;   b1 = (b & 0x40) >> 6;
	a2 = (a & 0x20) >> 5;   b2 = (b & 0x20) >> 5;
	a3 = (a & 0x10) >> 4;   b3 = (b & 0x10) >> 4;
	a4 = (a & 0x08) >> 3;   b4 = (b & 0x08) >> 3;
	a5 = (a & 0x04) >> 2;   b5 = (b & 0x04) >> 2;
	a6 = (a & 0x02) >> 1;   b6 = (b & 0x02) >> 1;
	a7 = (a & 0x01) >> 0;   b7 = (b & 0x01) >> 0;

	p0 = a2 ^ a6;
	p1 = a0 ^ a4;
	p2 = a1 ^ a5;
	p3 = a1 ^ a3;
	p4 = a2 ^ a7;
	p5 = a3 ^ a4;

	r0 = b5 ^ b7;
	r1 = b4 ^ b6;
	r2 = b0 ^ b2;
	r3 = b2 ^ b5;
	r4 = b1 ^ b7;
	r5 = b1 ^ b3;
	r6 = b0 ^ b6;
	r7 = b1 ^ b5;
	r8 = b1 ^ b6;
	r9 = b2 ^ b7;
	r10 = b4 ^ b7;
	r11 = b0 ^ b3;

	s0 = a0 ^ a3;
	s1 = a1 ^ a2;
	s2 = a3 ^ a7;
	s3 = a2 ^ a3;
	s4 = a4 ^ a7;
	s5 = p0 ^ p1;
	s6 = a0 ^ a2;
	s7 = a5 ^ a6;
	s8 = a0 ^ a5;
	s9 = a6 ^ a7;
	s10 = a5 ^ a7;
	s11 = p2 ^ a6;
	s12 = a4 ^ p0;
	s13 = p1 ^ p3;
	s14 = a4 ^ a6;
	s15 = p0 ^ a7;
	s16 = p2 ^ p4;
	s17 = a4 ^ a5;
	s18 = a1 ^ a6;
	s19 = p0 ^ p5;
	s20 = a7 ^ p5;

	t0 = ((a0 & b0)^1) ^ ((a2 & b1)^1);
	t1 = ((s0 & b2)^1) ^ ((s1 & b3)^1);
	t2 = ((p1 & b4)^1) ^ ((s2 & r0)^1);
	t3 = ((p2 & r1)^1) ^ ((p0 & b7)^1);
	t4 = t0 ^ t1;
	t5 = t2 ^ t3;
	c0 = t4 ^ t5;

	t6 = ((a1 & r2)^1) ^ ((s3 & b1)^1);
	t7 = ((s0 & r3)^1) ^ ((p3 & b3)^1);
	t8 = ((p0 & r1)^1) ^ ((s4 & b5)^1);
	t9 = ((p2 & b6)^1) ^ ((s5 & b7)^1);
	t10 = t6  ^ t7;
	t11 = t8  ^ t9;
	c1 = t10 ^ t11;

	t12 = ((a2 & b0)^1) ^ ((s0 & r4)^1);
	t13 = ((s1 & b2)^1) ^ ((s6 & b3)^1);
	t14 = ((s2 & r1)^1) ^ ((p2 & r0)^1);
	t15 = ((p0 & b6)^1) ^ ((s4 & b7)^1);
	t16 = t12 ^ t13;
	t17 = t14 ^ t15;
	c2 = t16 ^ t17;

	t18 = ((a3 & b0)^1) ^ ((a1 & r5)^1);
	t19 = ((s3 & b2)^1) ^ ((s0 & b3)^1);
	t20 = ((p1 & r1)^1) ^ ((p0 & r0)^1);
	t21 = ((s2 & b6)^1) ^ ((p2 & b7)^1);
	t22 = t18 ^ t19;
	t23 = t20 ^ t21;
	c3 = t22 ^ t23;

	t24 = ((a4 & r6)^1) ^ ((a6 & r7)^1);
	t25 = ((s4 & b2)^1) ^ ((s7 & b3)^1);
	t26 = ((s8 & r1)^1) ^ ((p4 & r0)^1);
	t27 = ((s2 & b6)^1) ^ ((p2 & b7)^1);
	t28 = t24 ^ t25;
	t29 = t26 ^ t27;
	c4 = t28 ^ t29;

	t30 = ((a5 & r2)^1) ^ ((s9 & r8)^1);
	t31 = ((s4 & r9)^1) ^ ((s10 & b3)^1);
	t32 = ((s11 & r10)^1) ^ ((s12 & b5)^1);
	t33 = ((s13 & b6)^1)  ^ ((a3 & r0)^1);
	t34 = t30 ^ t31;
	t35 = t32 ^ t33;
	c5 = t34 ^ t35;

	t36 = ((a6 & b0)^1) ^ ((s4 & r7)^1);
	t37 = ((s7 & b2)^1) ^ ((s14 & b3)^1);
	t38 = ((s15 & r10)^1) ^ ((a3 & b5)^1);
	t39 = ((s16 & b6)^1)  ^ ((s8 & r0)^1);
	t40 = t36 ^ t37;
	t41 = t38 ^ t39;
	c6 = t40 ^ t41;

	t42 = ((a7 & r11)^1) ^ ((a5 & r7)^1);
	t43 = ((s9 & b2)^1)  ^ ((s17 & b3)^1);
	t44 = ((s20 & r10)^1) ^ ((s18 & r0)^1);
	t45 = ((s19 & b6)^1)  ^ ((a0 & b7)^1);
	t46 = t42 ^ t43;
	t47 = t44 ^ t45;
	c7 = t46 ^ t47;

	return 0x80*(c0 & 0x01)
	    + 0x40*(c1 & 0x01)
	    + 0x20*(c2 & 0x01)
	    + 0x10*(c3 & 0x01)
	    + 0x08*(c4 & 0x01)
	    + 0x04*(c5 & 0x01)
	    + 0x02*(c6 & 0x01)
	    + 0x01*(c7 & 0x01);
    }


/* e is 7 bits, f and result are 8 bits */
    unsigned char div256_part2 ( unsigned char ee, unsigned char ff ) {

	unsigned char t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;
	unsigned char t10,t11,t12,t13,t14,t15;
	unsigned char e[7];
	unsigned char f[8];

	e[0] = (ee & 0x40) >> 6;  f[0] = (ff & 0x80) >> 7;
	e[1] = (ee & 0x20) >> 5;  f[1] = (ff & 0x40) >> 6;
	e[2] = (ee & 0x10) >> 4;  f[2] = (ff & 0x20) >> 5;
	e[3] = (ee & 0x8 ) >> 3;  f[3] = (ff & 0x10) >> 4;
	e[4] = (ee & 0x4 ) >> 2;  f[4] = (ff & 0x8 ) >> 3;
	e[5] = (ee & 0x2 ) >> 1;  f[5] = (ff & 0x4 ) >> 2;
	e[6] = (ee & 0x1 ) >> 0;  f[6] = (ff & 0x2 ) >> 1;
	f[7] = (ff & 0x1 ) >> 0;

	t0   = ((e[0] & f[0])^1) ^ ((e[3] & f[1])^1);
	t1   = ((e[2] & f[2])^1) ^ ((e[1] & f[3])^1);
	t2   = ((e[1] & f[0])^1) ^ ((e[4] & f[1])^1);
	t3   = ((e[5] & f[2])^1) ^ ((e[6] & f[3])^1);
	t4   = ((e[2] & f[0])^1) ^ ((e[1] & f[1])^1);
	t5   = ((e[4] & f[2])^1) ^ ((e[5] & f[3])^1);
	t6   = ((e[3] & f[0])^1) ^ ((e[2] & f[1])^1);
	t7   = ((e[1] & f[2])^1) ^ ((e[4] & f[3])^1);
	t8   = ((e[0] & f[4])^1) ^ ((e[3] & f[5])^1);
	t9   = ((e[2] & f[6])^1) ^ ((e[1] & f[7])^1);
	t10  = ((e[1] & f[4])^1) ^ ((e[4] & f[5])^1);
	t11  = ((e[5] & f[6])^1) ^ ((e[6] & f[7])^1);
	t12  = ((e[2] & f[4])^1) ^ ((e[1] & f[5])^1);
	t13  = ((e[4] & f[6])^1) ^ ((e[5] & f[7])^1);
	t14  = ((e[3] & f[4])^1) ^ ((e[2] & f[5])^1);
	t15  = ((e[1] & f[6])^1) ^ ((e[4] & f[7])^1);

	return 0x80*( (t0  ^ t1 ) & 0x01)
	    + 0x40*( (t2  ^ t3 ) & 0x01)
	    + 0x20*( (t4  ^ t5 ) & 0x01)
	    + 0x10*( (t6  ^ t7 ) & 0x01)
	    + 0x08*( (t8  ^ t9 ) & 0x01)
	    + 0x04*( (t10 ^ t11) & 0x01)
	    + 0x02*( (t12 ^ t13) & 0x01)
	    + 0x01*( (t14 ^ t15) & 0x01);

    }

/* All 8 bits */
    unsigned char div256( unsigned char num, unsigned char den) {
	unsigned char e_out;    /* 7 bits */
	unsigned char f_out;    /* 8 bits */

	f_out = mult256_conjb(num,den);  /* returns 8 bits */
	e_out = recip_norm256(den);      /* returns 7 bits */

	return div256_part2(e_out,f_out);
    }


/* aa is 8 bits, *proot1 and *proot2 are 8 bits too. */
    void quadroot256( unsigned char aa,
		      unsigned char* pnosol,
		      unsigned char* proot1,
		      unsigned char* proot2) {

	unsigned char r[8];
	unsigned char a[8];
	a[0] = (aa & 0x80) >> 7;
	a[1] = (aa & 0x40) >> 6;
	a[2] = (aa & 0x20) >> 5;
	a[3] = (aa & 0x10) >> 4;
	a[4] = (aa & 0x8 ) >> 3;
	a[5] = (aa & 0x4 ) >> 2;
	a[6] = (aa & 0x2 ) >> 1;
	a[7] = (aa & 0x1 ) >> 0;

	(*pnosol) = a[7];

	r[1] = a[0] ^ a[1] ^ a[3] ^ a[6];
	r[2] = a[0] ^ a[3] ^ a[4];
	r[3] = a[1] ^ a[2] ^ a[4] ^ a[5];
	r[4] = a[3] ^ a[4];
	r[5] = a[4] ^ a[5];
	r[6] = a[4];
	r[7] = a[5] ^ a[6];

	*proot1 = 0x80 * ( 0    & 0x01 )
	    + 0x40 * ( r[1] & 0x01 )
	    + 0x20 * ( r[2] & 0x01 )
	    + 0x10 * ( r[3] & 0x01 )
	    + 0x08 * ( r[4] & 0x01 )
	    + 0x04 * ( r[5] & 0x01 )
	    + 0x02 * ( r[6] & 0x01 )
	    + 0x01 * ( r[7] & 0x01 );
	*proot2 = 0x80 * ( 1    & 0x01 )
	    + 0x40 * ( r[1] & 0x01 )
	    + 0x20 * ( r[2] & 0x01 )
	    + 0x10 * ( r[3] & 0x01 )
	    + 0x08 * ( r[4] & 0x01 )
	    + 0x04 * ( r[5] & 0x01 )
	    + 0x02 * ( r[6] & 0x01 )
	    + 0x01 * ( r[7] & 0x01 );
    }


/* Return symbol position if exists, else -1 */
/* Do NOT try to match the virtual symbol 72 */
    int sym_mask(unsigned char r) {
	int i;
	int mask;
	mask = -1;
	for (i=0; i<72; i++) {
	    if (r == errlocs[i]) mask=71-i;
	}
	return mask;
    }

/* Return chip   position if exists, else -1 */
/* Do NOT try to match the virtual symbol 72 */
    int chip_mask(unsigned char r) {
	int i;
	int mask;
	mask = -1;
	for (i=0; i<18; i++) {
	    if (r == errlocs[i*4]) mask=17-i;
	}
	return mask;
    }

    unsigned long vandinv (unsigned long sp_in) {
	unsigned long sp[32];
	unsigned long v[32];
	unsigned long result;
	int i;
	for (i=0; i<32; i++)
	    if (((0x80000000L>>i) & sp_in) != 0) sp[i]=1; else sp[i]=0; /* Bit 0 on left, bit 31 on right */
	v[0]  = sp[1] ^ sp[ 9] ^ sp[17] ^ sp[24] ^ sp[25];
	v[1]  = sp[1] ^ sp[ 2] ^ sp[ 9] ^ sp[10] ^ sp[17] ^ sp[18] ^ sp[26];
	v[2]  = sp[2] ^ sp[ 3] ^ sp[10] ^ sp[11] ^ sp[18] ^ sp[19] ^ sp[27];
	v[3]  = sp[0] ^ sp[ 3] ^ sp[ 8] ^ sp[11] ^ sp[16] ^ sp[19] ^ sp[24];
	v[4]  = sp[5] ^ sp[13] ^ sp[21] ^ sp[28] ^ sp[29];
	v[5]  = sp[5] ^ sp[ 6] ^ sp[13] ^ sp[14] ^ sp[21] ^ sp[22] ^ sp[30];
	v[6]  = sp[6] ^ sp[ 7] ^ sp[14] ^ sp[15] ^ sp[22] ^ sp[23] ^ sp[31];
	v[7]  = sp[4] ^ sp[ 7] ^ sp[12] ^ sp[15] ^ sp[20] ^ sp[23] ^ sp[28];
	v[8]  = sp[1] ^ sp[ 3] ^ sp[11] ^ sp[16] ^ sp[18] ^ sp[24] ^ sp[25] ^ sp[26];
	v[9]  = sp[0] ^ sp[ 1] ^ sp[ 2] ^ sp[ 3] ^ sp[ 8] ^ sp[11] ^ sp[17] ^ sp[18] ^ sp[19] ^ sp[27];
	v[10] = sp[1] ^ sp[ 2] ^ sp[ 3] ^ sp[ 9] ^ sp[16] ^ sp[18] ^ sp[19] ^ sp[24];
	v[11] = sp[0] ^ sp[ 2] ^ sp[ 3] ^ sp[10] ^ sp[17] ^ sp[19] ^ sp[24] ^ sp[25];
	v[12] = sp[5] ^ sp[ 7] ^ sp[15] ^ sp[20] ^ sp[22] ^ sp[28] ^ sp[29] ^ sp[30];
	v[13] = sp[4] ^ sp[ 5] ^ sp[ 6] ^ sp[ 7] ^ sp[12] ^ sp[15] ^ sp[21] ^ sp[22] ^ sp[23] ^ sp[31];
	v[14] = sp[5] ^ sp[ 6] ^ sp[ 7] ^ sp[13] ^ sp[20] ^ sp[22] ^ sp[23] ^ sp[28];
	v[15] = sp[4] ^ sp[ 6] ^ sp[ 7] ^ sp[14] ^ sp[21] ^ sp[23] ^ sp[28] ^ sp[29];
	v[16] = sp[1] ^ sp[ 2] ^ sp[ 8] ^ sp[ 9] ^ sp[11] ^ sp[16] ^ sp[18] ^ sp[27];
	v[17] = sp[1] ^ sp[ 3] ^ sp[ 8] ^ sp[10] ^ sp[11] ^ sp[17] ^ sp[18] ^ sp[19] ^ sp[24] ^ sp[27];
	v[18] = sp[0] ^ sp[ 2] ^ sp[ 9] ^ sp[11] ^ sp[16] ^ sp[18] ^ sp[19] ^ sp[25];
	v[19] = sp[0] ^ sp[ 1] ^ sp[ 3] ^ sp[ 8] ^ sp[10] ^ sp[17] ^ sp[19] ^ sp[26];
	v[20] = sp[5] ^ sp[ 6] ^ sp[12] ^ sp[13] ^ sp[15] ^ sp[20] ^ sp[22] ^ sp[31];
	v[21] = sp[5] ^ sp[ 7] ^ sp[12] ^ sp[14] ^ sp[15] ^ sp[21] ^ sp[22] ^ sp[23] ^ sp[28] ^ sp[31];
	v[22] = sp[4] ^ sp[ 6] ^ sp[13] ^ sp[15] ^ sp[20] ^ sp[22] ^ sp[23] ^ sp[29];
	v[23] = sp[4] ^ sp[ 5] ^ sp[ 7] ^ sp[12] ^ sp[14] ^ sp[21] ^ sp[23] ^ sp[30];
	v[24] = sp[0] ^ sp[ 1] ^ sp[ 2] ^ sp[ 3] ^ sp[ 8] ^ sp[17] ^ sp[26] ^ sp[27];
	v[25] = sp[0] ^ sp[ 9] ^ sp[17] ^ sp[18] ^ sp[24] ^ sp[26];
	v[26] = sp[0] ^ sp[ 1] ^ sp[10] ^ sp[18] ^ sp[19] ^ sp[24] ^ sp[25] ^ sp[27];
	v[27] = sp[0] ^ sp[ 1] ^ sp[ 2] ^ sp[11] ^ sp[16] ^ sp[19] ^ sp[25] ^ sp[26];
	v[28] = sp[4] ^ sp[ 5] ^ sp[ 6] ^ sp[ 7] ^ sp[12] ^ sp[21] ^ sp[30] ^ sp[31];
	v[29] = sp[4] ^ sp[13] ^ sp[21] ^ sp[22] ^ sp[28] ^ sp[30];
	v[30] = sp[4] ^ sp[ 5] ^ sp[14] ^ sp[22] ^ sp[23] ^ sp[28] ^ sp[29] ^ sp[31];
	v[31] = sp[4] ^ sp[ 5] ^ sp[ 6] ^ sp[15] ^ sp[20] ^ sp[23] ^ sp[29] ^ sp[30];
	result=0;
	for (i=0; i<32; i++)
	{
	    result = result + (v[i] << (31-i));
	}
	return result;
    }

    int  sym2wire(int bgq, int pos) {
	int wire;
	if (bgq) {
	    if      (pos<32) wire=pos*2;     /* 0-31     0-63   */
	    else if (pos<64) wire=pos*2+8;   /* 32-63    72-135 */
	    else if (pos<68) wire=pos*2-64;  /* 64-67    64-71  */
	    else             wire=pos*2;     /* 68-71    136-143*/
	} else {
	    wire=pos;
	}
	return wire;
    }


/* ========================================== */
/* 2-ERROR DECODING                           */
/* ========================================== */

    int decode_2err( unsigned char* syndrome,
		     unsigned char* pe1,
		     int*           ppos1,
		     unsigned char* pe2,
		     int*           ppos2  ) {
	unsigned char UE1,UE2,UE3,UE4,UE5,UE;
	unsigned char s0,s1,s2,s3,s4,s5,s6,s1xs3,s0xs3,s0xs2,s1xs2;
	unsigned char a0,a1,a2;
	unsigned char et2,et4,et7;     /* Only 7 bits */
	unsigned char t0,t5,t6,t8,t9,t10,t11;
	unsigned char e1,e2,r1,r2;
	unsigned char w0,w1,w2,w3,w4,w5,w6,w7,w8,w9,w10,w11,w12,w13,w14,w15;
	unsigned char y1,y2;
	unsigned char e11,e12;
	unsigned char ct5;
	int           pos1,pos2;

	s0 = syndrome[0]; /* Split syndrome in to individual bytes */
	s1 = syndrome[1];
	s2 = syndrome[2];
	s3 = syndrome[3];
	s4 = syndrome[4];
	s5 = syndrome[5];
	s6 = syndrome[6];

	s1xs3 = mult256(s1,s3);
	s0xs3 = mult256(s0,s3);
	s0xs2 = mult256(s0,s2);
	s1xs2 = mult256(s1,s2);

	a0 = s1xs3 ^ square(s2);
	a1 = s0xs3 ^ s1xs2;
	a2 = s0xs2 ^ square(s1);

	t0 = mult256(a0,a2);
	t6 = square(a1);

	et2 = recip_norm256(a1);  /* result is 7 bits */
	et4 = recip_norm256(a2);  /* result is 7 bits */

	t5 = mult256_conjb(a2,a1);
	t8 = mult256_conjb(t0,t6);

	UE1 = (a0 == 0) || (a1 == 0) || (a2 == 0);

	w0 = s3 ^ s4;
	w1 = w0 ^ s2;
	w2 = w0 ^ s5;
	w3 = s4 ^ s5 ^ s6;
	w4 = a0 ^ a1;
	w5 = a0 ^ a2;
	w6 = a1 ^ a2;

	w7 = mult256(s3,w4);
	w8 = mult256(s4,w5);
	w9 = mult256(s5,w6);
	w10= mult256(w1,a0);
	w11= mult256(w2,a1);
	w12= mult256(w3,a2);
	w13= w7 ^ w8 ^ w10;
	w14= w7 ^ w9 ^ w11;
	w15= w8 ^ w9 ^ w12;

	UE3 = (w13!=0) || (w14!=0) || (w15!=0);

	/* et2 is 7 bits long, so is et7 */
	et7=0x40*( ((et2 & 0x40)>>6) ^ ((et2 & 0x10)>>4) )                     /* et2(0) xor et2(2)  */
	    +0x20*( ((et2 & 0x10)>>4)                     )                     /* et2(2)             */
	    +0x10*( ((et2 & 0x20)>>5) ^ ((et2 & 0x08)>>3) )                     /* et2(1) xor et2(3)  */
	    +0x08*( ((et2 & 0x08)>>3)                     )                     /* et2(3)             */
	    +0x04*( ((et2 & 0x40)>>6) ^ ((et2 & 0x10)>>4) ^ ((et2 & 0x08)>>3) ) /* et2(0) xor et2(2) xor et2(3) */
	    +0x02*( ((et2 & 0x20)>>5)                     )                     /* et2(1)             */
	    +0x01*( ((et2 & 0x10)>>4) ^ ((et2 & 0x20)>>5) ^ ((et2 & 0x08)>>3) );/* et2(2) xor et2(1) xor et2(3) */

	t9 = div256_part2(et7,t8);
	t10= div256_part2(et2,t5);

	quadroot256(t9, &UE2,&y1,&y2);

	e11 = mult256(y2,s0);
	e12 = mult256(t10,s1);

	ct5 = ( (t5 & 0xF0) ^ ((t5 & 0x0F)<<4) )   /* ct5(0 to 3) <= t5(0 to 3) xor t5(4 to 7); */
	    + ( (t5 & 0x0F)                    );  /* ct5(4 to 7) <= t5(4 to 7);                */

	t11 = div256_part2(et4,ct5);               /* et4 is 7 bits */

	r1  = mult256(t11,y1);

	e1 = e11 ^ e12;                            /* Error pattern 1 */
	e2 = e1  ^ s0;                             /* Error pattern 2 */
	r2 = t11 ^ r1;

	pos1 = sym_mask(r1);                       /* bit position 0-71 or -1 if none */
	pos2 = sym_mask(r2);                       /* bit position 0-71 or -1 if none */

	if ((r1 == errlocs[72]) && ((e1 & 0xEE)==0)) pos1 = 72; /* Indicates hashing error */
	if ((r2 == errlocs[72]) && ((e2 & 0xEE)==0)) pos2 = 72; /* Indicates hashing error */

	UE4 = (pos1 == -1);
	UE5 = (pos2 == -1);

	UE = UE1 | UE2 | UE3 | UE4 | UE5;

	*pe1   = e1;
	*ppos1 = pos1;
	*pe2   = e2;
	*ppos2 = pos2;

	return UE;
    }

/* ========================================== */
/* SINGLE / CHIPKILL DECODING                 */
/* ========================================== */

    int decode_chipkill( unsigned char* syndrome,
			 int            chipmark, /* 0..17 chip number from left, or -1 for none */
			 unsigned char* pe,
			 int*           ppos,
			 unsigned char* pe1,
			 unsigned char* pe2,
			 unsigned char* pe3,
			 unsigned char* pe4,
			 int*           pcpos,
			 ostringstream& details
	) {

	unsigned char s0,s1,s2,s3,s4,s5,s6,sh0,sh1,sh2;                /* 8 bits */
	unsigned char sp0,sp1,sp2,sp3,sp4,sp5,sp6;                     /* 8 bits */
	unsigned char t20,t25,t26,t27,t28,t29,t30,t31,t32,t33,t34;     /* 8 bits */
	unsigned char L,linv1,linv2,linv3,linv5;                       /* 8 bits */
	unsigned char v1,v2,v3,v4,rp;                                  /* 8 bits */
	unsigned char t39,t40,t41,t42,pre_e;                           /* 7 bits */
	unsigned char te,e,e1,e2,e3,e4,r;                              /* 8 bits */
	unsigned char w16;                                             /* 8 bits */
	unsigned char w17,w18,w19,w20,w21,w22,w23,w24,w25,w26; /* 1 bit  */
	unsigned char UE6,UE7,UE8,UE9,UE10,UE11,UE12,UE13,UE;          /* 1 bit  */
	unsigned long int sp,v;                                        /* 32 bits */
	int      pos,cpos;                                             /* positions */
	unsigned char linv1_conj;

	if (chipmark==-1) {
	    linv1 = 0xD2;
	} else if ((chipmark<0) || (chipmark>17)) {
	    details << "CHIP: Invalid chipmark!!";
		return 1;
	} else {
	    linv1 = chipmarks_inv[chipmark];
	}
	linv1_conj = linv1 ^ ((linv1 & 0x0F)<<4);
	pre_e      = recip_norm256(linv1);
	L          = div256_part2(pre_e,linv1_conj);
	linv2      = square(linv1);
	linv3      = mult256(linv1,linv2);
	linv5      = multsquare(linv1,linv2);

	s0 = syndrome[0];
	s1 = syndrome[1];
	s2 = syndrome[2];
	s3 = syndrome[3];
	s4 = syndrome[4];
	s5 = syndrome[5];
	s6 = syndrome[6];

	sp0 = s0;
	sp1 = mult256(s1,linv1);
	sp2 = mult256(s2,linv2);
	sp3 = mult256(s3,linv3);
	sp4 = multsquare(s4,linv2);
	sp5 = mult256(s5,linv5);
	sp6 = multsquare(s6,linv3);

	sh0 = cmult_u_inv(sp0 ^ sp4) ^ sp0 ^ sp3;
	sh1 = cmult_u_inv(sp1 ^ sp5) ^ sp1 ^ sp4;
	sh2 = cmult_u_inv(sp2 ^ sp6) ^ sp2 ^ sp5;

	t20 = square(sh0);

	sp  = sp0;           /* Use  unsigned long  as 32 bits here */
	sp  = (sp<<8) | sp1;
	sp  = (sp<<8) | sp2;
	sp  = (sp<<8) | sp3; /* 32 bits sp0 & sp1 & sp2 & sp3 */
	v   = vandinv(sp);   /* output 32 bits */

	w20 = ((L == 0x6E) ? 1 : 0); /* Test for default chipmark */

	v1  = (v & 0xFF000000) >> 24;
	v2  = (v & 0x00FF0000) >> 16;
	v3  = (v & 0x0000FF00) >>  8;
	v4  = (v & 0x000000FF) >>  0;

	w16 = mult256(sh0,sh2) ^ square(sh1);

	UE6 = (w16==0) ? 0 : 1; /* UE6 <= or_reduce(w16) */
	w17 = (sh0==0) ? 0 : 1; /* w17 <= or_reduce(sh0) */
	w18 = (sh2==0) ? 0 : 1; /* w18 <= or_reduce(sh2) */
	UE7 = w17 ^ w18;

	t25 = mult256_conjb(t20,sh0);       /* Returns 8 bits */
	t26 = mult256_conjb(t20,sh1);
	t27 = t25 ^ t26;
	t31 = sh0 ^ sh1;

	t28 = cmult_a1(t25)   ^ t26;
	t32 = cmult_a1_2(sh0) ^ cmult_a1(sh1);

	t29 = cmult_a2(t25)   ^ t26;
	t33 = cmult_a2_2(sh0) ^ cmult_a2(sh1);

	t30 = cmult_a3(t25)   ^ t26;
	t34 = cmult_a3_2(sh0) ^ cmult_a3(sh1);

	t39 = recip_norm256(t31);  /* 7 bits output */
	t40 = recip_norm256(t32);  /* 7 bits output */
	t41 = recip_norm256(t33);  /* 7 bits output */
	t42 = recip_norm256(t34);  /* 7 bits output */

	if ((t31!=0) && (t32!=0) && (t33!=0) && (t34!=0)) w19=0; else w19=1; /* w19 <= not(or_reduce(t31) and or_reduce(t32) and or_reduce(t33) and or_reduce(t34));*/

	UE8 = w17 & w19;

	rp = div256(sh1,sh0);

	r = mult256(L,rp);

	e1 = div256_part2(t39,t27) ^ v1; /* 1st arg 7 bits, 2nd arg 8 bits */
	e2 = div256_part2(t40,t28) ^ v2; /* These are the chipkill correct pattern */
	e3 = div256_part2(t41,t29) ^ v3; /* Note that left wire is e4 and right wire is e1 */
	e4 = div256_part2(t42,t30) ^ v4;

	te  = e1 ^ e2 ^ e3 ^ e4;
	w21 = ((e1==0) ? 0 : 1); /* w21 <= or_reduce(e1); */
	w22 = ((e2==0) ? 0 : 1); /* w22 <= or_reduce(e2); */
	w23 = ((e3==0) ? 0 : 1); /* w23 <= or_reduce(e3); */
	w24 = ((e4==0) ? 0 : 1); /* w24 <= or_reduce(e4); */
	w25 = ( ((w21 | w22)^1) & ((w23 | w24)^1) )^1; /* OR of all 4 */

	UE9 = w20 & w25;

	e   = s0 ^ te;       /* This is the single error pattern */

	UE10= ((e==0) ? 0 : 1) & (w17^1);

	pos = sym_mask(r);   /* -1 if no match */
	cpos= chip_mask(L);  /* -1 if none */

	if ((r == errlocs[72]) && ((e & 0xEE)==0)) pos = 72; /* Indicates hashing error */

	w26 = ((pos==-1) ? 0 : 1); /* 0 if no match  (or_reduce(tmaskr(0 to 71));  */

	UE11 = ((w17^1) | w26)^1;

	/* w27 = (symmark == 0) ? 1 : 0;               No sym mark in Prism        */
	/* w28 = (symmark == r) ? 1 : 0;                                           */
	/* UE12= (!w20) & (!w27) & w17 & (!w28);                                   */

	UE12= 0;

	/* UE13 only applies to BGQ, for Prism SingleWireMode should be always '0' */
	/* w30 <= OR_REDUCE(e(0 to 3)); -- h1(e) is not 0                          */
	/* w31 <= OR_REDUCE(e(4 to 7)); -- h2(e) is not 0                          */
	/* single_wire_check <= singleWireMode and not(w20) and w27;               */
	/* UE13 <= single_wire_check and w30 and w31;                              */

	UE13 = 0;

	UE = UE6 | UE7 | UE8 | UE9 | UE10 | UE11 | UE12 | UE13;

	*pe   = e;
	*ppos = pos;
	*pe1  = e1;
	*pe2  = e2;
	*pe3  = e3;
	*pe4  = e4;
	*pcpos= cpos;

	return UE;
    }

    int reverse( int i ) {
	return (0x80*(( i & 0x01)>>0))
	    + (0x40*(( i & 0x02)>>1))
	    + (0x20*(( i & 0x04)>>2))
	    + (0x10*(( i & 0x08)>>3))
	    + (0x08*(( i & 0x10)>>4))
	    + (0x04*(( i & 0x20)>>5))
	    + (0x02*(( i & 0x40)>>6))
	    + (0x01*(( i & 0x80)>>7));
    }

    void ddr_drilldown(unsigned long errsyn, unsigned long mc, unsigned long rank, ostringstream& details ) {

    unsigned char syndrome[7];
    int i,bgq,regfmt,chip_rc,err2_rc,b;
    unsigned char e,e1,e2,e3,e4;
    int pos,cpos,pos1,pos2,mark,corr=0;

    regfmt=1;
    bgq=1;

    for (i=0; i<7; i++) {
	b=(errsyn >> (7-i)*8) & 0xFF;
	if (regfmt==1) {
	    syndrome[i]=reverse(b);
	}
	else {
	    syndrome[i]=b;
	}
    }

    err2_rc=decode_2err(syndrome,&e1,&pos1,&e2,&pos2); /* 2err has priority if it can find a solution */
    if (err2_rc==0) {
	if (pos1==72 || pos2==72) {
	    details << " 2ERR: Uncorrectable (address hash error): ";
	} else {
	    corr=1;
            if ((e1&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos1) / 8) << ".DQ" << sym2wire(1, pos1) % 8;
            if ((e1&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos1) / 8) << ".DQ" << sym2wire(1, pos1) % 8 + 1;
            if ((e2&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos2) / 8) << ".DQ" << sym2wire(1, pos2) % 8;
            if ((e2&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos2) / 8) << ".DQ" << sym2wire(1, pos2) % 8 + 1;
	}
    } 
    else {

	chip_rc=decode_chipkill(syndrome,-1,&e,&pos,&e1,&e2,&e3,&e4,&cpos,details);

	if (chip_rc==0) {

	    if ((pos==72)&&(e!=0)) {
		details << " CHIP: Uncorrectable (address hash error) with no mark: ";
	    } 
	    else {
		corr=1;
		if ((e &0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos) / 8)          << ".DQ" << sym2wire(1, pos) % 8;
		if ((e &0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos) / 8)          << ".DQ" << sym2wire(1, pos) % 8 + 1;
		if ((e1&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 0) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 0) % 8;
		if ((e1&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 0) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 0) % 8 + 1;
		if ((e2&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 1) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 1) % 8;
		if ((e2&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 1) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 1) % 8 + 1;
		if ((e3&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 2) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 2) % 8;
		if ((e3&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 2) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 2) % 8 + 1;
		if ((e4&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 3) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 3) % 8;
		if ((e4&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 3) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 3) % 8 + 1;
	    }
	} 
	else {

	    for (mark=0; mark<18; mark++) {
		chip_rc=decode_chipkill(syndrome,mark,&e,&pos,&e1,&e2,&e3,&e4,&cpos,details);
		if (chip_rc==0) {

		    if ((pos==72)&&(e!=0)) {
			details << " CHIP: Uncorrectable (address hash error) IF mark on chip " << sym2wire(bgq,mark*4) << ":" << (sym2wire(bgq,mark*4+3)+bgq);
		    } 
		    else {
			corr=1;
	                if ((e &0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos) / 8)          << ".DQ" << sym2wire(1, pos) % 8;
	                if ((e &0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, pos) / 8)          << ".DQ" << sym2wire(1, pos) % 8 + 1;
	                if ((e1&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 0) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 0) % 8;
	                if ((e1&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 0) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 0) % 8 + 1;
	                if ((e2&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 1) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 1) % 8;
	                if ((e2&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 1) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 1) % 8 + 1;
	                if ((e3&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 2) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 2) % 8;
	                if ((e3&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 2) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 2) % 8 + 1;
	                if ((e4&0xF0)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 3) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 3) % 8;
	                if ((e4&0x0F)!=0) details << " Chip=" << GET_CHIP(mc, rank, sym2wire(1, cpos * 4 + 3) / 8) << ".DQ" << sym2wire(1, cpos * 4 + 3) % 8 + 1;
		    }
		}
	    }
	}
    }
    
    if (corr==0) details << " Cannot be identified, because the error is uncorrectable.";
    
    }
}    

using namespace DdrDrilldown;
    
extern "C" {


    void fw_DdrArbiter_drilldown( RasEvent& event, const vector<uint64_t>& mbox ) {

	ostringstream details;

	if ( mbox.size() < 3 ) {
	    details << "INTERNAL ERROR: missing details.";
	}
	else {



	    uint64_t unit    = mbox[0];
	    uint64_t erraddr = mbox[1];
	    uint64_t errsyn  = mbox[2];

	    uint64_t rank = (unsigned)((erraddr>>60)&0x7);

	    details 
		<< "DDR" << unit 
		<< " Rank=" << rank
		<< " Bank=" << (unsigned)((erraddr>>53)&0x7)
		<< " Row="  << (unsigned)((erraddr>>37)&0xFFFF)
		<< " Col="  << (unsigned)((erraddr>>28)&0x1FF);

	    ddr_drilldown( errsyn, unit, rank, details );
	}

	event.setDetail( "DETAILS", details.str() );
    }


    void fw_DdrArbiter_badDramDecoder( RasEvent& event, const vector<uint64_t>& mbox ) {

	ostringstream details;

	uint64_t i  = mbox[0];
	uint64_t rr = mbox[1];
	uint64_t j  = mbox[2];

	details << " MC" << i << " Rank=" << rr << " Chip=" << GET_CHIP(i, rr, j);

	event.setDetail( "DETAILS", details.str() );
    }

}


