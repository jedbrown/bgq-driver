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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>

#include "bgq_util/include/ArbitraryLengthNumber.h"
#include "bgq_util/include/InternalError.h"

/*!
// Convert the databits in this data into a binary string
// @param  sepStr -- separator string -- string to insert as a separator
// @param  sepInterval -- byte interval to place the seperator
// @returns --  binary string of the contents.
*/
const string ArbitraryLengthNumber::getBinString(const string& sepStr, unsigned sepInterval) const
{
    if (getBitCount() == 0)
        return("");
    ostringstream ostr;
    unsigned bitcount = getBitCount();
    for (unsigned n = 0; n < bitcount; n++)
    {
        if (sepStr.length())
        {
            if ((n > 0) && ((n % sepInterval) == 0))
                ostr << sepStr;
        }
        ostr << getBit(n);

    }
    return(ostr.str());

}
const string ArbitraryLengthNumber::getBinStringRev(const string& sepStr , unsigned sepInterval) const
{
    if (getBitCount() == 0)
        return("");
    ostringstream ostr;
    unsigned bitcount = getBitCount();
    for (unsigned n = 0; n < bitcount; n++)
    {
        if (sepStr.length())
        {
            if ((n > 0) && ((n % sepInterval) == 0))
                ostr << sepStr;
        }
        ostr << getBit(bitcount-n-1);
    }
    return(ostr.str());

}

/*!
// Convert the databits in this data into a hex string
// @param  sepStr -- separator string -- string to insert as a separator
// @param  sepInterval -- byte interval to place the seperator
// @returns --  hex string of the contents.
*/
const string ArbitraryLengthNumber::getHexString(const string& sepStr, unsigned sepInterval) const
{
    if (getByteCount() == 0)
        return("");
    unsigned align = (getBitCount() % 8);
    unsigned byteCount = 0;
    int n = getByteCount();
    ostringstream ostr;
    if (align == 0)
        align = 8;
    uint8_t mask = uint8_t(unsigned(1<<align)-1);
    ostr << hex << setw(2) << setfill('0') << (unsigned)(_data[--n] & mask);
    byteCount++;
    while (n > 0)
    {
        if ((byteCount > 0) && ((byteCount % sepInterval) == 0))
            ostr << sepStr;
        ostr << hex << setw(2) << setfill('0') << (unsigned)_data[--n];
        byteCount++;

    }
    return(ostr.str());
}


/*!
// Convert the databits in this data into a hex string
// @param  startbit -- index of bit to start with
// @param  endbit -- byte interval to place the seperator
// @returns --  hex string of the contents.
*/
const string ArbitraryLengthNumber::getHexString(unsigned startBit, unsigned endBit) const
{
    if (startBit > endBit)
        throw InternalError("ArbitraryLengthNumber:getHexString -- startBit > endBit", __FILE__, __LINE__);
    if (endBit >= _nbits)
        throw InternalError("ArbitraryLengthNumber:getHexString -- not enough bits", __FILE__, __LINE__);
    if (getBitCount() == 0)
        return("");

    // byte aligned start and end??
    ostringstream ostr;
    if (isByteAligned(startBit, endBit, _nbits) )
    {   
       unsigned int index = ((_nbits-1)-startBit)/8;
       unsigned nbytes = ((endBit-startBit+1) / 8);
       for (unsigned n = 0; n < nbytes; n++)
       {
          unsigned value = _data[index--];
          ostr << hex << setw(2) << setfill('0') << value;
       }
       return(ostr.str());
    }
	
    uint8_t mask = (1 << (((_nbits-1)-startBit)%8));  
    unsigned int index = ((_nbits-1)-startBit)/8;
    uint8_t ch = _data[index];
    unsigned value = 0;
    unsigned bit = startBit;
    unsigned endoffset = endBit%8;
    for (; bit <= endBit; bit++)
    {   
       if (mask == 0)
       {
          if (index == 0)
             throw InternalError("ArbitraryLengthNumber:getHexString -- index out of range", __FILE__, __LINE__);
          ch = _data[--index];    // pick up the new one...
          mask = 0x80;
       }
       value = (value << 1) | ((ch & mask) ? 1 : 0);
       mask >>= 1;
       if( bit%8 == endoffset )
       {
          ostr << hex << setw(2) << setfill('0') << value;
          value = 0;
       }
    }
    return(ostr.str());
}


/*!
// Retrieve the bit count for this number.
// @returns -- number of bits in this arbitrary number.
*/
const uint32_t ArbitraryLengthNumber::getBitCount() const 
{
    return(_nbits); 
};

void ArbitraryLengthNumber::setBitCount(unsigned numbits) 
{
    _nbits = numbits;
    _data.resize((_nbits+7)/8);
};
/*!
// Retrieve the bit count for this number.
// @returns -- number of bits in this arbitrary number.
*/
const uint32_t ArbitraryLengthNumber::getByteCount() const 
{
    return(_data.size()); 
};

/*!
// Get the 1st 32 bits (LSB) of this number and convert it to a uint32 in the
// native byte format of the machine.
// @param startbit -- starting bit
// @param endbit -- 
// @returns -- 1st 32 bits of the number.
*/
const uint32_t ArbitraryLengthNumber::getUint32(unsigned int startBit, unsigned int endBit) const
{
    if (startBit > endBit)
        throw InternalError("ArbitraryLengthNumber:getUint32 -- startBit > endBit", __FILE__, __LINE__);
    if  (endBit >= _nbits)
        throw InternalError("ArbitraryLengthNumber::getUint32 -- not enough bits", __FILE__, __LINE__);
    if ((endBit-startBit+1) > 32)
        throw InternalError("ArbitraryLengthNumber:getUint32 -- bitcount > 32", __FILE__, __LINE__);


    // byte aligned start and end??
    if (isByteAligned(startBit, endBit, _nbits) )
    {   
        uint32_t value = 0;
        // pick this up, lsb to msb
        unsigned index = ((_nbits-1)-endBit)/8;
        unsigned nbytes = ((endBit-startBit+1) / 8);
        for (unsigned n = 0; n < nbytes; n++)
            value |= (uint32_t(_data[index++]) << (n*8));
        return(value);            
    }

    uint32_t value = 0;
    uint8_t mask = (1 << (((_nbits-1)-startBit)%8));  
    unsigned int index = ((_nbits-1)-startBit)/8;
    uint8_t ch = _data[index];
    for (int bit = startBit; bit <= (int)endBit; bit++)
    {   
        if (mask == 0)
        {
            //_data[index] = ch;       // save the current character
            if (index == 0)
                throw InternalError("ArbitraryLengthNumber:getUint32 -- index out of range", __FILE__, __LINE__);
            ch = _data[--index];    // pick up the new one...
            mask = 0x80;
        }
        value = (value << 1) | ((ch & mask) ? 1 : 0);
        mask >>= 1;
    }
    return(value);

}
/*!
// Get the 1st 32 bits (LSB) of this number and convert it to a uint32 in the
// native byte format of the machine.
// @returns -- 1st 32 bits of the number.
*/
const uint32_t ArbitraryLengthNumber::getUint32() const
{
    if (_nbits == 0)
        return(0);
    if (_nbits < 32)
        return getUint32(0, _nbits-1);
    if (_nbits > 32)
        throw InternalError("ArbitraryLengthNumber:getUint32 -- _nbits > 32", __FILE__, __LINE__);


    return(((uint32_t)_data[0]  << 0) |
           ((uint32_t)_data[1]  << 8) |
           ((uint32_t)_data[2]  << 16) |
           ((uint32_t)_data[3]  << 24));
};

/*!
// Get the 1st 64 bits (LSB) of this number and convert it to a uint64 in the
// native byte format of the machine.
// @param startbit -- starting bit (default 0)
// @param endbit -- ending bit (64)
// @returns -- 1st 64 bits of the number.
*/

const uint64_t ArbitraryLengthNumber::getUint64(unsigned int startBit, unsigned int endBit) const
{
    if (startBit > endBit)
        throw InternalError("ArbitraryLengthNumber:getUint64 -- startBit > endBit", __FILE__, __LINE__);
    if (endBit >= _nbits)
        throw InternalError("ArbitraryLengthNumber:getUint64 -- not enough bits", __FILE__, __LINE__);
    if ((endBit-startBit+1) > 64)
        throw InternalError("ArbitraryLengthNumber:getUint64 -- bitcount > 64", __FILE__, __LINE__);

    // byte aligned start and end??
    if (isByteAligned(startBit, endBit, _nbits) )
    {   
        uint64_t value = 0;
        // pick this up, lsb to msb
        unsigned index = ((_nbits-1)-endBit)/8;
        unsigned nbytes = ((endBit-startBit+1) / 8);
        for (unsigned n = 0; n < nbytes; n++)
            value |= (uint64_t(_data[index++]) << (n*8));
        return(value);            
    }

    uint64_t value = 0;
    uint8_t mask = (1 << (((_nbits-1)-startBit)%8));  
    unsigned int index = ((_nbits-1)-startBit)/8;
    uint8_t ch = _data[index];
    for (int bit = startBit; bit <= (int)endBit; bit++)
    {   
        if (mask == 0)
        {
            //_data[index] = ch;       // save the current character
            if (index == 0)
                throw InternalError("ArbitraryLengthNumber:getUint64 -- index out of range", __FILE__, __LINE__);
            ch = _data[--index];    // pick up the new one...
            mask = 0x80;
        }
        value = (value << 1) | ((ch & mask) ? 1 : 0);
        mask >>= 1;
    }
    return(value);
}
/*!
// Get the 1st 64 bits (LSB) of this number and convert it to a uint64 in the
// native byte format of the machine.
// @returns -- 1st 64 bits of the number.
*/
const uint64_t ArbitraryLengthNumber::getUint64() const 
{
    if (_nbits == 0)
        return(0);
    if (_nbits < 64)
        return getUint64(0, _nbits-1);
    if (_nbits > 64)
        throw InternalError("ArbitraryLengthNumber:getUint64 -- _nbits > 64", __FILE__, __LINE__);

    return(((uint64_t)_data[0]  << 0)  |
           ((uint64_t)_data[1]  << 8)  |
           ((uint64_t)_data[2]  << 16) |
           ((uint64_t)_data[3]  << 24) |
           ((uint64_t)_data[4]  << 32) |
           ((uint64_t)_data[5]  << 40) |
           ((uint64_t)_data[6]  << 48) |
           ((uint64_t)_data[7]  << 56));
};

/*!
// set the arbitrary number to the unsigned integer specified
// @param value -- value to set this to
//
*/
void ArbitraryLengthNumber::setUint32(uint32_t value) 
{
    if (_nbits < 32)
    {
        _nbits = 32;
        _data.resize(_nbits/8);
    }
    uint32_t v = value;
    for (unsigned int n = 0; n < 4; n++)
    {
        _data[n] = v & 0xFF;
        v >>= 8;
    }
}

/*!
// set the arbitrary number to the unsigned integer specified
// @param value -- value to set this to
// @param -- startbit -- starting bit to set
// @param -- endbit -- ending bit to set.
//
*/
void ArbitraryLengthNumber::setUint32(uint32_t value, unsigned int startBit, unsigned int endBit) 
{
    if (startBit > endBit)
        throw InternalError("ArbitraryLengthNumber::setUint32 -- startBit > endBit", __FILE__, __LINE__);
    if (((endBit-startBit) >= 32) || (endBit >= _nbits))
        throw InternalError("ArbitraryLengthNumber::setUint32 -- too many bits", __FILE__, __LINE__);
    // check for byte alignment of _nbits and start and end,
    // and make this more efficient...
    if ((_nbits == 32) && (startBit == 0) && (endBit == 31))
    {
        setUint32(value);
        return;
    }

    // there is further room for byte aligned begin and end bits..
    uint32_t v = value;
    uint8_t mask = (1 << (((_nbits-1)-endBit)%8));  
    unsigned int index = ((_nbits-1)-endBit)/8;
    uint8_t ch = _data[index];

    for (int bit = startBit; bit <= (int)endBit; bit++)
    {   
        #if 0
            cout << "bit = " << dec << bit 
                 << " _nbits " << _nbits 
                 << " index = " << index
                 << " mask = " << hex << setw(2) << setfill('0') << ((unsigned)mask & 0xFF) 
                 << endl << flush;
        #endif
        if (mask == 0)
        {
            _data[index] = ch;       // save the current character
            if (++index >= _data.size())
                throw InternalError("ArbitraryLengthNumber::setUint32 -- index out of range", __FILE__, __LINE__);
            ch = _data[index];    // pick up the new one...
            mask = 1;
        }
        ch = (ch & (~mask)) | ((v & 1) ? mask : 0);
        mask <<= 1;
        v >>= 1;
    }
    _data[index] = ch;      // save the last part of the current character...
}
/*!
// set the arbitrary number to the unsigned integer specified
// @param value -- value to set this to
//
*/
void ArbitraryLengthNumber::setUint64(uint64_t value) 
{
    if (_nbits < 64)
    {
        _nbits = 64;
        _data.resize(_nbits/8);
    }
    uint64_t v = value;
    for (unsigned int n = 0; n < 8; n++)
    {
        _data[n] = v & 0xFF;
        v >>= 8;
    }
}
/*!
// set the arbitrary number to the unsigned integer specified
// @param value -- value to set this to
// @param -- startbit -- starting bit to set
// @param -- endbit -- ending bit to set.
//
*/
void ArbitraryLengthNumber::setUint64(uint64_t value, unsigned int startBit, unsigned int endBit) 
{
    if (startBit > endBit)
        throw InternalError("ArbitraryLengthNumber:setUint64 -- startBit > endBit", __FILE__, __LINE__);
    if (((endBit-startBit) >= 64) || (endBit >= _nbits))
        throw InternalError("ArbitraryLengthNumber:setUint64 -- too many bits", __FILE__, __LINE__);

    if ((_nbits == 64) && (startBit == 0) && (endBit == 63))
    {
        setUint64(value);
        return;
    }

    uint64_t v = value;
    uint8_t mask = (1 << (((_nbits-1)-endBit)%8));  
    unsigned int index = ((_nbits-1)-endBit)/8;
    uint8_t ch = _data[index];

    for (int bit = startBit; bit <= (int)endBit; bit++)
    {   
        if (mask == 0)
        {
            _data[index] = ch;       // save the current character
            if (++index >= _data.size())
                throw InternalError("ArbitraryLengthNumber:setUint64 -- index out of range", __FILE__, __LINE__);
            ch = _data[index];    // pick up the new one...
            mask = 1;
        }
        ch = (ch & (~mask)) | ((v & 1) ? mask : 0);
        mask <<= 1;
        v >>= 1;
    }
    _data[index] = ch;      // save the last part of the current character...
    
}

/*!
// Retrieve a pointer to the raw data associated with this TDR..
//
// @param none
// @returns -- pointer to the raw data.
*/
const uint8_t *ArbitraryLengthNumber::getRawData() const 
{
    return(&_data[0]);
};

/*!
// set tdr raw data based on the raw data in the array.
// data is stored in the data array lsb byte to msb byte.  i.e.
// the same way the data is transmitted to the remote end
//
// @param bitcount -- number of bits to put in
// @param data -- pointer to data
*/
void ArbitraryLengthNumber::setRawData(unsigned bitcount, const uint8_t *data) 
{
    unsigned nbytes = (bitcount+7)/8;
    _nbits = bitcount;
    const uint8_t *psrc = data;
    _data.resize(nbytes);
    if (bitcount == 0)
        return;
    uint8_t bytemask = 0xFF;
    if( _nbits%8 != 0 )
       bytemask >>= 8 - (_nbits%8);
    uint8_t *pdest = &_data[0];
    if( nbytes > 1 )
    {
       for (unsigned n = 0; n < nbytes-1; n++)
          *pdest++ = *psrc++;
    }
    *pdest = (*psrc) & bytemask;
}

/*!
// set a raw byte in an already allocated data set.
//
// @param index -- index of byte to set.
// @param data -- data to set
// @returns none.
*/
void ArbitraryLengthNumber::setRawByte(unsigned index, uint8_t data)
{
    if (index >= _data.size())
        throw InternalError("ArbitraryLengthNumber:setRawByte -- index out of range", __FILE__, __LINE__);
    uint8_t bytemask = 0xFF;
    if( _nbits%8 != 0 )
       bytemask >>= 8 - (_nbits%8);
    _data[index] = data&bytemask;
}


/*!
// set a raw bit in the array... 0, is the lsb bit, 1 
//
// @param bitNum -- bit number of byte to set.
// @param data -- data to set
// @returns none.
*/
void ArbitraryLengthNumber::setRawBit(unsigned bitNum, uint8_t data)
{
    if ((bitNum+1) > _nbits)       // do we need to resize...
    {
        _nbits = bitNum+1;
        _data.resize((_nbits+7)/8);
    }

    uint8_t mask = (1 << (bitNum & 0x7));
    unsigned offset = bitNum >> 3;
    uint8_t bdata = ((data & 1) ? mask : 0);

    _data[offset] = (_data[offset] & (~mask)) | bdata;

}


/*!
// Retrieve a retrieve a raw data byte by index.
//
// @param index
// @returns -- data
*/
uint8_t ArbitraryLengthNumber::getRawByte(unsigned index) const
{
    if (index >= _data.size())
        throw InternalError("ArbitraryLengthNumber:getRawByte -- index out of range", __FILE__, __LINE__);
    uint8_t bytemask = 0xFF;
    if( _nbits%8 != 0 && index == _data.size()-1)
       bytemask >>= 8 - (_nbits%8);
    return(_data[index]&bytemask);

}

/*!
// set the data from a binary string.
// @param str -- binary string to set data from.
// @returns none.
*/
void ArbitraryLengthNumber::setBinaryString(const std::string &str, unsigned bitcount)
{
    if (bitcount)
    {
        setBitCount(bitcount);
        memset(&_data[0], 0, (bitcount+7)/8);
    }
    else
        setBitCount(str.size() ? str.size() : 1);
    std::string::const_reverse_iterator it;
    register uint8_t bit = 1;                           // bit position...
    unsigned byte = 0;
    register uint8_t d = 0;

    for (it = str.rbegin(); it != str.rend(); it++)
    {
        d |= ((*it == '1') ? bit : 0);
        bit <<= 1;
        if (bit == 0)
        {
            _data[byte++] = d;
            d = 0;
            bit = 1;
        }
    }
    if( str.size()%8 != 0 )
       _data[byte] = d;        // flush out the last value...
    
}


/*!
// get the data as a binary string.
// @returns binary string data.
*/
std::string ArbitraryLengthNumber::getBinaryString() const
{

    std::string value = "";
    unsigned startBit = 0;
    unsigned endBit = _nbits-1;
    uint8_t mask = (1 << (((_nbits-1)-startBit)%8));  
    unsigned int index = ((_nbits-1)-startBit)/8;
    uint8_t ch = _data[index];
    unsigned bit = startBit;
    for (; bit <= endBit; bit++)
    {   
       if (mask == 0)
       {
          if (index == 0)
             throw InternalError("ArbitraryLengthNumber:getBinaryString -- index out of range", __FILE__, __LINE__);
          ch = _data[--index];    // pick up the new one...
          mask = 0x80;
       }
       value.append( ((ch & mask) ? "1" : "0"));
       mask >>= 1;
    }
    return(value);
    
}

void ArbitraryLengthNumber::setHexString(const std::string &str, unsigned bitcount)
{

    unsigned bytecount = ((bitcount+7)/8);
    if (bitcount)
    {
        setBitCount(bitcount);
        memset(&_data[0], 0, bytecount);
    }
    else
    {
        bytecount = str.size() ? (str.size()+1)/2 : 1;      // 1 byte for every 2 nibbles, rounded up
        setBitCount(str.size() ? (str.size()*4) : 1);       // 4 bits per nibble
    }
    std::string::const_reverse_iterator it;
    register unsigned  nibble = 0;                           // bit position...
    unsigned byte = 0;

    for (it = str.rbegin(); it != str.rend(); it++)
    {
        register uint8_t d = 0;
        if (byte >= bytecount)      // end of the line...
            break;
        unsigned char ch = *it;
        if ((ch >= '0') && (ch <= '9'))
            d = ch - '0';
        else
        if ((ch >= 'A') && (ch <= 'F'))
            d = ch - 'A' + 10;
        else 
        if ((ch >= 'a') && (ch <= 'f'))
            d = ch - 'a' + 10;
        
        if (nibble++ & 1)       // even odd, even odd
            _data[byte++] |= d<<4;
        else
            _data[byte] = d;


    }

}

ArbitraryLengthNumber ArbitraryLengthNumber::reverse()
{
  ArbitraryLengthNumber arbNumber;
  arbNumber.setBitCount(1);
  arbNumber.setUint32(0,0,0);

  unsigned length = getBitCount();
  
  if(!length)
    return(arbNumber);
  arbNumber.setBitCount(length);

  for(unsigned pos=0; pos < length; pos++)
    {
      arbNumber.setBit(length-pos-1, getBit(pos));
    }
  return(arbNumber);
}
