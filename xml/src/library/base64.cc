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

#include "xml/include/library/XML.h"
#include "xml/include/library/base64.h"

using namespace std;



#if XML_FIXES
static char e64table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static unsigned char d64table[256] = 
  {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255,  62, 255, 255, 255,  63,  52,  53,  
    54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 
   255, 128, 255, 255, 255,   0,   1,   2,   3,   4,   
     5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  
    25, 255, 255, 255, 255, 255, 255,  26,  27,  28,  
    29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  
    39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
    49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
   255, 255, 255, 255, 255, 255
  };



void encode64_new(ostream& os, const string& buf) {
  unsigned bufsize = buf.length();  // how many characters to encode
  unsigned bufcount = 0;            // how many have been encoded so far
  
  while (bufcount <= bufsize) {
    unsigned char in[3];
    unsigned outcount;
    char c;
    in[0] = in[1] = in[2] = 0;

    for (outcount = 0; outcount < 3; outcount++){
      c = buf[bufcount++];

      if (bufcount > bufsize) {
	break;
      }
      in[outcount] = c;
    }

    if (outcount > 0) {
      unsigned char out[4];
      out[0] = e64table[in[0] >> 2];
      out[1] = e64table[((in[0] & 3) << 4) | (in[1] >> 4)];
      out[2] = outcount < 2 ? '=' : e64table[((in[1] & 0xf)<<2) | (in[2]>>6)];
      out[3] = outcount < 3 ? '=' : e64table[in[2] & 0x3f];

      for (unsigned i = 0; i < 4; i++) {
	os << out[i];
      }
    }
  }
}


string decode64_new(const char *buf, unsigned buflen) {
  string result;
  unsigned bufcount = 0;

  result.reserve(buflen * 3/4); // 6 bits get decoded into 8, so we can 
  // estimate the size and prevent internal copying

  for (;;) {
    unsigned char in[4];
    
    for (unsigned i = 0; i < 4; i++){
      unsigned char c;
      do {
	c = buf[bufcount++]; 
      } while ((c == '\n' || c == ' ') && bufcount <= buflen); // skip EOL and spaces

      if (bufcount > buflen) {
	return result;
      }

      if (d64table[c] == 0xff) { // invalid character
	XMLLIB_THROW("invalid character %u in xml stream\n", (unsigned) c);
      }

      in[i] = d64table[c];
    }

    // lookup value for '=' is 128
    unsigned outlen = in[2] == 128 ? 1 : (in[3] == 128 ? 2 : 3);

    unsigned char out[3];
    out[0] = (in[0] << 2) | (in[1] >> 4);
    out[1] = (in[1] << 4) | (in[2] >> 2);
    out[2] = (in[2] << 6) | in[3];

    for (unsigned j = 0; j < outlen; j++) {
      result += out[j];
    }

    if (outlen < 3){
      return result;   // we're done
    }
  }

  return result;
}


#else

static char etable[256] =
  { 65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  
    75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  
    85,  86,  87,  88,  89,  90,  97,  98,  99, 100, 
   101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 
   111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 
   121, 122,  48,  49,  50,  51,  52,  53,  54,  55,  
    56,  57,  43,  47,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   
     0,   0,   0,   0,   0,   0 
  };

static char dtable[256] = 
  {128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128,  62, 128, 128, 128,  63,  52,  53,  
    54,  55,  56,  57,  58,  59,  60,  61, 128, 128, 
   128,   0, 128, 128, 128,   0,   1,   2,   3,   4,   
     5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  
    25, 128, 128, 128, 128, 128, 128,  26,  27,  28,  
    29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  
    39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
    49,  50,  51, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
   128, 128, 128, 128, 128,   0
  };



void encode64(ostream &os, const char *source, unsigned ssize, bool newlines) {

  unsigned scount = 0;
  unsigned dcount = 0;
  unsigned done = 0;

  while (!done) {
    unsigned char igroup[3], ogroup[4];
    unsigned n;
    int c;
    igroup[0] = igroup[1] = igroup[2] = 0;

    for (n = 0; n < 3; n++){
      c = source[scount++];

      if (scount > ssize) {
	done = 1;
	break;
      }
      igroup[n] = c;
    }

    if (n > 0) {
      ogroup[0] = etable[igroup[0]>>2];
      ogroup[1] = etable[((igroup[0]&3)<<4) | (igroup[1]>>4)];
      ogroup[2] = etable[((igroup[1]&0xF)<<2) | (igroup[2]>>6)];
      ogroup[3] = etable[igroup[2]&0x3F];
      
      // fill up ogroup if necessary
      if (n < 3) {
	ogroup[3] = '=';
	if (n < 2){
	  ogroup[2] = '=';
	}
      }

      for (unsigned i = 0; i < 4; i++) {
	os << ogroup[i];
	dcount++;
	if (newlines && dcount > 0 && !(dcount % 72)) {
	  os << endl;
	}
      }
    }
  }

  if (newlines) {
    os << endl;
  }
}



void decode64(ostream &os, const char *source, unsigned ssize) {

  unsigned scount = 0;

  for (;;) {
    unsigned char a[4], b[4], o[3];
    unsigned i;
    
    for (i = 0; i < 4; i++){
      unsigned char c;
      do {
	c = source[scount++]; 
      } while ((c == '\n' || c == ' ') && scount <= ssize); // skip EOL and spaces

      if (scount > ssize) {
	return;
      }

      if (dtable[c] & 0x80){
	cerr << "Illegal character " << (unsigned) c << " in source buffer" << endl;
	exit(1);
      }

      a[i] = c;
      b[i] = dtable[c];
    }

    o[0] = (b[0]<<2) | (b[1]>>4);
    o[1] = (b[1]<<4) | (b[2]>>2);
    o[2] = (b[2]<<6) | b[3];

    i = a[2] == '=' ? 1 : (a[3] == '=' ? 2 : 3);

    for (unsigned j = 0; j < i; j++) {
      os << o[j];
    }

    if (i < 3){
      return;   // we're done
    }
  }
}



void encode64(ostream &os,
	      const string& source,
	      bool newlines) {
  encode64(os, source.c_str(), source.length(), newlines);
}

void decode64(ostream &os,
	      const string& source) {
  decode64(os, source.c_str(), source.length());
}
#endif
