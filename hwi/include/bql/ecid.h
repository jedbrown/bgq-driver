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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef __hwi_bql_ecid__h__
#define __hwi_bql_ecid__h__

/**
 * \file bql_ecid.h
 *
 * \brief BQL ECID
 *
 */

/** \brief BQL ECID Structure */
typedef struct  BqlEcid {
   unsigned char _data[16];  // binary data (non-ascii), 128 bits.

   const unsigned char* asBits() const  { return _data; }

   void clear()  { memset(&_data[0], 0x00, sizeof(_data)); }

   string identity() { return asString(); }

   string asString()    // Return a string containing Hex Character representation of this data.
   {
      // This data is binary data - must be converted into hex character representation w/i string.
      // Convert into character representation and copy into user's area. 
      stringstream tempStrStream;
      // Save tempStrStream's width and fill values so they can later be restored.
      streamsize saveTempStrStreamWidth = tempStrStream.width();  // save current width value.
      char       saveTempStrStreamFill  = tempStrStream.fill();   // save fill character.
      tempStrStream.fill('0');  // set fill character to a '0'.
      // Put hex character representation of the binary data into string stream.
      for (uint dataCntr = 0; dataCntr < sizeof(_data); ++dataCntr)
      {
	 tempStrStream << setw(2) << hex << (uint)_data[dataCntr] << dec;
      }
      // Restore tempStrStream's width and fill values.
      tempStrStream.width(saveTempStrStreamWidth);  // restore saved width value.
      tempStrStream.fill(saveTempStrStreamFill);    // restore saved fill character.
      // Actually put the string form of tempStrStream into a string and return to user.
      string sData;
      tempStrStream >> sData;
      return sData;
   }

   BqlEcid(const unsigned char* data) { memcpy(&_data[0], data, sizeof(_data)); }

   BqlEcid()  { clear(); }

} BqlEcid_t;

#endif /* __hwi_bql_ecid_h__ */
