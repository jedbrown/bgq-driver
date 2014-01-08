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
/*!
 *
 * \page bgpm_int_notes Internal Notes
 *
 * This pages collects some notes about internal operation.
 * Since my memory tends to swiss cheese over time, I need this.
 * Also, there are some internal concepts between software layers
 * which overlap and policies for handling which should be noted.
 *
 * \section bgpm_notes_sources Event Source Confusion
 *
 * The term "source" shows up in multiple places and is easily confused.
 * - Events in the event table are grouped by source type (upc_evttbl.h)
 *   This type is also used to select how to assign or reserve
 *   events, but the actual event ranges are used to select the major unit types.
 * - In the "Sources" elements of the Events-Map.xml, the name id is used to
 *   to document the name.  But, the "value" field is matched with the "Events.xml"
 *   source values to match them.
 * - The upc_p.h event group and subgroup defines look similar to a subset of
 *   the sources.  They only refer to sources within the upc_p, and software
 *   does the appropirate mappeing during reservations.
 *
 * The are three types of Modes which are very similar:
 * - Bgpm Mode gives a user level name to how the
 *   hardware settings and hardware/software usage context.
 *   The UPC and Counter modes can be completely determined from the
 *   Bgpm mode.
 *   \n\n
 *   The BGPM mode must be consistent across all threads on the node, and
 *   value is tracked in shared memory.  The value in shared memory is
 *   set by Bgpm_Init() by the 1st thread creating (initializing) the
 *   shared memory space.  The mode is indirectly released in Bgpm_Disable()
 *   when the last pShMem ptr is set to null, and ref counting in the
 *   smart pointer causes the shared memory space to be marked uninitialized.
 *   \n\n
 * - The UPC mode is 0, 1 or 2, which indicates distributed, detailed or trace.
 *   It is a hardware setting multiple Bgpm software and hardware modes
 *   map to distributed, but the 1 & 2 are only hardware modes.
 *   \n\n
 * - The UPC_Ctr_Mode_t provides further indication of how the upc_p's should
 *   be connected to the upc_c ring, and whether counters should be controlled
 *   in sync or independently.
 *   \n\n
 * - Both the UPC Mode and UPC_Ctr_Mode are tracked in a Upci_Mode_t object.
 *   The 1st Bgpm_Init() in each process creates one of these, the value is
 *   stored or verified against a copy in shared memory.
 *   Also, the Upci_Mode_Init() function does a kernel syscall to configure
 *   the hardware appropriately, and the kernel references counts the
 *   number of calls and insures they are consistent.
 *   \n\n
 *   The kernel reference counting is there for pure Upci interface tests
 *   and diagnostics.
 *   However, is not really needed under Bgpm since the shared memory is
 *   already being used to keep things consistent.
 *   Both will remain active, so is a potential
 *   place reference counts could get out of sync if bugs exist.
 *
 *
 *
 *
 * \section bgpm_notes_conventions Internal Code Conventions
 *
 * Some internal code conventions and symbol abbreviations (hopefully used consistently)
 * - strg = string
 * - ovf = overflow
 * - thd = thread
 * - prc = process
 * - core = processor core
 * - hwthd = hardware thread
 * - swthd = software thread
 * - ctr = counter
 * - evt = event
 * - p (start of name) = pointer
 * - h (start of name) = handle
 * - A (end of name) = array ??
 * - a (start of name) = accumulation
 * - idx = index (index used in place off offset)
 * - blk = block
 * - K = kernel
 * - r (start of name) = reference
 * - s (start of name) = structure name
 * - e (start of name) = enum name
 * - ALL CAPITALS - constants and enumerated values
 * - low level names (UPC stuff) tend to be lower case with underscores, typedefs with *_t in name.
 * - higher level names (C++ or API) use mixed case names with few underscores.
 * - higher level class/type names start with capital
 * - variable names start with lower case.
 * - Function and Method names start with Capital.
 * - Names are mixed case even when abbrev is acronym (e.g. BGPM = Bgpm in a symbol name)
 * - API functions start with Bgpm
 * - BGPM spi objects start with Bgpm
 * - UPC related spi objects start with Upc
 * - C++ Class header & source files use the mixed case class name
 * - C header and source are lower case containing '_'
 *
 */
