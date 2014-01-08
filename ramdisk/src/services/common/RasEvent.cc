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

// Includes
#include <ramdisk/include/services/common/RasEvent.h>
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <fcntl.h>
#include <errno.h>

LOG_DECLARE_FILE("cios.common");

using namespace bgcios;

RasEvent::RasEvent(uint32_t msgId,modeType mt)
{
   // Initialize private data.
   _rasFd = -1;
   memset(_eventInfo.msg, 0, sizeof(_eventInfo.msg));
   _eventData = (uint64_t *)_eventInfo.msg;

   // Setup the event structure.
   _eventInfo.msgId = msgId;
   _eventInfo.isBinary = mt;
   _eventInfo.len = 0;
}

RasEvent::~RasEvent()
{
   // Close the RAS event special file if it was opened.
   if (_rasFd != -1) {
      ::close(_rasFd);
   }
}

void
RasEvent::send(void)
{
   // Open the RAS event special file if it is not open.
   if (_rasFd < 0) {
      // Open the RAS event file.
      _rasFd = ::open(BG_RAS_FILE, O_WRONLY);
      if (_rasFd == -1) {
         // There is not much we can do to report a failure opening the RAS event special file.
         LOG_FATAL_MSG("error opening RAS event special file '" << BG_RAS_FILE << "': " << bgcios::errorString(errno));
         return;
      }
   }

   // Write the event structure to the RAS event special file.
   ssize_t rc = ::write(_rasFd, &_eventInfo, sizeof(_eventInfo));
   if (rc != sizeof(_eventInfo)) {
      LOG_FATAL_MSG("error writing RAS event to fd " << _rasFd << " rc " << rc << " != " << sizeof(_eventInfo) << ", errno=" << errno);
   }
   _eventInfo.len = 0;

#if 0
   LOG_ERROR_MSG("ras event " << _eventInfo.msgId << " has " << _dataCount << " data elements");
   for (int index = 0; index < _dataCount; ++index) {
      LOG_ERROR_MSG("element " << index << ": " << _eventData[index]);
   }
#endif

   return;
}
