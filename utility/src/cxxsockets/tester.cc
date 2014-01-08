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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
#include<iostream>
#include <string>
#include <tr1/tuple>
#include <sys/socket.h>
#include <sys/types.h>
// #include <unistd.h>
// #include <netinet/in.h>
#include <netdb.h>

using namespace std;

class A {
    int bar;
};


int main() {

    double d = 2.7; A a;
    std::tr1::tuple<int, double&, const A&> t(1, d, a);
    const std::tr1::tuple<int, double&, const A&> ct = t;
    //    int i = std::tr1::tuple::get<0>(t); i = t.std::tr1::tuple::get<0>();        // ok
    int j = std::tr1::get<0>(ct);                       // ok
//     get<0>(t) = 5;                            // ok 
 
    struct addrinfo hints;
    struct addrinfo *res;
    char *myservice = "30003";
    
    memset(&hints, 0, sizeof(hints));
    /* set-up hints structure */
    hints.ai_family = AF_INET6;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    int error = getaddrinfo(NULL, myservice, &hints, &res);
    if (error)
        perror(gai_strerror(error));
    else {
        /*
         * "res" has a chain of addrinfo structure filled with
         * 0.0.0.0 (for IPv4), 0:0:0:0:0:0:0:0 (for IPv6) and alike,
         * with port filled for "myservice".
         */
        int rescount = 0;
        while (res) {
            std::cout << "binding" << std::endl;
            ++rescount;
            int fd = socket(res->ai_addr->sa_family, SOCK_STREAM, 0);
            int reuse = 1;
            if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) 
                perror("setsockopt ");
            if(bind(fd, (sockaddr*)res->ai_addr,sizeof(struct sockaddr)) == -1)
                perror("bind ");
            if(listen(fd, 1))
                perror("listen ");
            res = res->ai_next;
            /* bind() and listen() to res->ai_addr */
        }
        std::cout << "rescount " << rescount << std::endl;
        sleep(60);
    }

//     struct addrinfo myai;
//     bzero(&myai, sizeof(myai));
//     myai.sa_family = AF_INET;
}
