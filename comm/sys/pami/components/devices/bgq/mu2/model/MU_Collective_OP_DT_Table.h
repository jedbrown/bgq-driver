/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
///
/// \file components/devices/bgq/mu2/model/MU_Collective_OP_DT_Table.h
/// \brief ???
///
#ifndef __components_devices_bgq_mu2_model_MU_Collective_OP_DT_Table_h__
#define __components_devices_bgq_mu2_model_MU_Collective_OP_DT_Table_h__

#include "sys/pami.h"
#include "spi/include/mu/DescriptorBaseXX.h"

namespace PAMI {
  namespace Device {
    namespace MU {

      const uint8_t unsupported_operation = 0xF0;

      //TheMUopcode-0xF0isinvalid
                                               //PAMI_COPY, PAMI_NOOP, PAMI_MAX,                                    PAMI_MIN,                                    PAMI_SUM,                                    PAMI_PROD, PAMI_LAND,                    PAMI_LOR,                    PAMI_LXOR,                    PAMI_BAND,                    PAMI_BOR,                    PAMI_BXOR,                    PAMI_MAXLOC, PAMI_MINLOC
#define MU_COLLECTIVE_DT_OP_UNDEFINED           {0xF0,      0xF0,      0xF0,                                        0xF0,                                        0xF0,                                        0xF0,      0xF0,                         0xF0,                        0xF0,                         0xF0,                         0xF0,                        0xF0,                         0xF0,        0xF0}
#define MU_COLLECTIVE_DT_OP_SIGNED_SHORT_INT    MU_COLLECTIVE_DT_OP_UNDEFINED
#define MU_COLLECTIVE_DT_OP_SIGNED_INT          {0xF0,      0xF0,      MUHWI_COLLECTIVE_OP_CODE_SIGNED_MAX,         MUHWI_COLLECTIVE_OP_CODE_SIGNED_MIN,         MUHWI_COLLECTIVE_OP_CODE_SIGNED_ADD,         0xF0,      MUHWI_COLLECTIVE_OP_CODE_AND, MUHWI_COLLECTIVE_OP_CODE_OR, MUHWI_COLLECTIVE_OP_CODE_XOR, MUHWI_COLLECTIVE_OP_CODE_AND, MUHWI_COLLECTIVE_OP_CODE_OR, MUHWI_COLLECTIVE_OP_CODE_XOR, 0xF0,        0xF0}
#define MU_COLLECTIVE_DT_OP_UNSIGNED_SHORT_INT  MU_COLLECTIVE_DT_OP_UNDEFINED
#define MU_COLLECTIVE_DT_OP_UNSIGNED_INT        {0xF0,      0xF0,      MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_MAX,       MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_MIN,       MUHWI_COLLECTIVE_OP_CODE_UNSIGNED_ADD,       0xF0,      MUHWI_COLLECTIVE_OP_CODE_AND, MUHWI_COLLECTIVE_OP_CODE_OR, MUHWI_COLLECTIVE_OP_CODE_XOR, MUHWI_COLLECTIVE_OP_CODE_AND, MUHWI_COLLECTIVE_OP_CODE_OR, MUHWI_COLLECTIVE_OP_CODE_XOR, 0xF0,        0xF0}
#define MU_COLLECTIVE_DT_OP_FLOAT               MU_COLLECTIVE_DT_OP_UNDEFINED
#define MU_COLLECTIVE_DT_OP_DOUBLE              {0xF0,      0xF0,      MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_MAX, MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_MIN, MUHWI_COLLECTIVE_OP_CODE_FLOATING_POINT_ADD, 0xF0,      MUHWI_COLLECTIVE_OP_CODE_AND, MUHWI_COLLECTIVE_OP_CODE_OR, MUHWI_COLLECTIVE_OP_CODE_XOR, MUHWI_COLLECTIVE_OP_CODE_AND, MUHWI_COLLECTIVE_OP_CODE_OR, MUHWI_COLLECTIVE_OP_CODE_XOR, 0xF0,        0xF0}

      const uint8_t mu_collective_op_table[PAMI_DT_COUNT][PAMI_OP_COUNT] =
      {
        MU_COLLECTIVE_DT_OP_UNSIGNED_SHORT_INT, //  PAMI_BYTE

        MU_COLLECTIVE_DT_OP_SIGNED_SHORT_INT,   //  PAMI_SIGNED_CHAR
        MU_COLLECTIVE_DT_OP_SIGNED_SHORT_INT,   //  PAMI_SIGNED_SHORT
        MU_COLLECTIVE_DT_OP_SIGNED_INT,         //  PAMI_SIGNED_INT
        MU_COLLECTIVE_DT_OP_SIGNED_INT,         //  PAMI_SIGNED_LONG
        MU_COLLECTIVE_DT_OP_SIGNED_INT,         //  PAMI_SIGNED_LONG_LONG

        MU_COLLECTIVE_DT_OP_UNSIGNED_SHORT_INT, //  PAMI_UNSIGNED_CHAR
        MU_COLLECTIVE_DT_OP_UNSIGNED_SHORT_INT, //  PAMI_UNSIGNED_SHORT
        MU_COLLECTIVE_DT_OP_UNSIGNED_INT,       //  PAMI_UNSIGNED_INT
        MU_COLLECTIVE_DT_OP_UNSIGNED_INT,       //  PAMI_UNSIGNED_LONG
        MU_COLLECTIVE_DT_OP_UNSIGNED_INT,       //  PAMI_UNSIGNED_LONG_LONG

        MU_COLLECTIVE_DT_OP_FLOAT,              //  PAMI_FLOAT
        MU_COLLECTIVE_DT_OP_DOUBLE,             //  PAMI_DOUBLE
        MU_COLLECTIVE_DT_OP_DOUBLE,             //  PAMI_LONG_DOUBLE

        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOGICAL1
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOGICAL2
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOGICAL4
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOGICAL8

        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_SINGLE_COMPLEX
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_DOUBLE_COMPLEX

        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_2INT
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_2FLOAT
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_2DOUBLE
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_SHORT_INT
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_FLOAT_INT
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_DOUBLE_INT
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_LONG_INT
        MU_COLLECTIVE_DT_OP_UNDEFINED,          //  PAMI_LOC_LONGDOUBLE_INT
      };
      
      const size_t mu_collective_size_table[PAMI_DT_COUNT] =
        {
          sizeof(unsigned char),      //  PAMI_BYTE

          sizeof(signed char),        //  PAMI_SIGNED_CHAR
          sizeof(signed short),       //  PAMI_SIGNED_SHORT
          sizeof(signed int),         //  PAMI_SIGNED_INT
          sizeof(signed long),        //  PAMI_SIGNED_LONG
          sizeof(signed long long),   //  PAMI_SIGNED_LONG_LONG

          sizeof(unsigned char),      //  PAMI_UNSIGNED_CHAR
          sizeof(unsigned short),     //  PAMI_UNSIGNED_SHORT
          sizeof(unsigned int),       //  PAMI_UNSIGNED_INT
          sizeof(unsigned long),      //  PAMI_UNSIGNED_LONG
          sizeof(unsigned long long), //  PAMI_UNSIGNED_LONG_LONG

          sizeof(float),              //  PAMI_FLOAT
          sizeof(double),             //  PAMI_DOUBLE
          sizeof(long double),        //  PAMI_LONG_DOUBLE

          1,                          //  PAMI_LOGICAL1
          2,                          //  PAMI_LOGICAL2
          4,                          //  PAMI_LOGICAL4
          8,                          //  PAMI_LOGICAL8

          -1,                         //  PAMI_SINGLE_COMPLEX
          -1,                         //  PAMI_DOUBLE_COMPLEX

          -1,                         //  PAMI_LOC_2INT
          -1,                         //  PAMI_LOC_2FLOAT
          -1,                         //  PAMI_LOC_2DOUBLE
          -1,                         //  PAMI_LOC_SHORT_INT
          -1,                         //  PAMI_LOC_FLOAT_INT
          -1,                         //  PAMI_LOC_DOUBLE_INT
          -1,                         //  PAMI_LOC_LONG_INT
          -1,                         //  PAMI_LOC_LONGDOUBLE_INT
        };
    }; //MU
  };   //Device
};     //PAMI

#endif
