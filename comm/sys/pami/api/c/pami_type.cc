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
/// \file api/c/pami_type.cc
/// \brief ???
///
#include "sys/pami.h"
#include "common/type/TypeMachine.h"
using namespace PAMI::Type;

#ifndef RETURN_ERR_PAMI
#define RETURN_ERR_PAMI(code, ...) return (code)
#endif

TypeContig *PAMI_TYPE_CONTIG_MAX         = new TypeContig(1);

pami_type_t PAMI_TYPE_BYTE               = new TypeContig(TypeCode::PRIMITIVE_TYPE_BYTE);         // ----------------------???

pami_type_t PAMI_TYPE_SIGNED_CHAR        = new TypeContig(TypeCode::PRIMITIVE_TYPE_SIGNED_CHAR);
pami_type_t PAMI_TYPE_SIGNED_SHORT       = new TypeContig(TypeCode::PRIMITIVE_TYPE_SIGNED_SHORT);
pami_type_t PAMI_TYPE_SIGNED_INT         = new TypeContig(TypeCode::PRIMITIVE_TYPE_SIGNED_INT);
pami_type_t PAMI_TYPE_SIGNED_LONG        = new TypeContig(TypeCode::PRIMITIVE_TYPE_SIGNED_LONG);
pami_type_t PAMI_TYPE_SIGNED_LONG_LONG   = new TypeContig(TypeCode::PRIMITIVE_TYPE_SIGNED_LONG_LONG);

pami_type_t PAMI_TYPE_UNSIGNED_CHAR      = new TypeContig(TypeCode::PRIMITIVE_TYPE_UNSIGNED_CHAR);
pami_type_t PAMI_TYPE_UNSIGNED_SHORT     = new TypeContig(TypeCode::PRIMITIVE_TYPE_UNSIGNED_SHORT);
pami_type_t PAMI_TYPE_UNSIGNED_INT       = new TypeContig(TypeCode::PRIMITIVE_TYPE_UNSIGNED_INT);
pami_type_t PAMI_TYPE_UNSIGNED_LONG      = new TypeContig(TypeCode::PRIMITIVE_TYPE_UNSIGNED_LONG);
pami_type_t PAMI_TYPE_UNSIGNED_LONG_LONG = new TypeContig(TypeCode::PRIMITIVE_TYPE_UNSIGNED_LONG_LONG);

pami_type_t PAMI_TYPE_FLOAT              = new TypeContig(TypeCode::PRIMITIVE_TYPE_FLOAT);
pami_type_t PAMI_TYPE_DOUBLE             = new TypeContig(TypeCode::PRIMITIVE_TYPE_DOUBLE);
pami_type_t PAMI_TYPE_LONG_DOUBLE        = new TypeContig(TypeCode::PRIMITIVE_TYPE_LONG_DOUBLE);

// Deprecated pami type. Must remain for binary compatibility
pami_type_t PAMI_TYPE_LOGICAL            = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOGICAL4);

pami_type_t PAMI_TYPE_LOGICAL1           = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOGICAL1);
pami_type_t PAMI_TYPE_LOGICAL2           = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOGICAL2);
pami_type_t PAMI_TYPE_LOGICAL4           = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOGICAL4);
pami_type_t PAMI_TYPE_LOGICAL8           = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOGICAL8);

pami_type_t PAMI_TYPE_SINGLE_COMPLEX     = new TypeContig(TypeCode::PRIMITIVE_TYPE_SINGLE_COMPLEX);
pami_type_t PAMI_TYPE_DOUBLE_COMPLEX     = new TypeContig(TypeCode::PRIMITIVE_TYPE_DOUBLE_COMPLEX);

pami_type_t PAMI_TYPE_LOC_2INT           = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_2INT);
pami_type_t PAMI_TYPE_LOC_2FLOAT         = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_2FLOAT);
pami_type_t PAMI_TYPE_LOC_2DOUBLE        = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_2DOUBLE);
pami_type_t PAMI_TYPE_LOC_SHORT_INT      = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_SHORT_INT);
pami_type_t PAMI_TYPE_LOC_FLOAT_INT      = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_FLOAT_INT);
pami_type_t PAMI_TYPE_LOC_DOUBLE_INT     = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_DOUBLE_INT);
pami_type_t PAMI_TYPE_LOC_LONG_INT       = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_LONG_INT);
pami_type_t PAMI_TYPE_LOC_LONGDOUBLE_INT = new TypeContig(TypeCode::PRIMITIVE_TYPE_LOC_LONGDOUBLE_INT);


pami_data_function PAMI_DATA_COPY        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_COPY;
pami_data_function PAMI_DATA_NOOP        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_NOOP;
pami_data_function PAMI_DATA_MAX         = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_MAX;
pami_data_function PAMI_DATA_MIN         = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_MIN;
pami_data_function PAMI_DATA_SUM         = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_SUM;
pami_data_function PAMI_DATA_PROD        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_PROD;
pami_data_function PAMI_DATA_LAND        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_LAND;
pami_data_function PAMI_DATA_LOR         = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_LOR;
pami_data_function PAMI_DATA_LXOR        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_LXOR;
pami_data_function PAMI_DATA_BAND        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_BAND;
pami_data_function PAMI_DATA_BOR         = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_BOR;
pami_data_function PAMI_DATA_BXOR        = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_BXOR;
pami_data_function PAMI_DATA_MAXLOC      = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_MAXLOC;
pami_data_function PAMI_DATA_MINLOC      = (pami_data_function) TypeFunc::PRIMITIVE_FUNC_MINLOC;

extern "C" {

  // BGQ does not use exceptions
#ifdef __pami_target_bgq__
#ifdef try
#undef try
#endif
#define try
#ifdef catch
#undef catch
#endif
#define catch(a) if(0)
#endif

pami_result_t PAMI_Type_create (pami_type_t * type)
{
    TypeCode * type_obj;
    try {
        type_obj = new TypeCode();
    } catch (std::bad_alloc) {
        RETURN_ERR_PAMI(PAMI_ENOMEM, "Out of memory.\n");
    }
    type_obj->AcquireReference();
    *type = (pami_type_t)type_obj;
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_add_simple (pami_type_t type,
                                    size_t     bytes,
                                    size_t     offset,
                                    size_t     count,
                                    size_t     stride)
{
    TypeCode * type_obj = (TypeCode *)type;
    if (type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Modifying a completed type.\n");
    }

    try {
        type_obj->AddShift(offset);
        type_obj->AddSimple(bytes, stride, count);
    } catch (std::bad_alloc) {
        RETURN_ERR_PAMI(PAMI_ENOMEM, "Out of memory.\n");
    }
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_add_typed (pami_type_t type,
                                   pami_type_t subtype,
                                   size_t     offset,
                                   size_t     count,
                                   size_t     stride)
{
    TypeCode * type_obj = (TypeCode *)type;
    if (type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Modifying a completed type.\n");
    }

    TypeCode * subtype_obj = (TypeCode *)subtype;
    if (!subtype_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Adding an incomplete subtype.\n");
    }

    try {
        type_obj->AddShift(offset);
        type_obj->AddTyped(subtype_obj, stride, count);
    } catch (std::bad_alloc) {
        RETURN_ERR_PAMI(PAMI_ENOMEM, "Out of memory.\n");
    }
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_complete (pami_type_t type, size_t atom_size)
{
    TypeCode * type_obj = (TypeCode *)type;
    if (type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Completing a completed type.\n");
    }

    // TODO: make MAX_ATOM_SIZE global and queriable
    const unsigned MIN_ATOM_SIZE = 1, MAX_ATOM_SIZE = 128;
    if (atom_size < MIN_ATOM_SIZE || atom_size > MAX_ATOM_SIZE) {
        RETURN_ERR_PAMI(PAMI_INVAL,
                "Atom size %lu is out of range [%lu, %lu].\n",
                atom_size, MIN_ATOM_SIZE, MAX_ATOM_SIZE);
    }

    size_t unit = type_obj->GetUnit();
    if (unit % atom_size != 0) {
        RETURN_ERR_PAMI(PAMI_INVAL,
                "Atom size %lu does not divide type unit %lu.\n",
                atom_size, unit);
    }

    try {
        type_obj->Complete();
        type_obj->SetAtomSize(atom_size);
    } catch (std::bad_alloc) {
        RETURN_ERR_PAMI(PAMI_ENOMEM, "Out of memory.\n");
    }
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_destroy (pami_type_t * type)
{
    TypeCode * type_obj = *(TypeCode **)type;
    *type = NULL;

    type_obj->ReleaseReference();
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_serialize (pami_type_t   type,
                                   void       ** address,
                                   size_t      * size)
{
    TypeCode * type_obj = (TypeCode *)type;
    if (! type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Serializing an incompleted type.\n");
    }
    *address = type_obj->GetCodeAddr();
    *size    = type_obj->GetCodeSize();
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_deserialize (pami_type_t * type,
                                     void        * address,
                                     size_t        size)
{
    try {
        TypeCode * type_obj = new TypeCode(address, size);
        assert(size == type_obj->GetCodeSize());
        type_obj->AcquireReference();
        *type = (pami_type_t)type_obj;
    } catch (std::bad_alloc) {
        *type = NULL;
        RETURN_ERR_PAMI(PAMI_INVAL, "Out of memory creating type.\n");
    }
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_query (pami_type_t           type,
                               pami_configuration_t  config[],
                               size_t                num_configs)
{
    TypeCode * type_obj = (TypeCode *)type;
    if (!type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Querying an incompleted type.\n");
    }

    for (unsigned i = 0; i < num_configs; i++) {
        switch (config[i].name) {
            case PAMI_TYPE_DATA_SIZE:
                config[i].value.intval = type_obj->GetDataSize();
                break;
            case PAMI_TYPE_DATA_EXTENT:
                config[i].value.intval = type_obj->GetExtent();
                break;
            case PAMI_TYPE_ATOM_SIZE:
                config[i].value.intval = type_obj->GetAtomSize();
                break;
            default:
                RETURN_ERR_PAMI(PAMI_INVAL,
                        "Invalid attribute '%s' to query.\n", config[i].name);
        }
    }
    return PAMI_SUCCESS;
}

pami_result_t PAMI_Type_transform_data (void               * src_addr,
                                        pami_type_t          src_type,
                                        size_t               src_offset,
                                        void               * dst_addr,
                                        pami_type_t          dst_type,
                                        size_t               dst_offset,
                                        size_t               size,
                                        pami_data_function   data_fn,
                                        void               * cookie)
{
    TypeCode * src_type_obj = (TypeCode *)src_type;
    TypeCode * dst_type_obj = (TypeCode *)dst_type;
    if (!src_type_obj->IsCompleted() || !dst_type_obj->IsCompleted()) {
        RETURN_ERR_PAMI(PAMI_INVAL, "Using incomplete type.\n");
    }

    if (likely(src_type_obj->IsContiguous() && dst_type_obj->IsContiguous()))
    {
        memcpy((char *)dst_addr+dst_offset,(char*)src_addr+src_offset,size); 
    }
    else if (src_type_obj->IsContiguous()) {
        // unpacking: contiguous to non-contiguous (or contiguous)
        TypeMachine unpacker(dst_type_obj);
        unpacker.SetCopyFunc(data_fn, cookie);
        unpacker.MoveCursor(dst_offset);
        unpacker.Unpack(dst_addr, (char *)src_addr + src_offset, size);

    } else if (dst_type_obj->IsContiguous()) {
        // packing: non-contiguous to contiguous
        TypeMachine packer(src_type_obj);
        packer.SetCopyFunc(data_fn, cookie);
        packer.MoveCursor(src_offset);
        packer.Pack((char *)dst_addr + dst_offset, src_addr, size);

    } else {
        // generic: non-contiguous to non-contiguous
        TypeMachine packer(src_type_obj);
        packer.MoveCursor(src_offset);

        TypeMachine unpacker(dst_type_obj);
        unpacker.SetCopyFunc(data_fn, cookie);
        unpacker.MoveCursor(dst_offset);

        // use a temporary buffer to copy in and out data
        const size_t TMP_BUF_SIZE = 8192;
        char tmp_buf[TMP_BUF_SIZE];

        for (size_t offset = 0; offset < size; offset += TMP_BUF_SIZE) {
            size_t bytes_to_copy = std::min(size - offset, TMP_BUF_SIZE);
            packer.Pack(tmp_buf, (char *)src_addr, bytes_to_copy);
            unpacker.Unpack((char *)dst_addr, tmp_buf, bytes_to_copy);
        }
    }

    return PAMI_SUCCESS;
}

} // extern "C"
