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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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
#include "network.h"

int JOBID = 0;
char TOOLNAME[64] = "/bgsys/drivers/ppcfloor/coreprocessor/bin/sdebug_proxy";
map<string, int> clientIPAddresses;

int main(int argc, char** argv)
{
    int rc;
    int x;
    int rank;
    char ipaddress[64];
    char verb[256];
    char data[256];
    char pid[64];
    std::vector<CxxSockets::Message> messages;
    char cmd[256];
    
    setbuf(stdout, NULL);
    
    bgq::utility::Properties::Ptr props;
    if (!props) {
        props = bgq::utility::Properties::create();
    }
    for(x=1; x<argc; x++)
    {
#define ARG(name) if(strncmp(name"=",argv[x], strlen(name"="))==0)
        ARG("--id")
        {
            JOBID = atoi(strstr(argv[x], "=")+1);
        }
        ARG("--tool")
        {
            strcpy(TOOLNAME, strstr(argv[x], "=")+1);
        }
    }
    
    if(JOBID == 0)
    {
        printf("Must specify a jobid\n");
        exit(-1);
    }
    
    printf("Launching tool on job %d\n", JOBID);
    
    snprintf(cmd, sizeof(cmd), "/bgsys/drivers/ppcfloor/bin/dump_proctable --id=%d", JOBID);
    FILE* proctable = popen(cmd, "r");
    char line[256];
    while(1)
    {
        if(fgets(line, sizeof(line), proctable) == NULL)
            break;
        if(strncmp(line, "Rank ", 5) == 0)
            break;
    }
    
    number_of_clients = 0;
    while(1)
    {
        rc = fscanf(proctable, "%d %s %s", &rank, ipaddress, pid);
        if(rc != 3)
            break;
        
        string ipaddress_str(ipaddress);
        clientIPAddresses[ipaddress_str] = 1;
    }
    number_of_clients = clientIPAddresses.size();
    
    printf("Number of IO nodes = %d\n", number_of_clients);
    if(number_of_clients == 0)
        return 0;
    
    snprintf(cmd, sizeof(cmd), "/bgsys/drivers/ppcfloor/bin/start_tool --id %d --tool %s", JOBID, TOOLNAME);
    system(cmd);
    sleep(5);
    
    bgq::utility::initializeLogging(*props);
    CxxSockets::SockAddrList side_salist(AF_INET, "", "34543");
    CxxSockets::ListenerSetPtr side_listener(new CxxSockets::ListenerSet(side_salist));

    for(std::map<string,int>::iterator it = clientIPAddresses.begin(); it != clientIPAddresses.end(); it++)
    {
        bool retry = false;
        CxxSockets::TCPSocketPtr side_sock;
        do
        {
            try
            {
                printf("Connecting to ionode %s\n", it->first.c_str());
                CxxSockets::SockAddr remote(AF_INET, it->first, "34543");
#if USE_SECURE_CERTIFICATE
                side_sock = (CxxSockets::SecureTCPSocketPtr) new CxxSockets::SecureTCPSocket(remote.family(), 0, CxxSockets::SECURE, CxxSockets::CERTIFICATE);
                bgq::utility::ClientPortConfiguration port_config(0);
                port_config.setProperties(props, "");
                port_config.notifyComplete();
                side_sock->Connect(remote, port_config);
#else
                side_sock = (CxxSockets::TCPSocketPtr) new CxxSockets::TCPSocket(remote.family(), 0);
                side_sock->Connect(remote);
#endif
            }
            catch(CxxSockets::HardError& e)
            {
                printf("connection CxxSockets::HardError failed\n");
                retry = true;
                sleep(1);
            }
        }
        while(retry);
        
        CxxSockets::Message side_msg;
        side_sock->Receive(side_msg);
        assert(side_msg.str() == "READY");
        side_msg.str().clear();
        SocketVector.push_back(side_sock);
    }
    
    while(1)
    {
        printf("COMMAND:\n");
        rc = fscanf(stdin, "%s %s", verb, data);
        if(rc != 2)
        {
            exit(0);
        }
        
        printf("verb=%s   data=%s\n", verb, data);
        if((strcmp(verb, "quit")==0) || (strcmp(verb, "exit")==0))
        {
            sendCommand("terminate");
            MsgMap msgs;
            receiveReply(msgs);
            break;
        }
        else if(strcmp(verb, "halt") == 0)
        {
            sendCommand("halt");
            MsgMap msgs;
            receiveReply(msgs);
        }
        else if(strcmp(verb, "run") == 0)
        {
            sendCommand("run");
            MsgMap msgs;
            receiveReply(msgs);
        }
        else if(strcmp(verb, "iar") == 0)
        {
            sendCommand("iar");
            MsgMap msgs;
            receiveReply(msgs);
        }
        else if(strcmp(verb, "stacks") == 0)
        {
            sendCommand("stacks");
            MsgMap msgs;
            receiveReply(msgs);
        }
        else if(strcmp(verb, "gprs") == 0)
        {
            sendCommand("gprs");
            MsgMap msgs;
            receiveReply(msgs);
        }
        else
        {
            printf("unknown verb: %s\n", verb);
        }
    }
}
