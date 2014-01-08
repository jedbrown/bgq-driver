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

//! \file  ClientMessage.cc
//! \brief Methods for bgcios::sysio::ClientMessage class.

// Includes
#include "ClientMessage.h"
#include <ramdisk/include/services/ServicesConstants.h>
#include <ramdisk/include/services/common/logging.h>
#include <stdlib.h>

using namespace bgcios::sysio;

LOG_DECLARE_FILE("cios.sysiod");

ClientMessage::~ClientMessage()
{
   if (_storage != NULL) {
      free(_storage);
   }
}

void *
ClientMessage::save(void)
{
   // Allocate storage for the message.
   int err = ::posix_memalign((void **)&_storage, 32, _message->length);
   if (err != 0) {
      LOG_ERROR_MSG("error allocating storage for client message: " << bgcios::errorString(err));
      return NULL;
   }

   // Save the message.
   ::memcpy(_storage, _message, _message->length);
   _message = (bgcios::MessageHeader *)_storage;

   return _storage;
}
