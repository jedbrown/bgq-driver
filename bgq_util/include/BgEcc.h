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

#ifndef __BG_ECC_H__
#define __BG_ECC_H__

#include <stdint.h>
#include "ArbitraryLengthNumber.h"

class BgEcc
{
public:
    static unsigned ecc64(uint64_t v);
    static uint32_t ecc65(uint64_t v, uint32_t bit64);
    static unsigned ecc72(const ArbitraryLengthNumber &v);
    static uint32_t ecc32(uint32_t v);
    static uint32_t ecc32_correct(uint32_t v, uint32_t ecc, uint32_t *rc  );
    static uint64_t ecc64_correct(uint64_t v, uint32_t ecc, uint32_t *rc  );
    static uint32_t ecc64_correct(uint64_t v,   // original 64 bit data
				  uint32_t ecc, // original  ecc
				  uint64_t & corrected_v,  // corrected data
				  uint32_t & corrected_ecc); // corrected ecc 

    static uint32_t ecc65_correct(uint64_t v, 
				  uint32_t bit64, 
				  uint32_t ecc,
				  uint64_t & corrected_v, 
				  uint32_t & corrected_bit64,
				  uint32_t & corrected_ecc );
    
    
    static uint32_t ecc32_syndrome(uint32_t v, uint32_t ecc)
    {
      uint32_t new_ecc = ecc32(v);
      return  new_ecc ^ ecc;
    }

    static uint32_t ecc64_syndrome(uint64_t v, uint32_t ecc)
    {
      uint32_t new_ecc = ecc64(v);
      return  new_ecc ^ ecc;
    }

    static uint32_t ecc65_syndrome(uint64_t v, uint32_t bit64, uint32_t ecc)
    {
      uint32_t new_ecc = ecc65(v,bit64);
      return  new_ecc ^ ecc;
    }


};



#endif

