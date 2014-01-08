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

#ifndef _PCIE_UTL_H_
#define _PCIE_UTL_H_


// Register: UTL System Bus Agent Status Register
// When an error occurs, the relevant bit is set by the hardware.  Software should write '1' to reset
// error indication; writing a '0' has no effect.

#define UTL_SBASR_RNPP _BN(32+0) // RXNP array (inbound non-posted requests FIFO) parity error.  When
          // set, indicates that the parity error was detected on array's read port.
#define UTL_SBASR_RPCP _BN(32+2) // RXPC array (inbound posted requests FIFO) parity error.  When set,
          // indicates that the parity error was detected on array's read port.
#define UTL_SBASR_RCIP _BN(32+4) // RXCIF array (Received completions indications FIFO) parity error.
          // When set, indicates that the parity error was detected on array's read port.
#define UTL_SBASR_RCCP _BN(32+5) // Receive completions control arrays parity error.  When set, indicates
          // that the parity error was detected on array's read port.
#define UTL_SBASR_RFTP _BN(32+8) // RXFT array (inbound read tags queue array) parity error.  When set,
          // indicates that the parity error was detected on array's read port.


// Register: UTL System Bus Agent Error Severity Register
// Each bit in this register determines the type of interrupt tiggered by the respective event from
// the UTL System Bus Agent Status Register. When set the event triggers the primary interrupt (AL_PINT).
// When clear, the event triggers the secondary interrupt (AL_SINT).

#define UTL_SBAESR_RNPS _BN(32+0) // RXNP array (inbound non-posted requests FIFO) parity error severity
          // .
#define UTL_SBAESR_RPCS _BN(32+2) // RXPC array (inbound posted requests FIFO) parity error severity.
#define UTL_SBAESR_RCIS _BN(32+4) // RXCIF array (Received completions indications FIFO) parity error
          // severity.
#define UTL_SBAESR_RCCS _BN(32+5) // Receive completions control arrays parity error severity.
#define UTL_SBAESR_RFTS _BN(32+8) // RXFT array (inbound read tags queue array) parity error severity
          // .


// Register: UTL System Bus Agent Interrupt Enable Register
// Each bit in this register enables UTL interrupts assertion upon detection of events indicated by
// the corresponding bits in the UTL System Bus Agent Status Register.

#define UTL_SBAIER_RNPI _BN(32+0) // RXNP array (inbound non-posted requests FIFO) parity error enable
          // .
#define UTL_SBAIER_RPCI _BN(32+2) // RXPC array (inbound posted requests FIFO) parity error enable.
#define UTL_SBAIER_RCII _BN(32+4) // RXCIF array (Received completions indications FIFO) parity error
          // enable.
#define UTL_SBAIER_RCCI _BN(32+5) // Receive completions control arrays parity error enable.
#define UTL_SBAIER_RFTI _BN(32+8) // RXFT array (inbound read tags queue array) parity error enable.


// Register: UTL Express Port Status Register
// When an error occurs, the relevant bit is set by the hardware.  Software should write '1' to reset
// error indication; writing a '0' has no effect.

#define UTL_PCIEPSR_TPCP _BN(32+1) // TXPC array (outbound posted requests FIFO) parity error.  When set,
          // indicates that a parity error was detected on the array's read port.
#define UTL_PCIEPSR_TNPP _BN(32+2) // TXNP array (outbound non-posted requests FIFO) parity error.  When
          // set, indicates that the parity error was detected on array's read port.
#define UTL_PCIEPSR_TFTP _BN(32+3) // TXFT array (output non-posted requests free tags FIFO) parity error.
          // When set, indicates that the parity error was detected on array's read port.
#define UTL_PCIEPSR_TCAP _BN(32+4) // TXCA array (transmit completions attributes array) parity error.
          // When set, indicates that the parity error was detected on array's read port.
#define UTL_PCIEPSR_TCIP _BN(32+5) // TXCIF array (transmit completions idications array) parity error.
          // When set, indicates that the parity error was detected on array's read port.
#define UTL_PCIEPSR_RCAP _BN(32+6) // RXCA array (receive completions attributes array) parity error. 
          // When set, indicates that the parity error was detected on array's read port.
#define UTL_PCIEPSR_PLUP _BN(32+8) // PCI Express LinkUp status.  When set, indicates that the PCI Express
          // Link moved into the LinkUp state.
#define UTL_PCIEPSR_PLDN _BN(32+9) // PCI Express LinkDown status.  When set, indicates that the PCI Express
          // Link moved into the LinkDown state.
#define UTL_PCIEPSR_OTDD _BN(32+10) // Outbound request discard status.  When set, indicates that the outbound
          // request TLP was discarded.


// Register: UTL Express Port Error Severity Register
// Each bit in this register determines the type of interrupt triggered by the respective event from
// the UTL PCI Express Port Status Register. When st, the event triggers the primary interrupt (AL_PINT).
// When cleared, the event triggers the secondary interrupt (AL_SINT).

#define UTL_PCIEPESR_TPCS _BN(32+1) // TXPC array (outbound posted requests FIFO) parity error severity
          // .
#define UTL_PCIEPESR_TNPS _BN(32+2) // TXNP array (outbound non-posted requests FIFO) parity error severity
          // .
#define UTL_PCIEPESR_TFTS _BN(32+3) // TXFT array (output non-posted requests free tags FIFO) parity error
          // severity.
#define UTL_PCIEPESR_TCAS _BN(32+4) // TXCA array (transmit completions attributes array) parity error severity
          // .
#define UTL_PCIEPESR_TCIS _BN(32+5) // TXCIF array (transmit completions idications array) parity error
          // severity.
#define UTL_PCIEPESR_RCAS _BN(32+6) // RXCA array (receive completions attributes array) parity error severity
          // .
#define UTL_PCIEPESR_PLUS _BN(32+8) // PCI Express LinkUp event severity.
#define UTL_PCIEPESR_PLDS _BN(32+9) // PCI Express LinkDown event severity.
#define UTL_PCIEPESR_OTDS _BN(32+10) // Outbound request discard event severity.


// Register: UTL Express Port Interrupt Enable Register
// Each bit in this register enables UTL interrupt assertion upon detection of events indicated by the
// corresponding bits in the UTL PCI Express Port Status Register.

#define UTL_PCIEPIER_TPCI _BN(32+1) // TXPC array (outbound posted requests FIFO) parity error enable.
#define UTL_PCIEPIER_TNPI _BN(32+2) // TXNP array (outbound non-posted requests FIFO) parity error enable
          // .
#define UTL_PCIEPIER_TFTI _BN(32+3) // TXFT array (output non-posted requests free tags FIFO) parity error
          // enable.
#define UTL_PCIEPIER_TCAI _BN(32+4) // TXCA array (transmit completions attributes array) parity error enable
          // .
#define UTL_PCIEPIER_TCII _BN(32+5) // TXCIF array (transmit completions idications array) parity error
          // enable.
#define UTL_PCIEPIER_RCAI _BN(32+6) // RXCA array (receive completions attributes array) parity error enable
          // .
#define UTL_PCIEPIER_PLUI _BN(32+8) // PCI Express LinkUp event enable.
#define UTL_PCIEPIER_PLDI _BN(32+9) // PCI Express LinkDown event enable.
#define UTL_PCIEPIER_OTDI _BN(32+10) // Outbound request discard event enable.


#endif
