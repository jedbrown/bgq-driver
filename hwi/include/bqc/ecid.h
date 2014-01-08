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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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

#ifndef __hwi_bqc_ecid__h__
#define __hwi_bqc_ecid__h__


/*!
// \file bqc_ecid.h                                                                                                                           
//                                                                                                                                            
// \brief BQC ECID                                                                                                                            
//                                                                                                                                            
// described in                                                                                                                               
//                                                                                                                                            
// https://yktgsa.ibm.com/projects/b/bgq/web/design/testint/BQC_JTAG_interface.pdf                                                            
//  revision - 1.7 section 4.2.8.1 
// the headerfile <bgq_util/include/BgEcidDecode.h> contains the detailed decode
//
*/
 


/** \brief BQC ECID Structure */
typedef struct  BqcEcid
{

   std::string asString() { return(_serialized_data); };
   std::string asString(unsigned size)    {
      return(_serialized_data.substr(0, size)); };

    void clear()  
    { 
        _serialized_data.assign(256/4,'0');
        _identity_str.assign(127/4, '0');
        _selectiveVoltageBinning = 0;      
        _waferId = 0;            
        _xpos = 0;                         
        _ypos = 0;                         
        _fvba = 0;                         
        _svb_bins = 0;                     
        _max_svc_psro = 0;                 
        _max_hvt_psro = 0;                 
        _max_rvt_psro = 0;                 
        _ecc = 0;                          
        _redundancy_ctrl = 0;              
        _partial_good = 0;                 
        _reduncancy_avail = 0;             
        _version = 0;                      
        _iddq = 0;                         
        _delta_svt_psro = 0;               
        _delta_hvt_psro = 0;               
        _delta_rvt_psro = 0;               
        _burnt_in = 0;                     
    }

    bool partial_good_valid() {
       return (_reduncancy_avail);
    };
    /** \brief Return Partial Identifier: bit 146 - '0' of AG/PG (17 or 18 cores good) ;  '1' for HG (9 cores good out of first 10) */
    bool partial_good() {
       return (_partial_good);
    }
    
    /** \brief Return core redundancy mask/info - bits 128:145, 18 bits, 0 =good core, 1=bad core,  Bit 128 is for core 0 and bit 145 is for core 17 */
    unsigned core_redundancy_mask() {
       return (_redundancy_ctrl); 
    }
    

    std::string _serialized_data;           //! serialized data to use for storing in vpd

    std::string _identity_str;
    unsigned _selectiveVoltageBinning;      //! 0 :  RVT Mean PSRO >= 179 ns   (slower than svb-cut)            
                                            //! 1 :  RVT Mean PSRO < 179 ns     (faster than svb cut) 
    unsigned long long _waferId;            //!    4-63        WaferID                                          
    unsigned _xpos;                         //!    64-71       x position of chip on wafer                      
    unsigned _ypos;                         //!    72-79       y position of chip on wafer                      
    unsigned _fvba;                         //!    80-81       FVBA  (??)                                       
    unsigned _svb_bins;                     //!    82-89       SVB bins:  82-85 = svb_cut; 86-89 = svb_psro     
    unsigned _max_svc_psro;                 //!    90:98       Max SVT PSRO   (psro0)                           
    unsigned _max_hvt_psro;                 //!    99:107      Max  HVT PSRO (psro2)                            
    unsigned _max_rvt_psro;                 //!    108:116     Max RVT PSRO  (psro3)                            
                                            //!    117:119     reserved                                         
    unsigned _ecc;                          //!    120:127     ECC  (presumably over bits 0:119)

                                            //    ECID macro bits 128:255 (user defined fields).      ECID                                                                                          
    unsigned _redundancy_ctrl;              //    0:17    Redundancy control: 1 bit per PUnit (core), reflecting manufacturing test results. 0 is good core, 1 is bad core. See also section 4.2.2.4
    unsigned _partial_good;                 //    18          0 = All Good/Partial Good chip (i.e. 17 or 18 cores good)                                                                             
                                            //                1 = Half Good chip  (logical core 0-8 are good)                                                                                       
    unsigned _reduncancy_avail;             //    19          0 = no redundancy info available in bits 0:18                                                                                         
                                            //                1 = redundancy info in bits 0-18 is valid.                                                                                            
    unsigned _version;                      //    20:27   Version:                                                                                                                                  
                                            //                0 = BQC DD1.0                                                                                                                         
    unsigned _iddq;                         //    28:36   Iddq (in units of 100 mA)                                                                                                                 
    unsigned _delta_svt_psro;               //    37:42   Delta SVT PSRO (ns)   (psro0)                                                                                                             
    unsigned _delta_hvt_psro;               //    43:48   Delta HVT PSRO (ns)  (psro2)                                                                                                              
    unsigned _delta_rvt_psro;               //    49:54   Delta RVT PSRO  (ns)  (psro3)                                                                                                             
    unsigned _burnt_in;                     //    55          0 = not burnt-in                                                                                                                      
                                            //                1 = burnt-in                                                                                                                          


    BqcEcid()  {
        clear();
    };

} BqcEcid_t;


#endif /* __hwi_bqc_ecid_h__ */
