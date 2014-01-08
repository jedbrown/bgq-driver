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
/* (C) Copyright IBM Corp.  2008, 2011                              */
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
#include "CoreFile.h"
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include <iomanip>

CoreFile::CoreFile()
{
   _jobId = 0;
   _programName = "?";
   memset(&_processData, '\0', sizeof(_processData));
   _threadList.clear();
}

#if 0
int
CoreFile::writeFile(char *directory, char *prefix)
{
   // Just return if no data was saved.
   if (_faultThreadId < 0) {
      return 0;
   }

   // Open the core file.
   ostringstream name;
   if (directory != NULL) {
      name << directory << "/";
   }
   if (prefix != NULL) {
      name << prefix;
   }
   else {
      name << "lwcore";
   }
   name << "." << _jobId << "." << _processData.rank;
   _corefile.open(name.str().c_str(), ios::out);

   // Write the header.
   _corefile << "+++PARALLEL TOOLS CONSORTIUM LIGHTWEIGHT COREFILE FORMAT version 1.0" << endl;
   _corefile << "+++LCB 1.0" << endl;
   _corefile << "Program: " << _programName << endl;
   _corefile << "Job ID: " << _jobId << endl;
   _corefile << "Rank: " << _processData.rank << endl;
   _corefile << "ABCDET Coords: " << _processData.aCoord << " " << _processData.bCoord << " " << _processData.cCoord << " ";
   _corefile << _processData.dCoord << " " << _processData.eCoord << " " << _processData.tCoord << endl;
   _corefile << "Run time: " << _processData.jobTime.tv_sec << "." << _processData.jobTime.tv_usec << " seconds" << endl;
   if (getBooleanVar("BG_COREDUMP_MEMORY", 1)) {
      _corefile << "Process Memory:" << endl;
      _corefile << "  Heap:    0x" << setfill('0') << setw(16) << hex << _processData.heapStartAddr;
      _corefile << "...0x" << setfill('0') << setw(16) << _processData.heapEndAddr;
      _corefile << " (break at 0x" << setfill('0') << setw(16) << hex << _processData.heapBreakAddr << dec << ")" << endl;
      _corefile << "  Mmap:    ";
      if (_processData.mmapStartAddr != 0) {
         _corefile << "0x" << setfill('0') << setw(16) << hex << _processData.mmapStartAddr;
         _corefile << "...0x" << setfill('0') << setw(16) << _processData.mmapEndAddr << dec << endl;
      }
      else {
         _corefile << "none allocated" << endl;
      }
      _corefile << "  Shared:  ";
      if (_processData.sharedMemoryStartAddr != 0) {
         _corefile << "0x" << setfill('0') << setw(16) << hex << _processData.sharedMemoryStartAddr;
         _corefile << "...0x" << setfill('0') << setw(16) << _processData.sharedMemoryEndAddr << dec << endl;
      }
      else {
         _corefile << "none allocated" << endl;
      }
      _corefile << "  Persist: ";
      if (_processData.persistMemoryStartAddr != 0) {
         _corefile << "0x" << setfill('0') << setw(16) << hex << _processData.persistMemoryStartAddr;
         _corefile << "...0x" << setfill('0') << setw(16) << _processData.persistMemoryEndAddr << dec << endl;
      }
      else {
         _corefile << "none allocated" << endl;
      }
   }

   for (int index = 0; index <_threadList.size(); ++index) {
      CoreFileThreadPtr thread = _threadList.at(index);
      _corefile << "+++ID TGID " << _processData.tgid << ", Thread " << thread->getThreadId();
      _corefile << " (core " << thread->getCore() << " hwthrd " << thread->getHwThread() << ")" << endl;
      if (thread->getThreadId() == _faultThreadId) {
         _corefile << "***FAULT Encountered unhandled signal 0x" << setfill('0') << setw(8) << hex << _faultSignum << dec << " (" << _faultSignum << ")" << endl;
         _corefile << "While executing instruction at 0x" << setfill('0') << setw(16) << hex << thread->getIAR() << dec << endl;
         _corefile << "Dereferencing memory at        0x" << setfill('0') << setw(16) << hex << thread->getDEAR() << dec << endl;
      }
      _corefile << "State: ";
      switch (thread->getState()) {
         case Running:
            _corefile << "running" << endl;
            break;
         case Sleeping:
            _corefile << "sleeping" << endl;
            break;
         case Waiting:
            _corefile << "waiting" << endl;
            break;
         case Idle:
            _corefile << "idle" << endl;
            break;
         default:
            _corefile << "unknown" << endl;
            break;
      }

      // Write the general purpose registers.
      int dumpRegs = getBooleanVar("BG_COREDUMP_REGS", 1);
      if (dumpRegs && getBooleanVar("BG_COREDUMP_GPR", 1)) {
         writeGPRs(thread->getGeneralRegs());
      }

      // Write the special purpose registers.
      if (dumpRegs && getBooleanVar("BG_COREDUMP_SPR", 1)) {
         writeSPRs(thread->getSpecialRegs());
      }

      // Write the debug registers.
      if (dumpRegs && getBooleanVar("BG_COREDUMP_DBR", 1)) {
         writeDBRs(thread->getDebugRegs());
      }

      // Write the floating point registers.
      if (dumpRegs && getBooleanVar("BG_COREDUMP_FPR", 1)) {
         writeFPRs(thread->getFloatRegs());
      }

      // Write the thread memory info.
      if (getBooleanVar("BG_COREDUMP_MEMORY", 1)) {
         writeMemory(thread->getThreadData());
      }

      // Write the stack trace back.
      if (getBooleanVar("BG_COREDUMP_STACK", 1)) {
         writeStack(thread);
      }

      _corefile << "---ID" << endl;

   }

   _corefile << "---LCB" << endl;
   _corefile.close();
   return 0;
}

void
CoreFile::writeGPRs(const GetGeneralRegsAckCmd *regs)
{
   _corefile << "General Purpose Registers:";
   for (int index = 0; index < NumGPRegs; ++index) {
      if (index % 4 == 0) {
         _corefile << endl << " ";
      }
      _corefile << " r" << setfill('0') << setw(2) << index << "=" << setfill('0') << setw(16) << hex << regs->gpr[index] << dec;
   }
   _corefile << endl;
   return;
}

void
CoreFile::writeSPRs(const GetSpecialRegsAckCmd *regs)
{
   _corefile << "Special Purpose Registers:" << endl << hex;
   _corefile << "  lr=" << setfill('0') << setw(16) << regs->lr;
   _corefile << " cr=" << setfill('0') << setw(16) << regs->cr;
   _corefile << " xer=" << setfill('0') << setw(16) << regs->xer;
   _corefile << " ctr=" << setfill('0') << setw(16) << regs->ctr << endl;
   _corefile << "  msr=" << setfill('0') << setw(16) << regs->msr;
   _corefile << " dear=" << setfill('0') << setw(16) << regs->dear;
   _corefile << " esr=" << setfill('0') << setw(16) << regs->esr;
   _corefile << " fpscr=" << setfill('0') << setw(16) << regs->fpscr << endl;
   return;
}

void
CoreFile::writeDBRs(const GetDebugRegsAckCmd *regs)
{
   _corefile << "Debug Registers:" << endl << hex;
   _corefile << "  dbcr0=" << setfill('0') << setw(16) << regs->dbcr0;
   _corefile << " dbcr1=" << setfill('0') << setw(16) << regs->dbcr1;
   _corefile << " dbcr2=" << setfill('0') << setw(16) << regs->dbcr2;
   _corefile << " dbcr3=" << setfill('0') << setw(16) << regs->dbcr3 << endl;
   _corefile << " dac1=" << setfill('0') << setw(16) << regs->dac1;
   _corefile << " dac2=" << setfill('0') << setw(16) << regs->dac2;
   _corefile << " dac3=" << setfill('0') << setw(16) << regs->dac3;
   _corefile << " dac4=" << setfill('0') << setw(16) << regs->dac4 << endl;
   _corefile << " dbsr=" << setfill('0') << setw(16) << regs->dbsr << endl << dec;
   return;
}

void
CoreFile::writeFPRs(const GetFloatRegsAckCmd *regs)
{
   _corefile << "Floating Point Registers:";
#if 0
   for (int index = 0; index < NumFPRegs; ++index) {
      if (index % 2 == 0) {
         _corefile << endl;
      }
      _corefile << "  f" << setfill('0') << setw(2) << index << "=" << setfill('0') << setw(8) << hex << regs->fprs[index].w0;
      _corefile << " " << setfill('0') << setw(8) << regs->fprs[index].w1;
      _corefile << "  " << setfill('0') << setw(8) << regs->fprs[index].w2;
      _corefile << " " << setfill('0') << setw(8) << regs->fprs[index].w3 << dec;
   }
#endif
   _corefile << endl;
   return;
}

void
CoreFile::writeMemory(const GetThreadDataAckCmd *tdata)
{
   _corefile << "Thread Memory:" << endl;
   _corefile << "  Stack:  0x" << setfill('0') << setw(16) << hex << tdata->stackStartAddr;
   _corefile << "...0x" << setfill('0') << setw(16) << tdata->stackCurrentAddr << dec << endl;
   _corefile << "  Guard:  ";
   if ((tdata->guardStartAddr == (BG_Addr_t)-1) && (tdata->guardEndAddr == (BG_Addr_t)-1)) {
      _corefile << "disabled" << endl;
   }
   else {
      _corefile << "0x" << setfill('0') << setw(16) << hex << tdata->guardStartAddr;
      _corefile << "...0x" << setfill('0') << setw(16) << tdata->guardEndAddr << dec << endl;
   }
   return;
}

void
CoreFile::writeStack(CoreFileThreadPtr thread)
{
   _corefile << "+++STACK" << endl;
   
   // Run addr2line to resolve the addresses and save the results in a temporary file.
   const BG_Stack_Info_t *stackInfo = thread->getStackInfo();
   ostringstream tracebackFilePath;
   tracebackFilePath << "/tmp/" << _jobId << "." << _processData.tgid << ".traceback";
   ostringstream addr2lineCommand;
   addr2lineCommand << "addr2line -e " << _programName << " -f 0x" << hex << thread->getIAR() << dec;
   for (uint32_t frame = 1; frame < thread->getNumStackFrames(); ++frame) {
      addr2lineCommand << " 0x" << hex << (stackInfo[frame].savedLR - 4) << dec;
   }
   addr2lineCommand << " >" << tracebackFilePath.str();
   int rc = system(addr2lineCommand.str().c_str());
   
   // Read the output from addr2line to generate the stack trace back.
   ifstream tracebackFile(tracebackFilePath.str().c_str(), ios::in);
   string function;
   getline(tracebackFile, function);
   string lineno;
   getline(tracebackFile, lineno);
   _corefile << "0x" << setfill('0') << setw(16) << hex << thread->getIAR() << dec << " in " << function << "() at " << lineno << endl;
   for (uint32_t frame = 1; frame < thread->getNumStackFrames(); ++frame) {
      getline(tracebackFile, function);
      getline(tracebackFile, lineno);
      if (stackInfo[frame].savedLR != 0) {
         _corefile << "0x" << setfill('0') << setw(16) << hex << (stackInfo[frame].savedLR - 4) << dec << " in " << function << "() at " << lineno << endl;
      }
   }
   
   // Close and remove the temporary file.
   tracebackFile.close();
   unlink(tracebackFilePath.str().c_str());
   
   _corefile << "---STACK" << endl;
   return;
}

#endif

int
CoreFile::getBooleanVar(const char *name, int defaultValue)
{
   int value = defaultValue;
   char *vp = getenv(name);
   if (vp != NULL) {
      value = atoi(vp);
   }
   return value;
}
