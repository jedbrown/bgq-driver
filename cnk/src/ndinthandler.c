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

#include "Kernel.h"
#include "flih.h"
#include <hwi/include/bqc/BIC_inlines.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/nd_rese_dcr.h>

uint64_t nd_allowed_non_fatal_err[2] = {0, 0};
uint64_t nd_seen_non_fatal_err[2] = {0, 0};

// buffer to hold fatal dump data. Must be 512 byte aligned
uint8_t mu_hdumparea[512] __attribute__ ((aligned ( 512))); 

typedef struct msg_intreg_t
{
    short isr_offset;
    short n_intbits;
    short n_intinfo;
    const char* regname;
} msg_intreg_t;

typedef struct msg_intbit_t
{
    short  bit_id;
    short  is_fatal;
    const char* msg;
} msg_intbit_t;

typedef struct msg_intinfo_t
{
    short    reg_offset;
    short    n_fields;
    uint64_t trigger_bits;
} msg_intinfo_t;

typedef struct msg_infofield_t
{
    short       start_bit;
    short       end_bit;
    const char* msg;
} msg_infofield_t;



#define MU_DCR_ISR_OFFSET(X) MU_DCR__ ## X ## __STATE_offset
#define MU_DCR_OFFSET(X) MU_DCR__ ## X ## _offset

#define MARK_END (-1)

// 
// DEF_INTREG(OFFSET, REGNAME) : interrupt information for decoding
//    offset of ISR, offset of the corresponding info reg, and bits that trigger info
// DEF_INTBIT(BITNO, IS_FATAL, MSG) : information of each interrupt bit
//    Bit ID, is_fatal flag,  and description of error
// DEF_INTINFO(INFO_OFFSET, TRIGGER_BITS) : information of intinfo
// 
// DEF_INFOFIELD(SBIT, EBIT, NAME) : information of a field in intinfo
//

#define MU_INTS \
    DEF_INTREG(FIFO_INTERRUPTS, "MU rmFIFO FULL",				\
	       _list(DEF_INTBIT(63, 0, "(software notification) There is insufficient space in an rmFIFO to receive the next packet.")), \
	       _list(DEF_INTINFO(RMFIFO, _BN(63),			\
				 _list(DEF_INFOFIELD(55, 63, "The ID of the rmFIFO that has insufficient space"))))), \
    DEF_INTREG(ICSRAM_INTERRUPTS, "MU ICSRAM",			\
	       _list(DEF_INTBIT(20, 1, "(software error) imFIFO - software attempted to write read-only free space"), \
		     DEF_INTBIT(21, 1, "(software error) imFIFO - start, head, or tail address are not 64B aligned, or size is not 64B-1 aligned," \
				"or atomic address was specified"),	\
		     DEF_INTBIT(22, 1, "(software error) imFIFO - start, head, tail or size are out of range (bits(0 to 26) are non-zero"), \
		     DEF_INTBIT(23, 0, "(software notification) - rget FIFO is full, rME is waiting for software to free up space in fifo")), \
	       _list(DEF_INTINFO(ICSRAM_RGET_FIFO_FULL, _BN(23),	\
				 _list(DEF_INFOFIELD(54, 63, "Remote get imFIFO ID which is full."))))), \
    DEF_INTREG(MASTER_PORT0_INTERRUPTS, "MU master port0", \
	       _list(DEF_INTBIT(24, 1, "(software error) MU Master port0 newreq_write_range_err - write request address range check violation"), \
		     DEF_INTBIT(25, 1, "(software error) MU Master port0 newreq_read_range_err - read request address range check violation")), \
	       _list(DEF_INTINFO(XM0_ERR_INFO, _BGQ_SET(2, 25, 3UL), \
				 _list(DEF_INFOFIELD(26, 26, "System request violated range check"), \
				       DEF_INFOFIELD(27, 63, "Address that violated range check"))))), \
    DEF_INTREG(MASTER_PORT1_INTERRUPTS, "MU master port1", \
	       _list(DEF_INTBIT(24, 1, "(software error) MU Master port1 newreq_write_range_err - write request address range check violation"), \
		     DEF_INTBIT(25, 1, "(software error) MU Master port1 newreq_read_range_err - read request address range check violation")), \
	       _list(DEF_INTINFO(XM1_ERR_INFO, _BGQ_SET(2, 25, 3UL), \
				 _list(DEF_INFOFIELD(26, 26, "System request violated range check"), \
				       DEF_INFOFIELD(27, 63, "Address that violated range check"))))), \
    DEF_INTREG(MASTER_PORT2_INTERRUPTS, "MU master port2", \
	       _list(DEF_INTBIT(24, 1, "(software error) MU Master port2 newreq_write_range_err - write request address range check violation"), \
		     DEF_INTBIT(25, 1, "(software error) MU Master port2 newreq_read_range_err - read request address range check violation")), \
	       _list(DEF_INTINFO(XM2_ERR_INFO, _BGQ_SET(2, 25, 3UL), \
				 _list(DEF_INFOFIELD(26, 26, "System request violated range check"), \
				       DEF_INFOFIELD(27, 63, "Address that violated range check"))))), \
    DEF_INTREG(MCSRAM_INTERRUPTS, "MU MCSRAM", \
	       _list(DEF_INTBIT(15, 1, "(software error) Remote get packet payload address is atomic"),  \
		     DEF_INTBIT(16, 1, "(software error) Payload size > 512B"), \
		     DEF_INTBIT(17, 1, "(software error) Payload size = 0 for R-put or payload size < 64B for R-get"), \
		     DEF_INTBIT(18, 1, "(software error) Payload size != 64N for R-get"), \
		     DEF_INTBIT(19, 1, "(software error) Payload size = 0 for non-zero message length"), \
		     DEF_INTBIT(20, 1, "(software error) Payload address starts in non-atomic area but ends in atomic area"), \
		     DEF_INTBIT(21, 1, "(software error) Message len != 64N for R-get"), \
		     DEF_INTBIT(22, 1, "(software error) Message len != 8 for atomic payload address"), \
		     DEF_INTBIT(23, 1, "(software error) Message len = 0 for R-put, or message len < 64B for R-get"), \
		     DEF_INTBIT(24, 1, "(software error) User imFIFO sending system VC packet (VC = 3,6)"), \
		     DEF_INTBIT(25, 1, "(software error) Invalid MU packet type (type = 11)"), \
		     DEF_INTBIT(26, 1, "(software error) User imFIFO specifying system iME in FIFO map"), \
		     DEF_INTBIT(27, 1, "(software error) FIFO map is 0 (i.e. specifying no iME)"), \
		     DEF_INTBIT(31, 1, "(software error DD2 only) Invalid VC (VC=7 is undefined)"), \
		     DEF_INTBIT(32, 1, "(software error DD2 only) Put offset wrapped (put offset + msg len overflows 36bit limit)")), \
	       _list(DEF_INTINFO(MCSRAM_ERR_FIFO_ID, _BGQ_SET(13, 25, 0x1fffUL) | _BGQ_SET(2, 31, 3UL), \
				 _list(DEF_INFOFIELD(54, 63, "imFIFO ID that caused error"))), \
		     DEF_INTINFO(IMFIFO, _BN(26), \
				 _list(DEF_INFOFIELD(54, 63, "imFIFO ID that caused error"))))), \
    DEF_INTREG(MISC_INTERRUPTS, "MU MISC", \
	       _list(DEF_INTBIT(3, 1, "(software error) rME requested system imFIFO for user R-get packet"), \
		     DEF_INTBIT(4, 1, "(software error) rME requested imFIFO whose rget flag is not set"), \
		     DEF_INTBIT(5, 1, "(software error) rME requested imFIFO not enabled by DCR")), \
	       _list(DEF_INTINFO(IMFIFO_ACCESS_ERROR_ID, _BGQ_SET(3, 5, 0x7UL), \
				 _list(DEF_INFOFIELD(54, 63, "The imFIFO ID that caused error"))))), \
    DEF_INTREG(MMREGS_INTERRUPTS, "MU MMREGS", \
	       _list(DEF_INTBIT(9, 1, "(software error) rd_imfifo_enable_err - software attempted to read the write-only enable imFIFO register"), \
		     DEF_INTBIT(10, 1, "(software error) rd_imfifo_disable_err - software attempted to read the write-only disable imFIFO register"), \
		     DEF_INTBIT(11, 1, "(software error) rd_fifo_int_clr_err - software attempted to read the write-only clear interrupt status register"), \
		     DEF_INTBIT(12, 1, "(software error) wr_imfifo_enable_status_err - software attempted to write the read-only imFIFO enable status register"), \
		     DEF_INTBIT(13, 1, "(software error) wr_fifo_int_status_err - software attempted to write the read-only interrupt status register"), \
		     DEF_INTBIT(14, 1, "(software error) imfifo_enable_prot_err - user attempted to enable or disable a system imFIFO"), \
		     DEF_INTBIT(15, 1, "(software error) fifo_int_clr_prot_err - user attempted to clear an interrupt corresponding to a system imFIFO or rmFIFO"), \
		     DEF_INTBIT(16, 1, "(software error) wr_range_err - software attempted to write (set) unused MMIO bits, either bits 0-31 in imFIFO enable/disable/high-priority, or bits 0-60 in global interrupt control"), \
		     DEF_INTBIT(17, 1, "(software error) wr_barriers_nd_err - software attempted to write the read-only global interrupt status register"), \
		     DEF_INTBIT(18, 1, "(software error) barrier_mu_prot_err - user attempted to write a system global interrupt control bit")), \
	       _list(DEF_INTINFO(MMREGS_FIFO_PROT_ERR, _BN(14)|_BN(15)|_BN(18),	\
				 _list(DEF_INFOFIELD(53, 63, "The internal mmregs index of the offending write. Bits 0-4 are the group for FIFO protection errors. Bits 7-10 are the class for barrier protection errors."))))), \
    DEF_INTREG(RCSRAM_INTERRUPTS, "MU RCSRAM", \
	       _list(DEF_INTBIT(12, 1, "(software error) RCSRAM user packet getting tail of system rmFIFO"), 				\
		     DEF_INTBIT(13, 1, "(software error) RCSRAM get-tail request index out of bound (>= 272)"), 				\
		     DEF_INTBIT(14, 1, "(software error) RCSRAM requested rmFIFO not enabled by DCR register"), 				\
		     DEF_INTBIT(15, 1, "(software error) RCSRAM write data from slave not aligned to 64B or greater than 2**36-1")), \
	       _list(DEF_INTINFO(RMFIFO_ACCESS_ERROR_ID, _BGQ_SET(4, 15, 0xfUL), \
				 _list(DEF_INFOFIELD(55, 63, "The rmFIFO ID on which the software error happened"))))), \
    DEF_INTREG(RPUTSRAM_INTERRUPTS, "MU RPUTSRAM", \
	       _list(DEF_INTBIT(8,  1, "(software error) RPUT SRAM user packet accessing put base but it is system entry"), \
		     DEF_INTBIT(9,  1, "(software error) RPUT SRAM user packet accessing counter base but it is system entry"), \
		     DEF_INTBIT(10, 1, "(software error) RPUT SRAM rput/counter address addition overflow"), \
		     DEF_INTBIT(11, 1, "(software error) RPUT SRAM rput destination area crossing atomic/nonatomic boundary"), \
		     DEF_INTBIT(12, 1, "(software error) RPUT SRAM data size != 8 for atomic rput address"), \
		     DEF_INTBIT(13, 1, "(software error) RPUT SRAM rput counter address in non-atomic")), \
	       _list(DEF_INTINFO(RMFIFO_ACCESS_ERROR_ID, 0, _list(DEF_INFOFIELD(0, 0, NULL))))) /* dummy */,  \
    DEF_INTREG(RME_INTERRUPTS0, "MU rME 0-7", \
	       _list(DEF_INTBIT(5, 1,  "(software error) rME0 packet header field error"), \
		     DEF_INTBIT(11, 1, "(software error) rME1 packet header field error"), \
		     DEF_INTBIT(17, 1, "(software error) rME2 packet header field error"), \
		     DEF_INTBIT(23, 1, "(software error) rME3 packet header field error"), \
		     DEF_INTBIT(29, 1, "(software error) rME4 packet header field error"), \
		     DEF_INTBIT(35, 1, "(software error) rME5 packet header field error"), \
		     DEF_INTBIT(41, 1, "(software error) rME6 packet header field error"), \
		     DEF_INTBIT(47, 1, "(software error) rME7 packet header field error")), \
	       _list(DEF_INTINFO(RME_HEADER_ERR0, _BN(5),  \
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR1, _BN(11),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR2, _BN(17),    	\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR3, _BN(23),    	\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR4, _BN(29),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR5, _BN(35),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR6, _BN(41),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR7, _BN(47),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))))), \
    DEF_INTREG(RME_INTERRUPTS1, "MU rME 8-15",				\
	       _list(DEF_INTBIT(4, 1,  "(software error) rME8 packet header field error"), \
		     DEF_INTBIT(10, 1, "(software error) rME9 packet header field error"), \
		     DEF_INTBIT(16, 1, "(software error) rME10 packet header field error"), \
		     DEF_INTBIT(22, 1, "(software error) rME11 packet header field error"), \
		     DEF_INTBIT(28, 1, "(software error) rME12 packet header field error"), \
		     DEF_INTBIT(34, 1, "(software error) rME13 packet header field error"), \
		     DEF_INTBIT(40, 1, "(software error) rME14 packet header field error"), \
		     DEF_INTBIT(46, 1, "(software error) rME15 packet header field error")), \
	       _list(DEF_INTINFO(RME_HEADER_ERR0, _BN(4),  \
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR1, _BN(10),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR2, _BN(16),    	\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR3, _BN(22),    	\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR4, _BN(28),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR5, _BN(34),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR6, _BN(40),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))), \
		     DEF_INTINFO(RME_HEADER_ERR7, _BN(46),		\
				 _list(DEF_INFOFIELD(57, 63, "Header error bit map"), \
				       DEF_INFOFIELD(60, 60, "Sram index overflow flag (>543 for rget/rput, >271 for memory FIFO)"), \
				       DEF_INFOFIELD(61, 61, "A system rME received a packet that has user VC (i.e. VC !=3,6)"), \
				       DEF_INFOFIELD(62, 62, "remote put counter base address ID too large (>543) for remote put packet"))))), \
    DEF_INTREG(SLAVE_PORT_INTERRUPTS, "MU slave port",			\
	       _list(DEF_INTBIT(21, 1, "(software error) bad_op_err - unsupported opcode (ttype) sent to MU slave, see slave error info register"), \
		     DEF_INTBIT(22, 1,"(software error) rd_size_err - read request size is not 8B, see slave error info register" ), \
		     DEF_INTBIT(23, 1, "(software error) byte_valid_err - more than 8B of byte valids asserted OR byte valids inconsistent with odd/even address"), \
		     DEF_INTBIT(24, 1, "(software error) addr_range_err - request's address lies outside MU MMIO address space, see slave error info register"), \
		     DEF_INTBIT(25, 1, "(software error) icsram_perm_err - user is attempting to access a system imFIFO"), \
		     DEF_INTBIT(26, 1, "(software error) rcsram_perm_err - user is attempting to access a system rmFIFO"), \
		     DEF_INTBIT(27, 1, "(software error) rputsram_perm_err - user is attempting to access a system rput base address")),	\
	       _list(DEF_INTINFO(XS_ERR_INFO, _BGQ_SET(11, 10, 0x3ffUL) | _BGQ_SET(7, 27, 0x7fUL), \
				 _list(DEF_INFOFIELD(14, 18, "ID of core/master that issued request"), \
				       DEF_INFOFIELD(19, 20, "ID of thread that issued request"), \
				       DEF_INFOFIELD(21, 54, "Address of request"), \
				       DEF_INFOFIELD(55, 60, "Ttype of request"), \
				       DEF_INFOFIELD(61, 63, "Size of load request"))))) \
      


// define intreg struct
#define _list(...)      __VA_ARGS__
#define LIST_LEN(...)   (sizeof((char[]){'\0', __VA_ARGS__}) - 1)

#define DEF_INTBIT(BITNO, IS_FATAL, MSG) '\0'
#define DEF_INTINFO(INFO_OFFSET, TRIGGER_BITS, INFOFIELDS) '\0'
#define DEF_INTREG(OFFSET, REGNAME, INTBITS, INTINFO) { MU_DCR_ISR_OFFSET(OFFSET), LIST_LEN(INTBITS), LIST_LEN(INTINFO), REGNAME }
static const msg_intreg_t msg_intreg[] = { MU_INTS };
#undef DEF_INTBIT
#undef DEF_INTINFO
#undef DEF_INTREG

// define intbit struct

#define DEF_INTREG(OFFSET, REGNAME, INTBITS, INTINFO) INTBITS
#define DEF_INTBIT(BITNO, IS_FATAL, MSG) { BITNO, IS_FATAL, MSG }
static const msg_intbit_t msg_intbit[] = { MU_INTS};
#undef DEF_INTREG
#undef DEF_INTBIT

// define intinfo struct
#define DEF_INFOFIELD(SBIT, EBIT, NAME) '\0'
#define DEF_INTREG(OFFSET, REGNAME, INTBITS, INTINFO) INTINFO
#define DEF_INTINFO(INFO_OFFSET, TRIGGER_BITS, INFOFIELDS) {MU_DCR_OFFSET(INFO_OFFSET), LIST_LEN(INFOFIELDS), TRIGGER_BITS }
static const msg_intinfo_t msg_intinfo[] = { MU_INTS};
#undef DEF_INTREG
#undef DEF_INTINFO
#undef DEF_INFOFIELD

// define infofield struct
#define DEF_INFOFIELD(SBIT, EBIT, NAME) {SBIT, EBIT, NAME }
#define DEF_INTREG(OFFSET, REGNAME, INTBITS, INTINFO) INTINFO
#define DEF_INTINFO(INFO_OFFSET, TRIGGER_BITS, INFOFIELDS) INFOFIELDS
static const msg_infofield_t msg_infofield[] = { MU_INTS };
#undef DEF_INFOFIELD
#undef DEF_INTREG
#undef DEF_INTINFO

static void dump_rme_headers(CoreBuffer *buffer)
{
    uint u;
    uint64_t* rme_header_buffer = (uint64_t*)mu_hdumparea; // point to the 512B header dump buffer area

    coredump_printf(buffer, "    Printing rME bad packet header dump buffer @ %p\n", rme_header_buffer);
    _bgq_msync();
    for (u = 0; u < 16; u++)
    {
        uint64_t* hdr = &rme_header_buffer[u * 4];
        coredump_printf(buffer, "    rME%u\t%016lx %016lx %016lx %016lx\n",
                        u, hdr[0], hdr[1], hdr[2], hdr[3]);
    }
}

static void mask_interrupt(uint64_t addr, uint64_t bits)
{
    int i;
    uint64_t lconfig = DCRReadPriv(addr+1);
    uint64_t hconfig = DCRReadPriv(addr+2);
    int ldiff = 0;
    int hdiff = 0;

    for (i = 0; i < 32; i++)
    {
        int lbit = i + 32;
        int hbit = i;
        if (_BN(lbit) & bits)
        {
            lconfig &= ~_BGQ_SET(2, i*2+1, 3UL);
            ldiff = 1;
        }
        if (_BN(hbit) & bits)
        {
            hconfig &= ~_BGQ_SET(2, i*2+1, 3UL);
            hdiff = 1;
        }
    }

    if (ldiff) DCRWritePriv(addr+1, lconfig);
    if (hdiff) DCRWritePriv(addr+2, hconfig);
}

static int decode_intbits(CoreBuffer *buffer, uint64_t addr, uint64_t regval, int ib_idx, int n_intbits)
{
    uint64_t unknown_bits = regval;
    int i;
    int is_fatal = 0;

    for (i = 0; i < n_intbits; i++)
    {
        const msg_intbit_t *mib = &msg_intbit[i+ib_idx];
        uint64_t bit = _BN(mib->bit_id);
        unknown_bits &= ~bit;
        // printf("checking bit %d (%s)\n", mib->bit_id, mib->msg);
        if (regval & bit)
        {
            if (buffer)
            {
                coredump_printf(buffer, "      Bit %d asserted : %s\n", mib->bit_id, mib->msg);
            }
            is_fatal |= mib->is_fatal;
        }
    }
    if (unknown_bits)
    {
        if (buffer)
        {
            coredump_printf(buffer, "      Some unkown bits are set : %016lx -- can't decode.\n", unknown_bits);
        }
        is_fatal = 1;
    }
    return is_fatal;
}

static int decode_infofield(CoreBuffer *buffer, uint64_t infoval, int if_idx, int n_infofield)
{
    int i;
    for (i = 0; i < n_infofield; i++)
    {
        const msg_infofield_t* mif = &msg_infofield[i+if_idx];
        int sb = mif->start_bit;
        int eb = mif->end_bit;

        uint64_t fval = _BGQ_GET(eb-sb+1, eb, infoval);
        if (buffer)
        {
            coredump_printf(buffer, "      Intinfo(%s) : %016lx", mif->msg, fval);
        }
    }
    return 0;
}

static int decode_intinfo(CoreBuffer *buffer, uint64_t regval, int ii_idx, int* p_if_idx, int n_intinfo)
{
    int i;
    int is_fatal = 0;
    int if_idx = *p_if_idx;

    for (i = 0; i < n_intinfo; i++)
    {
        const msg_intinfo_t *mii = &msg_intinfo[i+ii_idx];
        uint64_t trigger_bits = mii->trigger_bits;
        int n_infofield = mii->n_fields;

        if (regval & trigger_bits)
        {
            uint64_t infoval = DCRReadPriv(MU_DCR_base + mii->reg_offset);
            is_fatal |= decode_infofield(buffer, infoval, if_idx, n_infofield);
        }
        if_idx += n_infofield;
    }
    *p_if_idx = if_idx;
    return is_fatal;
}

static int decode_mu_nocrit_interrupts(CoreBuffer *buffer)
{
    int is_fatal = 0;
    uint32_t i,j;
    int ib_idx = 0;
    int ii_idx = 0;
    int if_idx = 0;

    // walk through DCR int regs
    for (i = 0; i < sizeof(msg_intreg)/sizeof(msg_intreg[0]); i++)
    {
        const msg_intreg_t *mir = &msg_intreg[i];
        uint64_t addr = MU_DCR_base + mir->isr_offset;
        int n_intbits = mir->n_intbits;
        int n_intinfo = mir->n_intinfo;
        const char* regname = mir->regname;
        uint64_t regval = DCRReadPriv(addr);

        if (regval && buffer)
        {
            coredump_printf(buffer, "    MU int reg %lx (%s) = %016lx\n", addr, regname, regval);
        }
        /* decode intbits*/
        if (regval)
            is_fatal |= decode_intbits(buffer, addr, regval, ib_idx, n_intbits);
        ib_idx += n_intbits;

        // decode intinfo
        is_fatal |= decode_intinfo(buffer, regval, ii_idx, &if_idx, n_intinfo);
        ii_idx += n_intinfo;

        // if the interrupt was not fatal, disable it and return
        if (regval && !is_fatal)
        {
            if (!buffer) // only do the following when we are running in the app, not during the coredump processing
            {
                // Generate a flight recorder entry that a non-fatal MU interrupt occurred.
                Kernel_WriteFlightLog(FLIGHTLOG, FL_NDINTALMU, addr, regval,0,0);

                // Bump the node scoped performance counter
                CountNodeEvent(CNK_NODEPERFCOUNT_MU);

                // Since the interrupt was not fatal, trying to mask it so we dont see it again
                mask_interrupt(addr, regval);

                for (j=0; j<CONFIG_MAX_APP_PROCESSES; j++)
                {
                    AppProcess_t *proc = &NodeState.AppProcess[j];
                    if (proc && proc->State == ProcessState_Started)
                    {
                        // Send signal to this process.
                        Signal_Deliver(proc, 0, SIGMUFIFOFULL);
                    }
                }
            }
        }
    }
    return is_fatal;
}

Lock_Atomic_t lock_nd_dcr;


static int decode_nd_int1(CoreBuffer *buffer) 
{
    int i;
    int do_dump = 0;
    int is_fatal = 0;

    for (i = 0; i < 2; i++)
    {
        uint64_t addr = ND_500_DCR(NON_FATAL_ERR0) + i;
        uint64_t regval = DCRReadPriv(addr);
        uint64_t allowed = nd_allowed_non_fatal_err[i];
        if (regval != 0)
        {
            if ((~allowed & regval) == 0) // only allowed bits set
            {
                nd_seen_non_fatal_err[i] |= regval;
                if (!buffer)  // only do the following when we are running in the job; not during the coredump processing
                {
                    Kernel_Lock(&lock_nd_dcr);
                    if (i == 0) DCRWritePriv(ND_500_DCR(CTRL_CLEAR0), regval); // clear the interrupt

                    // Generate a flight recorder entry that an allowed non-fatal interrupt condition has occurred.
                    Kernel_WriteFlightLog(FLIGHTLOG, FL_NDINTALND, addr, regval, nd_seen_non_fatal_err[i],0);

                    // Bump the node scoped performance counter
                    CountNodeEvent(CNK_NODEPERFCOUNT_ND);

                    Kernel_Unlock(&lock_nd_dcr);
                }
            }
            else
            {
                if (buffer)
                {
                    coredump_printf(buffer, "    ND_500_DCR non_fatal_err%d is nonzero : %016lx and not all bits are in allowed_non_fatal_err[%d]=%016lx\n", i,  regval, i, allowed);
                }
                do_dump = 1;
                is_fatal = 1;
            }
        }
    }
    if (do_dump && buffer)
    {
        for (i = 4; i < 8; i++)
        {
            coredump_printf(buffer, "    ND_500_DCR stat%d = %016lx\n", i, DCRReadPriv(ND_500_DCR(STAT0)+i));
        }
    }

    for (i = 0; i < 11; i++)
    {
        uint64_t addr =  ND_RESE_DCR(i, FATAL_ERR);
        uint64_t regval = DCRReadPriv(addr);
        if (regval != 0)
        {
            if (buffer)
            {
                coredump_printf(buffer, "    ND_RESE(%d)_DCR FATAL_ERROR is nonzero : %016lx\n", i, regval);
            }
            is_fatal = 1;
        }
    }
    return is_fatal;
}


void IntHandler_ND(int status_reg, int bitnum)
{
    // Determine if this is as fatal condition. If it is not, generate flight recorder entries, mask the interrrupt,
    // and return.
    int fatal;
    fatal = decode_nd_int1((CoreBuffer*)NULL);  // pass null core buffer pointer to indicate this is run-time processing.
    fatal |= decode_mu_nocrit_interrupts((CoreBuffer*)NULL); // pass null core buffer pointer to indicate this is run-time processing.
    if (!fatal)
    {
        return; // The previously called functions have reset any interrupt conditions and prepared for us to continue.
    }
    // If we reached this point, we are either going to terminate the job with a signal or terminate the node/kernel with RAS.

    // Test to see if a job is active on the node. If it is, we will report this condition as a signal, assuming
    // that this condition was cause by the application software.

    // Find all active processes and send signals to them.     
    int i;
    int signal_sent = 0;

    for (i=0; i<CONFIG_MAX_APP_PROCESSES; i++)
    {
        AppProcess_t *proc = &NodeState.AppProcess[i];
        if (proc && proc->State == ProcessState_Started)
        {
            // Send signal to this process.
            Signal_Deliver(proc, 0, SIGMUNDFATAL);
            signal_sent++;
        }
    }
    if (signal_sent)
    {
        // The above signal is a terminating condition, however to complete job termination we must
        // disable these ND/MU interrupt from occurring during the exit flows since external interrupts must be 
        // re-enabled to complete the exit. We do not want to disturb the interrupt status bits in the 
        // MU or GEA since we want the core dump code to properly decode and output the error condition, therfore
        // we shut of the interupt in the PUEA. The PUEA will be reinitialized at the very end of job termination
        nd_disablemund();
    }
    else
    {
        // Job is not active. Consider this a problem in system software and report this condition as a RAS event
        // We will not get the decoded information, however the interesting raw registers will be dumped
        RASBEGIN(26);
        RASPUSH(DCRReadPriv(ND_500_DCR(NON_FATAL_ERR0)));
        RASPUSH(DCRReadPriv(ND_500_DCR(NON_FATAL_ERR1)));
        for (i=0; i<11; i++)
        {
            RASPUSH(DCRReadPriv(ND_RESE_DCR(i, FATAL_ERR)));
        }
        for (i=0; i<13; i++)
        {
            uint64_t addr = MU_DCR_base + msg_intreg[i].isr_offset;
            RASPUSH(DCRReadPriv(addr));
        }
        RASFINAL(RAS_KERNELFATALMUND);
        Kernel_Crash(RAS_KERNELFATALMUND);
    }
}

void coredump_ND(CoreBuffer* buffer)
{
    uint64_t st0 = BIC_ReadGeaInterruptStatus(0);
    uint64_t st1 = BIC_ReadGeaInterruptStatus(1);
    uint64_t st2 = BIC_ReadGeaInterruptStatus(2);
    uint64_t expected_st1_bit = GEA_DCR__GEA_INTERRUPT_STATUS1__MU_INT_set(1);
    uint64_t expected_st2_bit = GEA_DCR__GEA_INTERRUPT_STATUS2__ND_INT_set(1);

    // Core dump print start section +++ for ND MU error info
    coredump_printf(buffer, "MU / ND Fatal Error\n");

    if (st0 != 0)
    {
        coredump_printf(buffer, "    GEA status0 is nonzero: %lx, can't decode the source.\n", st0);
    }

    if (st1 & ~expected_st1_bit)
    {
        coredump_printf(buffer, "    Some GEA status1 bits are set, besides MU no-crit interrupt. pat= %lx, can't decode the source.\n", st1);
    }

    if (st2 & ~expected_st2_bit)
    {
        coredump_printf(buffer, "    Some GEA status2 bits are set, besites ND interrupt #1. pat = %lx, can't decode the source.\n", st2);
    }

    // decode MU interrupts
    if (st1 & expected_st1_bit)
    {
        decode_mu_nocrit_interrupts(buffer);
    }
    // decode ND interrupts
    if (st2 & expected_st2_bit)
    {
        decode_nd_int1(buffer);
    }
    dump_rme_headers(buffer);
}

void nd_interrupt_init()  
{
    // Mask the following conditions 
    nd_allowed_non_fatal_err[0] = _BN(ND_500_DCR__NON_FATAL_ERR0__COLL_DOWNTREE_GRANT_TIMEOUT_position) |
                                  _BN(ND_500_DCR__NON_FATAL_ERR0__RCP_C0_OVERFLOW_SET_position) |
                                  _BN(ND_500_DCR__NON_FATAL_ERR0__RCP_C0_NAN_SET_position) |
                                  _BN(ND_500_DCR__NON_FATAL_ERR0__COLL_NAN_GENERATED_position) |
                                  _BN(ND_500_DCR__NON_FATAL_ERR0__COLL_OVERFLOW_position) |
                                  _BN(ND_500_DCR__NON_FATAL_ERR0__RCP_C1_OVERFLOW_SET_position) |
                                  _BN(ND_500_DCR__NON_FATAL_ERR0__RCP_C1_NAN_SET_position); 
    int i;
    for (i = 0; i < 2; i++)
    {
        DCRWritePriv(ND_500_DCR(NON_FATAL_ERR_ENABLE)+i, ~0UL);
    }
    memset(mu_hdumparea, 0, 512); // clear the buffer
    // set up MU DCR regarding header dump
    DCRWritePriv(MU_DCR(RME_HDUMP),
                 MU_DCR__RME_HDUMP__EN_set(1) |
                 MU_DCR__RME_HDUMP__BASE_set(((uint64_t)mu_hdumparea) >> 9));
}



