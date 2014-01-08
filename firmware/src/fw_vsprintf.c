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


// Write formatted text to host console.
// This is a simplified version of printf.
// It accepts only %% %c %x %d %u %s formatting specifiers.

#include <stdarg.h>

/**
 * @brief Put one character into the buffer (no additional formatting
 *        required).
 */

inline void fw_putc(char x, char** bufp) {
  **bufp = x; 
  (*bufp)++;
}

inline void fw_put1x(char x, char** bufp) {
  fw_putc( (x >= 0xa) ? ('A' + (x - 0xA)) : ('0' + x), bufp );
}


inline int fw_putNx(unsigned long x, unsigned n, char** bufp) {
  
  int i;

  for (i = 0; i < n; i++) {
    unsigned long mask = 0xFul << ( (n-i-1)*4 );
    unsigned long xx = (x & mask) >> ((n-i-1)*4);
    fw_put1x( xx, bufp );
  }
  return n;
}


/**
 * @brief Formats a binary unsigned integer into string format.
 */

inline int fw_putu(unsigned long x, char** bufp) {

    if (x == 0) {
      fw_putc('0', bufp);
      return 1;
    }

    char buf[20]; // room for largest unsigned long integer plus 1 for null
    char *cp = &buf[sizeof(buf)];
    *--cp = 0;

    while (x) {
      unsigned digit = x % 10;
      *--cp = digit + '0';
      x /= 10;
    }

    int len = 0;
    while (*cp) {
      fw_putc(*cp++, bufp);
      len++;
    }

    return len;
}


/**
 * @brief: Prints a signed binary integer.
 */

inline int fw_putd(long int x, char** bufp) {

  int len = 0;

  if (x < 0) {
    fw_putc('-', bufp);
    len++;
    len += fw_putu(-x, bufp);
  }
  else {
    len = fw_putu(x, bufp);
  }

  return len;
}

/**
 * @brief Formats a string (easy)
 */

inline int fw_putstr(const char *x, char** bufp) {
  int len = 0;
  while (*x) {
    fw_putc(*x++, bufp);
    len++;
  }
  return len;
}


#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')

int fw_vsprintf( char *buf, const char* fmt, va_list args ) {

  int rc = 0;

  while (*fmt) {

    char ch = *fmt++;

    if (ch == 0)
      break;

    if (ch != '%') {
      fw_putc(ch, &buf);
      rc++;
      continue;
    }

    // Skip fieldwidth and precision specifiers ... we don't support them
    while ( ISDIGIT(*fmt) || (*fmt == '.') || (*fmt == '-') )
      fmt++;

    // Skip over 'l' (e.g. %lx) and 'll' (e.g. %llx)
    
    if (*fmt == 'l')
      fmt++;

    if (*fmt == 'l')
      fmt++;


    switch (*fmt) 
      {
      case '%' :
	{
	  rc++;
	  fw_putc('%', &buf);
	  break;
	}

      case 'c':
	{
	  rc++;
	  fw_putc(va_arg(args, int), &buf);
	  break;
	}
	
      case 'd':
	{
	  rc += fw_putd(va_arg(args, int), &buf);
	  break;
	}

      case 'u': {
	rc += fw_putu(va_arg(args, unsigned), &buf);
	break;
      }

      case 'p': 
      case 'x':
      case 'X':
	{ 
	  unsigned long x;

	  if (*fmt == 'p') {
	    void* ptr = va_arg(args,void*);
	    x = (unsigned long)ptr;
	  }
	  else {
	    x = va_arg(args,unsigned long);
	  }
	  rc += fw_putNx( x, 16, &buf);
	  break;
	}

      case 's':
	{
	  rc += fw_putstr(va_arg(args, char *), &buf);
	  break;
	}

      default:
	{
	rc += fw_putstr("<%",&buf); 
	fw_putc(*--fmt, &buf); 
	rc++;
	rc += fw_putstr("?>\n", &buf);
	break;
	}
      }
    
    fmt++;
  }

  return rc;
}
