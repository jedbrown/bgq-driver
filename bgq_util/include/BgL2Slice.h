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

#ifndef __BG_L2_SLICE_H__
#define __BG_L2_SLICE_H__

#include <stdint.h>
#include "ArbitraryLengthNumber.h"

class BgL2Slice
{
public:

    /*!
    // convert a flat address into an l2 address, this has to match the code in
    // in ddx/share/vhdl/sh_slice_select.vhdl.
    //
    // @param selctrl
    // @param physaddr [in] flat address to convert.
    // @param l2slice [out] l2slice to target.
    // @param l2addr [out] l2 address.
    //
    // @returns none.
    */
    static void sh_slice_select(unsigned selctrl, uint64_t physaddr, unsigned &l2slice, uint64_t &switch_addr);


    /*!
    // convert a flat address into an l2 address, this has to match the code in
    // in ddx/share/vhdl/sh_slice_select.vhdl.
    //
    // @param selctrl
    // @param physaddr [in] flat address to convert.
    // @param l2slice [out] l2slice to target.
    // @param l2addr [out] l2 address.
    //
    // @returns none.
    */
    static void sh_slice_select(const ArbitraryLengthNumber &selctrl, 
                             const ArbitraryLengthNumber &physaddr, 
                             ArbitraryLengthNumber &l2slice, 
                             ArbitraryLengthNumber &switch_addr);

    /*!
    // convert a flat address into an l2 address, this has to match the code in
    // in ddx/share/vhdl/sh_slice_select.vhdl.
    //
    // @param selctrl
    // @param l2slice [out] l2slice to target.
    // @param switch)addr [out] switch_addr
    //
    // @returns none.
    */
    static uint64_t l2ToPhysAddr(unsigned selctrl, unsigned l2slice, uint64_t switch_addr);

    /*!
    // Convert a l2 alice and address into a phys address.
    //
    // @param selctrl [in] select control to use for the conversion
    // @param l2slice [in] l2 slice portion of the address
    // @param l2addr  [in] l2 portion of the address
    // @returns phys address
    */
    static uint64_t sh_l2_to_phys(const ArbitraryLengthNumber &sel_ctrl, 
                                 const ArbitraryLengthNumber &slice, 
                                 const ArbitraryLengthNumber &l2_addr);


    static unsigned xor_reduce8(unsigned v) {
        v = v ^ (v>>4);
        v = v ^ (v>>2);
        v = v ^ (v>>1);
        return(v & 1);
    };

    static unsigned Decode_3to8(unsigned v) {
        return (0x80 >> v);     
    };

    static unsigned nor_reduce(unsigned v)
    {
        return(v ? 0 : 1 );
    };
    static unsigned or_reduce(unsigned v)
    {
        return(v ? 1 : 0 );
    }

protected:

private:

};

#endif


