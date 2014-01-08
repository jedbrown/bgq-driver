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

#ifndef __BG_ECID_DECODE_H__
#define __BG_ECID_DECODE_H__

#include <string>
#include <sstream>
#include <iomanip>

#include <hwi/include/bqc/ecid.h>
#include <bgq_util/include/ArbitraryLengthNumber.h>

/*!
// \file BgEcidDecoder.h                                                                                                                           
//                                                                                                                                            
// \brief BQC ECID Decoder                                                                                                                        

// https://yktgsa.ibm.com/projects/b/bgq/web/design/testint/BQC_JTAG_interface.pdf                                                            
//  revision - 1.7 section 4.2.8.1 
// ECID0                                                                                                                                                
//    Electronic Chip Identification , with the following sub-fields:                                                                                   
//    ECID                                                                                                                                              
//    0           Selective voltage binning (SVB):                                                                                                      
//                0 :  RVT Mean PSRO >= 179 ns   (slower than svb-cut)                                                                                  
//                1 :  RVT Mean PSRO < 179 ns     (faster than svb cut)                                                                                 
//    1-3         b000                                                                                                                                  
//    4-63        WaferID                                                                                                                               
//    64-71       x position of chip on wafer                                                                                                           
//    72-79       y position of chip on wafer                                                                                                           
//    80-81       FVBA  (??)                                                                                                                            
//    82-89       SVB bins:  82-85 = svb_cut; 86-89 = svb_psro                                                                                          
//    90:98       Max SVT PSRO   (psro0)                                                                                                                
//    99:107      Max  HVT PSRO (psro2)                                                                                                                 
//    108:116     Max RVT PSRO  (psro3)                                                                                                                 
//    117:119     reserved                                                                                                                              
//    120:127     ECC  (presumably over bits 0:119)                                                                                                     
//                                                                                                                                                      
// ECID1                                                                                                                                                
//    ECID macro bits 128:255 (user defined fields).      ECID                                                                                          
//    0:17    Redundancy control: 1 bit per PUnit (core), reflecting manufacturing test results. 0 is good core, 1 is bad core. See also section 4.2.2.4
//    18          0 = All Good/Partial Good chip (i.e. 17 or 18 cores good)                                                                             
//                1 = Half Good chip  (logical core 0-8 are good)                                                                                       
//    19          0 = no redundancy info available in bits 0:18                                                                                         
//                1 = redundancy info in bits 0-18 is valid.                                                                                            
//    20:27   Version:                                                                                                                                  
//                0 = BQC DD1.0                                                                                                                         
//    28:36   Iddq (in units of 100 mA)                                                                                                                 
//    37:42   Delta SVT PSRO (ns)   (psro0)                                                                                                             
//    43:48   Delta HVT PSRO (ns)  (psro2)                                                                                                              
//    49:54   Delta RVT PSRO  (ns)  (psro3)                                                                                                             
//    55          0 = not burnt-in                                                                                                                      
//                1 = burnt-in                                                                                                                          
//    56:127  reserved                                                                                                                                  
//                                                                                                                                                      
// ECID2:                                                                                                                                               
//    0:127   ECID macro bits 256:383 (static memory timing settings).                                                                                  
//                                                                                                                                                      
// ECID3:                                                                                                                                               
//    0:127   ECID macro bits 384:511 (static memory timing settings).    ECID                                                                          
*/

/** \brief BQC ECID decoder */

class BqcEcidDecode
{
public:


    /*!
    //  decode the raw ecid information from the                   
    //  tdrs and put it into the ecid hw structure                 
    //                                                             
    //  @param ecidTdr0..3 -- the three raw ecid arbitrary numbers.
    //  @param ecid [out] -- decoded ecid.                         
    //  @return none                                               
    */
    static void decode(const ArbitraryLengthNumber &ecidTdr0,
                       const ArbitraryLengthNumber &ecidTdr1,
                       const ArbitraryLengthNumber &ecidTdr2,
                       const ArbitraryLengthNumber &ecidTdr3,
                       BqcEcid &bqcEcid) {

            
            bqcEcid._serialized_data = ecidTdr0.getHexString() + ecidTdr1.getHexString();
            bqcEcid._identity_str = ecidTdr0.getHexString();
        
            bqcEcid._selectiveVoltageBinning = ecidTdr0.getUint32(0,0);       //    0           Selective voltage binning (SVB):                                                                                                      
            bqcEcid._waferId = ecidTdr0.getUint64(4,63);                      //    4-63        WaferID                                          
            bqcEcid._xpos = ecidTdr0.getUint32(64,71);                        //    64-71       x position of chip on wafer                      
            bqcEcid._ypos = ecidTdr0.getUint32(72,79);                        //    72-79       y position of chip on wafer                      
            bqcEcid._fvba  = ecidTdr0.getUint32(80,81);                       //    80-81       FVBA  (??)                                       
            bqcEcid._svb_bins  = ecidTdr0.getUint32(82,89);                   //    82-89       SVB bins:  82-85 = svb_cut; 86-89 = svb_psro     
            bqcEcid._max_svc_psro  = ecidTdr0.getUint32(90,98);               //    90:98       Max SVT PSRO   (psro0)                           
            bqcEcid._max_hvt_psro  = ecidTdr0.getUint32(99,107);              //    99:107      Max  HVT PSRO (psro2)                            
            bqcEcid._max_rvt_psro = ecidTdr0.getUint32(108,116);              //    108:116     Max RVT PSRO  (psro3)                            
            bqcEcid._ecc = ecidTdr0.getUint32(120,127);;                      //    120:127     ECC  (presumably over bits 0:119)
        
                                //    ecidTdr macro bits 128:255 (user defined fields).      ecidTdr                                                                                          
            bqcEcid._redundancy_ctrl = ecidTdr1.getUint32(0,17);              //    0:17    Redundancy control: 1 bit per PUnit (core), reflecting 
                                                                              //            manufacturing test results. 0 is good core, 1 is bad core. See also section 4.2.2.4
            bqcEcid._partial_good = ecidTdr1.getUint32(18,18);                //    18          0 = All Good/Partial Good chip (i.e. 17 or 18 cores good)                                                                             
            bqcEcid._reduncancy_avail = ecidTdr1.getUint32(19,19);            //    19          0 = no redundancy info available in bits 0:18                                                                                         
            bqcEcid._version = ecidTdr1.getUint32(20,27);                     //    20:27   Version:                                                                                                                                  
            bqcEcid._iddq = ecidTdr1.getUint32(28,36);                        //    28:36   Iddq (in units of 100 mA)                                                                                                                 
            bqcEcid._delta_svt_psro = ecidTdr1.getUint32(37,42);              //    37:42   Delta SVT PSRO (ns)   (psro0)                                                                                                             
            bqcEcid._delta_hvt_psro = ecidTdr1.getUint32(43,48);              //    43:48   Delta HVT PSRO (ns)  (psro2)                                                                                                              
            bqcEcid._delta_rvt_psro = ecidTdr1.getUint32(49,54);              //    49:54   Delta RVT PSRO  (ns)  (psro3)                                                                                                             
            bqcEcid._burnt_in = ecidTdr1.getUint32(55,55);                    //    55          0 = not burnt-in                                                                                                                      



    };
    
    /*!
    // convert a stored string into a ecid value.
    // @param str -- string to convert           
    // @param bqcEcid -- deserialized structure  
    // @return none                              
    */
    static void deserialize(const std::string &str, BqcEcid &bqcEcid)
    {
        ArbitraryLengthNumber ecidTdr0(128), ecidTdr1(128), ecidTdr2(128), ecidTdr3(128);
        std::string str0(str.substr(0*128/4,128/4));     // lsb to msb ...
        std::string str1(str.substr(1*128/4,128/4));     // for now we only save the first 256 bytes

        ecidTdr0.setHexString(str0, 128);
        ecidTdr1.setHexString(str1, 128);

        decode(ecidTdr0, ecidTdr1, ecidTdr2, ecidTdr3, bqcEcid);
    };

    /*!
    // serialize the BqcEcid structure into a simple string to be stored in the vpd
    // @param bqcEcid -- ecid structure to serialze.                               
    // @return -- serialized string ready for storge                               
    */
    static std::string serialize(const BqcEcid &bqcEcid) {
        return(bqcEcid._serialized_data);
    }

};

#endif

