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

#include "globals.h"
#include "EvtSet.h"

using namespace bgpm;


int EvtSet::InvalidEvtSet() { return lastErr.PrintOrExit(BGPM_EINV_SET_HANDLE, BGPM_ERRLOC); }
int EvtSet::UnassignedEvtSet() { return lastErr.PrintOrExit(BGPM_EEVT_SET_UNASSIGNED, BGPM_ERRLOC); }
int EvtSet::UnsupportedOperation() { return lastErr.PrintOrExit(BGPM_EINV_OP, BGPM_ERRLOC); }




EvtSet::ESTypes EvtSet::ESType() { return ESType_Base; }
const char *EvtSet::ESTypeLabel() { return "Base"; }
//EvtSet *EvtSet::Spawn() { return new EvtSet(); }
//EvtSet *EvtSet::Clone(uint64_t target) { return new EvtSet(*this); }  // ignore target
bool EvtSet::IsEventIdCompatible(int evtId) { return false; }
int EvtSet::DefaultError() { return InvalidEvtSet(); }
int EvtSet::AddEvent(unsigned evtId, EvtSet* & pNewEvtSet) { return DefaultError(); }
int EvtSet::AddEventList(unsigned *evtIds, unsigned num, EvtSet* & pNewEvtSet) { return DefaultError(); }
int EvtSet::DumpEventsInError(unsigned failedEvtId) { return 0; }  // by default - ignore
int EvtSet::SetPreReserved() { return DefaultError(); }
int EvtSet::NumEvents() const { return 0; }
int EvtSet::NumTargets() const { return 1; }
int EvtSet::Apply(int hEvtSet) { return DefaultError(); }
int EvtSet::Attach(uint64_t target, uint64_t agentTarg) { return DefaultError(); }
int EvtSet::Detach() { return DefaultError(); }
int EvtSet::Start() { return DefaultError(); }
int EvtSet::Stop() { return DefaultError(); }
int EvtSet::ResetStart(){ return DefaultError(); }
int EvtSet::Reset(){ return DefaultError(); }
int EvtSet::ReadEvent(unsigned idx, uint64_t *pVal) { return DefaultError(); }
int EvtSet::WriteEvent(unsigned idx, uint64_t val) { return DefaultError(); }
int EvtSet::L2_ReadEvent(unsigned slice, unsigned idx, uint64_t *pVal) { return DefaultError(); }
int EvtSet::L2_WriteEvent(unsigned slice, unsigned idx, uint64_t val) { return DefaultError(); }
int EvtSet::ReadThreadEvent(unsigned idx, uint64_t appMask, uint64_t agentMask, uint64_t *pVal) { return DefaultError(); }
int EvtSet::ReadLinkEvent(unsigned idx, uint16_t linkMask, uint64_t *pVal) { return DefaultError(); }
int EvtSet::Punit_GetHandles(Bgpm_Punit_Handles_t *pH){ return DefaultError(); }
int EvtSet::Punit_GetHandles4Thread(int hwThd, Bgpm_Punit_Handles_t *pH){ return DefaultError(); }
int EvtSet::Punit_GetLLHandles(Bgpm_Punit_LLHandles_t *pH){ return DefaultError(); }
uint64_t EvtSet::GetActiveCtrMask() { return 0; }
const char * EvtSet::GetEventLabel(unsigned idx){ DefaultError(); return NULL; }
int EvtSet::GetEventId(unsigned idx) { return DefaultError(); }
int EvtSet::GetEventIndex(unsigned evtId, unsigned startIdx) { return DefaultError(); }
int EvtSet::SetEvtUser1(unsigned idx, uint64_t val) { return DefaultError(); }
int EvtSet::GetEvtUser1(unsigned idx, uint64_t *pVal) { return DefaultError(); }
int EvtSet::SetEvtUser2(unsigned idx, uint64_t val) { return DefaultError(); }
int EvtSet::GetEvtUser2(unsigned idx, uint64_t *pVal) { return DefaultError(); }
int EvtSet::SampleCNKEvent(unsigned idx, uint64_t *pVal) { return DefaultError(); }
int EvtSet::SetOverflowHandler(Bgpm_OverflowHandler_t handler) { return DefaultError(); }
int EvtSet::SetOverflow(unsigned idx, uint64_t threshold) { return DefaultError(); }
int EvtSet::GetOverflow(unsigned idx, uint64_t *pThreshold) { *pThreshold = 0; return 0; }
Bgpm_OverflowHandler_t EvtSet::GetOverflowHandler() { return NULL; }
bool EvtSet::AccumResetThresholds(uint64_t intMask) { return false; }
int EvtSet::GetOverflowIndices(uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen) { return DefaultError(); }
int EvtSet::SetContext(Bgpm_Context ctx) { return DefaultError(); }
int EvtSet::GetContext(Bgpm_Context *pCtx) { return DefaultError(); }
int EvtSet::AllowMixedContext() { return DefaultError(); }
int EvtSet::SetQfpuGrpMask(unsigned idx, uint64_t mask) { return DefaultError(); }
int EvtSet::GetQfpuGrpMask(unsigned idx, uint64_t *pMask) { return DefaultError(); }
int EvtSet::SetXuGrpMask(unsigned idx, uint64_t mask) { return DefaultError(); }
int EvtSet::GetXuGrpMask(unsigned idx, uint64_t *pMask) { return DefaultError(); }
int EvtSet::SetCountFP(unsigned idx, ushort countFp) { return DefaultError(); }
int EvtSet::GetCountFP(unsigned idx, ushort *pCountFp) { return DefaultError(); }
int EvtSet::SetEdge(unsigned idx, ushort edge) { return DefaultError(); }
int EvtSet::GetEdge(unsigned idx, ushort *pEdge) { return DefaultError(); }
int EvtSet::SetInvert(unsigned idx, ushort invert) { return DefaultError(); }
int EvtSet::GetInvert(unsigned idx, ushort *pInvert) { return DefaultError(); }
int EvtSet::SetXuMatch(unsigned idx, uint16_t match, uint16_t mask) { return DefaultError(); }
int EvtSet::GetXuMatch(unsigned idx, uint16_t *pMatch, uint16_t *pMask) { return DefaultError(); }
int EvtSet::SetQfpuMatch(unsigned idx, uint16_t match, uint16_t mask, ushort fpScale) { return DefaultError(); }
int EvtSet::GetQfpuMatch(unsigned idx, uint16_t *pMatch, uint16_t *pMask, ushort *pFpScale) { return DefaultError(); }
int EvtSet::SetFpSqrScale(ushort fpScale) { return DefaultError(); }
int EvtSet::GetFpSqrScale(ushort *pFpScale) { return DefaultError(); }
int EvtSet::SetFpDivScale(ushort fpScale) { return DefaultError(); }
int EvtSet::GetFpDivScale(ushort *pFpScale) { return DefaultError(); }
int EvtSet::PrintCurrentReservations() { return DefaultError(); }
int EvtSet::SwitchMuxGrp() { return DefaultError(); }
int EvtSet::GetMux(Bgpm_MuxStats_t* pMuxStats) { return DefaultError(); }
int EvtSet::GetMuxGrps() { return 0; }
uint64_t EvtSet::GetMuxCycles(unsigned muxGrp) { return 0; }
uint64_t EvtSet::GetMuxEventCycles(unsigned evtIdx, ushort normalize) { return 0; }
uint64_t EvtSet::GetMuxIntStatus(uint64_t intMask) { return 0; }
int EvtSet::SetVirtChannels(unsigned idx, UPC_NW_Vchannels vchan) { return DefaultError(); }
int EvtSet::GetVirtChannels(unsigned idx, UPC_NW_Vchannels * pVchan) { return DefaultError(); }


EvtSet::ESTypes ES_Unassigned::ESType() { return ESType_Unassigned; }
const char *ES_Unassigned::ESTypeLabel() { return "Unassigned"; }
//EvtSet *ES_Unassigned::Clone(uint64_t target) { return new ES_Unassigned(*this); }  // ignore target
//EvtSet *ES_Unassigned::Spawn() { return new ES_Unassigned(); }
int ES_Unassigned::DefaultError() { return UnassignedEvtSet(); }


EvtSet::ESTypes ES_Active::ESType() { return ESType_Active; }
const char *ES_Active::ESTypeLabel() { return "Active"; }
//vtSet *ES_Active::Clone(uint64_t target) { return new ES_Active(*this); }  // ignore target
//EvtSet *ES_Active::Spawn() { return new ES_Active(); }
int ES_Active::DefaultError() { return UnsupportedOperation(); }





