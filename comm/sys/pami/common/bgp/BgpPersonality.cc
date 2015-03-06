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
 * \file common/bgp/BgpPersonality.cc
 * \brief ???
 */

#include "BgpPersonality.h"

PAMI::BgpPersonality::BgpPersonality ()
{
  _BGP_Personality_t * p = (_BGP_Personality_t *) this;
  int err = Kernel_GetPersonality(p, sizeof(_BGP_Personality_t));
  if (err != 0) abort();

  _x = BGP_Personality_xCoord(p);
  _y = BGP_Personality_yCoord(p);
  _z = BGP_Personality_zCoord(p);

  _Xnodes = BGP_Personality_xSize(p);
  _Ynodes = BGP_Personality_ySize(p);
  _Znodes = BGP_Personality_zSize(p);

  _numpset  = BGP_Personality_psetNum(p);
  _sizepset = BGP_Personality_psetSize(p);
  _rankpset = BGP_Personality_rankInPset(p);

  switch (BGP_Personality_processConfig(p))
    {
      case _BGP_PERS_PROCESSCONFIG_SMP:
        {
          _Tnodes = 1;
          _maxThreads = 4;
          _t = 0;
          break;
        }
      case _BGP_PERS_PROCESSCONFIG_VNM:
        {
          _Tnodes = 4;
          _maxThreads = 1;                   // 1 thread per virtual node
          _t = Kernel_PhysicalProcessorID(); // T is either 0, 1, 2, or 3
          break;
        }
      case _BGP_PERS_PROCESSCONFIG_2x2:
        {
          _Tnodes = 2;
          _maxThreads = 2;                       // 2 threads per virtual node
          _t = Kernel_PhysicalProcessorID() / 2; // Processor ID is either 0 or 2
          // T is either 0 or 1
          break;
        }
    }

  _isTorusX = BGP_Personality_isTorusX(p);
  _isTorusY = BGP_Personality_isTorusY(p);
  _isTorusZ = BGP_Personality_isTorusZ(p);
  _isTorus  = (_isTorusX && _isTorusY && _isTorusZ);

  _memSize  = BGP_Personality_DDRSizeMB(p);
  _clockMHz = BGP_Personality_clockMHz(p);

  _isHTCmode = (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_HighThroughput);
};

void PAMI::BgpPersonality::location (char location[], size_t size)
{
  _BGP_Personality_t * p = (_BGP_Personality_t *) this;
  char tmp[BGPPERSONALITY_MAX_LOCATION+1];
  BGP_Personality_getLocationString(p, tmp);

  snprintf (location, MIN(size,BGPPERSONALITY_MAX_LOCATION), " %s", tmp);

  return;
};

void PAMI::BgpPersonality::dumpPersonality ()
{
  _BGP_Personality_t * p = (_BGP_Personality_t *) this;
  int i, tmp;

  printf("Personality: CRC 0x %04x, Version %d, Size %d.\n",
         p->CRC, p->Version, p->PersonalitySizeWords );

  printf(" BlockID:         0x %08x\n", p->Network_Config.BlockID );
  printf(" RASPolicy:       0x %08x\n", p->Kernel_Config.RASPolicy );
  printf(" ProcessConfig:   0x %08x\n", p->Kernel_Config.ProcessConfig );
  printf(" TraceConfig:     0x %08x\n", p->Kernel_Config.TraceConfig );
  printf(" NodeConfig:      0x %08x\n", p->Kernel_Config.NodeConfig );
  printf(" L1Config:        0x %08x\n", p->Kernel_Config.L1Config );
  printf(" L2Config:        0x %08x\n", p->Kernel_Config.L2Config );
  printf(" L3Config:        0x %08x\n", p->Kernel_Config.L3Config );
  printf(" L3Select:        0x %08x\n", p->Kernel_Config.L3Select );

  printf(" FreqMHz:         %d\n",      p->Kernel_Config.FreqMHz );

  printf(" Torus:         ( %2d, %2d, %2d) in ( %2d %c, %2d %c, %2d %c)\n",
         p->Network_Config.Xcoord,
         p->Network_Config.Ycoord,
         p->Network_Config.Zcoord,
         p->Network_Config.Xnodes,
         (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_TorusMeshX?'M':'T'),
         p->Network_Config.Ynodes,
         (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_TorusMeshY?'M':'T'),
         p->Network_Config.Znodes,
         (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_TorusMeshZ?'M':'T'));

  printf(" IOnodes:         %d\n", p->Network_Config.IOnodes );
  printf(" Rank:         %d\n", p->Network_Config.Rank );
  for ( i = 0, tmp = 0 ; i < 16 ; i++ )
    {
      if ( p->Network_Config.TreeRoutes[i] )
        {
          tmp++;
          printf(" TreeRoutes[%d]: 0x %04x\n",
                 i, p->Network_Config.TreeRoutes[i] );
        }
    }
  if ( !tmp )
    printf(" TreeRoutes:           (none defined)\n" );


  printf(" DDRSizeMB:         %d\n",      p->DDR_Config.DDRSizeMB   );
  printf(" DDRChips:        0x %02x\n", p->DDR_Config.Chips    );
  printf(" DDRCAS:         %d\n",      p->DDR_Config.CAS      );
  printf(" DDRThrottle:         %d %% \n",    p->DDR_Config.Throttle );
  printf(" MTU:         %d\n", p->Ethernet_Config.MTU );

  printf(" EmacID:         %02x:%02x:%02x:%02x:%02x:%02x\n",
         p->Ethernet_Config.EmacID[0],
         p->Ethernet_Config.EmacID[1],
         p->Ethernet_Config.EmacID[2],
         p->Ethernet_Config.EmacID[3],
         p->Ethernet_Config.EmacID[4],
         p->Ethernet_Config.EmacID[5] );

  printf(" IPAddress:         %d.%d.%d.%d\n",
         p->Ethernet_Config.IPAddress.octet[12],
         p->Ethernet_Config.IPAddress.octet[13],
         p->Ethernet_Config.IPAddress.octet[14],
         p->Ethernet_Config.IPAddress.octet[15]  );

  printf(" IPNetmask:         %d.%d.%d.%d\n",
         p->Ethernet_Config.IPNetmask.octet[12],
         p->Ethernet_Config.IPNetmask.octet[13],
         p->Ethernet_Config.IPNetmask.octet[14],
         p->Ethernet_Config.IPNetmask.octet[15]  );

  printf(" IPBroadcast:       %d.%d.%d.%d\n",
         p->Ethernet_Config.IPBroadcast.octet[12],
         p->Ethernet_Config.IPBroadcast.octet[13],
         p->Ethernet_Config.IPBroadcast.octet[14],
         p->Ethernet_Config.IPBroadcast.octet[15]  );

  printf(" IPGateway:         %d.%d.%d.% d\n",
         p->Ethernet_Config.IPGateway.octet[12],
         p->Ethernet_Config.IPGateway.octet[13],
         p->Ethernet_Config.IPGateway.octet[14],
         p->Ethernet_Config.IPGateway.octet[15]  );

  printf(" NFSServer:         %d.%d.%d.%d\n",
         p->Ethernet_Config.NFSServer.octet[12],
         p->Ethernet_Config.NFSServer.octet[13],
         p->Ethernet_Config.NFSServer.octet[14],
         p->Ethernet_Config.NFSServer.octet[15]  );

  printf(" serviceNode:       %d.%d.%d.%d\n",
         p->Ethernet_Config.serviceNode.octet[12],
         p->Ethernet_Config.serviceNode.octet[13],
         p->Ethernet_Config.serviceNode.octet[14],
         p->Ethernet_Config.serviceNode.octet[15]  );

  printf(" NFSExportDir:         > %s < \n", p->Ethernet_Config.NFSExportDir );
  printf(" NFSMountDir:          > %s < \n", p->Ethernet_Config.NFSMountDir  );

  return;
};
