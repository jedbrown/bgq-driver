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

///////////////////////////////////////////////////////////////////
//
// This file contains the class Elf32File.  
//
// This class is used to parse and read the contents of a Elf32 
// (Executable and Linkable Format) formatted file.
//
// 

#include <stddef.h>     // defines NULL and other stuff
#include <stdio.h>

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>
#include <math.h>

#include "Elf32File.h"



#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}

//
// Make sure we have these macros
//

#ifndef __max
#define __max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef __min
#define __min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

Elf32File::Elf32File()
{
    //m_nFileHandle = -1;
    m_pFile = NULL;
    m_pStringTable = NULL;
    m_ulStringTblSize = 0;
    memset(&m_ElfFileHdr, 0, sizeof(m_ElfFileHdr));
}
Elf32File::~Elf32File()
{
    Close();
        

}

/////////////////////////////////////////////////////////////
// int Elf32File::Open(const char *pszFileName)
/*!
// Open the elf32 file and read the header into memory.
// verify the file is indeed an elf32 file.
//
// @param pszFileName [in] the file name of elf file to open.
// 
// @return ELFERR_SUCCESS if successful.
//               - error codes TBD.
//
*/
int Elf32File::Open(const char *pszFileName)
{
    int nErr = ELFERR_INTERNAL_ERR;
    unsigned char szHeader[sizeof(Elf32_Ehdr)] = {};

    Close();    // if there was a file open, close it and dump its memory.


    m_pFile = fopen(pszFileName, "rb");         // readonly and binary

    // m_nFileHandle = open(pszFileName,               // *filename
    //                     O_BINARY | O_RDONLY,     // oflag
    //                     S_IREAD);                 // pmode
    if (m_pFile == NULL)
        FAILERR(nErr,FileErr(errno));


    // now read the header into a buffer in memory
    size_t nLenRead;

    nLenRead = fread(szHeader,              // pBuffer
                     1,                     // size (item size)
                     sizeof(Elf32_Ehdr),    // count (of items)
                     m_pFile);              // *stream

    //nLenRead = read(m_nFileHandle,          // handle
    //                szHeader,               // *buffer
    //                sizeof(Elf32_Ehdr));

    if (nLenRead != sizeof(Elf32_Ehdr))
        FAILERR(nErr, ELFERR_NOT_ELF_FILE);
    

    
    //
    // copy the first part of the data so we can tell how to parse the
    // rest
    memcpy(m_ElfFileHdr.e_ident,            // dest
           szHeader,                        // src
           sizeof(m_ElfFileHdr.e_ident));   // length
    
    //
    // next check the magic numbers in the headers.
    //
    if ((m_ElfFileHdr.e_ident[EI_MAG0] != 0x7f) ||
        (m_ElfFileHdr.e_ident[EI_MAG1] != 'E')  ||
        (m_ElfFileHdr.e_ident[EI_MAG2] != 'L')  ||
        (m_ElfFileHdr.e_ident[EI_MAG3] != 'F'))
    {
        FAILERR(nErr, ELFERR_NOT_ELF_FILE);
    }

    //
    // This reader only recognizes elf32.
    // 
    if (m_ElfFileHdr.e_ident[EI_CLASS] != ELFCLASS32)
        FAILERR(nErr, ELFERR_NOT_ELF_FILE);
    
    m_nDataType = m_ElfFileHdr.e_ident[EI_DATA];

    if ((m_nDataType != ELFDATA2MSB) && 
        (m_nDataType != ELFDATA2LSB))
    {
        FAILERR(nErr, ELFERR_NOT_ELF_FILE);
    }

    if (m_ElfFileHdr.e_ident[EI_VERSION] != EV_CURRENT)
        FAILERR(nErr, ELFERR_NOT_ELF_FILE);


    unsigned char *pData;
    // start parsing past the identifier
    pData = szHeader + sizeof(m_ElfFileHdr.e_ident);

    // parse off all the fields in here in order.
    pData = GetData(pData, m_ElfFileHdr.e_type);
    pData = GetData(pData, m_ElfFileHdr.e_machine);
    pData = GetData(pData, m_ElfFileHdr.e_version);
    pData = GetData(pData, m_ElfFileHdr.e_entry);
    pData = GetData(pData, m_ElfFileHdr.e_phoff);
    pData = GetData(pData, m_ElfFileHdr.e_shoff);
    pData = GetData(pData, m_ElfFileHdr.e_flags);
    pData = GetData(pData, m_ElfFileHdr.e_ehsize);
    pData = GetData(pData, m_ElfFileHdr.e_phentsize);
    pData = GetData(pData, m_ElfFileHdr.e_phnum);
    pData = GetData(pData, m_ElfFileHdr.e_shentsize);
    pData = GetData(pData, m_ElfFileHdr.e_shnum);
    pData = GetData(pData, m_ElfFileHdr.e_shstrndx);

    //
    // Now that we have a header, construct the list of program headers
    // and the list of segment headers.
    //

    nErr = ReadProgramHeaders();
    if (nErr != ELFERR_SUCCESS)
        FAIL;
    nErr = ReadSectionHeaders();
    if (nErr != ELFERR_SUCCESS)
        FAIL;
    nErr = ReadStringTable();
    if (nErr != ELFERR_SUCCESS)
        FAIL;

    return(ELFERR_SUCCESS);
Fail:
    Close();    
    return(nErr);
}


///////////////////////////////////////////////////////////////
// void Elf32File::Close()
/*!
// close the file opened by the Open call.
//
//
*/
void Elf32File::Close()
{
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }
    m_vectProgramHeaders.clear();
    m_vectSectionHeaders.clear();    
    if (m_pStringTable)
        delete [] m_pStringTable, m_pStringTable = NULL;
    m_ulStringTblSize = 0;
}
////////////////////////////////////////////////////////////////
bool Elf32File::IsOpen()
/*
// Check to see if the elf file is open.
// 
// @param none.
//
// @return true if file is open.
//
*/
{
    if (m_pFile == NULL)
        return(false);
    else
        return(true);
}

////////////////////////////////////////////////////////////
// const Elf32_Ehdr *Elf32File::GetHeader() const
/*!
// Retrieve the file header associated the elf32 file.
//
// @param   none.
// 
// @return -- pointer to the file header.
//
*/
const Elf32_Ehdr *Elf32File::GetHeader() const
{
    if (m_pFile == NULL)
        return(NULL);
    else
        return(&m_ElfFileHdr);
}

///////////////////////////////////////////////////////////
// const Elf32_Shdr *Elf32File::GetSectionHeader(int nSection)
/*!
// Get a pointer to the section header.
// 
// @param   nSection [in] section number of header to retrieve.
// 
// @return -- pointer to section header.
//
*/
const Elf32_Shdr *Elf32File::GetSectionHeader(int nSection) const
{
    const Elf32_Shdr *pHdr = NULL;

    if (nSection >= (int)m_vectSectionHeaders.size())
        FAIL;

    pHdr = &m_vectSectionHeaders[nSection];

    return(pHdr);
Fail:
    return(NULL);
}
///////////////////////////////////////////////////////////
// const Elf32_Shdr *GetSectionHeader(int nSection) const; 
/*!
// Get a pointer to the section header.
// 
// @param   nSection [in] section number of header to retrieve.
// @param   szName [out] Return the section Name 
//
// @return -- pointer to section header.
//
//
*/
const Elf32_Shdr *Elf32File::GetSectionHeader(int nSection,
                                              std::string &szName) const
{
    const Elf32_Shdr *pHdr = NULL;
    pHdr = GetSectionHeader(nSection);
    szName.erase();
    if (pHdr)
    {
        if (pHdr->sh_name < m_ulStringTblSize)
            szName = &m_pStringTable[pHdr->sh_name];
    }
    return(pHdr);
}                                              

////////////////////////////////////////////////////////////
// int Elf32File::GetNumSectionHeaders() const
/*!
// Retrieve the total number of section headers in this elf file.
//
// @param   none.
// 
// @return  Total number of section headers.
//
*/
int Elf32File::GetNumSectionHeaders() const
{
    return(m_ElfFileHdr.e_shnum);
}

/////////////////////////////////////////////////////////////
// const Elf32_Phdr *Elf32File::GetProgramHeader(int nHeader)
/*!
// Retrieve a pointer to the program header
//
// @param  nHeader [in] header to retrieve (0.. NumProgramHeaders);
//
// @return  Pointer to the program header.
//
*/
const Elf32_Phdr *Elf32File::GetProgramHeader(int nHeader) const
{

    const Elf32_Phdr *pHdr = NULL;

    if (nHeader >= (int)m_vectProgramHeaders.size())
        FAIL;

    pHdr = &m_vectProgramHeaders[nHeader];

    return(pHdr);
Fail:
    return(NULL);
}

/////////////////////////////////////////////////////////////
// const Elf32_Phdr *GetProgramHeader(const Elf32_Shdr *pSectHdr);
/*!
// Retrieve a pointer to the program header associated with
// a given section header.
//
//
// @param  pSectHdr [in] -- pointer to section header to find
//                          the associated program header.
//
// @return  Pointer to the program header.
//
*/
const Elf32_Phdr *Elf32File::GetProgramHeader(const Elf32_Shdr *pSectHdr) const
{
    const Elf32_Phdr *pHdr = NULL;
    const Elf32_Phdr *pFoundHdr = NULL;
    int n;
    int nNumHdrs;
    if (pSectHdr == NULL)       // hey, don't do that...
        FAIL;


    nNumHdrs = m_vectProgramHeaders.size();

    for (n = 0; n < nNumHdrs; n++)
    {
        pHdr = &m_vectProgramHeaders[n];
        if ((pHdr->p_type != PT_LOAD) ||       // not what we want
            (pHdr->p_memsz < 2) ||
            ((pHdr->p_flags & (PF_X | PF_W | PF_R)) == 0) )
        {
            continue;                       //next
        }
        //
        // does the phdr contain the starting virtual address
        // of the segment in question.
        //
        if ((pSectHdr->sh_addr >= pHdr->p_vaddr ) && 
            (pSectHdr->sh_addr < (pHdr->p_vaddr + (pHdr->p_memsz - 1))) )
        {
            pFoundHdr = pHdr;
            break;
        }
    }   // for (n ...
    return(pFoundHdr);
Fail:
    return(NULL);
}

/////////////////////////////////////////////////////////////
// int GetSectionPhysAddr(const Elf32_Shdr *pSectHdr,
//                        uint32_t &ulPhysAddr);
/*!
//
// Retrieve the physical address assoicated with the given section
// header
// a given section header.
//
//
// @param  pSectHdr [in] -- pointer to section header to find
//                          the associated program header.
//
//
// @return ELFERR_SUCCESS if ok.
//         ulPhysAddr -- physical address of the section.
//
*/
int Elf32File::GetSectionPhysAddr(const Elf32_Shdr *pSectHdr,
                                  uint32_t &ulPhysAddr)
{
    int nErr = ELFERR_INTERNAL_ERR;
    const Elf32_Phdr *pPhdr;
    
    pPhdr = GetProgramHeader(pSectHdr);     // no pheader matching this.
    if (pPhdr == NULL)
        FAIL;

    // phys addr is the phdr phys address plus the delta of
    // the section and phdr virtual addresses.
    ulPhysAddr = pPhdr->p_paddr + (pSectHdr->sh_addr - pPhdr->p_vaddr);


    return(ELFERR_SUCCESS);     // made it this far, success
Fail:
    return(nErr);
}

//////////////////////////////////////////////////////////////////
// int Elf32File::GetNumProgramHeaders() const
/*!
// retrieve the total number of program headers in this file
//
// inputs:
//    none.
// outputs:
//    returns -- total number of program headers in this elf file.
//
*/
int Elf32File::GetNumProgramHeaders() const
{
    return(m_ElfFileHdr.e_phnum);
}

////////////////////////////////////////////////////////////
// int Elf32File::FileErr(int nErr)
/*!
// Map a system file error to an ELF error code.
//
// @param nErr -- file error to map.
// 
// @return -- elf error code for this error.
//
*/
int Elf32File::FileErr(int nErr) const
{
    switch (nErr)
    {
        case EACCES: nErr = ELFERR_FILEACCESS;      break;      // File sharing error
        case EEXIST: nErr = ELFERR_FILEOPEN_ERR;    break;      // _O_CREAT and _O_EXCL flags specified, 
                                                                // but filename already exists
        case EINVAL: nErr = ELFERR_FILEOPEN_ERR;    break;      // invalid argument
        case EMFILE: nErr = ELFERR_FILEOPEN_ERR;    break;      // too many open file handles
        case ENOENT: nErr = ELFERR_FILE_NOT_FOUND;  break;      // file not found.
        default: nErr = ELFERR_FILEACCESS;          break;
    }
    return(nErr);
}

///////////////////////////////////////////////////////
// unsigned char *Elf32File::GetData(unsigned char *pData,
//                                   Elf64_Half &nValue)
/*!
// Retrieve an Elf half word (16 bits) from the data stream.
//
// @param pData [in] pointer to data to extract half word from
// @param nValue [out] value retrieved
//
// @return Points past the data extracted.
//
*/
unsigned char *Elf32File::GetData(unsigned char *pData,
                                  Elf64_Half &nValue)
{
   if (m_nDataType == ELFDATA2LSB)
       nValue = Elf64_Half((pData[1]<<8) | pData[0]);
   else
       nValue = Elf64_Half((pData[0]<<8) | pData[1]);
    return(pData + 2);
}
///////////////////////////////////////////////////////
// unsigned char *Elf32File::GetData(unsigned char *pData,
//                                   Elf64_Word &nValue)
/*!
// Retrieve an Elf half word (32 bits) from the data stream.
//
// @param pData [in] pointer to data to extract word from
// @param nValue [out] value retrieved
//
// @return Points past the data extracted.
//
*/
unsigned char *Elf32File::GetData(unsigned char *pData,
                                  Elf64_Word &nValue)
{
   if (m_nDataType == ELFDATA2LSB)
       nValue = (pData[3]<<24) | (pData[2]<<16) | (pData[1]<<8) | pData[0];
   else
       nValue = (pData[0]<<24) | (pData[1]<<16) | (pData[2]<<8) | pData[3];
    return(pData + 4);
}

/////////////////////////////////////////////////////////////
// int Elf32File::ReadProgramHeaders()
/*!
// Read all the program headers into our own internal list
// 
// @param none.
// @return ELFERR_SUCCESS if failure.
//
*/
int Elf32File::ReadProgramHeaders()
{
    int nErr = ELFERR_INTERNAL_ERR;
    int nNumHdrs;
    int n;

    if (m_ElfFileHdr.e_phoff == 0)
        return(ELFERR_SUCCESS);

    //
    // program header entry size.
    //    
    if ((m_ElfFileHdr.e_phentsize == 0) || 
        (m_ElfFileHdr.e_phentsize < sizeof(Elf32_Phdr)))
    {
        FAILERR(nErr, ELFERR_INVALID_FILE_DATA);
    }

    long nSeek;
    
    nSeek = m_ElfFileHdr.e_phoff;       // starting offset
    nNumHdrs = GetNumProgramHeaders();
    for (n = 0; n < nNumHdrs; n++)
    {
        unsigned char szHeader[sizeof(Elf32_Phdr)] = {};
        int nRet;
        //
        // Seek to header location.
        //

        nRet = fseek(m_pFile,               // *stream
                     nSeek,                 // offset
                     SEEK_SET);             // origin

        //lRet = lseek(m_nFileHandle,     // handle
        //             nSeek,             // offset.
        //             SEEK_SET);         // origin
        if (nRet != 0)
            FAILERR(nErr, ELFERR_INVALID_FILE_DATA);

        //
        // these are rather small, so do them on the stack.
        //    
        Elf32_Phdr ProgramHdr;
        
        //
        // read the data into a buffer.
        size_t nLenRead;
        nLenRead = fread(szHeader,              // pBuffer
                         1,                     // size (item size)
                         sizeof(Elf32_Phdr),    // count (of items)
                         m_pFile);              // *stream

        //nLenRead = read(m_nFileHandle,          // handle
        //                szHeader,               // *buffer
        //                sizeof(Elf32_Phdr));
        if (nLenRead != sizeof(Elf32_Phdr))
            FAILERR(nErr, ELFERR_INVALID_FILE_DATA);
            
        unsigned char *pData;
        pData = szHeader;

        pData = GetData(pData, ProgramHdr.p_type);
        pData = GetData(pData, ProgramHdr.p_offset);
        pData = GetData(pData, ProgramHdr.p_vaddr);
        pData = GetData(pData, ProgramHdr.p_paddr);
        pData = GetData(pData, ProgramHdr.p_filesz);
        pData = GetData(pData, ProgramHdr.p_memsz);
        pData = GetData(pData, ProgramHdr.p_flags);
        pData = GetData(pData, ProgramHdr.p_align);

        m_vectProgramHeaders.push_back(ProgramHdr); // add it to the list
                                     
        nSeek += m_ElfFileHdr.e_phentsize;  // next offset
    
    } 
    return(ELFERR_SUCCESS);
Fail:
    return(nErr);
}

////////////////////////////////////////////////////////////
// int Elf32File::ReadSectionHeaders()
/*!
// Read all the section headers into our own internal list
// 
// @param none.
// @return ELFERR_SUCCESS if failure.
//
*/
int Elf32File::ReadSectionHeaders()
{
    int nErr = ELFERR_INTERNAL_ERR;
    int nNumHdrs;
    int n;

    if (m_ElfFileHdr.e_shoff == 0)
        return(ELFERR_SUCCESS);

    //
    // program header entry size.
    //    
    if ((m_ElfFileHdr.e_shentsize == 0) || 
        (m_ElfFileHdr.e_shentsize < sizeof(Elf32_Shdr)))
    {
        FAILERR(nErr, ELFERR_INVALID_FILE_DATA);
    }

    long nSeek;
    
    nSeek = m_ElfFileHdr.e_shoff;       // starting offset
    nNumHdrs = GetNumSectionHeaders();
    for (n = 0; n < nNumHdrs; n++)
    {
        unsigned char szHeader[sizeof(Elf32_Shdr)] = {};
        int nRet;
        //
        // Seek to header location.
        //
        nRet = fseek(m_pFile,               // *stream
                     nSeek,                 // offset
                     SEEK_SET);             // origin

        if (nRet != 0)
            FAILERR(nErr, ELFERR_INVALID_FILE_DATA);

        //
        // these are rather small, so do them on the stack.
        //    
        Elf32_Shdr SectionHdr;
        
        //
        // read the data into a buffer.
        size_t nLenRead;
        nLenRead = fread(szHeader,              // pBuffer
                         1,                     // size (item size)
                         sizeof(Elf32_Shdr),    // count (of items)
                         m_pFile);              // *stream
        // nLenRead = read(m_nFileHandle,          // handle
        //                szHeader,               // *buffer
        //                sizeof(Elf32_Shdr));
        if (nLenRead != sizeof(Elf32_Shdr))
            FAILERR(nErr, ELFERR_INVALID_FILE_DATA);
        unsigned char *pData;
        pData = szHeader;

        pData = GetData(pData, SectionHdr.sh_name);
        pData = GetData(pData, SectionHdr.sh_type);
        pData = GetData(pData, SectionHdr.sh_flags);
        pData = GetData(pData, SectionHdr.sh_addr);
        pData = GetData(pData, SectionHdr.sh_offset);
        pData = GetData(pData, SectionHdr.sh_size);
        pData = GetData(pData, SectionHdr.sh_link);
        pData = GetData(pData, SectionHdr.sh_info);
        pData = GetData(pData, SectionHdr.sh_addralign);
        pData = GetData(pData, SectionHdr.sh_entsize);

        m_vectSectionHeaders.push_back(SectionHdr); // add it to the list
                                     
        nSeek += m_ElfFileHdr.e_shentsize;  // next offset
    
    } 
    return(ELFERR_SUCCESS);
Fail:
    return(nErr);
}

/////////////////////////////////////////////////////
// int Elf32File::ReadStringTable()
/*!
// Read the string table into an internal buffer to support
// fast lookups.
// 
// @param none.
// @return ELFERR_SUCCESS if failure.
//
*/
int Elf32File::ReadStringTable()
{
    int nErr = ELFERR_INTERNAL_ERR;

    if (m_ElfFileHdr.e_shstrndx == SHN_UNDEF)
        return(ELFERR_SUCCESS);

    //
    // Look up the string table section.
    //
    const Elf32_Shdr *pStrTblSection;
    pStrTblSection = GetSectionHeader(m_ElfFileHdr.e_shstrndx);
    if (pStrTblSection == NULL)
        FAILERR(nErr, ELFERR_INVALID_FILE_DATA);

    //
    // This MUST be a string table, otherwise we have a problem
    //
    if (pStrTblSection->sh_type != SHT_STRTAB)
        FAILERR(nErr, ELFERR_INVALID_FILE_DATA);
    
    if (pStrTblSection->sh_size == 0)
        FAILERR(nErr, ELFERR_INVALID_FILE_DATA);
    
                                    
    int nRet;
    long nSeek;
    nSeek = pStrTblSection->sh_offset;


    //
    // Seek to table location location.
    //
    nRet = fseek(m_pFile,               // *stream
                 nSeek,                 // offset
                 SEEK_SET);             // origin
    // lRet = lseek(m_nFileHandle,     // handle
    //              nSeek,             // offset.
    //              SEEK_SET);         // origin
    if (nRet != 0)
        FAILERR(nErr, ELFERR_INVALID_FILE_DATA);

    m_ulStringTblSize = pStrTblSection->sh_size;
    m_pStringTable = new char[m_ulStringTblSize];
    if (m_pStringTable == NULL)
        FAILERR(nErr, ELFERR_NO_MEMORY);

    size_t nLenRead;
    nLenRead = fread(m_pStringTable,        // pBuffer
                     1,                     // size (item size)
                     m_ulStringTblSize,     // count (of items)
                     m_pFile);              // *stream
    // nLenRead = read(m_nFileHandle,           // handle
    //                 m_pStringTable,          // *buffer
    //                m_ulStringTblSize);// count

    if (nLenRead != m_ulStringTblSize)
        FAILERR(nErr, ELFERR_NOT_ELF_FILE);
    
   
    return(ELFERR_SUCCESS);
Fail:
    return(nErr);
}

////////////////////////////////////////////////////////
// int Elf32File::GetSectionData(int nSection,
//                              unsigned char *pData,
//                              uint32_t &nLen)
/*!
// Retrieve the section data specified and place it in the
// data buffer indicated.
//
// If the pData pointer is NULL then just return the length 
// of the data buffer needed to read the data.
//
// If the data buffer is too short then return an error.
//
// @param   nSection [in] section number of header to retrieve.
// @param   pData [in/out] buffer to stuff the data.
// @param   nLen [in/out] On input the maximum length of the data to
//                        read.
//                        On output, the length of the data read.
//
// @return ELFERR_SUCCESS if ok.
//         ELFERR_BUFFER_TOO_SHORT -- if the buffer is too short.
*/
int Elf32File::GetSectionData(int nSection,
                              void *pData,
                              uint32_t &nLen)
{
    int nErr = ELFERR_INTERNAL_ERR;
    const Elf32_Shdr *pHdr = NULL;
    uint8_t *pDest = (uint8_t *)pData;

    if (m_pFile == NULL)
        FAILERR(nErr, ELFERR_FILE_NOT_OPEN);

    pHdr = GetSectionHeader(nSection);
    if (pHdr == NULL)
        FAILERR(nErr, ELFERR_INVALID_PARAM);
    
    if (pData == NULL)
    {
        nLen = pHdr->sh_size;
    }
    else
    {
        // do we have enough space.
        if (nLen < pHdr->sh_size)
            FAILERR(nErr, ELFERR_BUFFER_TOO_SHORT);

        int nRet;
        //
        // Seek to header location.
        //
        nRet = fseek(m_pFile,               // *stream
                     pHdr->sh_offset,       // offset
                     SEEK_SET);             // origin
        //lRet = lseek(m_nFileHandle,     // handle
        //             pHdr->sh_offset,   // offset.
        //             SEEK_SET);         // origin
        if (nRet != 0)
            FAILERR(nErr, ELFERR_NOT_ELF_FILE);
            
        nLen = pHdr->sh_size;
        size_t nLenRead;

        //
        // Read this in 16k chunks so we don't run into
        // a problem with the total length not being
        // able to be containined in an int.
        //
        size_t nLenRemaining;
        size_t nLenToRead;
        nLenRemaining = nLen;    
        while (nLenRemaining)
        {   
            nLenToRead = __min(nLenRemaining, 0x4000); 
            nLenRead = fread(pDest,             // pBuffer
                             1,                 // size (item size)
                             nLenToRead,        // count (of items)
                             m_pFile);          // *stream
            // nLenRead = read(m_nFileHandle,          // handle
            //                 pData,                  // *buffer
            //                nLenToRead);             // count
            
            if (nLenRead != nLenToRead)
                FAILERR(nErr, ELFERR_NOT_ELF_FILE);
            nLenRemaining -= nLenToRead;
            pDest += nLenToRead;        // advance the pointer.
        }

    }
    return(ELFERR_SUCCESS);
Fail:
    return(nErr);

}
