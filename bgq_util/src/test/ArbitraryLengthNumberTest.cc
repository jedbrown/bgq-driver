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

#include <stdlib.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>
#include <bitset>
#include <stdexcept>

//#include "ArbitraryLengthNumberTest.h"
#include <bgq_util/include/ArbitraryLengthNumber.h>
#include <bgq_util/include/hexw.h>

//using namespace std:

void check64(const string &idstr, unsigned expByteCount, unsigned gotByteCount, unsigned lineno)
{

    if (expByteCount != gotByteCount)
    {
        cout << "ERROR : " << dec << "(" << lineno << ")" 
        << idstr << " " << dec 
        << "exp = " << expByteCount << " " 
        << "got = " << gotByteCount << endl;
        throw runtime_error("Test FAILED");
    }

}
void checkStr(const string &idstr, const string &expStr, const string &gotStr, unsigned lineno )
{
    if (expStr != gotStr)
    {
        cout << "ERROR : " << dec << "(" << lineno << ")" 
        << idstr << " " 
        << "exp = " << expStr << " " 
        << "got = " << gotStr << endl;
        throw runtime_error("Test FAILED");
    }
}

string getHexString(uint32_t pattern, unsigned patsize )
{
    ostringstream ostr;
    ostr << setfill('0') << setw(patsize) << hex << pattern;
    //cout << "string getHexString(uint32_t pattern, " << dec << patsize << ")" << endl;
    return(ostr.str());

}
string getHexString(uint64_t pattern, unsigned patsize )
{
    ostringstream ostr;
    ostr << setfill('0') << setw(patsize) << hex << pattern;
    //cout << "string getHexString(uint64_t pattern, " << dec << patsize << ")" << endl;
    return(ostr.str());

}

string getHexString( const uint8_t pattern[], unsigned patsize)
{
    ostringstream ostr;
    for (const uint8_t *p = pattern + patsize - 1; p >= pattern; p--)
        ostr << setfill('0') << setw(2) << hex << (unsigned)*p;

    return(ostr.str());
}
string getHexString(const uint32_t pattern[], unsigned patsize)
{
    ostringstream ostr;
    for (const uint32_t *p = pattern; p < pattern + patsize; p++)
        ostr << setfill('0') << setw(8) << hex << *p;

    return(ostr.str());
}

string getHexString(const uint64_t pattern[], unsigned patsize)
{
    ostringstream ostr;
    for (const uint64_t *p = pattern; p < pattern + patsize; p++)
        ostr << setfill('0') << setw(16) << hex << *p;

    return(ostr.str());
}


static ArbitraryLengthNumber checkBasic8( const uint8_t pattern[], unsigned patsize )
{
    // First, try some random-sized numbers
    for ( unsigned i = 0; i < 5; i++ )
    {
        unsigned randlen = (rand() % (patsize*8)) + 1;
        uint32_t bytelen = (randlen-1)/8 + 1;
        uint8_t bytemask = 0xFF;
        if ( randlen%8 != 0 )
            bytemask >>= 8 - (randlen%8);
        ArbitraryLengthNumber aln;
        aln.setRawData( randlen, pattern );
        cout << "bitlen = " << randlen << "; " << aln.getHexString() << endl;

        cout << std::dec;
        check64( "setRawData:getByteCount", bytelen, aln.getByteCount(), __LINE__ );
        check64( "setRawData:getBitCount", randlen, aln.getBitCount(), __LINE__  );
        cout << std::hex;

        const uint8_t* rawdata = aln.getRawData();
        if ( bytelen > 1 )
        {
            for ( unsigned byte = 0; byte < bytelen-1; byte++ )
            {
                check64( "setRawData:getRawData", pattern[byte], rawdata[byte], __LINE__ );
                check64( "setRawData:getRawByte", pattern[byte], aln.getRawByte(byte), __LINE__ );
            }
        }
        check64( "setRawData:getRawData", pattern[bytelen-1]&bytemask, rawdata[bytelen-1], __LINE__  );
        check64( "setRawData:getRawByte", pattern[bytelen-1]&bytemask, aln.getRawByte(bytelen-1), __LINE__  );
    }

    const unsigned patchunk = 1;
    ArbitraryLengthNumber aln(patsize*patchunk*8);
    aln.setRawData( patsize*patchunk*8, pattern );

    cout << std::dec;
    check64( "getByteCount", patsize*patchunk, aln.getByteCount(), __LINE__  );
    check64( "getBitCount", patsize*patchunk*8, aln.getBitCount(), __LINE__  );

    cout << std::hex;
    if ( patsize == 1 )
    {
        check64( "getUint32()", pattern[0], aln.getUint32(), __LINE__  );
        check64( "getUint64()", pattern[0], aln.getUint64(), __LINE__  );
    }

    const uint8_t* rawdata = aln.getRawData();
    for ( unsigned byte = 0; byte < aln.getByteCount(); byte++ )
    {
        check64( "getRawData", pattern[byte], rawdata[byte], __LINE__  );
        check64( "getRawByte", pattern[byte], aln.getRawByte(byte), __LINE__  );
    }

    checkStr( "getHexString()", getHexString(pattern,patsize), aln.getHexString(), __LINE__ );
    checkStr( "getHexString(0,b)", getHexString(pattern,patsize), aln.getHexString(0,aln.getBitCount()-1), __LINE__ );

    return aln;
}

static ArbitraryLengthNumber checkBasic32( const uint32_t pattern[], unsigned patsize )
{
    const unsigned patchunk = 4;
    ArbitraryLengthNumber aln(patsize*patchunk*8);
    aln.setUint32( pattern[0] );
    for ( unsigned i = 1; i < patsize; i++ )
    {
        aln.setUint32( pattern[i], i*patchunk*8 );
    }

    cout << std::dec;
    check64( "getByteCount", patsize*patchunk, aln.getByteCount(), __LINE__  );
    check64( "getBitCount", patsize*patchunk*8, aln.getBitCount(), __LINE__  );

    cout << std::hex;
    if ( patsize == 1 )
    {
        check64( "getUint32()", pattern[0], aln.getUint32(), __LINE__ );
    }

    for ( unsigned i = 0; i < patsize; i++ )
    {
        unsigned startbit = i*patchunk*8;
        check64( "getUint32(s)", pattern[i], aln.getUint32(startbit), __LINE__  );
        check64( "getUint32(s,e)", pattern[i], aln.getUint32(startbit,startbit+31), __LINE__  );
    }

    uint32_t mask = 0xFF;
    for ( unsigned byte = 0; byte < aln.getByteCount(); byte++ )
    {
        uint32_t checkval = pattern[patsize-(byte/patchunk)-1] & mask;
        checkval = checkval/((mask >> 8)+1);
        check64( "getRawByte", checkval, aln.getRawByte(byte), __LINE__  );
        mask <<= 8;
        if ( mask == 0 )
            mask = 0xFF;
    }

    for ( unsigned i = 0; i<10; i++ )
    {
        unsigned startbit = rand() % aln.getBitCount();
        unsigned endbit = startbit + (rand() % 32);
        if ( endbit >= aln.getBitCount() )
            endbit = aln.getBitCount() - 1;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;
        uint32_t mask = 0xFFFFFFFF;

        unsigned startpat = startbit/32;
        uint32_t checku32 = pattern[startpat];
        checku32 <<= (startbit%32);
        if ( startbit%32 != 0 )
            checku32 |= (pattern[startpat+1] >> (32 - (startbit%32) ) );
        if ( endbit - startbit + 1 != 32 )
            mask <<= 32 - (endbit-startbit+1);

        if ( startbit+31 < aln.getBitCount() )
        {
            check64( "getUint32(s)(unaligned)", checku32, aln.getUint32(startbit), __LINE__  );
            check64( "getUint32(s,s+31)(unaligned)", checku32, aln.getUint32(startbit, startbit+31), __LINE__  );
        }
        checku32 &= mask;
        if ( endbit - startbit + 1 != 32  )
            checku32 = checku32/(((~mask)+1));
        //if( endbit - startbit + 1 != 32 && endbit%32 != 0 )
        //checku32 >>= (32 - endbit-startbit+1);
        check64( "getUint32(s,e)(unaligned)", checku32, aln.getUint32(startbit,endbit), __LINE__  );

        checkStr( "getHexString(s,e)", getHexString(checku32, (((endbit-startbit)/8)+1)*2), aln.getHexString(startbit,endbit), __LINE__);      
    }

    checkStr( "getHexString()", getHexString(pattern,patsize), aln.getHexString(), __LINE__ );
    checkStr( "getHexString(0,b)", getHexString(pattern,patsize), aln.getHexString(0,aln.getBitCount()-1), __LINE__ );

    return aln;
}

#ifdef __LP64__
static ArbitraryLengthNumber checkBasic64( const uint64_t pattern[], unsigned patsize )
{
    const unsigned patchunk = 8;
    ArbitraryLengthNumber aln(patsize*patchunk*8);
    aln.setUint64( pattern[0] );
    for ( unsigned i = 1; i < patsize; i++ )
    {
        aln.setUint64( pattern[i], i*patchunk*8 );
    }

    cout << std::dec;
    check64( "getByteCount", patsize*patchunk, aln.getByteCount(), __LINE__  );
    check64( "getBitCount", patsize*patchunk*8, aln.getBitCount(), __LINE__  );

    cout << std::hex;
    if ( patsize == 1 )
    {
        check64( "getUint()", pattern[0], aln.getUint(), __LINE__ );
        check64( "getUint64()", pattern[0], aln.getUint64(), __LINE__ );
    }

    for ( unsigned i = 0; i < patsize; i++ )
    {
        unsigned startbit = i*patchunk*8;
        check64( "getUint64(s)", pattern[i], aln.getUint64(startbit), __LINE__ );
        check64( "getUint64(s,e)", pattern[i], aln.getUint64(startbit,startbit+63), __LINE__ );
    }

    uint64_t mask = 0xFF;
    for ( unsigned byte = 0; byte < aln.getByteCount(); byte++ )
    {
        uint64_t checkval = pattern[patsize-(byte/patchunk)-1] & mask;
        checkval = checkval/((mask >> 8)+1);
        check64( "getRawByte", checkval, aln.getRawByte(byte), __LINE__ );
        mask <<= 8;
        if ( mask == 0 )
            mask = 0xFF;
    }

    for ( unsigned i = 0; i<10; i++ )
    {
        unsigned startbit = rand() % aln.getBitCount();
        unsigned endbit = startbit + (rand() % 64);
        if ( endbit >= aln.getBitCount() )
            endbit = aln.getBitCount() - 1;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;
        uint64_t mask = 0xFFFFFFFFFFFFFFFF;

        unsigned startpat = startbit/64;
        uint64_t checku64 = pattern[startpat];
        checku64 <<= (startbit%64);
        if ( startbit%64 != 0 )
            checku64 |= (pattern[startpat+1] >> (64 - (startbit%64)) );
        if ( endbit - startbit != 63 )
            mask <<= 63 - (endbit-startbit);

        if ( startbit+63 < aln.getBitCount() )
        {
            check64( "getUint64(s)(unaligned)", checku64, aln.getUint64(startbit), __LINE__ );
            check64( "getUint64(s,s+63)(unaligned)", checku64, aln.getUint64(startbit, startbit+63), __LINE__ );
        }
        checku64 &= mask;
        checku64 = checku64/((~mask)+1);
        check64( "getUint64(s,e)(unaligned)", checku64, aln.getUint64(startbit,endbit), __LINE__ );

        checkStr( "getHexString(s,e)", getHexString(checku64, (((endbit-startbit)/8)+1)*2), aln.getHexString(startbit,endbit), __LINE__);      
    }

    checkStr( "getHexString()", getHexString(pattern,patsize), aln.getHexString(), __LINE__ );
    cout << aln.getHexString(" ",8) << endl;
    checkStr( "getHexString(0,b)", getHexString(pattern,patsize), aln.getHexString(0,aln.getBitCount()-1), __LINE__ );

    return aln;
}
#endif

static void shuffle( ArbitraryLengthNumber aln )
{
    uint32_t bitcount = aln.getBitCount();
    uint32_t bytecount = aln.getByteCount();

    // Check get/set of random bits
    for ( unsigned i= 0; i<10; i++ )
    {
        cout << "shuffle before " << aln.getHexString() << endl;
        uint32_t bitpos = rand() % bitcount;
        uint8_t bit = rand() % 2;

        uint32_t checkbyte = bitpos/8;
        uint8_t check1 = 0;
        uint8_t check2 = 0; 
        uint8_t check3 = 0;
        cout << dec << "bitpos = " << bitpos << "; bit = " << (unsigned)bit << "; checkbyte = " << checkbyte << hex << endl;
        if ( checkbyte > 0 )
            check1 = aln.getRawByte( checkbyte-1 );
        check2 = aln.getRawByte( checkbyte );
        if ( checkbyte < bytecount-1 )
            check3 = aln.getRawByte( checkbyte+1 );

        uint32_t checkbit = aln.getBit( bitcount - bitpos - 1 );
        aln.setRawBit( bitpos, checkbit );
        check64( "setRawBit(0)", checkbit, aln.getBit( bitcount - bitpos - 1 ), __LINE__ );

        if ( checkbyte > 0 )
            check64( "setRawBit(1)", check1, aln.getRawByte( checkbyte-1 ), __LINE__ );
        check64( "setRawBit(2)", check2, aln.getRawByte( checkbyte ), __LINE__ );
        if ( checkbyte < bytecount-1 )
            check64( "setRawBit(3)", check3, aln.getRawByte( checkbyte+1 ), __LINE__ );

        checkbit = bit;
        cout << "shuffle before " << aln.getHexString() << endl;
        aln.setRawBit( bitpos, bit );
        cout << "shuffle after  " << aln.getHexString() << endl;
        check64( "setRawBit(4)", checkbit, aln.getBit( bitcount - bitpos - 1 ), __LINE__ );
        uint8_t mask = 1 << (bitpos%8);
        if ( bit )
            check2 |= mask;
        else
            check2 &= ~mask;

        if ( checkbyte > 0 )
            check64( "setRawBit(5)", check1, aln.getRawByte( checkbyte-1 ), __LINE__ );
        check64( "setRawBit(6)", check2, aln.getRawByte( checkbyte ), __LINE__ );
        if ( checkbyte < bytecount-1 )
            check64( "setRawBit(7)", check3, aln.getRawByte( checkbyte+1 ), __LINE__ );
    }

    // Check get/set of random bits
    for ( unsigned i= 0; i<10; i++ )
    {
        uint32_t bitpos = rand() % bitcount;
        uint32_t bit = rand() % 2;
        cout << dec << "bitpos = " << bitpos << "; bit = " << bit << hex << endl;

        uint32_t checkbyte = bytecount - bitpos/8 - 1;
        uint8_t check1 = 0;
        uint8_t check2 = 0; 
        uint8_t check3 = 0;
        if ( checkbyte > 0 )
            check1 = aln.getRawByte( checkbyte-1 );
        check2 = aln.getRawByte( checkbyte );
        if ( checkbyte < bytecount-1 )
            check3 = aln.getRawByte( checkbyte+1 );

        uint32_t checkbit = aln.getBit( bitpos );
        aln.setBit( bitpos, checkbit );
        check64( "setBit(0)", checkbit, aln.getBit( bitpos ), __LINE__ );

        if ( checkbyte > 0 )
            check64( "setBit(1)", check1, aln.getRawByte( checkbyte-1 ), __LINE__ );
        check64( "setBit(2)", check2, aln.getRawByte( checkbyte ), __LINE__ );
        if ( checkbyte < bytecount-1 )
            check64( "setBit(3)", check3, aln.getRawByte( checkbyte+1 ), __LINE__ );

        aln.setBit( bitpos, bit );
        check64( "setBit(4)", bit, aln.getBit( bitpos ), __LINE__ );
        uint8_t mask = 0x80 >> (bitpos%8);
        if ( bit )
            check2 |= mask;
        else
            check2 &= ~mask;

        if ( checkbyte > 0 )
            check64( "setBit(5)", check1, aln.getRawByte( checkbyte-1 ), __LINE__ );
        check64( "setBit(6)", check2, aln.getRawByte( checkbyte ), __LINE__ );
        if ( checkbyte < bytecount-1 )
            check64( "setBit(7)", check3, aln.getRawByte( checkbyte+1 ), __LINE__ );
    }

    // Check get/set of random bytes
    for ( unsigned i= 0; i<10; i++ )
    {
        uint32_t bytepos = rand() % bytecount;
        uint8_t byteval = rand() % 16;

        uint8_t check1 = 0;
        uint8_t check2 = 0; 
        uint8_t check3 = 0;
        if ( bytepos > 0 )
            check1 = aln.getRawByte( bytepos-1 );
        check2 = aln.getRawByte( bytepos );
        if ( bytepos < bytecount-1 )
            check3 = aln.getRawByte( bytepos+1 );

        aln.setRawByte( bytepos, check2 );

        if ( bytepos > 0 )
            check64( "setRawByte(1)", check1, aln.getRawByte( bytepos-1 ), __LINE__ );
        check64( "setRawByte(2)", check2, aln.getRawByte( bytepos ), __LINE__ );
        if ( bytepos < bytecount-1 )
            check64( "setRawByte(3)", check3, aln.getRawByte( bytepos+1 ), __LINE__ );

        aln.setRawByte( bytepos, byteval );

        if ( bytepos > 0 )
            check64( "setRawByte(4)", check1, aln.getRawByte( bytepos-1 ), __LINE__ );
        check64( "setRawByte(5)", byteval, aln.getRawByte( bytepos ), __LINE__ );
        if ( bytepos < bytecount-1 )
            check64( "setRawByte(6)", check3, aln.getRawByte( bytepos+1 ), __LINE__ );
    }

    // Get/set random uint32
    int bytes = (int)bytecount;
    for ( unsigned i = 0; i<10; i++ )
    {
        uint32_t value = rand() % 0xFFFFFFFF;

        uint32_t checku32 = aln.getUint32(bitcount-32,bitcount-1);
        aln.setUint32( checku32 );
        check64( "setUint32():0", checku32, aln.getUint32(bitcount-32,bitcount-1), __LINE__ );

        aln.setUint32( value );
        check64( "setUint32():1", value, aln.getUint32(bitcount-32,bitcount-1), __LINE__ );

        unsigned startbit = rand() % (aln.getBitCount()-31);
        unsigned endbit = startbit + 31;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;

        aln.setUint32( checku32, startbit );
        check64( "setUint32:1(s)", checku32, aln.getUint32( startbit ), __LINE__ );

        int checkbyte1 = bytecount - startbit/8 - 1;
        uint8_t check1 = 0;
        uint8_t check1mask = 0xFF;
        if ( checkbyte1 > 0 )
        {
            check1 = aln.getRawByte( checkbyte1 );
            check1mask <<= 8-(startbit%8);
        }
        int checkbyte2 = bytecount - endbit/8 - 1;
        uint8_t check2 = 0;
        uint8_t check2mask = 0xFF;
        if ( checkbyte2 < bytes-1 )
        {
            check2 = aln.getRawByte( checkbyte2 );
            check2mask >>= ((endbit%8) + 1);
        }

        checku32 = aln.getUint32(startbit);
        aln.setUint32( checku32, startbit );
        check64( "setUint32(s):2", checku32, aln.getUint32(startbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint32(s):3", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint32(s):4", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );

        aln.setUint32( value, startbit );
        check64( "setUint32(s):5", value, aln.getUint32(startbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint32(s):6", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint32(s):7", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );


        startbit = rand() % (aln.getBitCount()-31);
        endbit = startbit + (rand() % 32);
        if ( endbit >= aln.getBitCount() )
            endbit = aln.getBitCount() - 1;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;

        checkbyte1 = bytecount - startbit/8 - 1;
        check1 = 0;
        check1mask = 0xFF;
        if ( checkbyte1 > 0 )
        {
            check1 = aln.getRawByte( checkbyte1 );
            check1mask <<= 8-(startbit%8);
        }
        checkbyte2 = bytecount - endbit/8 - 1;
        check2 = 0;
        check2mask = 0xFF;
        if ( checkbyte2 < bytes-1 )
        {
            check2 = aln.getRawByte( checkbyte2 );
            check2mask >>= ((endbit%8) + 1);
        }

        checku32 = aln.getUint32(startbit, endbit);

        aln.setUint32( checku32, startbit, endbit );
        check64( "setUint32:2(s,e):0", checku32, aln.getUint32( startbit, endbit ), __LINE__ );      
        if ( checkbyte1 > 0 )
            check64( "setUint32(s,e):1", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint32(s,e):2", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );

        aln.setUint32( value, startbit, endbit );
        uint32_t valuemask = 0xFFFFFFFF;
        valuemask >>= 32 - (endbit-startbit)%32 - 1;
        check64( "getUint32(s,e):3", value&valuemask, aln.getUint32(startbit,endbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint32(s,e):4", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint32(s,e):5", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );


    }
}

#ifdef __LP64__
static void shuffle64( ArbitraryLengthNumber aln )
{
    uint32_t bitcount = aln.getBitCount();
    uint32_t bytecount = aln.getByteCount();
    int bytes = bytecount;

    // Get/set random uint64
    for ( unsigned i = 0; i<10; i++ )
    {
        uint64_t value = rand() % 0xFFFFFFFFFFFFFFFF;

        uint64_t checku64 = aln.getUint64(bitcount-64,bitcount-1);
        aln.setUint64( checku64 );
        check64( "setUint64():0", checku64, aln.getUint64(bitcount-64,bitcount-1), __LINE__ );

        aln.setUint64( value );
        check64( "setUint64():1", value, aln.getUint64(bitcount-64,bitcount-1), __LINE__ );

        unsigned startbit = rand() % (aln.getBitCount()-63);
        unsigned endbit = startbit + 63;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;

        aln.setUint64( checku64, startbit );
        check64( "setUint64:1(s)", checku64, aln.getUint64( startbit ), __LINE__ );

        int checkbyte1 = bytecount - startbit/8 - 1;
        uint8_t check1 = 0;
        uint8_t check1mask = 0xFF;
        if ( checkbyte1 > 0 )
        {
            check1 = aln.getRawByte( checkbyte1 );
            check1mask <<= 8-(startbit%8);
        }
        int checkbyte2 = bytecount - endbit/8 - 1;
        uint8_t check2 = 0;
        uint8_t check2mask = 0xFF;
        if ( checkbyte2 < bytes-1 )
        {
            check2 = aln.getRawByte( checkbyte2 );
            check2mask >>= ((endbit%8) + 1);
        }

        checku64 = aln.getUint64(startbit);
        aln.setUint64( checku64, startbit );
        check64( "setUint64(s):2", checku64, aln.getUint64(startbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint64(s):3", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint64(s):4", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );

        aln.setUint64( value, startbit );
        check64( "setUint64(s):5", value, aln.getUint64(startbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint64(s):6", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint64(s):7", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );


        startbit = rand() % (aln.getBitCount()-63);
        endbit = startbit + (rand() % 64);
        if ( endbit >= aln.getBitCount() )
            endbit = aln.getBitCount() - 1;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;

        checkbyte1 = bytecount - startbit/8 - 1;
        check1 = 0;
        check1mask = 0xFF;
        if ( checkbyte1 > 0 )
        {
            check1 = aln.getRawByte( checkbyte1 );
            check1mask <<= 8-(startbit%8);
        }
        checkbyte2 = bytecount - endbit/8 - 1;
        check2 = 0;
        check2mask = 0xFF;
        if ( checkbyte2 < bytes-1 )
        {
            check2 = aln.getRawByte( checkbyte2 );
            check2mask >>= ((endbit%8) + 1);
        }

        checku64 = aln.getUint64(startbit, endbit);

        aln.setUint64( checku64, startbit, endbit );
        check64( "setUint64:2(s,e):0", checku64, aln.getUint64( startbit, endbit ), __LINE__ );      
        if ( checkbyte1 > 0 )
            check64( "setUint64(s,e):1", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask , __LINE__);
        if ( checkbyte2 < bytes-1 )
            check64( "setUint64(s,e):2", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );

        aln.setUint64( value, startbit, endbit );
        uint64_t valuemask = 0xFFFFFFFFFFFFFFFF;
        valuemask >>= 64 - (endbit-startbit)%64 - 1;
        check64( "getUint64(s,e):3", value&valuemask, aln.getUint64(startbit,endbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint64(s,e):4", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint64(s,e):5", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );


    }
    // Get/set random uint64
    for ( unsigned i = 0; i<10; i++ )
    {
        uint64_t value = rand() % 0xFFFFFFFFFFFFFFFF;

        uint64_t checku64 = aln.getUint64(bitcount-64,bitcount-1);
        aln.setUint( checku64 );
        check64( "setUint():0", checku64, aln.getUint64(bitcount-64,bitcount-1), __LINE__ );

        aln.setUint( value );
        check64( "setUint():1", value, aln.getUint64(bitcount-64,bitcount-1), __LINE__ );

        unsigned startbit = rand() % (aln.getBitCount()-63);
        unsigned endbit = startbit + 63;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;

        aln.setUint( checku64, startbit );
        check64( "setUint:1(s)", checku64, aln.getUint64( startbit ), __LINE__ );

        int checkbyte1 = bytecount - startbit/8 - 1;
        uint8_t check1 = 0;
        uint8_t check1mask = 0xFF;
        if ( checkbyte1 > 0 )
        {
            check1 = aln.getRawByte( checkbyte1 );
            check1mask <<= 8-(startbit%8);
        }
        int checkbyte2 = bytecount - endbit/8 - 1;
        uint8_t check2 = 0;
        uint8_t check2mask = 0xFF;
        if ( checkbyte2 < bytes-1 )
        {
            check2 = aln.getRawByte( checkbyte2 );
            check2mask >>= ((endbit%8) + 1);
        }

        checku64 = aln.getUint64(startbit);
        aln.setUint( checku64, startbit );
        check64( "setUint(s):2", checku64, aln.getUint64(startbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint(s):3", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint(s):4", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );

        aln.setUint( value, startbit );
        check64( "setUint(s):5", value, aln.getUint64(startbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint(s):6", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint(s):7", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );


        startbit = rand() % (aln.getBitCount()-63);
        endbit = startbit + (rand() % 64);
        if ( endbit >= aln.getBitCount() )
            endbit = aln.getBitCount() - 1;
        cout << std::dec << "s = " << startbit << "; e = " << endbit << std::hex << endl;

        checkbyte1 = bytecount - startbit/8 - 1;
        check1 = 0;
        check1mask = 0xFF;
        if ( checkbyte1 > 0 )
        {
            check1 = aln.getRawByte( checkbyte1 );
            check1mask <<= 8-(startbit%8);
        }
        checkbyte2 = bytecount - endbit/8 - 1;
        check2 = 0;
        check2mask = 0xFF;
        if ( checkbyte2 < bytes-1 )
        {
            check2 = aln.getRawByte( checkbyte2 );
            check2mask >>= ((endbit%8) + 1);
        }

        checku64 = aln.getUint64(startbit, endbit);

        aln.setUint( checku64, startbit, endbit );
        check64( "setUint:2(s,e):0", checku64, aln.getUint64( startbit, endbit ), __LINE__ );      
        if ( checkbyte1 > 0 )
            check64( "setUint(s,e):1", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint(s,e):2", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );

        aln.setUint( value, startbit, endbit );
        uint64_t valuemask = 0xFFFFFFFFFFFFFFFF;
        valuemask >>= 64 - (endbit-startbit)%64 - 1;
        check64( "getUint(s,e):3", value&valuemask, aln.getUint64(startbit,endbit), __LINE__ );
        if ( checkbyte1 > 0 )
            check64( "setUint(s,e):4", check1&check1mask, aln.getRawByte(checkbyte1)&check1mask, __LINE__ );
        if ( checkbyte2 < bytes-1 )
            check64( "setUint(s,e):5", check2&check2mask, aln.getRawByte(checkbyte2)&check2mask, __LINE__ );


    }
}      
#endif

static void resize( ArbitraryLengthNumber aln )
{

    // Add uint32
    for ( unsigned i= 0; i<5; i++ )
    {
        uint32_t newbitcount = rand() % 32;
        aln.setBitCount( newbitcount );
        uint32_t bitcount = aln.getBitCount();
        check64( "setBitCount", newbitcount, bitcount, __LINE__ );
        uint32_t value = rand() % 0xFFFFFFFF;

        aln.setUint32( value );

        uint32_t bytecount = aln.getByteCount();
        check64( "setUint32(v):len", 32, aln.getBitCount( ), __LINE__ );
        check64( "setUint32(v):byte", 4, bytecount, __LINE__ );
        check64( "setUint32(v):value", value, aln.getUint32(), __LINE__ );

    }

    // Add bits
    for ( unsigned i= 0; i<5; i++ )
    {
        uint32_t bitcount = aln.getBitCount();

        uint32_t bitpos = bitcount + (rand() % 66);
        uint8_t bit = rand() % 2;

        uint8_t lastbit = aln.getBit( 0 );
        cout << dec << "bitpos = " << bitpos << "; bit = " << (unsigned)bit << hex << endl;

        cout << "shuffle before (" << dec << bitcount << ") " << aln.getHexString() << endl;
        aln.setRawBit( bitpos, bit );
        uint32_t newbitcount = aln.getBitCount();
        cout << "shuffle after (" << dec << newbitcount << ") " << aln.getHexString() << endl;

        check64( "setRawBit:bit", bit, aln.getBit( 0 ), __LINE__ );
        check64( "setRawBit:lastbit", lastbit, aln.getBit( newbitcount - bitcount ), __LINE__ );
        check64( "setRawBit:len", bitpos+1, newbitcount, __LINE__ );

    }


#ifdef __LP64__
    // Add uint64
    for ( unsigned i= 0; i<5; i++ )
    {
        uint32_t newbitcount = rand() % 65;
        aln.setBitCount( newbitcount );
        uint32_t bitcount = aln.getBitCount();
        check64( "setBitCount", newbitcount, bitcount, __LINE__ );
        uint32_t value = rand() % 0xFFFFFFFFFFFFFFFF;

        aln.setUint64( value );

        uint32_t bytecount = aln.getByteCount();
        check64( "setUint64(v):len", 64, aln.getBitCount( ), __LINE__ );
        check64( "setUint64(v):byte", 8, bytecount, __LINE__ );
        check64( "setUint64(v):value", value, aln.getUint64(), __LINE__ );

    }
#endif   
}

void checkbits( uint32_t value )
{
    cout << "checkbits value = " << hex << value << endl; 

    const bitset<32> bits(value);
    std::ostringstream bitstr;
    bitstr << bits;
    cout << "bitstr = " << bitstr.str().c_str() << endl;

    ArbitraryLengthNumber aln;
    aln.setBinaryString( bitstr.str() );
    check64( "setBinaryString:len", 32, aln.getBitCount(), __LINE__ );
    check64( "setBinaryString:byte", 4, aln.getByteCount(), __LINE__ );
    checkStr( "setBinaryString:getHexString ", getHexString( &value, 1 ), aln.getHexString(), __LINE__ );
    check64( "setBinaryString:getUint32 ", value, aln.getUint32(), __LINE__ );

}

void setbinarystring( )
{
    uint32_t bitlen = rand() % 256;
    cout << "setbinarystring bitlen = " << dec << bitlen << hex << endl;

    ArbitraryLengthNumber aln(bitlen);
    std::string bitstring;

    for ( unsigned i=1; i< bitlen+9; i++ )
    {
        unsigned randbit = rand() % 2;
        bitstring.append( randbit ? "1" : "0" );
        aln.setBinaryString( bitstring.c_str() );
        check64( "setBinaryString:len", i, aln.getBitCount(), __LINE__ );
        check64( "setBinaryString:bit", randbit, aln.getBit(i-1), __LINE__ );
        checkStr( "getBinaryString", bitstring, aln.getBinaryString(), __LINE__ );
    }
}

void setrawbit()
{
    uint32_t bitlen = (rand() % 256) + 1;
    cout << "sethexstring bitlen = " << dec << bitlen << hex << endl;

    ArbitraryLengthNumber aln;

    for ( unsigned i=1; i< bitlen; i++ )
    {
        unsigned randbit = rand() % 2;
        aln.setRawBit( i, randbit );
        check64( "setRawBit:len", i+1, aln.getBitCount(), __LINE__ );
        check64( "setRawBit:bit", randbit, aln.getBit(0), __LINE__ );
    }
}

void sethexstring( )
{
    uint32_t bitlen = rand() % 256;
    cout << "sethexstring bitlen = " << dec << bitlen << hex << endl;

    ArbitraryLengthNumber aln(bitlen);
    std::string bitstring;

    for ( unsigned i=1; i< bitlen+9; i++ )
    {
        unsigned randbit = rand() % 2;
        bitstring.append( randbit ? "1" : "0" );
        aln.setBinaryString( bitstring.c_str() );
        string hexstr = aln.getHexString();     // convert from hex, back to hex and back and compare
        ArbitraryLengthNumber hln; hln.setHexString(hexstr, i);
        check64( "setBinaryString:len", i, hln.getBitCount(), __LINE__ );
        check64( "sethexstring:bit", randbit, hln.getBit(i-1), __LINE__ );
        checkStr( "sethexstring:getBinaryString", hexstr, hln.getHexString(), __LINE__);
    }
}


/*!
main does it all
*/
int main( int argc, char* argv[] )
{
    try
    {
        srand( time(NULL));

        cout << "===Basic8===" << endl;
        const uint8_t pat8[] = {0xAB,0xCD,0xEF,0x12,0x34};
        ArbitraryLengthNumber aln6 = checkBasic8( pat8, 5 );
        ArbitraryLengthNumber aln7 = checkBasic8( pat8, 5 );
        assert( aln6 == aln7 );
        const uint8_t patF8[] = {0xFF};
        ArbitraryLengthNumber alnF8 = checkBasic8( patF8, 1 );
        const uint8_t pat08[] = {0};
        ArbitraryLengthNumber aln08 = checkBasic8( pat08, 1 );
        assert( aln08 != aln7 );
        assert( aln6 < alnF8 );   

        cout << "===Basic32===" << endl;
        const uint32_t pat32[] = {0,0,0xABCDEF01,0x23456789};
        ArbitraryLengthNumber aln1 = checkBasic32( pat32, 4 );
        ArbitraryLengthNumber aln2 = checkBasic32( pat32, 4 );
        assert( aln1 == aln2 );

        const uint32_t pat0[] = {0};
        ArbitraryLengthNumber aln0 = checkBasic32( pat0, 1 );
        const uint32_t patF32[] = {0xFFFFFFFF};
        ArbitraryLengthNumber alnF32 = checkBasic32( patF32, 1 );
        assert( aln0 != alnF32 );
        assert( aln0 != aln1 );
        assert( alnF32 != aln2 );
        assert( aln0 < aln2 );

        cout << "===Shuffle aln1===" << endl;   
        shuffle( aln1 );
        cout << "===Shuffle aln0===" << endl;   
        shuffle( aln0 );
        cout << "===Shuffle alnF32===" << endl;   
        shuffle( alnF32 );
        cout << "===Resize aln2===" << endl;   
        resize( aln2 );
        cout << "===Resize alnF32===" << endl;   
        resize( alnF32 );
        cout << "===Resize aln0===" << endl;   
        resize( aln0 );

#ifdef __LP64__
        cout << "===Basic64===" << endl;
        const uint64_t pat64[] = {0,0,0xABCDEF0123456789,0xFFFFFFFFFFFFFFFF};
        ArbitraryLengthNumber aln4 = checkBasic64( pat64, 4 );
        ArbitraryLengthNumber aln5 = checkBasic64( pat64, 4 );
        assert( aln4 == aln5 );

        const uint64_t pat064[] = {0};
        ArbitraryLengthNumber aln064 = checkBasic64( pat064, 1 );
        const uint64_t patF64[] = {0xFFFFFFFFFFFFFFFF};
        ArbitraryLengthNumber alnF64 = checkBasic64( patF64, 1 );
        assert( aln064 != alnF64 );
        assert( aln064 != aln4 );
        assert( alnF64 != aln5 );
        assert( aln064 < aln5 );


        cout << "===Shuffle64 aln5===" << endl;   
        shuffle64( aln5 );
        cout << "===Shuffle64 alnF64===" << endl;   
        shuffle64( alnF64 );
        cout << "===Shuffle64 aln064===" << endl;   
        shuffle64( aln064 );
        cout << "===Shuffle aln4===" << endl;   
        shuffle( aln4 );
        cout << "===Shuffle alnF64===" << endl;   
        shuffle( alnF64 );
        cout << "===Shuffle aln064===" << endl;   
        shuffle( aln064 );
        cout << "===Resize aln4===" << endl;   
        resize( aln4 );
        cout << "===Resize alnF64===" << endl;   
        resize( alnF64 );
        cout << "===Resize aln064===" << endl;   
        resize( aln064 );

#endif

        checkbits( 0 );
        checkbits( 0xFFFFFFFF );
        checkbits( 0xABCDEF01 );

        setbinarystring();
        setbinarystring();
        setrawbit();
        setrawbit();


        sethexstring();
        sethexstring();
    }
    catch (exception &e)
    {
        cout << "" << e.what() << endl;   
        if (e.what() != "Test FAILED")        
            cout << "Test FAILED" << endl;            
        return(1);                                
    }
    catch (...)
    {
        cout << "\nUnknown exception " << endl;   
        cout << "Test FAILED" << endl;            
        return(1);                                
    }

    //getHexString("hi");
    return 0;
}
