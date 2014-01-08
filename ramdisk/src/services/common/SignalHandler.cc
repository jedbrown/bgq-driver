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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  SignalHandler.cc
//! \brief Methods for bgcios::SignalHandler and bgcios::SigtermHandler classes.

// Includes
#include <ramdisk/include/services/common/SignalHandler.h>
#include <ramdisk/include/services/common/logging.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/Cioslog.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <execinfo.h>
#include <iomanip>
#include <fcntl.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

//! Pointer to SigtermHandler object so handler can find it.
void *SigtermHandler::thisPtr;

SignalHandler::SignalHandler(int signum)
{
   // Set data members.
   _signalNumber = signum;

   // Enable handler for the signal.
   setHandler();
}

int
SignalHandler::setHandler(info_handler_t handler)
{
   // Set a handler for the signal.
   int err = 0;
   struct sigaction sigact;
   memset(&sigact, '\0', sizeof(sigact));
   sigact.sa_flags |= SA_SIGINFO;
   sigact.sa_sigaction = handler;
   if (sigaction(_signalNumber, &sigact, NULL) != 0) {
      err = errno;
      LOG_ERROR_MSG("error setting action to handle for signal " << _signalNumber << ": " << bgcios::errorString(err));
   }

   return err;
}

int
SignalHandler::setIgnore(void)
{
   // Ignore the signal.
   int err = 0;
   struct sigaction sigact;
   memset(&sigact, '\0', sizeof(sigact));
   sigact.sa_handler = SIG_IGN;
   if (sigaction(_signalNumber, &sigact, NULL) != 0) {
      err = errno;
      LOG_ERROR_MSG("error setting action to ignore for signal " << _signalNumber << ": " << bgcios::errorString(err));
   }

   return err;
}

int
SignalHandler::setDefault(void)
{
   // Set default action for the signal.
   int err = 0;
   struct sigaction sigact;
   memset(&sigact, '\0', sizeof(sigact));
   sigact.sa_handler = SIG_DFL;
   if (sigaction(_signalNumber, &sigact, NULL) != 0) {
      err = errno;
      LOG_ERROR_MSG("error setting action to default for signal " << _signalNumber << ": " << bgcios::errorString(err));
   }

   return err;
}

void
SignalHandler::myHandler(int signum, siginfo_t *siginfo, void *sigcontext)
{
#ifdef __PPC64__
   // Log information from the context.
   ucontext_t *context = (ucontext_t *)sigcontext;
   LOG_ERROR_MSG("Received signal " << signum << ", code=" << siginfo->si_code << " errno=" << siginfo->si_errno <<
                 " address=" << siginfo->si_addr << " nip=" << (void *)(context->uc_mcontext.regs->nip) <<
                 " lr=" << (void *)(context->uc_mcontext.regs->link));
#else

   // Don't do anything, just need to reference the variables so it compiles.
   siginfo = 0; sigcontext = 0;

#endif

#if 0
   LOG_ERROR_MSG(std::setfill('0') << std::hex << 
                  "gpr00=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[0] <<
                 " gpr01=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[1] <<
                 " gpr02=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[2] <<
                 " gpr03=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[3]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex << 
                  "gpr04=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[4] <<
                 " gpr05=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[5] <<
                 " gpr06=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[6] <<
                 " gpr07=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[7]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex << 
                  "gpr08=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[8]  <<
                 " gpr09=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[9]  <<
                 " gpr10=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[10] <<
                 " gpr11=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[11]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex << 
                  "gpr12=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[12] <<
                 " gpr13=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[13] <<
                 " gpr14=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[14] <<
                 " gpr15=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[15]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex << 
                  "gpr16=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[16] <<
                 " gpr17=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[17] <<
                 " gpr18=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[18] <<
                 " gpr19=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[19]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex <<
                  "gpr20=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[20] <<
                 " gpr21=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[21] <<
                 " gpr22=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[22] <<
                 " gpr23=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[23]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex <<
                  "gpr24=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[24] <<
                 " gpr25=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[25] <<
                 " gpr26=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[26] <<
                 " gpr27=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[27]);
   LOG_ERROR_MSG(std::setfill('0') << std::hex <<
                  "gpr28=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[28] <<
                 " gpr29=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[29] <<
                 " gpr30=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[30] <<
                 " gpr31=0x" << std::setw(16) << context->uc_mcontext.regs->gpr[31]);
#endif
   // Log a back trace of the stack.
   logStackBackTrace(MaxBackTraceSize);

   // End the program.
   switch (signum){
    case SIGABRT:
    case SIGSEGV:
    {  
      struct sigaction sigact;
      memset(&sigact,0,sizeof(sigact) );
      sigact.sa_handler = SIG_DFL;
      sigaction(signum,&sigact,NULL);
      printlastLogEntries(4);
      raise(signum);
      break;
    }
    default:
   _exit(99);
  }
}

SigtermHandler::SigtermHandler()
{
   // Set data members.
   _signalNumber = SIGTERM;
   _caught = false;
   this->thisPtr = this;

   // Enable a handler for the specified signal.
   setHandler(myHandler);
}

void
#ifdef LOG_CIOS_INFO_DISABLE
SigtermHandler::myHandler(int, siginfo_t*, void *)
#else
SigtermHandler::myHandler(int signum, siginfo_t *siginfo, void *sigcontext)
#endif
{
   LOG_CIOS_INFO_MSG("Received signal " << signum << ", code=" << siginfo->si_code << " errno=" << siginfo->si_errno <<
                " address=" << siginfo->si_addr << " nip=" << (void *)(((ucontext_t *)sigcontext)->uc_mcontext.regs->nip) <<
                " lr=" << (void *)(((ucontext_t *)sigcontext)->uc_mcontext.regs->link));

   // Remember that signal has been caught.
   SigtermHandler *me = (SigtermHandler *)thisPtr;
   me->_caught = true;

   return;
}

Sigusr2Handler::Sigusr2Handler()
{
   // Set data members.
   _signalNumber = SIGUSR2;

   // Enable a handler for the specified signal.
   setHandler(myHandler);
}

void
Sigusr2Handler::myHandler(int, siginfo_t *, void *)
{  

   return;
}

//! Pointer to SigWritePipe object so handler can find it.
void *SigWritePipe::thisPtr;

SigWritePipe::SigWritePipe(int signal_number)
{
   // Set data members.
   _signalNumber = signal_number;

   this->thisPtr = this;

   // Enable a handler for the specified signal.
   setHandler(myHandler);

   // pipe info for signal handler
   
    if ( pipe(_pipe_descriptor) != 0 ) {
      _pipe_descriptor[0]=-1;
      _pipe_descriptor[1]=-1;
    } else {
        fcntl(_pipe_descriptor[0], F_SETFD, fcntl(_pipe_descriptor[0], F_GETFD) | FD_CLOEXEC);
        fcntl(_pipe_descriptor[1], F_SETFD, fcntl(_pipe_descriptor[1], F_GETFD) | FD_CLOEXEC);
    }
}

SigWritePipe::~SigWritePipe()
{
    if ( _pipe_descriptor[0] != -1 ) {
        close( _pipe_descriptor[0] );
        _pipe_descriptor[0] = -1;
    }
    if ( _pipe_descriptor[1] != -1 ) {
        close( _pipe_descriptor[1] );
        _pipe_descriptor[1] = -1;
    }
}

void
SigWritePipe::myHandler(int , siginfo_t *siginfo, void *)
{  
   SigWritePipe *me = (SigWritePipe *)thisPtr;
   (void)write( me->writePipeFd(), siginfo, sizeof(siginfo_t) );
   return;
}
