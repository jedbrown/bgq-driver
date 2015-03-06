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
 * \file math/bgp/collective_network/DblUtils.h
 * \brief ???
 */

#ifndef __math_bgp_collective_network_DblUtils_h__
#define __math_bgp_collective_network_DblUtils_h__

/* ******************************************************************** */
/*                  asm macros for processing exponents                 */
/* ******************************************************************** */

/**
 * \brief Number of bits needed to pad carry for full-system SUM
 *
 * Offset needed, in bits, to ensure that a full-system SUM does not
 * overflow. Basically, 2^EXPO_OFFSET is the max number of "ranks"
 * that can participate in a 2-Pass DOUBLE-SUM operation.
 */
#define EXPO_OFFSET	17

/**
 * \brief Value needed in 2-Pass pre-processing to adjust exponent
 *
 * This is EXPO_OFFSET minus 11 exponent bits and 1 sign bit, which
 * are bits that are being stripped-off the mantissa and can be used
 * for carries. But we need 1 bit for the implied "1" and also a bit
 * to ensure we maintain the sign, so we add back 2.
 */
#define EXPO_PRE_OFFSET	(EXPO_OFFSET - 11 - 1 + 1 + 1)

/**
 * \brief Value needed in 2-Pass post-processing to adjust exponent
 *
 * This is EXPO_OFFSET minus 1 "implied-1" bit, which will be removed.
 * This value is used to shift the result in order to nortmalize it.
 * Since we had to add a sign bit and the implied "1", we must also
 * shift those here, so need to add back 2.
 */
#define EXPO_POST_OFFSET	(EXPO_OFFSET - 1 + 1 + 1)


#define LDDH(r,pkt) \
asm volatile ("lwzu %0,8(%1)" : "=b"(r), "+Ob" (pkt) :: "memory");
#define STSI(r,pkt) \
asm volatile ("sthu %1,2(%0)" : "+Ob" (pkt) : "b"(r) : "memory");

#define EXPMASK(r) \
asm volatile("rlwinm %0,%0,12,21,31;": "+b"(r) ); \



/* pre process one exp for sending */
#define EXPPRE1E(dbl,exp) \
{ \
   LDDH(r, dbl); EXPMASK(r); STSI(r, exp); \
}


/* pre process one quad (8 exponents)  */
#define EXPPREQ(dbl,exp) \
{ \
  register unsigned ir0, ir1, ir2, ir3; \
  register unsigned ir4, ir5, ir6, ir7; \
  LDDH(ir0, dbl); LDDH(ir1, dbl); LDDH(ir2, dbl); LDDH(ir3, dbl); \
  LDDH(ir4, dbl); LDDH(ir5, dbl); LDDH(ir6, dbl); LDDH(ir7, dbl); \
  EXPMASK(ir0); EXPMASK(ir1); EXPMASK(ir2); EXPMASK(ir3); \
  STSI(ir0, exp); STSI(ir1, exp); STSI(ir2, exp); STSI(ir3, exp); \
  EXPMASK(ir4); EXPMASK(ir5); EXPMASK(ir6); EXPMASK(ir7); \
  STSI(ir4, exp); STSI(ir5, exp); STSI(ir6, exp); STSI(ir7, exp); \
}


/* pre process 6 exponents */
#define EXPPRE6E(dbl,exp) \
{ \
  register unsigned ir0, ir1, ir2, ir3; \
  register unsigned ir4, ir5, ir6, ir7; \
  LDDH(ir0, dbl); LDDH(ir1, dbl); LDDH(ir2, dbl); LDDH(ir3, dbl); \
  LDDH(ir4, dbl); LDDH(ir5, dbl); \
  EXPMASK(ir0); EXPMASK(ir1); EXPMASK(ir2); EXPMASK(ir3); \
  STSI(ir0, exp); STSI(ir1, exp); STSI(ir2, exp); STSI(ir3, exp); \
  EXPMASK(ir4); EXPMASK(ir5); \
  STSI(ir4, exp); STSI(ir5, exp);\
}

/* pre process 8 exponents */
#define EXPPRE8E(dbl,exp) \
{ \
  register unsigned ir0, ir1, ir2, ir3; \
  register unsigned ir4, ir5, ir6, ir7; \
  LDDH(ir0, dbl); LDDH(ir1, dbl); LDDH(ir2, dbl); LDDH(ir3, dbl); \
  LDDH(ir4, dbl); LDDH(ir5, dbl); \
  EXPMASK(ir0); EXPMASK(ir1); EXPMASK(ir2); EXPMASK(ir3); \
  STSI(ir0, exp); STSI(ir1, exp); STSI(ir2, exp); STSI(ir3, exp); \
  EXPMASK(ir4); EXPMASK(ir5); \
  STSI(ir4, exp); STSI(ir5, exp); \
  LDDH(ir0, dbl); LDDH(ir1, dbl); \
  EXPMASK(ir0); EXPMASK(ir1); \
  STSI(ir0, exp); STSI(ir1, exp); \
}


/* pre process 3 quads (24 exponenents) */
#define EXPPRE3Q(dbl,exp) \
{ \
  register unsigned ir0, ir1, ir2, ir3; \
  register unsigned ir4, ir5, ir6, ir7; \
  register unsigned ir8, ir9, irA, irB; \
  register unsigned irC, irD, irE, irF; \
  LDDH(ir0, dbl); LDDH(ir1, dbl); LDDH(ir2, dbl); LDDH(ir3, dbl); \
  LDDH(ir4, dbl); LDDH(ir5, dbl); LDDH(ir6, dbl); LDDH(ir7, dbl); \
  EXPMASK(ir0); EXPMASK(ir1); EXPMASK(ir2); EXPMASK(ir3); \
  LDDH(ir8, dbl); LDDH(ir9, dbl); LDDH(irA, dbl); LDDH(irB, dbl); \
  EXPMASK(ir4); EXPMASK(ir5); EXPMASK(ir6); EXPMASK(ir7); \
  STSI(ir0, exp); STSI(ir1, exp); STSI(ir2, exp); STSI(ir3, exp); \
  EXPMASK(ir8); EXPMASK(ir9); EXPMASK(irA); EXPMASK(irB); \
  LDDH(irC, dbl); LDDH(irD, dbl); LDDH(irE, dbl); LDDH(irF, dbl); \
  STSI(ir4, exp); STSI(ir5, exp); STSI(ir6, exp); STSI(ir7, exp); \
  EXPMASK(irC); EXPMASK(irD); EXPMASK(irE); EXPMASK(irF); \
  LDDH(ir0, dbl); LDDH(ir1, dbl); LDDH(ir2, dbl); LDDH(ir3, dbl); \
  STSI(ir8, exp); STSI(ir9, exp); STSI(irA, exp); STSI(irB, exp); \
  EXPMASK(ir0); EXPMASK(ir1); EXPMASK(ir2); EXPMASK(ir3); \
  LDDH(ir4, dbl); LDDH(ir5, dbl); LDDH(ir6, dbl); LDDH(ir7, dbl); \
  STSI(irC, exp); STSI(irD, exp); STSI(irE, exp); STSI(irF, exp); \
  EXPMASK(ir4); EXPMASK(ir5); EXPMASK(ir6); EXPMASK(ir7); \
  STSI(ir0, exp); STSI(ir1, exp); STSI(ir2, exp); STSI(ir3, exp); \
  STSI(ir4, exp); STSI(ir5, exp); STSI(ir6, exp); STSI(ir7, exp); \
}

/* ******************************************************************** */
/*                  asm macros for processing mantissas                 */
/* ******************************************************************** */

#define LDI(r,pkt) \
asm volatile ("lwzu %0,4(%1)" : "=b"(r), "+Ob" (pkt) :: "memory" );

#define LDH(r,pkt) \
asm volatile ("lhzu %0,2(%1)" : "=b"(r), "+Ob" (pkt) :: "memory" );

#define STI(r,pkt) \
asm volatile ("stwu %1,4(%0)" : "+Ob" (pkt) : "b"(r) : "memory");


/* ----------------------------------------------------------------------- */
/*                                    pre-process a mantissa, 64 bits wide */
/* ----------------------------------------------------------------------- */

/*  Params
 *  mh   (inout): in: double high word, out: man high word
 *  ml   (inout): man low  word
 *  maxe (in)   : max exp
 *  r1,r2,r3    : work registers used internally by the macro
 */

/* algorithm skeleton: dobule d = [mh,ml]  ( []: concatenation )
 *  N  = maxe - exp(d) + 5;
 *  mh = man_high_word_mask(mh);
 *  if ( exp(d) != 0 )
 *    set_implicit_first_1(mh);
 *  [mh,ml] = [mh,ml] >> N;
 *  if ( d < 0 )
 *    [mh,ml] = - [mh,ml];
 */
#define MAN64PRECORE(mh,ml,maxe,r1,r2,r3) \
  asm volatile( "rlwinm  %[R2],%[MH],12,21,31;\n\t" /* R2 = exp(m)           */\
                "addi    %[N],%[NE],%[XOFF];\n\t"   /* N = maxe + 5          */\
                "cmpwi   4,%[R2], 0;\n\t"           /* check: exp(m) ? 0     */\
                "rlwinm. %[R1],%[MH],0,0,0;\n\t"    /* R1 = sign(m), cr0 !   */\
                "subf    %[N], %[R2], %[N];\n\t"    /* N = maxe + 5 - exp(m) */\
                "rlwinm  %[MH],%[MH],0,12,31;\n\t"  /* mh = man_high(m)      */\
                "cmpwi   7, %[N], 64;\n\t"          /* check: N ? 64         */\
                "beq-    4, 0f;\n\t"                /* exp==0 => goto 0      */\
                "oris    %[MH],%[MH],0x10;\n"       /* set implicit first 1  */\
        "0:\n\t"\
                "bge-    7, 1f;\n\t"                /* N >= 64, goto 1       */\
                "subfic  %[R2],%[N],32;\n\t"  /* left shift by (0<=)N(<64)   */\
                "srw     %[ML],%[ML],%[N];\n\t"\
                "slw     %[R1],%[MH],%[R2];\n\t"\
                "or      %[ML],%[ML],%[R1];\n\t"\
                "addi    %[R2],%[N],-32;\n\t"\
                "srw     %[R1],%[MH],%[R2];\n\t"\
                "or      %[ML],%[ML],%[R1];\n\t"\
                "srw     %[MH],%[MH],%[N];\n\t"\
                "beq+    0, 2f;\n\t"               /* sign(m)==0 => goto 2   */\
                "nor     %[ML],%[ML],%[ML];\n\t" /* m = -m (2's complement)  */\
                "addic   %[ML],%[ML],1;\n\t"\
                "nor     %[MH],%[MH],%[MH];\n\t"\
                "addze   %[MH],%[MH];\n"\
                "b       2f;\n"\
        "1:\n\t"\
                "li      %[MH],0;\n\t"           /* left shift by N > 64     */\
                "li      %[ML],0;\n\t"\
        "2:\n\t"\
                : \
                [MH] "+r"(mh),\
                [ML] "+r"(ml),\
                [NE] "+b"(maxe),\
                [N]  "+b"(r1),\
                [R1] "+r"(r2),\
                [R2] "+r"(r3) \
                : \
                [XOFF] "i" EXPO_PRE_OFFSET	\
                : "cr0", "cr4", "cr7" );


/* pre process 1 mantissa (64 bit wide) */
#define MAN64PRE1M(ud,um,uhme) \
{  LDI(mh0,ud); LDI(ml0,ud); LDH(me0,uhme);\
   MAN64PRECORE(mh0,ml0,me0,r01,r02,r03); \
   STI(mh0,um); STI(ml0,um); \
}

/* pre process 2Q mantissas ( i.e. 4 mantissas ) */
#define MAN64PRE2Q(ud,um,uhme) \
{  LDI(mh0,ud); LDI(ml0,ud); LDH(me0,uhme);\
   LDI(mh1,ud); LDI(ml1,ud); LDH(me1,uhme);\
   MAN64PRECORE(mh0,ml0,me0,r01,r02,r03); \
   STI(mh0,um); STI(ml0,um); \
   LDI(mh2,ud); LDI(ml2,ud); LDH(me2,uhme);\
   MAN64PRECORE(mh1,ml1,me1,r11,r12,r13); \
   STI(mh1,um); STI(ml1,um); \
   LDI(mh0,ud); LDI(ml0,ud); LDH(me0,uhme);\
   MAN64PRECORE(mh2,ml2,me2,r21,r22,r23); \
   STI(mh2,um); STI(ml2,um); \
   MAN64PRECORE(mh0,ml0,me0,r01,r02,r03); \
   STI(mh0,um); STI(ml0,um); \
}


/* -------------------------------------------------------------------------- */
/*                                       pre-process a mantissa, 96 bits wide */
/* -------------------------------------------------------------------------- */

/*  Params
 *  mh   (inout): in: double high word, out: man high word
 *  ml   (inout): man middle  word
 *  mt   (out)  : man low (tail) word
 *  maxe (in)   : max exp
 *  r1,r2,r3,r4 : work registers used internally by the macro
 *
 * algorithm skeleton: dobule d = [mh,ml]  ( []: concatenation )
 *  N  = maxe - exp(d) + 5;
 *  mh = man_high_word_mask(mh);
 *  mt = 0;
 *  if ( exp(d) != 0 )
 *    set_implicit_first_1(mh);
 *  [mh,ml,mt] = [mh,ml,mt] >> N;
 *  if ( d < 0 )
 *    [mh,ml,mt] = - [mh,ml,mt];
 */
#define MAN96PRECORE(mh,ml,mt,maxe,r1,r2,r3,r4) \
  asm volatile( "li      %[MT],0;\n\t"                /* mt = 0              */\
                "rlwinm. %[R1],%[MH],0,0,0;\n\t"      /* R1 = sign, cr0 !    */\
                "rlwinm  %[R2],%[MH],12,21,31;\n\t"   /* R2 = exp            */\
                "addi    %[N],%[NE],%[XOFF];\n\t"     /* N = maxe + 5        */\
                "cmpwi   4,%[R2], 0;\n\t"             /* check: exp ? 0      */\
                "subf    %[N], %[R2], %[N];\n\t"      /* N = maxe - exp + 5  */\
                "rlwinm  %[MH],%[MH],0,12,31;\n\t"    /* mh = man_high       */\
                "cmpwi   1, %[N],96;\n\t"             /* check: N ? 96       */\
                "cmpwi   7, %[N],64;\n\t"             /* check: N ? 64       */\
                "cmpwi   2, %[N],32;\n\t"             /* check: N ? 32       */\
                "beq-    4, 0f;\n\t"                  /* exp==0 => goto 0    */\
                "oris    %[MH],%[MH],0x10;\n"         /* set first implicit 1*/\
        "0:\n\t"\
                "bge-    1, 1f;\n\t"                  /* N>=96 => goto 1     */\
                "bge-    7, 5f;\n\t"                  /* N>=64 => goto 5     */\
                "bge-    2, 2f;\n\t"                  /* N>=32 => goto 2     */\
                "subfic  %[R1],%[N],32;\n\t"      /* left shift by 0<=N<32   */\
                "srw     %[MT],%[MT],%[N];\n\t"\
                "slw     %[R2],%[ML],%[R1];\n\t"\
                "or      %[MT],%[MT],%[R2];\n\t"\
                "srw     %[ML],%[ML],%[N];\n\t"\
                "slw     %[R3],%[MH],%[R1];\n\t"\
                "or      %[ML],%[ML],%[R3];\n\t"\
                "srw     %[MH],%[MH],%[N];\n\t"\
                "b       3f;\n"\
        "5:\n\t"\
                "addic   %[N], %[N],-64;\n\t"   /* left shift by 32=<N<64    */\
                "li      %[ML],0;\n\t"\
                "srw     %[MT],%[MH],%[N];\n\t"\
                "li      %[MH],0;\n\t"\
                "b       3f;\n"\
        "2:\n\t"\
                "addic    %[R1],%[N],-32;\n\t"  /* left shift by 64=<N<96    */\
                "subfic   %[R2],%[N],64;\n\t"\
                "srw      %[MT],%[ML],%[R1];\n\t"\
                "slw      %[R3],%[MH],%[R2];\n\t"\
                "or       %[MT],%[MT],%[R3];\n\t"\
                "srw      %[ML],%[MH],%[R1];\n\t"\
                "li       %[MH],0;\n"\
        "3:\n\t"\
                "beq+    0, 4f;\n\t"               /* sign==0 => goto 4      */\
                "nor     %[MT],%[MT],%[MT];\n\t" /* sign==1 => m = -m        */\
                "addic   %[MT],%[MT],1;\n\t"\
                "nor     %[ML],%[ML],%[ML];\n\t"\
                "addze   %[ML],%[ML];\n"\
                "nor     %[MH],%[MH],%[MH];\n\t"\
                "addze   %[MH],%[MH];\n"\
                "b       4f;\n"\
        "1:\n\t"\
                "li      %[MH],0;\n\t"         /* left shift by N>=64       */\
                "li      %[ML],0;\n\t"\
                "li      %[MT],0;\n\t"\
        "4:\n\t"\
                : \
                [MH] "+r"(mh),\
                [ML] "+r"(ml),\
                [MT] "+b"(mt),\
                [NE] "+b"(maxe),\
                [N]  "+b"(r1),\
                [R1] "+r"(r2),\
                [R2] "+b"(r3),\
                [R3] "+r"(r4) \
                :\
                [XOFF] "i" EXPO_PRE_OFFSET	\
                : "cr0", "cr2", "cr4", "cr7" );




/* pre process 3Q mantissas ( i.e. 4 mantissas, each 96 bits wide) */
#define MAN96PRE3Q(ud,um,uhme) \
{  LDI(mh0,ud); LDI(ml0,ud); LDH(me0,uhme);\
   LDI(mh1,ud); LDI(ml1,ud); LDH(me1,uhme);\
   MAN96PRECORE(mh0,ml0,mt0,me0,r01,r02,r03,r04); \
   STI(mh0,um); STI(ml0,um); STI(mt0,um);\
   LDI(mh2,ud); LDI(ml2,ud); LDH(me2,uhme);\
   MAN96PRECORE(mh1,ml1,mt1,me1,r11,r12,r13,r14); \
   STI(mh1,um); STI(ml1,um); STI(mt1,um); \
   LDI(mh0,ud); LDI(ml0,ud); LDH(me0,uhme);\
   MAN96PRECORE(mh2,ml2,mt2,me2,r21,r22,r23,r24); \
   STI(mh2,um); STI(ml2,um); STI(mt2,um); \
   MAN96PRECORE(mh0,ml0,mt0,me0,r01,r02,r03,r04); \
   STI(mh0,um); STI(ml0,um); STI(mt0,um);\
}


/* pre process 1 mantissa ( 96 bits wide ) */
#define MAN96PRE1M(ud,um,uhme) \
{ LDI(mh0,ud); LDI(ml0,ud); LDH(me0,uhme);\
  MAN96PRECORE(mh0,ml0,mt0,me0,r01,r02,r03,r04); \
  STI(mh0,um); STI(ml0,um); STI(mt0,um);\
}


/* -------------------------------------------------------------------------- */
/*                                                    post-process a mantissa */
/* -------------------------------------------------------------------------- */


/* insert sign bit */
#define SIGNINS(mh,s) \
asm volatile("or %0,%0,%1;" : "+r"(mh) : "r"(s) );


/* insert exponent, if e >= 2047 SNaN, if ne >= 2047 Infinity  */
#define EXPINS(mh,ml,e,ne) \
asm volatile("cmpwi 1, %[NE], 2047;\n\t"\
             "blt+  1, 0f;\n\t"\
             "li    %[NE],2047;\n\t"\
             "li    %[MH],0;"\
             "li    %[ML],0;\n"\
     "0:\n\t"\
             "cmpwi 4, %[E], 2047;\n\t"\
             "blt+  4, 1f;\n\t"\
             "li    %[NE],2047;\n\t"\
             "lis   %[MH],0x7;\n\t"\
     "1:\n\t"\
             "rlwimi %[MH],%[NE],20,1,11;"\
           : [MH] "+r"(mh),\
             [ML] "+r"(ml),\
             [E] "+r"(e),\
             [NE] "+b"(ne) :: "cr1", "cr4" );



/* check sign and compute 2's complement if necessary, 64 bits wide mantissa */
#define CHECKSIGN64(s,mh,ml) \
asm volatile("rlwinm. %[S],%[MH],0,0,0;\n\t"\
             "beq+    0,0f;\n\t"\
             "nor     %[ML],%[ML],%[ML];\n\t"\
             "addic   %[ML],%[ML],1;\n\t"\
             "nor     %[MH],%[MH],%[MH];\n\t"\
             "addze   %[MH],%[MH];\n" \
             "0:\n\t"\
           : [S] "=r"(s), [MH] "+r"(mh), [ML] "+b"(ml) :: "cr0" );



/* -------------------------------------------------------------------------- */
/*                                         normalize a mantissa, 64 bits wide */
/* -------------------------------------------------------------------------- */
/*  Params
 *  mh   (inout): man high word
 *  ml   (inout): man low  word
 *  e    (in)   : current exp
 *  ne   (out)  : normalized exp
 *  c,r1,r2     : work registers used internally by the macro
 *
 * algorithm skeleton: dobule d = [mh,ml]  ( []: concatenation )
 *
 *  if ( [mh,ml] == 0 )
 *     ne = 0;
 *  else
 *    {
 *      c  = count_leading_zeros([mh,ml]);
 *      NE = e + 16 + c;
 *      if ( c < 11 )
 *         [mh,ml] = [mh,ml] >> ( 11 - c );
 *      else
 *        {
 *          r2 = c - 11;
 *          if ( ne < 1 )
 *           {
 *             ne = 0;
 *             r2 = e + 5 + ( e != 0 );
 *           }
 *          [mh,ml] = [mh,ml] << r2;
 *        }
 *    }
 */
#define NORMALIZE64(mh,ml,e,ne,c,r1,r2) \
asm volatile("or.     %[NE],%[MH],%[ML];\n\t"  /* check: m == 0  (cr0!)      */\
             "cntlzw  %[CN],%[MH]\n\t"         /* CN = cntlz(mh)             */\
             "beq-    0, 0f;\n\t"              /* m == 0 => goto 0           */\
             "addi    %[NE],%[E],%[XOFF2];"    /* NE = E + 16                */\
             "cmpwi   4,%[CN],32\n\t"          /* check: CN ? 32 (cr4)       */\
             "bne-    4,1f\n\t"                /* CN<32 => goto 1            */\
             "cntlzw  %[CN],%[ML]\n\t"         /* CN = cntlz(ml)             */\
             "addi    %[CN],%[CN],32\n"        /* CN = cntlz(mh)+cntlz(ml)   */\
      "1:\n\t"\
             "addic.  %[R2],%[CN],-11;\n\t"    /* R2 = CN - 11 (cr0!)        */\
             "subf    %[NE],%[CN],%[NE];\n\t"  /* NE = E+16-CN               */\
             "cmpwi   7,%[NE],1;\n\t"          /* check: NE ? 1 (cr7)        */\
             "blt-    0, 3f;\n\t"              /* CN <11 => goto 3           */\
             "bge+    7, 2f;\n\t"              /* NE >= 1 => goto 2          */\
             "li      %[NE],0;\n\t"            /* NE = 0 (not normalized!)   */\
             "addic   %[R2],%[E],%[XOFF1];\n\t" /* R2 = E + 5                 */\
             "addic   %[R1],%[E],-1;\n\t"      /* check if E == 0, step 1.   */\
             "subfe   %[R1],%[R1],%[E];\n\t"   /* check if E == 0, step 2.   */\
             "subf    %[R2],%[R1],%[R2];\n"    /* R2 = E + 5 - ( E!=0 )      */\
      "2:\n\t"                           /* left shift by 0=<R2<53           */\
             "subfic  %[CN],%[R2],32;\n\t"\
             "slw     %[MH],%[MH],%[R2];\n\t"\
             "srw     %[R1],%[ML],%[CN];\n\t"\
             "or      %[MH],%[MH],%[R1];\n\t"\
             "addi    %[CN],%[R2],-32;\n\t"\
             "slw     %[R1],%[ML],%[CN];\n\t"\
             "or      %[MH],%[MH],%[R1];\n\t"\
             "slw     %[ML],%[ML],%[R2];\n\t"\
             "b      0f;\n" \
      "3:\n\t"\
             "neg    %[R2],%[R2];\n\t"  /* right shift by 0<R2<=11           */\
             "subfic %[CN],%[R2],32;\n\t"\
             "srw    %[ML],%[ML],%[R2];\n\t"\
             "slw    %[R1],%[MH],%[CN];\n\t"\
             "srw    %[MH],%[MH],%[R2];\n\t"\
             "or     %[ML],%[ML],%[R1];\n\t"\
      "0:\n\t"\
            :\
             [MH] "+r"(mh),\
             [ML] "+r"(ml),\
             [E]  "+b"(e),\
             [NE] "+r"(ne),\
             [CN] "+b"(c),\
             [R1] "+r"(r1),\
             [R2] "+b"(r2) \
            :\
             [XOFF1] "i" EXPO_PRE_OFFSET,\
             [XOFF2] "i" EXPO_POST_OFFSET\
            : "cr0","cr4", "cr7" );



/* core of post-processing a mantisssa, 64 bits wide */
#define MAN64POSTCORE(mh,ml,e) \
{ register unsigned s1, c1, r1, r2, ne;\
 CHECKSIGN64(s1,mh,ml);\
 NORMALIZE64(mh,ml,e,ne,c1,r1,r2);\
 EXPINS(mh,ml,e,ne);\
 SIGNINS(mh,s1);\
}


/* post-process 1 mantissa */
#define MAN64POST1M(um,ue,ud) \
{ \
  LDI(mh1,um);LDI(ml1,um);LDH(e1,ue); \
  MAN64POSTCORE(mh1,ml1,e1); \
  STI(mh1,ud); STI(ml1,ud); \
 }


/* post-process 2 quad mantissas (4 doubles) */
#define MAN64POST2Q(um,ue,ud) \
{ \
  LDI(mh1,um);LDI(ml1,um);LDH(e1,ue); \
  LDI(mh2,um);LDI(ml2,um);LDH(e2,ue); \
  LDI(mh3,um);LDI(ml3,um);LDH(e3,ue); \
  MAN64POSTCORE(mh1,ml1,e1); \
  LDI(mh4,um);LDI(ml4,um);LDH(e4,ue); \
  MAN64POSTCORE(mh2,ml2,e2); \
  STI(mh1,ud); STI(ml1,ud); \
  MAN64POSTCORE(mh3,ml3,e3); \
  STI(mh2,ud); STI(ml2,ud); \
  MAN64POSTCORE(mh4,ml4,e4); \
  STI(mh3,ud); STI(ml3,ud); \
  STI(mh4,ud); STI(ml4,ud); \
}


/* -------------------------------------------------------------------------- */
/*                                                normalize mantissa, 96 bits */
/* -------------------------------------------------------------------------- */

/*
 * mh,ml(inout),mt(in) : 96 bits mantissa
 * e              (in) : current exponent
 * ne            (out) : new exponent
 * c,r1,r2,r3 : work registers used internally by the macro
 *
 * algorithm skeleton: dobule d = [mh,ml,mt]  ( []: concatenation )
 *
 * if ( [mh,ml,mt] == 0 )
 *   ne = 0;
 * else
 *  {
 *    c  = count_leading_zeros([mh,ml,mt]);
 *    ne = e + 16 + c;
 *    if ( c < 11 )
 *      [mh,ml] = [mh,ml,mt] >> ( 11 - c );
 *    else
 *      {
 *         r2 = c - 11;
 *         if ( ne < 1 )
 *           {
 *             ne = 0;
 *             r2 = e + 5 + ( e != 0 );
 *           }
 *         [mh,ml] = [mh,ml,mt] << r2;
 *      }
 * }
 */
#define NORMALIZE96(mh,ml,mt, e,ne,c,r1,r2,r3) \
asm volatile("or     %[R1],%[MH],%[ML];\n\t"    /* check if m == 0           */\
             "or.    %[NE],%[R1],%[MT];\n\t"    /* check if m == 0           */\
             "cntlzw  %[CN],%[MH]\n\t"          /* CN = cntlzw(MH)           */\
             "beq-    0, 0f;\n\t"               /* m == 0 holds, goto 0      */\
             "addi    %[NE],%[E],%[XOFF2];"     /* NE = E + 16               */\
             "cmpwi   4,%[CN],32\n\t"           /* check if CN == 32         */\
             "bne+    4,1f\n\t"                 /* CN == cntlzw(m), goto 1   */\
             "cntlzw  %[R1],%[ML]\n\t"          /* R1=cntlz(ML)              */\
             "cmpwi   2,%[R1],32;\n\t"          /* check if R1==32           */\
             "addic   %[CN],%[R1],32;\n\t"      /* CN = R1+32-11             */\
             "bne+    2, 1f;\n\t"               /* CN == cntlzw(m), goto 1   */\
             "cntlzw  %[R2],%[MT]\n\t"          /* R2=cntlz(MT)              */\
             "addic   %[CN],%[R2],64;\n\t"      /* CN=R2+64                  */\
      "1:\n\t"                            /* calc shift count and NE         */\
             "addic.  %[R3], %[CN],-11;\n\t"    /* R3 = CN - 11              */\
             "subf    %[NE],%[CN],%[NE];\n\t"   /* NE -= CN;                 */\
             "blt-    0, 3f;\n\t"               /* if CN<11, goto 3          */\
             "cmpwi   7,%[NE],1;\n\t"           /* check if NE == 1          */\
             "bge+    7, 4f;\n\t"               /* NE>=1, normalized num     */\
             "addic   %[R2],%[E],-1\n\t"        /* check if e==0, step 1     */\
             "subfe   %[R1],%[R2],%[E];\n\t"    /* E==0 ? R1=0 : R1=1        */\
             "li      %[NE],0;\n\t"             /* NE = 0                    */\
             "addic   %[R3],%[E],%[XOFF1];\n\t" /* R3 = E + 5                */\
             "subf    %[R3],%[R1],%[R3];\n"     /* R3 -= R1                  */\
      "4:\n\t"\
             "cmpwi   5,%[R3],64;\n\t"          /* check if shift count < 64 */\
             "cmpwi   1,%[R3],32;\n\t"          /* check if shift count < 32 */\
             "bgt-    5, 5f;\n\t"\
             "bgt-    1, 2f;\n\t"\
                                          /* left shift, m = m<<R3, 0<=R3<21 */\
             "subfic  %[CN],%[R3],32;\n\t"\
             "slw     %[MH],%[MH],%[R3];\n\t"\
             "srw     %[R1],%[ML],%[CN];\n\t"\
             "srw     %[R2],%[MT],%[CN];\n\t"\
             "or      %[MH],%[MH],%[R1];\n\t"\
             "slw     %[ML],%[ML],%[R3];\n\t"\
             "or      %[ML],%[ML],%[R2];\n\t"\
             "b      0f;\n" \
      "3:\n\t"                            /* right shift, m = m>>R3, 0<=R3<11*/\
             "neg    %[R3],%[R3];\n\t"\
             "subfic %[CN],%[R3],32;\n\t"\
             "slw    %[R2],%[MH],%[CN];\n\t"\
             "srw    %[ML],%[ML],%[R3];\n\t"\
             "srw    %[MH],%[MH],%[R3];\n\t"\
             "or     %[ML],%[ML],%[R2];\n\t"\
             "b      0f;\n"\
      "2:\n\t"                            /* left shift, m = m<<R3, 32<=R3<64*/\
             "addi   %[R1],%[R3],-32;\n\t"\
             "subfic %[CN],%[R3],64;\n\t"\
             "slw    %[MH],%[ML],%[R1];\n\t"\
             "srw    %[R2],%[MT],%[CN];\n\t"\
             "slw    %[ML],%[MT],%[R1];\n\t"\
             "or     %[MH],%[MH],%[R2];\n\t"\
             "b      0f;\n\t"\
      "5:\n\t"                            /* left shift, m = m<<R3, 64<=R3<96*/\
             "addi   %[R1],%[R3],-64;\n\t"\
             "subfic %[CN],%[R3],96;\n\t"\
             "slw    %[MH],%[MT],%[R1];\n\t"\
             "srw    %[R2],%[MT],%[CN];\n\t"\
             "or     %[MH],%[MH],%[R2];\n\t"\
             "li     %[ML],0;\n\t"\
      "0:\n\t"\
            :\
             [MH] "+r"(mh),\
             [ML] "+r"(ml),\
             [MT] "+r"(mt),\
             [E]  "+b"(e),\
             [NE] "+r"(ne),\
             [CN] "+b"(c),\
             [R1] "+b"(r1),\
             [R2] "+b"(r2),\
             [R3] "+b"(r3) \
            :\
             [XOFF1] "i" EXPO_PRE_OFFSET,\
             [XOFF2] "i" EXPO_POST_OFFSET\
            : "cr0","cr2","cr4","cr5", "cr7" );



/* check sign and compute 2's complement if necessary, 96 bits wide mantissa */
#define CHECKSIGN96(s,mh,ml,mt) \
asm volatile("rlwinm. %[S],%[MH],0,0,0;\n\t"\
             "beq+    0,0f;\n\t"\
             "nor     %[MT],%[MT],%[MT];\n\t"\
             "addic   %[MT],%[MT],1;\n\t"\
             "nor     %[ML],%[ML],%[ML];\n\t"\
             "addze   %[ML],%[ML];\n\t"\
             "nor     %[MH],%[MH],%[MH];\n\t"\
             "addze   %[MH],%[MH];\n" \
             "0:\n\t"\
           : [S] "=r"(s),\
             [MH] "+r"(mh),\
             [ML] "+r"(ml),\
             [MT] "+b"(mt) :: "cr0" );


/* core of post-processing a mantissa, 96 bist wide */
#define MAN96POSTCORE(mh,ml,mt,e,s,ne,c,r1,r2,r3) \
{ \
 CHECKSIGN96(s,mh,ml,mt);\
 NORMALIZE96(mh,ml,mt,e,ne,c,r1,r2,r3);\
 EXPINS(mh,ml,e,ne);\
 SIGNINS(mh,s);\
}



/* post process 3Q mantissas ( 4 mantissas, each 96 bits wide ) */
#define MAN96POST3Q(um,ue,ud) \
{ \
  LDI(mh1,um);LDI(ml1,um);LDI(mt1,um);LDH(e1,ue); \
  LDI(mh2,um);LDI(ml2,um);LDI(mt2,um);LDH(e2,ue); \
  MAN96POSTCORE(mh1,ml1,mt1,e1,s1,ne1,c1,r11,r12,r13); \
  STI(mh1,ud); STI(ml1,ud); \
  LDI(mh1,um);LDI(ml1,um);LDI(mt1,um);LDH(e1,ue); \
  MAN96POSTCORE(mh2,ml2,mt2,e2,s2,ne2,c2,r21,r22,r23); \
  STI(mh2,ud); STI(ml2,ud); \
  LDI(mh2,um);LDI(ml2,um);LDI(mt2,um);LDH(e2,ue); \
  MAN96POSTCORE(mh1,ml1,mt1,e1,s1,ne1,c1,r11,r12,r13); \
  STI(mh1,ud); STI(ml1,ud); \
  MAN96POSTCORE(mh2,ml2,mt2,e2,s2,ne2,c2,r21,r22,r23); \
  STI(mh2,ud); STI(ml2,ud); \
}



/* post process 1 mantissa ( 96 bits wide ) */
#define MAN96POST1M(um,ue,ud) \
{ \
  LDI(mh2,um);LDI(ml2,um);LDI(mt2,um);LDH(e2,ue); \
  MAN96POSTCORE(mh2,ml2,mt2,e2,s2,ne2,c2,r21,r22,r23); \
  STI(mh2,ud); STI(ml2,ud); \
}




#endif
