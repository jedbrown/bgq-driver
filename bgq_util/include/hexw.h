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

#ifndef __HEXW_H__
#define __HEXW_H__

#include <iostream>
#include <iomanip>

class hexw
{
public:
    explicit hexw(unsigned w): 
        width(w) {};                            
    unsigned width;
    friend std::ostream & operator <<(std::ostream &os, const hexw &hw)
    {
        os << "0x" << std::hex << std::setw(hw.width) << std::setfill('0');
        return os;
    };
};

class decw
{
public:
    explicit decw(unsigned w): 
        width(w) {};                            
    unsigned width;
    friend std::ostream & operator <<(std::ostream &os, const decw &hw)
    {
        os << std::dec << std::setw(hw.width) << std::setfill('0');
        return os;
    };
};

#endif


