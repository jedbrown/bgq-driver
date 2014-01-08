/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp. 2012 All Rights Reserved                 */
/* US Government Users Restricted Rights - Use,                     */
/* duplication or disclosure restricted by GSA ADP                  */
/* Schedule contract with IBM Corp.                                 */
/*                                                                  */
/* This software is available to you under the                      */
/* GNU General Public License (GPL).                                */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

/* IBM Blue Gene/Q RAS generation application
 *
 * Author: Jay S. Bryant <jsbryant@us.ibm.com>
 *
*/

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <asm/bluegene_ras.h>




void usage(char* msg, char* argv0, int rc)
{
        fprintf(stderr, "%s\nError code is %d\nusage: %s <messageId> \"<message>\" -- The RAS message may also be provided via stdin.\n",
                msg, rc, argv0);

        exit(rc);
}




// This program will generate a RAS event from either a string or the contents of a file.
int main(int argc,
	 char** argv)
{
	int rc, devFile;
	bg_ras ras;
	uint64_t localMsgId;
	char error[128];
	FILE *input;

	if (argc < 2 || argc > 3)
		usage("Invalid number of arguments", argv[0], EINVAL);
	else {
		localMsgId = strtoull(argv[1], NULL, 0);

		//Make sure the messageId is in the the range of acceptable values.
		if((localMsgId < 0) || (localMsgId > 0xFFFFFFFF))
			usage("messageId not in the range 0x00000000-0xFFFFFFFF", argv[0], EINVAL);
		else
		        ras.msgId = (uint32_t) localMsgId;

		ras.isBinary = ras.len = 0;

		if(argv[2]) {

		        //Check the size of the string that has been sent and error if it is
		        //too large.
		        if(strlen(argv[2]) >= sizeof(ras.msg)-1) {
			        sprintf(error, "message exceeds the maximum of %d characters", 
					(sizeof(ras.msg)-1));
				usage(error, argv[0], EINVAL);
			}
			       
			//Otherwise we set up the RAS message.
			strncpy(ras.msg, argv[2], sizeof(ras.msg));
			ras.len = strlen(ras.msg); 
		} else {

			input = fopen("/dev/stdin", "r");
			if (input) {
				char* buf = ras.msg;

				//Can only read BG_RAS_MAX_PAYLOAD-1 characters to leave room for
				//null termination which is handled in bgq_fw.c
				while (ras.len < sizeof(ras.msg)-1) {
					rc = fread(buf, 1, sizeof(ras.msg) - ras.len - 1, input);
					if (rc > 0) {
						ras.len += rc; 
						buf += rc;
					} else 
						break;
				}

				//Make sure we got all the data.  If not, we need to throw an error.
				if (!feof(input)) {
			        	sprintf(error, "message exceeds the maximum of %d characters", 
						sizeof(ras.msg) - 1);	
					usage(error, argv[0], EINVAL);
				}
			
				fclose(input);
			} else //Couldn't open stdin
			       usage("Failure reading from stdin", argv[0], 0);
		} //End if argv[2]/else stdin

		//If we have a message, write the RAS event.
		if ( ras.len > 0 ) {

	               devFile = open(BG_RAS_FILE, O_WRONLY);				

		       if (devFile > 0) {
 
			      rc = write(devFile, (void*) &ras, offsetof(bg_ras, msg) + ras.len);
			      if (rc <= 0)
				     fprintf(stderr, "\nERROR: Failure writing RAS data - errno %d.\n", errno);

			      close(devFile);

		       } else { //Couldn't open devFile
			      fprintf(stderr, "\nERROR: Failure opening %s - errno %d.\n", BG_RAS_FILE, errno);
			      exit(errno);
		       }
		} else {	

		       usage("No RAS message provided!", argv[0], 1);

		}//End if ras.len

		return 0;

	} //End if appropriate number of arguments

} //End Main
