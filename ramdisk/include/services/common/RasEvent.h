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

//! \file  RasEvent.h 
//! \brief Declaration and inline methods for bgcios::RasEvent class.

#ifndef COMMON_RASEVENT_H
#define COMMON_RASEVENT_H

// Includes
#include <inttypes.h>
#include <linux/include/asm/bluegene_ras.h>

namespace bgcios
{

//! Generate a RAS event.

class RasEvent
{
public:
  enum modeType { charMode=0,binMode=1};

   //! \brief  Default constructor.
   //! \param  msgId Message id for RAS event.

   RasEvent(uint32_t msgId, modeType mt=binMode);

   //! \brief  Default destructor.

   ~RasEvent();

   //! \brief  Add a data element to the RAS event.
   //! \param  data New data element.
   //! \return Nothing.

   void addData(int data)
   {
      if (_eventInfo.len < MaxDetailWords) {
         _eventData[_eventInfo.len++] = (uint64_t)data;
      }
      return;
   }

   //! \brief  Add a data element to the RAS event.
   //! \param  data New data element.
   //! \return Reference to this object.

   RasEvent& operator<<(const int data)
   { 
      if (_eventInfo.len < MaxDetailWords) {
         _eventData[_eventInfo.len++] = (uint64_t)data;
      }
      return *this;
   }

   //! \brief  Add a data element to the RAS event.
   //! \param  data New data element.
   //! \return Reference to this object.

   RasEvent& operator<<(const uint64_t data)
   { 
      if (_eventInfo.len < MaxDetailWords) {
         _eventData[_eventInfo.len++] = data;
      }
      return *this;
   }

   //! \brief  Send the event to the RAS database.
   //! \return Nothing.

   void send(void);

   //! Maximum number of detail words per RAS event.
   static const uint16_t MaxDetailWords = 64;

char * getRasBuff(){return _eventInfo.msg;}
int getRasBuffSize(){return BG_RAS_MAX_PAYLOAD;}
void setLength(int l){_eventInfo.len=l;}

private:

   //! File descriptor to RAS event special file.
   int _rasFd;

   //! Structure to write to RAS event special file.
   bg_ras _eventInfo;

   //! Pointer to event data buffer.
   uint64_t* _eventData;

};

enum RasID
{
   BaseID = 0x000b0000,
       
   DaemonFailed,
   /*
      <rasevent 
         id="000b0001"
         category="Software_Error"
         component="CIOS"
         severity="WARN"
         message="CIOS daemon in process $(%d,PID) received signal $(%d,SIGNAL)."
         description="A CIOS daemon received a terminating signal and ended.  The daemon was restarted if the maximum number of restarts has not been exceeded."
         service_action="$(CheckLevels) Check the I/O node log file for additional information."
      />
   */
       
   DaemonStartFailed,      
   /*
      <rasevent 
         id="000b0002"
         category="Software_Error"
         component="CIOS"
         severity="FATAL"
         message="iosd failed to start a daemon with process $(%d,PID), errno $(%d,ERRNO)."
         description="A daemon path property in the cios.iosd section of the bg.properties file is invalid or the executable is not available from the I/O node."
         service_action="Check the I/O node log file for additional information.  Correct the property in the bg.properties file.  Verify the executable is available on the I/O node."
         control_action="SOFTWARE_IN_ERROR"
      />
   */

   DaemonInitFailed,      
   /*
      <rasevent 
         id="000b0003"
         category="Software_Error"
         component="CIOS"
         severity="FATAL"
         message="A CIOS daemon failed to initialize and is not ready, errno $(%d,ERRNO)."
         description="An error occurred when a CIOS daemon was initializing.  The daemon is not ready for handling messages."
         service_action="$(CheckLevels) Check the I/O node log file for additional information."
         control_action="SOFTWARE_IN_ERROR"
      />
   */

   DaemonRestarted,
   /*
      <rasevent
         id="000b0004"
         category="Software_Error"
         component="CIOS"
         severity="FATAL"
         message="A CIOS daemon running in process $(%d,OLDPID) was restarted after it failed and is now running in process $(%d,NEWPID) after $(%d,RESTARTS) restart attempts."
         description="A CIOS daemon needed for servicing compute nodes was restarted.  Any compute blocks serviced by this I/O node were freed."
         service_action="$(CheckLevels) Check the I/O node log file for additional information."
         control_action="END_JOB,FREE_COMPUTE_BLOCK"
      />
   */

   DaemonRestartLimit,
   /*
      <rasevent
         id="000b0005"
         category="Software_Error"
         component="CIOS"
         severity="FATAL"
         message="A CIOS daemon running in process $(%d,PID) has reached the restart limit after being restarted $(%d,RESTARTS) times and was not restarted."
         description="The maximum number of restarts for a CIOS daemon was reached.  Based on the setting of the cios.iosd.max_service_restarts property, the daemon was not restarted."
         service_action="$(CheckLevels) Check the I/O node log file for additional information."
         control_action="END_JOB,FREE_COMPUTE_BLOCK"
      />
   */

      SysiodSyscallHangNoSignal,      
   /*
      <rasevent 
         id="000b0006"
         category="Software_Error"
         component="CIOS"
         severity="WARN"
         message="The sysiod process seems to be stuck in a system call while running.  Flight log $(DETAILS)"
         description="The SYSIOD process syscall-watchdog monitor detected a system call issued on behalf of a compute node did not complete in a reasonable time.  This may a file-system or network issue."
         service_action="Check the I/O node log file for additional information.  Check the file system.  Check the network"
      />
   */
         SysiodSyscallHangOnSignal,      
   /*
      <rasevent 
         id="000b0007"
         category="Software_Error"
         component="CIOS"
         severity="WARN"
         message="The sysiod process seems to be stuck in a system call while ending.  Flight log $(DETAILS)"
         description="The SYSIOD process syscall-watchdog monitor detected a system call issued on behalf of a compute node did not complete in a reasonable time.  This may a file-system or network issue."
         service_action="Check the I/O node log file for additional information.  Check the file system.  Check the network"
      />
   */

        StringTest     
   /*
      <rasevent 
         id="000b0008"
         category="Software_Error"
         component="CIOS"
         severity="WARN"
         message="A CIOS daemon is testing RAS."
         description="A CIOS daemon was issuing RAS. StringInfo=$(STRINGINFO)"
         service_action="Check the I/O node log file for additional information."
         control_action="SOFTWARE_IN_ERROR"
      />
   */


};

} // namespace bgcios

#endif // COMMON_RASEVENT_H

