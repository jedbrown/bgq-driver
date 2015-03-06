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
/**
 * \file common/type/ReferenceCount.h
 * \brief ???
 */
#ifndef __common_type_ReferenceCount_h__
#define __common_type_ReferenceCount_h__

#include <assert.h>
#include "Compiler.h"

namespace PAMI
{

    class ReferenceCount
    {
        public:
            ReferenceCount();
            virtual ~ReferenceCount();
            void    AcquireReference();
            void    ReleaseReference();
            ssize_t GetReferenceCount() const;
        private:
            ssize_t ref_cnt;
    };

    inline ReferenceCount::ReferenceCount()
        : ref_cnt(0)
    {
    }

    inline ReferenceCount::~ReferenceCount()
    {
        assert(ref_cnt == 0);
    }

    inline void ReferenceCount::AcquireReference()
    {
        __sync_fetch_and_add(&ref_cnt, 1);
    }

    inline void ReferenceCount::ReleaseReference()
    {
        assert(ref_cnt > 0);
        ssize_t old_cnt = __sync_fetch_and_add(&ref_cnt, -1);
        if (old_cnt == 1)
            delete this;

        // NOTE: The reference-counted object must always be from the heap.
        //       Otherwise delete will cause segfault.
    }

    inline ssize_t ReferenceCount::GetReferenceCount() const
    {
        return ref_cnt;
    }

}

#endif // _PAMI_REFERENCE_COUNT_H
