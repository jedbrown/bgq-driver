/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include "svc_host.h"
#include <firmware/include/mailbox.h>


void svc_pers_CopySwapCrc( Personality_t *in, Personality_t *out ) {

   int i;
   uint16_t tCrc;

   memcpy( out, in, sizeof(Personality_t) );

   out->CRC                  = 0;
   out->Version              = PERSONALITY_VERSION;
   out->PersonalitySizeWords = (uint8_t)(sizeof(Personality_t)/sizeof(uint32_t));

   if ( 0x01234567 != htonl(0x01234567) ) { // Little Endian host

      // Personality_Kernel_t
      out->Kernel_Config.UCI             = swap64( in->Kernel_Config.UCI         );
      out->Kernel_Config.NodeConfig      = swap64( in->Kernel_Config.NodeConfig  );
      out->Kernel_Config.TraceConfig     = swap64( in->Kernel_Config.TraceConfig );
      out->Kernel_Config.FreqMHz         = swap32( in->Kernel_Config.FreqMHz     );
      out->Kernel_Config.RASPolicy       = swap32( in->Kernel_Config.RASPolicy   );
      out->Kernel_Config.ClockStop       = swap64( in->Kernel_Config.ClockStop   );


      // Personality_DDR_t
      out->DDR_Config.DDRFlags           = swap32( in->DDR_Config.DDRFlags         );
      out->DDR_Config.DDRSizeMB          = swap32( in->DDR_Config.DDRSizeMB        );

      // Personality_Network_t
      out->Network_Config.BlockID         = swap32( in->Network_Config.BlockID    );
      out->Network_Config.NetFlags        = swap64( in->Network_Config.NetFlags   );
      out->Network_Config.NetFlags2       = swap64( in->Network_Config.NetFlags2  );
      // Anodes, Bnodes, ...,  and Acoords, Bcoords, ....  are single bytes

      out->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs     = swap16( in->Network_Config.PrimordialClassRoute.GlobIntUpPortInputs );
      out->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs    = swap16( in->Network_Config.PrimordialClassRoute.GlobIntUpPortOutputs );
      out->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs = swap16( in->Network_Config.PrimordialClassRoute.CollectiveTypeAndUpPortInputs );
      out->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs       = swap16( in->Network_Config.PrimordialClassRoute.CollectiveUpPortOutputs );
   

      for ( i = 0 ; i < sizeof(in->Network_Config.ZoneRoutingMasks) / sizeof(in->Network_Config.ZoneRoutingMasks[0]) ; i++ ) {
	out->Network_Config.ZoneRoutingMasks[i] = swap32( in->Network_Config.ZoneRoutingMasks[i]);
      }

      out->Network_Config.MuFlags = swap64( in->Network_Config.MuFlags   );
      
   }

   tCrc = _bgp_Crc16n( JMB_INITIAL_CRC,
                       &(out->Version),
                       (sizeof(Personality_t) - 2) );

   out->CRC = swap16( tCrc );
}

