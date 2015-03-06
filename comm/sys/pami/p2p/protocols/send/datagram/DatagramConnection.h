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
/// \file p2p/protocols/send/datagram/DatagramConnection.h
/// \brief Connection class for reliable, deterministic datagram protocols
///
#ifndef __p2p_protocols_send_datagram_DatagramConnection_h__
#define __p2p_protocols_send_datagram_DatagramConnection_h__

#include <stdlib.h>

#ifndef TRACE_ERR
#define TRACE_ERR(x) //fprintf x
#endif

namespace PAMI
{
  namespace Protocol
  {
    namespace Send
    {
      template <class T_Device, unsigned T_Size = 100>
      class DatagramConnection
      {
        private:
        struct datagram_connection_t
          {
            public:
            pami_client_t     client;
            size_t          context;
            void           ** array;
          };

        public:
          DatagramConnection (T_Device & device) :
            _device (device)
          {
          };

          ~DatagramConnection () {}

          /// \todo Do this in a threadsafe way
          inline void ** getConnectionArray ( pami_client_t     client, size_t context)
          {
            void ** connection = NULL;

            unsigned i;
            unsigned first_empty_connection = 0;
            bool found_empty_connection = false;
            bool found_previous_connection = false;
            for (i=0; i<T_Size; i++)
            {
              if (_datagram_connection[i].array == NULL)
              {
                if (!found_empty_connection)
                {
                  first_empty_connection = i;
                  found_empty_connection = true;
                }
              }
              else if (_datagram_connection[i].context == context)
              {
                found_previous_connection = true;
                connection = _datagram_connection[i].array;
              }
            }

            if (!found_previous_connection)
            {
              if (!found_empty_connection)
              {
                return NULL;
              }
                          _datagram_connection[first_empty_connection].client = client;
              _datagram_connection[first_empty_connection].context = context;
              _datagram_connection[first_empty_connection].array =
                (void **) calloc (_device.peers(), sizeof(void *));
              connection = _datagram_connection[first_empty_connection].array;
            }

            return connection;
          }

        private:

          T_Device & _device;
          static datagram_connection_t _datagram_connection[T_Size];
      };

      template <class T_Device, unsigned T_Size>
      struct DatagramConnection<T_Device,T_Size>::datagram_connection_t DatagramConnection<T_Device,T_Size>::_datagram_connection[T_Size];
    };
  };
};



#undef TRACE_ERR
#endif // __pami_p2p_protocol_send_datagram_datagramconnection_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
