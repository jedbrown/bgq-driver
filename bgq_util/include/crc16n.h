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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef __CRC16N_H__
#define __CRC16N_H__

#if !defined(__ASSEMBLY__) && defined(__cplusplus)
extern "C" {
#endif

//////////////////////////////////////////////////////
//
// This file contains the function to generate a 16 bit
// CRC utilizing a 16 byte table.
//
//
// The CRC polynomial used here is:
//
// x^16 + x^12 + x^5 + 1
//
//
//


///////////////////////////////////////////////////////////
unsigned short Crc16n(unsigned short usInitialCrc,
                     unsigned char *pData,
                     unsigned long ulLen);
//
// Calcuate the CRC for a given buffer of data.
// To do just one buffer start with an usInitialCrc of 0.
// To continue a multibuffer CRC provide the value
// returned from the last call to Crc16 as the usInitialCrcValue.
//
// inputs:
//    usInitialCrc -- initial value for the CRC.
//    pData -- pointer to the data to calculate the CRC for.
//    ulLen -- length of the data to calculate the CRC for.
// outputs:
//    returns -- the CRC of the buffer.
//
//
////////////////////////////////////////////////////////////

#if !defined(__ASSEMBLY__) && defined(__cplusplus)
}
#endif


#endif
