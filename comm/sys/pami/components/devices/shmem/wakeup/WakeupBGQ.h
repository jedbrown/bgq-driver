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
/// \file components/devices/shmem/wakeup/WakeupBGQ.h
/// \brief Blue Gene/Q "wakeup region" implementation of the shmem wakeup interface
///
#ifndef __components_devices_shmem_wakeup_WakeupBGQ_h__
#define __components_devices_shmem_wakeup_WakeupBGQ_h__

#include "Global.h"

#include "components/devices/shmem/wakeup/WakeupInterface.h"

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif

namespace PAMI
{
  namespace Wakeup
  {
    ///
    /// \brief Blue Gene/Q "wakeup region" implementation of the shmem wakeup interface
    ///
    class BGQ : public Wakeup::Interface < Wakeup::BGQ  >
    {
      public:

        friend class Wakeup::Interface <Wakeup::BGQ>;

        template <typename T>
        class Region : public Wakeup::Interface<BGQ>::Region<Region <T> >
        {
          public:

            friend class Wakeup::Interface<BGQ>::Region<Region <T> >;
#ifdef ELEMENT_ARRAY
            class Element
            {
              public:

                operator bool() const
                {
                  return (bool) this->value;
                };

                inline Element & operator= (size_t rhs)
                {
                  this->value = rhs;
                  return (*this);
                };

                inline Element & operator= (const Element & rhs)
                {
                  this->value = rhs.value;
                  return (*this);
                };

                inline bool operator== (size_t rhs) const
                {
                  return (bool) (this->value == rhs);
                };

                inline bool operator== (const Element & rhs) const
                {
                  return (bool) (this->value == rhs.value);
                };

                inline bool operator!= (size_t rhs) const
                {
                  return (bool) (this->value != rhs);
                };

                inline bool operator!= (const Element & rhs) const
                {
                  return (bool) (this->value != rhs.value);
                };

                size_t value;
            };
#endif
            inline void init_impl (Region & original)
            {
              _array = original._array;
            };

            template <class T_MemoryManager>
            inline void init_impl (T_MemoryManager * mm,
                                   char            * key,
                                   size_t            count,
                                   Wakeup::BGQ     * wakeup)
            {
              pami_result_t mmrc = PAMI_ERROR;

              mmrc = wakeup->_mm->memalign ((void **) & _array,
                                            16,                         // align
                                            count * sizeof(T),          // bytes
                                            key,                        // unique
                                            Memory::MemoryManager::memzero,
                                            NULL);                      // cookie

              PAMI_assertf(mmrc == PAMI_SUCCESS, "memalign failed for bgq wakeup resources, rc=%d\n", mmrc);
            };
#ifdef ELEMENT_ARRAY
            inline Element & operator[] (size_t position)
            {
              return _array[position];
            };

          private:

            Element * _array;
#else
            inline T & operator[] (size_t position) volatile
            {
              return _array[position];
            };

          private:

            T * _array;
#endif
        };  // class PAMI::Wakeup::BGQ::Region

        inline BGQ () :
            Wakeup::Interface <Wakeup::BGQ> (),
            _mm (NULL),
            _poke (NULL)
        {
        };

        inline BGQ (Wakeup::BGQ & clone) :
            Wakeup::Interface <Wakeup::BGQ> (clone),
            _mm (clone._mm),
            _poke (clone._poke)
        {
        };

        inline ~BGQ () {};

      protected:

        inline BGQ (PAMI::Memory::GenMemoryManager * mm) :
            Wakeup::Interface <Wakeup::BGQ> (),
            _mm (mm)
        {
          pami_result_t mmrc = PAMI_ERROR;

          mmrc = mm->memalign ((void **) & _poke,
                               16,                         // align
                               sizeof(size_t),             // bytes
                               "/wakeup-process-poke",     // unique
                               Memory::MemoryManager::memzero,
                               NULL);                      // cookie

          PAMI_assertf(mmrc == PAMI_SUCCESS, "memalign failed for bgq wakeup 'poke' resources, rc=%d\n", mmrc);
        };

        ///
        /// \see PAMI::Wakeup::Interface::generate()
        ///
        static BGQ * generate_impl (size_t peers, const char * key)
        {
          BGQ * wakeup = NULL;

          pami_result_t mmrc = PAMI_ERROR;
          mmrc = __global.heap_mm->memalign((void **) & wakeup,
                                            16,                      // align
                                            sizeof(*wakeup) * peers, // bytes
                                            key,                     // unique
                                            BGQ::array_initialize,   // static fn
                                            (void *)peers);          // cookie
          PAMI_assertf(mmrc == PAMI_SUCCESS, "heap memalign failed for bgq wakeup object array, rc=%d\n", mmrc);

          return wakeup;
        };

        ///
        /// \see PAMI::Wakeup::Interface::poke()
        ///
        inline pami_result_t poke_impl ()
        {
          (*_poke)++;

          return PAMI_SUCCESS;
        };

      private:

        ///
        /// \brief Initialize an array of bgq wakeup objects
        ///
        /// \see PAMI::Memory::MM_INIT_FN
        ///
        static void array_initialize (void       * memory,
                                      size_t       bytes,
                                      const char * key,
                                      unsigned     attributes,
                                      void       * cookie)
        {
          TRACE_ERR((stderr, ">> Wakeup::BGQ::array_initialize(%p, %zu, \"%s\", %d, %p)\n", memory, bytes, key, attributes, cookie));

          Wakeup::BGQ * wakeup = (Wakeup::BGQ *) memory;

          TRACE_ERR((stderr, "   Wakeup::BGQ::array_initialize(%p, %zu, \"%s\", %d, %p), __global._wuRegion_mms = %p\n", memory, bytes, key, attributes, cookie, __global._wuRegion_mms));
          PAMI_assert(__global._wuRegion_mms != NULL);

          size_t i, peers = (size_t) cookie;

          for (i = 0; i < peers; i++)
            {
              new (&wakeup[i]) BGQ (&__global._wuRegion_mms[i]);
            }

          TRACE_ERR((stderr, "<< Wakeup::BGQ::array_initialize(%p, %zu, \"%s\", %d, %p)\n", memory, bytes, key, attributes, cookie));
        };

        PAMI::Memory::GenMemoryManager * _mm;
        volatile size_t                * _poke;

    }; // class PAMI::Wakeup::BGQ
  }; // namespace Wakeup
}; // namespace PAMI

#endif // __components_device_shmem_wakeup_WakeupBGQ_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
