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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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


#include <hwi/include/common/compiler_support.h>
#include <stddef.h> // for size_t

__BEGIN_DECLS

// Note:  strcpy, strcat, strcmp implementations were replaced with strncpy, strncat, strncmp versions to protect against buffer overruns
//        in the kernel.  

int strncmp( const char *s1, const char *s2, size_t n )
{
    int rc = 0;

    while ( n-- && !(rc = (*s1 - *s2)) && *s1 )
       {
       s1++;
       s2++;
       }

    return( rc );
}

size_t strlen( const char *s )
{
   const char *p = s;

   while ( *p )
      p++;

   return( p - s );
}

// Note: As per SUSV3, n is number of bytes to copy from src or zero,
//       not a limiter on max len of dest.
char *strncpy( char *dest, const char *src, size_t n )
{
   char *p = dest;

   while ( n && (*p++ = *src++) )
      n--;

   if (n != 0) {
      while ( --n )
         *p++ = 0;
   }

   return( dest );
}

char *strncat( char *dest, const char *src, size_t n )
{
   char *p = dest;

   while ( *p )
      p++;

   while ( (*p++ = *src++) && n-- )
      ;

   *p = 0; // if n ran out before src

   return( dest );
}

char* strstr(const char* source, const char* substr)
{
    size_t len = strlen(substr);
    while(*source)
    {
        if(strncmp(source, substr, len) == 0)
            return (char*)source;
        source++;
    }
    return NULL;
}

__END_DECLS
