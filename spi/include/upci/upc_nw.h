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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef _UPC_NW_H_
#define _UPC_NW_H_

#include "stdio.h"

#include "spi/include/kernel/location.h"
#include "spi/include/kernel/upc_hw.h"

#include <hwi/include/bqc/nd_rese_dcr.h>
#include <hwi/include/bqc/nd_500_dcr.h>

#include "node_parms.h"

__BEGIN_DECLS

//! \addtogroup upc_spi
//@{
/**
 *
 * \file
 *
 * \brief UPC SPI Network Types and Operations
 *
 * Network Counting Usage Notes:
 *
 * - 11 Links * 6 counters per link (64 bits/counter)
 * - 10 torus links (links 0-9), and 1 I/O link (link 10)
 * - Packet sizes are from [1 (header only) to 17 (header + 512 bytes payload)]  * 32 byte chunks
 * - 7 Virtual Channels (0-6)
 * - Counter VC programmed via enable masks (\ref UPC_NW_Vchannels)
 *   - VC mask per counter (ctrs 0-3)  send chunk counters
 *   - single VC mask for counters 4&5 receive contention counters
 * - Architecture does not allow user commworld and system collective on the same link.
 *
 * - Network counters must only be controlled (configure/clear/start/stop) from one software thread.
 *   Last thread wins otherwise and the data be incorrect (espeically the elapsed time)
 *   Caller must regulate.
 * - Start and Stop operations must not be nested.
 *
 *
 * Counters
 * - Counters 0-3 count number of 32 byte chunks sent on link (for programmed VC) (does not include retransmissions)
 * - Counters 4 & 5 are receiver counts to measure contention for packets in receives,
 *   - thus for Little's law
 *     - avg queue length (L)
 *     - avg waiting times (W)
 *     - arrival rate (lamda)
 *     - L = lamda * W
 *   - counter 4 counts total num of packets received on vc each network cycle
 *     - ctr 4 / elapsed nw cycles = arrival rate (lamda)
 *   - counter 5 accumulates the number of packets in receiver fifo each network cycle
 *     - ctr 5 / elapsed nw cycles = avg queue length (L)
 *
 * - UPC_GetNWCycles() returns the number of network which have occurred between
 *     start and stop. The timebase snapshots occur close to the network counter start and stop
 *     to give a more accurate calculations for 4,5.  The return value is scaled to the network cycle rate.
 *     It is valid only after a Upci_Mode_Init() has occurred.
 *
 * Counter Default Programming (all links):
 * - ctr 0 = count # of user pt-pt packet chunks sent (UPC_NW_VC_USER_DYNAMIC | UPC_NW_VC_USER_ESCAPE | UPC_NW_VC_USER_PRIORITY)
 * - ctr 1 = count # of system pt-pt packet chunks sent (UPC_NW_VC_SYSTEM)
 * - ctr 2 = count # of collective commworld chunks sent (UPC_NW_VC_COL_USER_WORLD | UPC_NW_VC_COL_SYSTEM)
 * - ctr 3 = count # of sub comm packet chunks sent (UPC_NW_VC_COL_USER_SUBCOMM)
 * - ctr 4&5 = count receive packet metrics for user pt-pt packets (UPC_NW_VC_USER_DYNAMIC | UPC_NW_VC_USER_ESCAPE | UPC_NW_VC_USER_PRIORITY)
 */
//@}



//! \typedef UPC_NW_Vchannels
typedef enum {
    UPC_NW_VC_USER_DYNAMIC      = 0x40,  //!< User dynamically routed point to point
    UPC_NW_VC_USER_ESCAPE       = 0x20,  //!< User deterministic (escape) point to point
    UPC_NW_VC_USER_PRIORITY     = 0x10,  //!< User high priority point to point
    UPC_NW_VC_SYSTEM            = 0x08,  //!< System point to point
    UPC_NW_VC_COL_USER_WORLD    = 0x04,  //!< Collective user commworld
    UPC_NW_VC_COL_USER_SUBCOMM  = 0x02,  //!< Collective user subcommunicator
    UPC_NW_VC_COL_SYSTEM        = 0x01,  //!< System collective transfers
    UPC_NW_VC_ALL               = 0x7F,  //!< All channels
} UPC_NW_Vchannels;



//! \typedef UPC_NW_UnitCtr
//! is used to map nw events to available counters in event tables.
typedef enum {
    UPC_NW_UC_NONE              = 0,     //!< Counter not assigned yet
    UPC_NW_UC_SENDCHKS          = 1,     //!< Send chunk event - uses ctrs 0-3.
    UPC_NW_UC_RCVPKTS           = 4,     //!< Counter with # packets received each nw cycle
    UPC_NW_UC_FIFOPKTS          = 5,     //!< Counter with # packets in receiver FIFO each nw cycle
    UPC_NW_UC_CYCLES            = 6,     //!< Virtual cycle time counter
                                         //!< (UPC does not track cycles, this is flag to Bgpm level)
} UPC_NW_UnitCtr;



#define UPC_NW_LINK_bit(b)        (1<<(15-(b)))     //!< get nw link bit mask from single link number



//! \typedef UPC_NW_LinkMasks
//! 16 bit Masks which allow link selection by name
typedef enum {
    UPC_NW_ALL_LINKS        = 0xFFE0,  //!< all nw links
    UPC_NW_ALL_TORUS_LINKS  = 0xFFC0,  //!< all nw torus links
    UPC_NW_ALL_IO_LINK      = 0x0020,  //!< the nw io link
    UPC_NW_LINK_A           = 0xC000,  //!< the A dimension torus links
    UPC_NW_LINK_A_MINUS     = 0x8000,  //!< the A minus dimension torus links
    UPC_NW_LINK_A_PLUS      = 0x4000,  //!< the A plus dimension torus links
    UPC_NW_LINK_B           = 0x3000,  //!< the B dimension torus links
    UPC_NW_LINK_B_MINUS     = 0x2000,  //!< the B minus dimension torus links
    UPC_NW_LINK_B_PLUS      = 0x1000,  //!< the B plus dimension torus links
    UPC_NW_LINK_C           = 0x0C00,  //!< the B dimension torus links
    UPC_NW_LINK_C_MINUS     = 0x0800,  //!< the B minus dimension torus links
    UPC_NW_LINK_C_PLUS      = 0x0400,  //!< the B plus dimension torus links
    UPC_NW_LINK_D           = 0x0300,  //!< the B dimension torus links
    UPC_NW_LINK_D_MINUS     = 0x0200,  //!< the B minus dimension torus links
    UPC_NW_LINK_D_PLUS      = 0x0100,  //!< the B plus dimension torus links
    UPC_NW_LINK_E           = 0x00C0,  //!< the B dimension torus links
    UPC_NW_LINK_E_MINUS     = 0x0080,  //!< the B minus dimension torus links
    UPC_NW_LINK_E_PLUS      = 0x0040,  //!< the B plus dimension torus links
} UPC_NW_LinkMasks;



//! \brief UPC_NW_GetEnabledLinks
//! return a link mask indicating which links are enabled
__INLINE__ uint16_t UPC_NW_GetEnabledLinks()
{
    uint16_t linkMask = 0;
    Personality_t personality;
    Kernel_GetPersonality(&personality, sizeof(Personality_t));
    uint64_t flags  = personality.Network_Config.NetFlags;
    uint64_t flags2 = personality.Network_Config.NetFlags2;
    if (ND_GET_ENABLE_ION_LINK(flags)) linkMask |= UPC_NW_LINK_bit(10);
    linkMask |= (flags2 >> 48) & 0xFFC0;  // leave just the link enable bits - already correct order
    return linkMask;
}



//! \brief UPC_NW_CtrCtlMask
//! Generate NW Counter Control Mask from separate link and counter masks for
//! used in NW UPC counter controls
//! @param[in] linkMask  Mask of desired links - left aligned into 16 bits.
//! @param[in] ctrMask   Mask of desired counters (0-5) - left aligned into 16 bits.
//! @note
//! Counters 4 & 5 are controlled together via one bit.
//! This function can use both counter masks with 6 bits or 5 bits.
uint64_t UPC_NW_CtrCtlMask(uint16_t linkMask, uint16_t ctrMask);

// counter macros
#define UPC_NW_CTR_bit(b)         (1<<(15-(b)))     //!< get nw ctr mask from single ctr number
#define UPC_NW_ALL_SEND_CTRS      (0xF000)          //!< send chunk ctr ctrl
#define UPC_NW_ALL_RCV_CTRS       (0x0800)          //!< receive packet ctr ctrl
#define UPC_NW_ALL_CTRS           (0xF800)          //!< bit mask indicating all nw upc counter controls

// CtrCtlMask for all links, all counters
#define UPC_NW_ALL_LINKCTRS       (0xFFFFFFFFFFFFFE00ULL) //!< bit mask indicating all links and counters

#define UPC_NW_NUM_LINKS            11
#define UPC_NW_NUM_CTRS             ND_RESE_DCR__UPC_range



//! \brief: UPC_NW_GetCtrVChannel
//! Get single link counter virtual channels
//! @param[in] link      link number
//! @param[in] ctr       ctr (0-5)
//! @note This routine allows the counter to use to be variable, rather than a compile time parm
//! @note See UPC_NW_SetCtrVChannel for caveot on compute nodes
__INLINE__ uint8_t UPC_NW_GetCtrVChannel(unsigned link, unsigned ctr) {
    uint64_t val = DCRReadUser(ND_RESE_DCR(link, UPC_CTRL));
    if (ctr > 4) ctr = 4;
    val =  (val >> ((7 - ctr) * 8)) & 0x7F;
    return (uint8_t)val;
}




#define UPC_NW_PRINT_DEBUG(...) // printf(_AT_ __VA_ARGS__ )
//! \brief: UPC_NW_SetCtrVChannel
//! Set single link counter virtual channels
//! @param[in] link      link number
//! @param[in] ctr       ctr (0-5)
//! @param[in] vcMask    virtual Channel mask
//! @note On compute nodes, when the network links are configured as a mesh rather
//!       than torus, links which go off the mesh are head in reset.  This prevents
//!       us from successfully writing to the UPC_CTRL register for that link.
//!       It doesn't cause a failure - just has no effect.
//!       Counts read from these links will always return 0.
__INLINE__ void UPC_NW_SetCtrVChannel(unsigned link, unsigned ctr, uint8_t vcMask) {
    uint64_t val = DCRReadUser(ND_RESE_DCR((uint64_t)link, UPC_CTRL));
    UPC_NW_PRINT_DEBUG(" link=%02d, ctr=%d, vcMask=%02x, startMask=0x%016lx\n",
                       link, ctr, vcMask, val);

    if (ctr > 4) ctr = 4;

    // shift mask to ctr's position
    uint64_t mask = vcMask & 0x7F;
    mask <<= (7-ctr) * 8;  // 7 bit fields aligned to the right on every 8th bit.
    //printf(_AT_ " link=%02d, ctr=%d, vcMask=%02x, mask=0x%016lx, endMask=0x%016lx\n",
    //        link, ctr, vcMask, mask, val);

    // mask off existing ctr's bits.
    val &= ~(0x7FUL << ((7 - ctr) * 8));
    UPC_NW_PRINT_DEBUG(" link=%02d, ctr=%d, vcMask=%02x, mask=0x%016lx, endMask=0x%016lx\n",
            link, ctr, vcMask, mask, val);

    // or on new mask
    val |= mask;

    // write back
    UPC_NW_PRINT_DEBUG(" link=%02d, ctr=%d, vcMask=%02x, mask=0x%016lx, endMask=0x%016lx\n",
            link, ctr, vcMask, mask, val);

    DCRWriteCommon(ND_RESE_DCR(link, UPC_CTRL), val);

    #if 0
    unsigned retries=0;
    uint64_t chkVal = 0;
    while (retries<10) {
        DCRWriteUser(ND_RESE_DCR(link, UPC_CTRL), val);
        chkVal = DCRReadUser(ND_RESE_DCR((uint64_t)link, UPC_CTRL));
        if (chkVal == val) break;
        retries++;
    }
    if (chkVal != val) {
        UPC_NW_PRINT_DEBUG(" Mismatch : retries=%02d, link=%02d, ctr=%02d, wrote=0x%016lx, read=0x%016lx\n", retries, link, ctr, val, chkVal);
    }
    else {
        UPC_NW_PRINT_DEBUG(" Match    : retries=%02d, link=%02d, ctr=%02d, wrote=0x%016lx, read=0x%016lx\n", retries, link, ctr, val, chkVal);
    }
    #endif
}



//! \brief: UPC_NW_SetLinkCtrVChannel
//! Set Link counter virtual channel for multiple links
//! @param[in] linkMask  Mask of desired links - left aligned into 16 bits.
//! @param[in] ctr       ctr (0-5)
//! @param[in] vcMask    virtual Channel mask
void UPC_NW_SetLinkCtrVChannel(uint16_t linkMask, unsigned ctr, uint8_t vcMask);



//! \brief: UPC_NW_Clear
//! Clear set of network UPC counters.
//! @param[in] ctrCtlMask  nw control mask (use UPC_NW_CtrCtlMask to generate)
//! @note
//! - Masks used must be consistent with eventual higher level tracking and combining.
//! - only one sw thread should control nw counting - timing results indeterminant otherwise.
//! - start/stop operations must not be nested.
__INLINE__ void UPC_NW_Clear(uint64_t ctrCtlMask)
{
    uint64_t curCtl = DCRReadUser(ND_500_DCR(CTRL_UPC_CLEAR));
    DCRWriteCommon(ND_500_DCR(CTRL_UPC_CLEAR), (curCtl | ctrCtlMask) );
    DCRWriteCommon(ND_500_DCR(CTRL_UPC_CLEAR), (curCtl & ~ctrCtlMask) );
    mbar();
}



//! \brief: UPC_NW_Stop
//! Stop set of network UPC counters.
//! @param[in] ctrCtlMask  nw control mask (use UPC_NW_CtrCtlMask to generate)
//! @note
//! - Masks used must be consistent with eventual higher level tracking and combining.
//! - only one sw thread should control nw counting for each link - timing results indeterminant otherwise.
//! - start/stop operations must not be nested.
__INLINE__ void UPC_NW_Stop(uint64_t ctrCtlMask)
{
    uint64_t curCtl = DCRReadUser(ND_500_DCR(CTRL_UPC_STOP));
    DCRWriteCommon(ND_500_DCR(CTRL_UPC_STOP), (curCtl | ctrCtlMask) );
    mbar();
}



//! \brief: UPC_NW_Start
//! Start set of network UPC counters.
//! @param[in] ctrCtlMask  nw control mask (use UPC_NW_CtrCtlMask to generate)
//! @note
//! - Masks used must be consistent with eventual higher level tracking and combining.
//! - only one sw thread should control nw counting - timing results indeterminant otherwise.
//! - start/stop operations must not be nested.
__INLINE__ void UPC_NW_Start(uint64_t ctrCtlMask)
{
    uint64_t curCtl = DCRReadUser(ND_500_DCR(CTRL_UPC_STOP));
    DCRWriteCommon(ND_500_DCR(CTRL_UPC_STOP), (curCtl & ~ctrCtlMask) );
    mbar();
}



//! \brief: UPC_NW_EnableUPC
//! Enable Network units UPC operation
//! (there really is little to do to enable UPC in the network units.
//! function just added for consistency with other units.
//! @note
//! - Network unit operations independently of other UPC units and is
//!   independent of the UPC mode.
//! - NW counters must also be started and stopped independently of other units.
//! - Assumes counters configured prior to start if counter values are to have meaning.
__INLINE__ void UPC_NW_EnableUPC()
{
    UPC_NW_Stop(UPC_NW_ALL_LINKCTRS);
    UPC_NW_Clear(UPC_NW_ALL_LINKCTRS);
}



//! \brief: UPC_NW_ReadCtr
//! Read a NW counter from a particular link.
//! @param[in]  link       link number
//! @param[in]  ctr        ctr (0-5)
__INLINE__ uint64_t UPC_NW_ReadCtr(unsigned link, unsigned ctr)
{
    assert(link < ND_RESE_DCR_num);
    assert(ctr < ND_RESE_DCR__UPC_range);
    return DCRReadUser(ND_RESE_DCR(link, UPC) + ctr);
}



//! \brief: UPC_NW_Convert2NWCycles
//! convert the elapsed cpu cycles to network cycles.
//! \note
//! - results only meaningful when counters already stopped
__INLINE__ uint64_t UPC_NW_Convert2NWCycles(uint64_t cpuCycles)
{
    const Upci_Node_Parms_t * pNodeParms = Upci_GetNodeParms();
    return (cpuCycles * pNodeParms->nwSpeed) / pNodeParms->cpuSpeed;
}



//! \brief: UPC_NW_LinkCtrTotal
//! Read and Total counters given a link mask.
//! @param[in] linkMask  Mask of desired links - left aligned into 16 bits.
//! @param[in]  ctr        ctr (0-5)
uint64_t UPC_NW_LinkCtrTotal(uint16_t linkMask, unsigned ctr);



//! Network Counter Array
//! Structure to receive complete array of network counters for debug analysis.
//! Network counter access is slow (dcr bus ~ 500 cycles per read).
//! Thus, multiple calculations based on network counters will work faster from a memory copy.
typedef struct sUPC_NW_CtrArray {
    uint32_t nwSpeed;  // copy of nw cycles per sec from upcNodeParms.
    uint64_t elapsedNwCycles;
    uint64_t ctr[UPC_NW_NUM_LINKS][UPC_NW_NUM_CTRS];   // array - [linkidx][ctridx]
} UPC_NW_CtrArray_t;



//! UPC_NW_GetCtrArray
//! Read all NW counters into counter array
//! @param[in,out]  pCtrs
void UPC_NW_GetCtrArray(UPC_NW_CtrArray_t *pCtrs);



//! UPC_NW_DumpCtrArray
//! Dump out all the NW counter values from array to stdout
//! @param[in]  indent   indent output lines by given columns
//! @param[in]  o        ptr to ctr array
void UPC_NW_DumpCtrArray(unsigned indent, UPC_NW_CtrArray_t *o);



void UPC_NW_Dump();


__END_DECLS

#endif


