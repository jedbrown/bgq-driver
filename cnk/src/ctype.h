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

#ifndef _BP_CTYPE_H // prevent multiple inclusion
#define _BP_CTYPE_H

#define _CTYPE_H
#define _LINUX_CTYPE_H

#include <hwi/include/common/compiler_support.h>

__BEGIN_DECLS

#define C_ALPHA  (0x0001)
#define C_ALNUM  (0x0002)
#define C_ASCII  (0x0004)
#define C_BLANK  (0x0008)
#define C_CNTRL  (0x0010)
#define C_DIGIT  (0x0020)
#define C_GRAPH  (0x0040)
#define C_LOWER  (0x0080)
#define C_PRINT  (0x0100)
#define C_PUNCT  (0x0200)
#define C_SPACE  (0x0400)
#define C_UPPER  (0x0800)
#define C_XDIGIT (0x1000)

#define C_MASK (~0x7F)

#if !defined(__ASSEMBLY__)

extern unsigned short _bp_ctype[]; // see SimLib/lib/cnk_ctype.c

#define isalpha( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_ALPHA )
#define isalnum( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_ALNUM )
#define isascii( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_ASCII )
#define isblank( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_BLANK )
#define iscntrl( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_CNTRL )
#define isdigit( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_DIGIT )
#define isgraph( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_GRAPH )
#define islower( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_LOWER )
#define isprint( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_PRINT )
#define ispunct( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_PUNCT )
#define isspace( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_SPACE )
#define isupper( c )  (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_UPPER )
#define isxdigit( c ) (((c) & C_MASK) ? 0 : _bp_ctype[(unsigned)(c)] & C_XDIGIT)

#define tolower(c) (isupper(c) ? ((c) - ('A' - 'a')) : (c))
#define toupper(c) (islower(c) ? ((c) + ('A' - 'a')) : (c))

#endif // __ASSEMBLY__

__END_DECLS

#endif // Add nothing below this line.

