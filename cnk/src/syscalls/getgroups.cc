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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

// Includes
#include "Kernel.h"

//! \brief  Function ship the getgroups system call.
//! \param  r3,r4,...,r8 Syscall parameters.
//! \return CNK_RC_SUCCESS(number of bytes returned) or CNK_RC_FAILURE(errno).

uint64_t sc_getgroups(SYSCALL_FCN_ARGS)
{
   int gidsetsize  = r3;
   gid_t *grouplist = (gid_t *)r4;
   int listsize = sizeof(gid_t) * gidsetsize;

   TRACESYSCALL(("(I) %s%s: gidsetsize=%d, grouplist=0x%p\n", __func__, whoami(), gidsetsize, grouplist));

   AppState_t *appState = GetMyAppState();

   // Just return the number of secondary groups.
   if (gidsetsize == 0) {
      return CNK_RC_SUCCESS(appState->NumSecondaryGroups);
   }

   // Check for error conditions.
   if (gidsetsize < (int)appState->NumSecondaryGroups) {
      return CNK_RC_FAILURE(EINVAL);
   }

   if (!VMM_IsAppAddress(grouplist, listsize)) {
      return CNK_RC_FAILURE(EFAULT);
   }

#if 0
   // Copy the list of secondary groups to the caller's buffer.
   memcpy(grouplist, appState->SecondaryGroups, sizeof(gid_t) * appState->NumSecondaryGroups);
   return CNK_RC_SUCCESS(appState->NumSecondaryGroups);
#endif

   return CNK_RC_SUCCESS(0);
}

