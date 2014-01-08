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

#ifndef __ARBITRARY_NUMBER_H__
#define __ARBITRARY_NUMBER_H__


/*!
// extension/helper classes for manipulating the jtag PAL api.
//
// These classes are used to extract meaningful data from the jtag PALapi
//
*/

#include <vector>
#include <string>
#if _AIX
    #include <inttypes.h>
#else
    #include <stdint.h>
#endif

using namespace std;


/*!
// This class forms the base class used for TDR's, and DCR's when communicating
// with jtag devices
//
*/
class ArbitraryLengthNumber
{
protected:
    uint32_t         _nbits;
    vector<uint8_t>  _data;


public:
    /*!
    // @param bitcount -- total number of bits in the arbitrary number
    */
    ArbitraryLengthNumber(uint32_t bitCount = 0) 
    : _nbits( bitCount )                // size in bits.
    , _data( ((bitCount+7)/8), 0x00 )   // populate the vector with zeroed bytes. 
    {}

    virtual ~ArbitraryLengthNumber() {};

    bool operator==(const ArbitraryLengthNumber& other) const
    {
        return(_data == other._data);
    };

    bool operator!=(const ArbitraryLengthNumber& other) const
    {
        return(_data != other._data);
    };

    bool operator<(const ArbitraryLengthNumber& other) const
    {
        return(_data < other._data);
    };

    /*!
    // Convert the databits in this data into a hex string
    // @param  sepStr -- separator string -- string to insert as a separator
    // @param  sepInterval -- byte interval to place the seperator
    // @returns --  hex string of the contents.
    */
    const string getHexString(const string& sepStr = string(""), unsigned sepInterval = 4) const;
    
    /*!
    // Convert the databits in this data into a binary string
    // @param  sepStr -- separator string -- string to insert as a separator
    // @param  sepInterval -- byte interval to place the seperator
    // @returns --  binary string of the contents.
    */
    const string getBinString(const string& sepStr = string(""), unsigned sepInterval = 4) const;
    const string getBinStringRev(const string& sepStr = string(""), unsigned sepInterval = 4) const;

    /*!
    // Convert the databits in this data into a hex string
    // @param  startbit -- index of bit to start with
    // @param  endbit -- byte interval to place the seperator
    // @returns --  hex string of the contents.
    */
    const string getHexString(unsigned startbit, unsigned endbit) const;

    /*!
    // Retrieve the bit count for this number.
    // @returns -- number of bits in this arbitrary number.
    */
    const uint32_t getBitCount() const;

    /*!
    // Set/reset the total number of bits in this number.
    //
    // @param numbits -- total number of bits to set
    */
    void setBitCount(unsigned numbits);
    /*!
    // Retrieve the byte count for this number.
    // @returns -- number of byte in this arbitrary number.
    */
    const uint32_t getByteCount() const;

    /*!
    // Get the 32 bits (LSB) of this number and convert it to a uint32 in the
    // native byte format of the machine.
    // @param startbit -- starting bit
    // @param endbit --  ending bit...
    // @returns -- 1st 32 bits of the number.
    */
    const uint32_t getUint32(unsigned int startBit, unsigned int endBit) const;
    /*!
    // Get the 32 bits (LSB) of this number and convert it to a uint32 in the
    // native byte format of the machine.
    // @param startbit -- starting bit
    // @returns -- 1st 32 bits of the number.
    */
    const uint32_t getUint32(unsigned int startBit) const { return(getUint32(startBit, startBit+31));};
    /*!
         // Get the 32 bits (LSB) of this number and convert it to a uint32 in the
         // native byte format of the machine.
         // @returns -- 1st 32 bits of the number.
         */
    const uint32_t getUint32() const ;
    /*!
    // Get the 64 bits (LSB) of this number and convert it to a uint64 in the
    // native byte format of the machine.
    // @param startbit -- starting bit (default 0)
    // @param endbit -- ending bit (64)
    // @returns -- 1st 64 bits of the number.
    */
    const uint64_t getUint64(unsigned int startBit, unsigned int endBit) const;
    /*!
    // Get the 64 bits (LSB) of this number and convert it to a uint64 in the
    // native byte format of the machine.
    // @param startbit -- starting bit (default 0)
    // @returns -- 1st 64 bits of the number.
    */
    const uint64_t getUint64(unsigned int startBit) const { return(getUint64(startBit, startBit+63));};
    /*!
     // Get the 64 bits (LSB) of this number and convert it to a uint64 in the
     // native byte format of the machine.
     // @returns -- 1st 64 bits of the number.
     */
    const uint64_t getUint64() const ;
    const uint64_t getUint()  const {return(getUint64()); };
    

    /*!
    // set the arbitrary number to the unsigned integer specified
    // @param value -- value to set this to
    //
    */
    void setUint32(uint32_t value);
    /*!
    // set the arbitrary number to the unsigned integer specified
    // @param value -- value to set this to
    // @param startbit -- starting bit to set
    //
    */
    void setUint32(uint32_t value, unsigned int startBit) { setUint32(value, startBit, startBit+31);};

    /*!
    // set the arbitrary number to the unsigned integer specified
    // @param value -- value to set this to
    // @param startbit -- starting bit to set
    // @param endbit -- ending bit to set.
    //
    */
    void setUint32(uint32_t value, unsigned int startBit, unsigned int endBit) ;

    /*!
         // set the arbitrary number to the unsigned integer specified
         // @param value -- value to set this to
         //
         */
    void setUint64(uint64_t value) ;
    void setUint(uint64_t value)  { setUint64(value); };

    /*!
    // set the arbitrary number to the unsigned integer specified
    // @param value -- value to set this to
    // @param startbit -- starting bit to set
    //
    */
    void setUint64(uint64_t value, unsigned int startBit) { setUint64(value, startBit, startBit+63);};
    void setUint(uint64_t value, unsigned int startBit) { setUint64(value, startBit); };
    /*!
    // set the arbitrary number to the unsigned integer specified
    // @param value -- value to set this to
    // @param startbit -- starting bit to set
    // @param endbit -- ending bit to set.
    //
    */
    void setUint64(uint64_t value, unsigned int startBit, unsigned int endBit);
    void setUint(uint64_t value, unsigned int startBit, unsigned int endBit) {setUint64(value, startBit, endBit); };
    /*!
    // Retrieve a pointer to the raw data.
    //
    // @param none
    // @returns -- pointer to the raw data.
    */
    const uint8_t *getRawData() const ;
    /*!
    // Retrieve a raw data byte by index.
    //
    // @param index
    // @returns -- data
    */
    uint8_t getRawByte(unsigned index) const ;

    /*!
    // Get the bit at the specified location
    // @param bitnum - bit number ( bit 0 is MSB )
    // @returns -- 0 or 1
    */
    const uint32_t getBit(unsigned bit) const { return getUint32(bit, bit);}

    /*!
    // Set the bit at the specified location
    // @param bitnum - bit number ( bit 0 is MSB )
    // @param value  - 0 or 1
    */
    void setBit(unsigned bit, uint32_t value) { setUint32(value, bit, bit);}

    /*!
    // set raw data based on the raw data in the array.
    // data is stored in the data array lsb byte to msb byte.  i.e.
    // the same way the data is transmitted to the remote end
    //
    // @param bitcount -- number of bits to put in
    // @param data -- pointer to data
    */
    void setRawData(unsigned bitcount, const uint8_t *data);

    /*!
    // set a raw data byte in the array.
    //
    // @param index -- index of byte to set.
    // @param data -- data to set
    // @returns none.
    */
    void setRawByte(unsigned index, uint8_t data);

    /*!
    // set a raw bit in the array... 0, is the lsb bit, 1 
    //
    // @param index -- index of byte to set.
    // @param data -- data to set
    // @returns none.
    */
    void setRawBit(unsigned bitNum, uint8_t data);

    /*!
    // set the data from a binary string.
    // @param str -- binary string to set data from.
    // @param bitcount -- bit count (if not provided, set from string length).
    // @returns none.
    */
    void setBinaryString(const std::string &str, unsigned bitcount = 0);
    std::string getBinaryString() const;


    /*!
    // set an arbitrary number from a hex string                              
    // @param str -- binary string to set data from.                          
    // @param bitcount -- bit count (if not provided, set from string length).
    // @returns none.                                                         
    */
    void setHexString(const std::string &str, unsigned bitcount = 0);

    ArbitraryLengthNumber reverse();

protected:
    const bool isByteAligned(unsigned startBit, unsigned endBit, unsigned nbits) const
    {
        if ((((nbits-1-((int)startBit-1)) % 8)  == 0) && (((nbits-1-((int)endBit)) % 8) == 0) )
            return(true);
        else
            return(false);
    };


private:
};


////// scan ring values based on the BgpDiagArbitrary number concept...
////class BgpDiagScanRing : public ArbitraryLengthNumber 
////{
////public:
////    BgpDiagScanRing(int s) : ArbitraryLengthNumber(s) {};
////    BgpDiagScanRing()  : ArbitraryLengthNumber(0){};
////private:
////};


#endif
