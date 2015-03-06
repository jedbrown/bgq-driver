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
 * \file util/queue/MatchQueue.h
 * \brief ???
 */

#ifndef __util_queue_MatchQueue_h__
#define __util_queue_MatchQueue_h__

#include "util/queue/Queue.h"

#define NUMQS    16

namespace PAMI
{
  template <class T_KeyType = unsigned>
  class MatchQueueElem  : public Queue::Element
  {
    protected:
      T_KeyType               _key;       ///The key to insert and search elements

    public:
      ///Constructor
      MatchQueueElem (T_KeyType key) : Queue::Element (), _key(key)
      {
      }

      ///Get the key
      T_KeyType    key()
      {
        return _key;
      }
  };

  template <class T_KeyType = unsigned>
  class MatchQueue
  {
    protected:

      ///The queues to store keys and later search for them
      Queue                    _localQ[NUMQS];

    public:

      MatchQueue ()
      {
      }

      ///\brief Inserts an element at the head of the queue
      void pushHead (MatchQueueElem<T_KeyType>  * elem)
      {
        T_KeyType qid = (elem->key()) % NUMQS;

        _localQ[qid].pushHead (elem);
      }

      ///\brief Inserts an element at the head of the queue
      void pushTail (MatchQueueElem<T_KeyType>  * elem)
      {
        T_KeyType qid = (elem->key()) % NUMQS;

        _localQ[qid].pushTail (elem);
      }

      ///\brief Deletes the element at the queue
      void deleteElem (MatchQueueElem<T_KeyType>   * elem)
      {
        T_KeyType qid = (elem->key()) % NUMQS;

        _localQ[qid].deleteElem (elem);
      }

      ///\brief Find the first the element that has the key
      MatchQueueElem<T_KeyType>   *findAndDelete (T_KeyType key)
      {
        T_KeyType qid = key % NUMQS;
        Queue &queue = _localQ[qid];
        MatchQueueElem<T_KeyType> *head = (MatchQueueElem<T_KeyType> *) queue.peekHead();

        while (head != NULL)
          {
            if (head->key() == key)
              {
                ///Element was found in posted queue
                queue.deleteElem (head);
                return  head;
              }

            head =  (MatchQueueElem<T_KeyType> *) head->next();
          }

        return NULL;
      }

      MatchQueueElem<T_KeyType>   *find (T_KeyType key)
      {
        T_KeyType qid = key % NUMQS;
        PAMI::Queue &queue = _localQ[qid];
        MatchQueueElem<T_KeyType> *head = (MatchQueueElem<T_KeyType> *) queue.peekHead();

        while (head != NULL)
        {
          if (head->key() == key)
          {
            ///Element was found in posted queue
            return  head;
          }
          head =  (MatchQueueElem<T_KeyType> *) head->next();
        }
        return NULL;
      }

      size_t  size ()
      {
        size_t totalSize = 0;
        T_KeyType qid = 0;
        for(; qid < NUMQS; qid++)
        {
          totalSize += _localQ[qid].size();
        }
        return totalSize;
      }

  };  //- MatchQueue
};  //- PAMI

#endif // __util_queue_matchqueue_h__
