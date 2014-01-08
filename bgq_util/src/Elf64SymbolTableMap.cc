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

#include <iomanip>
#include <sstream>
#include <iostream>
#include <sys/types.h>
#include "Elf64SymbolTableMap.h"
#include <bgq_util/include/Time.h>


//#define TRACE_COUT(x) cout << x;
  #define TRACE_COUT(x)

//#define TRACE_COUT_VERBOSE(x) cout << x;
  #define TRACE_COUT_VERBOSE(x)


Elf64SymbolTableMap::Elf64SymbolTableMap()   {}

Elf64SymbolTableMap::Elf64SymbolTableMap(const char* pElfFileName)
{
   Time now;

   int iRc = includeSymbolInformation(pElfFileName);
   if (iRc)
   {
      cout << "! " << now.curTimeString(true) << " Elf64SymbolTableMap constructor failure - includeSymbolInformation() failed with rc = " << iRc << flush << endl;
      return;
   }
}
// End Elf64SymbolTableMap::Elf64SymbolTableMap(const char* pElfFileName)


int Elf64SymbolTableMap::includeSymbolInformation(const string& sElfFileName)
{
   Time now;
   
   string sCmd = sElfFileName;
   FILE* pPipe = fopen(sCmd.c_str(), "r");
   if (pPipe == NULL)
   {
      cout << "! " << now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation failed - unable to process objdump command" << flush << endl;
      return -1;
   }
   
   //---------------------------------------------------------------------------
   // Process the output from the command (this contains the abovementioned symbol information and starting address in memory).
   // Note: objdump's output looks like this:
   //   'Disassembly of section .text:'
   //   ' '
   //   '000003fffffd7000 <_fw_Vec_DEBUG>:'
   //   ' 3fffffd7000:  li      r3,1'
   //   ' 3fffffd7004:  mfspr   r4,58'
   //---------------------------------------------------------------------------
   char aLine[256+1];   // one additional character for 'end of line' character.
   while ( true ) 
   {
      //------------------------------------------------------------------------
      // Read next line of output.
      //------------------------------------------------------------------------
      memset(aLine, 0x00, sizeof(aLine));
      fgets(aLine, sizeof(aLine), pPipe);
      // Check & see if EOF occurred.
      if ( feof(pPipe) )
      {  // hit eof - so we have processed all of the output.
         break;
      }
      // Ensure the read was successful.
      if ( ferror(pPipe) )
      {  // read error occurred.
         cout << "! " << now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation failed - fgets failed while processing objdump command's output" << flush << endl;
         return -2;
      }

      //------------------------------------------------------------------------
      // See if this is a 'start of method' line or not.
      //------------------------------------------------------------------------
      // Ensure line is at least 19 characters long.
      string sTemp = aLine;
      if (sTemp.size() < 19)
      {  // skip to next line in output.
         TRACE_COUT_VERBOSE( now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation - line is too short (" << sTemp.size() << " characters)" << endl);
         continue;
      }
      TRACE_COUT_VERBOSE( now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation - line is '" << sTemp << "'" << endl);

      // Replace any tabs with blanks.
      size_t offsetTab = 0;
      char cTab = 0x09;
      char cBlank = 0x20;
      while (true)
      {
         offsetTab = sTemp.find(cTab);
         if (offsetTab == string::npos)
            break;
         sTemp[offsetTab] = cBlank;
      }

      // Ensure column 17 has a blank character AND column 18 a < character.
      if ( (sTemp[16] != 0x20) || (sTemp[17] != 0x3C) )
      {  // skip to next line in output.
         TRACE_COUT_VERBOSE( now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation - column 17 is not a blank or column 18 is not a '<'" << endl);
         continue;
      }

      // Ensure that column 1 - 16 are a hex 64-bit memory address.
      size_t offsetNonHexChar = sTemp.find_first_not_of("0123456789abcdefABCDEF");
      if ( (offsetNonHexChar != string::npos) && (offsetNonHexChar < 16) )
      {  // this is not a 'start of method line' as one of first 16 columns has non-hex character.
         TRACE_COUT_VERBOSE( now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation - invalid hex character in first 16 columns" << endl);
         continue;
      }

      // Convert the memory address into unsigned long long.
      istringstream iss1( sTemp.substr(0, 16) );  // memory location starting in column 1 continuing for total of 16 ascii characters.
      unsigned long long ullMemoryAddr = 0;
      iss1 >> hex >> ullMemoryAddr >> dec;

      // Remove end of line character if present at end of string.
      if (sTemp[ sTemp.size()-1 ] == 0x0A)
         sTemp.erase(sTemp.size()-1, 1);
      
      // Remove ":" if present at the end of string.
      if (sTemp[ sTemp.size()-1 ] == 0x3A)
         sTemp.erase(sTemp.size()-1, 1);
      
      //------------------------------------------------------------------------
      // Actually insert the method information into the map.
      //------------------------------------------------------------------------
      TRACE_COUT( now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation - creating map entry - " 
                  << "map[0x" << hex << ullMemoryAddr << dec << "] = '" << sTemp.substr(17) << "'" << endl );
      _map[ullMemoryAddr] = sTemp.substr(17);  // symbol info starting in column 18 continuing for rest of line.
   }

   // Close pipe.
   int iRc = fclose(pPipe);
   if (iRc != 0)
   {
      cout << "! " << now.curTimeString(true) << " Elf64SymbolTableMap::includeSymbolInformation failed - pclose failed with rc = " << iRc << flush << endl;
      return -3;
   }

   return 0;
}
// End Elf64SymbolTableMap::includeSymbolInformation(const string& sElfFileName)


string Elf64SymbolTableMap::getSymbol(unsigned long long location)
{
   // Ensure that this map has at least one entry in it.
   if (_map.size() == 0)
   {  // this map does not have any entries in it - there is no symbol table information in this entire map.
      // Return appropriate string for symbol's information.
      return "";  // "!!!No symbol table information for this ELF file!!!";
   }

   // Find the specified key in this map.
   map<unsigned long long, string>::iterator iter;
   iter = _map.lower_bound(location);
   if ((iter == _map.end()) || (iter->first != location))
   {  // this entry has a key value that is higher than the specified value - back up and use the previous entry.
      // Ensure that we are not already pointing at the first entry in the map.
      if (iter == _map.begin())
      {  // we are already pointing at the very first entry (there is no previous entry) - there is no symbol table info for this memory location.
         // Return appropriate string for symbol's information.
         return "";   // "!!!No symbol table information for this memory location!!!";
      }
      // Back up and point to the previous entry.
      --iter;
      // Give caller the symbol's information.
      return iter->second;
   }
   else
   {
      // Give caller the symbol's information.
      return iter->second;
   }
}
// End Elf64SymbolTableMap::getSymbol(unsigned long long location)

string Elf64SymbolTableMap::getSymbolAndOffset(unsigned long long location)
{
   // Ensure that this map has at least one entry in it.
   if (_map.size() == 0)
   {  // this map does not have any entries in it - there is no symbol table information in this entire map.
      // Return appropriate string for symbol's information.
      return "";  // "!!!No symbol table information for this ELF file!!!";
   }

   // Find the specified key in this map.
   map<unsigned long long, string>::iterator iter;
   iter = _map.lower_bound(location);
   if ((iter == _map.end()) || (iter->first != location))
   {  // this entry has a key value that is higher than the specified value - back up and use the previous entry.
      // Ensure that we are not already pointing at the first entry in the map.
      if (iter == _map.begin())
      {  // we are already pointing at the very first entry (there is no previous entry) - there is no symbol table info for this memory location.
         // Return appropriate string for symbol's information.
         return "";   // "!!!No symbol table information for this memory location!!!";
      }
      // Back up and point to the previous entry.
      --iter;
   }

   // Take the symbol info and add the offset into the method.
   unsigned long long ullMethodOffset = location - iter->first;
   ostringstream sSymbol;
   uint uSizeSymbolInfo = iter->second.size();
   if (iter->second[ uSizeSymbolInfo-1 ] == '>')
   {
      sSymbol << iter->second.substr(0, (uSizeSymbolInfo-1) );
    //sSymbol << "+0x" << setw(3) << setfill('0') << hex << ullMethodOffset << dec << ">";
      sSymbol << "+0x" << hex << ullMethodOffset << dec << ">";
   }
   else
   {
      sSymbol << iter->second;
    //sSymbol << "+0x" << setw(3) << setfill('0') << hex << ullMethodOffset << dec;
      sSymbol << "+0x" << hex << ullMethodOffset << dec;
   }

   return sSymbol.str();
}
// End Elf64SymbolTableMap::getSymbolAndOffset(unsigned long long location)

