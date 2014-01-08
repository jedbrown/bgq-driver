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
string MyIPAddress;
map<string, int> clientIPAddresses;
void findIPs(std::vector<string>& IPs);

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
    std::vector<string> myips;
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
    
    findIPs(myips);
    
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
    while(1)
    {
        rc = fscanf(proctable, "%d %s %s", &rank, ipaddress, pid);
        if(rc != 3)
            break;
        if(rank == 0)
        {
            int class_a, class_b, ipa, ipb;
            sscanf(ipaddress, "%d.%d", &class_a, &class_b);
            for(vector<string>::iterator it = myips.begin(); it != myips.end(); it++)
            {
                sscanf(it->c_str(), "%d.%d", &ipa, &ipb);
                if((ipa == class_a)&&(ipb == class_b))
                    MyIPAddress = *it;
            }
        }
        
        string ipaddress_str(ipaddress);
        clientIPAddresses[ipaddress_str]++;
    }

    if ( MyIPAddress.empty() ) {
        printf("Could not find IP address of functional network!\n");
        exit(-1);
    }
    
    number_of_clients = clientIPAddresses.size();
    printf("Number of IO nodes = %d\n", number_of_clients);
    printf("IP address of the functional network is %s\n", MyIPAddress.c_str());
    if(number_of_clients == 0)
        return 0;
    
    snprintf(cmd, sizeof(cmd), "/bgsys/drivers/ppcfloor/bin/start_tool --id %d --tool %s --args %s", JOBID, TOOLNAME, MyIPAddress.c_str());
    system(cmd);
    
    bgq::utility::initializeLogging(*props);
    CxxSockets::SockAddrList side_salist(AF_INET, "", "34543");
    CxxSockets::ListenerSetPtr side_listener(new CxxSockets::ListenerSet(side_salist));

    for(x=0; x<number_of_clients; x++)
    {
#if USE_SECURE_CERTIFICATE
        CxxSockets::SecureTCPSocketPtr sock(new CxxSockets::SecureTCPSocket());
        bgq::utility::ServerPortConfiguration port_config(0);
        port_config.setProperties(props, "");
        port_config.notifyComplete();
        side_listener->AcceptNew(sock, port_config);
#else
        CxxSockets::TCPSocketPtr sock(new CxxSockets::TCPSocket());
        side_listener->AcceptNew(sock);
#endif        
        {
            CxxSockets::Message side_msg("READY");
            sock->Send(side_msg);
            side_msg.str().clear();
            SocketVector.push_back(sock);
        }
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
