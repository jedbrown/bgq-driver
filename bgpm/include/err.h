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

#ifndef _BGPM_ERR_H_  // Prevent multiple inclusion
#define _BGPM_ERR_H_


//! \addtogroup bgpm_user_api
//@{
/**
 * \file
 *
 * \brief BGPM Error Values
 *
 */
//@}

#include <unistd.h>
#include <stdio.h>

#include "spi/include/upci/upc_common.h"
#include "spi/include/upci/err.h"


__BEGIN_DECLS


//! Error values to be captured in variables to remain consistent with syscall return variables
//! - 0 means successful
//! - error codes are negative, and values from -999 thru -1 map to negative errno values (usually returned by syscalls)
//! - errors from -1999 thru -1000 are UPCI errors
//! - errors from -2999 thru -2000 are Bgpm Errors
//! - positive values from 3000 thru 4000 are Bgpm Warning values
//! WARNING: The UPCI Error defines below must remain consistent with values in
//! - bgpm/include/err.h
//! - bgpm/src/err.cc

#define BGPM_EOK                            (0) //!<    operation successful

#define BGPM_EINTERNAL                      (-1000) //!<    Internal Error Encountered - see ??? for more information
#define BGPM_ENOTIMPL                       (-1001) //!<    operation or mode not implemented (yet)
#define BGPM_EINV_EVENT                     (-1002) //!<    invalid event
#define BGPM_EINV_PARM                      (-1003) //!<    invalid parm passed to function
#define BGPM_ENOT_INIT                      (-1004) //!<    not init yet
#define BGPM_ERESERVED                      (-1005) //!<    Generic Resource Already Reserved
#define BGPM_ESIGRESERVED                   (-1006) //!<    A2 Signals Reserved (possibles for event are all used)
#define BGPM_ECTRRESERVED                   (-1007) //!<    Counter Reserved (possibles for event are all used)
#define BGPM_EL1PMODERESERVED               (-1008) //!<    Current L1p mode for this core conflicts with requested event L1p mode
#define BGPM_ETHRDCORE_MISMATCH             (-1009) //!<    EventRec thread or core not equal to prior added events (PunitConfig)
#define BGPM_ESYSCALL_DATASIZE              (-1010) //!<    Syscall data size mismatch. Insure that application and kernel have been compiled with equal driver versions
#define BGPM_ESYSCALL_NOTIMPL               (-1011) //!<    Syscall is not implemented yet
#define BGPM_ESYSCALL_INVALID               (-1012) //!<    Syscall operation is invalid
#define BGPM_ESYSCALL_RETRY                 (-1013) //!<    (Internal Value) Retry this syscall locked by another thread
#define BGPM_EMODE_MISMATCH                 (-1014) //!<    Syscall requested mode (upc or ctr) from Upci_Mode_Init does not match active hardware modes
#define BGPM_EMODE_MISMATCH_UNIT            (-1015) //!<    Syscall requested detailed or trace mode unit number does not match active hardware unit
#define BGPM_EINV_EVT_IDX                   (-1016) //!<    Invalid event index; no event configured at given index within event set
#define BGPM_EINV_OVF                       (-1017) //!<    Invalid Ovf Period period value must be less than 63 bits (< 0x8000_0000_0000_0000)
#define BGPM_ERESERVED1                     (-1018) //!<    Unused reserved error value
#define BGPM_EINV_OPCTRMODE                 (-1019) //!<    Counter mode is invalid for this operation (can't set ovf if in sync mode)
#define BGPM_EINV_UKVER_MISMATCH            (-1020) //!<    Compiled version mismatch between User and Kernel syscall structures
#define BGPM_ETHD_CTRS_RSV                  (-1021) //!<    Core base pm counters normally used by this hwthread are already reserved by a different hwthread
#define BGPM_ECSEVT_REQALLTHDS              (-1022) //!<    Events with core-shared scope require that all core counters are owned by single thread

// Prior error codes are common with UPCI see spi/include/upci/err.h
#define BGPM_MIN_ERRNO                      (-2000)
#define BGPM_EINITED                        (-2000) //!<    bgpm is already initialized
#define BGPM_EHWINITED                      (-2001) //!<    bgpm hardware perspective is already initialized
#define BGPM_ECONF_MODE                     (-2002) //!<    init mode conflicts with another initialized thread mode on the node
#define BGPM_ENOMEM                         (-2003) //!<    out of memory resources
#define BGPM_EINV_SET_HANDLE                (-2004) //!<    invalid handle
#define BGPM_EEVT_SET_UNASSIGNED            (-2005) //!<    event set has not yet been assigned a unit type
#define BGPM_ECONF_UNIT                     (-2006) //!<    event id conflicts with unit assigned to event set
#define BGPM_EXHAUSTED                      (-2007) //!<    Unable to add event - available resources exhausted
#define BGPM_EINV_ATTRIB                    (-2008) //!<    Invalid attribute tag
#define BGPM_EINV_ATTRIB_VAL                (-2009) //!<    Invalid attribute value
#define BGPM_EUNSUP_ATTRIB                  (-2010) //!<    Attrib is not supported by this event
#define BGPM_EUNSUP_OVERFLOW                (-2011) //!<    Overflow is not supported for requested event
#define BGPM_ENO_OVERFLOW                   (-2012) //!<    No overflow applied
#define BGPM_EARRAY_TOO_SMALL               (-2013) //!<    User supplied array too small to receive data
#define BGPM_ENOT_MUXED                     (-2014) //!<    Event Set is not multiplex enabled
#define BGPM_ECONF_L2_COMBINE               (-2015) //!<    An L2 'slice' or 'combined' event was requested while the L2 mode is 'combined' or 'slice' respectively
#define BGPM_ENO_CONFIG                     (-2016) //!<    No events configured
#define BGPM_ENOT_MASK_EVENT                (-2017) //!<    Event is not a generic instruction MASK event (PEVT_INSTR_QPFU_MASK or PEVT_INSTR_XU_MASK)
#define BGPM_ENOT_QFPU_INST_EVENT           (-2018) //!<    Event is not a QFPU instruction event (PEVT_INST_QFPU_*)
#define BGPM_ENOT_EMPTY                     (-2019) //!<    Event Set not empty
#define BGPM_EUNREC_EVTID                   (-2020) //!<    Unrecognized event id (event id value doesn't match known event)
#define BGPM_EINV_OP                        (-2021) //!<    Requested Bgpm Operation is invalid for set type
#define BGPM_ESHM_OPEN_ERR                  (-2022) //!<    Can't open shared memory - check errno value
#define BGPM_ESWTHD_MOVED                   (-2023) //!<    SW Thread has been moved between hardware thread since Bgpm_Init.
#define BGPM_ENOT_APPLIED                   (-2024) //!<    Event set is not applied or attached
#define BGPM_EINV_L2_SLICE                  (-2025) //!<    Invalid L2 Slice Number
#define BGPM_ETHD_COUNTS_RSV                (-2026) //!<    Punit counters needed for attach target thread have already been reserved by another event set
#define BGPM_EINV_SUBNODE                   (-2027) //!<    Subnode job detected and is not supported by Bgpm
#define BGPM_EPRERSV_EXISTS                 (-2028) //!<    Prereserved Signals and Counter have already been defined
#define BGPM_ECTX_MISMATCH                  (-2029) //!<    Punit Context mismatch requested context conflicts with existing A2 for context on core
#define BGPM_EEVT_CTX_MISMATCH              (-2030) //!<    Punit Context mismatch - event set contains other than A2 events while core context filter set to user or kernel context only
#define BGPM_EES_FROZEN                     (-2031) //!<    EvtSet definition has been frozen by prior operation (e.g. Apply or Attach). No further changes allowed
#define BGPM_EQFPU_RSV_MISMATCH             (-2032) //!<    Punit Qfpu Instr Match already reserved for core with different value
#define BGPM_EXU_RSV_MISMATCH               (-2033) //!<    Punit XU Instr Match already reserved for core with different value
#define BGPM_ESQRT_SCALE_MISMATCH           (-2034) //!<    Punit Square-root operation floating point scale already reserved for core with different value
#define BGPM_EDIV_SCALE_MISMATCH            (-2035) //!<    Punit Division operation floating point scale already reserved for core with different value
#define BGPM_ETHRES_MISMATCH                (-2036) //!<    A different node wide overflow threshold has already been reserved for this counter (L2 or I/O events)
#define BGPM_ENOT_CONTROL_THREAD            (-2037) //!<    Operation disallowed.  Only the creator thread may perform controlling operations on an event set (apply,attach,write,reset,start,stop)
#define BGPM_EUPDATE_DISALLOWED             (-2038) //!<    Event Update Operation disallowed while event set is actively running
#define BGPM_EMAX_EVTSETS                   (-2039) //!<    Maximum number of event sets per thread (10*#thds) have been reached (Use envvar EVTSETS_PER_THREAD to increase)
#define BGPM_ECTRS_IN_USE                   (-2040) //!<    Counters needed by event set have already been attached by another event set
#define BGPM_ELINK_CTRS_INCOMPAT            (-2041) //!<    Link counters already attached by another event set in a configuration incompatible with current event set
#define BGPM_ELONGDESC_CORRUPT              (-2042) //!<    The Event Long Description file is not in expected format or has been corrupted
#define BGPM_EINV_WRT2OVF                   (-2043) //!<    Write Event operations are invalid for events with active overflow (may only read or reset)

#define BGPM_MAX_ERRNO                      (-2043)

// Next defines are positive warning codes (they don't cause bgpm to exit if ExitOnError is set.
#define BGPM_MIN_WARNING_NO                 (3000)
#define BGPM_WALREADY_ATTACHED              (3000)  //!<    Resource has already been attached
#define BGPM_WNOT_ATTACHED                  (3001)  //!<    Not attached by this sw thread
#define BGPM_WALREADY_STARTED               (3002)  //!<    Shared counters are already started
#define BGPM_WALREADY_STOPPED               (3003)  //!<    Shared counters are already stopped
#define BGPM_WEVTSET_STILL_USED             (3004)  //!<    Event Set could not be deleted; still in use by other threads or logic.  See Bgpm_DerefEventSet().
#define BGPM_WNOT_LINK_CTL                  (3005)  //!<    Event Set doesn't control any links

#define BGPM_MAX_WARNING_NO                 (3005)




/** Bgpm_LastErr()
 * return the last Bgpm function error which occurred on this thread.
 * The value is not reset on successful function completion.
 * @return  error value
 */
int Bgpm_LastErr();



/** Bgpm_LastErr()
 * return the last Bgpm function error string which occurred on this thread.
 * The value points to a static memory location owned by Bgpm.
 * String may include names or type names of objects which were in error.
 * It does not include the name of the function which ended in error.
 * @return  English error string
 */
const char *Bgpm_LastErrStrg();



/** Bgpm_ErrStrg()
 * return Bgpm function error string for given error number.
 * Possible object names in string are filled with generic names.
 * @param   errnum  error number to interprete.
 * @return  English error string
 */
const char *Bgpm_ErrStrg(int errnum);



//! Error String Table Records
typedef struct sBgpmErrTblRec {
    short errNum;       //!<    expected error num at this index to sanity check that table indexes haven't got out of sync with define values
    const char *desc;
} BgpmErrTblRec;



__END_DECLS


#endif
