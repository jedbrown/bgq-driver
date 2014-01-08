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
#include <malloc.h>
//#include <not-cancel.h>

static void my_init_hook(void);
static void *my_malloc_hook(size_t, const void*);
static void my_free_hook(void*, const void*);

void (*__malloc_initialize_hook) (void) = my_init_hook;

static void* (*old_malloc_hook) (size_t size, const void*);
static void (*old_free_hook) (void*, const void*);



//#define PRINTBUF(s) write_not_cancel (STDOUT_FILENO, s, strlen(s))
static void my_init_hook (void)
{
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
