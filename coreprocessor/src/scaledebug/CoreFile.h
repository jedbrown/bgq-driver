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

#ifndef COREFILE_H
#define COREFILE_H

// Includes
#include <ramdisk/include/services/ToolctlMessages.h>
#include <string>
#include <vector>
#include <fstream>
#include <tr1/memory>

using namespace bgcios::toolctl;
using namespace std;

class CoreFileThread
{

public:

   //! \brief  Default constructor.

   CoreFileThread()
   {
      memset(&_threadData, '\0', sizeof(_threadData));
      memset(&_gprs, '\0', sizeof(_gprs));
      memset(&_sprs, '\0', sizeof(_sprs));
      memset(&_fprs, '\0', sizeof(_fprs));
      memset(&_dbrs, '\0', sizeof(_dbrs));
   }

   //! \brief  Set the thread id.
   //! \param  tid New thread id value.
   //! \return Nothing.

   void setThreadId(BG_ThreadID_t tid) { _threadData.threadID = tid; }

   //! \brief  Get the thread id.
   //! \return Thread id value.

   BG_ThreadID_t getThreadId(void) const { return _threadData.threadID; }

   //! \brief  Set the data for the thread.
   //! \param  tdata Pointer to thread data structure.
   //! \return Nothing.

   void setThreadData(GetThreadDataAckCmd *tdata) { memcpy(&_threadData, tdata, sizeof(_threadData)); }

   //! \brief  Get the thread data for the thread.
   //! \return Pointer to thread data structure.

   const GetThreadDataAckCmd *getThreadData(void) const { return &_threadData; }

   //! \brief  Get the core number that thread is running on.
   //! \return Core number value.

   int getCore(void) const { return _threadData.core; }

   //! \brief  Get the hardware thread number that thread is running on.
   //! \return Hardware thread number value.

   int getHwThread(void) const { return _threadData.thread; }

   //! \brief  Get the state of the thread.
   //! \return Thread state value.

   BG_Thread_State getState(void) const { return _threadData.state; }

   //! \brief  Get the number of stack frames in the list.
   //! \return Number of stack frames.

   uint32_t getNumStackFrames(void) const { return _threadData.numStackFrames; }

   //! \brief  Get the list of stack frames.
   //! \return Pointer to list of stack frames.

   const BG_Stack_Info *getStackInfo(void) const { return _threadData.stackInfo; }

   //! \brief  Set the general purpose registers for the thread.
   //! \param  regs Pointer to general purpose registers structure.
   //! \return Nothing.

   void setGeneralRegs(GetGeneralRegsAckCmd *regs) { memcpy(&_gprs, regs, sizeof(_gprs)); }

   //! \brief  Get the general purpose registers for the thread.
   //! \return Pointer to general purpose registers structure.

   const GetGeneralRegsAckCmd *getGeneralRegs(void) const { return &_gprs; }

   //! \brief  Set the special purpose registers for the thread.
   //! \param  regs Pointer to special purpose registers structure.
   //! \return Nothing.

   void setSpecialRegs(GetSpecialRegsAckCmd *regs) { memcpy(&_sprs, regs, sizeof(_sprs)); }

   //! \brief  Get the special purpose registers for the thread.
   //! \return Pointer to special purpose registers structure.

   const GetSpecialRegsAckCmd *getSpecialRegs(void) const { return &_sprs; }

   //! \brief  Get the instruction address register value.
   //! \return Register value.

   BG_Reg_t getIAR(void) const { return _sprs.sregs.iar; } 

   //! \brief  Get the data exception address register value.
   //! \return Register value.

   BG_Reg_t getDEAR(void) const { return _sprs.sregs.dear; }

   //! \brief  Set the floating point registers for the thread.
   //! \param  regs Pointer to floating point registers structure.
   //! \return Nothing.

   void setFloatRegs(GetFloatRegsAckCmd *regs) { memcpy(&_fprs, regs, sizeof(_fprs)); }

   //! \brief  Get the floating point registers for the thread.
   //! \return Pointer to floating point registers structure.

   const GetFloatRegsAckCmd *getFloatRegs(void) const { return &_fprs; }

   //! \brief  Set the debug registers for the thread.
   //! \param  regs Pointer to debug registers structure.
   //! \return Nothing.

   void setDebugRegs(GetDebugRegsAckCmd *regs) { memcpy(&_dbrs, regs, sizeof(_dbrs)); }

   //! \brief  Get the debug registers for the thread
   //! \return Pointer to debug registers structure.

   const GetDebugRegsAckCmd *getDebugRegs(void) const { return &_dbrs; }


private:

   //! Info about the thread.
   GetThreadDataAckCmd _threadData;

   //! General purpose registers.
   GetGeneralRegsAckCmd _gprs;

   //! Special purpose registers.
   GetSpecialRegsAckCmd _sprs;

   //! Floating point registers.
   GetFloatRegsAckCmd _fprs;

   //! Debug registers.
   GetDebugRegsAckCmd _dbrs;

};

//! Smart pointer for CoreFileThread object.
typedef std::tr1::shared_ptr<CoreFileThread> CoreFileThreadPtr;

// Forward reference.
class CoreFile;

//! Smart pointer for CoreFile object.
typedef std::tr1::shared_ptr<CoreFile> CoreFilePtr;


class CoreFile
{
public:

   //! \brief  Default constructor.

   CoreFile();

   //! \brief  Write the core file to the file system.
   //! \param  directory Path to directory where core file is created or NULL for current working directory.
   //! \param  prefix Prefix for core file name or NULL for default prefix.
   //! \return 0 when successful, errno when unsuccessful..

   int writeFile(char *directory, char *prefix);

   //! \brief  Write the general purpose registers to the core file.
   //! \param  regs Pointer to general purpose registers structure.
   //! \return Nothing.

   void writeGPRs(const GetGeneralRegsAckCmd *regs);

   //! \brief  Write the special purpose registers to the core file.
   //! \param  regs Pointer to special purpose registers structure.
   //! \return Nothing.

   void writeSPRs(const GetSpecialRegsAckCmd *regs);

   //! \brief  Write the floating point registers to the core file.
   //! \param  regs Pointer to floating point registers structure.
   //! \return Nothing.

   void writeFPRs(const GetFloatRegsAckCmd *regs);

   //! \brief  Write the debug registers to the core file.
   //! \param  regs Pointer to debug registers structure.
   //! \return Nothing.

   void writeDBRs(const GetDebugRegsAckCmd *regs);

   //! \brief  Write the thread memory info to the core file.
   //! \param  tdata Pointer to thread data structure.
   //! \return Nothing.

   void writeMemory(const GetThreadDataAckCmd *tdata);

   //! \brief  Write a stack traceback to the core file.
   //! \param  thread Pointer to CoreFileThread object.
   //! \return Nothing.

   void writeStack(CoreFileThreadPtr thread);

   //! \brief  Get the value of a boolean environment variable.
   //! \param  name Environment variable name.
   //! \param  defaultValue Default value if environment variable is not set.
   //! \return Boolean value of environment variable.

   static int getBooleanVar(const char *name, int defaultValue = 0);
   
   //! \brief  Compare two core files and sort by instruction address register.
   //! \param  a Pointer to first CoreFile object.
   //! \param  b Pointer to second CoreFile object.
   //! \return True if IAR in first core file is less than IAR in second core file.

   static int compareByIAR(CoreFilePtr a, CoreFilePtr b);


   //! \brief  Set the job id.
   //! \param  jobid New job id value.
   //! \return Nothing.

   void setJobId(int jobid) { _jobId = jobid; }

   //! \brief  Set the process information.
   //! \param  pdata Pointer to process data structure.
   //! \return Nothing.

   void setProcessData(GetProcessDataAckCmd *pdata) { memcpy(&_processData, pdata, sizeof(_processData)); }

   void setRank(uint32_t rank) { _processData.rank = rank; }

   //! \brief  Get the rank of the process.
   //! \return Rank value.

   uint32_t getRank(void) const { return _processData.rank; }

   //! \brief  Set the program name.
   //! \param  name Pointer to program name string.
   //! \return Nothing.

   void setProgramName(string name) { _programName = name; }

   //! \brief  Add a thread to the list.
   //! \param  thread Pointer to CoreFileThread object.
   //! \return Nothing.

   void clearThreads() { _threadList.clear(); }
   void addThread(CoreFileThreadPtr thread) { _threadList.push_back(thread); }

   //! \brief  Get the number of threads in the list.
   //! \return Number of threads in list.

   int getNumThreads(void) const { return _threadList.size(); }

   //! \brief  Get thread id of a thread in the list.
   //! \param  index Index of thread in list.
   //! \return Thread id.

   BG_ThreadID_t getThreadIdByIndex(int index) const { return _threadList.at(index)->getThreadId(); }

   //! \brief  Get a thread from the list by its thread id.
   //! \param  tid Thread id.
   //! \return Pointer to CoreFileThread object.

   CoreFileThreadPtr getThreadById(BG_ThreadID_t tid)
   {
      for (int index = 0; index < _threadList.size(); ++index) {
         CoreFileThreadPtr thread = _threadList.at(index);
         if (thread->getThreadId() == tid) {
            return thread;
         }
      }
      CoreFileThreadPtr notFound;
      return notFound;
   }

   bool validProcessInfo;
   bool validThreadInfo;
   
private:

   //! Job id.
   int _jobId;

   // Path to program.
   string _programName;

   //! Information about the process.
   GetProcessDataAckCmd _processData;
   
   //! List of threads in the process.
   vector <CoreFileThreadPtr> _threadList;
};

#endif // COREFILE_H
