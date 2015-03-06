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
 * \file math/ppc450d/_optim_fp64_dual_src.x.h
 * \brief ???
 * X-Macro for optimized (asm) dual source, double, functions.
 * i.e., c = op(a,b)
 *
 * Requires that the following macros be defined:
 *
 * OP2 =	the code to do the desired operation
 *		where a is the dest and src, e.g. OP2(a,b) -> a += b
 * OP3 =	the code to do the desired operation
 *		where a is the dest and b,c are the src,
 *		e.g. OP3(a,b,c) -> a = b + c
 *
 * Assumes context: variables double *f0, double *f1, double *f2
 * (f0 <- src0, f1 <- src1, f2 <- dst),
 * and size128 which is the number of 16-double loops
 * to perform (i.e. count >> 4).
 */

/// \brief Load a double into an FP reg.
#define LFDUX(fpt, base, delta)		\
        asm volatile ("lfdux %0, %1, %2"\
                        : "=f"(fpt),	\
                          "+b"(base)	\
                        : "r"(delta))

/// \brief Load two doubles into FP regs (quad-aligned).
#define LFPDUX(fpt, base, delta)		\
        asm volatile ("lfpdux %0, %1, %2"	\
                        : "=f"(fpt),		\
                          "+b"(base)		\
                        : "r"(delta))

/// \brief Store two double FP regs into quad-aligned memory.
#define STFPDUX(fpt, base, delta)		\
        asm volatile ("stfpdux %0, %1, %2"	\
                        : "=f"(fpt),		\
                          "+b"(base)		\
                        : "r"(delta))

/// \brief Declare a variable as a FP reg.
#define FP_REG(var, reg_num) \
        register double var asm("fr" #reg_num)

/// \brief Saving higher 8 FP registers (we actually only need f12-f15 saved)
#define FP_REG_SAVE()					\
        register double *saved_fprs_base_p;		\
        double saved_fprs_data[32] __attribute__((__aligned__(16))); \
        saved_fprs_base_p = saved_fprs_data - 2;	\
        STFPDUX (s0, saved_fprs_base_p, sixteen);	\
        STFPDUX (s1, saved_fprs_base_p, sixteen);	\
        STFPDUX (s2, saved_fprs_base_p, sixteen);	\
        STFPDUX (s3, saved_fprs_base_p, sixteen);	\
        STFPDUX (s4, saved_fprs_base_p, sixteen);	\
        STFPDUX (s5, saved_fprs_base_p, sixteen);	\
        STFPDUX (s6, saved_fprs_base_p, sixteen);	\
        STFPDUX (s7, saved_fprs_base_p, sixteen);       \
        STFPDUX (t0, saved_fprs_base_p, sixteen);	\
        STFPDUX (t1, saved_fprs_base_p, sixteen);	\
        STFPDUX (t2, saved_fprs_base_p, sixteen);	\
        STFPDUX (t3, saved_fprs_base_p, sixteen);	\
        STFPDUX (t4, saved_fprs_base_p, sixteen);	\
        STFPDUX (t5, saved_fprs_base_p, sixteen);	\
        STFPDUX (t6, saved_fprs_base_p, sixteen);	\
        STFPDUX (t7, saved_fprs_base_p, sixteen);


/// \brief Restore higher 8 FP registers (we actually only need f12-f15 saved)
#define FP_REG_RESTORE()			\
        saved_fprs_base_p = saved_fprs_data - 2;\
        LFPDUX (s0, saved_fprs_base_p, sixteen);\
        LFPDUX (s1, saved_fprs_base_p, sixteen);\
        LFPDUX (s2, saved_fprs_base_p, sixteen);\
        LFPDUX (s3, saved_fprs_base_p, sixteen);\
        LFPDUX (s4, saved_fprs_base_p, sixteen);\
        LFPDUX (s5, saved_fprs_base_p, sixteen);\
        LFPDUX (s6, saved_fprs_base_p, sixteen);\
        LFPDUX (s7, saved_fprs_base_p, sixteen);\
        LFPDUX (t0, saved_fprs_base_p, sixteen);\
        LFPDUX (t1, saved_fprs_base_p, sixteen);\
        LFPDUX (t2, saved_fprs_base_p, sixteen);\
        LFPDUX (t3, saved_fprs_base_p, sixteen);\
        LFPDUX (t4, saved_fprs_base_p, sixteen);\
        LFPDUX (t5, saved_fprs_base_p, sixteen);\
        LFPDUX (t6, saved_fprs_base_p, sixteen);\
        LFPDUX (t7, saved_fprs_base_p, sixteen);


#if defined(OP2) && defined(OP3)

if (((uint32_t)f0 | (uint32_t)f1 | (uint32_t)f2) & 0x0f) {
        // Unaligned - can't use quad-load/store instructions.

        unsigned eight = 8;
        register double r0;
        register double r1;
        register double r2;
        register double r3;
        register double r4;
        register double r5;
        register double r6;
        register double r7;

        register double s0;
        register double s1;
        register double s2;
        register double s3;
        register double s4;
        register double s5;
        register double s6;
        register double s7;


        --f0; // adjust for LFDUX(...8)
        --f1;
        --f2;
        size128--;

        ///
        /// We use the LFDUX assembly instruction as the compiler does not
        /// seem to easily generate that instruction
        ///

        LFDUX(s0, f0, eight);
        LFDUX(s1, f0, eight);
        LFDUX(s2, f0, eight);
        LFDUX(s3, f0, eight);

        LFDUX(r0, f1, eight);
        LFDUX(r1, f1, eight);
        LFDUX(r2, f1, eight);
        LFDUX(r3, f1, eight);

        OP2(r0, s0);

        while (size128--) {
                LFDUX(s4, f0, eight);
                LFDUX(r4, f1, eight);
                OP2(r1, s1);
                *(++f2) = r0;

                LFDUX(s5, f0, eight);
                LFDUX(r5, f1, eight);
                OP2(r2, s2);
                *(++f2) = r1;

                LFDUX(s6, f0, eight);
                LFDUX(r6, f1, eight);
                OP2(r3, s3);
                *(++f2) = r2;

                LFDUX(s7, f0, eight);
                LFDUX(r7, f1, eight);
                OP2(r4, s4);
                *(++f2) = r3;

                LFDUX(s0, f0, eight);
                LFDUX(r0, f1, eight);
                OP2(r5, s5);
                *(++f2) = r4;

                LFDUX(s1, f0, eight);
                LFDUX(r1, f1, eight);
                OP2(r6, s6);
                *(++f2) = r5;

                LFDUX(s2, f0, eight);
                LFDUX(r2, f1, eight);
                OP2(r7, s7);
                *(++f2) = r6;

                LFDUX(s3, f0, eight);
                LFDUX(r3, f1, eight);
                OP2(r0, s0);
                *(++f2) = r7;

                LFDUX(s4, f0, eight);
                LFDUX(r4, f1, eight);
                OP2(r1, s1);
                *(++f2) = r0;

                LFDUX(s5, f0, eight);
                LFDUX(r5, f1, eight);
                OP2(r2, s2);
                *(++f2) = r1;

                LFDUX(s6, f0, eight);
                LFDUX(r6, f1, eight);
                OP2(r3, s3);
                *(++f2) = r2;

                LFDUX(s7, f0, eight);
                LFDUX(r7, f1, eight);
                OP2(r4, s4);
                *(++f2) = r3;

                LFDUX(s0, f0, eight);
                LFDUX(r0, f1, eight);
                OP2(r5, s5);
                *(++f2) = r4;

                LFDUX(s1, f0, eight);
                LFDUX(r1, f1, eight);
                OP2(r6, s6);
                *(++f2) = r5;

                LFDUX(s2, f0, eight);
                LFDUX(r2, f1, eight);
                OP2(r7, s7);
                *(++f2) = r6;

                LFDUX(s3, f0, eight);
                LFDUX(r3, f1, eight);
                OP2(r0, s0);
                *(++f2) = r7;
        }

        OP2(r1, s1);
        OP2(r2, s2);
        OP2(r3, s3);

        *(++f2) = r0;
        *(++f2) = r1;
        *(++f2) = r2;
        *(++f2) = r3;

        LFDUX(s4, f0, eight);
        LFDUX(s5, f0, eight);
        LFDUX(s6, f0, eight);
        LFDUX(s7, f0, eight);

        LFDUX(r4, f1, eight);
        LFDUX(r5, f1, eight);
        LFDUX(r6, f1, eight);
        LFDUX(r7, f1, eight);

        OP2(r4, s4);
        OP2(r5, s5);
        OP2(r6, s6);
        OP2(r7, s7);

        *(++f2) = r4;
        *(++f2) = r5;
        *(++f2) = r6;
        *(++f2) = r7;

        LFDUX(s0, f0, eight);
        LFDUX(s1, f0, eight);
        LFDUX(s2, f0, eight);
        LFDUX(s3, f0, eight);

        LFDUX(r0, f1, eight);
        LFDUX(r1, f1, eight);
        LFDUX(r2, f1, eight);
        LFDUX(r3, f1, eight);

        OP2(r0, s0);
        OP2(r1, s1);
        OP2(r2, s2);
        OP2(r3, s3);

        *(++f2) = r0;
        *(++f2) = r1;
        *(++f2) = r2;
        *(++f2) = r3;

        LFDUX(s4, f0, eight);
        LFDUX(s5, f0, eight);
        LFDUX(s6, f0, eight);
        LFDUX(s7, f0, eight);

        LFDUX(r4, f1, eight);
        LFDUX(r5, f1, eight);
        LFDUX(r6, f1, eight);
        LFDUX(r7, f1, eight);

        OP2(r4, s4);
        OP2(r5, s5);
        OP2(r6, s6);
        OP2(r7, s7);

        *(++f2) = r4;
        *(++f2) = r5;
        *(++f2) = r6;
        *(++f2) = r7;
        ++f0;
        ++f1;
        ++f2;

} else {
        // Aligned - use quad-load/store instructions.
        unsigned sixteen = 16;

        FP_REG(r0, 0);
        FP_REG(r1, 1);
        FP_REG(r2, 2);
        FP_REG(r3, 3);
        FP_REG(r4, 4);
        FP_REG(r5, 5);
        FP_REG(r6, 6);
        FP_REG(r7, 7);

        FP_REG(s0, 8);
        FP_REG(s1, 9);
        FP_REG(s2, 10);
        FP_REG(s3, 11);
        FP_REG(s4, 12);
        FP_REG(s5, 13);
        FP_REG(s6, 14);
        FP_REG(s7, 15);

        FP_REG(t0, 16);
        FP_REG(t1, 17);
        FP_REG(t2, 18);
        FP_REG(t3, 19);
        FP_REG(t4, 20);
        FP_REG(t5, 21);
        FP_REG(t6, 22);
        FP_REG(t7, 23);

        FP_REG_SAVE();

        f0 -= 2; // adjust for LFPDUX(...16)
        f1 -= 2;
        f2 -= 2;
        size128--;

        LFPDUX(s0, f0, sixteen);
        LFPDUX(s1, f0, sixteen);
        LFPDUX(s2, f0, sixteen);
        LFPDUX(s3, f0, sixteen);

        LFPDUX(r0, f1, sixteen);
        LFPDUX(r1, f1, sixteen);
        LFPDUX(r2, f1, sixteen);
        LFPDUX(r3, f1, sixteen);

        OP3(t0, r0, s0);

        while (size128--) {
                LFPDUX(s4, f0, sixteen);
                LFPDUX(r4, f1, sixteen);
                OP3(t1, r1, s1);
                STFPDUX(t0, f2, sixteen);

                LFPDUX(s5, f0, sixteen);
                LFPDUX(r5, f1, sixteen);
                OP3(t2, r2, s2);
                STFPDUX(t1, f2, sixteen);

                LFPDUX(s6, f0, sixteen);
                LFPDUX(r6, f1, sixteen);
                OP3(t3, r3, s3);
                STFPDUX(t2, f2, sixteen);

                LFPDUX(s7, f0, sixteen);
                LFPDUX(r7, f1, sixteen);
                OP3(t4, r4, s4);
                STFPDUX(t3, f2, sixteen);

                LFPDUX(s0, f0, sixteen);
                LFPDUX(r0, f1, sixteen);
                OP3(t5, r5, s5);
                STFPDUX(t4, f2, sixteen);

                LFPDUX(s1, f0, sixteen);
                LFPDUX(r1, f1, sixteen);
                OP3(t6, r6, s6);
                STFPDUX(t5, f2, sixteen);

                LFPDUX(s2, f0, sixteen);
                LFPDUX(r2, f1, sixteen);
                OP3(t7, r7, s7);
                STFPDUX(t6, f2, sixteen);

                LFPDUX(s3, f0, sixteen);
                LFPDUX(r3, f1, sixteen);
                OP3(t0, r0, s0);
                STFPDUX(t7, f2, sixteen);
        }

        OP3(t1, r1, s1);
        OP3(t2, r2, s2);
        OP3(t3, r3, s3);

        STFPDUX(t0, f2, sixteen);
        STFPDUX(t1, f2, sixteen);
        STFPDUX(t2, f2, sixteen);
        STFPDUX(t3, f2, sixteen);

        LFPDUX(s4, f0, sixteen);
        LFPDUX(s5, f0, sixteen);
        LFPDUX(s6, f0, sixteen);
        LFPDUX(s7, f0, sixteen);

        LFPDUX(r4, f1, sixteen);
        LFPDUX(r5, f1, sixteen);
        LFPDUX(r6, f1, sixteen);
        LFPDUX(r7, f1, sixteen);

        OP3(t4, r4, s4);
        OP3(t5, r5, s5);
        OP3(t6, r6, s6);
        OP3(t7, r7, s7);

        STFPDUX(t4, f2, sixteen);
        STFPDUX(t5, f2, sixteen);
        STFPDUX(t6, f2, sixteen);
        STFPDUX(t7, f2, sixteen);

        FP_REG_RESTORE();

        f0 += 2;
        f1 += 2;
        f2 += 2;
}

#else /* !OP2 || !OP3 */
#error "_optim_fp64_dual_src.x.h included without required defines"
#endif /* !OP2 || !OP3 */
