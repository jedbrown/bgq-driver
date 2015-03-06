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
 * \file common/socklinux/Mapping.h
 * \brief ???
 */

#ifndef __common_socklinux_Mapping_h__
#define __common_socklinux_Mapping_h__

#include "Platform.h"
#include "util/common.h"
#include "common/BaseMappingInterface.h"
#include "common/UdpSocketMappingInterface.h"
#include "common/SocketMappingInterface.h"
#include "common/NodeMappingInterface.h"
#include "components/memory/MemoryManager.h"
#include <pami.h>
#include <errno.h>
#include <unistd.h>
//#include <pmi.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>

#undef TRACE_COUTMAP
#ifndef TRACE_COUTMAP
#define TRACE_COUTMAP(x)//  std::cout << x << std::endl
#endif

#define PAMI_MAPPING_CLASS PAMI::Mapping

namespace PAMI
{
    class Mapping : public Interface::Mapping::Base<Mapping>,
                       public Interface::Mapping::UdpSocket<Mapping>,
                       public Interface::Mapping::Socket<Mapping>,
                       public Interface::Mapping::Node<Mapping, 1>
    {
    protected:
        typedef struct
        {
          char     host[128];
          unsigned peers;
          unsigned udpport;
          Interface::Mapping::nodeaddr_t addr;
        } node_table_t;

    public:
      inline Mapping () :
        Interface::Mapping::Base<Mapping>(),
        Interface::Mapping::UdpSocket<Mapping>(),
        Interface::Mapping::Socket<Mapping>(),
        Interface::Mapping::Node<Mapping, 1>()
      {
      };
      inline ~Mapping ()
      {
        PAMI::Memory::MemoryManager::heap_mm->free( _tcpConnTable );
        PAMI::Memory::MemoryManager::heap_mm->free( _udpConnTable );
      };

      inline bool loadMapFile()
      {
        char * udp_config;
        std::ifstream inFile;

        size_t i;
        size_t tmp_task;
        std::string tmp_host_in;
        //std::string tmp_port;
        size_t tmp_port;
//        int sockFd;
        int rc;
        struct addrinfo hints, *servinfo;
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_INET;  // AF_UNSPEC for 6 support too
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags |= AI_PASSIVE;



        udp_config = getenv ("PAMI_UDP_CONFIG");
        if (udp_config == NULL ) {
          std::cout << "Environment variable PAMI_UDP_CONFIG must be set" << std::endl;
          abort();
        }
        TRACE_COUTMAP("The current UDP configuration file is: " << udp_config);

        // Now open the configuration file
        inFile.open(udp_config);
        if (!inFile )
        {
          std::cout << "Unable to open UDP configuration file: " << udp_config << std::endl;
          abort();
        }

        // Allocate the node table - this contains the information from the map file
	pami_result_t ret;
	ret = PAMI::Memory::MemoryManager::heap_mm->memalign(
			(void **)&_nodetable, 0, _size * sizeof(*_nodetable));
	PAMI_assertf(ret == PAMI_SUCCESS, "Failed to alloc _nodetable");
        for (i=0; i<_size; i++)
        {
          _nodetable[i].host[0] = 0; // NULL
          _nodetable[i].peers   = 0;
        }
        size_t num_global_nodes = 0;

        // Read in the configuration file: rank host port
        for ( i=0; i<_size; i++ )
        {

          inFile >> tmp_task >> tmp_host_in >> tmp_port;
          TRACE_COUTMAP("  Entry: " << tmp_task << " " << tmp_host_in << " " << tmp_port);
          char tmp_port_str[128];
          snprintf(tmp_port_str, 127, "%zu", tmp_port);

          // Make sure we can locate the host
          struct hostent *tmp_host;
          tmp_host = gethostbyname(tmp_host_in.data());
          if (tmp_host == NULL )
          {
            std::cout << "Unable to get host by name.  Name in config file: " << tmp_host_in << std::endl;
            abort();
          }

          //if ( (rc = getaddrinfo( tmp_host_in.data(), tmp_port.data(), &hints, &servinfo ) ) != 0 )
          if ( (rc = getaddrinfo( tmp_host_in.data(), tmp_port_str, &hints, &servinfo ) ) != 0 )
          {
            std::cout << "getaddrinfo call failed:" << gai_strerror(rc) << std::endl;
          }
//          if ( (sockFd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol ) ) == -1 )
//          {
//            std::cout << "socket call failed" << std::endl;
//          }
          //TRACE_COUTMAP( "addr " << servinfo->ai_addr << " len  " << servinfo->ai_addrlen );
          //TRACE_COUTMAP( "ai_canonname " << servinfo->ai_canonname );

          unsigned j;
          bool found_previous = false;
          for (j=0; j<_size; j++)
          {
            if ( (j!=tmp_task) && strcmp( _nodetable[j].host, tmp_host->h_name ) == 0 )
            {
              // Found a previous host entry
              _nodetable[j].peers++;
              _nodetable[tmp_task].addr.global = _nodetable[j].addr.global;
              _nodetable[tmp_task].addr.local  = _nodetable[j].addr.local + 1;
              //_nodetable[tmp_task].udpport = tmp_port.data();
              _nodetable[tmp_task].udpport = tmp_port;
              _nodetable[tmp_task].peers = _nodetable[j].peers;
              strncpy (_nodetable[tmp_task].host, _nodetable[j].host, 127);
              found_previous = true;
            }
          }
          if (! found_previous)
          {
            // Did not find a previous host entry
            //num_global_nodes++;
            _nodetable[tmp_task].addr.global = num_global_nodes;
            _nodetable[tmp_task].addr.local  = 0;
            _nodetable[tmp_task].peers = 1;
            //_nodetable[tmp_task].udpport = tmp_port.data();
            _nodetable[tmp_task].udpport = tmp_port;
            strncpy (_nodetable[tmp_task].host, tmp_host->h_name, 127);
            num_global_nodes++;
          }
        }
        inFile.close();

        TRACE_COUTMAP("num_global_nodes: " << num_global_nodes);
        for (i=0; i<_size; i++)
        {
          TRACE_COUTMAP("node[" << i << "].host:    " << _nodetable[i].host);
          TRACE_COUTMAP("node[" << i << "].peers:   " << _nodetable[i].peers);
          TRACE_COUTMAP("node[" << i << "].udpport: " << _nodetable[i].udpport);
          TRACE_COUTMAP("node[" << i << "].addr:    {" << _nodetable[i].addr.global << ", " << _nodetable[i].addr.local << "}");

        }

        // Cache the number of peers for this task.
        _peers = _nodetable[_task].peers;

        return true;
      };



      inline bool isUdpActive()
      {
        return  _udpConnInit;
      }

      inline int activateUdp()
      {
//        char * udp_config;
//        std::ifstream inFile;
        size_t tmp_task;
        std::string tmp_host_in;
        std::string tmp_port;
        int sockFd;
        int rc;
        struct addrinfo hints, *servinfo;
        memset(&hints,0,sizeof(hints));
        hints.ai_family = AF_INET;  // AF_UNSPEC for 6 support too
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags |= AI_PASSIVE;

        _udpConnInit = true;
        // Allocate space for the connection table
	pami_result_t ret;
	ret = PAMI::Memory::MemoryManager::heap_mm->memalign(
			(void **)&_udpConnTable, 0, _size * sizeof(*_udpConnTable));
	PAMI_assertf(ret == PAMI_SUCCESS, "Failed to alloc _udpConnTable");

        size_t i;
        for ( i=0; i<_size; i++) {
          _udpConnTable[i].send_fd = 0;
        }

        // Read in the configuration file: rank host port
        for ( i=0; i<_size; i++ )
        {
          // Make sure we can locate the host
          struct hostent *tmp_host;
          tmp_host = gethostbyname(_nodetable[i].host);
          if (tmp_host == NULL )
          {
            std::cout << "Unable to get host by name.  Name in config file: " << _nodetable[i].host << std::endl;
            abort();
          }

          char tmp_port_str[128];
          snprintf(tmp_port_str, 127, "%d", _nodetable[i].udpport);

          if ( (rc = getaddrinfo( _nodetable[i].host, tmp_port_str, &hints, &servinfo ) ) != 0 )
          {
            std::cout << "getaddrinfo call failed:" << gai_strerror(rc) << std::endl;
          }
          if ( (sockFd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol ) ) == -1 )
          {
            std::cout << "socket call failed" << std::endl;
          }

          if (_task == i)
          {
            char host[128];
            int str_len=128;
            int err = gethostname( host, str_len );
            // TRACE_COUTMAP(host);
            if ( strcmp( host, tmp_host->h_name ) != 0 )
            {
              std::cout << "Entry for this node" << _task << " is incorrect: " << std::endl;
              std::cout << "     " << host << " does not match " << tmp_host->h_name << std::endl;
            }
            // Set up to receive
            _udpRcvConn = sockFd;
            if ( bind(_udpRcvConn, servinfo->ai_addr, servinfo->ai_addrlen ) == -1)
            {
              perror("bind");
              close (_udpRcvConn);
              std::cout << "bind call failed" << std::endl;
              abort();
            }

          } else {
            // Save info for sending
          _udpConnTable[i].send_fd =  sockFd;
          memcpy( &(_udpConnTable[i].send_addr), servinfo->ai_addr, servinfo->ai_addrlen );
          _udpConnTable[i].send_addr_len = servinfo->ai_addrlen;
          }
        }

        return 0;
      }

      inline int deactivateUdp()
      {
        size_t i;
        _udpConnInit = false;
        close( _udpRcvConn );
        for (i=0; i<_size; i++ )
        {
          close( _udpConnTable[i].send_fd );
        }
        return 0;
      }

      inline bool isTcpActive()
      {
        return  _tcpConnInit;
      }

      inline int activateTcp()
      {
        _tcpConnInit = true;
        // Allocate space for the connection table
	pami_result_t rc;
	rc = PAMI::Memory::MemoryManager::heap_mm->memalign(
			(void **)&_tcpConnTable, 0, _size * sizeof(*_tcpConnTable));
	PAMI_assertf(rc == PAMI_SUCCESS, "Failed to alloc *_tcpConnTable");

        // All are sockets entries
        size_t i;
        for ( i=0; i<_size; i++) {
          _tcpConnTable[i].network = PAMI_SOCKET_NETWORK;
        }
        std::cout << "TCP not supported yet " << std::endl;
        abort ();
        return 0;
      }

     inline int deactivateTcp()
        {
          _tcpConnInit = false;
          return 0;
        }

    protected:
        size_t            _task;
        size_t            _size;
        size_t            _peers;
        // static const int  __pmiNameLen = 128;
        // char              __pmiName[__pmiNameLen];
        node_table_t * _nodetable;

        typedef struct
        {
          int                            send_fd;
          sockaddr_storage               send_addr;
          int                            send_addr_len;
          Interface::Mapping::nodeaddr_t node_addr;
        } udp_conn_t;

        pami_coord_t *     _tcpConnTable;
        bool              _tcpConnInit;
        udp_conn_t *      _udpConnTable;
        int               _udpRcvConn;
        bool              _udpConnInit;


        static int rank_compare(const void *a, const void *b) {
                size_t *aa = (size_t *)a, *bb = (size_t *)b;
                size_t r1 = *aa;
                size_t r2 = *bb;
                int x = r1 - r2;
                return x;
        }
    public:
      inline pami_result_t init(size_t &min_rank, size_t &max_rank,
                                size_t &num_local, size_t **local_ranks)
      {
        char * tmp;
        tmp = getenv("PAMI_SOCK_SIZE");
        if (tmp == NULL ) {
          std::cout << "Environment variable PAMI_SOCK_SIZE must be set" << std::endl;
          abort();
        }
        _size = strtoul( tmp, NULL, 0 );
        TRACE_COUTMAP( "Size = " << _size );
        tmp = getenv("PAMI_SOCK_TASK");
        if (tmp == NULL ) {
          std::cout << "Environment variable PAMI_SOCK_TASK must be set" << std::endl;
          abort();
        }
        _task = strtoul( tmp, NULL, 0 );
        TRACE_COUTMAP( "Task = " << _task );
        if ( _task >= _size )
        {
          std::cout << "Task " << _task << "is >= size " << _size << std::endl;
          abort();
        }

        // None of the ranks are local
        //*local_ranks = xxxx;
        num_local = 0;

        min_rank = 0;
        max_rank = _size-1;

        loadMapFile ();

        return PAMI_SUCCESS;
      }

      inline size_t task_impl()
        {
          return _task;
        }

      inline size_t size_impl()
        {
          return _size;
        }

      inline pami_result_t task2network (size_t task, pami_coord_t *addr, pami_network type)
      {
        std::cout << "task2network not supported" << std::endl;
        abort();
      }

      inline pami_result_t network2task_impl(const pami_coord_t *addr,
                                                pami_task_t *task,
                                                pami_network *type)
      {
        std::cout << "network2task not supported" << std::endl;
        abort();
      }

      inline void udpAddr_impl(int & recvfd) const
      {
        recvfd = _udpRcvConn;
      }

     inline pami_result_t task2udp_impl( size_t task, int & sendfd, void * sockAddr, int & len )
     {
       if ( task >= _size )
       {
         return PAMI_ERROR;
       }
       sendfd = _udpConnTable[task].send_fd;
       len = _udpConnTable[task].send_addr_len;
       memcpy( sockAddr, &(_udpConnTable[task].send_addr), len );
       return PAMI_SUCCESS;
     }

     inline pami_result_t udp2task_impl( int sendfd, void * sockAddr, int len , size_t & task )
     {
       abort();
     }

     inline void socketAddr_impl (size_t & recv_fd, size_t & send_fd) { abort(); }

     inline pami_result_t task2socket_impl (size_t task, size_t & recv_fd, size_t & send_fd) { abort(); }

     inline pami_result_t socket2task_impl (size_t recv_fd, size_t send_fd, size_t & task) { abort(); }

     inline size_t globalDims()
     {
          return 1;
     }

     ///
     /// \brief Get the number of possible tasks on any node
     ///
     inline pami_result_t nodeTasks_impl (size_t global, size_t & tasks)
     {
       PAMI_abort();
       return PAMI_ERROR;
     };

     ///
     /// \brief Get the number of peer tasks on the local node
     /// \see PAMI::Interface::Mapping::Node::nodePeers()
     ///
     inline pami_result_t nodePeers_impl (size_t & peers)
     {
       TRACE_COUTMAP("Mapping::nodePeers_impl(), peers = " << _peers);
       peers = _peers;
       return PAMI_SUCCESS;
     }

     /// \see PAMI::Interface::Mapping::Node::isLocal()
     inline bool isLocal_impl (size_t task)
     {
       return isPeer (task, task_impl());
     }

     ///
     /// \brief Determines if two tasks are located on the same node
     ///
     inline bool isPeer_impl (size_t task1, size_t task2)
     {
       return (_nodetable[task1].addr.global == _nodetable[task2].addr.global);
     }

     ///
     /// \brief Node address for the local task
     ///
     /// \param[out] address Node address
     ///
     inline void nodeAddr_impl (Interface::Mapping::nodeaddr_t & address)
     {
       TRACE_COUTMAP("Mapping::nodeAddr_impl(), _task = " << _task << " _nodetable = " << _nodetable);
       address = _nodetable[_task].addr;
     }

     ///
     /// \brief Node address for a specific task
     ///
     /// The global task identifier monotonically increases from zero to
     /// PAMI::Mapping::Interface::Base.size() - 1.
     ///
     /// \param[in]  task    Global task identifier
     /// \param[out] address Node address
     ///
     inline pami_result_t task2node_impl (size_t task, Interface::Mapping::nodeaddr_t & address)
     {
       address = _nodetable[task].addr;
       return PAMI_SUCCESS;
     }

     ///
     /// \brief Global task identifier associated with a specific node address
     ///
     /// The global task identifier monotonically increases from zero to
     /// PAMI::Mapping::Interface::Base.size() - 1.
     ///
     /// \param[in]  address Node address
     /// \param[out] task    Global task identifier
     ///
     inline pami_result_t node2task_impl (Interface::Mapping::nodeaddr_t & address, size_t & task)
     {
       PAMI_abort();
       return PAMI_ERROR;
     }

     ///
     /// \brief Peer identifier associated with a specific node address
     ///
     /// The local peer identifier monotonically increases from zero to
     /// PAMI::Mapping::Interface::Node.nodePeers() - 1.
     ///
     /// \param[in]  address Node address
     /// \param[out] peer    peer identifier
     ///
     inline pami_result_t node2peer_impl (Interface::Mapping::nodeaddr_t & address, size_t & peer)
     {
       peer = address.local;
       return PAMI_SUCCESS;
     }

   }; // class Mapping
};  // namespace PAMI
#endif // __common_socklinux_Mapping_h__
