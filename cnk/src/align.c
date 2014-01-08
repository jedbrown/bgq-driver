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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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


#include "Kernel.h"

int align_handler(Regs_t* regs)
{
   char             buffer[sizeof(QPX_Reg_t)] ALIGN_QPX_VECTOR;
   char             old_qvr0[sizeof(QPX_Reg_t)] ALIGN_QPX_VECTOR;
   uint64_t         old_msr;
   uint32_t         opcode;
   uint64_t         ra;
   uint64_t         rt;
   uint32_t         update     = 0;
   size_t           storebytes = 0;
   int              handled    = 1;

   KThread_t *kthr = GetMyKThread();
   kthr->AlignmentExceptionCount++;
   if (kthr->AlignmentExceptionCount > kthr->pAppProc->AlignmentExceptionMax)
   {
      return 0;  // not handled
   }

   if(!VMM_IsAppAddress((void*)regs->dear, sizeof(buffer)))
      return 0;  // not handled
   // Load current state into a 32-byte aligned buffer
   memcpy(buffer, (void*)regs->dear, sizeof(buffer));

#define PRINT_SP_BUFFER printf("buffer values: %08x, %08x, %08x, %08x\n", *buffer, *(buffer+4), *(buffer+8), *(buffer+12));
#define PRINT_DP_BUFFER printf("buffer values: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n", *buffer, *(buffer+4), *(buffer+8), *(buffer+12), (*buffer+16), *(buffer+20), *(buffer+24), *(buffer+28));

   if(!VMM_IsAppAddress((void *) regs->ip, sizeof(uint32_t)))
      return 0;  // not handled
   opcode = *((uint32_t*)regs->ip);

   // decode register values.  
   // RT is for loading/storing fp value.  
   // RA is for update.  If updated, RA=DEAR
   // RB is not needed since we already know the effective EA from the DEAR

   rt = (opcode & 0x03e00000) >> 21;
   ra = (opcode & 0x001f0000) >> 16;

   // The QPX is disabled at this point.  We need to enable it and manually
   // preserve any QVRs we plan to use.
   old_msr = mfmsr();
   mtmsr(old_msr | MSR_FP);
   if (rt != 0)
   {
      // Move the target vector to qvr0, first preserving the content there.
      asm volatile("qvstfdx 0,0,%0" : : "b" (old_qvr0) : "memory");    /*Ignore BEAM complaint:*/  /*uninitialized*/
      switch (rt)
      {
	 case 1: asm volatile("qvfmr 0,1"); break;
	 case 2: asm volatile("qvfmr 0,2"); break;
	 case 3: asm volatile("qvfmr 0,3"); break;
	 case 4: asm volatile("qvfmr 0,4"); break;
	 case 5: asm volatile("qvfmr 0,5"); break;
	 case 6: asm volatile("qvfmr 0,6"); break;
	 case 7: asm volatile("qvfmr 0,7"); break;
	 case 8: asm volatile("qvfmr 0,8"); break;
	 case 9: asm volatile("qvfmr 0,9"); break;
	 case 10: asm volatile("qvfmr 0,10"); break;
	 case 11: asm volatile("qvfmr 0,11"); break;
	 case 12: asm volatile("qvfmr 0,12"); break;
	 case 13: asm volatile("qvfmr 0,13"); break;
	 case 14: asm volatile("qvfmr 0,14"); break;
	 case 15: asm volatile("qvfmr 0,15"); break;
	 case 16: asm volatile("qvfmr 0,16"); break;
	 case 17: asm volatile("qvfmr 0,17"); break;
	 case 18: asm volatile("qvfmr 0,18"); break;
	 case 19: asm volatile("qvfmr 0,19"); break;
	 case 20: asm volatile("qvfmr 0,20"); break;
	 case 21: asm volatile("qvfmr 0,21"); break;
	 case 22: asm volatile("qvfmr 0,22"); break;
	 case 23: asm volatile("qvfmr 0,23"); break;
	 case 24: asm volatile("qvfmr 0,24"); break;
	 case 25: asm volatile("qvfmr 0,25"); break;
	 case 26: asm volatile("qvfmr 0,26"); break;
	 case 27: asm volatile("qvfmr 0,27"); break;
	 case 28: asm volatile("qvfmr 0,28"); break;
	 case 29: asm volatile("qvfmr 0,29"); break;
	 case 30: asm volatile("qvfmr 0,30"); break;
	 case 31: asm volatile("qvfmr 0,31"); break;
      }
   }

   // Loads/Stores from/to address rt need to be double-hummer loads
   // Loads/Stores from/to address buffer need to match the emulated instruction
   // Emulated load operations must first load the current contents of rt into fpr0 unless the emulated 
   //    load will touch both primary&secondary parts of the fpr
   // Update operations must set the update flag.
   // Store operations must set the amount of storage that they are touching in 'buffer'.
   switch((opcode & 0xfc000000)>>26)
   {
      case 31: // xform
	 switch((opcode & 0x7fe)>>1)
	 {
	    // regular floating point opcodes
	    case 551: // qvlfsuxa
	       update = 1;   /*fall through*/
	    case 519: // qvlfsxa
	       asm volatile("qvlfsx  0, 0, %0;" : : "r" (buffer));
	       break;
	       
	    case 615: // qvlfduxa
	       update = 1;   /*fall through*/
	    case 583: // qvlfdxa
	       asm volatile("qvlfdx  0, 0, %0;" : : "r" (buffer));
	       break;
	       
	    case 39: // qvlfcsuxa
	       update = 1;   /*fall through*/
            case 7: // qvlfcsxa
               asm volatile("qvlfcsx  0, 0, %0;" : : "r" (buffer));
               break;

            case 103: // qvlfcduxa
		update = 1;   /*fall through*/
            case 71: // qvlfcdxa
               asm volatile("qvlfcdx  0, 0, %0;" : : "r" (buffer));
               break;

            case 871: // qvlfiwaxa
               asm volatile("qvlfiwax  0, 0, %0;" : : "r" (buffer));
               break;

	    case 839: // qvlfiwzxa
               asm volatile("qvlfiwzxa  0, 0, %0;" : : "r" (buffer));
               break;

            case 679: // qvstfsuxa
               update = 1;   /*fall through*/
            case 647: // qvstfsxa
               storebytes = 32;
               asm volatile("qvstfsx 0, 0, %0;" : : "r" (buffer) : "memory");
               break;

	    case 743: // qvstfduxa
	       update = 1;   /*fall through*/
	    case 711: // qvstfdxa
	       storebytes = 32;
	       asm volatile("qvstfdx 0, 0, %0;" : : "r" (buffer) : "memory");
	       break;

	    case 677: // qvstfsuxia
               update = 1;   /*fall through*/
	    case 645: // qvstfsxia
	       storebytes = 16;
	       asm volatile("qvstfsxi 0, 0, %0;" : : "r" (buffer) : "memory");
	       break;
	      
            case 741: // qvstfduxia
               update = 1;   /*fall through*/
            case 709: // qvstfdxia
               storebytes = 32;
               asm volatile("qvstfdxi 0, 0, %0;" : : "r" (buffer) : "memory");
               break;

            case 167: // qvstfcsuxa
               update = 1;   /*fall through*/
            case 135: // qvstfcsxa
               storebytes = 8;
               asm volatile("qvstfcsx 0, 0, %0;" : : "r" (buffer) : "memory");
               break;

	    case 231: // qvstfcduxa
               update = 1;   /*fall through*/
            case 199: // qvstfcdxa
               storebytes = 16;
               asm volatile("qvstfcdx 0, 0, %0;" : : "r" (buffer) : "memory");
               break;


            case 165: // qvstfcsuxia
               update = 1;   /*fall through*/
            case 133: // qvstfcsxia
               storebytes = 8;
               asm volatile("qvstfcsxi 0, 0, %0;" : : "r" (buffer) : "memory");
               break;

            case 229: // qvstfcduxia
               update = 1;   /*fall through*/
            case 197: // qvstfcdxia
               storebytes = 16;
               asm volatile("qvstfcdxi 0, 0, %0;" : : "r" (buffer) : "memory");
               break;

            case 967: // qvstfiwxa
               storebytes = 16;
               asm volatile("qvstfiwx 0, 0, %0;" : : "r" (buffer) : "memory");
               break;

	    default:
	       handled = 0;  // not handled
	       break;
	 }
	 break;
	 
#if 0
// non QPX instructions commented out for now
      case 49: // lfsu
	 update = 1;   /*fall through*/
      case 48: // lfs
	 asm volatile("lfpdx  0, 0, %0;"
		      "lfsx   0, 0, %1;"
		      "stfpdx 0, 0, %0;" : : "b" (rt), "b" (buffer) : "fr0", "memory");
	 break;
      case 51: // lfdu
	 update = 1;   /*fall through*/
      case 50: // lfd
	 asm volatile("lfpdx 0, 0, %0;"
		      "lfdx  0, 0, %1;"
		      "stfdx 0, 0, %0;" : : "b" (rt), "b" (buffer) : "fr0", "memory");
	 break;
	 
      case 53: // stfsu
	 update = 1;   /*fall through*/
      case 52: // stfs
	 storebytes = 4;
	 asm volatile("lfpdx 0, 0, %0;"
		      "stfsx 0, 0, %1;" : : "b" (rt), "b" (buffer) : "fr0", "memory");

	 break;

      case 55: // stfdu
	 update = 1;   /*fall through*/
      case 54: // stfd
	 storebytes = 8;
	 asm volatile("lfpdx 0, 0, %0;"
		      "stfdx 0, 0, %1;" : : "b" (rt), "b" (buffer) : "fr0", "memory");	 
	 break;
#endif

      default:
	 handled = 0;  // not handled
	 break;
   }

   // Restore preserved QVRs and re-disable QPX.
   if (rt != 0)
   {
      // Move the target vector back where it came from.
      switch (rt)
      {
	 case 1: asm volatile("qvfmr 1,0"); break;
	 case 2: asm volatile("qvfmr 2,0"); break;
	 case 3: asm volatile("qvfmr 3,0"); break;
	 case 4: asm volatile("qvfmr 4,0"); break;
	 case 5: asm volatile("qvfmr 5,0"); break;
	 case 6: asm volatile("qvfmr 6,0"); break;
	 case 7: asm volatile("qvfmr 7,0"); break;
	 case 8: asm volatile("qvfmr 8,0"); break;
	 case 9: asm volatile("qvfmr 9,0"); break;
	 case 10: asm volatile("qvfmr 10,0"); break;
	 case 11: asm volatile("qvfmr 11,0"); break;
	 case 12: asm volatile("qvfmr 12,0"); break;
	 case 13: asm volatile("qvfmr 13,0"); break;
	 case 14: asm volatile("qvfmr 14,0"); break;
	 case 15: asm volatile("qvfmr 15,0"); break;
	 case 16: asm volatile("qvfmr 16,0"); break;
	 case 17: asm volatile("qvfmr 17,0"); break;
	 case 18: asm volatile("qvfmr 18,0"); break;
	 case 19: asm volatile("qvfmr 19,0"); break;
	 case 20: asm volatile("qvfmr 20,0"); break;
	 case 21: asm volatile("qvfmr 21,0"); break;
	 case 22: asm volatile("qvfmr 22,0"); break;
	 case 23: asm volatile("qvfmr 23,0"); break;
	 case 24: asm volatile("qvfmr 24,0"); break;
	 case 25: asm volatile("qvfmr 25,0"); break;
	 case 26: asm volatile("qvfmr 26,0"); break;
	 case 27: asm volatile("qvfmr 27,0"); break;
	 case 28: asm volatile("qvfmr 28,0"); break;
	 case 29: asm volatile("qvfmr 29,0"); break;
	 case 30: asm volatile("qvfmr 30,0"); break;
	 case 31: asm volatile("qvfmr 31,0"); break;
      }
      asm volatile("qvlfdx 0,0,%0" : : "b" (old_qvr0) : "memory");
   }
   mtmsr(old_msr);
    
   if (handled)
   {
      regs->ip += 4; // load/store was handled, advance to next instruction
      if(update)
      {
	 regs->gpr[ra] = regs->dear;
	 if (ra >= 14)
	 {
	    // The register to be updated is a non-volatile.  It has not been
	    // saved explicitly in this context and therefore will not be
	    // restored.  Set a pending bit to force its update.
	    kthr->Pending |= KTHR_PENDING_NVGPR;
	    kthr->PendingNVGPR = ra;
	 }
      }
      if(storebytes)
      {
	 // Store only the update data to memory
	 memcpy((void*)regs->dear, buffer, storebytes);
      }
   }
   return handled;
}

__END_DECLS
