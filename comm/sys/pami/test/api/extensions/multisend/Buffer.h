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
 * \file test/api/extensions/multisend/Buffer.h
 * \brief Multicast test buffer support class
 */
#ifndef __test_api_extensions_multisend_Buffer_h__
#define __test_api_extensions_multisend_Buffer_h__

#include <stdio.h>

#include <pami.h>

#include "PipeWorkQueue.h"
#include "Topology.h"

#define DBG_FPRINTF(x) /*fprintf x */
#define DBGF_FUNCTIONNAME DBG_FPRINTF((stderr,"%.*s\n",_function_name_len(__PRETTY_FUNCTION__),_function_name(__PRETTY_FUNCTION__)))

namespace PAMI
{
  namespace Test
  {
    char* _function_name(const char*name)
    {
      char* pName = strstr((char*)name,">::");
      pName += strlen(">::");
      return pName;
    }
    unsigned _function_name_len(const char*name)
    {
      char* pName = _function_name(name);
      char* pEndName = strstr(pName,"[with");
      return(pEndName - pName);
    }


    template <size_t T_BufSize>
    class Buffer
    {
    private:
      char                _source[T_BufSize];
      PAMI::PipeWorkQueue  _srcPwq;
      PAMI::PipeWorkQueue *_pSrcPwq;

      char                _destination[T_BufSize];
      PAMI::PipeWorkQueue  _dstPwq;
      PAMI::PipeWorkQueue *_pDstPwq;

    public:
      Buffer(bool isRoot  = false) :
      _pSrcPwq(&_srcPwq),
      _pDstPwq(&_dstPwq)
      {
        DBG_FPRINTF((stderr,"<%p>%s src %p/%p, dst %p/%p\n",this,__PRETTY_FUNCTION__, &_srcPwq, _source, &_dstPwq, _destination));
        reset(isRoot);
      }

      ~Buffer()
      {
      }
      PAMI::PipeWorkQueue * srcPwq()
      {
        return _pSrcPwq;
      }
      PAMI::PipeWorkQueue * dstPwq()
      {
        return _pDstPwq;
      }
      inline void reset(bool isRoot  = false)
      {
        DBGF_FUNCTIONNAME;
        return reset(&_srcPwq,&_dstPwq,isRoot);
      }

      inline void reset(PAMI::PipeWorkQueue* src,
                        PAMI::PipeWorkQueue* dst,
                        bool isRoot  = false)
      {
        DBGF_FUNCTIONNAME;
        DBG_FPRINTF((stderr,"<%p> src %p, dst %p\n",this,src, dst));
        src->configure(_source, sizeof(_source), sizeof(_source));

        dst->configure(_destination, sizeof(_destination), 0);

        return setup(src, dst, isRoot);
      }

      inline void set(PAMI::PipeWorkQueue* src,
                      PAMI::PipeWorkQueue* dst)
      {
        DBGF_FUNCTIONNAME;
        DBG_FPRINTF((stderr,"<%p> src %p, dst %p\n",this,src, dst));
        _pSrcPwq = src;
        _pDstPwq = dst;
        return;
      }

      char* buffer()
      {
        return _source;
      }
      inline void setup(bool isRoot  = false)
      {
        DBGF_FUNCTIONNAME;
        return setup(&_srcPwq,&_dstPwq,isRoot);
      }

      inline void setup(PAMI::PipeWorkQueue* src,
                        PAMI::PipeWorkQueue* dst,
                        bool isRoot  = false,
                        size_t count = T_BufSize)
      {
        DBGF_FUNCTIONNAME;

        _pSrcPwq = src;
        _pDstPwq = dst;

        char* source      = _pSrcPwq->bufferToConsume();
        char* destination = _pDstPwq->bufferToProduce();
        DBG_FPRINTF((stderr, "src %p/%p, dst %p/%p\n",
                     src, source, dst, destination));

        size_t count_of_unsigneds = count/sizeof(unsigned);
        unsigned value = 0;
        size_t x;

        for(x = 0; x < count_of_unsigneds; ++x)
        {
          ((unsigned *)source)[x] = isRoot? value++ : -1;
          ((unsigned *)destination)[x] = -1;
        }
        return ;
      }

      inline pami_result_t validate(size_t &bytesConsumed,
                                   size_t &bytesProduced,
                                   bool isRoot  = false,
                                   bool isDest  = true,
                                   size_t count = T_BufSize)
      {
        return validate(_pSrcPwq,
                        _pDstPwq,
                        bytesConsumed,
                        bytesProduced,
                        isRoot,
                        isDest,
                        count);
      }
      inline pami_result_t validate(PAMI::PipeWorkQueue* src,
                                   PAMI::PipeWorkQueue* dst,
                                   size_t &bytesConsumed,
                                   size_t &bytesProduced,
                                   bool isRoot  = false,
                                   bool isDest  = true,
                                   size_t count = T_BufSize)
      {
        DBGF_FUNCTIONNAME;

        _pSrcPwq = src;
        _pDstPwq = dst;

        bytesConsumed = _pSrcPwq->getBytesConsumed();
        bytesProduced = _pDstPwq->getBytesProduced();

        _pSrcPwq->reset();
        _pDstPwq->reset();

        char* source      = _pSrcPwq->bufferToConsume();
        char* destination = _pDstPwq->bufferToProduce();

        DBG_FPRINTF((stderr, "src %p/%p, dst %p/%p, bytesConsumed %zu, bytesProduced %zu\n",
                     src, source, dst, destination, bytesConsumed, bytesProduced));

        unsigned errors = 0;
        unsigned value = 0;
        size_t count_of_unsigneds = count/sizeof(unsigned);
        size_t x;


        if(isRoot) /* validate untouched src buffer */
          for(x = 0; x < count_of_unsigneds && errors < 5 ; ++x)
          {
            if(((unsigned *)source)[x] != value++)
            {
              fprintf(stderr, "Corrupted source buffer(%d) at index %zu.\n",((unsigned *)source)[x], x);
              errors++;/*break; */
            }
          }
        else /* validate untouched buffer (-1) */
          for(x = 0; x < count_of_unsigneds  && errors < 5 ; ++x)
          {
            if(((unsigned *)source)[x] != (unsigned)-1)
            {
              fprintf(stderr, "Corrupted source buffer(%d) at index %zu.\n",((unsigned *)source)[x], x);
              errors++;/*break; */
            }
          }

        value = 0;
        /* Validate the output data is correct */
        if(isDest) /* validate expected results */
          for(x = 0; x < count_of_unsigneds && errors < 5 ; ++x)
          {
            if(((unsigned *)destination)[x] != value++)
            {
              fprintf(stderr, "Incorrect result buffer(%d) at index %zu.\n",((unsigned *)destination)[x], x);
              errors++;/*break; */
            }
          }
        else /* validate untouched buffer (-1) */
          for(x = 0; x < count_of_unsigneds && errors < 5 ; ++x)
          {
            if(((unsigned *)destination)[x] != (unsigned)-1)
            {
              fprintf(stderr, "Corrupted result buffer(%d) at index %zu.\n",((unsigned *)destination)[x], x);
              errors++;/*break; */
            }
          }
        if(errors) /*(x - errors) < count_of_unsigneds) */
        {
          fprintf(stderr, "FAIL validation %d\n",errors);
          return PAMI_ERROR;
        }
        fprintf(stderr, "PASS validation\n");
        return PAMI_SUCCESS;
      }
      /*==================================================================== */
      /* Following MIN0 functions assume an unsigned/MIN [all]reduce. A designated "root" will set */
      /* the buffers to 0 so that should be the MIN result. */
      /*==================================================================== */
      inline void resetMIN0(bool isRoot  = false)
      {
        DBGF_FUNCTIONNAME;
        return resetMIN0(&_srcPwq,&_dstPwq,isRoot);
      }

      inline void resetMIN0(PAMI::PipeWorkQueue* src,
                        PAMI::PipeWorkQueue* dst,
                        bool isRoot  = false)
      {
        DBGF_FUNCTIONNAME;
        DBG_FPRINTF((stderr,"<%p> src %p, dst %p\n",this,src, dst));
        src->configure(_source, sizeof(_source), sizeof(_source));

        dst->configure(_destination, sizeof(_destination), 0);

        return setupMIN0(src, dst, isRoot);
      }

      inline void setupMIN0(bool isRoot  = false)
      {
        DBGF_FUNCTIONNAME;
        return setupMIN0(&_srcPwq,&_dstPwq,isRoot);
      }

      inline void setupMIN0(PAMI::PipeWorkQueue* src,
                        PAMI::PipeWorkQueue* dst,
                        bool isRoot  = false,
                        size_t count = T_BufSize)
      {
        DBGF_FUNCTIONNAME;

        _pSrcPwq = src;
        _pDstPwq = dst;

        char* source      = _pSrcPwq->bufferToConsume();
        char* destination = _pDstPwq->bufferToProduce();
        DBG_FPRINTF((stderr, "src %p/%p, dst %p/%p\n",
                     src, source, dst, destination));

        size_t count_of_unsigneds = count/sizeof(unsigned);
        size_t x;
        unsigned value = 0;

        for(x = 0; x < count_of_unsigneds; ++x)
        {
          ((unsigned *)source)[x] = isRoot? 0 : value++;
          ((unsigned *)destination)[x] = -1;
        }
        return ;
      }

      inline pami_result_t validateMIN0(size_t &bytesConsumed,
                                   size_t &bytesProduced,
                                   bool isRoot  = false,
                                   bool isDest  = true,
                                   size_t count = T_BufSize)
      {
        return validateMIN0(_pSrcPwq,
                        _pDstPwq,
                        bytesConsumed,
                        bytesProduced,
                        isRoot,
                        isDest,
                        count);
      }
      inline pami_result_t validateMIN0(PAMI::PipeWorkQueue* src,
                                   PAMI::PipeWorkQueue* dst,
                                   size_t &bytesConsumed,
                                   size_t &bytesProduced,
                                   bool isRoot  = false,
                                   bool isDest  = true,
                                   size_t count = T_BufSize)
      {
        DBGF_FUNCTIONNAME;

        _pSrcPwq = src;
        _pDstPwq = dst;

        bytesConsumed = _pSrcPwq->getBytesConsumed();
        bytesProduced = _pDstPwq->getBytesProduced();

        _pSrcPwq->reset();
        _pDstPwq->reset();

        char* source      = _pSrcPwq->bufferToConsume();
        char* destination = _pDstPwq->bufferToProduce();

        DBG_FPRINTF((stderr, "src %p/%p, dst %p/%p, bytesConsumed %zu, bytesProduced %zu\n",
                     src, source, dst, destination, bytesConsumed, bytesProduced));

        unsigned errors = 0;
        unsigned value = 0;
        size_t count_of_unsigneds = count/sizeof(unsigned);
        size_t x;


        if(isRoot) /* validate untouched src buffer */
          for(x = 0; x < count_of_unsigneds && errors < 5 ; ++x)
          {
            if(((unsigned *)source)[x] != 0)
            {
              fprintf(stderr, "Corrupted source buffer(%d) at index %zu.\n",((unsigned *)source)[x], x);
              errors++;/*break; */
            }
          }
        else /* validate untouched src buffer (-1) */
          for(x = 0; x < count_of_unsigneds  && errors < 5 ; ++x)
          {
            if(((unsigned *)source)[x] != value++)
            {
              fprintf(stderr, "Corrupted source buffer(%d) at index %zu.\n",((unsigned *)source)[x], x);
              errors++;/*break; */
            }
          }

        /* Validate the output data is correct */
        for(x = 0; x < count_of_unsigneds && errors < 5 ; ++x)
        {
          if(((unsigned *)destination)[x] != 0)
          {
            fprintf(stderr, "Incorrect result buffer(%d) at index %zu.\n",((unsigned *)destination)[x], x);
            errors++;/*break; */
          }
        }

        if(errors)
        {
          fprintf(stderr, "FAIL validation %d\n",errors);
          return PAMI_ERROR;
        }
        fprintf(stderr, "PASS validation\n");
        return PAMI_SUCCESS;
      }
    }; /* class Buffer */

  }; /* namespace Test */
}; /* namespace PAMI */

#endif /* __test_multisend_buffer_h__ */
