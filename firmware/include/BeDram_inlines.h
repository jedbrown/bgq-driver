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

#ifndef _BEDRAM_INLINES_H_ // Prevent multiple inclusion.
#define _BEDRAM_INLINES_H_

#include <hwi/include/common/compiler_support.h>
#include <hwi/include/common/bgq_bitnumbers.h>

__BEGIN_DECLS

//
// These inlines use the privileged address range so are all for Kernel use only!
//
//\Warning: If/when needed, caller must provide any necessary mbar's or msync's.
//\Note: An SPI may be provided for User Access...
//
#if defined(__KERNEL__) && !defined(__ASSEMBLY__)

#include <sys/types.h>
#include <stdint.h>

#include <hwi/include/bqc/BeDram.h>
#include <firmware/include/VirtualMap.h>

typedef volatile uint64_t BeDRAM_Lock_t;

//
// Reserve/Allocate some locks
//
#define BeDRAM_LOCKNUM_BIG_STACK_IN_USE        (16) // The "big stack" is currently in use
#define BeDRAM_LOCKNUM_INST_VEC0_IN_PROGRESS   (17) // Indicates that some thread is installing an interrupt vector in domain 0
#define BeDRAM_LOCKNUM_FW_THREADS_PRESENT      (18) // Each H/W Thread Reports in.
#define BeDRAM_LOCKNUM_PRESENT                 BeDRAM_LOCKNUM_FW_THREADS_PRESENT // For backward compatibility @deprecated
#define BeDRAM_LOCKNUM_EXIT                    (19) // "Crash" terminates immediately, but "exit" barriers till PRESENT==EXIT.
#define BeDRAM_LOCKNUM_KERNEL_INIT             (20) // All HW Threads block until CNK initialization done by InitCore:0
#define BeDRAM_LOCKNUM_MSG_BOX                 (21) // A lock for the inter-domain message box
#define BeDRAM_LOCKNUM_INST_VEC1_IN_PROGRESS   (22) // Indicates that some thread is install an interrupt vector in domain 1
#define BeDRAM_LOCKNUM_REPROBARRIER            (23) // Cycle Reproducibility Barrier
#define BeDRAM_LOCKNUM_REPROCOUNTER            (24) // Cycle Reproducibility Reset Counter
#define BeDRAM_LOCKNUM_RAS_HISTORY             (25) // RAS storm filter lock
#define BeDRAM_LOCKNUM_CS_BARRIER              (26) // Control system barrier
#define BeDRAM_LOCKNUM_DDR_FIFOS_FLUSHED       (27) // DD1 workaoround to flush all FIFOs
#define BeDRAM_LOCKNUM_TAKE_CPU_READY          (28) // Ensures takeCPU is ready
#define BeDRAM_LOCKNUM_MBOX_INIT               (29) // Indicates that the mailbox is initialized
#define BeDRAM_LOCKNUM_MC_PENDING              (30) // Atomic counter of pending machine checks
#define BeDRAM_LOCKNUM_FW_THREADS_INITD        (31) // The number of firmware threads that have completed initialization
#define BeDRAM_LOCKNUM_RAS_FLUSH_LOCK          (32) // Used to synchronized flushing of accumulated events
#define BeDRAM_LOCKNUM_EXIT_STATUS_GATE        (33) // <not currently used>
#define BeDRAM_LOCKNUM_MBOX_FULL               (34) // Used to track mailbox full conditions.
#define BeDRAM_LOCKNUM_MBOX_OPEN_AGAIN         (35) // Used to track mailbox full conditions.
#define BeDRAM_LOCKNUM__NOT_USED_36            (36) // <not currently used>
#define BeDRAM_LOCKNUM__NOT_USED_37            (37) // <not currently used>
#define BEDRAM_LOCKNUM__NOT_USED_38            (38) // <not currently used>
#define BeDRAM_LOCKNUM__NOT_USED_39            (39) // <not currently used>
#define BeDRAM_LOCKNUM_COREINIT0               (40)
#define BeDRAM_LOCKNUM_COREINIT(c)             (BeDRAM_LOCKNUM_COREINIT0 + (c)) // Locks 40-56
#define BeDRAM_LOCKNUM_MBOX_TICKET             (57)
#define BeDRAM_LOCKNUM_JTAG_LOAD_COMPLETE      (58)
#define BeDRAM_LOCKNUM_MALLOCLOCK              (59)
#define BeDRAM_LOCKNUM_TAKECPU                 (60)
#define BeDRAM_LOCKNUM_DDRINIT                 (61)
#define BeDRAM_LOCKNUM_TAKECPU_SIG0            (62)
#define BeDRAM_LOCKNUM_TAKECPU_SIG(c)          (BeDRAM_LOCKNUM_TAKECPU_SIG0 + (c)) // Locks 62-78
#define BeDRAM_LOCKNUM_MBOX_SERVE              (79)
#define BeDRAM_LOCKNUM_TIMESYNC_BARRIER        (80)
#define BeDRAM_LOCKNUM_FWEXT_FIRST             (96)  // Slots reserved for fw extensions
#define BeDRAM_LOCKNUM_FWEXT_LAST              (127)

#define BeDRAM_LOCK_COUNT 128            // The number of locks supported in BeDRAM
//
// BeDRAM Locks Functional Interface
//
/*!
 \brief Write value from BeDRAM
 \note Non-Atomic
 */
__INLINE__ void BeDRAM_Write( size_t lknum, uint64_t value )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_WRITE);

  *pLK = value;
}

/*!
 \brief Read value from BeDRAM
 \note Non-Atomic
 */
__INLINE__ uint64_t BeDRAM_Read( size_t lknum )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READ);

   return *pLK;
}

/*!
 \brief Increment BeDRAM by 'value'
 \note Atomic
 */
__INLINE__ void BeDRAM_WriteAdd( size_t lknum, uint64_t value )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_WRITEADD);

  *pLK = value;
}

/*!
 \brief Read memory location and atomically clear
 \note Atomic
 */
__INLINE__ uint64_t BeDRAM_ReadClear( size_t lknum )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READCLEAR);

   return *pLK;
}

/*!
 \brief Read memory location and atomically increment (value saturates at 0xffffffffffffffff)
 \note Atomic
 */
__INLINE__ uint64_t BeDRAM_ReadIncSat( size_t lknum )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READINCSAT);

   return *pLK;
}

/*!
 \brief Read memory location and atomically increment (value wraps at 0xffffffffffffffff)
 \note Atomic
 */
__INLINE__ uint64_t BeDRAM_ReadIncWrap( size_t lknum )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READINCWRAP);

   return *pLK;
}

/*!
 \brief Read memory location and atomically decrement (value floors at 0x0)
 \note Atomic
 */
__INLINE__ uint64_t BeDRAM_ReadDecSat( size_t lknum )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READDECSAT);

   return *pLK;
}

/*!
 \brief Read memory location and atomically decrement (value wraps at 0x0)
 \note Atomic
 */
__INLINE__ uint64_t BeDRAM_ReadDecWrap( size_t lknum )
{
  BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
					 VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READDECWRAP);

   return *pLK;
}

/*!
 \brief Write without ECC correction  (priv area only)
 */
__INLINE__ void BeDRAM_WriteRaw( size_t lknum, uint64_t value )
{
    BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
                                           VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_WRITERAW);
    
    *pLK = value;
}

/*!
 \brief Read without ECC correction  (priv area only)
 */
__INLINE__ uint64_t BeDRAM_ReadRaw( size_t lknum )
{
    BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
                                           VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READRAW);
    
    return *pLK;
}

/*!
 \brief Write ECC to BeDRAM  (priv area only)
 */
__INLINE__ void BeDRAM_WriteECC( size_t lknum, uint64_t value )
{
    BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
                                           VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_WRITEECC);
    
    *pLK = value;
}

/*!
 \brief Read ECC from BeDRAM  (priv area only)
 */
__INLINE__ uint64_t BeDRAM_ReadECC( size_t lknum )
{
    BeDRAM_Lock_t *pLK = (BeDRAM_Lock_t *)(VA_MINADDR_BEDRAM_ATOMICOPS |
                                           VA_PRIVILEGEDOFFSET | 
                                           BeDRAM_LockOffset(lknum) |
 					   BeDRAM_HWTIndex() | 
                                           BeDRAM_READECC);
    
    return *pLK;
}

/*
 * Pointer-based BeDRAM atomic access.
 */

/*!
 * \brief Convert a pointer to the equivalent BeDRAM atomic-op pointer
 *
 * Internal utility function.
 *
 * \param[in]  ptr  Storage location to be operated on.
 * \param[in]  op   Operation to be performed.
 */
__INLINE__ volatile uint64_t *__bedram_op_ptr(volatile uint64_t *ptr,
					      const int op)
{
    int lknum = (((uint64_t) ptr) - (VA_MINADDR_BEDRAM|VA_PRIVILEGEDOFFSET))/8;
    
    return (volatile uint64_t *) ((VA_MINADDR_BEDRAM_ATOMICOPS |
							VA_PRIVILEGEDOFFSET) |
				  BeDRAM_LockOffset(lknum) |
				  BeDRAM_HWTIndex() |
				  op);
}

/*!
 * \brief Perform a load-based BeDRAM atomic operation
 *
 * Internal utility function.
 *
 * \param[in]  ptr  Storage location to be operated on.
 * \param[in]  op   Operation to be performed.
 */
__INLINE__ uint64_t __bedram_load_op(volatile uint64_t *ptr, const int op)
{
    return *__bedram_op_ptr(ptr, op);
}

/*!
 * \brief Perform a store-based BeDRAM atomic operation
 *
 * Internal utility function.
 *
 * \param[in]  ptr  Storage location to be operated on.
 * \param[in]  op   Operation to be performed.
 */
__INLINE__ void __bedram_store_op(volatile uint64_t *ptr, uint64_t value,
				  const int op)
{
    *__bedram_op_ptr(ptr, op) = value;
}

/*!
 * \brief Perform a BeDRAM atomic Read operation
 *
 * Fetch current value from storage location.
 *
 * \param[in]  ptr  Storage location to be fetched.
 */
__INLINE__ uint64_t BeDRAM_AtomicRead(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READ);
}

/*!
 * \brief Perform a BeDRAM atomic Read Clear operation
 *
 * Fetch current value and clear storage location.
 *
 * \param[in]  ptr  Storage location to be fetched and cleared.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadClear(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READCLEAR);
}

/*!
 * \brief Perform a BeDRAM atomic Read Increment Saturate operation
 *
 * Fetch current value and increment storage location, with saturation.
 *
 * \param[in]  ptr  Storage location to be fetched and incremented.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadIncrementSaturate(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READINCSAT);
}

/*!
 * \brief Perform a BeDRAM atomic Read Increment Wrap operation
 *
 * Fetch current value and increment storage location, with wrap.
 *
 * \param[in]  ptr  Storage location to be fetched and incremented.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadIncrementWrap(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READINCWRAP);
}

/*!
 * \brief Perform a BeDRAM atomic Read Decrement Saturate operation
 *
 * Fetch current value and decrement storage location, with saturation.
 *
 * \param[in]  ptr  Storage location to be fetched and decremented.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadDecrementSaturate(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READDECSAT);
}

/*!
 * \brief Perform a BeDRAM atomic Read Decrement Wrap operation
 *
 * Fetch current value and decrement storage location, with wrap.
 *
 * \param[in]  ptr  Storage location to be fetched and decremented.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadDecrementWrap(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READDECWRAP);
}

/*!
 * \brief Perform a BeDRAM atomic Read Raw operation
 *
 * Fetch current value without ECC correction.
 *
 * \param[in]  ptr  Storage location to be fetched.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadRaw(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READRAW);
}

/*!
 * \brief Perform a BeDRAM atomic Read ECC operation
 *
 * Fetch ECC information.
 *
 * \param[in]  ptr  Storage location whose ECC information is to be fetched.
 */
__INLINE__ uint64_t BeDRAM_AtomicReadECC(volatile uint64_t *ptr)
{
    return __bedram_load_op(ptr, BeDRAM_READECC);
}


/*!
 * \brief Perform a BeDRAM atomic Write operation
 *
 * Write value to storage location.
 *
 * \param[in]  ptr    Storage location to be written.
 * \param[in]  value  Value to be stored.
 */
__INLINE__ void BeDRAM_AtomicWrite(volatile uint64_t *ptr, uint64_t value)
{
    __bedram_store_op(ptr, value, BeDRAM_WRITE);
}

/*!
 * \brief Perform a BeDRAM atomic Write Add operation
 *
 * Add value to storage location.
 *
 * \param[in]  ptr    Storage location to be modified.
 * \param[in]  value  Value to be added.
 */
__INLINE__ void BeDRAM_AtomicWriteAdd(volatile uint64_t *ptr, uint64_t value)
{
    __bedram_store_op(ptr, value, BeDRAM_WRITEADD);
}

/*!
 * \brief Perform a BeDRAM atomic Write Raw operation
 *
 * Write value to storage location without ECC correction.
 *
 * \param[in]  ptr    Storage location to be written.
 * \param[in]  value  Value to be stored.
 */
__INLINE__ void BeDRAM_AtomicWriteRaw(volatile uint64_t *ptr, uint64_t value)
{
    __bedram_store_op(ptr, value, BeDRAM_WRITERAW);
}

/*!
 * \brief Perform a BeDRAM atomic Write ECC operation
 *
 * Write ECC information for storage location.
 *
 * \param[in]  ptr  Storage location whose ECC information is to be written.
 */
__INLINE__ void BeDRAM_AtomicWriteECC(volatile uint64_t *ptr, uint64_t value)
{
    __bedram_store_op(ptr, value, BeDRAM_WRITEECC);
}

#endif // __KERNEL__

__END_DECLS

#endif // Add nothing below this line.
