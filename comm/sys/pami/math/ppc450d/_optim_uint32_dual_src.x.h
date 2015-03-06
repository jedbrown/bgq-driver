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
 * \file math/ppc450d/_optim_uint32_dual_src.x.h
 * \brief ???
 * X-Macro for optimized (asm) dual source, double, functions.
 * i.e., c = op(a,b)
 *
 * Requires that the following macros be defined:
 *
 * OP2 =	the code to do the desired operation
 *		where a is the dest and src, e.g. OP2(a,b) -> a += b
 *
 * Assumes context: variables uint32_t *s0, uint32_t *s1, uint32_t *s2
 * (s0 <- src0, s1 <- src1, s2 <- dst), and num the number of passes
 * e.g. count >> 3
 */

/// \brief Load a uint32 into a reg.
#define LWZUX(reg, base, delta)		\
        asm volatile ("lwzux %0, %1, %2"\
                        : "=r"(reg),	\
                          "+b"(base)	\
                        : "r"(delta))

/// \brief Store uint32 regs into memory.
#define STWUX(reg, base, delta)		\
        asm volatile ("stwux %0, %1, %2"\
                        : "=r"(reg),	\
                          "+b"(base)	\
                        : "r"(delta))

#if defined(OP2)

        // This code revolves around the 32-byte L1 cacheline, i.e. 8x uint32_t.
        // There are three prefetch buffers for the L1, so we can start filling
        // those - i.e. 96-byte prefetch - however we'll always take the hit
        // (probably) for the first three cachelines. We also will be fetching
        // beyond the end of our buffers.
        unsigned four = 4;

        register uint32_t r0 asm("r10");
        register uint32_t r1 asm("r11");
        register uint32_t r2 asm("r12");
        register uint32_t r3 asm("r13");
        register uint32_t r4 asm("r14");
        register uint32_t r5 asm("r15");
        register uint32_t r6 asm("r16");
        register uint32_t r7 asm("r17");

        register uint32_t i96 = 96 - 16 + 4;
        register uint32_t j96 = 96 - 32 + 4;

        --num;
        --s0; // adjust for LWZUX(...4)
        --s1;
        --s2;

        // Before/after loop completes 8 uint32_t dual-src operations, in out-of-phase pairs
        LWZUX(r0, s0, four);
        LWZUX(r4, s1, four);
        LWZUX(r1, s0, four);
        LWZUX(r5, s1, four);

        // Each loop completes 8 uint32_t dual-src operations, in out-of-phase pairs
        while (num--) {
                LWZUX(r2, s0, four);
                LWZUX(r6, s1, four);
                LWZUX(r3, s0, four);
                LWZUX(r7, s1, four);
                                        // s1 is +12 beyond original/prior value
                OP2(r0, r4);
                OP2(r1, r5);

                DCBT(s1, i96);

                STWUX(r0, s2, four);
                STWUX(r1, s2, four);

                LWZUX(r0, s0, four);
                LWZUX(r4, s1, four);
                LWZUX(r1, s0, four);
                LWZUX(r5, s1, four);

                OP2(r2, r6);
                OP2(r3, r7);

                STWUX(r2, s2, four);
                STWUX(r3, s2, four);

                LWZUX(r2, s0, four);
                LWZUX(r6, s1, four);
                LWZUX(r3, s0, four);
                LWZUX(r7, s1, four);
                                        // s0 is +28 beyond original/prior value
                OP2(r0, r4);
                OP2(r1, r5);

                DCBT(s0, j96);

                STWUX(r0, s2, four);
                STWUX(r1, s2, four);

                LWZUX(r0, s0, four);
                LWZUX(r4, s1, four);
                LWZUX(r1, s0, four);
                LWZUX(r5, s1, four);

                OP2(r2, r6);
                OP2(r3, r7);

                STWUX(r2, s2, four);
                STWUX(r3, s2, four);
        }
        LWZUX(r2, s0, four);
        LWZUX(r6, s1, four);
        LWZUX(r3, s0, four);
        LWZUX(r7, s1, four);

        OP2(r0, r4);
        OP2(r1, r5);

        STWUX(r0, s2, four);
        STWUX(r1, s2, four);

        LWZUX(r0, s0, four);
        LWZUX(r4, s1, four);
        LWZUX(r1, s0, four);
        LWZUX(r5, s1, four);

        OP2(r2, r6);
        OP2(r3, r7);

        STWUX(r2, s2, four);
        STWUX(r3, s2, four);

        LWZUX(r2, s0, four);
        LWZUX(r6, s1, four);
        LWZUX(r3, s0, four);
        LWZUX(r7, s1, four);

        OP2(r0, r4);
        OP2(r1, r5);

        STWUX(r0, s2, four);
        STWUX(r1, s2, four);

        OP2(r2, r6);
        OP2(r3, r7);

        STWUX(r2, s2, four);
        STWUX(r3, s2, four);

        ++s0;	// compensate for prior adjust for LWZUX
        ++s1;
        ++s2;

#else /* !OP2 */
#error "_optim_uint32_dual_src.x.h included without required defines"
#endif /* !OP2 */
