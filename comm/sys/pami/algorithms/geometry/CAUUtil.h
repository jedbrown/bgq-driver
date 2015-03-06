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
#ifndef __algorithms_geometry_CAUUtil_h__
#define __algorithms_geometry_CAUUtil_h__


#include "algorithms/geometry/Metadata.h"
#include "common/lapiunix/lapifunc.h"
namespace PAMI
{
  namespace CollRegistration
  {
    namespace CAU
    {
          const bool support[PAMI_DT_COUNT][PAMI_OP_COUNT] =
            {
         //  COPY,             NOOP,      MAX,      MIN,      SUM,      PROD,      LAND,      LOR,      LXOR,      BAND,      BOR,      BXOR,      MAXLOC,      MINLOC
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_BYTE
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_SIGNED_CHAR
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_SIGNED_SHORT
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//PRIMITIVE_TYPE_SIGNED_INT
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//PRIMITIVE_TYPE_SIGNED_LONG
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//PRIMITIVE_TYPE_SIGNED_LONG_LONG
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_UNSIGNED_CHAR
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_UNSIGNED_SHORT
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//PRIMITIVE_TYPE_UNSIGNED_INT
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//PRIMITIVE_TYPE_UNSIGNED_LONG
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     true,      true,     true,      false,       false},//PRIMITIVE_TYPE_UNSIGNED_LONG_LONG
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_FLOAT
            {false,            false,     true,     true,     true,     false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_DOUBLE
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LONG_DOUBLE
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOGICAL1
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOGICAL2
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOGICAL4
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOGICAL8
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_SINGLE_COMPLEX
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_DOUBLE_COMPLEX
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_2INT
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_2FLOAT
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_2DOUBLE
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_SHORT_INT
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_FLOAT_INT
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_DOUBLE_INT
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false},//PRIMITIVE_TYPE_LOC_LONG_INT
            {false,            false,     false,    false,    false,    false,     false,     false,    false,     false,     false,    false,     false,       false} //PRIMITIVE_TYPE_LOC_LONGDOUBLE_INT
            };

      template <class T_reduce_type>
      inline metadata_result_t op_dt_metadata_function(struct pami_xfer_t *in)
        {
          T_reduce_type     *reduction = (T_reduce_type*)&in->cmd;
          metadata_result_t  result    = {0};
          uintptr_t          op;
          uintptr_t          dt;
          PAMI::Type::TypeFunc::GetEnums(reduction->stype,
                                         reduction->op,
                                         dt,
                                         op);
          if(op < PAMI_OP_COUNT && dt < PAMI_DT_COUNT)
            result.check.datatype_op = support[dt][op]?0:1;
          else
            result.check.datatype_op = false;

          return(result);
        }
    }//PAMI
  }//CollRegistration
}//pami

#endif
