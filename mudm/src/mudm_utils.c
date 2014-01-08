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



#include <hwi/include/common/compiler_support.h>
#include <mudm/include/mudm.h>
#include <mudm/include/mudm_utils.h>
#include "mudm_macro.h"




void mudm_dumphex(unsigned char *dmpPtr, int len) {

   unsigned char temp[]="                                                                                  ";
   int i=0;
   while (len >= 16) {    // do 16 byte chunks
      MPRINT("%.16llx    %02x%02x%02x%02x %02x%02x%02x%02x  %02x%02x%02x%02x %02x%02x%02x%02x \n",
             (long long unsigned int) dmpPtr,dmpPtr[0],dmpPtr[1],dmpPtr[2],dmpPtr[3],dmpPtr[4],dmpPtr[5],dmpPtr[6],dmpPtr[7],
                   dmpPtr[8],dmpPtr[9],dmpPtr[10],dmpPtr[11],dmpPtr[12],dmpPtr[13],dmpPtr[14],dmpPtr[15]);
      dmpPtr +=16;
      len -= 16;
   }
   for (i=0;i<len;i++)temp[i]=dmpPtr[i];
   if (0 != len) {     // more to do
      MPRINT("%.16llx    %02x%02x%02x%02x %02x%02x%02x%02x  %02x%02x%02x%02x %02x%02x%02x%02x \n",
             (long long unsigned int) dmpPtr,temp[0],temp[1],temp[2],temp[3],temp[4],temp[5],temp[6],temp[7],
                   temp[8],temp[9],temp[10],temp[11],temp[12],temp[13],temp[14],temp[15]);
   }
}   //dumphex


//int dumphex(unsigned char *dmpPtr, int len){return 0;}
