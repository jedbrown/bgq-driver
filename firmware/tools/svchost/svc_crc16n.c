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



#if defined( _AIX )
#include <inttypes.h>
#else
#include <stdint.h>
#endif

//
// This file contains the function to generate a 16 bit
//   CRC utilizing a 16 byte table.
//
// The CRC polynomial used here is:
//
//    x^16 + x^12 + x^5 + 1
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

uint16_t Crc16x4_Table[ 16 ] = {
    0x0000, 0x1081, 0x2102, 0x3183, 0x4204, 0x5285, 0x6306, 0x7387,
    0x8408, 0x9489, 0xa50a, 0xb58b, 0xc60c, 0xd68d, 0xe70e, 0xf78f  };


uint16_t _bgp_Crc16n( unsigned short usInitialCrc,
                      unsigned char *pData,
                      unsigned long ulLen )
{
    unsigned long n;
    unsigned short t;
    unsigned char *p;
    unsigned short usCrc = usInitialCrc;


    for (n = ulLen, p = pData;
         n > 0;
         n--, p++)
    {
        unsigned char c;
        c = *p;                     // brab the character.

        t = usCrc ^ (c & 0x0f);                         // lower nibble
        usCrc = (usCrc>>4) ^ Crc16x4_Table[t & 0xf];
        t = usCrc ^ (c>>4);                             // upper nibble.
        usCrc = (usCrc>>4) ^ Crc16x4_Table[t & 0xf];
    }

    return(usCrc);
}




