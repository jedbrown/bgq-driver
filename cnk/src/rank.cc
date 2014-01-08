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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#include "Kernel.h"
#include "rank.h"
#include <fcntl.h>
#include <hwi/include/bqc/nd_500_dcr.h>
#include <hwi/include/bqc/mu_dcr.h>
#include <spi/include/mu/GIBarrier.h>
#include <ramdisk/include/services/JobctlMessages.h>
#include "ctype.h"

#define open internal_open
#define read internal_read
#define close internal_close
#define strtoull strtoull_
#include <spi/include/mu/RankMap.h>

MUSPI_GIBarrier_t systemBlockGIBarrier;
MUSPI_GIBarrier_t systemJobGIBarrier;


// Perform a Barrier Using the System GI Barrier.
//
// \param[in]  parm  Pointer to a data structure that has been initialized
//                   by MUSPI_GIBarrierInit() to use the system GI barrier.
//
int32_t systemGIBarrierFn ( void *parm )
{
  int32_t rc;
  MUSPI_GIBarrier_t *systemGIBarrier = (MUSPI_GIBarrier_t *)parm;

  rc = MUSPI_GIBarrierEnterAndWait ( systemGIBarrier );

  return (rc);
}

// Configure a blockwide GI Barrier()
// 
int configureBlockGIBarrier()
{
  int32_t rc;
  // Setup a data structure to access the system GI barrier.
  rc = MUSPI_GIBarrierInit ( &systemBlockGIBarrier,
			     FW_GI_BARRIER_BOOT_CLASS_ROUTE );
  return (rc);
}

int deconfigureUserGI()
{
    uint64_t value;
    value  = DCRReadPriv(ND_500_DCR(CTRL_GI_CLASS_00_01));
    value &= (ND_500_DCR__CTRL_GI_CLASS_00_01__CLASS00_UP_PORT_I_set(~1) |
              ND_500_DCR__CTRL_GI_CLASS_00_01__CLASS00_UP_PORT_O_set(~0));
    DCRWritePriv(ND_500_DCR(CTRL_GI_CLASS_00_01), value);
    return 0;
}

int setNDSenderEnable(int link, int newvalue)
{
    uint64_t nd_ctrl = DCRReadPriv(ND_RESE_DCR(link, CTRL));
    ND_RESE_DCR__CTRL__SE_DROP_PKT_USR_insert(nd_ctrl, !newvalue);
    DCRWritePriv(ND_RESE_DCR(link, CTRL),  nd_ctrl);               // drop user packets on ND senders
    return 0;
}

int configureJobClassroutes(struct bgcios::jobctl::SetupJobMessage* jobinfo)
{
    uint32_t x;
    Personality_t* pers = GetPersonality();
    uint64_t value;
    uint64_t sysbarrier;
    uint64_t* controlRegPtr;
    if((Personality_IsFPGA() == 0) && ((NodeState.Personality.Kernel_Config.NodeConfig & PERS_ENABLE_MU) != 0))
    {   
#if CONFIG_ND_PRIVACY_FENCE
        // Setup subblock privacy fence:
        // handle "minus" links.  If corner_coord = mycoord and !torus, then we're on the edge of the mesh - - disable the link.  Otherwise, sender should be enabled.  
#define ENABLEND(coord, mycoord, maxjobcoord, maxcoord) (((coord == mycoord) && (maxjobcoord != maxcoord))?0:1)
        setNDSenderEnable(0, ENABLEND(jobinfo->corner.aCoord, pers->Network_Config.Acoord, jobinfo->shape.aCoord, pers->Network_Config.Anodes));
        setNDSenderEnable(2, ENABLEND(jobinfo->corner.bCoord, pers->Network_Config.Bcoord, jobinfo->shape.bCoord, pers->Network_Config.Bnodes));
        setNDSenderEnable(4, ENABLEND(jobinfo->corner.cCoord, pers->Network_Config.Ccoord, jobinfo->shape.cCoord, pers->Network_Config.Cnodes));
        setNDSenderEnable(6, ENABLEND(jobinfo->corner.dCoord, pers->Network_Config.Dcoord, jobinfo->shape.dCoord, pers->Network_Config.Dnodes));
        setNDSenderEnable(8, ENABLEND(jobinfo->corner.eCoord, pers->Network_Config.Ecoord, jobinfo->shape.eCoord, pers->Network_Config.Enodes));
#undef ENABLEND
        
        // handle "plus" links.  If corner_coord + size_coord -1 = mycoord and !torus, then we're on the edge of the mesh - - disable the link.  Otherwise, sender should be enabled.  
#define ENABLEND(coord, mycoord, maxjobcoord, maxcoord) ((((coord + maxjobcoord - 1) == mycoord) && (maxjobcoord != maxcoord))?0:1)
        setNDSenderEnable(1, ENABLEND(jobinfo->corner.aCoord, pers->Network_Config.Acoord, jobinfo->shape.aCoord, pers->Network_Config.Anodes));
        setNDSenderEnable(3, ENABLEND(jobinfo->corner.bCoord, pers->Network_Config.Bcoord, jobinfo->shape.bCoord, pers->Network_Config.Bnodes));
        setNDSenderEnable(5, ENABLEND(jobinfo->corner.cCoord, pers->Network_Config.Ccoord, jobinfo->shape.cCoord, pers->Network_Config.Cnodes));
        setNDSenderEnable(7, ENABLEND(jobinfo->corner.dCoord, pers->Network_Config.Dcoord, jobinfo->shape.dCoord, pers->Network_Config.Dnodes));
        setNDSenderEnable(9, ENABLEND(jobinfo->corner.eCoord, pers->Network_Config.Ecoord, jobinfo->shape.eCoord, pers->Network_Config.Enodes));
#undef ENABLEND
#endif
        
        for(x=0; x<jobinfo->numNodes; x++)
        {

            if( (jobinfo->nodesInJob[x].coords.aCoord == pers->Network_Config.Acoord) && 
                (jobinfo->nodesInJob[x].coords.bCoord == pers->Network_Config.Bcoord) &&
                (jobinfo->nodesInJob[x].coords.cCoord == pers->Network_Config.Ccoord) &&
                (jobinfo->nodesInJob[x].coords.dCoord == pers->Network_Config.Dcoord) &&
                (jobinfo->nodesInJob[x].coords.eCoord == pers->Network_Config.Ecoord))
            {
                
                // Set user global interrupt classroute
                value =
                    ND_500_DCR__CTRL_GI_CLASS_00_01__CLASS00_UP_PORT_I_set(jobinfo->nodesInJob[x].subBlockClassRoute.input) |
                    ND_500_DCR__CTRL_GI_CLASS_00_01__CLASS00_UP_PORT_O_set(jobinfo->nodesInJob[x].subBlockClassRoute.output);
                DCRWritePriv(ND_500_DCR(CTRL_GI_CLASS_00_01), value);
                
                // Set user collective classroute
                value =
                    ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS00_TYPE_set(2) |  // user subcomm type
                    ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS00_UP_PORT_I_set(jobinfo->nodesInJob[x].npClassRoute.input) | 
                    ND_500_DCR__CTRL_COLL_CLASS_00_01__CLASS00_UP_PORT_O_set(jobinfo->nodesInJob[x].npClassRoute.output);
                DCRWritePriv(ND_500_DCR(CTRL_COLL_CLASS_00_01), value);
                
                // Set system job-wide global interrupt classroute
                value = DCRReadPriv(ND_500_DCR(CTRL_GI_CLASS_14_15));
                ND_500_DCR__CTRL_GI_CLASS_14_15__CLASS14_UP_PORT_I_insert(value, jobinfo->nodesInJob[x].subBlockClassRoute.input);
                ND_500_DCR__CTRL_GI_CLASS_14_15__CLASS14_UP_PORT_O_insert(value, jobinfo->nodesInJob[x].subBlockClassRoute.output);
                DCRWritePriv(ND_500_DCR(CTRL_GI_CLASS_14_15), value);
                
                // Set system job-wide collective classroute
                value = DCRReadPriv(ND_500_DCR(CTRL_COLL_CLASS_14_15));
                ND_500_DCR__CTRL_COLL_CLASS_14_15__CLASS14_TYPE_insert(value, 1);  // system type
                ND_500_DCR__CTRL_COLL_CLASS_14_15__CLASS14_UP_PORT_I_insert(value, jobinfo->nodesInJob[x].subBlockClassRoute.input);
                ND_500_DCR__CTRL_COLL_CLASS_14_15__CLASS14_UP_PORT_O_insert(value, jobinfo->nodesInJob[x].subBlockClassRoute.output);
                DCRWritePriv(ND_500_DCR(CTRL_COLL_CLASS_14_15), value);
                
                // Make barrier classroute 0 available to user space
                value = DCRReadPriv(MU_DCR(SYS_BARRIER));
                sysbarrier  = MU_DCR__SYS_BARRIER__VALUE_get(value);
                sysbarrier &= ~_BN(48 + 0);                 // Set classroute 0 to user-accessible.
                sysbarrier |=  _BN(48 + 14) | _BN(48+15);   // Set classroutes 14 and 15 to system-only.
                MU_DCR__SYS_BARRIER__VALUE_insert(value, sysbarrier);
                DCRWritePriv(MU_DCR(SYS_BARRIER), value);
                
                /* Reset the GI control register to the initial state */
                controlRegPtr = (uint64_t*)(BGQ_MU_GI_CONTROL_OFFSET(0, 0));
                *controlRegPtr = MUSPI_GIBARRIER_INITIAL_STATE;
                
                controlRegPtr = (uint64_t*)(BGQ_MU_GI_CONTROL_OFFSET(0, 14));
                *controlRegPtr = MUSPI_GIBARRIER_INITIAL_STATE;                
                
                MUSPI_GIBarrierInit ( &systemJobGIBarrier,
                                      14);
                
                if(!Personality_CiosEnabled())
                {
                    // barrier with other nodes in the full block when CIOS isn't enabled.
                    // this simulates the implicit runjob barrier when CIOS is enabled.  
                    MUSPI_GIBarrierEnterAndWait(&systemBlockGIBarrier);
                }
                return 0;
            }
        }
        // Could not find a classroute match for our coordinates!!!
        return EINVAL;
    }
    return 0;
}

int verifyJobClassroute()
{
    uint64_t* statusRegPtr;
    
    // Verify that the GI status register has propagated to the expected value.
    statusRegPtr  = (uint64_t*)(BGQ_MU_GI_STATUS_OFFSET(0, 0));
    assert((*statusRegPtr & 0x7) != MUSPI_GIBARRIER_INITIAL_STATE);
    
    statusRegPtr  = (uint64_t*)(BGQ_MU_GI_STATUS_OFFSET(0, 14));
    assert((*statusRegPtr & 0x7) != MUSPI_GIBARRIER_INITIAL_STATE);
    
    
    // \todo Need a barrier here before anyone uses the GI barrier!!!
    return 0;
}

Lock_Atomic_t mapping_storageLock;;
char mapping_storage[65536];

int computeTaskCoordinates(uint32_t startEntry, size_t mapsize, BG_CoordinateMapping_t* map, uint64_t* numEntries)
{
    int rc;
    uint32_t np = GetMyAppState()->ranksActive;
    if(np * sizeof(BG_CoordinateMapping_t) > mapsize)
    {
        return EINVAL;
    }
    *numEntries = np;
    
    Personality_t* pers = GetPersonality();
    AppState_t *appState = GetMyAppState();
    const char* filename = appState->mapOrder;
    BG_JobCoords_t jobcoord;
    BG_CoordinateMapping_t mycoord;

    mycoord.a = pers->Network_Config.Acoord - appState->corner.aCoord;
    mycoord.b = pers->Network_Config.Bcoord - appState->corner.bCoord;
    mycoord.c = pers->Network_Config.Ccoord - appState->corner.cCoord;
    mycoord.d = pers->Network_Config.Dcoord - appState->corner.dCoord;
    mycoord.e = pers->Network_Config.Ecoord - appState->corner.eCoord;
    mycoord.t = GetMyProcess()->Tcoord;
    
    jobcoord.corner.a = appState->corner.aCoord;
    jobcoord.corner.b = appState->corner.bCoord;
    jobcoord.corner.c = appState->corner.cCoord;
    jobcoord.corner.d = appState->corner.dCoord;
    jobcoord.corner.e = appState->corner.eCoord;
    jobcoord.shape.a  = appState->shape.aCoord;
    jobcoord.shape.b  = appState->shape.bCoord;
    jobcoord.shape.c  = appState->shape.cCoord;
    jobcoord.shape.d  = appState->shape.dCoord;
    jobcoord.shape.e  = appState->shape.eCoord;
    
    if(appState->mapFilePath[0] != 0)
        filename = appState->mapFilePath;
    
    Kernel_Lock(&mapping_storageLock);
    rc = MUSPI_GenerateCoordinates(filename, &jobcoord, &mycoord, appState->ranksPerNode, np,
                                   sizeof(mapping_storage), mapping_storage, map, NULL);
    Kernel_Unlock(&mapping_storageLock);
    return rc;
}

int getMyRank(uint32_t* rank)
{
    int rc;
    Personality_t* pers = GetPersonality();
    AppState_t *appState = GetMyAppState();
    BG_JobCoords_t jobcoord;
    BG_CoordinateMapping_t mycoord;
    const char* filename = appState->mapOrder;
    if(appState->mapFilePath[0] != 0)
        filename = appState->mapFilePath;
    
    *rank = ~0;
    mycoord.a = pers->Network_Config.Acoord - appState->corner.aCoord;
    mycoord.b = pers->Network_Config.Bcoord - appState->corner.bCoord;
    mycoord.c = pers->Network_Config.Ccoord - appState->corner.cCoord;
    mycoord.d = pers->Network_Config.Dcoord - appState->corner.dCoord;
    mycoord.e = pers->Network_Config.Ecoord - appState->corner.eCoord;
    mycoord.t = GetMyProcess()->Tcoord;
    
    jobcoord.corner.a = appState->corner.aCoord;
    jobcoord.corner.b = appState->corner.bCoord;
    jobcoord.corner.c = appState->corner.cCoord;
    jobcoord.corner.d = appState->corner.dCoord;
    jobcoord.corner.e = appState->corner.eCoord;
    jobcoord.shape.a  = appState->shape.aCoord;
    jobcoord.shape.b  = appState->shape.bCoord;
    jobcoord.shape.c  = appState->shape.cCoord;
    jobcoord.shape.d  = appState->shape.dCoord;
    jobcoord.shape.e  = appState->shape.eCoord;
    
    Kernel_Lock(&mapping_storageLock);
    rc = MUSPI_GenerateCoordinates(filename, &jobcoord, &mycoord, appState->ranksPerNode, appState->ranksActive,
                                   sizeof(mapping_storage), mapping_storage, NULL, rank);
    Kernel_Unlock(&mapping_storageLock);
    
    return rc;
}
