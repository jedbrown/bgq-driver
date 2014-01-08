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

extern "C"
{
#include <mudm/include/mudm.h>
}

#if CONFIG_DISTRO_MAPFILE
uint64_t mapfile_open(const char *path, int oflag, mode_t mode);
uint64_t mapfile_read(int fd, void* buffer, size_t length);
uint64_t mapfile_close(int fd);
#define open mapfile_open
#define read mapfile_read
#define close mapfile_close
#else
#define open internal_open
#define read internal_read
#define close internal_close
#endif
#define strtoull strtoull_
#include <spi/include/mu/RankMap.h>

MUSPI_GIBarrier_t systemBlockGIBarrier;
MUSPI_GIBarrier_t systemJobGIBarrier;

// storage should be at least able to handle 64 * strlen("aa bb cc dd ee tt") + 1 = 1152 bytes

typedef struct
{
    uint64_t chunkid;
    size_t   length;
    char     storage[CONFIG_MAPFILECHUNKSIZE];
} MapFileData_t;

uint32_t      mapfile_distromap = 0;
uint64_t      mapfileData_unset[CONFIG_MAPFILETRACKS];
MapFileData_t mapfileData[CONFIG_MAPFILETRACKS];
MapFileData_t cachemapfileData[CONFIG_MAPFILETRACKS];
size_t        MAPFILE_BYTE_OFFSET[CONFIG_MAPFILETRACKS];
Lock_Atomic_t cacheMapLock[CONFIG_MAPFILETRACKS];

Lock_Atomic_t mapping_storageLock[CONFIG_MAPFILETRACKS];
char mapping_storage[CONFIG_MAPFILETRACKS][CONFIG_MAPFILESTORAGE/CONFIG_MAPFILETRACKS];

struct remoteget_atomic_controls mapfile_remoteget;
uint64_t mapfile_pacingsem[2] ALIGN_L2_CACHE = { 0, CONFIG_MAPFILEREMOTEPACING };



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

int selectTrack()
{
    if(mapfile_distromap == 0)
        return 0;
    
    return GetMyProcess()->Tcoord % CONFIG_MAPFILETRACKS;
}

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
    
    uint64_t thread = selectTrack();
    Kernel_Lock(&mapping_storageLock[thread]);
    rc = MUSPI_GenerateCoordinates(filename, &jobcoord, &mycoord, appState->ranksPerNode, np,
                                   sizeof(mapping_storage[thread]), &mapping_storage[thread][0], map, NULL);
    Kernel_Unlock(&mapping_storageLock[thread]);
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
    
    uint64_t thread = selectTrack();
    Kernel_Lock(&mapping_storageLock[thread]);
    rc = MUSPI_GenerateCoordinates(filename, &jobcoord, &mycoord, appState->ranksPerNode, appState->ranksActive,
                                   sizeof(mapping_storage[thread]), &mapping_storage[thread][0], NULL, rank);
    Kernel_Unlock(&mapping_storageLock[thread]);
    return rc;
}


int calculateTorusNode(size_t byteOffset, uint32_t track, MUHWI_Destination_t* coord)
{
    AppState_t *appState = GetMyAppState();
    uint64_t chunkID = byteOffset/CONFIG_MAPFILECHUNKSIZE;
    uint64_t tmp = chunkID;
    uint64_t index = (chunkID + track * appState->shape.aCoord * appState->shape.bCoord * appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord / CONFIG_MAPFILETRACKS) % (appState->shape.aCoord * appState->shape.bCoord * appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord);
    
    assert(chunkID < (uint64_t)appState->shape.aCoord * appState->shape.bCoord * appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord);
    
    coord->Destination.Destination = 0;

    tmp = appState->shape.bCoord * appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord;
    coord->Destination.A_Destination = index / tmp;
    index -= coord->Destination.A_Destination * appState->shape.bCoord * appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord;
    
    tmp = appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord;
    coord->Destination.B_Destination = index / tmp;
    index -= coord->Destination.B_Destination * appState->shape.cCoord * appState->shape.dCoord * appState->shape.eCoord;
    
    tmp = appState->shape.dCoord * appState->shape.eCoord;
    coord->Destination.C_Destination = index / tmp;
    index -= coord->Destination.C_Destination * appState->shape.dCoord * appState->shape.eCoord;

    tmp = appState->shape.eCoord;
    coord->Destination.D_Destination = index / tmp;
    index -= coord->Destination.D_Destination * appState->shape.eCoord;
    
    coord->Destination.E_Destination = index;
    
    coord->Destination.A_Destination += appState->corner.aCoord;    
    coord->Destination.B_Destination += appState->corner.bCoord;    
    coord->Destination.C_Destination += appState->corner.cCoord;    
    coord->Destination.D_Destination += appState->corner.dCoord;
    coord->Destination.E_Destination += appState->corner.eCoord;
    return 0;
}

int setupMapFile()
{
#if CONFIG_DISTRO_MAPFILE
    AppState_t *appState = GetMyAppState();
    int fd;
    int rc;
    int64_t bytes;
    int x;
    uint64_t totalOffset = 0;
    if(appState->mapFilePath[0] == 0)
        return 0;

    mapfile_distromap = 0;
    App_GetEnvValue("BG_DISTRIBUTEDMAPFILE", &mapfile_distromap);
    if(mapfile_distromap == 0)
        return 0;
    
    for(x=0; x<CONFIG_MAPFILETRACKS; x++)
    {
        mapfileData[x].chunkid = (~0);
        mapfileData_unset[x] = sizeof(MapFileData_t);
        memset(&mapfileData[x], 0, sizeof(MapFileData_t));
        cachemapfileData[x].chunkid = (~0);
    }
    ppc_msync();
    
    rc = MUSPI_GIBarrierEnterAndWait( &systemJobGIBarrier );
    if(App_IsJobLeaderNode())
    {
        memset(&cachemapfileData[0], 0, sizeof(cachemapfileData[0]));
        
        fd = internal_open(appState->mapFilePath, O_RDONLY | O_LARGEFILE, 0666);
        while((bytes = internal_read(fd, &cachemapfileData[0].storage[0], CONFIG_MAPFILECHUNKSIZE)) > 0)
        {
            for(x=0; x<CONFIG_MAPFILETRACKS; x++)
            {
                MUHWI_Destination_t coord;
                calculateTorusNode(totalOffset, x, &coord);
                
                cachemapfileData[0].chunkid= totalOffset / CONFIG_MAPFILECHUNKSIZE;
                cachemapfileData[0].length = bytes;
                
                // \note msync not necessary; torus mmio write has implicit msync
                Kernel_WriteFlightLog(FLIGHTLOG, FL_MAPFILEWR, cachemapfileData[0].chunkid, cachemapfileData[0].length, coord.Destination.Destination, x);
                
                rc = mudm_rdma_write_on_torus(NodeState.MUDM, coord, sizeof(MapFileData_t), &mapfileData_unset[x], &cachemapfileData[0], &mapfileData[x]);
                volatile uint64_t remoteCounter = ~0;
                do
                {
                    volatile uint64_t pollcounter = sizeof(remoteCounter);
                    rc = mudm_rdma_read_on_torus(NodeState.MUDM, coord, sizeof(remoteCounter), (void*)&pollcounter, (void*)&remoteCounter, &mapfileData_unset[x]);
                    while(pollcounter != 0)
                    {
                        Delay(100);
                        ppc_msync();
                    }
                }
                while(remoteCounter != 0);
            }
            
            totalOffset += bytes;
            memset(&cachemapfileData[0], 0, sizeof(cachemapfileData[0]));
        }
        internal_close(fd);
        cachemapfileData[0].chunkid = (~0);
    }
    rc = MUSPI_GIBarrierEnterAndWait( &systemJobGIBarrier );
#endif
    return 0;
}

uint64_t mapfile_open(const char *path, int oflag, mode_t mode)
{
    if(mapfile_distromap == 0)
        return internal_open(path, oflag, mode);
    
    uint64_t fd = selectTrack();
    MAPFILE_BYTE_OFFSET[fd] = 0;    
    Kernel_WriteFlightLog(FLIGHTLOG, FL_MAPFILEOP, fd,0,0,0);
    return fd + 3;
}

uint64_t mapfile_read(int fd, void* ptr, size_t length)
{
    if(mapfile_distromap == 0)
        return internal_read(fd, ptr, length);
    
    int rc;
    char* buffer = (char*)ptr;
    uint64_t read_bytes = 0;
    size_t bytes = (~0);
    Personality_t* pers = GetPersonality();
    int track = selectTrack();
    
    MUHWI_Destination_t mycoord;
    mycoord.Destination.Destination = 0;
    mycoord.Destination.A_Destination = pers->Network_Config.Acoord;
    mycoord.Destination.B_Destination = pers->Network_Config.Bcoord;
    mycoord.Destination.C_Destination = pers->Network_Config.Ccoord;
    mycoord.Destination.D_Destination = pers->Network_Config.Dcoord;
    mycoord.Destination.E_Destination = pers->Network_Config.Ecoord;
    while((length) && (bytes != 0))
    {
        uint64_t chunkid;
        MUHWI_Destination_t coord;

        chunkid = MAPFILE_BYTE_OFFSET[track] / CONFIG_MAPFILECHUNKSIZE;
        while(chunkid != cachemapfileData[track].chunkid)
        {
            calculateTorusNode(MAPFILE_BYTE_OFFSET[track], track, &coord);
            
            if(mycoord.Destination.Destination == coord.Destination.Destination)
            {
                memcpy(&cachemapfileData[track], &mapfileData[track], sizeof(cachemapfileData[track]));
            }
            else
            {
                Kernel_Lock(&cacheMapLock[0]);
                
                do
                {
                    mapfile_remoteget.torus_destination = coord;
                    mapfile_remoteget.atomic_op = MUHWI_ATOMIC_OPCODE_LOAD_DECREMENT_BOUNDED;
                    mapfile_remoteget.paddr_here = (uint64_t)(&mapfile_remoteget);
                    mapfile_remoteget.remote_paddr = (uint64_t)(&mapfile_pacingsem[1]);
                    NodeState.remoteget.returned_val = 0;
                    NodeState.remoteget.local_counter = 8;
                    mudm_remoteget_load_atomic(NodeState.MUDM,  &mapfile_remoteget);
                    while(mapfile_remoteget.local_counter)
                    {
                        ppc_msync();
                    }
                    
                    if(mapfile_remoteget.returned_val != 0x8000000000000000)
                        break;
                    Kernel_WriteFlightLog(FLIGHTLOG, FL_MAPFILESM, coord.Destination.Destination, MAPFILE_BYTE_OFFSET[track], track, 0);
                    Delay(1000000);
                }
                while(1);
                
                volatile uint64_t pollcounter = sizeof(cachemapfileData[track]);
                Kernel_WriteFlightLog(FLIGHTLOG, FL_MAPFILERD, chunkid, pollcounter, coord.Destination.Destination, track);
                do
                {
                    rc = mudm_rdma_read_on_torus(NodeState.MUDM, coord, sizeof(cachemapfileData[track]), (void*)&pollcounter, &cachemapfileData[track], &mapfileData[track]);
                    if(rc == -EBUSY)
                    {
                        while(1) { }
                    }
                }
                while(rc == -EBUSY);
                
                while(pollcounter != 0)
                {
                    Delay(100);
                    ppc_msync();
                }
                ppc_msync();
                
                mapfile_remoteget.torus_destination = coord;
                mapfile_remoteget.atomic_op = MUHWI_ATOMIC_OPCODE_LOAD_INCREMENT;
                mapfile_remoteget.paddr_here = (uint64_t)(&mapfile_remoteget);
                mapfile_remoteget.remote_paddr = (uint64_t)(&mapfile_pacingsem[1]);
                NodeState.remoteget.returned_val = 0;
                NodeState.remoteget.local_counter = 8;
                mudm_remoteget_load_atomic(NodeState.MUDM,  &mapfile_remoteget);
                while(mapfile_remoteget.local_counter)
                {
                    ppc_msync();
                }
                
                Kernel_Unlock(&cacheMapLock[0]);
            }
        }
        bytes = MIN( cachemapfileData[track].length + chunkid*CONFIG_MAPFILECHUNKSIZE - MAPFILE_BYTE_OFFSET[track], length);
        memcpy(buffer, &cachemapfileData[track].storage[MAPFILE_BYTE_OFFSET[track] % CONFIG_MAPFILECHUNKSIZE], bytes);
        
        read_bytes              += bytes;
        buffer                  += bytes;
        length                  -= bytes;
        MAPFILE_BYTE_OFFSET[track] += bytes;
    }
    return read_bytes;
}

uint64_t mapfile_close(int fd)
{
    if(mapfile_distromap == 0)
        return internal_close(fd);
    
    fd -= 3;
    Kernel_WriteFlightLog(FLIGHTLOG, FL_MAPFILECL, fd,0,0,0);
    return 0;
}
