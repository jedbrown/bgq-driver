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


#include "bgpm/include/err.h"


//! These are the error string tables for lookup by errStrg() and lastErrStrg() methods
//! WARNING: The strings are indexed via the absolute value of the Bgpm Error defines in bgpm/include/err.h
//! which must stay consistent with the following headers
//! - spi/include/upci/err.h
//! - bgpm/include/err.h

//! Error String Table Records
//! BgpmErrRec found in ErrBkt.h

const BgpmErrTblRec bgpmOkStrgTbl[] = {
{   BGPM_EOK                            ,   "operation successful"  },
};


const BgpmErrTblRec upciStrgTbl[] = {
{   BGPM_EINTERNAL                      ,   "Internal Error Encountered - see ??? for more information" },
{   BGPM_ENOTIMPL                       ,   "operation or mode not implemented (yet)"   },
{   BGPM_EINV_EVENT                     ,   "invalid event" },
{   BGPM_EINV_PARM                      ,   "invalid parm passed to function"   },
{   BGPM_ENOT_INIT                      ,   "not init yet"  },
{   BGPM_ERESERVED                      ,   "Generic Resource Already Reserved" },
{   BGPM_ESIGRESERVED                   ,   "A2 Signals Reserved (possibles for event are all used)"    },
{   BGPM_ECTRRESERVED                   ,   "Counter Reserved (all possible counters for event are used)"   },
{   BGPM_EL1PMODERESERVED               ,   "Current L1p mode for this core conflicts with requested event L1p mode"    },
{   BGPM_ETHRDCORE_MISMATCH             ,   "EventRec thread or core not equal to prior added events (PunitConfig)" },
{   BGPM_ESYSCALL_DATASIZE              ,   "Syscall data size mismatch. Insure that application and kernel have been compiled with equal driver versions"  },
{   BGPM_ESYSCALL_NOTIMPL               ,   "Syscall is not implemented yet"    },
{   BGPM_ESYSCALL_INVALID               ,   "Syscall operation is invalid"  },
{   BGPM_ESYSCALL_RETRY                 ,   "(Internal Value) Retry this syscall - locked by another thread"    },
{   BGPM_EMODE_MISMATCH                 ,   "Syscall requested mode (upc or ctr) from Upci_Mode_Init does not match active hardware modes"  },
{   BGPM_EMODE_MISMATCH_UNIT            ,   "Syscall requested detailed or trace mode unit number does not match active hardware unit"  },
{   BGPM_EINV_EVT_IDX                   ,   "Invalid event index; no event configured at given index within event set"  },
{   BGPM_EINV_OVF                       ,   "Invalid Ovf Period - period value must be less than 63 bits (< 0x8000_0000_0000_0000) "    },
{   BGPM_ERESERVED1                     ,   "Unused reserved error value"   },
{   BGPM_EINV_OPCTRMODE                 ,   "Counter mode is invalid for this operation (can't set ovf if in sync mode)"    },
{   BGPM_EINV_UKVER_MISMATCH            ,   "Compiled version mismatch between User and Kernel syscall structures"  },
{   BGPM_ETHD_CTRS_RSV                  ,   "PM counters needed for this hwthread are already reserved by a different hwthread" },
{   BGPM_ECSEVT_REQALLTHDS              ,   "PM Counting active on another core thread while core-shared scope events require a single thread"  },
};


const BgpmErrTblRec bgpmStrgTbl[] = {
{   BGPM_EINITED                        ,   "bgpm is already initialized"   },
{   BGPM_EHWINITED                      ,   "bgpm hardware perspective is already initialized"  },
{   BGPM_ECONF_MODE                     ,   "init mode conflicts with another initialized thread mode on the node"  },
{   BGPM_ENOMEM                         ,   "out of memory resources"   },
{   BGPM_EINV_SET_HANDLE                ,   "invalid handle"    },
{   BGPM_EEVT_SET_UNASSIGNED            ,   "event set has not yet been assigned a unit type"   },
{   BGPM_ECONF_UNIT                     ,   "event id conflicts with unit assigned to event set"    },
{   BGPM_EXHAUSTED                      ,   "Unable to add event - available resources exhausted"   },
{   BGPM_EINV_ATTRIB                    ,   "Invalid attribute tag" },
{   BGPM_EINV_ATTRIB_VAL                ,   "Invalid attribute value"   },
{   BGPM_EUNSUP_ATTRIB                  ,   "Attrib is not supported by this event" },
{   BGPM_EUNSUP_OVERFLOW                ,   "Overflow is not supported for requested event" },
{   BGPM_ENO_OVERFLOW                   ,   "No overflow applied"   },
{   BGPM_EARRAY_TOO_SMALL               ,   "User supplied array too small to receive data" },
{   BGPM_ENOT_MUXED                     ,   "Event Set is not multiplex enabled"    },
{   BGPM_ECONF_L2_COMBINE               ,   "An L2 'slice' or 'combined' event was requested while the L2 mode is 'combined' or 'slice' respectively"   },
{   BGPM_ENO_CONFIG                     ,   "No events configured"  },
{   BGPM_ENOT_MASK_EVENT                ,   "Event is not a generic instruction MASK event (PEVT_INSTR_QPFU_MASK or PEVT_INSTR_XU_MASK)"    },
{   BGPM_ENOT_QFPU_INST_EVENT           ,   "Event is not a QFPU instruction event (PEVT_INST_QFPU_*)"  },
{   BGPM_ENOT_EMPTY                     ,   "Event Set not empty"   },
{   BGPM_EUNREC_EVTID                   ,   "Unrecognized event id (event id value doesn't match known event)"  },
{   BGPM_EINV_OP                        ,   "Requested Bgpm Operation is invalid for set type"  },
{   BGPM_ESHM_OPEN_ERR                  ,   "Can't open shared memory - check errno value"  },
{   BGPM_ESWTHD_MOVED                   ,   "SW Thread has been moved between hardware thread since Bgpm_Init. "    },
{   BGPM_ENOT_APPLIED                   ,   "Event set is not applied or attached"  },
{   BGPM_EINV_L2_SLICE                  ,   "Invalid L2 Slice Number"   },
{   BGPM_ETHD_COUNTS_RSV                ,   "Punit counters needed for attach target thread have already been reserved by another event set"    },
{   BGPM_EINV_SUBNODE                   ,   "Subnode job detected and is not supported by Bgpm" },
{   BGPM_EPRERSV_EXISTS                 ,   "Prereserved Signals and Counter have already been defined" },
{   BGPM_ECTX_MISMATCH                  ,   "Punit Context mismatch -  requested context conflicts with existing A2 for context on core" },
{   BGPM_EEVT_CTX_MISMATCH              ,   "Punit Context mismatch - event set contains other than A2 events while core context filter set to user or kernel context only "    },
{   BGPM_EES_FROZEN                     ,   "EvtSet definition has been frozen by prior operation (e.g. Apply or Attach). No further changes allowed"   },
{   BGPM_EQFPU_RSV_MISMATCH             ,   "Punit Qfpu Instr Match already reserved for core with different value" },
{   BGPM_EXU_RSV_MISMATCH               ,   "Punit XU Instr Match already reserved for core with different value"   },
{   BGPM_ESQRT_SCALE_MISMATCH           ,   "Punit Square-root operation floating point scale already reserved for core with different value"   },
{   BGPM_EDIV_SCALE_MISMATCH            ,   "Punit Division operation floating point scale already reserved for core with different value"  },
{   BGPM_ETHRES_MISMATCH                ,   "A different node wide overflow threshold has already been reserved for this counter (L2 or I/O events)"    },
{   BGPM_ENOT_CONTROL_THREAD            ,   "Operation disallowed.  Only the creator thread may perform controlling operations on an event set (apply,attach,write,reset,start,stop)"   },
{   BGPM_EUPDATE_DISALLOWED             ,   "Event Update Operation disallowed while event set is actively running" },
{   BGPM_EMAX_EVTSETS                   ,   "Maximum number of event sets per thread (10*#thds) have been reached (Use envvar EVTSETS_PER_THREAD to increase)"  },
{   BGPM_ECTRS_IN_USE                   ,   "Counters needed by event set have already been attached by another event set"  },
{   BGPM_ELINK_CTRS_INCOMPAT            ,   "Link counters already attached by another event set in a configuration incompatible with current event set"    },
{   BGPM_ELONGDESC_CORRUPT              ,   "The Event Long Description file is not in expected format or has been corrupted"   },
{   BGPM_EINV_WRT2OVF                   ,   "Write Event operations are invalid for events with active overflow (may only read or reset)"   },
};


// Next table is the positive warning codes (they don't cause bgpm to exit if ExitOnError is set).
const BgpmErrTblRec bgpmWarnStrgTbl[] = {
{   BGPM_WALREADY_ATTACHED              ,   "Resource has already been attached "   },
{   BGPM_WNOT_ATTACHED                  ,   "Not attached by this sw thread"    },
{   BGPM_WALREADY_STARTED               ,   "Shared counters are already started"   },
{   BGPM_WALREADY_STOPPED               ,   "Shared counters are already stopped"   },
{   BGPM_WEVTSET_STILL_USED             ,   "Event Set could not be deleted; still in use by other threads or logic.  See Bgpm_DerefEventSet()."    },
{   BGPM_WNOT_LINK_CTL                  ,   "Event Set doesn't control any links"   },
};
