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
/// \file components/devices/shmem/wakeup/WakeupSemaphore.h
/// \brief Semaphore implementation of the wakeup interface
///
/// \warning The maximum number of peers supported with this semaphore
///          implementation depends on the size of an \c unsigned \c short -
///          which is usually 64k.
///
/// \todo Implement a multi-job version. Currently multiple jobs running on the
///       same node will use the same semaphore key value.
///
#ifndef __components_devices_shmem_wakeup_WakeupSemaphore_h__
#define __components_devices_shmem_wakeup_WakeupSemaphore_h__

#include "components/devices/shmem/wakeup/WakeupInterface.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <limits.h>

namespace PAMI
{
  namespace Wakeup
  {
    ///
    /// \brief Semaphore implementation of the wakeup interface
    ///
    /// \tparam T_Key Unique key used to create the semaphore set.
    ///
    template < unsigned T_Key = 0x5ca1ab1e >
    class Semaphore : public Wakeup::Interface < Semaphore<T_Key> >
    {
      public:

        friend class Wakeup::Interface <Semaphore<T_Key> >;

        template <typename T>
        class Region : public Wakeup::Interface<Semaphore<T_Key> >::template Region<Region <T> >
        {
          public:

            //friend class Wakeup::Interface<Semaphore<T_Key> >::template Region<Region <T> >;

            class Element
            {
              public:

                Element (Semaphore * wakeup) :
                    value (0),
                    wakeup (*wakeup)
                {};

                operator bool() const
                {
                  return (bool) this->value;
                };

                inline Element & operator= (size_t rhs)
                {
                  this->value = rhs;
                  this->wakeup.poke();
                  return (*this);
                };

                inline Element & operator= (const Element & rhs)
                {
                  this->value = rhs.value;
                  this->wakeup.poke();
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

                T           value;
                Semaphore & wakeup;
            };

            inline void init_impl (Region & original)
            {
              _array = original._array;
            };

            template <class T_MemoryManager>
            inline void init_impl (T_MemoryManager * mm,
                                   char            * key,
                                   size_t            count,
                                   Semaphore       * wakeup)
            {
              pami_result_t mmrc = PAMI_ERROR;

              mmrc = mm->memalign ((void **) & _array,
                                   16,                                  // align
                                   count * sizeof(Element),             // bytes
                                   key,                                 // unique
                                   Semaphore<T_Key>::Region<T>::array_initialize,
                                   (void *)wakeup);                     // cookie

              PAMI_assertf(mmrc == PAMI_SUCCESS, "memalign failed for semaphore wakeup resources, rc=%d\n", mmrc);
            };

            inline Element & operator[] (size_t position)
            {
              return _array[position];
            };

            inline size_t operator[] (size_t position) const
            {
              return _array[position].value;
            }

          private:

            static void array_initialize (void       * memory,
                                          size_t       bytes,
                                          const char * key,
                                          unsigned     attributes,
                                          void       * cookie)
            {
              Semaphore * wakeup = (Semaphore *) cookie;
              Element * array = (Element *) memory;

              size_t i, count = bytes / sizeof(Element);

              for (i = 0; i < count; i++)
                {
                  new (&array[i]) Element (wakeup);
                }
            };

            Element * _array;
        };


        inline Semaphore (Semaphore & clone) :
            Wakeup::Interface <Semaphore> (clone),
            _semid (clone._semid),
            _thispeer (clone._thispeer)
        {
        };

        inline ~Semaphore () {};

      protected:

        inline Semaphore (unsigned short thispeer, int semid) :
            Wakeup::Interface <Semaphore> (),
            _semid (semid),
            _thispeer (thispeer)
        {
        };

        inline void init_impl (Semaphore & clone)
        {
          _semid = clone._semid;
          _thispeer = clone._thispeer;
        }

        ///
        /// \see PAMI::Interface::Wakeup::generate()
        ///
        static Semaphore * generate_impl (size_t peers, const char * key)
        {
          PAMI_assert_debug(peers <= USHRT_MAX);

          Semaphore * wakeup = (Semaphore *) malloc (sizeof(Semaphore) * peers);

          int semid;
          key_t semkey = (key_t) T_Key;

          if ((semid = semget (semkey, peers, 0)) == -1)
            {
              // Semaphore does not exist - Create.
              if ((semid = semget (semkey, peers, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) != -1)
                {
                  size_t n;
                  unsigned short value[peers];

                  for (n = 0; n < peers; n++) value[n] = 0;

                  int err = semctl (semid, 0, SETALL, value);

                  if (err < 0)
                    {
                      return NULL;
                    }
                }
              else if (errno == EEXIST)
                {
                  if ((semid = semget(semkey, peers, 0)) == -1)
                    {
                      perror ("IPC error 1: semget");
                      exit(1);
                    }
                }
              else
                {
                  perror ("IPC error 2: semget");
                  exit(1);
                }
            }

          unsigned short i;

          for (i = 0; i < peers; i++)
            {
              new (&wakeup[i]) Semaphore (i, semid);
            }

          return wakeup;
        };

        ///
        /// \see PAMI::Wakeup::Interface::poke()
        ///
        inline pami_result_t poke_impl ()
        {
          int value = 0;
          int err = semctl (_semid, _thispeer, SETVAL, value);

          if (err < 0)
            {
              perror ("IPC status 1: semctl");
              return PAMI_ERROR;
            }

          return PAMI_SUCCESS;
        };
#if 0
        ///
        /// \see PAMI::Interface::Wakeup::sleep()
        ///
        template <class T_Prepare>
        inline pami_result_t sleep_impl (T_Prepare & prepare)
        {
          prepare();

          struct sembuf op[2];

          op[0].sem_num = _thispeer;
          op[0].sem_op  = 1;	// "up" the semaphore
          op[0].sem_flg = 0;

          op[1].sem_num = _thispeer;
          op[1].sem_op  = 0;	// "wait on zero"
          op[1].sem_flg = 0;

          int err = semop (_semid,  &op[0], 1);

          if (err < 0)
            {
              perror("after semop 1");
              return PAMI_ERROR;
            }

          err = semop (_semid, &op[1], 1);

          if (err < 0)
            {
              perror("after semop 2");
              return PAMI_ERROR;
            }

          return PAMI_SUCCESS;
        };
#endif
        int            _semid;
        unsigned short _thispeer;

    }; // class PAMI::Wakeup::Semaphore
  }; // namespace Wakeup
}; // namespace PAMI

#endif // __components_device_shmem_wakeup_WakeupSemaphore_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
