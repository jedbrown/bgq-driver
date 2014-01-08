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


//************************************************************************
// WARNING: This is an script generated file - do not manually change.
//   The file is generated from bgpm/events/Events-*.xsl
//   based on the data in Events.xml and Events-map.xml and bgpm_event_tips.htpl
//
// make events
//   generates the bgpm_events* files and copies to appropriate directories for
//   manual SVN merge.
//
//************************************************************************



    

#define UPCI_EVENTS_C
#include "spi/include/upci/upc_p.h"
#include "spi/include/upci/upc_evttbl.h"
#include "spi/include/upci/upc_nw.h"

//! \addtogroup upci_spi
//@{ 
/**
 *
 * \file
 *
 * \brief UPCI Event Tables 
 *
 * The initialized tables of supported events and their attributes.  
 *
 * \note: This file is originally generated from 
 *     bgpm/events/Events-CrtCTable.xsl and
 *     bgpm/events/Events/Events.xml
 *     requiring an XSLT 2.0 processor.
 *
 */
//@}


  

    
    
const UPC_PunitEvtTableRec_t punitEvtTbl[] =
{
// id                             , label                             , evtSrc         , selVal , scope              , x1Dmn , cycle , invert , reserve1 , metric               , reserve2 , ctrMask  , t0Grp, t0Ctr, t1Grp, t1Ctr, t2Grp, t2Ctr, t3Grp, t3Ctr, desc } ,


{  PEVT_UNDEF                    , "PEVT_UNDEF"                       , UPC_ES_Undef   , 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x000000UL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "undefined" },    

{  PEVT_AXU_INSTR_COMMIT         , "PEVT_AXU_INSTR_COMMIT"            , UPC_ES_A2_AXU  , 0     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU Instruction Committed" },    

{  PEVT_AXU_CR_COMMIT            , "PEVT_AXU_CR_COMMIT"               , UPC_ES_A2_AXU  , 1     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU CR Instruction Committed" },    

{  PEVT_AXU_IDLE                 , "PEVT_AXU_IDLE"                    , UPC_ES_A2_AXU  , 2     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU Idle" },    

{  PEVT_AXU_FP_DS_ACTIVE         , "PEVT_AXU_FP_DS_ACTIVE"            , UPC_ES_A2_AXU  , 3     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU FP Divide or Square root in progress" },    

{  PEVT_AXU_FP_DS_ACTIVE_CYC     , "PEVT_AXU_FP_DS_ACTIVE_CYC"        , UPC_ES_A2_AXU  , 3     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU FP Divide or Square root in progress cycles" },    

{  PEVT_AXU_DENORM_FLUSH         , "PEVT_AXU_DENORM_FLUSH"            , UPC_ES_A2_AXU  , 4     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU Denormal Operand flush" },    

{  PEVT_AXU_UCODE_OPS_COMMIT     , "PEVT_AXU_UCODE_OPS_COMMIT"        , UPC_ES_A2_AXU  , 5     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU uCode Operations Committed" },    

{  PEVT_AXU_FP_EXCEPT            , "PEVT_AXU_FP_EXCEPT"               , UPC_ES_A2_AXU  , 6     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU Floating Point Exception" },    

{  PEVT_AXU_FP_ENAB_EXCEPT       , "PEVT_AXU_FP_ENAB_EXCEPT"          , UPC_ES_A2_AXU  , 7     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x20, 0x020, 0x20, 0xC00, 0x20, 0x300, 0x20, "AXU Floating Point Enabled Exception" },    

{  PEVT_IU_IL1_MISS              , "PEVT_IU_IL1_MISS"                 , UPC_ES_A2_IU   , 0     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IL1 Miss" },    

{  PEVT_IU_IL1_MISS_CYC          , "PEVT_IU_IL1_MISS_CYC"             , UPC_ES_A2_IU   , 0     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IL1 Miss cycles" },    

{  PEVT_IU_IL1_RELOADS_DROPPED   , "PEVT_IU_IL1_RELOADS_DROPPED"      , UPC_ES_A2_IU   , 1     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IL1 Reloads Dropped" },    

{  PEVT_IU_RELOAD_COLLISIONS     , "PEVT_IU_RELOAD_COLLISIONS"        , UPC_ES_A2_IU   , 2     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Reload Collisions" },    

{  PEVT_IU_RELOAD_COLLISIONS_CYC , "PEVT_IU_RELOAD_COLLISIONS_CYC"    , UPC_ES_A2_IU   , 2     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Reload Collisions cycles" },    

{  PEVT_IU_IU0_REDIR_CYC         , "PEVT_IU_IU0_REDIR_CYC"            , UPC_ES_A2_IU   , 3     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IU0 Redirected cycles" },    

{  PEVT_IU_IERAT_MISS            , "PEVT_IU_IERAT_MISS"               , UPC_ES_A2_IU   , 4     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IERAT Miss" },    

{  PEVT_IU_IERAT_MISS_CYC        , "PEVT_IU_IERAT_MISS_CYC"           , UPC_ES_A2_IU   , 4     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IERAT Miss cycles" },    

{  PEVT_IU_ICACHE_FETCH          , "PEVT_IU_ICACHE_FETCH"             , UPC_ES_A2_IU   , 5     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "ICache Fetch" },    

{  PEVT_IU_ICACHE_FETCH_CYC      , "PEVT_IU_ICACHE_FETCH_CYC"         , UPC_ES_A2_IU   , 5     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "ICache Fetch cycles" },    

{  PEVT_IU_INSTR_FETCHED         , "PEVT_IU_INSTR_FETCHED"            , UPC_ES_A2_IU   , 6     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Instructions Fetched" },    

{  PEVT_IU_INSTR_FETCHED_CYC     , "PEVT_IU_INSTR_FETCHED_CYC"        , UPC_ES_A2_IU   , 6     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Instructions Fetched cycles" },    

{  PEVT_IU_RSV_ANY_L2_BACK_INV   , "PEVT_IU_RSV_ANY_L2_BACK_INV"      , UPC_ES_A2_IU   , 8     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "reserved any L2 Back Invalidates" },    

{  PEVT_IU_RSV_ANY_L2_BACK_INV_CYC, "PEVT_IU_RSV_ANY_L2_BACK_INV_CYC"  , UPC_ES_A2_IU   , 8     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "reserved any L2 Back Invalidates cycles" },    

{  PEVT_IU_L2_BACK_INV_HITS      , "PEVT_IU_L2_BACK_INV_HITS"         , UPC_ES_A2_IU   , 9     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "L2 Back Invalidates - Hits" },    

{  PEVT_IU_L2_BACK_INV_HITS_CYC  , "PEVT_IU_L2_BACK_INV_HITS_CYC"     , UPC_ES_A2_IU   , 9     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "L2 Back Invalidates - Hits cycles" },    

{  PEVT_IU_IBUFF_EMPTY           , "PEVT_IU_IBUFF_EMPTY"              , UPC_ES_A2_IU   , 10    , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IBuff Empty" },    

{  PEVT_IU_IBUFF_EMPTY_CYC       , "PEVT_IU_IBUFF_EMPTY_CYC"          , UPC_ES_A2_IU   , 10    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IBuff Empty cycles" },    

{  PEVT_IU_IBUFF_FLUSH           , "PEVT_IU_IBUFF_FLUSH"              , UPC_ES_A2_IU   , 11    , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IBuff Flush" },    

{  PEVT_IU_IBUFF_FLUSH_CYC       , "PEVT_IU_IBUFF_FLUSH_CYC"          , UPC_ES_A2_IU   , 11    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IBuff Flush cycles" },    

{  PEVT_IU_IS1_STALL_CYC         , "PEVT_IU_IS1_STALL_CYC"            , UPC_ES_A2_IU   , 12    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IU5 Stage Register Dependency Stall" },    

{  PEVT_IU_IS2_STALL_CYC         , "PEVT_IU_IS2_STALL_CYC"            , UPC_ES_A2_IU   , 13    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "IU6 Instruction Issue Stall" },    

{  PEVT_IU_BARRIER_OP_STALL_CYC  , "PEVT_IU_BARRIER_OP_STALL_CYC"     , UPC_ES_A2_IU   , 14    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Barrier Op Stall" },    

{  PEVT_IU_SLOW_SPR_ACCESS_CYC   , "PEVT_IU_SLOW_SPR_ACCESS_CYC"      , UPC_ES_A2_IU   , 15    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Slow SPR Access" },    

{  PEVT_IU_RAW_DEP_HIT_CYC       , "PEVT_IU_RAW_DEP_HIT_CYC"          , UPC_ES_A2_IU   , 16    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "RAW Dep Hit" },    

{  PEVT_IU_WAW_DEP_HIT_CYC       , "PEVT_IU_WAW_DEP_HIT_CYC"          , UPC_ES_A2_IU   , 17    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "WAW Dep Hit" },    

{  PEVT_IU_SYNC_DEP_HIT_CYC      , "PEVT_IU_SYNC_DEP_HIT_CYC"         , UPC_ES_A2_IU   , 18    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Sync Dep Hit" },    

{  PEVT_IU_SPR_DEP_HIT_CYC       , "PEVT_IU_SPR_DEP_HIT_CYC"          , UPC_ES_A2_IU   , 19    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "SPR Dep Hit" },    

{  PEVT_IU_AXU_DEP_HIT_CYC       , "PEVT_IU_AXU_DEP_HIT_CYC"          , UPC_ES_A2_IU   , 20    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "AXU Dep Hit" },    

{  PEVT_IU_FXU_DEP_HIT_CYC       , "PEVT_IU_FXU_DEP_HIT_CYC"          , UPC_ES_A2_IU   , 21    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "FXU Dep Hit" },    

{  PEVT_IU_AXU_FXU_DEP_HIT_CYC   , "PEVT_IU_AXU_FXU_DEP_HIT_CYC"      , UPC_ES_A2_IU   , 22    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "AXU CR Instruction Committed" },    

{  PEVT_IU_AXU_ISSUE_PRI_LOSS_CYC, "PEVT_IU_AXU_ISSUE_PRI_LOSS_CYC"   , UPC_ES_A2_IU   , 25    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "AXU Issue Priority Loss" },    

{  PEVT_IU_FXU_ISSUE_PRI_LOSS_CYC, "PEVT_IU_FXU_ISSUE_PRI_LOSS_CYC"   , UPC_ES_A2_IU   , 26    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "FXU Issue Priority Loss" },    

{  PEVT_IU_FXU_ISSUE_COUNT       , "PEVT_IU_FXU_ISSUE_COUNT"          , UPC_ES_A2_IU   , 28    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "FXU Issue Count" },    

{  PEVT_IU_TOT_ISSUE_COUNT       , "PEVT_IU_TOT_ISSUE_COUNT"          , UPC_ES_A2_IU   , 29    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x08, 0x020, 0x08, 0xC00, 0x08, 0x300, 0x08, "Total Issue Count" },    

{  PEVT_XU_PROC_BUSY             , "PEVT_XU_PROC_BUSY"                , UPC_ES_A2_XU   , 0     , UPC_SCOPE_CORESHARED, 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000088UL, 0x002, 0xC0, 0    , 0   , 0    , 0   , 0    , 0   , "Processor Busy" },    

{  PEVT_XU_BR_COMMIT_CORE        , "PEVT_XU_BR_COMMIT_CORE"           , UPC_ES_A2_XU   , 0     , UPC_SCOPE_CORESHARED, 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000044UL, 0    , 0   , 0x001, 0xC0, 0    , 0   , 0    , 0   , "Branch Commit" },    

{  PEVT_XU_BR_MISPRED_COMMIT_CORE, "PEVT_XU_BR_MISPRED_COMMIT_CORE"   , UPC_ES_A2_XU   , 0     , UPC_SCOPE_CORESHARED, 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x000022UL, 0    , 0   , 0    , 0   , 0x008, 0xC0, 0    , 0   , "Branch Mispredict Commit" },    

{  PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT_CORE, "PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT_CORE", UPC_ES_A2_XU   , 0     , UPC_SCOPE_CORESHARED, 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x000011UL, 0    , 0   , 0    , 0   , 0    , 0   , 0x004, 0xC0, "Branch Target Address Mispredict Commit" },    

{  PEVT_XU_THRD_RUNNING          , "PEVT_XU_THRD_RUNNING"             , UPC_ES_A2_XU   , 1     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Thread Running" },    

{  PEVT_XU_TIMEBASE_TICK         , "PEVT_XU_TIMEBASE_TICK"            , UPC_ES_A2_XU   , 2     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Timebase Tick" },    

{  PEVT_XU_SPR_READ_COMMIT       , "PEVT_XU_SPR_READ_COMMIT"          , UPC_ES_A2_XU   , 3     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "SPR Read Commit" },    

{  PEVT_XU_SPR_WRITE_COMMIT      , "PEVT_XU_SPR_WRITE_COMMIT"         , UPC_ES_A2_XU   , 4     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "SPR Write Commit" },    

{  PEVT_XU_STALLED_ON_WAITRSV    , "PEVT_XU_STALLED_ON_WAITRSV"       , UPC_ES_A2_XU   , 5     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Cycles stalled on waitrsv" },    

{  PEVT_XU_STALLED_ON_WAITRSV_CYC, "PEVT_XU_STALLED_ON_WAITRSV_CYC"   , UPC_ES_A2_XU   , 5     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Cycles stalled on waitrsv cycles" },    

{  PEVT_XU_EXT_INT_ASSERT        , "PEVT_XU_EXT_INT_ASSERT"           , UPC_ES_A2_XU   , 6     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "External Interrupt Asserted" },    

{  PEVT_XU_CRIT_EXT_INT_ASSERT   , "PEVT_XU_CRIT_EXT_INT_ASSERT"      , UPC_ES_A2_XU   , 7     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Critical External Interrupt Asserted" },    

{  PEVT_XU_PERF_MON_INT_ASSERT   , "PEVT_XU_PERF_MON_INT_ASSERT"      , UPC_ES_A2_XU   , 8     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Performance Monitor Interrupt Asserted" },    

{  PEVT_XU_PPC_COMMIT            , "PEVT_XU_PPC_COMMIT"               , UPC_ES_A2_XU   , 9     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "PPC Commit" },    

{  PEVT_XU_COMMIT                , "PEVT_XU_COMMIT"                   , UPC_ES_A2_XU   , 10    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "XU Operations Committed" },    

{  PEVT_XU_UCODE_COMMIT          , "PEVT_XU_UCODE_COMMIT"             , UPC_ES_A2_XU   , 11    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "uCode Sequence Commit" },    

{  PEVT_XU_ANY_FLUSH             , "PEVT_XU_ANY_FLUSH"                , UPC_ES_A2_XU   , 12    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Any Flush" },    

{  PEVT_XU_BR_COMMIT             , "PEVT_XU_BR_COMMIT"                , UPC_ES_A2_XU   , 13    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Branch Commit" },    

{  PEVT_XU_BR_MISPRED_COMMIT     , "PEVT_XU_BR_MISPRED_COMMIT"        , UPC_ES_A2_XU   , 14    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Branch Mispredict Commit" },    

{  PEVT_XU_BR_TAKEN_COMMIT       , "PEVT_XU_BR_TAKEN_COMMIT"          , UPC_ES_A2_XU   , 15    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Branch Taken Commit" },    

{  PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT, "PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT", UPC_ES_A2_XU   , 16    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Branch Target Address Mispredict Commit" },    

{  PEVT_XU_MULT_DIV_COLLISION    , "PEVT_XU_MULT_DIV_COLLISION"       , UPC_ES_A2_XU   , 17    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Mult/Div Collision" },    

{  PEVT_XU_EXT_INT_PEND          , "PEVT_XU_EXT_INT_PEND"             , UPC_ES_A2_XU   , 18    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "External Interrupt Pending" },    

{  PEVT_XU_CRIT_EXT_INT_PEND     , "PEVT_XU_CRIT_EXT_INT_PEND"        , UPC_ES_A2_XU   , 19    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Critical External Interrupt Pending" },    

{  PEVT_XU_PERF_MON_INT_PEND     , "PEVT_XU_PERF_MON_INT_PEND"        , UPC_ES_A2_XU   , 20    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Performance Monitor Interrupt Pending" },    

{  PEVT_XU_RUN_INSTR             , "PEVT_XU_RUN_INSTR"                , UPC_ES_A2_XU   , 22    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Run Instructions" },    

{  PEVT_XU_INTS_TAKEN            , "PEVT_XU_INTS_TAKEN"               , UPC_ES_A2_XU   , 23    , UPC_SCOPE_CORE      , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Interrupts Taken" },    

{  PEVT_XU_EXT_INT_TAKEN         , "PEVT_XU_EXT_INT_TAKEN"            , UPC_ES_A2_XU   , 24    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "External Interrupt Taken" },    

{  PEVT_XU_CRIT_EXT_INT_TAKEN    , "PEVT_XU_CRIT_EXT_INT_TAKEN"       , UPC_ES_A2_XU   , 25    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Critical External Interrupt Taken" },    

{  PEVT_XU_PERF_MON_INT_TAKEN    , "PEVT_XU_PERF_MON_INT_TAKEN"       , UPC_ES_A2_XU   , 26    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Performance Monitor Interrupt Taken" },    

{  PEVT_XU_DOORBELL_INT_TAKEN    , "PEVT_XU_DOORBELL_INT_TAKEN"       , UPC_ES_A2_XU   , 27    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Doorbell Interrupt Taken" },    

{  PEVT_XU_STCX_FAIL             , "PEVT_XU_STCX_FAIL"                , UPC_ES_A2_XU   , 28    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "stcx fail" },    

{  PEVT_XU_ICSWX_FAILED          , "PEVT_XU_ICSWX_FAILED"             , UPC_ES_A2_XU   , 29    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "icswx failed" },    

{  PEVT_XU_ICSWX_COMMIT          , "PEVT_XU_ICSWX_COMMIT"             , UPC_ES_A2_XU   , 30    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "icswx commit" },    

{  PEVT_XU_MULT_DIV_BUSY         , "PEVT_XU_MULT_DIV_BUSY"            , UPC_ES_A2_XU   , 31    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000FFUL, 0x002, 0xC0, 0x001, 0xC0, 0x008, 0xC0, 0x004, 0xC0, "Mult/Div Busy" },    

{  PEVT_LSU_COMMIT_STS           , "PEVT_LSU_COMMIT_STS"              , UPC_ES_A2_LSU  , 0     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Stores" },    

{  PEVT_LSU_COMMIT_ST_MISSES     , "PEVT_LSU_COMMIT_ST_MISSES"        , UPC_ES_A2_LSU  , 1     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Store Misses" },    

{  PEVT_LSU_COMMIT_LD_MISSES     , "PEVT_LSU_COMMIT_LD_MISSES"        , UPC_ES_A2_LSU  , 2     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Load Misses" },    

{  PEVT_LSU_COMMIT_CACHE_INHIB_LD_MISSES, "PEVT_LSU_COMMIT_CACHE_INHIB_LD_MISSES", UPC_ES_A2_LSU  , 3     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Cache-Inhibited Load Misses" },    

{  PEVT_LSU_COMMIT_CACHEABLE_LDS , "PEVT_LSU_COMMIT_CACHEABLE_LDS"    , UPC_ES_A2_LSU  , 4     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Cacheable Loads" },    

{  PEVT_LSU_COMMIT_DCBT_MISSES   , "PEVT_LSU_COMMIT_DCBT_MISSES"      , UPC_ES_A2_LSU  , 5     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed DCBT Misses" },    

{  PEVT_LSU_COMMIT_DCBT_HITS     , "PEVT_LSU_COMMIT_DCBT_HITS"        , UPC_ES_A2_LSU  , 6     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed DCBT Hits" },    

{  PEVT_LSU_COMMIT_AXU_LDS       , "PEVT_LSU_COMMIT_AXU_LDS"          , UPC_ES_A2_LSU  , 7     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed AXU Loads" },    

{  PEVT_LSU_COMMIT_AXU_STS       , "PEVT_LSU_COMMIT_AXU_STS"          , UPC_ES_A2_LSU  , 8     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed AXU Stores" },    

{  PEVT_LSU_COMMIT_STCX          , "PEVT_LSU_COMMIT_STCX"             , UPC_ES_A2_LSU  , 9     , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed STCX" },    

{  PEVT_LSU_COMMIT_WCLR          , "PEVT_LSU_COMMIT_WCLR"             , UPC_ES_A2_LSU  , 10    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed WCLR" },    

{  PEVT_LSU_COMMIT_WCLR_WL       , "PEVT_LSU_COMMIT_WCLR_WL"          , UPC_ES_A2_LSU  , 11    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed WCLR Watchlost" },    

{  PEVT_LSU_COMMIT_LDAWX         , "PEVT_LSU_COMMIT_LDAWX"            , UPC_ES_A2_LSU  , 12    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed LDAWX" },    

{  PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH, "PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH", UPC_ES_A2_LSU  , 13    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Unsupported Alignment Flush" },    

{  PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH, "PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH", UPC_ES_A2_LSU  , 14    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Reload Resource Conflict Flush" },    

{  PEVT_LSU_COMMIT_DUPLICATE_LDAWX, "PEVT_LSU_COMMIT_DUPLICATE_LDAWX"  , UPC_ES_A2_LSU  , 15    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Duplicate LDAWX" },    

{  PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH, "PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH", UPC_ES_A2_LSU  , 16    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Inter-Thread Directory Access Flush" },    

{  PEVT_LSU_LMQ_DEPENDENCY_FLUSH , "PEVT_LSU_LMQ_DEPENDENCY_FLUSH"    , UPC_ES_A2_LSU  , 17    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Load Miss Queue Dependency Flush" },    

{  PEVT_LSU_COMMIT_WCHKALL       , "PEVT_LSU_COMMIT_WCHKALL"          , UPC_ES_A2_LSU  , 18    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed WCHKALL" },    

{  PEVT_LSU_COMMIT_SUCC_WCHKALL  , "PEVT_LSU_COMMIT_SUCC_WCHKALL"     , UPC_ES_A2_LSU  , 19    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Committed Successful WCHKALL" },    

{  PEVT_LSU_LD_MISS_Q_FULL_FLUSH , "PEVT_LSU_LD_MISS_Q_FULL_FLUSH"    , UPC_ES_A2_LSU  , 20    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Load Miss Queue Full Flush" },    

{  PEVT_LSU_ST_Q_FULL_FLUSH      , "PEVT_LSU_ST_Q_FULL_FLUSH"         , UPC_ES_A2_LSU  , 21    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Store Queue Full Flush" },    

{  PEVT_LSU_HIT_LD_FLUSH         , "PEVT_LSU_HIT_LD_FLUSH"            , UPC_ES_A2_LSU  , 22    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Hit Against Outstanding Load Flush" },    

{  PEVT_LSU_HIT_IG1_REQ_FLUSH    , "PEVT_LSU_HIT_IG1_REQ_FLUSH"       , UPC_ES_A2_LSU  , 23    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Hit Against Outstanding I=G=1 Request Flush" },    

{  PEVT_LSU_LARX_FINISHED        , "PEVT_LSU_LARX_FINISHED"           , UPC_ES_A2_LSU  , 24    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "LARX Finished" },    

{  PEVT_LSU_INTER_THRD_ST_WATCH_LOST, "PEVT_LSU_INTER_THRD_ST_WATCH_LOST", UPC_ES_A2_LSU  , 25    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Inter-Thread Store Set Watch Lost Indicator" },    

{  PEVT_LSU_RELOAD_WATCH_LOST    , "PEVT_LSU_RELOAD_WATCH_LOST"       , UPC_ES_A2_LSU  , 26    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Reload Set Watch Lost Indicator" },    

{  PEVT_LSU_BACK_INV_WATCH_LOST  , "PEVT_LSU_BACK_INV_WATCH_LOST"     , UPC_ES_A2_LSU  , 27    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "Back-Invalidate Set Watch Lost Indicator" },    

{  PEVT_LSU_L1_DCACHE_BACK_INVAL , "PEVT_LSU_L1_DCACHE_BACK_INVAL"    , UPC_ES_A2_LSU  , 28    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "L1 Data Cache Back-Invalidate" },    

{  PEVT_LSU_L1_DCACHE_BACK_INVAL_HITS, "PEVT_LSU_L1_DCACHE_BACK_INVAL_HITS", UPC_ES_A2_LSU  , 29    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "L1 Data Cache Back-Invalidate Hits" },    

{  PEVT_LSU_L1_CACHE_PTYERR_DETECTED, "PEVT_LSU_L1_CACHE_PTYERR_DETECTED", UPC_ES_A2_LSU  , 30    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CONSEQ   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "L1 Cache Parity Error Detected" },    

{  PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC, "PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC"   , UPC_ES_A2_LSU  , 31    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0x0C0, 0x02, 0x020, 0x02, 0xC00, 0x02, 0x300, 0x02, "(event is broken) Load Latency memory subsystem" },    

{  PEVT_MMU_TLB_HIT_DIRECT_IERAT , "PEVT_MMU_TLB_HIT_DIRECT_IERAT"    , UPC_ES_A2_MMU  , 0     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "TLB hit direct after ierat miss" },    

{  PEVT_MMU_TLB_MISS_DIRECT_IERAT, "PEVT_MMU_TLB_MISS_DIRECT_IERAT"   , UPC_ES_A2_MMU  , 1     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "TLB miss direct after ierat miss" },    

{  PEVT_MMU_TLB_MISS_INDIR_IERAT , "PEVT_MMU_TLB_MISS_INDIR_IERAT"    , UPC_ES_A2_MMU  , 2     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "TLB miss indirect after ierat miss" },    

{  PEVT_MMU_HTW_HIT_IERAT        , "PEVT_MMU_HTW_HIT_IERAT"           , UPC_ES_A2_MMU  , 3     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Hardware Table walk hit after ierat miss" },    

{  PEVT_MMU_HTW_MISS_IERAT       , "PEVT_MMU_HTW_MISS_IERAT"          , UPC_ES_A2_MMU  , 4     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Hardware Table walk miss after ierat miss" },    

{  PEVT_MMU_TLB_HIT_DIRECT_DERAT , "PEVT_MMU_TLB_HIT_DIRECT_DERAT"    , UPC_ES_A2_MMU  , 5     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "TLB hit direct after derat miss" },    

{  PEVT_MMU_TLB_MISS_DIRECT_DERAT, "PEVT_MMU_TLB_MISS_DIRECT_DERAT"   , UPC_ES_A2_MMU  , 6     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "TLB miss direct after derat miss" },    

{  PEVT_MMU_TLB_MISS_INDIR_DERAT , "PEVT_MMU_TLB_MISS_INDIR_DERAT"    , UPC_ES_A2_MMU  , 7     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "TLB miss indirect after derat miss" },    

{  PEVT_MMU_HTW_HIT_DERAT        , "PEVT_MMU_HTW_HIT_DERAT"           , UPC_ES_A2_MMU  , 8     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Hardware Table walk hit after derat miss" },    

{  PEVT_MMU_HTW_MISS_DERAT       , "PEVT_MMU_HTW_MISS_DERAT"          , UPC_ES_A2_MMU  , 9     , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Hardware Table walk miss after derat miss" },    

{  PEVT_MMU_IERAT_MISS           , "PEVT_MMU_IERAT_MISS"              , UPC_ES_A2_MMU  , 10    , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Ierat miss" },    

{  PEVT_MMU_IERAT_MISS_CYC       , "PEVT_MMU_IERAT_MISS_CYC"          , UPC_ES_A2_MMU  , 10    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Ierat miss cycles" },    

{  PEVT_MMU_DERAT_MISS           , "PEVT_MMU_DERAT_MISS"              , UPC_ES_A2_MMU  , 11    , UPC_SCOPE_THREAD    , 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Derat miss" },    

{  PEVT_MMU_DERAT_MISS_CYC       , "PEVT_MMU_DERAT_MISS_CYC"          , UPC_ES_A2_MMU  , 11    , UPC_SCOPE_THREAD    , 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xFFFFFFUL, 0x0C0, 0x80, 0x020, 0x80, 0xC00, 0x80, 0x300, 0x80, "Derat miss cycles" },    

{  PEVT_MMU_IERAT_MISS_TOT       , "PEVT_MMU_IERAT_MISS_TOT"          , UPC_ES_A2_MMU  , 12    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x888888UL, 0x0C0, 0x80, 0    , 0   , 0    , 0   , 0    , 0   , "Ierat miss total" },    

{  PEVT_MMU_DERAT_MISS_TOT       , "PEVT_MMU_DERAT_MISS_TOT"          , UPC_ES_A2_MMU  , 13    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x888888UL, 0x0C0, 0x80, 0    , 0   , 0    , 0   , 0    , 0   , "derat miss total" },    

{  PEVT_MMU_TLB_MISS_DIRECT_TOT  , "PEVT_MMU_TLB_MISS_DIRECT_TOT"     , UPC_ES_A2_MMU  , 14    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x888888UL, 0x0C0, 0x80, 0    , 0   , 0    , 0   , 0    , 0   , "tlb miss direct total" },    

{  PEVT_MMU_TLB_HIT_FIRSTSIZE_TOT, "PEVT_MMU_TLB_HIT_FIRSTSIZE_TOT"   , UPC_ES_A2_MMU  , 15    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x888888UL, 0x0C0, 0x80, 0    , 0   , 0    , 0   , 0    , 0   , "tlb hit firstsize total" },    

{  PEVT_MMU_TLB_HIT_INDIR_TOT    , "PEVT_MMU_TLB_HIT_INDIR_TOT"       , UPC_ES_A2_MMU  , 12    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x444444UL, 0    , 0   , 0x020, 0x80, 0    , 0   , 0    , 0   , "tlb hit indirect total" },    

{  PEVT_MMU_HTW_PTERELOAD_TOT    , "PEVT_MMU_HTW_PTERELOAD_TOT"       , UPC_ES_A2_MMU  , 13    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x444444UL, 0    , 0   , 0x020, 0x80, 0    , 0   , 0    , 0   , "htw ptereload total" },    

{  PEVT_MMU_LRAT_TRANS_TOT       , "PEVT_MMU_LRAT_TRANS_TOT"          , UPC_ES_A2_MMU  , 14    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x444444UL, 0    , 0   , 0x020, 0x80, 0    , 0   , 0    , 0   , "lrat translation total" },    

{  PEVT_MMU_LRAT_MISS_TOT        , "PEVT_MMU_LRAT_MISS_TOT"           , UPC_ES_A2_MMU  , 15    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x444444UL, 0    , 0   , 0x020, 0x80, 0    , 0   , 0    , 0   , "lrat miss total" },    

{  PEVT_MMU_PT_FAULT_TOT         , "PEVT_MMU_PT_FAULT_TOT"            , UPC_ES_A2_MMU  , 12    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x222222UL, 0    , 0   , 0    , 0   , 0xC00, 0x80, 0    , 0   , "pt fault total" },    

{  PEVT_MMU_PT_INELIG_TOT        , "PEVT_MMU_PT_INELIG_TOT"           , UPC_ES_A2_MMU  , 13    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x222222UL, 0    , 0   , 0    , 0   , 0xC00, 0x80, 0    , 0   , "pt inelig total" },    

{  PEVT_MMU_TLBWEC_FAIL_TOT      , "PEVT_MMU_TLBWEC_FAIL_TOT"         , UPC_ES_A2_MMU  , 14    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x222222UL, 0    , 0   , 0    , 0   , 0xC00, 0x80, 0    , 0   , "tlbwec fail total" },    

{  PEVT_MMU_TLBWEC_SUCC_TOT      , "PEVT_MMU_TLBWEC_SUCC_TOT"         , UPC_ES_A2_MMU  , 15    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x222222UL, 0    , 0   , 0    , 0   , 0xC00, 0x80, 0    , 0   , "tlbwec success total" },    

{  PEVT_MMU_TLBILX_SRC_TOT       , "PEVT_MMU_TLBILX_SRC_TOT"          , UPC_ES_A2_MMU  , 12    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x111111UL, 0    , 0   , 0    , 0   , 0    , 0   , 0x300, 0x80, "tlbilx local source total" },    

{  PEVT_MMU_TLBIVAX_SRC_TOT      , "PEVT_MMU_TLBIVAX_SRC_TOT"         , UPC_ES_A2_MMU  , 13    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x111111UL, 0    , 0   , 0    , 0   , 0    , 0   , 0x300, 0x80, "tlbivax local source total" },    

{  PEVT_MMU_TLBIVAX_SNOOP_TOT    , "PEVT_MMU_TLBIVAX_SNOOP_TOT"       , UPC_ES_A2_MMU  , 14    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_EVENTS   , 0        , 0x111111UL, 0    , 0   , 0    , 0   , 0    , 0   , 0x300, 0x80, "tlbivax snoop total" },    

{  PEVT_MMU_TLB_FLUSH_REQ_TOT    , "PEVT_MMU_TLB_FLUSH_REQ_TOT"       , UPC_ES_A2_MMU  , 15    , UPC_SCOPE_CORESHARED, 1     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x111111UL, 0    , 0   , 0    , 0   , 0    , 0   , 0x300, 0x80, "tlb flush req total" },    

{  PEVT_MMU_TLB_FLUSH_REQ_TOT_CYC, "PEVT_MMU_TLB_FLUSH_REQ_TOT_CYC"   , UPC_ES_A2_MMU  , 15    , UPC_SCOPE_CORESHARED, 1     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x111111UL, 0    , 0   , 0    , 0   , 0    , 0   , 0x300, 0x80, "tlb flush req total cycles" },    

{  PEVT_L1P_BAS_STRM_LINE_ESTB   , "PEVT_L1P_BAS_STRM_LINE_ESTB"      , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 0    , 0   , 2    , 0   , 4    , 0   , 6    , 0   , "L1p stream line establish" },    

{  PEVT_L1P_BAS_HIT              , "PEVT_L1P_BAS_HIT"                 , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 0    , 1   , 2    , 1   , 4    , 1   , 6    , 1   , "L1p hit " },    

{  PEVT_L1P_BAS_PF2DFC           , "PEVT_L1P_BAS_PF2DFC"              , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 0    , 2   , 2    , 2   , 4    , 2   , 6    , 2   , "L1p pf2dfc" },    

{  PEVT_L1P_BAS_MISS             , "PEVT_L1P_BAS_MISS"                , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 0    , 3   , 2    , 3   , 4    , 3   , 6    , 3   , "L1p miss" },    

{  PEVT_L1P_BAS_LU_DRAIN         , "PEVT_L1P_BAS_LU_DRAIN"            , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x0000F0UL, 0    , 4   , 2    , 4   , 4    , 4   , 6    , 4   , "L1p LU drain" },    

{  PEVT_L1P_BAS_LU_DRAIN_CYC     , "PEVT_L1P_BAS_LU_DRAIN_CYC"        , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x0000F0UL, 0    , 4   , 2    , 4   , 4    , 4   , 6    , 4   , "L1p LU drain Cyc" },    

{  PEVT_L1P_BAS_LD               , "PEVT_L1P_BAS_LD"                  , UPC_ES_L1P_BASE, 0     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 0    , 5   , 2    , 5   , 4    , 5   , 6    , 5   , "L1p Load" },    

{  PEVT_L1P_BAS_ST_WC            , "PEVT_L1P_BAS_ST_WC"               , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 0    , 6   , 0    , 7   , 2    , 6   , 2    , 7   , "L1p Store-WC" },    

{  PEVT_L1P_BAS_ST_32BYTE        , "PEVT_L1P_BAS_ST_32BYTE"           , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 1    , 0   , 3    , 0   , 5    , 0   , 7    , 0   , "L1p Store 32byte" },    

{  PEVT_L1P_BAS_ST_CRACKED       , "PEVT_L1P_BAS_ST_CRACKED"          , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 1    , 1   , 3    , 1   , 5    , 1   , 7    , 1   , "L1p Store-cracked" },    

{  PEVT_L1P_BAS_LU_STALL_SRT     , "PEVT_L1P_BAS_LU_STALL_SRT"        , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_CORE      , 0     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x000F00UL, 8    , 2   , 8    , 2   , 8    , 2   , 8    , 2   , "L1p LU stall-SRT" },    

{  PEVT_L1P_BAS_LU_STALL_SRT_CYC , "PEVT_L1P_BAS_LU_STALL_SRT_CYC"    , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x000F00UL, 8    , 2   , 8    , 2   , 8    , 2   , 8    , 2   , "L1p LU stall-SRT Cyc" },    

{  PEVT_L1P_BAS_LU_STALL_MMIO_DCR, "PEVT_L1P_BAS_LU_STALL_MMIO_DCR"   , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_CORE      , 0     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x0000F0UL, 9    , 2   , 9    , 2   , 9    , 2   , 9    , 2   , "L1p LU stall-MMIO/DCR" },    

{  PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC, "PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC", UPC_ES_L1P_BASE, 1     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x0000F0UL, 9    , 2   , 9    , 2   , 9    , 2   , 9    , 2   , "L1p LU stall-MMIO/DCR Cyc" },    

{  PEVT_L1P_BAS_LU_STALL_STRM_DET, "PEVT_L1P_BAS_LU_STALL_STRM_DET"   , UPC_ES_L1P_BASE, 1     , UPC_SCOPE_CORE      , 0     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x00000FUL, 10   , 2   , 10   , 2   , 10   , 2   , 10   , 2   , "L1p LU stall-stream detect" },    

{  PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC, "PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC", UPC_ES_L1P_BASE, 1     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x00000FUL, 10   , 2   , 10   , 2   , 10   , 2   , 10   , 2   , "L1p LU stall-stream detect Cyc" },    

{  PEVT_L1P_BAS_LU_STALL_LIST_RD , "PEVT_L1P_BAS_LU_STALL_LIST_RD"    , UPC_ES_L1P_BASE, 2     , UPC_SCOPE_CORE      , 0     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0xF00000UL, 11   , 2   , 11   , 2   , 11   , 2   , 11   , 2   , "L1p LU stall-list read" },    

{  PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC, "PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC", UPC_ES_L1P_BASE, 2     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0xF00000UL, 11   , 2   , 11   , 2   , 11   , 2   , 11   , 2   , "L1p LU stall-list read Cyc" },    

{  PEVT_L1P_BAS_ST               , "PEVT_L1P_BAS_ST"                  , UPC_ES_L1P_BASE, 2     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 1    , 2   , 3    , 2   , 5    , 2   , 7    , 2   , "L1p Store" },    

{  PEVT_L1P_BAS_LU_STALL_LIST_WRT, "PEVT_L1P_BAS_LU_STALL_LIST_WRT"   , UPC_ES_L1P_BASE, 2     , UPC_SCOPE_CORE      , 0     , 0     , 0      , 0        , UPC_METRIC_BEVENTS  , 0        , 0x00000FUL, 11   , 4   , 11   , 4   , 11   , 4   , 11   , 4   , "L1p LU stall-list write" },    

{  PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC, "PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC", UPC_ES_L1P_BASE, 2     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_BCYCLES  , 0        , 0x00000FUL, 11   , 4   , 11   , 4   , 11   , 4   , 11   , 4   , "L1p LU stall-list write Cyc" },    

{  PEVT_L1P_SW_MAS_SW_REQ_VAL    , "PEVT_L1P_SW_MAS_SW_REQ_VAL"       , UPC_ES_L1P_SWITCH, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 11   , 3   , 11   , 3   , 11   , 3   , 11   , 3   , "L1p switch master sw req valid" },    

{  PEVT_L1P_SW_MAS_SW_REQ_GATE   , "PEVT_L1P_SW_MAS_SW_REQ_GATE"      , UPC_ES_L1P_SWITCH, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 8    , 4   , 8    , 4   , 8    , 4   , 8    , 4   , "L1p switch master sw req gate" },    

{  PEVT_L1P_SW_MAS_SW_DATA_GATE  , "PEVT_L1P_SW_MAS_SW_DATA_GATE"     , UPC_ES_L1P_SWITCH, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 9    , 4   , 9    , 4   , 9    , 4   , 9    , 4   , "L1p switch master sw data gate" },    

{  PEVT_L1P_SW_SR_MAS_RD_VAL_2   , "PEVT_L1P_SW_SR_MAS_RD_VAL_2"      , UPC_ES_L1P_SWITCH, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 10   , 4   , 10   , 4   , 10   , 4   , 10   , 4   , "L1p switch sr master rd valid 2 " },    

{  PEVT_L1P_SW_SI_MAS_REQ_VAL_2  , "PEVT_L1P_SW_SI_MAS_REQ_VAL_2"     , UPC_ES_L1P_SWITCH, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 1    , 3   , 1    , 3   , 1    , 3   , 1    , 3   , "L1p switch si master req valid 2" },    

{  PEVT_L1P_SW_SW_MAS_SKED_VAL_2 , "PEVT_L1P_SW_SW_MAS_SKED_VAL_2"    , UPC_ES_L1P_SWITCH, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 3    , 3   , 3    , 3   , 3    , 3   , 3    , 3   , "L1p switch sw master sked valid 2" },    

{  PEVT_L1P_SW_SW_MAS_EAGER_VAL_2, "PEVT_L1P_SW_SW_MAS_EAGER_VAL_2"   , UPC_ES_L1P_SWITCH, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 5    , 3   , 5    , 3   , 5    , 3   , 5    , 3   , "L1p switch sw master eager valid 2" },    

{  PEVT_L1P_SW_TLB_FILL          , "PEVT_L1P_SW_TLB_FILL"             , UPC_ES_L1P_SWITCH, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 7    , 3   , 7    , 3   , 7    , 3   , 7    , 3   , "L1p switch tlb fill" },    

{  PEVT_L1P_SW_STWCX_FAIL        , "PEVT_L1P_SW_STWCX_FAIL"           , UPC_ES_L1P_SWITCH, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 1    , 4   , 3    , 4   , 5    , 4   , 7    , 4   , "L1p switch stwcx fail" },    

{  PEVT_L1P_SW_STWCX             , "PEVT_L1P_SW_STWCX"                , UPC_ES_L1P_SWITCH, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 1    , 5   , 3    , 5   , 5    , 5   , 7    , 5   , "L1p switch stwcx" },    

{  PEVT_L1P_SW_I_FETCH           , "PEVT_L1P_SW_I_FETCH"              , UPC_ES_L1P_SWITCH, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 1    , 6   , 3    , 6   , 5    , 6   , 7    , 6   , "L1p switch I-fetch" },    

{  PEVT_L1P_SW_MSYNC             , "PEVT_L1P_SW_MSYNC"                , UPC_ES_L1P_SWITCH, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 1    , 7   , 3    , 7   , 5    , 7   , 7    , 7   , "L1p switch Msync" },    

{  PEVT_L1P_SW_LWARX             , "PEVT_L1P_SW_LWARX"                , UPC_ES_L1P_SWITCH, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 8    , 0   , 9    , 0   , 10   , 0   , 11   , 0   , "L1p switch Lwarx" },    

{  PEVT_L1P_SW_KILL_L2_RSV       , "PEVT_L1P_SW_KILL_L2_RSV"          , UPC_ES_L1P_SWITCH, 5     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 8    , 1   , 8    , 1   , 8    , 1   , 8    , 1   , "L1p switch L1p killed L2 slice reserve" },    

{  PEVT_L1P_SW_L2_CANCEL_A2_RSV  , "PEVT_L1P_SW_L2_CANCEL_A2_RSV"     , UPC_ES_L1P_SWITCH, 5     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 9    , 1   , 9    , 1   , 9    , 1   , 9    , 1   , "L1p switch L2 slice cancelled A2 reserve" },    

{  PEVT_L1P_SW_L1_INVAL          , "PEVT_L1P_SW_L1_INVAL"             , UPC_ES_L1P_SWITCH, 5     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 10   , 1   , 10   , 1   , 10   , 1   , 10   , 1   , "L1p switch L1 inval" },    

{  PEVT_L1P_SW_WC_EVICT_ADDR     , "PEVT_L1P_SW_WC_EVICT_ADDR"        , UPC_ES_L1P_SWITCH, 5     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 11   , 1   , 11   , 1   , 11   , 1   , 11   , 1   , "L1p switch WC evict by nearby address" },    

{  PEVT_L1P_STRM_LINE_ESTB       , "PEVT_L1P_STRM_LINE_ESTB"          , UPC_ES_L1P_STREAM, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 8    , 3   , 8    , 3   , 8    , 3   , 8    , 3   , "L1p stream L1p single line established" },    

{  PEVT_L1P_STRM_HIT_FWD         , "PEVT_L1P_STRM_HIT_FWD"            , UPC_ES_L1P_STREAM, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 9    , 3   , 9    , 3   , 9    , 3   , 9    , 3   , "L1p stream L1p hit forwarded" },    

{  PEVT_L1P_STRM_L1_HIT_FWD      , "PEVT_L1P_STRM_L1_HIT_FWD"         , UPC_ES_L1P_STREAM, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 10   , 3   , 10   , 3   , 10   , 3   , 10   , 3   , "L1p stream L1 hit forwarded" },    

{  PEVT_L1P_STRM_EVICT_UNUSED    , "PEVT_L1P_STRM_EVICT_UNUSED"       , UPC_ES_L1P_STREAM, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 11   , 3   , 11   , 3   , 11   , 3   , 11   , 3   , "L1p stream L1p evict unused" },    

{  PEVT_L1P_STRM_EVICT_PART_USED , "PEVT_L1P_STRM_EVICT_PART_USED"    , UPC_ES_L1P_STREAM, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 8    , 4   , 8    , 4   , 8    , 4   , 8    , 4   , "L1p stream L1p evict partially used" },    

{  PEVT_L1P_STRM_REMOTE_INVAL_MATCH, "PEVT_L1P_STRM_REMOTE_INVAL_MATCH" , UPC_ES_L1P_STREAM, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 9    , 4   , 9    , 4   , 9    , 4   , 9    , 4   , "L1p stream L1p Remote Invalidate match" },    

{  PEVT_L1P_STRM_DONT_CACHE      , "PEVT_L1P_STRM_DONT_CACHE"         , UPC_ES_L1P_STREAM, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 10   , 4   , 10   , 4   , 10   , 4   , 10   , 4   , "L1p stream L1p don't cache" },    

{  PEVT_L1P_STRM_STRM_DEPTH_STEAL, "PEVT_L1P_STRM_STRM_DEPTH_STEAL"   , UPC_ES_L1P_STREAM, 5     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 1    , 3   , 3    , 3   , 5    , 3   , 7    , 3   , "L1p stream L1p Stream depth steal" },    

{  PEVT_L1P_STRM_STRM_ESTB       , "PEVT_L1P_STRM_STRM_ESTB"          , UPC_ES_L1P_STREAM, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 1    , 4   , 3    , 4   , 5    , 4   , 7    , 4   , "L1p stream L1p streams established" },    

{  PEVT_L1P_STRM_WRT_INVAL       , "PEVT_L1P_STRM_WRT_INVAL"          , UPC_ES_L1P_STREAM, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 1    , 5   , 3    , 5   , 5    , 5   , 7    , 5   , "L1p stream L1p Write Invalidate" },    

{  PEVT_L1P_STRM_LINE_ESTB_ALL_LIST, "PEVT_L1P_STRM_LINE_ESTB_ALL_LIST" , UPC_ES_L1P_STREAM, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 1    , 6   , 1    , 6   , 1    , 6   , 1    , 6   , "L1p stream L1p line establish (all list)" },    

{  PEVT_L1P_STRM_HIT_LIST        , "PEVT_L1P_STRM_HIT_LIST"           , UPC_ES_L1P_STREAM, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 1    , 7   , 3    , 7   , 5    , 7   , 7    , 7   , "L1p stream L1p hit (list)" },    

{  PEVT_L1P_STRM_PF2DFC_LIST     , "PEVT_L1P_STRM_PF2DFC_LIST"        , UPC_ES_L1P_STREAM, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 8    , 0   , 9    , 0   , 10   , 0   , 11   , 0   , "L1p stream L1p pf2dfc (list)" },    

{  PEVT_L1P_STRM_PART_INVAL_REFCH, "PEVT_L1P_STRM_PART_INVAL_REFCH"   , UPC_ES_L1P_STREAM, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 8    , 1   , 9    , 1   , 10   , 1   , 11   , 1   , "L1p stream L1p partial invalid refetched" },    

{  PEVT_L1P_LIST_SKIP_1          , "PEVT_L1P_LIST_SKIP_1"             , UPC_ES_L1P_LIST, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 8    , 3   , 8    , 3   , 8    , 3   , 8    , 3   , "L1p list skip by 1" },    

{  PEVT_L1P_LIST_SKIP_2          , "PEVT_L1P_LIST_SKIP_2"             , UPC_ES_L1P_LIST, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 9    , 3   , 9    , 3   , 9    , 3   , 9    , 3   , "L1p list skip by 2" },    

{  PEVT_L1P_LIST_SKIP_3          , "PEVT_L1P_LIST_SKIP_3"             , UPC_ES_L1P_LIST, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 10   , 3   , 10   , 3   , 10   , 3   , 10   , 3   , "L1p list skip by 3" },    

{  PEVT_L1P_LIST_SKIP_4          , "PEVT_L1P_LIST_SKIP_4"             , UPC_ES_L1P_LIST, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 11   , 3   , 11   , 3   , 11   , 3   , 11   , 3   , "L1p list skip by 4" },    

{  PEVT_L1P_LIST_SKIP_5          , "PEVT_L1P_LIST_SKIP_5"             , UPC_ES_L1P_LIST, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 8    , 4   , 8    , 4   , 8    , 4   , 8    , 4   , "L1p list skip by 5" },    

{  PEVT_L1P_LIST_SKIP_6          , "PEVT_L1P_LIST_SKIP_6"             , UPC_ES_L1P_LIST, 3     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 9    , 4   , 9    , 4   , 9    , 4   , 9    , 4   , "L1p list skip by 6" },    

{  PEVT_L1P_LIST_SKIP_7          , "PEVT_L1P_LIST_SKIP_7"             , UPC_ES_L1P_LIST, 4     , UPC_SCOPE_CORE      , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 10   , 4   , 10   , 4   , 10   , 4   , 10   , 4   , "L1p list skip by 7" },    

{  PEVT_L1P_LIST_ABANDON         , "PEVT_L1P_LIST_ABANDON"            , UPC_ES_L1P_LIST, 5     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 1    , 3   , 3    , 3   , 5    , 3   , 7    , 3   , "L1p list abandoned" },    

{  PEVT_L1P_LIST_CMP             , "PEVT_L1P_LIST_CMP"                , UPC_ES_L1P_LIST, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xF00000UL, 1    , 4   , 3    , 4   , 5    , 4   , 7    , 4   , "L1p list comparison" },    

{  PEVT_L1P_LIST_SKIP            , "PEVT_L1P_LIST_SKIP"               , UPC_ES_L1P_LIST, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0F0000UL, 1    , 5   , 3    , 5   , 5    , 5   , 7    , 5   , "L1p list skip" },    

{  PEVT_L1P_LIST_MISMATCH        , "PEVT_L1P_LIST_MISMATCH"           , UPC_ES_L1P_LIST, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 1    , 6   , 3    , 6   , 5    , 6   , 7    , 6   , "L1p list mismatch " },    

{  PEVT_L1P_LIST_STARTED         , "PEVT_L1P_LIST_STARTED"            , UPC_ES_L1P_LIST, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 1    , 7   , 3    , 7   , 5    , 7   , 7    , 7   , "L1p list started." },    

{  PEVT_L1P_LIST_OVF_MEM         , "PEVT_L1P_LIST_OVF_MEM"            , UPC_ES_L1P_LIST, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x0000F0UL, 8    , 0   , 9    , 0   , 10   , 0   , 11   , 0   , "L1p list overflow mem" },    

{  PEVT_L1P_LIST_CMP_OVRUN_PREFCH, "PEVT_L1P_LIST_CMP_OVRUN_PREFCH"   , UPC_ES_L1P_LIST, 6     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00000FUL, 8    , 1   , 9    , 1   , 10   , 1   , 11   , 1   , "L1p list cmp overrun prefetch" },    

{  PEVT_WAKE_HIT_10              , "PEVT_WAKE_HIT_10"                 , UPC_ES_WAKEUP  , 2     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x00F000UL, 8    , 5   , 9    , 5   , 10   , 5   , 11   , 5   , "Wakeup hit 10" },    

{  PEVT_WAKE_HIT_11              , "PEVT_WAKE_HIT_11"                 , UPC_ES_WAKEUP  , 2     , UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0x000F00UL, 8    , 6   , 9    , 6   , 10   , 6   , 11   , 6   , "Wakeup hit 11" },    

{  PEVT_CYCLES                   , "PEVT_CYCLES"                      , UPC_ES_OP_XU   , UPC_P_XU_OGRP_CYCLES, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_CYCLES   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "x1 cycles" },    

{  PEVT_INST_XU_MATCH            , "PEVT_INST_XU_MATCH"               , UPC_ES_OP_XU   , UPC_P_XU_OGRP_MATCH, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Match XU OpCode" },    

{  PEVT_INST_XU_GRP_MASK         , "PEVT_INST_XU_GRP_MASK"            , UPC_ES_OP_XU   , UPC_P_XU_OGRP_MASK, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "XU OpCode Mask Multiple Groups" },    

{  PEVT_INST_XU_FLD              , "PEVT_INST_XU_FLD"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_FLD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Load Instr Compl" },    

{  PEVT_INST_XU_FST              , "PEVT_INST_XU_FST"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_FST, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Store Instr Compl" },    

{  PEVT_INST_XU_QLD              , "PEVT_INST_XU_QLD"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_QLD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Load Instr Compl" },    

{  PEVT_INST_XU_QST              , "PEVT_INST_XU_QST"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_QST, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Store Instr Compl" },    

{  PEVT_INST_XU_BITS             , "PEVT_INST_XU_BITS"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_BITS, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Bit manipulation Instr Compl" },    

{  PEVT_INST_XU_BRC              , "PEVT_INST_XU_BRC"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_BRC, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Branch Conditional Instr Compl" },    

{  PEVT_INST_XU_BRU              , "PEVT_INST_XU_BRU"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_BRU, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Branch Unconditional Instr Compl" },    

{  PEVT_INST_XU_CINV             , "PEVT_INST_XU_CINV"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_CINV, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Cache Invalidate Instr Compl" },    

{  PEVT_INST_XU_CSTO             , "PEVT_INST_XU_CSTO"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_CSTO, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Cache Store Instr Compl" },    

{  PEVT_INST_XU_CTCH             , "PEVT_INST_XU_CTCH"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_CTCH, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Cache Touch Instr Compl" },    

{  PEVT_INST_XU_IADD             , "PEVT_INST_XU_IADD"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_IADD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Integer Arithmetic Instr Compl" },    

{  PEVT_INST_XU_ICMP             , "PEVT_INST_XU_ICMP"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_ICMP, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Compare Instr Compl" },    

{  PEVT_INST_XU_ICSW             , "PEVT_INST_XU_ICSW"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_ICSW, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Coprocessor Instr Compl" },    

{  PEVT_INST_XU_IDIV             , "PEVT_INST_XU_IDIV"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_IDIV, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "integer divide Instr Compl" },    

{  PEVT_INST_XU_ILOG             , "PEVT_INST_XU_ILOG"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_ILOG, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Logical Instr Compl" },    

{  PEVT_INST_XU_IMOV             , "PEVT_INST_XU_IMOV"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_IMOV, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quick Register move Instr Compl" },    

{  PEVT_INST_XU_IMUL             , "PEVT_INST_XU_IMUL"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_IMUL, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "integer multiply Instr Compl" },    

{  PEVT_INST_XU_INT              , "PEVT_INST_XU_INT"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_INT, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Interrupt/system call Instr Compl" },    

{  PEVT_INST_XU_LD               , "PEVT_INST_XU_LD"                  , UPC_ES_OP_XU   , UPC_P_XU_OGRP_LD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Load Instr Compl" },    

{  PEVT_INST_XU_LSCX             , "PEVT_INST_XU_LSCX"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_LSCX, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Load & Store reserve conditional Instr Compl" },    

{  PEVT_INST_XU_ST               , "PEVT_INST_XU_ST"                  , UPC_ES_OP_XU   , UPC_P_XU_OGRP_ST, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Store Instr Compl" },    

{  PEVT_INST_XU_SYNC             , "PEVT_INST_XU_SYNC"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_SYNC, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Context Sync Instr Compl" },    

{  PEVT_INST_XU_TLB              , "PEVT_INST_XU_TLB"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_TLB, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "TLB other Instr Compl" },    

{  PEVT_INST_XU_TLBI             , "PEVT_INST_XU_TLBI"                , UPC_ES_OP_XU   , UPC_P_XU_OGRP_TLBI, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "TLB Invalidate Instr Compl" },    

{  PEVT_INST_XU_WCH              , "PEVT_INST_XU_WCH"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_WCH, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Watch Instr Compl" },    

{  PEVT_INST_XU_ALL              , "PEVT_INST_XU_ALL"                 , UPC_ES_OP_XU   , UPC_P_XU_OGRP_ALL, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "All XU Instruction Completions" },    

{  PEVT_INST_QFPU_MATCH          , "PEVT_INST_QFPU_MATCH"             , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_MATCH, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "AXU Opcode Match" },    

{  PEVT_INST_QFPU_GRP_MASK       , "PEVT_INST_QFPU_GRP_MASK"          , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_MASK, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "AXU Opcode Mask Multiple Groups" },    

{  PEVT_INST_QFPU_FOTH           , "PEVT_INST_QFPU_FOTH"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FOTH, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Other Instr Compl" },    

{  PEVT_INST_QFPU_FRES           , "PEVT_INST_QFPU_FRES"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FRES, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Reciprocal Estimate Instr Compl" },    

{  PEVT_INST_QFPU_FADD           , "PEVT_INST_QFPU_FADD"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FADD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Add Instr Compl" },    

{  PEVT_INST_QFPU_FCVT           , "PEVT_INST_QFPU_FCVT"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FCVT, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Convert Instr Compl" },    

{  PEVT_INST_QFPU_FCMP           , "PEVT_INST_QFPU_FCMP"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FCMP, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Compare Instr Compl" },    

{  PEVT_INST_QFPU_FDIV           , "PEVT_INST_QFPU_FDIV"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FDIV, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Divide Instr Compl" },    

{  PEVT_INST_QFPU_FMOV           , "PEVT_INST_QFPU_FMOV"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FMOV, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Move Instr Compl" },    

{  PEVT_INST_QFPU_FMA            , "PEVT_INST_QFPU_FMA"               , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FMA, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Multiply-Add Instr Compl" },    

{  PEVT_INST_QFPU_FMUL           , "PEVT_INST_QFPU_FMUL"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FMUL, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Multiply Instr Compl" },    

{  PEVT_INST_QFPU_FRND           , "PEVT_INST_QFPU_FRND"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FRND, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Round Instr Compl" },    

{  PEVT_INST_QFPU_FSQE           , "PEVT_INST_QFPU_FSQE"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FSQE, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Square Root Estimate Instr Compl" },    

{  PEVT_INST_QFPU_FSQ            , "PEVT_INST_QFPU_FSQ"               , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_FSQ, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point Square Root Instr Compl" },    

{  PEVT_INST_QFPU_QMOV           , "PEVT_INST_QFPU_QMOV"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QMOV, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Move Instr Compl" },    

{  PEVT_INST_QFPU_QOTH           , "PEVT_INST_QFPU_QOTH"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QOTH, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Other Instr Compl" },    

{  PEVT_INST_QFPU_QADD           , "PEVT_INST_QFPU_QADD"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QADD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Add Instr Compl" },    

{  PEVT_INST_QFPU_QMUL           , "PEVT_INST_QFPU_QMUL"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QMUL, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Multiply Instr Compl" },    

{  PEVT_INST_QFPU_QRES           , "PEVT_INST_QFPU_QRES"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QRES, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Reciprocal Estimate Instr Compl" },    

{  PEVT_INST_QFPU_QMA            , "PEVT_INST_QFPU_QMA"               , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QMA, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Multiply-Add Instr Compl" },    

{  PEVT_INST_QFPU_QRND           , "PEVT_INST_QFPU_QRND"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QRND, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Round Instr Compl" },    

{  PEVT_INST_QFPU_QCVT           , "PEVT_INST_QFPU_QCVT"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QCVT, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Convert Instr Compl" },    

{  PEVT_INST_QFPU_QCMP           , "PEVT_INST_QFPU_QCMP"              , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_QCMP, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Quad Floating Point Compare Instr Compl" },    

{  PEVT_INST_QFPU_UCODE          , "PEVT_INST_QFPU_UCODE"             , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_UCODE, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "Floating Point microcoded instructions" },    

{  PEVT_INST_QFPU_ALL            , "PEVT_INST_QFPU_ALL"               , UPC_ES_OP_AXU  , UPC_P_AXU_OGRP_ALL, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "All AXU Instruction Completions" },    

{  PEVT_INST_QFPU_FPGRP1         , "PEVT_INST_QFPU_FPGRP1"            , UPC_ES_OP_FP_OPS, UPC_P_AXU_OGRP_FPGRP1, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 1 " },    

{  PEVT_INST_QFPU_FPGRP1_SINGLE  , "PEVT_INST_QFPU_FPGRP1_SINGLE"     , UPC_ES_OP_FP_OPS, UPC_P_AXU_OGRP_FPGRP1_SINGLE, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 1 single FP subset" },    

{  PEVT_INST_QFPU_FPGRP1_QUAD    , "PEVT_INST_QFPU_FPGRP1_QUAD"       , UPC_ES_OP_FP_OPS, UPC_P_AXU_OGRP_FPGRP1_QUAD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 1 quad FP subset" },    

{  PEVT_INST_QFPU_FPGRP2         , "PEVT_INST_QFPU_FPGRP2"            , UPC_ES_OP_FP_OPS, UPC_P_AXU_OGRP_FPGRP2, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 2 " },    

{  PEVT_INST_QFPU_FPGRP2_SINGLE  , "PEVT_INST_QFPU_FPGRP2_SINGLE"     , UPC_ES_OP_FP_OPS, UPC_P_AXU_OGRP_FPGRP2_SINGLE, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 2 single FP subset" },    

{  PEVT_INST_QFPU_FPGRP2_QUAD    , "PEVT_INST_QFPU_FPGRP2_QUAD"       , UPC_ES_OP_FP_OPS, UPC_P_AXU_OGRP_FPGRP2_QUAD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 2 quad FP subset" },    

{  PEVT_INST_QFPU_FPGRP1_INSTR   , "PEVT_INST_QFPU_FPGRP1_INSTR"      , UPC_ES_OP_FP_INSTR, UPC_P_AXU_OGRP_FPGRP1, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 1 instr count" },    

{  PEVT_INST_QFPU_FPGRP1_SINGLE_INSTR, "PEVT_INST_QFPU_FPGRP1_SINGLE_INSTR", UPC_ES_OP_FP_INSTR, UPC_P_AXU_OGRP_FPGRP1_SINGLE, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 1 single FP subset instr count" },    

{  PEVT_INST_QFPU_FPGRP1_QUAD_INSTR, "PEVT_INST_QFPU_FPGRP1_QUAD_INSTR" , UPC_ES_OP_FP_INSTR, UPC_P_AXU_OGRP_FPGRP1_QUAD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 1 quad FP subset instr count" },    

{  PEVT_INST_QFPU_FPGRP2_INSTR   , "PEVT_INST_QFPU_FPGRP2_INSTR"      , UPC_ES_OP_FP_INSTR, UPC_P_AXU_OGRP_FPGRP2, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 2 instr count" },    

{  PEVT_INST_QFPU_FPGRP2_SINGLE_INSTR, "PEVT_INST_QFPU_FPGRP2_SINGLE_INSTR", UPC_ES_OP_FP_INSTR, UPC_P_AXU_OGRP_FPGRP2_SINGLE, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 2 single FP subset instr count" },    

{  PEVT_INST_QFPU_FPGRP2_QUAD_INSTR, "PEVT_INST_QFPU_FPGRP2_QUAD_INSTR" , UPC_ES_OP_FP_INSTR, UPC_P_AXU_OGRP_FPGRP2_QUAD, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "FP Operations Group 2 quad FP subset instr count" },    

{  PEVT_INST_ALL                 , "PEVT_INST_ALL"                    , UPC_ES_OP_ALL  , UPC_P_OGRP_ALL, UPC_SCOPE_THREAD    , 0     , 1     , 0      , 0        , UPC_METRIC_SINGLE   , 0        , 0xFFFFFFUL, 0    , 0   , 0    , 0   , 0    , 0   , 0    , 0   , "All Instruction Completions" },    
    

};
           

    
const UPC_L2unitEvtTableRec_t l2unitEvtTbl[] =
{
// id                         , label                        , evtSrc         , selVal , desc } ,


{  PEVT_L2_HITS              , "PEVT_L2_HITS"                , UPC_ES_L2      , 0     , "L2 Hits" },    

{  PEVT_L2_MISSES            , "PEVT_L2_MISSES"              , UPC_ES_L2      , 1     , "L2 Misses" },    

{  PEVT_L2_PREFETCH          , "PEVT_L2_PREFETCH"            , UPC_ES_L2      , 2     , "L2 prefetch ahead of L1P prefetch" },    

{  PEVT_L2_FETCH_LINE        , "PEVT_L2_FETCH_LINE"          , UPC_ES_L2      , 3     , "L2 fetch 128 byte line from main memory" },    

{  PEVT_L2_STORE_LINE        , "PEVT_L2_STORE_LINE"          , UPC_ES_L2      , 4     , "L2 store 128 byte line to main memory" },    

{  PEVT_L2_STORE_PARTIAL_LINE, "PEVT_L2_STORE_PARTIAL_LINE"  , UPC_ES_L2      , 5     , "L2 store partial line to main memory" },    

{  PEVT_L2_REQ_REJECT        , "PEVT_L2_REQ_REJECT"          , UPC_ES_L2      , 6     , "L2 Req Reject" },    

{  PEVT_L2_REQ_RETIRE        , "PEVT_L2_REQ_RETIRE"          , UPC_ES_L2      , 7     , "L2 Req Retire" },    

{  PEVT_L2_REQ_MISS_AND_EVICT, "PEVT_L2_REQ_MISS_AND_EVICT"  , UPC_ES_L2      , 8     , "L2 Req Miss and Evict" },    

{  PEVT_L2_REQ_MISS_AND_EVICT_SPEC, "PEVT_L2_REQ_MISS_AND_EVICT_SPEC", UPC_ES_L2      , 9     , "L2 Req Miss and Evict Spec" },    

{  PEVT_L2_MISS_AND_EVICT    , "PEVT_L2_MISS_AND_EVICT"      , UPC_ES_L2      , 10    , "L2 Miss and Evict" },    

{  PEVT_L2_MISS_AND_EVICT_SPEC, "PEVT_L2_MISS_AND_EVICT_SPEC" , UPC_ES_L2      , 11    , "L2 Miss and Evict Spec" },    

{  PEVT_L2_REQ_SPEC_ADDR_CLEANUP, "PEVT_L2_REQ_SPEC_ADDR_CLEANUP", UPC_ES_L2      , 12    , "L2 Req Spec Addr Cleanup" },    

{  PEVT_L2_SPEC_ADDR_CLEANUP , "PEVT_L2_SPEC_ADDR_CLEANUP"   , UPC_ES_L2      , 13    , "L2 Spec Addr Cleanup" },    

{  PEVT_L2_SPEC_SET_CLEANUP  , "PEVT_L2_SPEC_SET_CLEANUP"    , UPC_ES_L2      , 14    , "L2 Spec Set Cleanup" },    

{  PEVT_L2_SPEC_RETRY        , "PEVT_L2_SPEC_RETRY"          , UPC_ES_L2      , 15    , "L2 Spec Retry" },    

{  PEVT_L2_HITS_SLICE        , "PEVT_L2_HITS_SLICE"          , UPC_ES_L2_SLICE, 0     , "L2 Cache Slice Hits " },    

{  PEVT_L2_MISSES_SLICE      , "PEVT_L2_MISSES_SLICE"        , UPC_ES_L2_SLICE, 1     , "L2 Cache Slice Misses" },    

{  PEVT_L2_PREFETCH_SLICE    , "PEVT_L2_PREFETCH_SLICE"      , UPC_ES_L2_SLICE, 2     , "L2 Cache Slice prefetch ahead of L1P prefetch" },    

{  PEVT_L2_FETCH_LINE_SLICE  , "PEVT_L2_FETCH_LINE_SLICE"    , UPC_ES_L2_SLICE, 3     , "L2 Cache Slice fetch 128 byte line from main memory" },    

{  PEVT_L2_STORE_LINE_SLICE  , "PEVT_L2_STORE_LINE_SLICE"    , UPC_ES_L2_SLICE, 4     , "L2 Cache Slice store 128 byte line to main memory" },    

{  PEVT_L2_STORE_PARTIAL_LINE_SLICE, "PEVT_L2_STORE_PARTIAL_LINE_SLICE", UPC_ES_L2_SLICE, 5     , "L2 Cache Slice store partial line to main memory" },    

{  PEVT_L2_REQ_REJECT_SLICE  , "PEVT_L2_REQ_REJECT_SLICE"    , UPC_ES_L2_SLICE, 6     , "L2 Cache Slice Req Reject" },    

{  PEVT_L2_REQ_RETIRE_SLICE  , "PEVT_L2_REQ_RETIRE_SLICE"    , UPC_ES_L2_SLICE, 7     , "L2 Cache Slice Req Retire" },    

{  PEVT_L2_REQ_MISS_AND_EVICT_SLICE, "PEVT_L2_REQ_MISS_AND_EVICT_SLICE", UPC_ES_L2_SLICE, 8     , "L2 Cache Slice Req Miss and Evict" },    

{  PEVT_L2_REQ_MISS_AND_EVICT_SPEC_SLICE, "PEVT_L2_REQ_MISS_AND_EVICT_SPEC_SLICE", UPC_ES_L2_SLICE, 9     , "L2 Cache Slice Req Miss and Evict Spec" },    

{  PEVT_L2_MISS_AND_EVICT_SLICE, "PEVT_L2_MISS_AND_EVICT_SLICE", UPC_ES_L2_SLICE, 10    , "L2 Cache Slice Miss and Evict" },    

{  PEVT_L2_MISS_AND_EVICT_SPEC_SLICE, "PEVT_L2_MISS_AND_EVICT_SPEC_SLICE", UPC_ES_L2_SLICE, 11    , "L2 Cache Slice Miss and Evict Spec" },    

{  PEVT_L2_REQ_SPEC_ADDR_CLEANUP_SLICE, "PEVT_L2_REQ_SPEC_ADDR_CLEANUP_SLICE", UPC_ES_L2_SLICE, 12    , "L2 Cache Slice Req Spec Addr Cleanup" },    

{  PEVT_L2_SPEC_ADDR_CLEANUP_SLICE, "PEVT_L2_SPEC_ADDR_CLEANUP_SLICE", UPC_ES_L2_SLICE, 13    , "L2 Cache Slice Spec Addr Cleanup" },    

{  PEVT_L2_SPEC_SET_CLEANUP_SLICE, "PEVT_L2_SPEC_SET_CLEANUP_SLICE", UPC_ES_L2_SLICE, 14    , "L2 Cache Slice Spec Set Cleanup" },    

{  PEVT_L2_SPEC_RETRY_SLICE  , "PEVT_L2_SPEC_RETRY_SLICE"    , UPC_ES_L2_SLICE, 15    , "L2 Cache Slice Spec Retry" },    
    

};
           

    
const UPC_IOunitEvtTableRec_t IOunitEvtTbl[] =
{
// id                         , label                        , evtSrc         , selVal , desc } ,


{  PEVT_MU_PKT_INJ           , "PEVT_MU_PKT_INJ"             , UPC_ES_MU      , 0     , "Mu packet injected" },    

{  PEVT_MU_MSG_INJ           , "PEVT_MU_MSG_INJ"             , UPC_ES_MU      , 1     , "Mu message injected" },    

{  PEVT_MU_FIFO_PKT_RCV      , "PEVT_MU_FIFO_PKT_RCV"        , UPC_ES_MU      , 2     , "Mu FIFO packet received" },    

{  PEVT_MU_RGET_PKT_RCV      , "PEVT_MU_RGET_PKT_RCV"        , UPC_ES_MU      , 3     , "Mu RGET packet received" },    

{  PEVT_MU_PUT_PKT_RCV       , "PEVT_MU_PUT_PKT_RCV"         , UPC_ES_MU      , 4     , "Mu PUT packet received" },    

{  PEVT_MU_PORT0_16B_WRT     , "PEVT_MU_PORT0_16B_WRT"       , UPC_ES_MU      , 5     , "Mu port 0 16B write" },    

{  PEVT_MU_PORT0_32B_RD      , "PEVT_MU_PORT0_32B_RD"        , UPC_ES_MU      , 6     , "Mu port 0 32B read" },    

{  PEVT_MU_PORT0_64B_RD      , "PEVT_MU_PORT0_64B_RD"        , UPC_ES_MU      , 7     , "Mu port 0 64B read" },    

{  PEVT_MU_PORT0_128B_RD     , "PEVT_MU_PORT0_128B_RD"       , UPC_ES_MU      , 8     , "Mu port 0 128B read" },    

{  PEVT_MU_PORT1_16B_WRT     , "PEVT_MU_PORT1_16B_WRT"       , UPC_ES_MU      , 9     , "Mu port 1 16B write" },    

{  PEVT_MU_PORT1_32B_RD      , "PEVT_MU_PORT1_32B_RD"        , UPC_ES_MU      , 10    , "Mu port 1 32B read" },    

{  PEVT_MU_PORT1_64B_RD      , "PEVT_MU_PORT1_64B_RD"        , UPC_ES_MU      , 11    , "Mu port 1 64B read" },    

{  PEVT_MU_PORT1_128B_RD     , "PEVT_MU_PORT1_128B_RD"       , UPC_ES_MU      , 12    , "Mu port 1 128B read" },    

{  PEVT_MU_PORT2_16B_WRT     , "PEVT_MU_PORT2_16B_WRT"       , UPC_ES_MU      , 13    , "Mu port 2 16B write" },    

{  PEVT_MU_PORT2_32B_RD      , "PEVT_MU_PORT2_32B_RD"        , UPC_ES_MU      , 14    , "Mu port 2 32B read" },    

{  PEVT_MU_PORT2_64B_RD      , "PEVT_MU_PORT2_64B_RD"        , UPC_ES_MU      , 15    , "Mu port 2 64B read" },    

{  PEVT_MU_PORT2_128B_RD     , "PEVT_MU_PORT2_128B_RD"       , UPC_ES_MU      , 16    , "Mu port 2 128B read" },    

{  PEVT_MU_SLV_PORT_RD       , "PEVT_MU_SLV_PORT_RD"         , UPC_ES_MU      , 17    , "Mu Slave port read" },    

{  PEVT_MU_SLV_PORT_WRT      , "PEVT_MU_SLV_PORT_WRT"        , UPC_ES_MU      , 18    , "Mu Slave port write" },    

{  PEVT_MU_PORT0_PEND_WRT    , "PEVT_MU_PORT0_PEND_WRT"      , UPC_ES_MU      , 34    , "Mu port 0 pending write requests" },    

{  PEVT_MU_PORT0_PEND_RD     , "PEVT_MU_PORT0_PEND_RD"       , UPC_ES_MU      , 35    , "Mu port 0 pending read requests" },    

{  PEVT_MU_PORT1_PEND_WRT    , "PEVT_MU_PORT1_PEND_WRT"      , UPC_ES_MU      , 36    , "Mu port 1 pending write requests" },    

{  PEVT_MU_PORT1_PEND_RD     , "PEVT_MU_PORT1_PEND_RD"       , UPC_ES_MU      , 37    , "Mu port 1 pending read requests" },    

{  PEVT_MU_PORT2_PEND_WRT    , "PEVT_MU_PORT2_PEND_WRT"      , UPC_ES_MU      , 38    , "Mu port 2 pending write requests" },    

{  PEVT_MU_PORT2_PEND_RD     , "PEVT_MU_PORT2_PEND_RD"       , UPC_ES_MU      , 39    , "Mu port 2 pending read requests" },    

{  PEVT_PCIE_INB_RD_BYTES    , "PEVT_PCIE_INB_RD_BYTES"      , UPC_ES_PCIE    , 40    , "Pcie Inbound Read Bytes" },    

{  PEVT_PCIE_INB_RDS         , "PEVT_PCIE_INB_RDS"           , UPC_ES_PCIE    , 41    , "Pcie Inbound Read Requests" },    

{  PEVT_PCIE_INB_RD_CMPLT    , "PEVT_PCIE_INB_RD_CMPLT"      , UPC_ES_PCIE    , 42    , "Pcie Inbound Read Completions" },    

{  PEVT_PCIE_OUTB_WRT_BYTES  , "PEVT_PCIE_OUTB_WRT_BYTES"    , UPC_ES_PCIE    , 43    , "Pcie outbound memory write bytes" },    

{  PEVT_PCIE_OUTB_CFG_X      , "PEVT_PCIE_OUTB_CFG_X"        , UPC_ES_PCIE    , 44    , "Pcie Outbound CFG transactions" },    

{  PEVT_PCIE_OUTB_IO_X       , "PEVT_PCIE_OUTB_IO_X"         , UPC_ES_PCIE    , 45    , "Pcie Outbound IO transactions" },    

{  PEVT_PCIE_RX_DLLP         , "PEVT_PCIE_RX_DLLP"           , UPC_ES_PCIE    , 46    , "Pcie RX DLLP Count" },    

{  PEVT_PCIE_RX_TLP          , "PEVT_PCIE_RX_TLP"            , UPC_ES_PCIE    , 47    , "Pcie RX TLP Count" },    

{  PEVT_PCIE_RX_TLP_SIZE     , "PEVT_PCIE_RX_TLP_SIZE"       , UPC_ES_PCIE    , 48    , "Pcie RX TLP Size in DWORDS" },    

{  PEVT_PCIE_TX_DLLP         , "PEVT_PCIE_TX_DLLP"           , UPC_ES_PCIE    , 49    , "Pcie TX DLLP Count" },    

{  PEVT_PCIE_TX_TLP          , "PEVT_PCIE_TX_TLP"            , UPC_ES_PCIE    , 50    , "Pcie TX TLP Count" },    

{  PEVT_PCIE_TX_TLP_SIZE     , "PEVT_PCIE_TX_TLP_SIZE"       , UPC_ES_PCIE    , 51    , "Pcie TX TLP Size in DWORDS" },    

{  PEVT_DB_PCIE_INB_WRT_BYTES, "PEVT_DB_PCIE_INB_WRT_BYTES"  , UPC_ES_DEVBUS  , 52    , "Devbus PCIe inbound write bytes written" },    

{  PEVT_DB_PCIE_OUTB_RD_BYTES, "PEVT_DB_PCIE_OUTB_RD_BYTES"  , UPC_ES_DEVBUS  , 53    , "Devbus PCIe outbound read bytes requested" },    

{  PEVT_DB_PCIE_OUTB_RDS     , "PEVT_DB_PCIE_OUTB_RDS"       , UPC_ES_DEVBUS  , 54    , "Devbus PCIe outbound read requests" },    

{  PEVT_DB_PCIE_OUTB_RD_CMPLT, "PEVT_DB_PCIE_OUTB_RD_CMPLT"  , UPC_ES_DEVBUS  , 55    , "Devbus PCIe outbound read completions" },    

{  PEVT_DB_BOOT_EDRAM_WRT_BYTES, "PEVT_DB_BOOT_EDRAM_WRT_BYTES", UPC_ES_DEVBUS  , 56    , "Devbus Boot eDRAM bytes written" },    

{  PEVT_DB_BOOT_EDRAM_RD_BYTES, "PEVT_DB_BOOT_EDRAM_RD_BYTES" , UPC_ES_DEVBUS  , 57    , "Devbus Boot eDRAM bytes read" },    

{  PEVT_DB_MSG_SIG_INTS      , "PEVT_DB_MSG_SIG_INTS"        , UPC_ES_DEVBUS  , 58    , "Devbus Message-Signaled Interrupts (MSIs)" },    
    

};
           

    
const UPC_NWunitEvtTableRec_t NWunitEvtTbl[] =
{
// id                         , label                        , evtSrc         , selVal , unitCtr, desc } ,


{  PEVT_NW_USER_PP_SENT      , "PEVT_NW_USER_PP_SENT"        , UPC_ES_NW      , (UPC_NW_VC_USER_DYNAMIC | UPC_NW_VC_USER_ESCAPE | UPC_NW_VC_USER_PRIORITY), UPC_NW_UC_SENDCHKS, "User pt-pt 32B packet chunks sent " },    

{  PEVT_NW_USER_DYN_PP_SENT  , "PEVT_NW_USER_DYN_PP_SENT"    , UPC_ES_NW      , UPC_NW_VC_USER_DYNAMIC, UPC_NW_UC_SENDCHKS, "User dynamic pt-pt 32B packet chunks sent " },    

{  PEVT_NW_USER_ESC_PP_SENT  , "PEVT_NW_USER_ESC_PP_SENT"    , UPC_ES_NW      , UPC_NW_VC_USER_ESCAPE, UPC_NW_UC_SENDCHKS, "User escape pt-pt 32B packet chunks sent " },    

{  PEVT_NW_USER_PRI_PP_SENT  , "PEVT_NW_USER_PRI_PP_SENT"    , UPC_ES_NW      , UPC_NW_VC_USER_PRIORITY, UPC_NW_UC_SENDCHKS, "User priority pt-pt 32B packet chunks sent " },    

{  PEVT_NW_SYS_PP_SENT       , "PEVT_NW_SYS_PP_SENT"         , UPC_ES_NW      , UPC_NW_VC_SYSTEM, UPC_NW_UC_SENDCHKS, "System pt-pt 32B packet chunks sent " },    

{  PEVT_NW_USER_WORLD_COL_SENT, "PEVT_NW_USER_WORLD_COL_SENT" , UPC_ES_NW      , UPC_NW_VC_COL_USER_WORLD, UPC_NW_UC_SENDCHKS, "User world collective 32B packet chunks sent " },    

{  PEVT_NW_USER_SUBC_COL_SENT, "PEVT_NW_USER_SUBC_COL_SENT"  , UPC_ES_NW      , UPC_NW_VC_COL_USER_SUBCOMM, UPC_NW_UC_SENDCHKS, "User sub-comm collective 32B packet chunks sent " },    

{  PEVT_NW_SYS_COL_SENT      , "PEVT_NW_SYS_COL_SENT"        , UPC_ES_NW      , UPC_NW_VC_COL_SYSTEM, UPC_NW_UC_SENDCHKS, "System collective 32B packet chunks sent " },    

{  PEVT_NW_COMMWORLD_COL_SENT, "PEVT_NW_COMMWORLD_COL_SENT"  , UPC_ES_NW      , (UPC_NW_VC_COL_USER_WORLD | UPC_NW_VC_COL_SYSTEM), UPC_NW_UC_SENDCHKS, "Comm-world collective 32B packet chunks sent " },    

{  PEVT_NW_USER_PP_RECV      , "PEVT_NW_USER_PP_RECV"        , UPC_ES_NW      , (UPC_NW_VC_USER_DYNAMIC | UPC_NW_VC_USER_ESCAPE | UPC_NW_VC_USER_PRIORITY), UPC_NW_UC_RCVPKTS, "User pt-pt packets received " },    

{  PEVT_NW_USER_DYN_PP_RECV  , "PEVT_NW_USER_DYN_PP_RECV"    , UPC_ES_NW      , UPC_NW_VC_USER_DYNAMIC, UPC_NW_UC_RCVPKTS, "User dynamic pt-pt packets received " },    

{  PEVT_NW_USER_ESC_PP_RECV  , "PEVT_NW_USER_ESC_PP_RECV"    , UPC_ES_NW      , UPC_NW_VC_USER_ESCAPE, UPC_NW_UC_RCVPKTS, "User escape pt-pt packets received " },    

{  PEVT_NW_USER_PRI_PP_RECV  , "PEVT_NW_USER_PRI_PP_RECV"    , UPC_ES_NW      , UPC_NW_VC_USER_PRIORITY, UPC_NW_UC_RCVPKTS, "User priority pt-pt packets received " },    

{  PEVT_NW_SYS_PP_RECV       , "PEVT_NW_SYS_PP_RECV"         , UPC_ES_NW      , UPC_NW_VC_SYSTEM, UPC_NW_UC_RCVPKTS, "System pt-pt packets received " },    

{  PEVT_NW_USER_WORLD_COL_RECV, "PEVT_NW_USER_WORLD_COL_RECV" , UPC_ES_NW      , UPC_NW_VC_COL_USER_WORLD, UPC_NW_UC_RCVPKTS, "User world collective packets received " },    

{  PEVT_NW_USER_SUBC_COL_RECV, "PEVT_NW_USER_SUBC_COL_RECV"  , UPC_ES_NW      , UPC_NW_VC_COL_USER_SUBCOMM, UPC_NW_UC_RCVPKTS, "User sub-comm collective packets received " },    

{  PEVT_NW_SYS_COL_RECV      , "PEVT_NW_SYS_COL_RECV"        , UPC_ES_NW      , UPC_NW_VC_COL_SYSTEM, UPC_NW_UC_RCVPKTS, "System collective packets received " },    

{  PEVT_NW_COMMWORLD_COL_RECV, "PEVT_NW_COMMWORLD_COL_RECV"  , UPC_ES_NW      , (UPC_NW_VC_COL_USER_WORLD | UPC_NW_VC_COL_SYSTEM), UPC_NW_UC_RCVPKTS, "Comm-world collective packets received " },    

{  PEVT_NW_USER_PP_RECV_FIFO , "PEVT_NW_USER_PP_RECV_FIFO"   , UPC_ES_NW      , (UPC_NW_VC_USER_DYNAMIC | UPC_NW_VC_USER_ESCAPE | UPC_NW_VC_USER_PRIORITY), UPC_NW_UC_FIFOPKTS, "User pt-pt packets in receive fifo " },    

{  PEVT_NW_USER_DYN_PP_RECV_FIFO, "PEVT_NW_USER_DYN_PP_RECV_FIFO", UPC_ES_NW      , UPC_NW_VC_USER_DYNAMIC, UPC_NW_UC_FIFOPKTS, "User dynamic pt-pt packets in receive fifo " },    

{  PEVT_NW_USER_ESC_PP_RECV_FIFO, "PEVT_NW_USER_ESC_PP_RECV_FIFO", UPC_ES_NW      , UPC_NW_VC_USER_ESCAPE, UPC_NW_UC_FIFOPKTS, "User escape pt-pt packets in receive fifo " },    

{  PEVT_NW_USER_PRI_PP_RECV_FIFO, "PEVT_NW_USER_PRI_PP_RECV_FIFO", UPC_ES_NW      , UPC_NW_VC_USER_PRIORITY, UPC_NW_UC_FIFOPKTS, "User priority pt-pt packets in receive fifo " },    

{  PEVT_NW_SYS_PP_RECV_FIFO  , "PEVT_NW_SYS_PP_RECV_FIFO"    , UPC_ES_NW      , UPC_NW_VC_SYSTEM, UPC_NW_UC_FIFOPKTS, "System pt-pt packets in receive fifo " },    

{  PEVT_NW_USER_WORLD_COL_RECV_FIFO, "PEVT_NW_USER_WORLD_COL_RECV_FIFO", UPC_ES_NW      , UPC_NW_VC_COL_USER_WORLD, UPC_NW_UC_FIFOPKTS, "User world collective packets in receive fifo " },    

{  PEVT_NW_USER_SUBC_COL_RECV_FIFO, "PEVT_NW_USER_SUBC_COL_RECV_FIFO", UPC_ES_NW      , UPC_NW_VC_COL_USER_SUBCOMM, UPC_NW_UC_FIFOPKTS, "User subcomm collective packets in receive fifo " },    

{  PEVT_NW_SYS_COL_RECV_FIFO , "PEVT_NW_SYS_COL_RECV_FIFO"   , UPC_ES_NW      , UPC_NW_VC_COL_SYSTEM, UPC_NW_UC_FIFOPKTS, "System collective packets in receive fifo " },    

{  PEVT_NW_COMMWORLD_COL_RECV_FIFO, "PEVT_NW_COMMWORLD_COL_RECV_FIFO", UPC_ES_NW      , (UPC_NW_VC_COL_USER_WORLD | UPC_NW_VC_COL_SYSTEM), UPC_NW_UC_FIFOPKTS, "Comm-world collective packets in receive fifo " },    

{  PEVT_NW_SENT              , "PEVT_NW_SENT"                , UPC_ES_NW      , 0     , UPC_NW_UC_SENDCHKS, "Set attribute to filter 32B packet chunks sent " },    

{  PEVT_NW_RECV              , "PEVT_NW_RECV"                , UPC_ES_NW      , 0     , UPC_NW_UC_RCVPKTS, "Set attribute to filter packets received" },    

{  PEVT_NW_RECV_FIFO         , "PEVT_NW_RECV_FIFO"           , UPC_ES_NW      , 0     , UPC_NW_UC_FIFOPKTS, "Set attribute to filter packets in receive fifo" },    

{  PEVT_NW_CYCLES            , "PEVT_NW_CYCLES"              , UPC_ES_NW      , 0     , UPC_NW_UC_CYCLES, "Network cycles since reset" },    
    

};
           

    
const UPC_CNKunitEvtTableRec_t CNKunitEvtTbl[] =
{
// id                         , label                        , evtSrc         , selVal , desc } ,


{  PEVT_CNKNODE_MUINT        , "PEVT_CNKNODE_MUINT"          , UPC_ES_CNKNODE , CNK_NODEPERFCOUNT_MU, "MU Non-fatal interrupt" },    

{  PEVT_CNKNODE_NDINT        , "PEVT_CNKNODE_NDINT"          , UPC_ES_CNKNODE , CNK_NODEPERFCOUNT_ND, "ND Non-fatal interrupt" },    

{  PEVT_CNKPROC_RSV          , "PEVT_CNKPROC_RSV"            , UPC_ES_CNKPROC , 0     , "Reserved - NO CNK Process events are defined" },    

{  PEVT_CNKHWT_SYSCALL       , "PEVT_CNKHWT_SYSCALL"         , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_SYSCALL , "System Calls" },    

{  PEVT_CNKHWT_STANDARD      , "PEVT_CNKHWT_STANDARD"        , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_STANDARD , "External Input Interrupts" },    

{  PEVT_CNKHWT_CRITICAL      , "PEVT_CNKHWT_CRITICAL"        , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_CRITICAL , "Critical Input Interrupts" },    

{  PEVT_CNKHWT_DECREMENTER   , "PEVT_CNKHWT_DECREMENTER"     , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_DECREMENTER, "Decrementer Interrupts" },    

{  PEVT_CNKHWT_FIT           , "PEVT_CNKHWT_FIT"             , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_FIT , "Fixed Interval Timer Interrupts" },    

{  PEVT_CNKHWT_WATCHDOG      , "PEVT_CNKHWT_WATCHDOG"        , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_WATCHDOG , "Watchdog Timer Interrupts" },    

{  PEVT_CNKHWT_UDEC          , "PEVT_CNKHWT_UDEC"            , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_UDEC , "User Decrementer Interrupts" },    

{  PEVT_CNKHWT_PERFMON       , "PEVT_CNKHWT_PERFMON"         , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_PERFMON , "Performance Monitor interrupts" },    

{  PEVT_CNKHWT_UNKDEBUG      , "PEVT_CNKHWT_UNKDEBUG"        , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_UNKDEBUG , "Unknown/Invalid Interrupts" },    

{  PEVT_CNKHWT_DEBUG         , "PEVT_CNKHWT_DEBUG"           , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_DEBUG , "Debug Interrupts" },    

{  PEVT_CNKHWT_DSI           , "PEVT_CNKHWT_DSI"             , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_DSI , "Data Storage Interrupts" },    

{  PEVT_CNKHWT_ISI           , "PEVT_CNKHWT_ISI"             , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_ISI , "Instruction Storage Interrupts" },    

{  PEVT_CNKHWT_ALIGNMENT     , "PEVT_CNKHWT_ALIGNMENT"       , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_ALIGNMENT , "Alignment Interrupts" },    

{  PEVT_CNKHWT_PROGRAM       , "PEVT_CNKHWT_PROGRAM"         , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_PROGRAM , "Program Interrupts" },    

{  PEVT_CNKHWT_FPU           , "PEVT_CNKHWT_FPU"             , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_FPU , "FPU Unavailable Interrupts" },    

{  PEVT_CNKHWT_APU           , "PEVT_CNKHWT_APU"             , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_APU , "APU Unavailable Interrupts" },    

{  PEVT_CNKHWT_DTLB          , "PEVT_CNKHWT_DTLB"            , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_DTLB , "Data TLB Interrupts" },    

{  PEVT_CNKHWT_ITLB          , "PEVT_CNKHWT_ITLB"            , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_ITLB , "Instruction TLB Interrupts" },    

{  PEVT_CNKHWT_VECTOR        , "PEVT_CNKHWT_VECTOR"          , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_VECTOR , "Vector Unavailable Interrupts" },    

{  PEVT_CNKHWT_UNDEF         , "PEVT_CNKHWT_UNDEF"           , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_UNDEF , "Undefined Interrupts" },    

{  PEVT_CNKHWT_PDBI          , "PEVT_CNKHWT_PDBI"            , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_PDBI , "Processor Doorbell Interrupts" },    

{  PEVT_CNKHWT_PDBCI         , "PEVT_CNKHWT_PDBCI"           , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_PDBCI , "Processor Doorbell Critical Ints" },    

{  PEVT_CNKHWT_GDBI          , "PEVT_CNKHWT_GDBI"            , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_GDBI , "Guest Doorbell Interrupts" },    

{  PEVT_CNKHWT_GDBCI         , "PEVT_CNKHWT_GDBCI"           , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_GDBCI , "Guest Doorbell Crit or MChk Ints" },    

{  PEVT_CNKHWT_EHVSC         , "PEVT_CNKHWT_EHVSC"           , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_EHVSC , "Embedded Hypervisor System Calls" },    

{  PEVT_CNKHWT_EHVPRIV       , "PEVT_CNKHWT_EHVPRIV"         , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_EHVPRIV , "Embedded Hypervisor Privileged" },    

{  PEVT_CNKHWT_LRATE         , "PEVT_CNKHWT_LRATE"           , UPC_ES_CNKHWT  , CNK_HWTPERFCOUNT_LRATE , "LRAT exception" },    
    

};
           
