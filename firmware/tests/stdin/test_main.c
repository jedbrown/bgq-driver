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


#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/fwext/fwext_lib.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <firmware/include/mailbox.h>

#if 0
#include <hwi/include/common/bgq_alignment.h>

#include <firmware/include/mailbox.h>



#include <string.h>
#include <stdio.h>
#endif

#include <stdlib.h>

int test_main( void ) {

    if ( ProcessorID() == 0 ) {


	char buffer[1024];
	int  len;


	printf("Enter a string > ");

	if ( ( len = fwext_get(buffer,sizeof(buffer)) ) > 0 ) {
	    printf("You entered %d characters -> %s <\n", len, buffer);
	}
	else {
	    printf("(E) rc=%d from fwext_get()\n", len);
	}

#if 0
	{
	    char buffer[1024];
	    fw_uint32_t messageType = 999;

	    fwext_getFwInterface()->deprecated.backdoorTest(0); // populate the inbox
	    int rc = fwext_getFwInterface()->pollInbox( buffer, &messageType, sizeof(buffer) );

	    MailBoxPayload_SystemRequest_t* sysreq = (MailBoxPayload_SystemRequest_t*)buffer;

	    printf("pollInbox rc=%d msgType=%d sysreq.id=%d sysreq.blockid=%X\n", rc, messageType, sysreq->sysreq_id, sysreq->details.shutdown_io_link.block_id );

	    printf("ENTERING POLLING STATE ...\n");
	    while ( ( rc = fwext_getFwInterface()->pollInbox( buffer, &messageType, sizeof(buffer) ) ) == FW_EAGAIN );

	    printf("pollInbox rc=%d msgType=%d %c%c%c%c\n", rc, messageType, buffer[0], buffer[1], buffer[2], buffer[3] );
    
	}
#endif

    }
  exit(0);
}



