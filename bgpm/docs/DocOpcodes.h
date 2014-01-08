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
/*!
 *
 * \page bgpm_opcodes BGPM Opcode Groups
 *
 * \htmlonly
<div class="toc"><!--[if IE]></div><div class="ie-toc"><![endif]-->
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"

- \ref opcode_xuinstr
- \ref opcode_qfpuinstr
\htmlonly
</div>
<div class="contents-right"><!--[if IE]></div><div class="ie-contents-right"><![endif]-->
\endhtmlonly
 *
 *
 * PUnit eventsets are able to count groups of completed instructions and floating point operations via
 * a hardware "opcode" unit.
 * The tables below provide more information about what is counted in each group,
 * how to select particular opcode matches, or what floating point weight is assigned to
 * instruction.
 *
 * \note None of the opcode events are "context sensitive".  That is, they can't be filtered
 * to count only in kernel or user state.  By default you cannot Bgpm_SetContext() to anything
 * but "both" if you are using any of these opcode events.   However, you can override this
 * behavior by using the Bgpm_AllowMixedContext() prior to adding events to a punit event set.
 *
 *
 *
 * \section opcode_xuinstr XU Instructions
 *
 * The event list defines some events for counting individual XU groups (most PEVT_INST_XU_???? events).
 * But, a few INST_XU events provide more flexibility.
 *
 * - PEVT_INST_XU_ALL event will count all completed XU (integer unit) instructions.
 *
 * - PEVT_INST_XU_GRP_MASK event will allow you to use the Bgpm_SetXuGrpMask() operation to
 *     configure a single counter to count instructions from multiple groups.
 *     You build the list of groups from UPC_P_XU_OGRP_??? define values which are listed
 *     in the table below.
 *
 * - PEVT_INST_XU_MATCH event allows you to configure a counter to count only a single
 *     instruction opcode.  However, only the "major" and "minor" bits of the instruction
 *     are matched, so it's not able to distinguish variations of an instruction.
 *     You use the Bgpm_SetXuMatch() operation and pass in either:
 *     \n\n
 *     - The Matchval and Maskval from the table.  For example, to match "add" operations you
 *       could do
 *       @code
 *           Bgpm_AddEvent(hPunitEvtSet, PEVT_INST_XU_MATCH);
 *           unsigned matchIdx = Bgpm_GetEventIndex(hPunitEvtSet, PEVT_INST_XU_MATCH, 0);
 *           Bgpm_SetXuMatch(hPunitEvtSet, matchIdx, 0x7D0A, 0xFDFF);
 *       @endcode
 *     \n\n
 *     - Use a BGPM_INST_<format>_FORM_CODE(major,minor) and BGPM_INST_<format>_FORM_MASK macros from
 *       bgpm/include/types.h to encode the match and mask values from the specific instruction
 *       "format" values indicated.  Thus, for example:
 *       @code
 *           Bgpm_AddEvent(hPunitEvtSet, PEVT_INST_XU_MATCH);
 *           unsigned matchIdx = Bgpm_GetEventIndex(hPunitEvtSet, PEVT_INST_XU_MATCH, 0);
 *           Bgpm_SetXuMatch(hPunitEvtSet, matchIdx, BGPM_INST_XO_FORM_CODE(31,266), BGPM_INST_XO_FORM_MASK);
 *       @endcode
 *
 *
 * \htmlinclude Instr-Map-XU.html
 *
 *
 *
 * \section opcode_qfpuinstr QFPU Instructions
 *
 * The event list defines some events for counting individual QFXU groups (most PEVT_INST_QFXU_???? events).
 * QFPU events can counter either instructions, or floating point operations.
 * The PM hardware counts floating point operations by using mostly hard-wired weights assigned to each
 * floating point instruction. You can enable floating point operation counting by using the
 * Bgpm_SetQfpuFP() operation on a QFPU instruction event, or by using the
 * PEVT_INST_QPFU_FPGRP1 or PEVT_INST_QPFU_FPGRP2.
 *
 * The FP operation weight of each instruction is shown by the FPScale field.
 * The FPGRP1_scale and FPGRP2_scale fields show which instructions are included in that event.
 *
 * Note that you can build your own list of instructions groups to filter using the PEVT_INST_QFPU_GRP_MASK events
 * and methods to configure the groups to include in a single counter.
 * Also note that the quad floating point square-root and division operation scaling is flexible, but defaults to
 * count 8 FP operation be each of these instructions.
 *
 *
 * Some special events:
 *
 * - PEVT_INST_QFPU_ALL event will count all completed QFPU (a.k.a AXU) instructions.
 *
 * - PEVT_INST_QFPU_GRP_MASK event will allow you to use the Bgpm_SetQfpuGrpMask() operation to
 *     configure a single counter to count instructions from multiple groups.
 *     You build the list of groups from UPC_P_AXU_OGRP_??? define values which are listed
 *     in the table below.
 *
 * - PEVT_INST_QFPU_MATCH event allows you to configure a counter to count only a single
 *     instruction opcode.  However, only the "major" and "minor" bits of the instruction
 *     are matched, so it's not able to distinguish variations of an instruction.
 *     You use the Bgpm_SetQfpuMatch() operation and pass in either:
 *     \n\n
 *     - The Matchval and Maskval from the table.  For example, to match "fabs" operations you
 *       could do
 *       @code
 *           Bgpm_AddEvent(hPunitEvtSet, PEVT_INST_QFPU_MATCH);
 *           unsigned matchIdx = Bgpm_GetEventIndex(hPunitEvtSet, PEVT_INST_QFPU_MATCH, 0);
 *           Bgpm_SetQfpuMatch(hPunitEvtSet, matchIdx, 0xFD08, 0xFFFF);
 *       @endcode
 *     \n\n
 *     - Use a BGPM_INST_<format>_FORM_CODE(major,minor) and BGPM_INST_<format>_FORM_MASK macros from
 *       bgpm/include/types.h to encode the match and mask values from the specific instruction
 *       "format" values indicated.  Thus, for example:
 *       @code
 *           Bgpm_AddEvent(hPunitEvtSet, PEVT_INST_QFPU_MATCH);
 *           unsigned matchIdx = Bgpm_GetEventIndex(hPunitEvtSet, PEVT_INST_QFPU_MATCH, 0);
 *           Bgpm_SetQfpuMatch(hPunitEvtSet, matchIdx, BGPM_INST_X_FORM_CODE(63,264), BGPM_INST_X_FORM_MASK);
 *       @endcode
 *
 *
 * \htmlinclude Instr-Map-QPU.html
 */
