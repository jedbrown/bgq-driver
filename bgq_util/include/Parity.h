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

#ifndef __PARITY_H__
#define __PARITY_H__

class ParityOdd
{
public:
    /*!
    // Compute a 8 bit odd parity
    //
    // @param ulValue -- value to compute the parity on
    // @returns odd parity 0 or 1
    //
    */
    static unsigned parity8(unsigned ulValue)
    {
        unsigned ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >> 4);
        ulPar = ulPar ^ (ulPar >> 2);
        ulPar = ulPar ^ (ulPar >> 1);
    
        return((ulPar & 1) ^ 1);        // final flip to make it odd..
    };
    
    /*!
    // Compute a 16 bit odd parity
    //
    // @param ulValue -- value to compute the parity on
    // @returns odd parity 0 or 1
    //
    */
    static unsigned parity16(unsigned ulValue)
    {
        unsigned ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >> 8);
        ulPar = ulPar ^ (ulPar >> 4);
        ulPar = ulPar ^ (ulPar >> 2);
        ulPar = ulPar ^ (ulPar >> 1);
    
        return((ulPar & 1) ^ 1);        // final flip to make it odd..
    };
    /*!
    // Compute a 32 bit odd parity
    //
    // @param nValue -- value to compute the parity on
    // @returns odd parity 0 or 1
    //
    */
    static unsigned parity32(unsigned ulValue)
    {
        unsigned ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >>16);
        ulPar = ulPar ^ (ulPar >> 8);
        ulPar = ulPar ^ (ulPar >> 4);
        ulPar = ulPar ^ (ulPar >> 2);
        ulPar = ulPar ^ (ulPar >> 1);
    
        return((ulPar & 1) ^ 1);        // final flip to make it odd..
    };

    static unsigned parity64(uint64_t ulValue)
    {
        uint64_t ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >> 32);
        ulPar = ulPar ^ (ulPar >> 16);
        ulPar = ulPar ^ (ulPar >>  8);
        ulPar = ulPar ^ (ulPar >>  4);
        ulPar = ulPar ^ (ulPar >>  2);
        ulPar = ulPar ^ (ulPar >>  1);
    
        return((ulPar & 1) ^ 1);        // final flip to make it odd..
    };
    
    /*!
    // Check the parity of an aribitrary number.
    //
    // @param data -- number to check
    // @returns odd parity for the total number of bits in 8 bit groups...
    //
    */
    static unsigned parity(const ArbitraryNumber &data)
    {
        int nBitCount = data.getBitCount();
    
        unsigned nPar = 0;

        int n;
        for (n = 0; n < nBitCount&(-8); n += 8)
            nPar = nPar ^ (parity8(data.getUint32(n, n+7)) & 1);
        for (n = 0; n < nBitCount; n++)
            nPar = nPar ^ data.getUint32(n,n);

        return((nPar & 1) ^ 1);        // final flip to make it odd..
    }

};

class ParityEven
{
public:
    /*!
    // Compute a 8 bit odd parity
    //
    // @param ulValue -- value to compute the parity on
    // @returns odd parity 0 or 1
    //
    */
    static unsigned parity8(unsigned ulValue)
    {
        unsigned ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >> 4);
        ulPar = ulPar ^ (ulPar >> 2);
        ulPar = ulPar ^ (ulPar >> 1);
    
        return((ulPar & 1));        
    };
    
    /*!
    // Compute a 16 bit odd parity
    //
    // @param ulValue -- value to compute the parity on
    // @returns even parity 0 or 1
    //
    */
    static unsigned parity16(unsigned ulValue)
    {
        unsigned ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >> 8);
        ulPar = ulPar ^ (ulPar >> 4);
        ulPar = ulPar ^ (ulPar >> 2);
        ulPar = ulPar ^ (ulPar >> 1);
    
        return((ulPar & 1));        
    };
    /*!
    // Compute a 32 bit odd parity
    //
    // @param nValue -- value to compute the parity on
    // @returns even parity 0 or 1
    //
    */
    static unsigned parity32(unsigned ulValue)
    {
        unsigned ulPar;
        ulPar = ulValue;            
        ulPar = ulPar ^ (ulPar >>16);
        ulPar = ulPar ^ (ulPar >> 8);
        ulPar = ulPar ^ (ulPar >> 4);
        ulPar = ulPar ^ (ulPar >> 2);
        ulPar = ulPar ^ (ulPar >> 1);
    
        return((ulPar & 1));        
    };
    
    /*!
    // Check the parity of an aribitrary number.
    //
    // @param data -- number to check
    // @returns even parity for the total number of bits in 8 bit groups...
    //
    */
    static unsigned parity(const ArbitraryNumber &data)
    {
        int nBitCount = data.getBitCount();
    
        unsigned nPar = 0;

        int n;
        for (n = 0; n < nBitCount&(-8); n += 8)
            nPar = nPar ^ (parity8(data.getUint32(n, n+7)) & 1);
        for (n = 0; n < nBitCount; n++)
            nPar = nPar ^ data.getUint32(n,n);

        return(nPar & 1);        
    }

};


#endif


