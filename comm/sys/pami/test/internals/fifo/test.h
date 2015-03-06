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
 * \file test/internals/fifo/test.h
 * \brief ???
 */
#ifndef __test_internals_fifo_test_h__
#define __test_internals_fifo_test_h__


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "Global.h"
#include "math/Memcpy.x.h"


#include "components/fifo/FifoInterface.h"
#include "components/fifo/PacketInterface.h"


class Consumer : public PAMI::Fifo::Interface::PacketConsumer <Consumer>
{
  public:

    friend class PAMI::Fifo::Interface::PacketConsumer <Consumer>;

    Consumer (const char * name = NULL) :
        _name (name)
    {
    };

  protected:

    template <class T_FifoPacket>
    inline bool consume_impl (T_FifoPacket & packet)
    {
      void * header  = packet.getHeader();
      size_t from = *((size_t *)header);

      fprintf (stdout, "[%s] - received a packet from %zu\n", _name, from);

      return true;
    };

    const char * _name;
};

class PingpongConsumer : public PAMI::Fifo::Interface::PacketConsumer <PingpongConsumer>
{
  public:

    friend class PAMI::Fifo::Interface::PacketConsumer <PingpongConsumer>;

    PingpongConsumer ()
    {
    };

  protected:

    template <class T_FifoPacket>
    inline bool consume_impl (T_FifoPacket & packet)
    {
      void * header  = packet.getHeader();
      size_t bytes = *((size_t *)header);

      uint8_t tmp[bytes];
      Core_memcpy (tmp, packet.getPayload(), bytes);

      return true;
    };

    const char * _name;
};

class Producer : public PAMI::Fifo::Interface::PacketProducer <Producer>
{


  public:

    friend class PAMI::Fifo::Interface::PacketProducer <Producer>;

    inline Producer () :
        _value ((size_t) - 1),
        _data (NULL),
        _bytes (0)
    {};

    inline void setValue (size_t value)
    {
      _value = value;
    }

    inline void setData (void * data, size_t bytes)
    {
      _data = data;
      _bytes = bytes;
    }

  protected:

    template <class T_Packet>
    inline bool produce_impl (T_Packet & packet)
    {
      void * header = packet.getHeader ();
      *((size_t *) header) = _value;

      memcpy (packet.getPayload(), _data, _bytes);

      return true;
    };

    template <class T_Packet>
    inline bool produce_impl (T_Packet & packet, bool & done)
    {
      produce_impl (packet);
      done = true;
      return true;
    };

    size_t _value;
    void * _data;
    size_t _bytes;

};


template <class T_Fifo>
class Test
{

  public:

    template <class T_MemoryManager>
    inline void init (T_MemoryManager * mm, size_t task, size_t size, const char * unique = "")
    {
      _task = task;
      _size = size;

      size_t fnum;

      for (fnum = 0; fnum < 64; fnum++)
        {
          char fifokey[1024];
          snprintf (fifokey, 1023, "/%s-fifo%zu", unique, fnum);
          _ififo[fnum].initialize (mm, fifokey, 64, fnum);
        }

      _rfifo.initialize (_ififo[_task]);
    };


    void functional (const char * name = NULL)
    {
      Producer producer;

      producer.setValue(_task);

      Consumer consumer(name);

      if (_task != 0)
        {
          while (! _ififo[0].producePacket(producer));

          //size_t sequence = _ififo[0].lastPacketProduced();
          //fprintf (stdout, "[%s] produced packet %zu into fifo 0\n", name, sequence);

          // bug!
          //while (ififo[0].lastPacketConsumed() < sequence);

          //fprintf (stdout, "packet %zu was consumed\n", sequence);
        }
      else
        {
          size_t expected = _size - 1;

          while (expected > 0)
            {
              if (_rfifo.consumePacket(consumer))
                {
                  //fprintf (stdout, "[%s] consumed packet %zu\n", name, _rfifo.lastPacketConsumed());
                  expected--;
                }
            }
        }
    };


    unsigned long long pingpong (size_t bytes, size_t iterations = 1, const char * name = NULL)
    {
      uint64_t sndbuf[1024];

      Producer producer;
      producer.setValue (bytes);
      producer.setData (sndbuf, bytes);

      PingpongConsumer consumer;

      unsigned i, warmup;
      unsigned long long elapsed = 0;

      for (warmup = 0; warmup < 2; warmup++)
        {
          if (_task == 0) // "ping-er"
            {
              unsigned long long t0 = __global.time.timebase();

              for (i = 0; i < iterations; i++)
                {

                  bool done = false;

                  do
                    {
                      done = _ififo[1].producePacket(producer);
                    }
                  while (!done);

                  done = false;

                  do
                    {
                      done = _rfifo.consumePacket(consumer);
                    }
                  while (!done);
                }

              elapsed = __global.time.timebase() - t0;
            }
          else if (_task == 1) // "pong-er"
            {
              for (i = 0; i < iterations; i++)
                {
                  bool done = false;

                  do
                    {
                      done = _rfifo.consumePacket(consumer);
                    }
                  while (!done);

                  done = false;

                  do
                    {
                      done = _ififo[0].producePacket(producer);
                    }
                  while (!done);
                }
            }
        }

      return elapsed;
    }

  protected:

    T_Fifo _ififo[64];
    T_Fifo _rfifo;
    size_t _task;
    size_t _size;
};

#endif
