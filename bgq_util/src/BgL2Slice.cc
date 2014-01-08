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


#include "BgL2Slice.h" 

#include <stdexcept>
#include <iostream>

using namespace std;


/*!
// convert a phys address into an l2 address, this has to match the code in
// in ddx/share/vhdl/sh_slice_select.vhdl.
//
// @param sel_ctrl [in] select control to use for the conversion
// @param addr [in] phys address to convert.
// @param slice [out] l2slice to target.
// @param switch_addr [out] l2 address.
//
// @returns none.
*/
void BgL2Slice::sh_slice_select(unsigned sel_ctrl, 
                             uint64_t addr, 
                             unsigned &slice, 
                             uint64_t &switch_addr)
{
    ArbitraryLengthNumber sel_ctrl_an(4*4);
    ArbitraryLengthNumber addr_an(64);

    ArbitraryLengthNumber slice_an(5);
    ArbitraryLengthNumber switch_addr_an(34);

    sel_ctrl_an.setUint(sel_ctrl,0,sel_ctrl_an.getBitCount()-1);
    addr_an.setUint(addr, 22, 63);

    sh_slice_select(sel_ctrl_an, addr_an, slice_an, switch_addr_an);

    slice = slice_an.getUint32(0,slice_an.getBitCount()-1);
    switch_addr = switch_addr_an.getUint64(0,switch_addr_an.getBitCount()-1);

    // going from phys to l2 slice, the address is divided into 4 8 bit columns, 
    // 
}

void BgL2Slice::sh_slice_select(const ArbitraryLengthNumber &sel_ctrl, 
                             const ArbitraryLengthNumber &addr, 
                             ArbitraryLengthNumber &slice, 
                             ArbitraryLengthNumber &switch_addr)
{

    unsigned mmio = addr.getUint32(22,22);
    //unsigned local_io = mmio & addr.getUint32(31,31) & (!addr.getUint32(35,35));
    unsigned net_io = mmio & addr.getUint32(31,31) & (addr.getUint32(35,35));

    // todo: make this more efficient...
    ArbitraryLengthNumber net_io_port(2);
    net_io_port.setUint(net_io & (addr.getUint32(36,43) == 0x80), 0, 0);
    net_io_port.setUint(net_io & (addr.getUint32(36,43) == 0x81), 1, 1);

    ArbitraryLengthNumber io_hash(4);
    ArbitraryLengthNumber slice_hash(4);
    ArbitraryLengthNumber shifted_addr(32);

    ArbitraryLengthNumber mux_addr(28);

    io_hash.setUint((nor_reduce(net_io_port.getUint32(0,1)) << 1) |  net_io_port.getUint32(0,0), 0,3);
    unsigned compr_atomic = addr.getUint32(23,23);

    // shifted_addr <= "000" & Mux_2to1(compr_atomic, addr(28 to 56), addr(25 to 53));
    shifted_addr.setUint((!compr_atomic ? addr.getUint32(28,56) : addr.getUint32(25,53)), 3, shifted_addr.getBitCount()-1);

    for (unsigned i = 0; i <= 3; i++)
    {
        ArbitraryLengthNumber dec(8);
        ArbitraryLengthNumber col(8);
        dec.setUint(Decode_3to8(sel_ctrl.getUint32(i*4+1, i*4+3)), 0, dec.getBitCount()-1);
        for (unsigned j = 0; j <= 7; j++)
        {
            unsigned v = shifted_addr.getUint32(j*4+i, j*4+i) & ((!sel_ctrl.getUint32(i*4,i*4)) | dec.getUint32(7-j, 7-j));
            col.setUint(v, j,j);
        }
        slice_hash.setUint(xor_reduce8(col.getUint32(0,8-1)),3-i,3-i);
    }

    slice.setUint(mmio, 0, 0);
    slice.setUint(((!mmio) ? slice_hash.getUint32(0,3) : io_hash.getUint32(0,3)), 1, slice.getBitCount()-1);

    // mux out one bit for each of the 4 slice selector bits.
    for (unsigned i=0; i <= 3; i++)
    {
        for (unsigned j = 0; j <= 6; j++)
        {
            unsigned v;
            if (!(sel_ctrl.getUint32(i*4+1,i*4+3) >= (7-j)))
                v = shifted_addr.getUint32(i+j*4, i+j*4);
             else 
                v = shifted_addr.getUint32(i+j*4+4, i+j*4+4);
            mux_addr.setUint(v,i+j*4, i+j*4);
        }
    }

    ArbitraryLengthNumber l2_addr(34);
    //-- operation if compr_atomic (000 = regular access)
    l2_addr.setUint((!compr_atomic) ? addr.getUint32(24,25) : addr.getUint32(54,56), 0,2);

    //-- L1 aliasing
    l2_addr.setUint(addr.getUint32(26,27), 3,  4);
    //-- compressed address, slice selector removed
    l2_addr.setUint(mux_addr.getUint32(3,27), 5, 29);
    //-- 8B entity in 128B line
    l2_addr.setUint(addr.getUint32(57,60), 30, 33);

    switch_addr.setUint((!mmio) ? l2_addr.getUint64(0,33) : addr.getUint64(30, 63),0,33);

}


/*!
// Convert a l2 alice and address into a phys address.
//
// @param sel_ctrl -- select control to use for this...
// @param slice [in] l2 slice portion of the address
// @param l2_addr  [in] l2 portion of the address
// @returns phys address
*/

uint64_t BgL2Slice::sh_l2_to_phys(const ArbitraryLengthNumber &sel_ctrl, 
                                 const ArbitraryLengthNumber &slice, 
                                 const ArbitraryLengthNumber &l2_addr)
{

    if ((sel_ctrl.getUint32(1,3) == 0x07) || 
        (sel_ctrl.getUint32(5,7) == 0x07) || 
        (sel_ctrl.getUint32(9,11) == 0x07))
    {
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << "Undefined sel_ctrl value" << endl;
        cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        throw runtime_error("Undefined sel_ctrl value");
    }
    
    // going from l2 to phys, we have l2 in 4 7 bit columns, and the resultant address
    // in 4 8 bit columsn.

    //-- of the value. These mux selects will be optimized out
    // l2_addr_pad <= "0000000" & l2_addr & "0000";         // vhdl code...
    ArbitraryLengthNumber l2_addr_pad(36);
    l2_addr_pad.setUint(l2_addr.getUint32(0,l2_addr.getBitCount()-1), 7, 35-4);           // padded address 

    //cout << "l2_addr = " << l2_addr.getHexString() << endl;
    //cout << "l2_addr_pad = " << l2_addr_pad.getHexString() << endl;


    ArbitraryLengthNumber phys_addr_pad(32);

    for (unsigned i=0; i <= 3; i++)
    {
        ArbitraryLengthNumber col(7);             
        for (unsigned j=0; j<=6; j++)
            col.setUint(l2_addr_pad.getUint32(j*4+i+4, j*4+i+4), j, j);

        unsigned unscr_slice;
        if (sel_ctrl.getUint32(i*4,i*4) == 0)
            unscr_slice = xor_reduce8((col.getUint32(0,6) << 1) | slice.getUint32(3-i,3-i));
        else
            unscr_slice = slice.getUint32(3-i,3-i);
        unsigned sel = sel_ctrl.getUint32(i*4+1, i*4+3);

        for (unsigned j=0; j<=7; j++)
        {
            unsigned v;
            if (sel > 7-j)
                v = l2_addr_pad.getUint32(j*4+i, j*4+i);
            else 
            if (sel == 7-j)
                v = unscr_slice;
            else 
            if (sel < 7-j)
                v = l2_addr_pad.getUint32(j*4+i+4, j*4+i+4);
            else
                throw runtime_error("internal error");
            phys_addr_pad.setUint(v,j*4+i,j*4+i);
        }
    }
    return(phys_addr_pad.getUint32(3,31));
}
 


uint64_t BgL2Slice::l2ToPhysAddr(unsigned selctrl, unsigned l2slice, uint64_t switch_addr)
{
    ArbitraryLengthNumber selctrl_an(4*4);
    ArbitraryLengthNumber l2_addr_an(25);
    ArbitraryLengthNumber slice_an(4);

    ArbitraryLengthNumber switch_addr_an(34);
    switch_addr_an.setUint(switch_addr, 0, switch_addr_an.getBitCount()-1);

    selctrl_an.setUint(selctrl,0,selctrl_an.getBitCount()-1);       // put this where we can pull it appart bit by bit.
    slice_an.setUint(l2slice&0xf,0,3);
    l2_addr_an.setUint(switch_addr_an.getUint32(5,29),0, l2_addr_an.getBitCount()-1);


    uint64_t phys_addr = sh_l2_to_phys(selctrl_an, slice_an, l2_addr_an);        // this is bits 26--56 of the out address

    ArbitraryLengthNumber addr_out(64);
    addr_out.setUint(phys_addr, 28,56);    
    addr_out.setUint(switch_addr_an.getUint32(0,4), 23, 27);        // L2 Atomics
    addr_out.setUint(switch_addr_an.getUint32(30,33), 57, 60);      // put back the 8 byte select...
    return(addr_out.getUint64(0,63));
}

