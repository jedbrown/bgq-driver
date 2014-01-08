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

#ifndef __DDRADDR_H__
#define __DDRADDR_H__

/*!
// Class to handle l2 slice to and from physical  calculations
*/

#include <bgq_util/include/ArbitraryLengthNumber.h>


#include <bgq_util/include/BgL2Slice.h>



//
// converter functions to convert an to and from the l2 address space.
//
class DdrAddr 
{
public:
    typedef enum {FAKE, REAL} MCVersion;

    /*!
    //
    // convert a physical address to the address to plug into the bradnelsion address
    //
    // 
    // @param mc_mccfg0 -- contents of the mccfg_0 register (bits 9..12)
    // @param rank_bit_sel_cfg [in] -- ddr arb configuration dcr setting
    // @param mc_mctl [in] -- memory mctl register
    // @param l2_selctrl [in] -- l2 select control
    // @param addrin [in] -- address to convert,
    // @param mcselout [out] -- memory controller select
    // @param addrout [out] -- address in the memory controller.
    //
    */
    static void phys_to_brad_nelson_addr(unsigned l2_selctrl,
                                         unsigned rank_bit_sel_cfg,
                                         unsigned mc_mcfgc0, 
                                         uint64_t mc_mctl, 
                                         uint64_t addrin, 
                                         unsigned &mcselout, 
                                         unsigned &rank,
                                         uint64_t &addrout);

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
    // @return -- 0 -1 if it overfloweds
    */
    // two versions during transition
    static int phys_to_fpga_addr(unsigned l2_selctrl,
                                  unsigned rank_bit_sel_cfg,
                                  unsigned mc_mcfgc0, 
                                  uint64_t mc_mctl, 
				  MCVersion mc_version,
                                  uint64_t addrin, 
                                  unsigned &mc_selout, 
                                  unsigned &rank, 
                                  uint64_t &addrout);


    /*!
    //
    // convert a physical to mc controller row bank col
    //
    // 
    // @param l2_selctrl [in] -- l2 select control
    // @param rank_bit_sel_cfg [in] -- ddr arb configuration dcr setting
    // @param mc_mctl [in] -- memory mctl register
    // @param mc_mctl [in] -- memory mctl register
    // @param addrin [in] -- address to convert,
    // @param mcselout [out] -- memory controller select
    // @param row [out] -- row address
    // @param col [out] -- column address
    // @param bank [out] -- bank address
    //
    */
    static void phys_to_mcrbc(unsigned l2_selctrl,
                              unsigned rank_bit_sel_cfg,
                              unsigned mc_mcfgc0, 
                              uint64_t mc_mctl, 
                              uint64_t addrin, 
                              unsigned &mc_selout, 
                              unsigned &rank,
                              unsigned &row,
                              unsigned &col,
                              unsigned &bank);


    /*!
    // convert a flat address into an l2 address, this has to match the code in
    // in ddx/share/vhdl/sh_slice_select.vhdl.
    //
    // @param l2_selctrl
    // @param physaddr [in] flat address to convert.
    // @param l2slice [out] l2slice to target.
    // @param l2addr [out] l2 address.
    //
    // @returns none.
    */
    static void sh_slice_select(unsigned l2_selctrl, uint64_t physaddr, unsigned &l2slice, uint64_t &switch_addr) {
        BgL2Slice::sh_slice_select(l2_selctrl, physaddr, l2slice, switch_addr); };

    /*!
    //
    // convert a convert an l2 address to a memory controller address on the south end of the
    // dcr arb...  basically this does the address transform that is done by the dcr arb...
    //
    // 
    // @param l2_slice [in] -- l2 slice select
    // @param l2_addrin[in] -- l2 address to convert,
    // @param rank_bit_sel_cfg [in] -- ddr arb configuration dcr setting
    // @param mcselout [out] -- memory controller select
    // @param mcaddrout [out] -- address in the memory controller.
    //
    */
    static void l2_to_mcaddr(unsigned l2_slice, 
                             unsigned l2_addrin, 
                             unsigned rank_bit_sel_cfg,
                             unsigned &mc_selout, 
                             uint64_t &mc_addrout);

    /*!
    // convert the mc address to row column bank.
    //
    // @param mc_mcfgc0 -- contents of the mc_mcfgc0 register (bits 9..12)
    // @param mcaddr [in] -- memory controller address to select
    // @param row [out] -- row address
    // @param col [out] -- column address
    // @param bank [out] -- bank address
    */
    static void mcaddr_to_mcrcb(unsigned mc_mcfgc0, 
                              uint64_t mcaddr,
                              unsigned &rank,
                              unsigned &row,
                              unsigned &col,
                              unsigned &bank);

    /*!
    *
    * convert the hot bank address xor as per page 293 of the mc workbook
    *
    * @param mc_mctl [in] -- contents of them mctl register.
    * @param col [in] -- column address to xor.
    * @param bankin [in] -- bank sel in.
    * @param bankout [out] -- bank sel out.
    */
    static void mc_hot_bank_addr_xor(uint64_t mc_mctl,
                                     unsigned col,
                                     unsigned bankin,
                                     unsigned &bankout);
                     
    /*!
    *
    * hot rank address xor as per page 337 of the mc workbook
    *
    * @param mctl [in] -- contents of them mctl register.
    * @param row [in] -- row address to xor.
    * @param rankin [in] -- rank sel in.
    * @param rankout [out] -- rank sel out.
    */
    static void mc_hot_rank_addr_xor(uint64_t mc_mctl,
				     unsigned row,
				     unsigned rankin,
				     unsigned &rankout);
    /*!
    *
    * convert the row,col, bank address to an address for the brad nelson model
    * 
    * @param row [in] -- row address
    * @param col [in] -- col address
    * @param bank [in] -- bank address
    * @param addrout[out] -- flat memory address
    */
    static void mcrcb_to_brad_nelson_addr(unsigned row,
                                          unsigned col,
                                          unsigned bank,
                                          uint64_t &addrout);

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
    static void mcrcb_to_fpga_addr(unsigned rank,
                                   unsigned row,
                                   unsigned col,
                                   unsigned bank,
                                   uint64_t &addrout);

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
    static void mcaddr_parse_rcb(uint64_t mc_addr,
                               unsigned r,
                               unsigned c,
                               unsigned b,
                               unsigned &rank,
                               unsigned &row,
                               unsigned &col,
                               unsigned &bank);

protected:

    static unsigned xor_reduce8(unsigned v) {
        v = v ^ (v>>4);
        v = v ^ (v>>2);
        v = v ^ (v>>1);
        return(v & 1);
    };


private:


};

#endif

