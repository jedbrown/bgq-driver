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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#ifndef _ELF_64_SYMBOL_TABLE_MAP_H
#define _ELF_64_SYMBOL_TABLE_MAP_H

#include <sys/types.h>
#include <map>
using namespace std;


/** Class to contain an Elf File's Symbol Table Map that maps memory location's to the symbol's information. **/
class Elf64SymbolTableMap
{
private:
   map<unsigned long long, string> _map;  // map memory address to a symbol's information.


public:
   /** Constructor(s) **/
   Elf64SymbolTableMap();
   Elf64SymbolTableMap(const char* pElfFileName);

   /** Get the specified memory location's symbol information **/
   string getSymbol(unsigned long long location);
   string getSymbolAndOffset(unsigned long long location);

   /** Add another Elf File's symbol information 
    *    Returns
    *       0 = success
    *      !0 = failure 
    **/
   int includeSymbolInformation(const string& sElfFileName);
   
   /** Get the number of entries */
   uint size() const  { return _map.size(); }
};

#endif
