/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2011, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <sstream>
#include <iostream>
#include <cassert>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
  if ( argc < 3 ) {
      std::cerr << "usage: listen_both46 host port [stream|dgram]" << std::endl;
      exit( EXIT_FAILURE );
  }

  std::cout << "Tries to get address info for both ipv4 and ipv6 addresses for a specified host/port pair" << std::endl;

  char* port = argv[2]; // port
  char* node = argv[1];
  char* dgram_stream = argv[3];
  struct addrinfo hints;
  struct addrinfo* addrinf;
  bzero(&hints, sizeof(addrinfo));

  hints.ai_family = AF_UNSPEC;
  if(dgram_stream != NULL) {
      if(!strcmp(dgram_stream,"stream")) {
          hints.ai_socktype = SOCK_STREAM;
      } else 
          hints.ai_socktype = SOCK_DGRAM;
  }

  int retval = getaddrinfo(node, port, &hints, &addrinf);
  if(retval != 0) {
      std::ostringstream msg;
      msg << "getaddrinfo failed to get address info: " << gai_strerror(retval) << " for addr " << node;
      msg << "errno " << errno;
      std::cout << msg.str() << std::endl;
      exit(EXIT_FAILURE);
  }

  if(!addrinf) {
      std::ostringstream msg;
      msg << "failed to get address info: " << gai_strerror(retval) << " for addr " << node;
      std::cout << msg.str() << std::endl;
      exit(EXIT_FAILURE);
  }
   
  int addrcount = 0;
  while(addrinf) {
      ++addrcount;
      char host_buf[NI_MAXHOST];
      int size = 0;
      if(addrinf->ai_family == AF_INET)
          size = sizeof(sockaddr_in);
      else
          size = sizeof(sockaddr_in6);

      int error = getnameinfo(addrinf->ai_addr, size, host_buf, sizeof(host_buf), 0,0, NI_NUMERICHOST);
      assert(!error);
      std::cout << "addr=" << host_buf << std::endl;
      addrinf = addrinf->ai_next;
  }
  std::cout << "got " << addrcount << " addrinfs" << std::endl;

  exit(0);
}

