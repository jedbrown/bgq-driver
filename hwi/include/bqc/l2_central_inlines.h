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
#ifndef _L2C_INLINE_H_
#define _L2C_INLINE_H_
//
// Thread level speculative execution device control facilities.
//

#include <stdint.h>
#include <stddef.h>
#include "hwi/include/bqc/PhysicalMap.h"
#include "hwi/include/bqc/l2_central_mmio.h"
#include "hwi/include/bqc/A2_inlines.h"

#ifndef __INLINE__
#define __INLINE__ extern inline
#endif

/*!
 * \brief Returns the number of speculation domains
 */
__INLINE__ uint64_t SPEC_GetNumberOfDomains()
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->NumDomains;
}

/*!
 * \brief Sets the number of speculation domains
 * \note This is typically a privileged operation
 */
__INLINE__ void SPEC_SetNumberOfDomains(uint64_t value)
{
    ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->NumDomains = value;
}



#if 1


/*!
 * \brief Sets the reserved1[0] field
 * \note This is typically a privileged operation
 */
__INLINE__ void SPEC_SetReserved1_0(uint64_t value)
{
    ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->reserved1[0] = value;
}


/*!
 * \brief Sets the reserved1[1] field
 * \note This is typically a privileged operation
 */
__INLINE__ void SPEC_SetReserved1_1(uint64_t value)
{
    ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->reserved1[1] = value;
}

#endif


/*!
 * \brief Returns the current privileges mapping
 *
 * \return The PrivMap value is a collection of bitwise OR'd L2C_PRIVMAP_DISABLEDOMAIN(), L2C_PRIVMAP_DISABLEREADFNC(), and/or L2C_PRIVMAP_DISABLEWRITEFNC()
 * values.  The DisableRead/DisableWrite Function bitvectors disable privileged access to the type of access.  The DisableDomain() bitvector
 * protects against user access to privileged domains.  
 * \see L2C_PRIVMAP_DISABLEDOMAIN
 * \see L2C_PRIVMAP_DISABLEREADFNC
 * \see L2C_PRIVMAP_DISABLEWRITEFNC 
 */
__INLINE__ uint64_t SPEC_GetPrivMap()
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->PrivMap;
}

/*!
 * \brief Sets the current privileges mapping
 * \note This is typically a privileged operation
 *
 * \param[in] value Collection of bitwise OR'd L2C_PRIVMAP_DISABLEDOMAIN(), L2C_PRIVMAP_DISABLEREADFNC(), and/or L2C_PRIVMAP_DISABLEWRITEFNC()
 * values.  The DisableRead/DisableWrite Function bitvectors disable privileged access to the type of access.  The DisableDomain() bitvector
 * protects against user access to privileged domains.  
 * \see L2C_PRIVMAP_DISABLEDOMAIN
 * \see L2C_PRIVMAP_DISABLEREADFNC
 * \see L2C_PRIVMAP_DISABLEWRITEFNC
 */
__INLINE__ void SPEC_SetPrivMap(uint64_t value)
{ 
    ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->PrivMap = value;
}

__INLINE__ uint64_t SPEC_GetIDStateTable(uint64_t i)
{ 
   return ((volatile L2Central_t *)
	   L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->IDStateTable[i];
}

/* Returns true when all speculation ids are available. */
__INLINE__ uint64_t SPEC_AllAvail(void)
{ 
  return !SPEC_GetIDStateTable(0)
      && !SPEC_GetIDStateTable(1)
      && !SPEC_GetIDStateTable(2)
      && !SPEC_GetIDStateTable(3);
}

/* Returns true when none of the speculation ids are in speculation mode. */
__INLINE__ uint64_t SPEC_AllNonSpec(void)
{ 
  uint64_t i;
  for (i=0; i<4; i++) {
    uint64_t s = SPEC_GetIDStateTable(i);
    s &= (~s) >> 1;
    s &= 0x5555555555555555;
    if (s)
      return 0;
  }
  return 1;
}

/* Returns true when none of the speculation ids are in speculation mode. */
__INLINE__ uint64_t SPEC_AllAvailOrInvalid(void)
{ 
  uint64_t i;
  for (i=0; i<4; i++) {
    uint64_t s = SPEC_GetIDStateTable(i);
    s ^= s >> 1;
    s &= 0x5555555555555555;
    if (s)
      return 0;
  }
  return 1;
}

/*!
 * \brief Atomically allocates a speculative ID for transactional memory
 * \param[in] groupmask Groupmask is a 16-bit vector (0-65535).  Each bit represents 8 consecutive speculative IDs. (8*16=128 SpecIDs)
 *
 * \note The groupmask is not precisely the same as a domainmask.  However, when numdomains=16, groupmask == domainmask.
 */
__INLINE__ L2C_SPECID_t SPEC_TryAlloc(uint64_t groupmask)
{ 
  return *((volatile int64_t*)(L2C_TRYALLOC_OFFSET(groupmask)));
}


/*!
 * \brief Returns the mode for the specified domain
 * \see L2C_DOMAINATTR_MODE
 * \see L2C_DOMAINATTR_INVAL_ON_CNFLT
 * \see L2C_DOMAINATTR_INT_ON_CNFLT
 * \see L2C_DOMAINATTR_INT_ON_COMMIT
 * \see L2C_DOMAINATTR_INT_ON_INVAL
 * \see L2C_DOMAINATTR_COMMITTO_ID01
 */
__INLINE__ uint64_t SPEC_GetDomainMode(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].DomainAttr;
}
__INLINE__ uint64_t SPEC_GetDomainMode_priv(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->domain[domain_id].DomainAttr;
}


/*!
 * \brief Sets the attributes for the specified domain
 *
 * This sets the domain mode (long/short running speculation, transactional memory, or rollback modes)
 * This sets interrupt behaviors (interrupt on conflict, commit, invalidate).
 * This sets invalidate on conflict
 * This sets setting committed storage to software-managed coherency
 * 
 * \see L2C_DOMAINATTR_MODE
 * \see L2C_DOMAINATTR_INVAL_ON_CNFLT
 * \see L2C_DOMAINATTR_INT_ON_CNFLT
 * \see L2C_DOMAINATTR_INT_ON_COMMIT
 * \see L2C_DOMAINATTR_INT_ON_INVAL
 * \see L2C_DOMAINATTR_COMMITTO_ID01
 
 * \note This is typicallly a privileged operation
 */
__INLINE__ void SPEC_SetDomainMode(uint64_t domain_id, uint64_t value)
{ 
   ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].DomainAttr = value;
}
__INLINE__ void SPEC_SetDomainMode_priv(uint64_t domain_id, uint64_t value)
{ 
   ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->domain[domain_id].DomainAttr = value;
}


/*! 
 * \brief Returns the next speculative ID to be allocated in the specified domain
 * \param[in] domain_id selected domain.  0-15
 * \return Next speculative ID
 */
__INLINE__ L2C_SPECID_t SPEC_GetAllocPtr(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].AllocPtr;
}
__INLINE__ L2C_SPECID_t SPEC_GetAllocPtr_priv(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->domain[domain_id].AllocPtr;
}

/*! 
 * \brief Sets the next speculative ID to be allocated in the specified domain
 * \param[in] domain_id Domain ID 0-15
 * \param[in] value     Next speculative ID to be allocated
 */
__INLINE__ void SPEC_SetAllocPtr(uint64_t domain_id, L2C_SPECID_t spec_id)
{ 
   ( ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].AllocPtr) = spec_id;
}


/*! 
 * \brief Returns the next speculative ID to be committed in the specified domain
 * \param[in] domain_id selected domain.  0-15
 * \return Next speculative ID committed
 */
__INLINE__ L2C_SPECID_t SPEC_GetCommitPtr(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].CommitPtr;
}

/*! 
 * \brief Sets the next speculative ID to be committed in the specified domain
 * \param[in] domain_id Domain ID 0-15
 * \param[in] value     Next speculative ID to be committed
 */
__INLINE__ void SPEC_SetCommitPtr(uint64_t domain_id, L2C_SPECID_t spec_id)
{ 
   ( ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].CommitPtr) = spec_id;
}

/*! 
 * \brief Returns the next speculative ID to be reclaimed in the specified domain
 * \param[in] domain_id selected domain.  0-15
 * \return Next speculative ID reclaimed
 */
__INLINE__ L2C_SPECID_t SPEC_GetReclaimPtr(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].ReclaimPtr;
}

/*! 
 * \brief Sets the next speculative ID to be reclaimed in the specified domain
 * \param[in] domain_id Domain ID 0-15
 * \param[in] value     Next speculative ID to be reclaimed
 */
__INLINE__ void SPEC_SetReclaimPtr(uint64_t domain_id, L2C_SPECID_t spec_id)
{ 
   ( ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].ReclaimPtr) = spec_id;
}

/*! 
 * \brief Allocates the next speculative ID in the specified domain
 * \param[in] domain_id selected domain.  0-15
 * \return Speculative ID allocated.  Or -1 if unable
 */
__INLINE__ L2C_SPECID_t SPEC_PtrTryAlloc(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].PtrTryAlloc;
}


/*! 
 * \brief Switch ID of commit_ptr to committed state
 * \param[in] domain_id selected domain.  0-15
 * \param[in] value unknown value goes here
 * \todo determine the significance of the value
 */
__INLINE__ void SPEC_PtrForceCommit(uint64_t domain_id, L2C_SPECID_t spec_id)
{ 
   ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].PtrForceCommit = spec_id;
}


/*! 
 * \brief Switch ID of commit_ptr to committed state if not conflicting
 * \param[in] domain_id selected domain.  0-15
 * \return -1 if commit failed
 */
__INLINE__ L2C_SPECID_t SPEC_PtrTryCommit(uint64_t domain_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->domain[domain_id].PtrTryCommit;
}



__INLINE__ uint64_t SPEC_GetL1PFastBase()
{
  uint64_t l1p_base_addr;
  asm volatile(
	       // create base address
	       "lis %0,0x0150;"
	       "rldimi %0,%0,28,13"
	       : "=b" (l1p_base_addr)
	       );
  return l1p_base_addr;
}

__INLINE__ uint64_t SPEC_GetL2FastBase(uint64_t l1p_base_addr)
{
  return l1p_base_addr + 0x100000;
  /*
  uint64_t l2_base_addr;
  asm volatile(
	       // create base address
	       "addis %0,%1,16;"
	       : "=b" (l2_base_addr) : "b" (l1p_base_addr)
	       );
  return l2_base_addr;
  */
}

__INLINE__ uint64_t SPEC_GetL2IDBase()
{
  uint64_t l2_base_addr;
  asm volatile(
	       // create base address
	       "lis %0,%1@h;"
	       "ori %0,%0,%1@l;"
	       "sldi %0,%0,15"
	       : "=b" (l2_base_addr) : "i" ((L2_CENTRAL_USER_BASE_TLS_ADDRESS | 0x8000) >> 15)
	       );
  return l2_base_addr;
}

__INLINE__ uint64_t SPEC_GetL2IDFastBase(uint64_t l2_base_addr)
{
  return l2_base_addr | 0x8000;
  /*
  uint64_t l2_id_base_addr;
  asm volatile(
	       // create base address
	       "ori %0,%1,32768;"
	       : "=b" (l2_id_base_addr) : "b" (l2_base_addr)
	       );
  return l2_id_base_addr;
  */
}

__INLINE__ uint64_t SPEC_GetL2AllocBase()
{
  uint64_t l2_base_addr;
  asm volatile(
	       // create base address
	       "lis %0,%1@h;"
	       "ori %0,%0,%1@l;"
	       "sldi %0,%0,19"
	       : "=b" (l2_base_addr) : "i" ((L2_CENTRAL_USER_BASE_TLS_ADDRESS | 0x80000) >> 19)
	       );
  return l2_base_addr;
}

__INLINE__ uint64_t SPEC_InsertIDBase(L2C_SPECID_t spec_id, uint64_t l2_base_addr)
{ 
  //return (l2_base_addr & (~0x3f80)) | ((spec_id<<7) & 0x3f80ll);
  _BGQ_INSERT(7,63-7,l2_base_addr,spec_id);
  return l2_base_addr;
}
__INLINE__ uint64_t SPEC_InsertAllocBase(uint64_t mask, uint64_t l2_base_addr)
{ 
  //return (l2_base_addr & (~0x7fff8)) | ((mask<<7) & 0x7fff8ll);
  _BGQ_INSERT(16,63-3,l2_base_addr,mask);
  return l2_base_addr;
}



/*! 
 * \brief Returns the state of the specified speculative ID
 * \param[in] spec_id Speculative ID.  0-127
 * \return State
 */
__INLINE__ uint64_t SPEC_GetIDState(L2C_SPECID_t spec_id)
{ 
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].state;
}

/*! 
 * \brief Sets the state of the specified speculative ID
 * \param[in] spec_id Speculative ID.  0-127
 * \param[in] new_state State
 */
__INLINE__ void SPEC_SetIDStateBase(uint64_t new_state, uint64_t l2_base_addr)
{ 
  *(volatile uint64_t *)(l2_base_addr + offsetof(L2Central_ID_t, state)) = new_state;
}
__INLINE__ void SPEC_SetIDState(L2C_SPECID_t spec_id, uint64_t new_state)
{ 
   ( ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].state ) = new_state;
}
__INLINE__ void SPEC_SetIDState_priv(L2C_SPECID_t spec_id, uint64_t new_state)
{ 
   ( ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->id[spec_id].state ) = new_state;
}


/*! 
 * \brief Returns the conflict bitvevctor for the specified speculative ID
 * \param[in] spec_id Speculative ID.  0-127
 * \return Conflict register
 */
__INLINE__ uint64_t SPEC_GetConflictBase(uint64_t l2_base_addr)
{ 
  return *(volatile uint64_t *)(l2_base_addr + offsetof(L2Central_ID_t, conflict));
}
__INLINE__ uint64_t SPEC_GetConflict(L2C_SPECID_t spec_id)
{ 
    //printf("SPEC_GetConflict(): spec_id = 0x%x, reg address %p\n", spec_id,
    //       & ((volatile  (TLS_L2_Central_Registers *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->_perSpeculationId[spec_id]._conflict) );
   return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].conflict;
}
__INLINE__ uint64_t SPEC_GetConflict_priv(L2C_SPECID_t spec_id)
{ 
    //printf("SPEC_GetConflict(): spec_id = 0x%x, reg address %p\n", spec_id,
    //       & ((volatile  (TLS_L2_Central_Registers *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->_perSpeculationId[spec_id]._conflict) );
   return ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->id[spec_id].conflict;
}


/*! 
 * \brief Sets the conflict bitvevctor for the specified speculative ID
 * \param[in] spec_id Speculative ID.  0-127
 * \param[in] new_conflict Conflict register
 */
__INLINE__ void SPEC_SetConflictBase(uint64_t new_conflict,uint64_t l2_base_addr)
{ 
  *(volatile uint64_t *)(l2_base_addr + offsetof(L2Central_ID_t, conflict)) = new_conflict;
}
__INLINE__ void SPEC_SetConflict(L2C_SPECID_t spec_id, uint64_t new_conflict)
{ 
   ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].conflict = new_conflict;
}

__INLINE__ void SPEC_SetConflict_priv(L2C_SPECID_t spec_id, uint64_t new_conflict)
{ 
   ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->id[spec_id].conflict = new_conflict;
}

/*! 
 * \brief Atomically clears bits in the conflict bitvevctor for the specified speculative ID
 * \param[in] spec_id Speculative ID.  0-127
 * \param[in] clearmask Any set bit will be atomically cleared in the conflict register
 */
__INLINE__ void SPEC_ClearConflictWithMask(L2C_SPECID_t spec_id, uint64_t clearmask)
{ 
   ( ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].conflictsc ) = clearmask;
}


/*! 
 * \brief Attempts to commit for the specified speculative ID
 * \param[in] spec_id Speculative ID.  0-127
 * \return Returns -1 on failure.  Committed specid on success.
 */
__INLINE__ L2C_SPECID_t SPEC_TryCommitBase(uint64_t l2_base_addr)
{ 
  return *(volatile uint64_t *)(l2_base_addr + offsetof(L2Central_ID_t, trycommit));
}
__INLINE__ L2C_SPECID_t SPEC_TryCommit(L2C_SPECID_t spec_id)
{ 
    return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].trycommit;
}


/*! 
 * \brief Changes state to invalid if old state is speculative. Returns old state.
 * \param[in] spec_id Speculative ID.  0-127
 * \return Returns old state
 */
__INLINE__ uint64_t SPEC_TryInvalidate(L2C_SPECID_t spec_id)
{ 
    return ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].trychangestate;
}


/*! 
 * \brief Changes ID to new state if old state matches predicate
 * \param[in] spec_id Speculative ID.  0-127
 * \param[in] predicate_newstate Predicate for old state in bit 58:61, new state in bit 62:63
 */
__INLINE__ void SPEC_TryChangeState(L2C_SPECID_t spec_id, uint64_t predicate_newstate)
{ 
  ( ((volatile L2Central_t *)L2_CENTRAL_USER_BASE_TLS_ADDRESS)->id[spec_id].trychangestate ) = predicate_newstate;
}
__INLINE__ void SPEC_TryChangeState_priv(L2C_SPECID_t spec_id, uint64_t predicate_newstate)
{ 
  ( ((volatile L2Central_t *)L2_CENTRAL_PRIV_BASE_TLS_ADDRESS)->id[spec_id].trychangestate ) = predicate_newstate;
}



__INLINE__ uint64_t SPEC_GetL1PBase()
{
  uint64_t l1p_base_addr;
  asm volatile(
	       // create base address
	       "li %0,%1;"
	       "sldi %0,%0,27"
	       : "=b" (l1p_base_addr) : "i" (PHYMAP_MINADDR_L1P>>27)
	       );
  return l1p_base_addr;
}

__INLINE__ uint64_t SPEC_GetL1PBase_priv()
{
  uint64_t l1p_base_addr;
  asm volatile(
	       // create base address
	       "li %0,%1;"
	       "sldi %0,%0,27"
	       : "=b" (l1p_base_addr) : "i" ((PHYMAP_MINADDR_L1P|PHYMAP_PRIVILEGEDOFFSET)>>27)
	       );
  return l1p_base_addr;
}


/*! 
 * \brief Returns the speculative ID for 'this' hardware thread
 * \return Speculative ID.  00=non-speculative.  01=non-speculative (software coherent).  02=invalid.  0x80+spec_id
 */
__INLINE__ uint64_t SPEC_GetSpeculationIDSelfBase(uint64_t l1p_base_addr)
{ 
  return *(volatile uint64_t *)(l1p_base_addr+(L1P_THR_SPECULATION_SELF-L1P_ESR));
}

__INLINE__ uint64_t SPEC_GetSpeculationIDSelf()
{ 
  return SPEC_GetSpeculationIDSelfBase(SPEC_GetL1PBase());
}
__INLINE__ uint64_t SPEC_GetSpeculationIDSelf_priv()
{ 
  return SPEC_GetSpeculationIDSelfBase(SPEC_GetL1PBase_priv());
}

/*! 0
 * \brief Sets the speculative ID for 'this' hardware thread
 * \param[in] newspecid Speculative ID.  00=non-speculative.  01=non-speculative (software coherent).  02=invalid.  0x80+spec_id
 */
__INLINE__ void SPEC_SetSpeculationIDSelfBase(uint64_t newspecid, uint64_t l1p_base_addr)
{ 
  *(volatile uint64_t *)(l1p_base_addr+(L1P_THR_SPECULATION_SELF-L1P_ESR)) = newspecid;
}

__INLINE__ void SPEC_SetSpeculationIDSelf(uint64_t newspecid)
{ 
  SPEC_SetSpeculationIDSelfBase(newspecid, SPEC_GetL1PBase());
}
__INLINE__ void SPEC_SetSpeculationIDSelf_priv(uint64_t newspecid)
{ 
  SPEC_SetSpeculationIDSelfBase(newspecid, SPEC_GetL1PBase_priv());
}

/*! 
 * \brief Returns the speculative ID for the specified hardware thread on 'this' core
 * \param[in] tid Hardware Thread ID.
 * \return Speculative ID.  00=non-speculative.  01=non-speculative (software coherent).  02=invalid.  0x80+spec_id
 */
__INLINE__ uint64_t SPEC_GetSpeculationID(uint64_t tid)
{ 
  return ((volatile L1P_THR_SPECULATION_t *)(L1P_THR_SPECULATION_SELF-PHYMAP_PRIVILEGEDOFFSET))->speculationIdTid[tid];
}

/*! 
 * \brief Sets the speculative ID for the specified hardware thread on 'this' core
 * \param[in] tid Hardware Thread ID.
 * \param[in] newspecid Speculative ID.  00=non-speculative.  01=non-speculative (software coherent).  02=invalid.  0x80+spec_id
 */
__INLINE__ void SPEC_SetSpeculationID(uint64_t tid, uint64_t value)
{ 
  ((volatile L1P_THR_SPECULATION_t *)(L1P_THR_SPECULATION_SELF-PHYMAP_PRIVILEGEDOFFSET))->speculationIdTid[tid] = value;
}

/*!
 * \brief Sets the L1 speculation mode controls
 * \see L1P_CFG_SPEC_pf_hit_fwd_l2
 * \see L1P_CFG_SPEC_pf_dfc_disable
 * \see L1P_CFG_SPEC_l1_hit_fwd_l2
 */
__INLINE__ void SPEC_SetSpeculationL1Mode(uint64_t mode)
{
    *((volatile uint64_t*)L1P_CFG_SPEC) = mode;
}

/*!
 * \brief Returns the L1 speculation mode controls
 * \see L1P_CFG_SPEC_pf_hit_fwd_l2
 * \see L1P_CFG_SPEC_pf_dfc_disable
 * \see L1P_CFG_SPEC_l1_hit_fwd_l2 
 */
__INLINE__ uint64_t SPEC_GetSpeculationL1Mode(uint64_t mode)
{
    return *((volatile uint64_t*)L1P_CFG_SPEC);
}

__INLINE__ uint64_t SPEC_Start_Spec_SR_Base(uint64_t spec_id, uint64_t l1p_base_addr, uint64_t l2_base_addr, uint64_t l1p_fast_base_addr)
{
  uint64_t state;
  asm volatile(
	       // not all l1 ld misses may have been sent from A2 to L1P (passed by next STD), this can cause tagging of this load...
	       // store ID, local_spec_reg  -> flushes all stores, as it is I/G
	       "std %3,%4(%2);"
	       // insert ID into base reg
	       "rldimi %1,%3,7,50;"
	       // load state of ID, must depend on store via I/G
	       "ld %0,%5(%1);"
	       // load R0, user_cfg  // blocks all further accesses until use 
	       // => guarantees spec id in place for tagging of subsequent accesses
	       "ld 0,%6(%7);"
	       // use register
	       "ori 0,0,1;"
	       : "=&r" (state),
		 "+b" (l2_base_addr)
	       : "b" (l1p_base_addr),  
		 "r" (spec_id), 
		 "i" (L1P_THR_SPECULATION_SELF-L1P_ESR), 
		 "i" (offsetof(L2Central_ID_t, state)),
		 "i" (L1P_CFG_PF_USR-L1P_ESR),
	         "b" (l1p_fast_base_addr)
	       : "r0"
	       );
  return state;
}

__INLINE__ void SPEC_Start_Spec_SR(uint64_t spec_id)
{
  uint64_t l1p_base_addr = SPEC_GetL1PBase();
  uint64_t l2_id_base_addr = SPEC_GetL2IDBase();
  uint64_t state = SPEC_Start_Spec_SR_Base(spec_id, l1p_base_addr, l2_id_base_addr, l1p_base_addr);
  if(state == L2C_IDSTATE_INVAL){
    // activate interrupt status and switch to invalid state, just in case we hit the race
    SPEC_SetSpeculationIDSelfBase(0x800 | 2, l1p_base_addr);
    while(1);
  }
}

__INLINE__ uint64_t SPEC_Start_Spec_SR_Base_Disable_L1P(uint64_t spec_id, uint64_t l1p_base_addr, uint64_t l2_base_addr, uint64_t l1p_fast_base_addr)
{
  uint64_t state;
  asm volatile(
	       // not all l1 ld misses may have been sent from A2 to L1P (passed by next STD), this can cause tagging of this load...
	       // store ID, local_spec_reg  -> flushes all stores, as it is I/G
	       "std %3,%4(%2);"
	       // insert ID into base reg
	       "rldimi %1,%3,7,50;"
	       // load state of ID, must depend on store via I/G
	       "ld %0,%5(%1);"
	       // load R0, user_cfg  // blocks all further accesses until use 
	       // => guarantees spec id in place for tagging of subsequent accesses
	       "ld 0,%6(%7);"
	       // use register. check if disabled
	       "cmpdi 7,0,0;"
	       // if disabled, done
	       "beq 7,1f;"
	       // turn prefetch off
	       "li 0,0;"
	       "std 0,%6(%2);"
	       // need to inval all? No, racing other core store will be completed by msync
	       "msync;"
	       "1:" 
	       : "=&r" (state),
		 "+b" (l2_base_addr)
	       : "b" (l1p_base_addr),  
		 "r" (spec_id), 
		 "i" (L1P_THR_SPECULATION_SELF-L1P_ESR), 
		 "i" (offsetof(L2Central_ID_t, state)),
		 "i" (L1P_CFG_PF_USR-L1P_ESR),
	         "b" (l1p_fast_base_addr)
	       : "r0","cr7"
	       );
  return state;
}

__INLINE__ void SPEC_Start_Spec_SR_Disable_L1P(uint64_t spec_id)
{
  uint64_t l1p_base_addr = SPEC_GetL1PBase();
  uint64_t l2_id_base_addr = SPEC_GetL2IDBase();
  uint64_t state = SPEC_Start_Spec_SR_Base_Disable_L1P(spec_id, l1p_base_addr, l2_id_base_addr, l1p_base_addr);
  if(state == L2C_IDSTATE_INVAL){
    // activate interrupt status and switch to invalid state, just in case we hit the race
    SPEC_SetSpeculationIDSelfBase(0x800 | 2, l1p_base_addr);
    while(1);
  }
}

__INLINE__ uint64_t SPEC_Start_Spec_LR_Base(uint64_t spec_id, uint64_t l1p_base_addr, uint64_t l2_base_addr, uint64_t l1p_fast_base_addr)
{
  uint64_t state;
  asm volatile(
	       // not all l1 ld misses may have been sent from A2 to L1P (passed by next STD), this can cause tagging of this load...
	       // store ID, local_spec_reg  -> flushes all stores, as it is I/G
	       "std %3,%4(%2);"
	       // insert ID into base reg
	       "rldimi %1,%3,7,50;"
	       // load state of ID, must depend on store via I/G
	       "ld %0,%5(%1);"
	       // load R0, user_cfg  // blocks all further accesses until use 
	       // => guarantees spec id in place for tagging of subsequent accesses
	       "ld 0,%6(%7);"
	       // use register
	       "ori 0,0,1;"
	       : "=&r" (state),
		 "+b" (l2_base_addr)
	       : "b" (l1p_base_addr),  
		 "r" (spec_id), 
		 "i" (L1P_THR_SPECULATION_SELF-L1P_ESR), 
		 "i" (offsetof(L2Central_ID_t, state)),
		 "i" (L1P_CFG_PF_USR-L1P_ESR),
	         "b" (l1p_fast_base_addr)
	       : "r0"
	       );
  return state;
}

__INLINE__ void SPEC_Start_Spec_LR(uint64_t spec_id)
{
  uint64_t l1p_base_addr = SPEC_GetL1PBase();
  uint64_t l2_id_base_addr = SPEC_GetL2IDBase();
  uint64_t state = SPEC_Start_Spec_LR_Base(spec_id, l1p_base_addr, l2_id_base_addr, l1p_base_addr);
  if(state == L2C_IDSTATE_INVAL){
    // activate interrupt status and switch to invalid state, just in case we hit the race
    SPEC_SetSpeculationIDSelfBase(0x800 | 2, l1p_base_addr);
    while(1);
  }
}

#endif
