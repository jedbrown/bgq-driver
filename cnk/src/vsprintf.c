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

// Notes: Subset of SingleUnixSpecV2 (susv2).  See susv2/functions/fprintf.html

#include <hwi/include/common/compiler_support.h>

#include <stdarg.h>
#include <stdint.h>
#include <sys/types.h>

#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

__BEGIN_DECLS

#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define FP_SINGLE_BIAS (0x007F) //  127
#define FP_DOUBLE_BIAS (0x03FF) // 1023

#define FP_DOUBLE_MAX_EXP (309)

#define FLAG_ALT      (0x8000)
#define FLAG_ZEROPAD  (0x4000)
#define FLAG_SPACE    (0x2000)
#define FLAG_SIGN     (0x1000)
#define FLAG_CAPS     (0x0800)
#define FLAG_LEFTJUST (0x0400)
#define FLAG_SCI      (0x0200)
#define FLAG_INFNAN   (0x0100)
#define FLAG_LONGLONG (0x0080)
#define FLAG_LONG     (0x0040)
#define FLAG_UNSIGNED (0x0020)
#define FLAG_GROUPING (0x0010)
#define FLAG_PREC     (0x0080)
#define FLAG_FLAGS    (0x0001)  // used to distinquish a function pointer from a flag entry

#define GROUP_HEX '_'   // eg 0x0123_4567
#define GROUP_DEC ','   // eg 1,234,567

#if 0
static void fmt_U32( char **p, uint32_t v, unsigned flags, int prec, int width, int neg )
{
  int i, j, len;
  int pad = ((flags & FLAG_ZEROPAD) ? '0' : ' ');
  char buf[64];

  i = 62;
  buf[63] = 0;
  if ( !v )
     {
     buf[i--] = '0';
     }
  else
     {
     for ( j = 1 ; v && i > 1 ; v /= 10, i--, j++ )
        {
        buf[i] = ((v % 10) + '0');
        if ( (flags & FLAG_GROUPING) && (j == 3) )
           {
           j = 0;
           buf[--i] = GROUP_DEC;
           }
        }
     }
  len = (63 - i);

  while( (prec >= len) && (i > 1) )
     {
     buf[i--] = '0';
     len++;
     }

  if ( width && (flags & FLAG_ZEROPAD) )
     {
     // must prepend the pad (sign goes before a zero-pad)
     for ( j = len ; (j <= width) && (i > 1) ; j++ )
        {
        buf[i--] = '0';
        }
     }

  if ( neg )
     {
     buf[i--] = '-';
     len++;
     }
  else if ( flags & FLAG_SPACE )
     {
     buf[i--] = ' ';
     len++;
     }
  else if ( flags & FLAG_SIGN )
     {
     buf[i--] = '+';
     len++;
     }

  if ( width && !(flags & (FLAG_LEFTJUST | FLAG_ZEROPAD)) )
     {
     // must prepend the pad (sign goes before a zero-pad)
     for ( j = len ; (j <= width) && (i > 1) ; j++ )
        {
        buf[i--] = pad;
        }
     }

  i++;
  while( buf[i] )
     {
     **p = buf[i];
     (*p)++;
     i++;
     }

  if ( width && (flags & FLAG_LEFTJUST) )
     {
     // must append spaces to fill width
     for ( j = len ; j <= width ; j++ )
        {
        **p = ' ';
        (*p)++;
        }
     }
}

static void fmt_I32( char **p, uint32_t v, unsigned flags, int prec, int width )
{
  int neg = 0;

  if ( v < 0 )
     {
     neg = 1;
     v = -v;
     }

  return fmt_U32( p, v, flags, prec, width, neg );
}

static void fmt_X32( char **p, uint32_t v, unsigned flags, int prec, int width )
{
    int i, j, nlen, nc, skip;  
    int pad = ((flags & FLAG_ZEROPAD) ? '0' : ' ');
    int A   = ((flags & FLAG_CAPS) ? ('A' - 0x0A) : ('a' - 0x0A));
    union { uint32_t ul;
        uint8_t  ch[sizeof(uint32_t)];
    } hack;
    
    hack.ul = v;
    
    nlen = (((32 - cntlz32(v)) + 3) >> 2);        // number of chars we're gonna get
    if ( !nlen )
        nlen = 1;
    i = ((int)sizeof(uint32_t) - ((nlen + 1) >> 1)); // starting nibble
    skip = (nlen & 1);                               // skip 1st nibble?
    
    // calc number of non-pad chars and number of leading zero's
    if ( prec < nlen )
    {
        nc   = nlen;  // total non-pad chars
        prec = 0;     // leading zeros
    }
    else
    {
        nc   = prec;  // total non-pad chars
        prec -= nlen; // leading zeros
    }
    
    width = MAX( width, nc );
    
    if ( width && !(flags & FLAG_LEFTJUST) )
    {
        // must prepend the pad
        for ( j = nc ; j < width ; j++ )
        {
            **p = pad;
            (*p)++;
        }
    }
    
    // it's not clear from the spec if this counts against width
    if ( flags & FLAG_ALT ) // leading 0x or 0X required
    {
        **p = '0';
        (*p)++;
        **p = ((flags & FLAG_CAPS) ? 'X' : 'x');
        (*p)++;
        nlen += 2;
    }
    
    while( prec-- )
    {
        **p = '0';
        (*p)++;
    }
    
    for ( ; i < (int)sizeof(uint32_t) ; i++ )
    {
        uint32_t nbl0 = ((hack.ch[i] & 0xF0)>>4);
        uint32_t nbl1 = (hack.ch[i] & 0x0F);
        
        if ( skip )
        {
            skip = 0;
        }
        else
        {
            **p = ((nbl0 < 0x0A) ? (nbl0+'0') : (nbl0+A));
            (*p)++;
        }
        
        **p = ((nbl1 < 0x0A) ? (nbl1+'0') : (nbl1+A));
        (*p)++;
        
        if ( (flags & FLAG_GROUPING) )
        {
            if ( (i & 1) && (i < (sizeof(uint32_t) - 1)) )
            {
                **p = GROUP_HEX;
                (*p)++;
            }
        }
    }
    
    if ( width && (flags & FLAG_LEFTJUST) )
    {
        // must append spaces to fill width
        for ( j = nc ; j < width ; j++ )
        {
            **p = ' ';
            (*p)++;
        }
    }
}
#endif

static char* fmt_U64(char *p, size_t* cnt, uint64_t v, unsigned flags, int prec, int width, int neg )
{
  int i, j, len;
  int pad = ((flags & FLAG_ZEROPAD) ? '0' : ' ');
  char buf[64];

  i = 62;
  buf[63] = 0;
  if ( !v )
     {
     buf[i--] = '0';
     }
  else
     {
     for ( j = 1 ; v && i > 1 ; i--, j++ )
        {
        buf[i] = ((v % 10) + '0');
        v /= 10;
        if ( (flags & FLAG_GROUPING) && (j == 3) && v )
           {
           j = 0;
           buf[--i] = GROUP_DEC;
           }
        }
     }
  len = (63 - i);

  while( (prec >= len) && (i > 1) )
     {
     buf[i--] = '0';
     len++;
     }

  if ( width && (flags & FLAG_ZEROPAD) )
     {
     // must prepend the pad (sign goes before a zero-pad)
     for ( j = len ; (j <= width) && (i > 1) ; j++ )
        {
        buf[i--] = '0';
        }
     }

  if ( neg )
     {
     buf[i--] = '-';
     len++;
     }
  else if ( flags & FLAG_SPACE )
     {
     buf[i--] = ' ';
     len++;
     }
  else if ( flags & FLAG_SIGN )
     {
     buf[i--] = '+';
     len++;
     }

  if ( width && !(flags & (FLAG_LEFTJUST | FLAG_ZEROPAD)) )
     {
     // must prepend the pad (sign goes before a zero-pad)
     for ( j = len ; (j <= width) && (i > 1) ; j++ )
        {
        buf[i--] = pad;
        }
     }

  i++;
  while( buf[i] )
     {
     *p++ = buf[i];
     *cnt-=1; if(*cnt < 2) return p;
     i++;
     }

  if ( width && (flags & FLAG_LEFTJUST) )
  {
      // must append spaces to fill width
      for ( j = len ; j <= width ; j++ )
      {
          *p++ = ' ';
          *cnt -= 1;
          if(*cnt < 2) 
              return p;
      }
  }
  return p;
}


static char* fmt_I64( char *p, size_t* cnt, int64_t v, unsigned flags, int prec, int width )
{
   int neg = 0;

   if ( v < 0 )
      {
      neg = 1;
      v = -v;
      }

   return fmt_U64( p, cnt, v, flags, prec, width, neg );
}


static char* fmt_X64( char *p, size_t* cnt, uint64_t v, unsigned flags, int prec, int width )
{
   int i, j, nlen, nc, skip;
   int pad = ((flags & FLAG_ZEROPAD) ? '0' : ' ');
   int A   = ('A' - 0x0A);
   union { uint64_t ull;
           uint32_t ul[2];
           uint8_t  ch[sizeof(uint64_t)];
           } hack;

   hack.ull = v;

   nlen = (32 - cntlz32(hack.ul[0]));
   if ( nlen )
      {
      nlen = ((nlen + 3) >> 2);
      nlen += 8;
      }
   else
      {
      nlen = (32 - cntlz32(hack.ul[1]));
      nlen = ((nlen + 3) >> 2);
      }
   if ( !nlen )
      nlen = 1;
   i = ((int)sizeof(uint64_t) - ((nlen + 1) >> 1)); // starting nibble
   skip = (nlen & 1);                            // skip 1st nibble?

   // calc number of non-pad chars and number of leading zero's
   if ( prec < nlen )
      {
      nc   = nlen;  // total non-pad chars
      prec = 0;     // leading zeros
      }
   else
      {
      nc   = prec;  // total non-pad chars
      prec -= nlen; // leading zeros
      }

   width = MAX( width, nc );

   if ( width && !(flags & FLAG_LEFTJUST) )
      {
      // must prepend the pad
      for ( j = nc ; j < width ; j++ )
         {
          *p++ = pad;
          *cnt-=1; if(*cnt < 2) return p;
         }
      }

   // it's not clear from the spec if this counts against width
   if ( flags & FLAG_ALT ) // leading 0x or 0X required
      {
      *p++ = '0';
      *cnt-=1; if(*cnt < 2) return p;
      *p++ = ((flags & FLAG_CAPS) ? 'X' : 'x');
      *cnt-=1; if(*cnt < 2) return p;
      nlen += 2;
      }

   while( prec-- )
      {
      *p++ = '0';
      *cnt-=1; if(*cnt < 2) return p;
      }

   for ( ; i < (int)sizeof(uint64_t) ; i++ )
      {
      uint32_t nbl0 = ((hack.ch[i] & 0xF0)>>4);
      uint32_t nbl1 = (hack.ch[i] & 0x0F);

      if ( skip )
         {
         skip = 0;
         }
      else
         {
         *p++ = ((nbl0 < 0x0A) ? (nbl0+'0') : (nbl0+A));
         *cnt-=1; if(*cnt < 2) return p;
         }

      *p++ = ((nbl1 < 0x0A) ? (nbl1+'0') : (nbl1+A));
      *cnt-=1; if(*cnt < 2) return p;

      if ( (flags & FLAG_GROUPING) )
         {
         if ( (i & 1) && (i < (sizeof(uint64_t) - 1)) )
            {
            *p++ = GROUP_HEX;
            }
         }
      }

   if ( width && (flags & FLAG_LEFTJUST) )
      {
      // must append spaces to fill width
      for ( j = nc ; j < width ; j++ )
         {
         *p++ = ' ';
         *cnt-=1; if(*cnt < 2) return p;
         }
      }
    return p;
}

static char* fmt_c1( char *p, size_t* cnt, uint64_t v, unsigned flags, int prec, int width )
{
    if ( width && !(flags & FLAG_LEFTJUST) )
    {
        while( --width )
        {
            *p++ = ' ';
            *cnt-=1; if(*cnt < 2) return p;
        }
    }
    *p++ = (char)v;
    *cnt-=1; if(*cnt < 2) return p;

    if ( width && (flags & FLAG_LEFTJUST) )
    {
        while( --width )
        {
            *p++ = ' ';
            *cnt-=1; if(*cnt < 2) return p;
        }
    }
    return p;
}

static char* fmt_str( char *p, size_t* cnt, uint64_t v, unsigned flags, int prec, int width )
{
    int i;
    const char* q; 
    const char* s = (const char*)v;
    if ( !s )
        s = "(NULL)";
    if ( width )
    {
        i = 0;
        q = s;
        while ( *q++ )
            i++;
        width -= i;
        if ( (width > 0) && !(flags & FLAG_LEFTJUST) ) 
        {
            while( width-- )
            {
                *p++ = ' ';
                *cnt-=1; if(*cnt < 2) return p;
            }
        }
        while ( *s )
        {
            *p++ = *s++;
            *cnt-=1; if(*cnt < 2) return p;
        }
        if ( (width > 0) && (flags & FLAG_LEFTJUST) )
        {
            while( width-- )
            {
                *p++ = ' ';
                *cnt-=1; if(*cnt < 2) return p;
            }
        }
    }
    else
    {
        while ( *s )
        {
            *p++ = *s++;
            *cnt-=1; if(*cnt < 2) return p;
        }
    }
    return p;
}


static char* fmt_x64( char *p, size_t* cnt, uint64_t v, unsigned flags, int prec, int width )
{
    int i, j, nlen, nc, skip;
    int pad = ((flags & FLAG_ZEROPAD) ? '0' : ' ');
    int A   = ('a' - 0x0A);
    union { uint64_t ull;
        uint32_t ul[2];
        uint8_t  ch[sizeof(uint64_t)];
    } hack;
    
    hack.ull = v;
    
    nlen = (32 - cntlz32(hack.ul[0]));
    if ( nlen )
    {
        nlen = ((nlen + 3) >> 2);
        nlen += 8;
    }
    else
    {
        nlen = (32 - cntlz32(hack.ul[1]));
        nlen = ((nlen + 3) >> 2);
    }
    if ( !nlen )
        nlen = 1;
    i = ((int)sizeof(uint64_t) - ((nlen + 1) >> 1)); // starting nibble
    skip = (nlen & 1);                            // skip 1st nibble?
    
    // calc number of non-pad chars and number of leading zero's
    if ( prec < nlen )
    {
        nc   = nlen;  // total non-pad chars
        prec = 0;     // leading zeros
    }
    else
    {
        nc   = prec;  // total non-pad chars
        prec -= nlen; // leading zeros
    }
    
    width = MAX( width, nc );
    
    if ( width && !(flags & FLAG_LEFTJUST) )
    {
        // must prepend the pad
        for ( j = nc ; j < width ; j++ )
        {
            *p++ = pad;
            *cnt-=1; if(*cnt < 2) return p;
        }
    }
    
    // it's not clear from the spec if this counts against width
    if ( flags & FLAG_ALT ) // leading 0x or 0X required
    {
        *p++ = '0';
        *cnt-=1; if(*cnt < 2) return p;
        *p++ = ((flags & FLAG_CAPS) ? 'X' : 'x');
        *cnt-=1; if(*cnt < 2) return p;
        nlen += 2;
    }
    
    while( prec-- )
    {
        *p++ = '0';
        *cnt-=1; if(*cnt < 2) return p;
    }
    
    for ( ; i < (int)sizeof(uint64_t) ; i++ )
    {
        uint32_t nbl0 = ((hack.ch[i] & 0xF0)>>4);
        uint32_t nbl1 = (hack.ch[i] & 0x0F);
        
        if ( skip )
        {
            skip = 0;
        }
        else
        {
            *p++ = ((nbl0 < 0x0A) ? (nbl0+'0') : (nbl0+A));
            *cnt-=1; if(*cnt < 2) return p;
        }
        
        *p++ = ((nbl1 < 0x0A) ? (nbl1+'0') : (nbl1+A));
        *cnt-=1; if(*cnt < 2) return p;
        
        if ( (flags & FLAG_GROUPING) )
        {
            if ( (i & 1) && (i < (sizeof(uint64_t) - 1)) )
            {
                *p++ = GROUP_HEX;
                *cnt-=1; if(*cnt < 2) return p;
            }
        }
    }
    
    if ( width && (flags & FLAG_LEFTJUST) )
    {
        // must append spaces to fill width
        for ( j = nc ; j < width ; j++ )
        {
            *p++ = ' ';
            *cnt-=1; if(*cnt < 2) return p;
        }
    }
    return p;
}


#if 0
static void _fmt_D( char **p, _BL_Double *d, unsigned flags, int prec, int width )
{
   int done = 1, len = 0;
   char *q;
   int exp  = ((d->ul[0] & 0x7FF00000) >> 20); // unbiased
   int bexp = (exp - FP_DOUBLE_BIAS);         // biased
   int sign = ((d->ul[0] & 0x80000000) >> 31);
   unsigned long fh = (d->ul[0] & 0x000FFFFF);
   unsigned long fl = d->ul[1];

   if ( sign )
      {
      **p = '-'; (*p)++; len = 1;
      }
   else if ( flags & FLAG_SPACE )
      {
      **p = ' '; (*p)++; len = 1;
      }
   else if ( flags & FLAG_SIGN )
      {
      **p = '+'; (*p)++; len = 1;
      }

   q = (void *)0;
   if ( _bl_DIsZero( sign, exp, fh, fl ) )
      {
      q = "0.0";
      }
   else if ( _bl_DIsInf( sign, exp, fh, fl ) )
      {
      q = ((flags & FLAG_CAPS) ? "INF" : "inf");
      }
   else if ( _bl_DIsQNaN( sign, exp, fh, fl ) )
      {
      q = "QNaN";
      }
   else if ( _bl_DIsSNaN( sign, exp, fh, fl ) )
      {
      q = "SNaN";
      }
   else
      {
      q = ((flags & FLAG_CAPS) ? "0X1." : "0x1.");
      done = 0;
      }

   if ( q )
      {
      while ( *q )
         {
         **p = *q++;
         (*p)++;
         len++;
         }
      }

   // show the mantissa, then the biased exponent
   d->ul[0] &= 0x000FFFFF;

   if ( !done )
      {
      fmt_X64( p, d->ull, (FLAG_ZEROPAD | (flags & FLAG_CAPS)), (5+8), (5+8) );
      **p = ((flags & FLAG_CAPS) ? 'P' : 'p');
      (*p)++;
      fmt_I32( p, bexp, (FLAG_ZEROPAD | FLAG_SIGN), 6, 3 );
      }

}
#endif

typedef char*(*FORMATTER)(char *p, size_t* cnt, uint64_t v, unsigned flags, int prec, int width, int neg);

const uint64_t PRINT_CONTROL[] = 
{
0, // char(0)
0, // char(1)
0, // char(2)
0, // char(3)
0, // char(4)
0, // char(5)
0, // char(6)
0, // char(7)
0, // char(8)
0, // char(9)
0, // char(10)
0, // char(11)
0, // char(12)
0, // char(13)
0, // char(14)
0, // char(15)
0, // char(16)
0, // char(17)
0, // char(18)
0, // char(19)
0, // char(20)
0, // char(21)
0, // char(22)
0, // char(23)
0, // char(24)
0, // char(25)
0, // char(26)
0, // char(27)
0, // char(28)
0, // char(29)
0, // char(30)
0, // char(31)
FLAG_SPACE | FLAG_FLAGS, // char(32) =  
0, // char(33) = !
0, // char(34) = "
FLAG_ALT | FLAG_FLAGS, // char(35) = #
0, // char(36) = $
0, // char(37) = %
0, // char(38) = &
0, // char(39) = '
0, // char(40) = (
0, // char(41) = )
0, // char(42) = *
FLAG_SIGN | FLAG_FLAGS, // char(43) = +
0, // char(44) = ,
FLAG_LEFTJUST | FLAG_FLAGS, // char(45) = -
FLAG_PREC | FLAG_FLAGS, // char(46) = .
0, // char(47) = /
FLAG_ZEROPAD | FLAG_FLAGS, // char(48) = 0
0, // char(49) = 1
0, // char(50) = 2
0, // char(51) = 3
0, // char(52) = 4
0, // char(53) = 5
0, // char(54) = 6
0, // char(55) = 7
0, // char(56) = 8
0, // char(57) = 9
0, // char(58) = :
0, // char(59) = ;
0, // char(60) = <
0, // char(61) = =
0, // char(62) = >
0, // char(63) = ?
0, // char(64) = @
0, // char(65) = A
0, // char(66) = B
0, // char(67) = C
0, // char(68) = D
0, // char(69) = E
0, // char(70) = F
0, // char(71) = G
0, // char(72) = H
0, // char(73) = I
0, // char(74) = J
0, // char(75) = K
FLAG_LONGLONG | FLAG_FLAGS, // char(76) = L
0, // char(77) = M
0, // char(78) = N
(uint64_t)fmt_X64, // char(79) = O
(uint64_t)fmt_X64, // char(80) = P
0, // char(81) = Q
0, // char(82) = R
0, // char(83) = S
0, // char(84) = T
0, // char(85) = U
0, // char(86) = V
0, // char(87) = W
(uint64_t)fmt_X64, // char(88) = X
0, // char(89) = Y
0, // char(90) = Z
0, // char(91) = [
FLAG_GROUPING | FLAG_FLAGS, // char(92) = backslash
0, // char(93) = ]
0, // char(94) = ^
0, // char(95) = _
0, // char(96) = `
0, // char(97) = a
0, // char(98) = b
(uint64_t)fmt_c1, // char(99) = c
(uint64_t)fmt_I64, // char(100) = d
0, // char(101) = e
0, // char(102) = f
0, // char(103) = g
0, // char(104) = h
(uint64_t)fmt_I64, // char(105) = i
0, // char(106) = j
0, // char(107) = k
FLAG_LONGLONG | FLAG_FLAGS, // char(108) = l
0, // char(109) = m
0, // char(110) = n
(uint64_t)fmt_x64, // char(111) = o
(uint64_t)fmt_x64, // char(112) = p
0, // char(113) = q
0, // char(114) = r
(uint64_t)fmt_str, // char(115) = s
0, // char(116) = t
(uint64_t)fmt_U64, // char(117) = u
0, // char(118) = v
0, // char(119) = w
(uint64_t)fmt_x64, // char(120) = x
0, // char(121) = y
0, // char(122) = z
0, // char(123) = {
0, // char(124) = |
0, // char(125) = }
0, // char(126) = ~
0, // char(127) = 
0, // char(128)
0, // char(129)
0, // char(130)
0, // char(131)
0, // char(132)
0, // char(133)
0, // char(134)
0, // char(135)
0, // char(136)
0, // char(137)
0, // char(138)
0, // char(139)
0, // char(140)
0, // char(141)
0, // char(142)
0, // char(143)
0, // char(144)
0, // char(145)
0, // char(146)
0, // char(147)
0, // char(148)
0, // char(149)
0, // char(150)
0, // char(151)
0, // char(152)
0, // char(153)
0, // char(154)
0, // char(155)
0, // char(156)
0, // char(157)
0, // char(158)
0, // char(159)
0, // char(160)
0, // char(161)
0, // char(162)
0, // char(163)
0, // char(164)
0, // char(165)
0, // char(166)
0, // char(167)
0, // char(168)
0, // char(169)
0, // char(170)
0, // char(171)
0, // char(172)
0, // char(173)
0, // char(174)
0, // char(175)
0, // char(176)
0, // char(177)
0, // char(178)
0, // char(179)
0, // char(180)
0, // char(181)
0, // char(182)
0, // char(183)
0, // char(184)
0, // char(185)
0, // char(186)
0, // char(187)
0, // char(188)
0, // char(189)
0, // char(190)
0, // char(191)
0, // char(192)
0, // char(193)
0, // char(194)
0, // char(195)
0, // char(196)
0, // char(197)
0, // char(198)
0, // char(199)
0, // char(200)
0, // char(201)
0, // char(202)
0, // char(203)
0, // char(204)
0, // char(205)
0, // char(206)
0, // char(207)
0, // char(208)
0, // char(209)
0, // char(210)
0, // char(211)
0, // char(212)
0, // char(213)
0, // char(214)
0, // char(215)
0, // char(216)
0, // char(217)
0, // char(218)
0, // char(219)
0, // char(220)
0, // char(221)
0, // char(222)
0, // char(223)
0, // char(224)
0, // char(225)
0, // char(226)
0, // char(227)
0, // char(228)
0, // char(229)
0, // char(230)
0, // char(231)
0, // char(232)
0, // char(233)
0, // char(234)
0, // char(235)
0, // char(236)
0, // char(237)
0, // char(238)
0, // char(239)
0, // char(240)
0, // char(241)
0, // char(242)
0, // char(243)
0, // char(244)
0, // char(245)
0, // char(246)
0, // char(247)
0, // char(248)
0, // char(249)
0, // char(250)
0, // char(251)
0, // char(252)
0, // char(253)
0, // char(254)
0 // char(255)
};

int vsnprintf( char *buf, size_t cnt, const char *fp, va_list args )
{
    char *p  = buf;
    int prec, width;
    unsigned flags;
    uint64_t    ull;
    
    if(cnt < 2) goto bailout;
    
    for (; *fp ; fp++ )
    {
        while ( *fp && *fp != '%' )
        {
            *p++ = *fp++;
            cnt-=1;
            if(cnt < 2) goto bailout;
        }
        
        if ( ! *fp )
            break;
        
        flags = 0;
        prec  = 0;
        width = 0;
        fp++;
        while(1)
        {
            unsigned char tmp = *fp;
            if(tmp == 0) break;
            uint64_t ctrl = PRINT_CONTROL[tmp];
            if(ctrl && !(ctrl &  FLAG_FLAGS)) 
            {
                ull = va_arg( args, uint64_t);
                p = ((FORMATTER)(ctrl))(p, &cnt, ull, flags, prec, width, 0);
                if(cnt < 2) goto bailout;
                break;
            }
            else
            {
                ctrl &= ~((uint64_t)FLAG_FLAGS);
                flags |= ctrl;
                if(ctrl == FLAG_PREC)
                {
                    prec = 0;
                    if ( ISDIGIT( *fp ) )
                    {
                        while ( ISDIGIT( *fp ) )
                        {
                            prec *= 10;
                            prec += (*fp++ - '0');
                        }
                    }
                    else
                    {
                        fp++;
                    }
                }
                else if(ISDIGIT(tmp))
                {
                    while ( ISDIGIT(tmp) )
                    {
                        width = 10*width + (tmp - '0');
                        fp++;
                        tmp = *fp;
                    }
                }
                else
                {
                    fp++;
                }
            }
        }
    }
bailout:    
  *p = 0;  // NULL terminate

  return( (int)((unsigned long)p - (unsigned long)buf) );
}


__END_DECLS
