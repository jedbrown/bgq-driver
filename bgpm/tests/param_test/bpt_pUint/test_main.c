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

#include "stdlib.h"
#include "stdio.h"

#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();


#define NUM_THREADS 68
#define INVALID_PARM -1
int Dummy=-1;

void PunitInstructionEventAttributes_test(int);
void PunitMiscOverrides_test(int);

int main(int argc, char *argv[])
{
    Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error


    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

    // set to look at Punit Counters
    int hPuSet = Bgpm_CreateEventSet();
    // define eventlist to be added
    unsigned measList[] = {
    PEVT_L1P_BAS_LD,
    PEVT_IU_IS1_STALL_CYC,
    PEVT_INST_QFPU_GRP_MASK,
    PEVT_CYCLES,
    PEVT_INST_XU_GRP_MASK,
    PEVT_INST_QFPU_MATCH,
    PEVT_INST_XU_MATCH,
    PEVT_XU_ANY_FLUSH,
    PEVT_INST_ALL
    };

    TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet,measList,9),0);

    PunitInstructionEventAttributes_test(hPuSet);
    PunitMiscOverrides_test(hPuSet);

    TEST_CHECK_EQUAL( Bgpm_NumEvents(hPuSet),9);
    TEST_RETURN();
}
void PunitInstructionEventAttributes_test(int hPuSet)
{
    uint64_t grpMask;
    uint64_t XugrpMask;

    TEST_FUNCTION(SetQfpuGrpMask() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    unsigned qfpuGrpIdx = Bgpm_GetEventIndex(hPuSet, PEVT_INST_QFPU_GRP_MASK, 0);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuGrpMask(hPuSet, qfpuGrpIdx, UPC_P_AXU_OGRP_FSQ | UPC_P_AXU_OGRP_FDIV ),0);
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuGrpMask(Dummy, qfpuGrpIdx, UPC_P_AXU_OGRP_FSQ | UPC_P_AXU_OGRP_FDIV ),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event index );
    TEST_CHECK_EQUAL(Bgpm_SetQfpuGrpMask(hPuSet,Dummy, UPC_P_AXU_OGRP_FSQ | UPC_P_AXU_OGRP_FDIV ),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION(GetQfpuGrpMask() PARAMETER TEST)
    TEST_FUNCTION_PARM(Invalid event handle)
    TEST_CHECK_EQUAL(Bgpm_GetQfpuGrpMask(Dummy, qfpuGrpIdx, &grpMask),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalidevent index )
    TEST_CHECK_EQUAL(Bgpm_GetQfpuGrpMask(hPuSet,Dummy, &grpMask),BGPM_EINV_EVT_IDX);
    Bgpm_GetQfpuGrpMask(hPuSet, qfpuGrpIdx, &grpMask);

    TEST_FUNCTION(SetXuGrpMask() PARAMETER TEST);
    unsigned xuGrpIdx = Bgpm_GetEventIndex(hPuSet, PEVT_INST_XU_GRP_MASK, 0);
    TEST_FUNCTION_PARM(Success Case)
    TEST_CHECK_EQUAL(Bgpm_SetXuGrpMask(hPuSet, xuGrpIdx, UPC_P_XU_OGRP_SYNC | UPC_P_XU_OGRP_IMUL),0);
    TEST_FUNCTION_PARM(Invalid Event Handle)
    TEST_CHECK_EQUAL(Bgpm_SetXuGrpMask(Dummy, xuGrpIdx, UPC_P_XU_OGRP_SYNC | UPC_P_XU_OGRP_IMUL),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index )
    TEST_CHECK_EQUAL(Bgpm_SetXuGrpMask(hPuSet,Dummy , UPC_P_XU_OGRP_SYNC | UPC_P_XU_OGRP_IMUL),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION(SetXuMatch() PARAMETER TEST)
    unsigned xuMatchIdx = Bgpm_GetEventIndex(hPuSet, PEVT_INST_XU_MATCH, 0);
    TEST_FUNCTION_PARM(Success case);
    TEST_CHECK_EQUAL(Bgpm_SetXuMatch(hPuSet,xuMatchIdx , BGPM_INST_XFX_FORM_CODE(31,854), BGPM_INST_XFX_FORM_MASK ),0);
    TEST_FUNCTION_PARM(Invalid Event Set Handle);
    TEST_CHECK_EQUAL(Bgpm_SetXuMatch(Dummy,xuMatchIdx , BGPM_INST_XFX_FORM_CODE(31,854), BGPM_INST_XFX_FORM_MASK ) ,BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_SetXuMatch(hPuSet, Dummy, BGPM_INST_XFX_FORM_CODE(31,854), BGPM_INST_XFX_FORM_MASK),BGPM_EINV_EVT_IDX);

    TEST_FUNCTION(GetXuMatch() PARAMETER TEST)
    TEST_FUNCTION_PARM(Success case);
    uint16_t match, mask; ushort scale;
    Bgpm_GetXuMatch(hPuSet, xuMatchIdx, &match, &mask);
    TEST_CHECK_EQUAL(match, BGPM_INST_XFX_FORM_CODE(31,854));
    TEST_CHECK_EQUAL(mask, BGPM_INST_XFX_FORM_MASK);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL( Bgpm_GetXuMatch(Dummy, xuMatchIdx, &match, &mask),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_GetXuMatch(hPuSet,Dummy , &match, &mask),BGPM_EINV_EVT_IDX);

    TEST_FUNCTION(GeXtuGrpMask() PARAMETER TEST)
    TEST_FUNCTION_PARM(Invalid event handle)
    TEST_CHECK_EQUAL(Bgpm_GetXuGrpMask(Dummy, xuGrpIdx, &XugrpMask),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalidevent index )
    TEST_CHECK_EQUAL(Bgpm_GetXuGrpMask(hPuSet,Dummy, &XugrpMask),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION_PARM(Success Case);
    Bgpm_GetXuGrpMask(hPuSet, xuGrpIdx, &XugrpMask);
    TEST_FUNCTION(SetQfpuFp() PARAMETER TEST)
    unsigned qfpuMatchIdx = Bgpm_GetEventIndex(hPuSet, PEVT_INST_QFPU_MATCH, 0);
    TEST_FUNCTION_PARM(Success Case);
    Bgpm_SetQfpuFp(hPuSet, qfpuMatchIdx, BGPM_TRUE);
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuFp(Dummy, qfpuMatchIdx, BGPM_TRUE),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuFp(hPuSet,Dummy,BGPM_TRUE),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION_PARM(Invalid Choice);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuFp(hPuSet,qfpuMatchIdx,-1),0);

    TEST_FUNCTION(SetQfpuMatch() PARAMETER TEST)
    TEST_FUNCTION_PARM(Success case);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuMatch(hPuSet, qfpuMatchIdx, BGPM_INST_A_FORM_CODE(63,22), BGPM_INST_A_FORM_MASK, 2),0);
    TEST_FUNCTION_PARM(Invalid Event Set Handle);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuMatch(Dummy, qfpuMatchIdx, BGPM_INST_A_FORM_CODE(63,22), BGPM_INST_A_FORM_MASK, 2),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuMatch(hPuSet, Dummy, BGPM_INST_A_FORM_CODE(63,22), BGPM_INST_A_FORM_MASK, 2),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION_PARM(Invalid Scale);
    TEST_CHECK_EQUAL(Bgpm_SetQfpuMatch(hPuSet, qfpuMatchIdx, BGPM_INST_A_FORM_CODE(63,22), BGPM_INST_A_FORM_MASK,Dummy),BGPM_EINV_PARM);

    TEST_FUNCTION(GetQfpuMatch() PARAMETER TEST)
    TEST_FUNCTION_PARM(Success case);
    Bgpm_GetQfpuMatch(hPuSet, qfpuMatchIdx, &match, &mask, &scale);
    TEST_CHECK_EQUAL(match, BGPM_INST_A_FORM_CODE(63,22));
    TEST_CHECK_EQUAL(mask, BGPM_INST_A_FORM_MASK);
    TEST_CHECK_EQUAL(scale, 2);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetQfpuMatch(Dummy, qfpuMatchIdx, &match, &mask, &scale),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_GetQfpuMatch(hPuSet,Dummy , &match, &mask, &scale),BGPM_EINV_EVT_IDX);
    TEST_FUNCTION(GetQfpuFp() PARAMETER TEST)
    TEST_FUNCTION_PARM(Success Case);
    ushort countFp;
    Bgpm_GetQfpuFp(hPuSet, qfpuMatchIdx, &countFp);
    TEST_CHECK_EQUAL(countFp, BGPM_TRUE);
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_GetQfpuFp(Dummy, qfpuMatchIdx,&countFp ),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid event index);
    TEST_CHECK_EQUAL(Bgpm_GetQfpuFp(hPuSet,Dummy,&countFp),BGPM_EINV_EVT_IDX);

    TEST_FUNCTION(SetFpSqrScale() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_SetFpSqrScale(hPuSet,2),0);
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_SetFpSqrScale(Dummy,2),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Scale Value);
    TEST_CHECK_EQUAL(Bgpm_SetFpSqrScale(hPuSet,Dummy),BGPM_EINV_PARM);

    TEST_FUNCTION(GetFpSqrScale() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetFpSqrScale(hPuSet,&scale),0);
    TEST_CHECK_EQUAL(scale, 2);
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_GetFpSqrScale(Dummy,&scale),BGPM_EINV_SET_HANDLE);

    TEST_FUNCTION(SetFpDivScale() PARAMETER TEST);
    TEST_FUNCTION_PARM(Scale already reserved for the core);
    TEST_CHECK_EQUAL(Bgpm_SetFpDivScale(hPuSet,1),BGPM_EDIV_SCALE_MISMATCH); //Scale already reserved for this core
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_SetFpDivScale(Dummy,2),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Scale Value);
    TEST_CHECK_EQUAL(Bgpm_SetFpDivScale(hPuSet,Dummy),BGPM_EINV_PARM);
    TEST_FUNCTION(GetFpDivScale() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetFpDivScale(hPuSet,&scale),0);
    TEST_CHECK_EQUAL(scale,0);                            // Scale didn't get ser here earlier , expecting 0.
    TEST_FUNCTION_PARM(Invalid Event handle);
    TEST_CHECK_EQUAL(Bgpm_GetFpDivScale(Dummy,&scale),BGPM_EINV_SET_HANDLE);
}

void PunitMiscOverrides_test(int hPuSet)
{

    TEST_FUNCTION(SetEventEdge() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    int EvtEdgIdx = Bgpm_GetEventIndex(hPuSet, PEVT_XU_ANY_FLUSH, 0);
    TEST_CHECK_EQUAL(Bgpm_SetEventEdge(hPuSet,EvtEdgIdx,BGPM_TRUE),0);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_SetEventEdge(Dummy,EvtEdgIdx,BGPM_TRUE),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_SetEventEdge(hPuSet,Dummy,BGPM_TRUE),BGPM_EINV_EVT_IDX);
    // TEST_FUNCTION_PARM(Invalid Event Edge); // Cann;t test as edge(-1) will be interpreted ad valied edge value.
    // TEST_CHECK_EQUAL(Bgpm_SetEventEdge(hPuSet,EvtEdgIdx,Dummy),BGPM_EINV_PARM);

    TEST_FUNCTION(GetEventEdge() PARAMETER TEST);
    ushort edge;
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetEventEdge(hPuSet,EvtEdgIdx,&edge),0);
    TEST_CHECK_EQUAL(edge,1);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetEventEdge(Dummy,EvtEdgIdx,&edge),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_GetEventEdge(hPuSet,Dummy,&edge),BGPM_EINV_EVT_IDX);

    TEST_FUNCTION(SetEventInvert() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success  Case);
    TEST_CHECK_EQUAL(Bgpm_SetEventInvert(hPuSet,EvtEdgIdx,BGPM_TRUE),0);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_SetEventInvert(Dummy,EvtEdgIdx,BGPM_TRUE),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_SetEventInvert(hPuSet,Dummy,BGPM_TRUE),BGPM_EINV_EVT_IDX);
    //TEST_FUNCTION_PARM(Invalid Event Invert); // Cann't test as Invert(-1) will be interpreted ad valied invert value.
    //TEST_CHECK_EQUAL(Bgpm_SetEventInvert(hPuSet,EvtEdgIdx,Dummy),BGPM_EINV_PARM);

    TEST_FUNCTION(GetEventInvert() PARAMETER TEST);
    ushort invert;
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_GetEventInvert(hPuSet,EvtEdgIdx,&invert),0);
    TEST_CHECK_EQUAL(invert,1);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL(Bgpm_GetEventInvert(Dummy,EvtEdgIdx,&invert),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_GetEventInvert(hPuSet,Dummy,&invert),BGPM_EINV_EVT_IDX);
}
