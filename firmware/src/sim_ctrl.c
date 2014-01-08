/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#include <firmware/include/Firmware.h>

#include <stdint.h>

int callthru0(int command)
   {
   register int c asm ("r3") = command;
   asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c) : "memory");
   return c;
   }

#if 0
int callthru1(int command, unsigned long arg1)
   {
   register int c asm ("r3") = command;
   register unsigned long a1 asm ("r4") = arg1;
   asm volatile (".long 0x000EAEB0" : "=r" (c): "r" (c), "r" (a1) : "memory");
   return c;
   }
#else
int callthru1( uint64_t command, const char *arg1 )
{
  asm volatile (".long 0x000EAEB0;"
                : "=b" (command)
                : "b"  (arg1),
                  "0"  (command)
                : "cc", "memory" );
  return command;
}
#endif

#define SimPrintCode 178
#define SimExitCode   31

void sim_putc( char src )
   {
   char buf[2] = { src, 0 };
   callthru1( SimPrintCode, (const char *)&(buf[0]) );
   }

int sim_puts( const char *src )
{
   return callthru1(SimPrintCode, src );
}

__attribute__((noreturn))
void
sim_exit(int rc)
   {
   callthru0(SimExitCode);
   (void)rc;
   for (;;);
   }

// Simulation control interface for mambo and fpga simulators.
//
// Here we define the magic memory locations by which a program
// running under simulation can:
//
// - emit ascii characters to standard output
// - exit(terminate) the simulation
//

typedef struct SimControlPseudoUart
   {
   volatile unsigned char output;   // output interpreted as ascii
            unsigned char pad[31];  // pad to maintain 32-byte alignment
   } SimControlPseudoUart;

typedef struct SimControlExit
   {
   volatile int  output; // output interpreted as exit code
            int  pad[7]; // pad to maintain 32-byte alignment
   } SimControlExit;

typedef struct SimControl
   {
   SimControlPseudoUart sim_uarts[64];  // one per thread
   SimControlExit sim_exit;             // one per simulation
   } SimControl;

//
// virtual memory address of "SimControl" data structure
// that will be monitored by simulator for store operations
// Note: assume a ERAT entry mapping top of 32-bit to top
// of physical address space.
//
#define SIM_CONTROL_PADDR 0x03ffFFFC0000ULL

#define SPRN_G4     0x104  // g4  any   none

#define SPRG_TID    SPRN_G4

typedef unsigned long REGISTER;

REGISTER getTID()
{
   REGISTER val;
   asm ("mfspr %0,%1" : "=r"(val) : "i"(SPRG_TID));
   return val;
}



void uart_putc(char src)
{
   unsigned    tid = getTID();                        // thread id (0..63)
   SimControl *sim = (SimControl *)SIM_CONTROL_PADDR; // assume vaddr==paddr
   sim->sim_uarts[tid].output = src;
}

void uart_puts(const char *src)
{
   unsigned    tid = getTID();                        // thread id (0..63)
   SimControl *sim = (SimControl *)SIM_CONTROL_PADDR; // assume vaddr==paddr
   while (*src)
      sim->sim_uarts[tid].output = *src++;
}

