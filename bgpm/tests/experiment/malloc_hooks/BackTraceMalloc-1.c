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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include <stdlib.h>
#include <malloc.h>


#include <hwi/include/common/bgq_alignment.h>
#include <hwi/include/bqc/A2_inlines.h>
#include "spi/include/kernel/location.h"


// To use:
//
// #include "BackTraceMalloc.h:
//
// Define the following environment variables before running.
//    extern BTMALLOC_LOG=btmalloc.log   # must be defined to work
//    extern BTMALLOC_LEVEL=1            # call stack level to print.  1 is default if not defined.
//                                       #  for example use 2 if want to see caller @ of a "new" operation.
//
// Use "mtrace" script from glib tools to process resulting log

#define BTMALLOC_LOG        "BTMALLOC_LOG"
#define BTMALLOC_LEVEL      "BTMALLOC_LEVEL"
#define BTMALLOC_LEVEL_DEF  (1)



static void  BTMallocInit();
static void *BtMallocHook(size_t, const void*);
static void  BtFreeHook(void*, const void*);

void (*__malloc_initialize_hook) (void) = BTMallocInit;

static void* (*old_malloc_hook) (size_t size, const void*);
static void (*old_free_hook) (void*, const void*);



__INLINE__ uint64_t Set_ThreadPriority_Low( void )
{
    uint64_t ppr32 = mfspr(SPRN_PPR32);
    asm volatile ( "or 1,1,1" : : : "memory" );
    return ppr32;
}

__INLINE__ void Restore_ThreadPriority( uint64_t pri )
{
    mtspr(SPRN_PPR32, pri);
}


typedef struct {
    ALIGN_L1D_CACHE volatile uint32_t lock;
    ALIGN_L1D_CACHE volatile uint64_t refCount;
} Lock_t;

#define LOCK_INITIALIZER {0,0}



static void Recursive_Lock(Lock_t *pLock)
{
    uint32_t lockIndex = Kernel_ProcessorID() + 1;
    uint32_t curValue;

    do {
        do {
            curValue = LoadReserved32( pLock->lock );
            if (curValue == 0 ) {
                break;
            }
            else if (curValue == lockIndex) {
                pLock->refCount++;
            }
            else {
                uint64_t savpri = Set_ThreadPriority_Low();
                while ( pLock->lock ) { // spin till free
                    asm volatile ("nop; nop; nop; nop;");  // avoid posssible live lock
                }
                Restore_ThreadPriority(savpri);
            }
        } while(1);
    } while ((curValue != lockIndex) && (!StoreConditional32(pLock->lock, lockIndex)));
}



static void Recursive_Unlock(Lock_t *pLock)
{
    uint32_t lockIndex = Kernel_ProcessorID() + 1;
    uint32_t curValue = pLock->lock;
    assert(curValue == lockIndex);
    if (pLock->refCount > 0) pLock->refCount--;
    else pLock->lock = 0;
    mbar();
}




static void BTMallocInit()
{
    const char *logName = getenv(BTMALLOC_LOG);
    if (logName == NULL) {
    }
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  printf(" ### Setting old_malloc_hook: %p old_free_hook: %p ###\n", __malloc_hook, __free_hook);
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;
  printf(" ### Setting __malloc_hook: %p __free_hook: %p ###\n", __malloc_hook, __free_hook);
}



static void * my_malloc_hook (size_t size, const void *caller)
{
  void *result;
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;

  /* Call recursively */
  result = malloc (size);

  /* printf might call malloc, so protect it too. */
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;

  printf ("caller: %p malloc (%u) returns: %p\n", caller, (unsigned int) size, result);

  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;
  return result;
}
     


static void my_free_hook (void *ptr, const void *caller)
{
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
  /* Call recursively */

  free (ptr);

  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;

  /* Save underlying hooks */
  /* printf might call free, so protect it too. */
  printf ("caller: %p freed pointer: %p\n", caller, ptr);
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;
}



main()
{

  int i;
  int* a[100];
  my_init_hook();

  printf("Starting the mallocs\n");
  for (i=0; i<100; i++) {
    a[i] = malloc(100);
    printf(" ### malloc %d results in %p ###\n", i, a[i]);
  }
  printf("Finished with mallocs, starting the frees\n");
  for (i=0; i<100; i++) {
    free(a[i]);
  }
  printf("Finished with frees\n");
}






