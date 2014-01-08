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

#ifndef _BGPM_FUNCS_H_  // Prevent multiple inclusion
#define _BGPM_FUNCS_H_

#include <unistd.h>
#include <stdint.h>

#include "hwi/include/common/compiler_support.h"
#include "spi/include/upci/upci.h"
#include "types.h"
#include "err.h"


//! \addtogroup bgpm_user_api
//!@{
/**
 *
 * \file
 *
 * \brief Bgpm user SPI C function prototypes.
 *
 */
//!@}


__BEGIN_DECLS;


/**
 * Initialize the software thread to use the Hardware Performance Counter library.
 * @note Each thread to call Bgpm operations must start with a Bgpm_Init() and preferably end with a Bgpm_Disable()
 *       to free up allocated resources.
 * @param[in]   mode     \ref Bgpm_Modes value to indicate the BGPM mode of operation,
 *                       which determines the various capabilities of BGPM hardware
 *                       and how resources are shared between threads.
 *                       See \ref bgpm_swfeatures_modes for more information.
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_ENOTIMPL
 *      - \ref BGPM_EINITED
 *      - \ref BGPM_ECONF_MODE
 *      - <em>list is incomplete</em>
 */
int Bgpm_Init(Bgpm_Modes mode);




/**
 * Disable BGPM operations in current software thread.
 * Needed to free all resources and initialize for new mode of Bgpm operation.
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_ENOT_INIT
 *
 */
int Bgpm_Disable();




// ****************************
// Functions to add events + functions to apply attributes
// ****************************


/**
 * Create a new empty event set.
 * @return
 *    - >= 0 - Event Set Handle value
 *    - < 0 - error code.  Some relevant errors:
 *      - \ref BGPM_ENOMEM
 *      - <em>list is incomplete</em>
 */
int Bgpm_CreateEventSet();

/**
 * Delete existing event set.
 * It will also stop any counters only in use by
 * this event set, and free counting resources.
 *
 * @param[in]   hEvtSet  handle to event set to remove.
 *
 * @return
 *    - 0   - success
 *    - < 0 - error code.  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_DeleteEventSet(unsigned hEvtSet);



/**
 * Append an event to existing event set.
 *
 * An event set is assigned to a Unit type (\ref bgpm_units) based on the
 * event source of the first event added (or based on a unique attribute type).
 * All events must be of the same unit type.
 *
 * Event resources (signals and counters) are reserved as each event is added.
 * If an event cannot be added (because of conflicting resources needs or out
 * of resources) the call will fail.
 *
 * Attributes to an event (Overflow for example) must be applied to
 * an event (indicated by event index) after adding to an event set
 * (for example Bgpm_SetOverflow()).
 *
 * @param[in]   hEvtSet   handle to event set
 * @param[in]   eventId   eventId to add
 *
 * @return
 *    - = 0 returns 0 if even has been added successfully.
 *    - <0 negative error codes.  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_ECONF_UNIT
 *      - \ref BGPM_EXHAUSTED
 *      - \ref BGPM_EL1PMODERESERVED
 *      - \ref BGPM_ECONF_L2_COMBINE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_AddEvent(unsigned hEvtSet, unsigned eventId);


/**
 * Append a list of events to existing event set.
 *
 * The applicable unit type is determined by the first event source type.
 *
 * @param[in]   hEvtSet     handle to event set
 * @param[in]   *eventIds   array of event ids to add,
 * @param[in]   num         Number of entries in array
 *
 * @return
 *    - =0 - returns 0 if the event set has been added successfully.
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_ECONF_UNIT
 *      - \ref BGPM_EXHAUSTED
 *      - \ref BGPM_EL1PMODERESERVED
 *      - \ref BGPM_ECONF_L2_COMBINE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_AddEventList(unsigned hEvtSet, unsigned *eventIds, unsigned num);



#if 0
/**
 * Append an event using attribute tag string.
 *
 * ** Stretch Goal **
 *
 * Add an event by event label and attribute tags.
 * \note this method requires more definitions, but events and attributes
 * would be specified using a string in the following fashion:
 *
 * \c event_label:attrib_tag="value":attrib_tag
 *
 * (provide reference to available tags - maybe more flags in event tables)
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtString     string giving event with attributes to add
 *
 * @return
 *    - >= 0 positive event index into hEvtSet
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_ECONF_UNIT
 *      - \ref BGPM_EXHAUSTED
 *      - \ref BGPM_EINV_ATTRIB
 *      - \ref BGPM_EINV_ATTRIB_VAL
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EL1PMODERESERVED
 *      - \ref BGPM_ECONF_L2_COMBINE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_AddEventString(unsigned hEvtSet, char * evtString);




/**
 * Append an event using array of attribute tag strings
 *
 * ** Stretch Goal **
 *
 * Add an event by event label and attribute tags.
 * \note this method requires more definitions, but events and attributes
 * would be specified using a string in the following fashion:
 *
 * \c event_label:attrib_tag="value":attrib_tag
 *
 * (provide reference to available tags - maybe more flags in event tables)
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   *evtStrings   array of event string,
 *                            list terminated by null entry.
 *
 * @return
 *    - >= 0 number of events added
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_ECONF_UNIT
 *      - \ref BGPM_EXHAUSTED
 *      - \ref BGPM_EINV_ATTRIB
 *      - \ref BGPM_EINV_ATTRIB_VAL
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EL1PMODERESERVED
 *      - \ref BGPM_ECONF_L2_COMBINE
 *      - <em>list is incomplete</em>
 */
int Bgpm_AddEventStringList(unsigned hEvtSet, char * evtString[]);
#endif



// ****************************
// Functions to retrieve event information
// ****************************


/**
 * Return number of events in event set
 * Events are indexed from 0 to Bgpm_NumEvents()-1.
 *
 * @param[in]   hEvtSet     handle to event set
 *
 * @return number of events
 *         0 if invalid event set
 *
 */
int Bgpm_NumEvents(unsigned hEvtSet);



/**
 * Return number of attached targets in event set
 *
 * NW and hardware mode punits require Bgpm_Attach operations,
 * and in these cases, the Bgpm_ReadEvent returns values that are
 * the aggregate of results for all attached targets.
 * For convenience, this functions returns the total number
 * of targets attached to event set.
 * Thus, you can determine the average result from the totals.
 *
 * @param[in]   hEvtSet     handle to event set
 *
 * @return number of attached target threads or links
 *         0 if invalid event set or not attached
 *
 */
int Bgpm_NumTargets(unsigned hEvtSet);



/**
 * Return label string for a particular event.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 *
 * @return
 *    - char string
 *    - NULL if error - use Bgpm_LastErr() to retrieve cause error code.
 *
 */
const char * Bgpm_GetEventLabel(unsigned hEvtSet, unsigned evtIdx);



/**
 * Return label string for a particular event ID
 *
 * @param[in]   evtId
 *
 * @return
 *    - char string
 *    - NULL if error - use Bgpm_LastErr() to retrieve cause error code.
 *
 */
const char * Bgpm_GetEventIdLabel(unsigned evtId);



/**
 * Return event id assigned to a event set index
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 *
 * \return
 *    - >0 - positive id value
 *    - 0  - invalid event id (shouldn't occur)
 *    - <0 - negative error code
 *
 */
int Bgpm_GetEventId(unsigned hEvtSet, unsigned evtIdx);



/**
 * Return event id matching a given event label string
 *
 * @param[in]   evtLabel     null terminated label string (case insensitive)
 *
 * \return
 *    - 0  - invalid event id if label not found
 *    - >0 - eventID
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_PARM
 *
 */
int Bgpm_GetEventIdFromLabel(const char *evtLabel);



/**
 * Return event index for event with particular event Id.
 * Since there may be multiple events with the same eventid within
 * an event set (with perhaps different attributes),
 * the startIdx parms indicates the index from which to start the search, if needed.
 *
 * If a the evtId is not found in the event set, it is not treated
 * as an error (errors by default result in exiting the program).
 * However, a subsequent use of the returned value as an index in
 * other functions would likely cause an error.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtId         index of event within event set
 * @param[in]   startIdx      starting index to examine (usually should be 0)
 *
 * \return
 *    - number of events in event set if evtId is not found.
 *    - other postive [0 - numEvts) is the event index within hEvtSet
 *    - <0 - negative error code.
 *
 */
int Bgpm_GetEventIndex(unsigned hEvtSet, unsigned evtId, unsigned startIdx);



/**
 * Return event details for an event id.
 *
 * @param[in]     evtId       supported Event ID
 * @param[in,out] pInfo       preallocated info structure to fill
 *
 * @return < 0 on error
 *
 */
int Bgpm_GetEventIdInfo(unsigned evtId, Bgpm_EventInfo_t *pInfo);



/** Bgpm_GetUnitType()
 * Return the unit type for an existing event set.
 *
 * @param[in]     hEvtSet       handle to event set
 *
 * @return < 0 on error
 * @return BgpmUnitTypes value otherwise
 *
 */
int Bgpm_GetUnitType(unsigned hEvtSet);



/**
 * Return long description for an event id.
 *
 * Note: value is read from file and is slow.
 *
 * @param[in]     evtId      supported Event ID
 * @param[in,out] pBuff      preallocated buffer
 * @param[in,out] pLen       pass in maximum buffer length, replaced wtih actual
 *
 * @return < 0 on error
 *
 */
int Bgpm_GetLongDesc(unsigned evtId, char *pBuff, int *pLen);






// ****************************
// Punit user defined attribute functions
// ****************************

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[in]     value
 */
int Bgpm_SetEventSetUser1(unsigned hEvtSet, uint64_t value);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[out]    pValue
 */
int Bgpm_GetEventSetUser1(unsigned hEvtSet, uint64_t *pValue);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[in]     value
 */
int Bgpm_SetEventSetUser2(unsigned hEvtSet, uint64_t value);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[out]    pValue
 */
int Bgpm_GetEventSetUser2(unsigned hEvtSet, uint64_t *pValue);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[in]     evtIdx        index to event within evt set
 * @param[in]     value
 */
int Bgpm_SetEventUser1(unsigned hEvtSet, unsigned evtIdx, uint64_t value);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[in]     evtIdx        index to event within evt set
 * @param[out]    pValue
 */
int Bgpm_GetEventUser1(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[in]     evtIdx        index to event within evt set
 * @param[in]     value
 */
int Bgpm_SetEventUser2(unsigned hEvtSet, unsigned evtIdx, uint64_t value);

/**
 * @param[in]     hEvtSet       handle to event set
 * @param[in]     evtIdx        index to event within evt set
 * @param[out]    pValue
 */
int Bgpm_GetEventUser2(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue);



// ****************************
// Punit attribute functions - to apply to punit event sets
// ****************************

/**
 * Set counting context (kernel or user or both)
 *
 * @copydoc Bgpm_Context
 *
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   context        context to set (\ref Bgpm_Context)
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_PARM
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetContext(unsigned hEvtSet, Bgpm_Context context);


/** Bgpm_GetContext() Return counting context (kernel or user)
 *
 * @copydoc Bgpm_Context
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[out]  *pContext      assigned context(\ref Bgpm_Context)
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_GetContext(unsigned hEvtSet, Bgpm_Context *pContext);


/**
 * Allow events with mixed context sensitivity while the Context setting is restricted.
 *
 * On the A2 Core punit events are able to be filtered according to the
 * kernel or user state of the program.  L1p or Opcode instruction events
 * always count all instances regardless of the context.
 * Unless the Bgpm_SetContext() is called, L1p or Opcode events require the
 * active context to count in "both" state.
 * But, by default, if you do restrict counts to kernel or user contexts,
 * L1p and Opcode instructions events cannot be assigned.
 *
 * This method will override the default restrictions to allow events from
 * L1p and Opcode to be assigned even when the context is restricted.
 * The advantage of allowing this mix of results is the ability to
 * compare similar A2 and Opcode events.
 * That is, there are some events
 * (like PEVT_XU_PPC_COMMIT and PEVT_INST_ALL) which give essentially duplicate
 * results, though the XU event is A2 context sensitive, but the instruction event
 * is not.  Thus, a single collection can help distinguish percentages of
 * kernel vs user activity.
 *
 * This setting is a core-wide attribute, and is not cleared until all event sets
 * for the core have been deleted.
 *
 * @param[in]   hEvtSet        handle to event set
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_PARM
 *      - <em>list is incomplete</em>
 */
int Bgpm_AllowMixedContext(unsigned hEvtSet);






/**
 * Return the L1p event mode (undef, stream, list, switch)
 *
 * The L1p Mode is set by via the events assigned to a Punit event set.
 * However, the mode config is node-wide and cannot be multiplexed, so
 * the 1st thread wins and attempts to configure configure events
 * from different modes will fail.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[out]  pMode          active L1p Mode
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_PARM
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetL1pMode(unsigned hEvtSet, Bgpml1pModes *pMode);




// ****************************
// Punit event attribute functions - to apply to punit events
// ****************************


/**
 * Set the Qfpu Group filter mask for the PEVT_INST_QFPU_MASK generic event.
 * See \ref bgpm_opcodes for a list of available UPC_P_AXU_OGRP_* group
 * selection values.  You "or" these together to create a desired mask of
 * instruction group events to accumulate in the same counter.
 *
 * Setting this mask will default to count instructions.  You must also call
 * Bgpm_SetQfpuFp() if you wish to count floating point operations instead.
 *
 * There are predefined events for the individual groups, for ALL groups to count
 * ALL QFPU instructions, and two typical floating point collections (FPGRP1 and FPGRP2)
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx        event index into the event set.
 * @param[in]   mask          mask to build from UPC_P_AXU_OGRP_* defines in spi/include/upci/upc_p.h
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_MASK_EVENT
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetQfpuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t mask);


/**
 * Return the Qfpu Group filter mask for the PEVT_INST_QFPU_MASK generic event.
 * See \ref Bgpm_SetQfpuGrpMask for more info.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index into the event set.
 * @param[out]  *pMask         returned mask value
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_MASK_EVENT
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetQfpuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t *pMask);



/**
 * Set the Xu Instruction Group filter mask for the PEVT_INST_XU_MASK generic event.
 * See \ref bgpm_opcodes for a list of available UPC_P_XU_OGRP_* group
 * selection values.  You "or" these together to create a desired mask of
 * instruction group events to accumulate in the same counter.
 *
 * There are predefined events for the individual groups, for ALL groups to count
 * ALL XU instructions
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        event index into the event set.
 * @param[in]   mask          mask to build from UPC_P_XU_OGRP_* defines in spi/include/upci/upc_p.h
 *
 * @return
 *    - 0 on success
 *    - <0 - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_MASK_EVENT
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_SetXuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t mask);


/**
 * Return the XU Group filter mask for the PEVT_INST_XU_MASK generic event.
 * See \ref Bgpm_SetXuGrpMask for more info.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx        event index into the event set.
 * @param[out]  *pMask         returned mask value
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_MASK_EVENT
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetXuGrpMask(unsigned hEvtSet, unsigned evtIdx, uint64_t *pMask);


/**
 * Indicate if you wish a Qfpu instruction event to count instructions or
 * floating point operations. By default, all PEVET_INST_QFPU_* events
 * count instructions by default,
 * except for PEVT_INST_QPFU_FPGRP1 and PEVT_INST_QPFU_FPGRP2
 * which count floating point operations by default.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx        event index into the event set.
 * @param[in]   countFp       true or false = count float point ops
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_QFPU_INST_EVENT
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetQfpuFp(unsigned hEvtSet, unsigned evtIdx, ushort countFp);



/**
 * Return the Qfpu Group filter mask for the PEVT_INST_QFPU_MASK generic event.
 * See \ref Bgpm_SetQfpuGrpMask for more info.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx        event index into the event set.
 * @param[out]  *pCountFp      returned true or false = count float point ops
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_QFPU_INST_EVENT
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetQfpuFp(unsigned hEvtSet, unsigned evtIdx, ushort *pCountFp);



/**
 * Set the Qpfu instruction match filter to be used with the PEVT_INST_QFPU_MATCH event.
 * The PEVT_INST_QPU_MATCH event, along with this filter value and mask allow you
 * to count only quad floating point instructions which match the given major and minor opcodes.
 * See \ref bgpm_opcodes for opcode matching value to use with each instruction.
 *
 * The "scale" parameter let's you set a value with which the each matching instruction
 * event will be scaled, when interested in the float point operations.  You must pass
 * the values 0-3 which will cause event counts to be scaled by 1, 2, 4, or 8.
 *
 * Note: While the match parameters are set on a particular event,
 * only one set of values may be configured per core at a time.
 * Once the value has been set, all event sets on the core must be destroyed
 * before a new value may be set.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index to PEVT_INST_XU_MATCH event
 * @param[in]   match          opcode match bits
 * @param[in]   mask           mask of which opcode bits to match
 * @param[in]   fpScale        scale the # events [0,1,2,3] == [1,2,4,8]
 *
 *
 * Not all instruction variables may be distinguished.  Only the "major" and "minor"
 * operation bits in a the instructions may be matched.
 * The following illustrates how the match and mask bits correspond to instruction bits:
 *
 * \image html OpcodeMask.jpg
 *
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetQfpuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t match, uint16_t mask, ushort fpScale);



/**
 * Return the Qpfu instruction match filter to be used with the PEVT_INST_QFPU_MATCH event.
 * The PEVT_INST_QFPU_MATCH event, along with this filter value and mask allow you
 * to count only instructions which match the given major and minor opcodes.
 * See \ref bgpm_opcodes for opcode matching value to use with each instruction.
 * See Bgpm_SetQpfuMatch() for more information about the parameters.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index to PEVT_INST_XU_MATCH event
 * @param[out]  pMatch         active opcode match bits
 * @param[out]  pMask          active mask of which opcode bits to match
 * @param[out]  pFpScale       active event scaling value [0,1,2,3] == [1,2,4,8]
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetQfpuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t *pMatch, uint16_t *pMask, ushort *pFpScale);



/**
 * Set the instruction match filter to be used with the PEVT_INST_XU_MATCH event.
 * The PEVT_INST_XU_MATCH event, along with this filter value and mask allow you
 * to count only instructions which match the given major and minor opcodes.
 * See \ref bgpm_opcodes for opcode matching value to use with each instruction.
 *
 * Note: While the match parameters are set on a particular event,
 * only one set of values may be configured per core at a time.
 * Once the value has been set, all event sets on the core must be destroyed
 * before a new value may be set.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index to PEVT_INST_XU_MATCH event
 * @param[in]   match          opcode match bits
 * @param[in]   mask           mask of which opcode bits to match
 *
 * Not all instruction variables may be distinguished.  Only the "major" and "minor"
 * operation bits in a the instructions may be matched.
 * The following illustrates how the match and mask bits correspond to instruction bits:
 *
 * \image html OpcodeMask.jpg
 *
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetXuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t match, uint16_t mask);



/**
 * Return the instruction match filter to be used with the PEVT_INST_XU_MATCH event.
 * The PEVT_INST_XU_MATCH event, along with this filter value and mask allow you
 * to count only instructions which match the given major and minor opcodes.
 * See \ref bgpm_opcodes for opcode matching value to use with each instruction.
 * See Bgpm_SetXuMatch() for more information about the parameters.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index to PEVT_INST_XU_MATCH event
 * @param[out]  pMatch         active opcode match bits
 * @param[out]  pMask          active mask of which opcode bits to match
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_EVT_IDX
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetXuMatch(unsigned hEvtSet, unsigned evtIdx, uint16_t *pMatch, uint16_t *pMask);



/**
 * Set the scaling value to use when counting Quad Square Root floating point operations.
 * Normally, the BGQ hardware will automatically scale the number of floating point operation
 * counts according to each instruction.  However, the Quad Square Root operation cannot be
 * directly counted.  At maximum, 8 floating point operations may occur each instruction,
 * which is the default scaling value.
 *
 * The scaling value is only used when SetQpfuFp() has been used on an event to indicate that
 * quad floating point operations should count the number of floating point operations,
 * or when the PEVT_INST_QFPU_FPGRP1, or PEVT_INST_QFPU_FPGRP2 floating point events are
 * selected.
 *
 * The "scale" parameter let's you set a value with which the each matching instruction
 * event will be scaled, when interested in the float point operations.  You must pass
 * the values 0-3 which will cause event counts to be scaled by 1, 2, 4, or 8.
 *
 * Note: only one value may be configured per core at a time.
 * Once the value has been set, all event sets on the core must be destroyed
 * before a new value may be set.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   fpScale        scale the # events [0,1,2,3] == [1,2,4,8]
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetFpSqrScale(unsigned hEvtSet, ushort fpScale);



/**
 * Return the scaling value to use when counting Quad Square Root floating point operations.
 * Normally, the BGQ hardware will automatically scale the number of floating point operation
 * counts according to each instruction.  However, the Quad Square Root operation cannot be
 * directly counted.  At maximum, 8 floating point operations may occur each instruction,
 * which is the default scaling value.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[out]  pFpScale       active event scaling value [0,1,2,3] == [1,2,4,8]
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetFpSqrScale(unsigned hEvtSet, ushort *pFpScale);




/**
 * Set the scaling value to use when counting Quad Division floating point operations.
 * Normally, the BGQ hardware will automatically scale the number of floating point operation
 * counts according to each instruction.  However, the Quad Square Root operation cannot be
 * directly counted.  At maximum, 8 floating point operations may occur each instruction,
 * which is the default scaling value.
 *
 * The scaling value is only used when SetQpfuFp() has been used on an event to indicate that
 * quad floating point operations should count the number of floating point operations,
 * or when the PEVT_INST_QFPU_FPGRP1, or PEVT_INST_QFPU_FPGRP2 floating point events are
 * selected.
 *
 * The "scale" parameter let's you set a value with which the each matching instruction
 * event will be scaled, when interested in the float point operations.  You must pass
 * the values 0-3 which will cause event counts to be scaled by 1, 2, 4, or 8.
 *
 * Note: only one value may be configured per core at a time.
 * Once the value has been set, all event sets on the core must be destroyed
 * before a new value may be set.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   fpScale        scale the # events [0,1,2,3] == [1,2,4,8]
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetFpDivScale(unsigned hEvtSet, ushort fpScale);



/**
 * Return the scaling value to use when counting Quad Division floating point operations.
 * Normally, the BGQ hardware will automatically scale the number of floating point operation
 * counts according to each instruction.  However, the Quad Division operation cannot be
 * directly counted.  At maximum, 8 floating point operations may occur each instruction,
 * which is the default scaling value.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[out]  pFpScale       active event scaling value [0,1,2,3] == [1,2,4,8]
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetFpDivScale(unsigned hEvtSet, ushort *pFpScale);



/**
 * Set an A2 or L1p event to count edges or cycles
 *
 * Detailed knowledge of the events and configuration is needed to make effective use of
 * this method.  It should only be used if necessary as a workaround or for debug.
 * Only a few events have meaningful results if the edge or cycles are counted, and
 * be warned that using this method will override normal settings for an event and
 * change it's meaning.
 *
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index for event
 * @param[in]   setEdge        set to true for event to count edges
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_ATTRIB  - when not valid for this particular event
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetEventEdge(unsigned hEvtSet, unsigned evtIdx, ushort setEdge);



/**
 * Return if counting is current set to edge for an event.
 *
 * Detailed knowledge of the events and configuration is needed to make effective use of
 * this method.  It should only be used if necessary as a workaround or for debug.
 * Only a few events have meaningful results if the edge or cycles are counted, and
 * be warned that using this method will override normal settings for an event and
 * change it's meaning.
 *
 * The functions also returns false (0) in pEdge if the attribute is not applicable to this
 * particular event.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index for event
 * @param[out]  pEdge          ptr to current edge setting
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetEventEdge(unsigned hEvtSet, unsigned evtIdx, ushort *pEdge);



/**
 * Invert the polarity for an A2 core event.  Thus, have the event count on the
 * alternate edge of the event signal.  If the event counts on edges, it shouldn't
 * effect the results, but it should count the opposite state for cycle effects.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index to PEVT_INST_XU_MATCH event
 * @param[in]   invert         set to true to invert the event signal polarity
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_ATTRIB  - when not valid for this particular event
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetEventInvert(unsigned hEvtSet, unsigned evtIdx, ushort invert);



/**
 * Return the inversion state for an A2 core event.
 *
 * The function also returns false (0) if inversion is not supported for the event.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   evtIdx         event index to PEVT_INST_XU_MATCH event
 * @param[out]  pInvert        active event inversion state
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EOK
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetEventInvert(unsigned hEvtSet, unsigned evtIdx, ushort *pInvert);




/**

* The following functions should only be used if necessary. The defined events have the event edge or cycle, and polarity set appropriate to the event.
*
* \li Bgpm_SetEventEdgeCycle() -- Set punit event to count edges or cycles (warning - only use if necessary - may change meaning of event)
* \li Bgpm_GetEventEdgeCycle() -- Return event edge or cycle setting
* \li Bgpm_SetEventPolarity()  -- Set punit event polarity (warning - only use if necessary - may change meaning of event)
* \li Bgpm_GetEventPolarity()  -- Return event polarity setting
*
*/



// ****************************
// L2unit attribute functions
// ****************************

/**
 * Return L2Unit combine attribute
 *
 * \ref bgpm_concept_l2_combine
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[out]  pEnable       Boolean indicating if L2 event combining is enabled
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetL2Combine(unsigned hEvtSet, UpciBool_t *pEnable);



// ****************************
// NWunit attribute functions
// ****************************

/**
 * Set Virtual Channels attribute for PEVT_NW_SENT, PEVT_NW_RECV, and PEVT_NW_RECV_FIFO events
 *
 * \ref nwunit_desc
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[in]   vchan         virtual channel mask, build by 'or'ing \ref UPC_NW_UnitCtr values
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetEventVirtChannels(unsigned hEvtSet, unsigned evtIdx, UPC_NW_Vchannels vchan);



/**
 * Return Virtual Channels attribute for event
 *
 * \ref nwunit_desc
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[out]  pVchan        virtual channel mask, build by 'or'ing \ref UPC_NW_UnitCtr values
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetEventVirtChannels(unsigned hEvtSet, unsigned evtIdx, UPC_NW_Vchannels * pVchan);




// ****************************
// Functions to apply and handle overflow thresholds
// ****************************

/** \typedef Bgpm_OverflowHandler_t
 *
 * @param[in]   hEvtSet        (unsigned) handle to event set
 * @param[in]   address        (uint64_t) Program counter where overflow interrupt occurred
 * @param[in]   ovfVector      (uint64_t) opaque vector indicating which events overflowed (use with \ref Bgpm_GetOverflowEventIndices)
 * @param[in]   pContext       (const ucontext*) interrupt context structure, in case more explicit actions are desired (see sys/ucontext.h)
 *                             Modifying contents is not supported (for example changing the IAR) which would cause unpredictable results.
 *
 * \note
 * There are various overflows which require different internal processing by BGPM.
 * - Punit evtset overflows cause counters to freeze when overflow occurs, and processing
 *   the overflow can proceed at some leisure without corrupting the counts.
 * - However, the counts do thaw for somewhere in the neighborhood of 700 instructions
 *   during the return from the signal handler, so there is some pollution of events.
 */
typedef void (*Bgpm_OverflowHandler_t)(int hEvtSet, uint64_t address, uint64_t ovfVector, const ucontext_t* pContext);


/**
 * Set overflow on event threshold for an event. This will set the event
 * counter for the current software thread to interrupt on overflow.
 * Bgpm_SetOverflowHandler() must be used to assign a handler.
 *
 * The event period is the number of events between overflows.
 *
 * \note
 * - BGPM supports multiple instances of the same event in each event set
 *   (perhaps with different attributes)
 *   so the event to modify is specified by event set and event index into
 *   the event set.
 * - Overflow events will not be multiplexed.
 * - Bgpm tracks the accumulated value of a overflowing event in software.
 *   The total of the accumulated value and the current hardware counter
 *   value is returned by the Bgpm_ReadEvent() and Bgpm_ReadEventList()
 *   standard access methods.
 * - A fast access method Bgpm_Punit_Read() will return the current
 *   hardware counter value which is a threshold value (a value counting upwards from
 *   64bits - the period value).
 *
 * \warning
 * - The act of processing an overflow does cause some counter noise.
 * - The noise will be variable depending on the types of events and or
 *   overflow type (fast or slow).  Fast overflows freeze the punit counters
 *   until they can be processed, but the return from signal handler imposes
 *   some event noise.
 * - Bgpm does not attempt to compensate for small period values.
 *   If you set a period value too small, than you may hang by spinning or
 *   thrashing by looping between interrupts and the signal handler.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[in]   period        event period between overflows
 *
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_EUNSUP_OVERFLOW
 *      - \ref BGPM_EINV_OVF
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_SetOverflow(unsigned hEvtSet, unsigned evtIdx, uint64_t period);

//! \deprecated
//! No longer does a conversion of period to threshold.  Bgpm_SetOverflow just receives the
//! period value instead.
#define BGPM_PERIOD2THRES(_period_) (_period_)



/**
 * Set overflow handler for an event set.
 * If an event has Overflow set, then the handler
 * will be scheduled to be called as a child of an BGPM signal handler.
 *
 * \note:
 * - BGPM uses the sigrtmax-3 signal, but so it should not be configured by
 *   any other process.
 * - Only one overflow handler is available to each event set, and
 *   the last one registered wins.
 * - BGPM supports multiple instances of the same event in each event set
 *   (perhaps with different attributes)
 *   so the event to modify is specified by event set and event index into
 *   the event set.
 * - It's not considered an error to fail to register an overflow handler while there are
 *   events which have overflow active.  The interrupts and signal handler will still occur
 *   but without a handler there is nothing to record the results.  The event count
 *   will still be incremented appropriately by the Bgpm signal handler and thresholds reset.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   handler       Overflow handler function to call threshold value = event period between overflows.
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_OVERFLOW
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_SetOverflowHandler(unsigned hEvtSet, Bgpm_OverflowHandler_t handler);


/**
 * Get overflow settings on an event: the threshold and active handler for the event set.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[in]   *pPeriod      event period between overflows.
 * @param[in]   *pHandler     Overflow handler function to call each event period between overflows.
 *
 * Not: will also return a Null handler value and 0 period value if overflows are applicable to the event set type.
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_ENO_OVERFLOW
 *      - <em>list is incomplete</em>
*/
int Bgpm_GetOverflow(unsigned hEvtSet, unsigned evtIdx, uint64_t *pPeriod, Bgpm_OverflowHandler_t *pHandler);



/**
 * Within an overflow event handler, convert the passed opaque ovfVector into a list of
 * indicies into the event set to the events which overflowed.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[in]   ovfVector     index of event within event set
 * @param[in,out]  *pIndicies  user allocated array to receive output indicies
 * @param[in,out]  *pLen          Input array length / output number of filled indicies
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EARRAY_TOO_SMALL
 *             pLen is changed to the minimum required number of entires when too small.
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetOverflowEventIndices(unsigned hEvtSet, uint64_t ovfVector, unsigned *pIndicies, unsigned *pLen);
#define BGPM_MAX_OVERFLOW_EVENTS UPC_P_NUM_COUNTERS



// ****************************
// Functions to handle multiplexing features
// ****************************

/**
 * Enable multiplexing on a Punit event set.
 *
 * See \ref bgpm_swfeatures_multiplex
 *
 * Multiplexing should be enabled prior to adding events to a Punit event set.
 * With Multiplexing active on Punit event set, we can have as many Punit events as we like, regardless of Max Events.
 * Muxing will create as many groups as needed to hold all the events, but by default only assigns up to 5 events per group.
 * (A hidden 6th event is used to count the number of cycles the group has been active).
 * 
 * SetMultiplex2 takes an additional argument "maxEventsPerGroup" which allows you to increase the max number of events per group to 11 or 23, 
 * but at the cost of limiting the number of threads per core which can be doing counting.
 * 
 * The multiplexing period is a node-wide setting, and the 1st config
 * wins.  Subsequent Bgpm_SetMultiplex with conflicting period values will fail.
 *
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   period        number of cycles between multiplex switches.
 *                            A value of 0 disables multiplexing for this thread,
 *                            which is not considered a period conflict.
 * @param[in]   normalize     Pass in BGPM_NORMAL or BGPM_NOTNORMAL to
 *                            choose whether event counts reported by Bgpm_ReadEvent() or Bgpm_ReadEventList()
 *                            be normalized to the maximum time spent in a mux group.
 *                            With BGPM_NOTNORMAL, you can use Bgpm_GetMultiplex() data
 *                            and Bgpm_GetMuxEventElapsedCycles() to do your own normalization if needed.
 *
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_PARM
 *      - <em>list is incomplete</em>
 */
int Bgpm_SetMultiplex(unsigned hEvtSet, uint64_t period, int normalize);
int Bgpm_SetMultiplex2(unsigned hEvtSet, uint64_t period, int normalize,RsvMuxCtrs maxEventsPerGroup);
#define BGPM_NORMAL 1
#define BGPM_NOTNORMAL 0



/**
 * Return Punit event set multiplex settings and statistics
 *
 * See \ref bgpm_swfeatures_multiplex
 *
 * Multiplexing should be enabled prior to adding events to a Punit event set.
 *
 * @param[in]   hEvtSet        handle to event set
 * @param[out]  pMuxStats      fill in struct with mux settings and statistics
 *
 * @return
 *    - 0 success
 *    - <0 - negative error code: Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EUNSUP_ATTRIB
 *      - \ref BGPM_EINV_PARM
 *      - <em>list is incomplete</em>
 */
int Bgpm_GetMultiplex(unsigned hEvtSet, Bgpm_MuxStats_t* pMuxStats);


/**
 * Switch a multiplex enabled event set to next Mux group of events.
 * Does nothing if event set is not multiplexed.
 *
 * @param[in]   hEvtSet       handle to event set
 *
 * @return
 *    - 0 on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_ENOT_MUXED
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_SwitchMux(unsigned hEvtSet);


/**
 * Get number of mux groups in an event set.
 * Returns 1 if multiplexing is not enabled.
 *
 * @param[in]   hEvtSet       handle to event set
 *
 * @return
 *    - num of mux sets on success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_ENOT_MUXED
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_GetMuxGroups(unsigned hEvtSet);



/**
 * Get elapsed CPU cycles particular multiple group has been counting.
 * Value is accumulated via the time base counter.
 * Note that 0 is returned if the muxGrpNum doesn't exist.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   muxGrpNum     group number [0-n]
 *
 * @return
 *    - active cycles.
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_ENOT_MUXED
 *      - \ref BGPM_EINV_PARM  (typically if pass muxGrpNum is larger than # groups in set)
 *      - <em>list is incomplete</em>
 *
 */
uint64_t Bgpm_GetMuxElapsedCycles(unsigned hEvtSet, unsigned muxGrpNum);


/**
 * Get elapsed CPU cycles a particular event in a mux set has been counting.
 * Value is accumulated via the time base counter.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx
 *
 * @return
 *    - active cycles.
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVT_IDX
 *      - \ref BGPM_ENOT_MUXED
 *      - <em>list is incomplete</em>
 *
 */
uint64_t Bgpm_GetMuxEventElapsedCycles(unsigned hEvtSet, unsigned evtIdx);





// ****************************
// Standard Counter Control Functions
// ****************************

/**
 *
 * Apply the event set config to hardware in preparation for counting.
 *
 * Note that the apply implictly resets the hardware counter values, so be careful on shared counters (like L2).
 * That is, multiple applies by different threads or event processes will reset the node wide counter value each time.
 *
 * Punits event sets
 * - this method only applies the eventset for the current thread.
 * - counting does take into account sw thread swapping
 *   (That is, punit counters are stopped while swapped out).
 * - Will not be able to apply if there exists an event set which has Bgpm_Attach()ed to
 *   to the current hwthread.
 *
 * @param[in]   hEvtSet        handle to event set
 *
 * @return
 *    - 0 = success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_ENO_CONFIG
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Apply(unsigned hEvtSet);




/**
 *
 * Bgpm_Attach() gives this event set exclusive ownership of the target resource
 * (hardware thread counters, or network link).
 *
 * With attach - only one sw thread owns the target resource for counting, though
 * other hw threads may use the read operations to get counts.
 *
 * - The Bgpm_ReadEvent() operations give the aggregate value of all attached thread counts.
 *   To retrieve the individual thread counters or a subset of the attached threads,
 *   use Bgpm_ReadThreadEvent().
 *   To gather a subset of attached link counts use Bgpm_ReadLinkEvent().
 *
 * - For punit event sets, the mask arguments indicate which hardware threads to
 *   attach and count.  The targMask bits give the application threads [0-63], and
 *   the agentMask indicate to attach to agent threads [64-67].
 *   You may build up the appropriate masks using
 *   UPC_APP_HWTID_MASK(hwThdId) and UPC_AGENT_HWTID_MASK(hwThdId) macros.  Each macro returns
 *   0 when the passed thdId doesn't apply.
 *   For example, the following code would attach the event set indicated by hEvtSet handle
 *   to all hardware threads:
\verbatim

    uint64_t appMask = 0;  uint64_t agentMask = 0;
    for (int hwThd=0; hwThd<68; hwThd++) {
        appMask   |= UPC_APP_HWTID_MASK(hwThd);
        agentMask |= UPC_AGENT_HWTID_MASK(hwThd);
    }
    Bgpm_Attach(hEvtSet, appMask, agentMask);

\endverbatim
 *
 * - The NW unit links may be attached in any mode.
 *   A network a target link mask must be built up by or-ing together values from
 *   \ref UPC_NW_LinkMasks .
 *
 * @param[in]   hEvtSet       handle to input event set
 * @param[in]   targMask      mask of application hw threads (threads 0-63)
 *                            -or-
 *                            nw link mask \ref UPC_NW_LinkMasks
 * @param[in]   agentMask     left aligned mask for agent threads (bits 0-3 == hwthds 64-67)
 *
 * @return
 *    - 0 = success
 *    - \ref BGPM_WALREADY_ATTACHED (positive value)
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_ECTRS_IN_USE
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_ETHD_COUNTS_RSV
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Attach(unsigned hEvtSet, uint64_t targMask, uint64_t agentMask);




/**
 *
 * Detach an attached event set from any reserved resources.  See Bgpm_Attach()
 *
 * @param[in]   hEvtSet        handle to event set
 *
 * @return
 *    - 0 = success
 *    - \ref BGPM_WNOT_ATTACHED (positive value)
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Detach(unsigned hEvtSet);



/**
 *
 * Return positive value if current thread is the
 * "controller" of the passed event set.
 *
 * That is, operations which modify an event set or
 * it's state are only able to successfully used by the
 * controlling thread (the same thread which created the
 * event set).  Other threads can do read operations on the
 * same event set.
 *
 * Across processes, for NW event sets, equivalent events sets
 * can be created, but only the 1st event set attaching to a link
 * will be able to control counting for the link.
 *
 * Thus, if necessary, the Bgpm_IsController() operation will allow you
 * to check whether the current thread is the controller, and so avoid
 * subsequent failures.
 *
 * @param[in]   hEvtSet        handle to event set
 *
 * @return
 *    - >0 : controlling thread
 *    - =0 : not the controlling thread.
 *    - <0 : \ref BGPM_EINV_SET_HANDLE
 *
 */
int Bgpm_IsController(unsigned hEvtSet);



/**
 *
 * Start counting for configured event set events for this thread.
 *
 * In SW Perspective Modes (\ref bgpm_perspective) only the events for
 * the current software thread are started.
 *
 * In HW Perspective Modes, all counters configured by this thread
 * are started.  The Punit, L2 and I/O counter start is synchronized by
 * hardware, but there is at least an 800 cycle delay before the start function
 * returns. Other units (network, etc) start also started, though with
 * some software delay.
 *
 * Note: for shared counters in SW perspective modes (L2, I/O, etc),
 * any thread can start or
 * stop counting. Thus, the 1st start or stop by any thread starts or stops counting,
 * Nested starts or late stops will return a positive warning status.
 * If a Punit event set contains a mix of shared and threaded counters, the
 * positive status only reflects the shared counters status.  Threaded counters are
 * always started or stopped appropriate to the thread.
 *
 * @param[in]   hEvtSet        handle to event set
 *
 * @return
 *    - 0 = success
 *    - \ref BGPM_WALREADY_STARTED (positive value)
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Start(unsigned hEvtSet);



/** Bgpm_ResetStart()
 *
 * Reset then start Start counting all configured
 * events for this thread.   If counters are shared (L2, I/O etc), they are only reset
 * if not actively counting (there is no direct indication that the reset didn't happen)
 *
 * @copydoc Bgpm_Start()
 *
 */
int Bgpm_ResetStart(unsigned hEvtSet);



/**
 * Reset counters for this event set.
 * Shared counters are only reset if currently stopped.
 *
 * @param[in]   hEvtSet        handle to event set
 *
 * @return
 *    - 0 = success
 *    - \ref BGPM_WALREADY_STARTED (positive value)
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Reset(unsigned hEvtSet);



/**
 * Stop counting of all configured events for this thread
 *
 * In SW Perspective Modes (\ref bgpm_perspective) only the events for
 * the current software thread are stopped.
 *
 * In Hw Perspective Modes, all counters configured by this thread
 * are stopped: Punit, L2 and I/O counter stop is synchronized by
 * hardware. There is at least a 1600 cycles delay before this function
 * returns. Other units (network, etc) start also stopped, though with
 * some software delay.
 *
 * Note: for shared counters in SW perspective modes (L2, I/O, etc),
 * any thread can start or
 * stop counting. Thus, the 1st start or stop by any thread starts or stops counting,
 * Nested starts or late stops will return a positive warning status.
 * If a Punit event set contains a mix of shared and threaded counters, the
 * positive status only reflects the shared counters status.  Threaded counters are
 * always started or stopped appropriate to the thread.
 *
 * @return
 *    - 0 = success
 *    - \ref BGPM_WALREADY_STOPPED (positive value)
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Stop(unsigned hEvtSet);



/**
 * Read counter for given event, and current thread.
 *
 * Counters do NOT need to be stopped before read
 * (though there is some small pollution of some event values caused by the act of reading events)
 *
 * - For network event sets which are attached to multiple links,
 *   Read returns the sum of the counter values, except for the
 *   network cycles event, which returns the maximum number of event cycles among all attached links.
 *   That is, the nw cycles is maintained separately for each link, but
 *   Bgpm assumes all attached links are started/stopped at same timees,
 *   but it's not enforced.  A reader event set could mix links from different
 *   controlling event sets, and they could be started/stopped at different times.
 *   You can use Bgpm_NW_ReadLinkEvent() to get values for a single link.
 *
 * - For punit event sets in BGPM_MODE_HW_DISTRIB, the Bgpm_ReadEvent returns the
 *   sum of values from each attached target hardware thread.  You must use Bgpm_ReadThreadEvent() to get
 *   be able to select an individual threads value.
 *
 * - If Punit multiplexing is active with the normalize argument set to BGPM_NORMAL, then
 *   Bgpm_ReadEvent() (or Bgpm_ReadEventList()) will scale the results to the
 *   maximum time any multiplex group has been active.  With normalize set to BGPM_NOTNORMAL,
 *   Then the raw values are returned. Use Bgpm_GetMuxEventElapsed() nad Bgpm_GetMuxElapsedCycles()
 *   to do you own scaling as appropriate.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[out]  *pValue       ptr to mem to receive counter value
 *
 * @return
 *    - 0 = success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_ReadEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue);



/**
 * Read all counters for a given event set and current thread
 *
 * Counters do NOT need to be stopped before read.
 *
 * @param[in]      hEvtSet        handle to event set
 * @param[in,out]  *pValues      user allocated array to receive output values
 * @param[in,out]  *pLen         Input array length / output number of filled values
 *
 * @return
 *    - 0 = success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EARRAY_TOO_SMALL
 *        pLen is fill with minimum number of entries
 *      - <em>list is incomplete</em>
 *
 *
 */
int Bgpm_ReadEventList(unsigned hEvtSet, uint64_t *pValues, unsigned *pLen);



/**
 * Write event counter to given value.
 *
 * Counters do NOT need to be explicitly stopped before write, but the process does require
 * stopping and restarting counters and so is not speedy.
 * If you wish to reduce these delays when writing multiple counts,
 * explicitly nest multiple write operations within a start and stop.
 *
 * Not all unit type counters may be written, nor can they be safely written if
 * overflow is active on the counter.
 * Bgpm will not prevent the write, but results may be unexpected.
 * Also, there is no coordination between sw threads, so last thread wins with
 * shared counters (node-wide).
 *
 *
 * @param[in]      hEvtSet       handle to event set
 * @param[in]      evtIdx        index of event within event set
 * @param[in]      value         value to write
 *
 * @return
 *    - 0 = success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_EINV_OP
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_WriteEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t value);



/**
 * Read counters for a subset of attached threads
 *
 * Counters do NOT need to be stopped before read.
 *
 *   You may build up the appropriate masks using
 *   UPC_APP_HWTID_MASK(hwThdId) and UPC_AGENT_HWTID_MASK(hwThdId) macros.  Each macro returns
 *   0 when the passed thdId doesn't apply.
 *   For example, the following code would read for current hw thread
\verbatim
    uint64_t count;
    uint64_t appMask   = UPC_APP_HWTID_MASK(Kernel_ProcessorID());
    uint64_t agentMask = UPC_AGENT_HWTID_MASK(Kernel_ProcessorID());
    Bgpm_ReadThreadEvent(hEvtSet, appMask, agentMask, &count);
\endverbatim
 *
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[in]   thdMask       application thread mask (build subset by oring together UPC_APP_HWTID_MASK(hwThdId)
 * @param[in]   agentMask     agent thread mask for core 17 threads only (when thdid = 64-67) (build mask via UPC_AGENT_HWTID_MASK(hwThdId))
 * @param[out]  *pValue       ptr to mem to receive aggregate counter value
 *
 * @return
 *    - 0 = success
 *    - positive warning codes
 *      - \ref BGPM_WNOT_ATTACHED - there is a link in mask which does not correspond to the attached links.
 *        Only the attached thread values have been accumulated.
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_ReadThreadEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t thdMask, uint64_t agentMask, uint64_t *pValue);



/**
 * Read counter for a subset of attached nw links
 *
 * Counters do NOT need to be stopped before read.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[in]   mask          link mask (build subset of links from mask values -\ref UPC_NW_LinkMasks)
 * @param[out]  *pValue       ptr to mem to receive counter value
 *
 * @return
 *    - 0 = success
 *    - positive warning codes
 *      - \ref BGPM_WNOT_ATTACHED - there is a link in mask which does not correspond to the attached links.
 *        Only the attached links have been accumulated.
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_NW_ReadLinkEvent(unsigned hEvtSet, unsigned evtIdx, UPC_NW_LinkMasks mask, uint64_t *pValue);



/**
 * Read counter from a particular L2 slice
 *
 * Counters do NOT need to be stopped before read.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   slice         L2 Slice # (0-15)
 * @param[in]   idx           index of event within event set
 * @param[out]  *pValue       ptr to mem to receive counter value
 *
 * @return
 *    - 0 = success
  *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_EINV_L2_SLICE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_L2_ReadEvent(unsigned hEvtSet, unsigned slice, unsigned idx, uint64_t *pValue);



/**
 * Write counter to a particular L2 slice
 *
 * Counters do NOT need to be stopped before read.
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   slice         L2 Slice # (0-15)
 * @param[in]   idx           index of event within event set
 * @param[in]   value         value to write
 *
 * @return
 *    - 0 = success
  *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - \ref BGPM_EINV_L2_SLICE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_L2_WriteEvent(unsigned hEvtSet, unsigned slice, unsigned idx, uint64_t value);



/**
 * Sample a specific CNK Event
 *
 * Value returned is the current event value since boot, regardless of the event set running state
 *
 * @param[in]   hEvtSet       handle to event set
 * @param[in]   evtIdx        index of event within event set
 * @param[out]  *pValue       ptr to mem to receive counter value
 *
 * @return
 *    - 0 = success
  *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - \ref BGPM_EINV_EVENT
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_CNK_SampleEvent(unsigned hEvtSet, unsigned evtIdx, uint64_t *pValue);




// ****************************
// Fast Punit Control Functions
// ****************************

/**
 *
 * Get fast control handle structure for the punit event set.
 * Only valid for \ref BGPM_MODE_SWDISTRIB mode.
 *
 * @param[in]      hEvtSet        handle to event set
 * @param[in,out]  pH             ptr to existing handles structure
 *
 * @return
 *    - 0 = success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Punit_GetHandles(unsigned hEvtSet, Bgpm_Punit_Handles_t *pH);




/**
 *
 * Fast start from punit control mask.
 * Only works for current hw thread.
 * Only functions properly for the \ref BGPM_MODE_SWDISTRIB mode only
 *
 * @param[in]      mask        allCtrMask or thdCtrMask from \ref Bgpm_Punit_Handles_t
 *
 */
__INLINE__ void Bgpm_Punit_Start(uint64_t mask)
{
    upc_p_local->control_w1s = mask;
    mbar();
}



/**
 *
 * Fast stop from punit control mask.
 * Only works for current hw thread.
 * Only functions properly for the \ref BGPM_MODE_SWDISTRIB mode only
 *
 * @note counter results are not stable until 800 cycles after stop.
 * Caller is responsible to Upci_Delay(800) as appropriate.
 *
 * @param[in]      mask        allCtrMask or thdCtrMask from \ref Bgpm_Punit_Handles_t
 *
 */
__INLINE__ void Bgpm_Punit_Stop(uint64_t mask)
{
    upc_p_local->control_w1c = mask;
    mbar();
}



/**
 *
 * Fast read from punit counter handle.
 * Only functions properly for the \ref BGPM_MODE_SWDISTRIB mode
 *
 * @param  hCtr   a counter handle from \ref Bgpm_Punit_Handles_t structure
 *
 */
__INLINE__ uint64_t Bgpm_Punit_Read(uint64_t hCtr)
{
    return *(uint64_t *)hCtr;
}



/**
 *
 * Get Low latency control handle structure for the punit event set.
 * Only functions properly for the \ref BGPM_MODE_LLDISTRIB mode only
 *
 * @param[in]      hEvtSet        handle to event set
 * @param[in,out]  pH            ptr to existing handles structure
 *
 * @return
 *    - 0 = success
 *    - negative error codes  Some relevant errors:
 *      - \ref BGPM_EINV_SET_HANDLE
 *      - <em>list is incomplete</em>
 *
 */
int Bgpm_Punit_GetLLHandles(unsigned hEvtSet, Bgpm_Punit_LLHandles_t *pH);



/**
 *
 * Low latency start from punit control mask.
 * Only works for current hw thread.
 * Only functions properly for the \ref BGPM_MODE_LLDISTRIB mode only
 *
 * @param[in]      mask        allCtrMask or thdCtrMask from \ref Bgpm_Punit_LLHandles_t
 *
 */
__INLINE__ void Bgpm_Punit_LLStart(uint64_t mask)
{
    upc_p_local->control_w1s = mask;
}



/**
 *
 * Low latency reset and start from punit control mask.
 * Only works for current hw thread.
 * Only functions properly for the \ref BGPM_MODE_LLDISTRIB mode only
 *
 * @param[in]      mask        allCtrMask or thdCtrMask from \ref Bgpm_Punit_LLHandles_t
 *
 */
__INLINE__ void Bgpm_Punit_LLResetStart(uint64_t mask)
{
    upc_p_local->control_w1s = mask | (mask << 32);
}



/**
 *
 * Low latency stop from punit control mask.
 * Only works for current hw thread.
 * Only functions properly for the \ref BGPM_MODE_LLDISTRIB mode only
 *
 * @param[in]      mask        allCtrMask or thdCtrMask from \ref Bgpm_Punit_LLHandles_t
 *
 */
__INLINE__ void Bgpm_Punit_LLStop(uint64_t mask)
{
    upc_p_local->control_w1c = mask;
}



/**
 *
 * LowLatency read from punit counter handle.
 * Counters are only 14 bits, with an extra (non-consecutive) bit which indicates
 * if the 14 bits have overflowed (use \ref BGPM_LLCOUNT_OVERFLOW to check)
 *
 * @param[in]  hCtr   a counter handle from \ref Bgpm_Punit_LLHandles_t structure
 *
 */
__INLINE__ uint64_t Bgpm_Punit_LLRead(uint64_t hCtr)
{
    return *(uint64_t *)hCtr;
}



/**
 *
 * LowLatency write from punit counter handle.
 * Counters are only 14 bits, with an extra (non-consecutive) bit which indicates
 * if the 14 bits have overflowed (use \ref BGPM_LLCOUNT_OVERFLOW to check)
 *
 * @param[in]  hCtr   a counter handle from \ref Bgpm_Punit_LLHandles_t structure
 * @param[in]  val    value to write (only lower 14 bits)
 *
 */
__INLINE__ void Bgpm_Punit_LLWrite(uint64_t hCtr, uint64_t val)
{
    *(uint64_t *)hCtr = UPC_P__COUNTER__COUNT_set(val);
}



/** BGPM_LLCOUNT_OVERFLOW()
 *
 * Return non-zero if low latency counter value indicate overflow.
 * Counters are only 14 bits, with an extra (non-consecutive) bit which indicates
 *
 * @param  _val_    value to write (only lower 14 bits)
 *
 */
#define BGPM_LLCOUNT_OVERFLOW(_val_) ((_val_ & UPC_P__COUNTER__BIG_OVF_set(1)) != 0)



/**
 *
 * Thread specific setting to have Bgpm print some error message text when an error occurs.
 * Bgpm is set to print by default.
 *
 * @param[in]  val    true to print (default is true)
 * @return     prior value
 *
 * @note Each thread must set this value if you don't wish to print on errors.
 *       It may be set before or after Bgpm_Init().
 *
 */
ushort Bgpm_PrintOnError(ushort val);



/**
 *
 * Thread specific setting to have Bgpm exit process after a error, or return error codes instead
 * Bgpm is set to exit by default.
 *
 * @param[in]  val    true (nonzero) to exit (default is true)
 * @return     prior value
 *
 * @note Each thread must set this value if you don't wish to exit on errors.
 *       It may be set before or after Bgpm_Init().
 *
 */
ushort Bgpm_ExitOnError(ushort val);



/**
 *
 * Thread specific setting to have Bgpm dump the call stack after a error instead of just exiting
 * (Bgpm creates a false assert error at the error location..
 * Bgpm is set to not dump the stack by default.
 *
 * @param[in]  val    true (nonzero) to dump (default is false)
 * @return     prior value
 *
 * @note Each thread must set this value if you wish to dump on errors.
 *       It may be set before or after Bgpm_Init().
 *
 */
ushort Bgpm_DumpOnError(ushort val);




/**
 * Always return True to indicate that Bgpm library has been linked.
 * To make this work, the using code must
 * have a local _attribute__(weak) version which returns false.
 * The linker will then replace the weak version with the strong version
 * from the library to indicate it has been linked.
 *
 */
int Bgpm_LibLinked();



/**
 * Debug Functions....
 */
#define BGPM_FEEDBACK_BUFSIZE 4100   //!< predicted maximum size needed for reservation feedback buffer (except when multiplexing active - which might be times number of 5 event groups)

int Bgpm_SetThreadFeedbackBuffer(unsigned hEvtSet, char *buffer, int bufSize); //!< pass a buffer which may be filled with reservations feedback text when adding events fail (not always)
int Bgpm_PrintFeedbackBuffer(const char *buffer, int bufSize);                 //!< print contents of feedback buffer in a way which prevents the bgq control system from splitting lines)
int Bgpm_PrintCurrentPunitReservations(unsigned hEvtSet);                      //!< print the current event/counter reservations associated with the punit event set.



/**
 * Debug function to print contents of structures.
 */
int   Bgpm_DumpEvtSet(unsigned hEvtSet, unsigned indentLvl);
int   Bgpm_DumpEvtSetList(unsigned indentLvl);
void  Bgpm_DebugDumpShrMem();
const BgpmDebugThreadVars_t *Bgpm_DebugGetThreadVars();
void  BgpmDebugPrintThreadVars();
void  Bgpm_DumpUPCPState();



/**
 * Debug functions to enable or disable trace printing (only when bgpm libary compiled with trace active - see Debug.h
 */
void Bgpm_EnableTracePrint();
void Bgpm_DisableTracePrint();




__END_DECLS


#endif
