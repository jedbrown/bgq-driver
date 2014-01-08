/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

//! \file  mudm_ras.h 
//! 


#ifndef	_MUDM_RAS_H_  /* Prevent multiple inclusion */
#define	_MUDM_RAS_H_

int injectRAWRAS(uint32_t message_id, size_t raslength, const uint64_t* rasdata);

/* Max payload 2K right now due to the size of the RAS message in the DB.
 * It would be possible to do a 15K max payload (16K mailbox - ~1K header)
 */
#define MUDM_RAS_MAX_PAYLOAD 2048
#define MUDM_RASBEGIN(numexpected) { uint64_t rasstorage=numexpected; uint64_t rasindex=0; uint64_t rasarray[numexpected];
#define MUDM_RASBYTES(bytes,ptr,message_id) injectRAWRAS(message_id, bytes, (uint64_t *)ptr)
#define MUDM_RASPUSH(value) if(rasstorage > rasindex) { rasarray[rasindex++] = value; }
#define MUDM_RASFINAL(message_id) injectRAWRAS(message_id, rasindex*sizeof(uint64_t), rasarray); }
#define MUDM_RAS_ERRNO(message_id,errval) {uint64_t error=errval; injectRAWRAS(message_id, sizeof(uint64_t), &errval);}
#define MUDM_RAS_INITERR(errval) MUDM_RASBEGIN(2) MUDM_RASPUSH(errval) MUDM_RASPUSH(__LINE__) MUDM_RASFINAL(MUDMRAS_INIT_FAILURE)
//#define MUDM_RAS_INITERR(errval)


typedef enum { MUDMRAS_NONE = 0x000C0000,
/*************************** */
       
       MUDMRAS_INIT_FAILURE = 0x000C0001,
       /*
         <rasevent 
         id="000C0001"
         category="Software_Error"
         component="MUDM"
         severity="FATAL"
         message="MUDM encountered a fatal error $(ERROR)"
         description="The MUDM component code failed to initialize at line $(LINE)."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */

       MUDMRAS_SOFTWARE_WARN = 0x000C0002,
       /*
         <rasevent 
         id="000C0002"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="MUDM encountered an error $(ERROR)"
         description="The kernel has enountered a conditon that was not expected"
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */

       MUDMRAS_STUCK_PKT = 0x000C0039,
       /*
         <rasevent 
         id="000C0039"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="A system packet appears to be stuck on the torus MU transmission queue.  Packet descriptor $(PKTD), timestamp $(TIMESTAMP) packet descriptor queued timestamp $(PKTDTIMESTAMP) packet descriptor count $(PKTDCOUNT) current reference count $(COUNT)."
         description="The kernel is reporting that a packet has not been transmitted for a long time."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */

       MUDMRAS_STUCK_INJ_DESCRIPTOR = 0x000C0040,
       /*
         <rasevent 
         id="000C0040"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="A descriptor appears to be stuck on a system torus MU injection FIFO.  The head pointer is currently injecting $(HEX1) $(HEX2) $(HEX3) $(HEX4) $(HEX5) $(HEX6) $(HEX7) $(HEX8)."
         description="The kernel is reporting that an injection descriptor has not been injected for a long time."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */


       MUDMRAS_STUCK_INJ_RESETCOMPUTES = 0x000C0042,
       /*
         <rasevent 
         id="000C0042"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="A descriptor appears to be stuck on the torus MU injection FIFO.  The FIFO pointers are start $(START), end $(END), head $(HEAD), and tail $(TAIL).  There are $(%d,UNINJECTED) injections pending.  The stuck packet has target torus location $(%d,A) $(%d,B) $(%d,C) $(%d,D) $(%d,E)"
         description="The kernel is reporting that an injection descriptor has not been injected for a long time."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         control_action="END_JOB,FREE_COMPUTE_BLOCK"
         />
       */


       MUDMRAS_STUCK_INJ_AT_LINKRESET = 0x000C0043,
       /*
         <rasevent 
         id="000C0043"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="A descriptor appears to be stuck on the torus MU injection FIFO.  The FIFO pointers are start $(START), end $(END), head $(HEAD), and tail $(TAIL).  There are $(%d,UNINJECTED) injections pending."
         description="The kernel is reporting that an injection descriptor has not been injected for a long time."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */


       MUDMRAS_PKT_RCV_ERR = 0x000CD000,
       /*
         <rasevent 
         id="000CD000"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="The callback on the receiving of a packet had error $(HEX0). The packet header is $(HEX1) $(HEX2) $(HEX3) $(HEX4)."
         description="An invalid packet was received and dropped."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */

      
       MUDMRAS_PKT_RCV_ERR_STOP = 0x000CD001,
       /*
         <rasevent 
         id="000CD001"
         category="Software_Error"
         component="MUDM"
         severity="FATAL"
         message="The callback on the receiving of a packet had error $(HEX0). The packet header is $(HEX1) $(HEX2) $(HEX3) $(HEX4)."
         description="The error disrupted effective communications."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         control_action="END_JOB,FREE_COMPUTE_BLOCK"
         />
       */

        MUDMRAS_PKT_REJECTED = 0x000CD002,
       /*
         <rasevent 
         id="000CD002"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="A remote node rejected a transmitted packet.  The debug code is $(HEX0).  The debug data is $(HEX1) $(HEX2) $(HEX3) $(HEX4)."
         description="An invalid packet was rejected by a remote node."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */

       MUDMRAS_CN_TOOK2LONG = 0x000CD003,
       /*
         <rasevent 
         id="000CD003"
         category="Software_Error"
         component="MUDM"
         severity="WARN"
         message="A connection is taking a long time to complete.   The packet header is $(HEX0) $(HEX1) $(HEX2) $(HEX3)."
         description="A connection is taking a long time to complete processing.."
         service_action="Please report these errors to IBM.  Logs, timestamps, and any other recreation material will be helpful."
         />
       */

       MUDMRAS_ID_MAX= 0xcffff    // always last  
} mudmras_msg_id;

#endif //	_MUDM_RAS_H_  /* Prevent multiple inclusion */
