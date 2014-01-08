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
/*
 * IBM Blue Gene/Q Linux hostid setting application
 *
 * Author: Jay S. Bryant <jsbryant@us.ibm.com>
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void usage(char* argv0) {

	fprintf(stderr, "\nERROR: Incorrect usage!  Usage: %s <new hex hostid>\n", argv0);
	exit(1);
}

int main(int argc, char** argv)
{
	int result;
	unsigned long newId;

	if(argc != 2) { usage(argv[0]); }

	newId = strtoul(argv[1], NULL, 0);	

	result = sethostid(newId);

	if (result == 0 )
	{ 
		printf("bgsethostid: successfully set hostid to 0x%x\n", newId);
		exit(0);
	} else {
		printf("ERROR : Unable to set hostid - rc %d\n", result);
		perror("ERROR ");
		exit(result);
	}
}
