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

#ifndef _ELF64FILE_H_
#define _ELF64FILE_H_

///////////////////////////////////////////////////////////////////
//
// This file contains the class Elf64File.  
//
// This class is used to parse and read the contents of a Elf32 
// (Executable and Linkable Format) formatted file.
//
// 


#if !_AIX
    #include <elf.h>
#endif    


#include <stdio.h>
#include <string>
#include <vector>

#define FAIL goto Fail
#define FAILERR(nErr, nValue) {(nErr) = (nValue); goto Fail;}

// linux devines an elf.h header file, aix does not, so define it here
//
#if _AIX
    #include "Elf32File.h"
    typedef struct
    {
        unsigned char e_ident[16]; /* ELF identification */
        Elf64_Half e_type; /* Object file type */
        Elf64_Half e_machine; /* Machine type */
        Elf64_Word e_version; /* Object file version */
        Elf64_Addr e_entry; /* Entry point address */
        Elf64_Off e_phoff; /* Program header offset */
        Elf64_Off e_shoff; /* Section header offset */
        Elf64_Word e_flags; /* Processor-specific flags */
        Elf64_Half e_ehsize; /* ELF header size */
        Elf64_Half e_phentsize; /* Size of program header entry */
        Elf64_Half e_phnum; /* Number of program header entries */
        Elf64_Half e_shentsize; /* Size of section header entry */
        Elf64_Half e_shnum; /* Number of section header entries */
        Elf64_Half e_shstrndx; /* Section name string table index */
    } Elf64_Ehdr;




    typedef struct
    {
        Elf64_Word sh_name; /* Section name */
        Elf64_Word sh_type; /* Section type */
        Elf64_Xword sh_flags; /* Section attributes */
        Elf64_Addr sh_addr; /* Virtual address in memory */
        Elf64_Off sh_offset; /* Offset in file */
        Elf64_Xword sh_size; /* Size of section */
        Elf64_Word sh_link; /* Link to other section */
        Elf64_Word sh_info; /* Miscellaneous information */
        Elf64_Xword sh_addralign; /* Address alignment boundary */
        Elf64_Xword sh_entsize; /* Size of entries, if section has table */
    } Elf64_Shdr;

    /* Program segment header.  */
    

    typedef struct
    {
        Elf64_Word p_type; /* Type of segment */
        Elf64_Word p_flags; /* Segment attributes */
        Elf64_Off p_offset; /* Offset in file */
        Elf64_Addr p_vaddr; /* Virtual address in memory */
        Elf64_Addr p_paddr; /* Reserved */
        Elf64_Xword p_filesz; /* Size of segment in file */
        Elf64_Xword p_memsz; /* Size of segment in memory */
        Elf64_Xword p_align; /* Alignment of segment */
    } Elf64_Phdr;

#else
    /*! Elf Error codes
    */
    enum {
        ELFERR_SUCCESS = 0,                         //!< Success code.
        ELFERR_BASE = -1000,                        //!< Error code base.
        ELFERR_UNIMPLEMENTED = (ELFERR_BASE-1),     //!< Unimplemented function.
        ELFERR_INTERNAL_ERR = (ELFERR_BASE-2),      //!< internal programming error
        ELFERR_FILEACCESS = (ELFERR_BASE-3),        //!< File open failed due to file access error
        ELFERR_FILEOPEN_ERR = (ELFERR_BASE-4),      //!< File open failed due to some file open error
        ELFERR_FILE_NOT_FOUND = (ELFERR_BASE-5),    //!< File not found
        ELFERR_NOT_ELF_FILE = (ELFERR_BASE-6),      //!< file is not an elf file.
        ELFERR_INVALID_FILE_DATA = (ELFERR_BASE-7), //!< invalid file data
        ELFERR_NO_MEMORY       = (ELFERR_BASE-8),   //!< Out of memory
        ELFERR_BUFFER_TOO_SHORT = (ELFERR_BASE-9),  //!< buffer is too short
        ELFERR_FILE_NOT_OPEN = (ELFERR_BASE-10),    //!< file is not open
        ELFERR_INVALID_PARAM = (ELFERR_BASE-11),    //!< invalid parameter
    
    };
#endif


class Elf64File
{
public:
    Elf64File();
    virtual ~Elf64File();

    /////////////////////////////////////////////////////////////
    // int Open(const char *pszFileName);
    /*!
    // Open the Elf64 file and read the header into memory.
    // verify the file is indeed an Elf64 file.
    //
    // @param pszFileName [in] the file name of elf file to open.
    // 
    // @return ELFERR_SUCCESS if successful.
    //               - error codes TBD.
    //
    */
    int Open(const char *pszFileName);
    
    ///////////////////////////////////////////////////////////////
    // void Close();
    /*!
    // close the file opened by the Open call.
    //
    //
    */
    void Close();

    ////////////////////////////////////////////////////////////////
    // bool IsOpen();
    /*
    // Check to see if the elf file is open.
    // 
    // @param none.
    //
    // @return true if file is open.
    //
    */
    bool IsOpen();

    ////////////////////////////////////////////////////////////
    // const Elf32_Ehdr *GetHeader() const;
    /*!
    // Retrieve the file header associated the Elf64 file.
    //
    // @param   none.
    // 
    // @return -- pointer to the file header.
    //
    */
    const Elf64_Ehdr *GetHeader() const;

    ///////////////////////////////////////////////////////////
    // const Elf64_Shdr *GetSectionHeader(int nSection) const; 
    /*!
    // Get a pointer to the section header.
    // 
    // @param   nSection [in] section number of header to retrieve.
    // 
    // @return -- pointer to section header.
    //
    */
    const Elf64_Shdr *GetSectionHeader(int nSection) const; 

    ///////////////////////////////////////////////////////////
    // const Elf64_Shdr *GetSectionHeader(int nSection) const; 
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
    const Elf64_Shdr *GetSectionHeader(int nSection,
                                       std::string &szName) const; 
    
    ////////////////////////////////////////////////////////////
    // int GetNumSectionHeaders();
    /*!
    // Retrieve the total number of section headers in this elf file.
    //
    // @param   none.
    // 
    // @return  Total number of section headers.
    //
    */
    int GetNumSectionHeaders() const;

    /////////////////////////////////////////////////////////////
    // const Elf64_Phdr *GetProgramHeader(int nHeader);
    /*!
    // Retrieve a pointer to the program header
    //
    // @param  nHeader [in] header to retrieve (0.. NumProgramHeaders);
    //
    // @return  Pointer to the program header.
    //
    */
    const Elf64_Phdr *GetProgramHeader(int nHeader) const;

    /////////////////////////////////////////////////////////////
    // const Elf64_Phdr *GetProgramHeader(const Elf64_Shdr *pSectHdr);
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
    const Elf64_Phdr *GetProgramHeader(const Elf64_Shdr *pSectHdr) const;

    /////////////////////////////////////////////////////////////
    // int GetSectionPhysAddr(const Elf64_Shdr *pSectHdr,
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
    int GetSectionPhysAddr(const Elf64_Shdr *pSectHdr,
                           Elf64_Addr &ulPhysAddr);



    //////////////////////////////////////////////////////////////////
    // int GetNumProgramHeaders();
    /*!
    // retrieve the total number of program headers in this file
    //
    // inputs:
    //    none.
    // outputs:
    //    returns -- total number of program headers in this elf file.
    //
    */
    int GetNumProgramHeaders() const;

    //////////////////////////////////////////////////////////////////
    // Elf64_Addr GetEntryPointAddress();
    /*!
    // retrieve the entry point address from the file header
    //
    // inputs:
    //    none.
    // outputs:
    //    returns -- entry point address for this elf file.
    //
    */
    Elf64_Addr GetEntryPointAddress() const;

    ////////////////////////////////////////////////////////
    // int GetSectionData(int nSection,
    //                    unsigned char *pData,
    //                    uint32_t &nLen)
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
    int GetSectionData(int nSection,
                       void *pData,
                       uint32_t &nLen);

    ////////////////////////////////////////////////////////
    // int GetSegmentData(int nSegment,
    //                    unsigned char *pData,
    //                    uint32_t &nLen)
    /*!
    // Retrieve the segment data specified and place it in the
    // data buffer indicated.
    //
    // If the pData pointer is NULL then just return the length 
    // of the data buffer needed to read the data.
    //
    // If the data buffer is too short then return an error.
    //
    // @param   nSegment [in] segment number of header to retrieve.
    // @param   pData [in/out] buffer to stuff the data.
    // @param   nLen [in/out] On input the maximum length of the data to
    //                        read.
    //                        On output, the length of the data read.
    //
    // @return ELFERR_SUCCESS if ok.
    //         ELFERR_BUFFER_TOO_SHORT -- if the buffer is too short.
    */
    int GetSegmentData(int nSegment,
                       void *pData,
                       uint32_t &nLen);


protected:

private:
    int FileErr(int nErr) const;
    unsigned char *GetData(unsigned char *pData,
                           Elf64_Half &nValue);
    unsigned char *GetData(unsigned char *pData,
                           Elf64_Word &nValue);
    unsigned char *GetData(unsigned char *pData,
                           Elf64_Addr &nValue);
    int ReadProgramHeaders();
    int ReadSectionHeaders();
    int ReadStringTable();

    
    FILE *m_pFile;
    //int m_nFileHandle;  // file handle of the open file

    Elf64_Ehdr m_ElfFileHdr;

    // elf data encoding big endian or little endian
    int m_nDataType;    // ELFDATA2LSB or ELFDATA2MSB

    std::vector<Elf64_Phdr> m_vectProgramHeaders;
    std::vector<Elf64_Shdr> m_vectSectionHeaders;

    char *m_pStringTable;
    uint32_t m_ulStringTblSize;

};



#endif

