/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp. 2012  All Rights Reserved                 */
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

/* IBM Blue Gene/Q Halt Program
 *
 *	This program has been written to handle the case where we are
 *	unable to boot into the NFS root space.  Upstart's init process
 *	isn't able to shutdown cleanly from that environment due to the
 *	fact that we are very early in the boot.  This program makes
 *	it possible to nicely completely a shutdown by just calling
 *	the reboot system call directly.
 *
 *
 * Author: Jay S. Bryant <jsbryant@us.ibm.com>
 *
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/reboot.h>

int main(int argc, char *argv[])
{
	printf( "\n\nRequesting BlueGene node halt ...\n\n");

	return reboot(RB_HALT_SYSTEM);
}

