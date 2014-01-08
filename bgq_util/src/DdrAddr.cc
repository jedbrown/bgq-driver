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

#include <stdexcept>
#include <iostream>

#include "DdrAddr.h"


/*!
//
// convert a physical address to the address to plug into the bradnelsion address
//
// 
// @param l2_selctrl [in] -- l2 select control
// @param mc_mctl [in] -- memory mctl register
// @param addrin [in] -- address to convert,
// @param mcselout [out] -- memory controller select
// @param addrout [out] -- address in the memory controller.
//
*/
void DdrAddr::phys_to_brad_nelson_addr(unsigned l2_selctrl,
                                       unsigned rank_bit_sel_cfg,
                                       unsigned mc_mcfg0, 
                                       uint64_t mc_mctl, 
                                       uint64_t addrin, 
                                       unsigned &mcselout, 
                                       unsigned &rank,
                                       uint64_t &addrout)
{
    unsigned row, col, bank;
    phys_to_mcrbc(l2_selctrl, rank_bit_sel_cfg, mc_mcfg0, mc_mctl, addrin, mcselout, rank, row, col,bank);
    mcrcb_to_brad_nelson_addr(row, col, bank, addrout);
}

/*!
//
// convert a physical address to the address to plug into the fpga address
//
// 
// @param l2_selctrl [in] -- l2 select control
// @param rank_bit_sel_cfg [in] -- ddr arb configuration dcr setting
// @param mc_mcfgc0 [in] -- memory mcfgc0 register
// @param mc_mctl [in] -- memory mctl register
// @param mc_version [in] -- memory controller version - fake or real
// @param addrin [in] -- address to convert,
// @param mcselout [out] -- memory controller select
// @param addrout [out] -- address in the memory controller.
//
*/
// two versions during transition
int DdrAddr::phys_to_fpga_addr(unsigned l2_selctrl,
                               unsigned rank_bit_sel_cfg,
                               unsigned mc_mcfg0, 
                               uint64_t mc_mctl, 
			       MCVersion mc_version,
                               uint64_t addrin, 
                               unsigned &mcselout, 
                               unsigned &rank, 
                               uint64_t &addrout)
{
    if (mc_version == REAL)
    {
	unsigned row, col, bank;
	phys_to_mcrbc(l2_selctrl, rank_bit_sel_cfg, mc_mcfg0, mc_mctl,
		      addrin, mcselout, rank, row, col, bank);
	mcrcb_to_fpga_addr(rank, row, col, bank, addrout);
    }
    else if (mc_version == FAKE)
    {
	// current implementation does not use the memory controller and arbiter, so bypass it here...
	uint64_t l2_addr;
	unsigned l2_slice;
	sh_slice_select(l2_selctrl,addrin, l2_slice, l2_addr);
	// l2 address cout of here is a 8B address
	rank = 0;
	mcselout = l2_slice >> 2; 
	addrout = (((uint64_t)l2_slice & 3) << 25) | ((l2_addr) & ((1LL<<25)-1)) ;
	if (l2_addr >> 25)
	    return(-1);
    }
    else
    {
        throw runtime_error("DdrAddr::phys_to_fpga_addr: invalid mc_version value");
    }
    return(0);
}

/*!
//
// convert a physical to mc controller row bank col
//
// 
// @param l2_selctrl [in] -- l2 select control
// @param mc_mctl [in] -- memory mctl register
// @param addrin [in] -- address to convert,
// @param mcselout [out] -- memory controller select
// @param row [out] -- row address
// @param col [out] -- column address
// @param bank [out] -- bank address
//
*/
void DdrAddr::phys_to_mcrbc(unsigned l2_selctrl,
                            unsigned rank_bit_sel_cfg,
                            unsigned mc_mcfg0, 
                            uint64_t mc_mctl, 
                            uint64_t addrin, 
                            unsigned &mc_selout, 
                            unsigned &rank,
                            unsigned &row,
                            unsigned &col,
                            unsigned &bank)
{
    uint64_t l2_addr;
    uint64_t mc_addr;
    unsigned l2_slice;

    sh_slice_select(l2_selctrl,addrin, l2_slice, l2_addr);
    l2_to_mcaddr(l2_slice, (l2_addr>>3), rank_bit_sel_cfg, mc_selout, mc_addr);     
    mcaddr_to_mcrcb(mc_mcfg0, mc_addr, rank, row, col, bank);
#if 0
    cout << hex << addrin
	<< ": l2_slice " << hex << l2_slice
	<< ", l2_addr " << hex << l2_addr
	<< ", mc_selout " << hex << mc_selout
	<< ", mc_addr " << hex << mc_addr
	<< ", rank " << hex << rank
	<< ", row " << hex << row
	<< ", col " << hex << col
	<< ", bank " << hex << bank
	<< endl;
#endif
    mc_hot_bank_addr_xor(mc_mctl, col, bank, bank);
    mc_hot_rank_addr_xor(mc_mctl, row, rank, rank);

}



/*!
//
// convert a convert an l2 address to a memory controller address on the south end of the
// dcr arb...  basically this does the address transform that is done by the dcr arb...
//
// 
// @param l2_slice[in] -- l2 select
// @param l2_adcdr in [in] -- l2 address to convert,
// @param rank_bit_sel_cfg [in] -- ???
// @param mcselout [out] -- memory controller select
// @param mcaddrout [out] -- address in the memory controller.
//
*/
void DdrAddr::l2_to_mcaddr(unsigned l2_slice, 
                           unsigned l2_addrin, 
                           unsigned rank_bit_sel_cfg,
                           unsigned &mc_selout, 
                           uint64_t &mc_addrout)
{
    /*!
    //   function from vhdl
    //   -----------------------------------------------------------------------------
    //   -- Function: MapAddress
    //   -- Address Mapping Function
    //   -----------------------------------------------------------------------------
    //   function MapAddress (
    //     slice             : std_ulogic_vector(0 to 2);           -- Slice Number
    //     address           : std_ulogic_vector(0 to 25);          -- Address from L2 slice
    //     rank_bit_sel_cfg  : std_ulogic_vector(0 to 2) := "010")  -- rank bit pointer
    //     return std_ulogic_vector is
    //     variable bank     : std_ulogic_vector(0 to 2);
    //     variable rank_bit : std_ulogic;
    //     variable res      : std_ulogic_vector(0 to 28);
    //   begin  -- function MapAddress
    //     bank     := slice xor address(22 to 24);
    //     rank_bit := xor_reduce(address(21) & bank);
    //     res(0) := Mux_2to1(Ne(rank_bit_sel_cfg, "000"),
    //                        rank_bit,
    //                        address(0));
    //     for i in 1 to 7 loop
    //       res(i) := select_1of3(Gt(rank_bit_sel_cfg, std_ulogic_vector(to_unsigned(i, 3))),
    //                             address(i),
    //                             Eq(rank_bit_sel_cfg, std_ulogic_vector(to_unsigned(i, 3))),
    //                             rank_bit,
    //                             Lt(rank_bit_sel_cfg, std_ulogic_vector(to_unsigned(i, 3))),
    //                             address(i-i));
    //     end loop;  -- i
    //     res(8 to 21)  := address(7 to 20);
    //     res(22 to 24) := slice;
    //     res(25 to 27) := bank;
    //     res(28)       := address(25);
    //     return res;
    //   end function MapAddress;
    */

    if (l2_addrin & (-(1<<26)))
        cout << "warning: l2_addr overflow " << hex << l2_addrin << endl;

    ArbitraryLengthNumber address(26);
    ArbitraryLengthNumber res(29);
    address.setUint32(l2_addrin, 0, 25);
    unsigned slice = l2_slice & 7;
    unsigned bank;      // 3 bits
    unsigned rank_bit;  // 1 bit

    //     bank     := slice xor address(22 to 24);
    bank = slice ^ address.getUint32(22,24);
    //     rank_bit := xor_reduce(address(21) & bank);
    rank_bit = xor_reduce8((address.getUint32(21,21)<<3) | bank);
    //     res(0) := Mux_2to1(Ne(rank_bit_sel_cfg, "000"),
    //                        rank_bit,
    //                        address(0));
    res.setUint32( ((!(rank_bit_sel_cfg != 0)) ? rank_bit : address.getUint32(0,0)),   0,0);
    //     for i in 1 to 7 loop
    //       res(i) := select_1of3(Gt(rank_bit_sel_cfg, std_ulogic_vector(to_unsigned(i, 3))),
    //                             address(i),
    //                             Eq(rank_bit_sel_cfg, std_ulogic_vector(to_unsigned(i, 3))),
    //                             rank_bit,
    //                             Lt(rank_bit_sel_cfg, std_ulogic_vector(to_unsigned(i, 3))),
    //                             address(i-i));
    //     end loop;  -- i
    for (unsigned i = 1; i <= 7; i++)
    {
        if (rank_bit_sel_cfg > i)
            res.setUint32(address.getUint32(i,i), i, i);
        else if (rank_bit_sel_cfg == i)
            res.setUint32(rank_bit, i, i);
        else
            res.setUint32(address.getUint32(i-1, i-1),i,i);
    }
    //     res(8 to 21)  := address(7 to 20);
    res.setUint32(address.getUint32(7,20), 8, 21);
    //     res(22 to 24) := slice;

    res.setUint32(slice, 22,24);
    //     res(25 to 27) := bank;
    res.setUint32(bank, 25, 27);
    //     res(28)       := address(25);
    res.setUint32(address.getUint32(25,25),28, 28);
    //     return res;
    mc_selout = (l2_slice >> 3) & 1;
    mc_addrout = res.getUint64(0,28);          // there may have to be some alignment stuff going on with this...
    
}

/*!
// convert the mc address to row column bank.
//
// @param mc_mcfg0 -- contents of the mccfg_0 register (bits 9..12)
// @param mcaddr [in] -- memory controller address to select
// @param row [out] -- row address
// @param col [out] -- column address
// @param bank [out] -- bank address
*/
void DdrAddr::mcaddr_to_mcrcb(unsigned mc_mcfg0, 
                              uint64_t mc_addr,
                              unsigned &rank,
                              unsigned &row,
                              unsigned &col,
                              unsigned &bank)
{

    struct Rcb {
        unsigned r;
        unsigned c;
        unsigned b;
    };
    static Rcb rcbTable[] = {
        { 13,10,3 }, // 0000         13/10/3
        { 13,11,3 }, // 0001         13/11/3
        { 14,10,3 }, // 0010         14/10/3
        { 14,11,3 }, // 0011         14/11/3
        { 15,10,3 }, // 0100         15/10/3
        { 15,11,3 }, // 0101         15/11/3
        { 16,10,3 }, // 0110         16/10/3
        { 16,11,3 }, // 0111         16/11/3
        { 16,13,3 } // 1000         16/12/3
    };    
    if (mc_mcfg0 >= sizeof(rcbTable)/sizeof(rcbTable[0]))
        throw runtime_error("DdrAddr: invalid mcfg0 value");
    Rcb &rcb(rcbTable[mc_mcfg0]);
    mcaddr_parse_rcb(mc_addr, rcb.r, rcb.c, rcb.b,  rank, row, col, bank); 
        
}


/*!
// parse out the row col bank based on the rcb field sizes
// @param mc_addr [in] address to convert
// @param r [in] row bits
// @param c [in] col bits
// @param b [in] bank bits
// @param row [in] -- row address
// @param col [in] -- col address
// @param bank [in] -- bank address
*/
void DdrAddr::mcaddr_parse_rcb(uint64_t mc_addr,
                               unsigned r,
                               unsigned c,
                               unsigned b,
                               unsigned &rank,
                               unsigned &row,
                               unsigned &col,
                               unsigned &bank)
{
    uint64_t addr  = (mc_addr>>1);      // go from 64 byte to 128 byte addr
    bank = addr & ((1<<b)-1);           // mask off the bit number
    addr >>= b;
    col = (addr & ((1<<(c-3))-1)) << 3; // column adjusted for burst
    addr >>= (c-3);
    row = addr & ((1<<r)-1);
    addr >>=  r;
    rank = addr & 0x07;         // now pick off the rank (up to three bits)
}


/*!
*
* convert the hot bank address xor as per page 293 of the mc workbook
*
* @param mctl [in] -- contents of them mctl register.
* @param col [in] -- column address to xor.
* @param bankin [in] -- bank sel in.
* @param bankout [out] -- bank sel out.
*/
void DdrAddr::mc_hot_bank_addr_xor(uint64_t mc_mctl,
                                 unsigned col,
                                 unsigned bankin,
                                 unsigned &bankout)
{
  if (mc_mctl & 0xFF800000)
    {
        unsigned addr_xor[4];
        addr_xor[0] = (mc_mctl >> (63-34)) & 0x07;      // address 0
        addr_xor[1] = (mc_mctl >> (63-37)) & 0x07;      // address 1
        addr_xor[2] = (mc_mctl >> (63-40)) & 0x07;      // address 2

        unsigned b[3];
        b[0] = (bankin >> 0) & 1;
        b[1] = (bankin >> 1) & 1;
        b[2] = (bankin >> 2) & 1;
        for (unsigned n = 0; n < 3; n++)
        {
            if (addr_xor[n])
	      b[n] = b[n] ^ ((col >> ((addr_xor[n]-1)+3)) & 1);
        }

        bankout = (b[2]<<2) |
	          (b[1]<<1) |
	          (b[0]<<0);
    }
    else
        bankout = bankin;

    // todo... channel xor...
}
                 
/*!
*
* hot rank address xor as per page 337 of the mc workbook
*
* @param mctl [in] -- contents of them mctl register.
* @param row [in] -- row address to xor.
* @param rankin [in] -- rank sel in.
* @param rankout [out] -- rank sel out.
*/
void DdrAddr::mc_hot_rank_addr_xor(uint64_t mc_mctl,
				   unsigned row,
				   unsigned rankin,
				   unsigned &rankout)
{
  if (mc_mctl & (1ll<<(63-44)))
      rankout = rankin ^ (row & 3);
    else
      rankout = rankin;
}
                 
/*!
*
* convert the row,col, bank address to an address for the brad nelson model
* 
* @param row [in] -- row address
* @param col [in] -- col address
* @param bank [in] -- bank address
* @param addrout[out] -- flat memory address
*/
void DdrAddr::mcrcb_to_brad_nelson_addr(unsigned row,
                                        unsigned col,
                                        unsigned bank,
                                        uint64_t &addrout)
{
    addrout = ((row & ((1<<15)-1)) << 13) | ((bank & ((1<<3)-1)) << 10) | (col & ((1<<10)-1));
}

/*!
*
* convert the rank,row,col,bank address to an address for the fpga model
* 
* @param rank [in] -- rank address
* @param row [in] -- row address
* @param col [in] -- col address
* @param bank [in] -- bank address
* @param addrout[out] -- flat memory address
*/
void DdrAddr::mcrcb_to_fpga_addr(unsigned rank,
				 unsigned row,
				 unsigned col,
				 unsigned bank,
				 uint64_t &addrout)
{
/*
 * Translated from Mohit Kapur email:
 *
 *      addrout(30) <= '0'     -- upper 4 bits of MIG address are connected
 *      addrout(29) <= '0'        to 0 as there is only 1GB available.
 *      addrout(28) <= '0'
 *      addrout(27) <= '0'
 *
 *      addrout(26) <= bank(2)
 *      addrout(25) <= bank(1)
 *      addrout(24) <= bank(0)
 *
 *      addrout(23) <= row(11) ^ rank(1) -- three bits (14 to 12) of row address from
 *      addrout(22) <= row(10) ^ rank(2)    MC is dropped to account for space
 *      addrout(21) <= row(9)     reduction from 8G to 1G.
 *      addrout(20) <= row(8)
 *      addrout(19) <= row(7)
 *      addrout(18) <= row(6)
 *      addrout(17) <= row(5)
 *      addrout(16) <= row(4)
 *      addrout(15) <= row(3)
 *      addrout(14) <= row(2)
 *      addrout(13) <= row(1)
 *      addrout(12) <= row(0)
 *
 *      addrout(11) <= col(9)  
 *      addrout(10) <= col(8)
 *      addrout(9)  <= col(7)
 *      addrout(8)  <= col(6)
 *      addrout(7)  <= col(5)
 *      addrout(6)  <= col(4)
 *      addrout(5)  <= col(3)  -- three bits (2 to 0) of column address from MC
 *                                are dropped to replace the BGQ burst of 8 by
 *                                DDR2 burst of 16.
 *
 *      addrout(4)  <= rank(0) ^ row(12)
 *
 *      addrout(3)  <= '0'     -- internally generated to transfer 4 bursts
 *      addrout(2)  <= '0'        of 4 72-byte beats for every cacheline.
 *                                Chuck/Ralph will have to generate these bits.
 *
 *      addrout(1)  <= '0'     -- 2 LSBs of MIG address have to be kept 0 for
 *      addrout(0)  <= '0'        the burst-of-4 mode operation of MIG.
 */

    if ((rank & ~0x1) != 0)
        throw runtime_error("mcrcb_to_fpga_addr: invalid rank value");
    if ((row & ~0xfff) != 0)
        throw runtime_error("mcrcb_to_fpga_addr: invalid row value");
    if ((col & ~0x3f8) != 0)
        throw runtime_error("mcrcb_to_fpga_addr: invalid col value");
    if ((bank & ~0x7) != 0)
        throw runtime_error("mcrcb_to_fpga_addr: invalid bank value");

    addrout = ((bank          << (4 + 1 + 7 + 12)) |
	       ((row&0xfff)   << (4 + 1 + 7)) |
	       ((col>>3)      << (4 + 1)) |
	       ((rank&1)      << (4))) ^
              ((((rank>>1)&1) << (4 + 1 + 7 + 11)) |
	       (((rank>>2)&1) << (4 + 1 + 7 + 10)) |
	       (((row>>12)&1) << (4)));
}
