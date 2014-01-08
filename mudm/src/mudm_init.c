/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (c) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/*                                                                  */
/* US Government Users Restricted Rights - Use, duplication or      */
/* disclosure restricted by GSA ADP Schedule Contract with IBM      */
/* Corporation.                                                     */
/*                                                                  */
/* This software is available to you under either the GNU General   */
/* Public License (GPL) version 2 or the Eclipse Public License     */
/* (EPL) at your discretion.                                        */
/*                                                                  */

//! \file  mudm_init.c
//! \brief Implementation of special programming interfaces for creating a MUDM context
//! \remarks primarily intended initialization
#define MUDM_LOOPBACK 1

#include <spi/include/kernel/MU.h>
#include <spi/include/kernel/memory.h>

#include <mudm/include/mudm.h>

#include <mudm/include/mudm_inlines.h>
#include <mudm/include/mudm_utils.h>



#include "common.h"
#include "rdma_object.h"

#include "mudm_pkt.h"
#include "common_inlines.h"

#include "mudm_ras.h"


#ifdef __FWEXT__
#include <firmware/include/fwext/fwext.h>
#include <firmware/include/Firmware.h>
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <firmware/include/fwext/fwext_lib.h>

#include <firmware/include/fwext/fwext_nd.h>
#include <firmware/include/fwext/fwext_mu.h>

#endif
#include <hwi/include/bqc/nd_500_dcr.h>
#include <firmware/include/personality.h>
#include <spi/include/mu/Addressing_inlines.h>

#include <hwi/include/bqc/testint_dcr.h>

#include "mudm_wu.h"


#define TESTINT_CONFIG1_BIT_SWAP(v)        _BGQ_SET(5,125-64,v)
#define TESTINT_CONFIG1_IS_IONODE          _BN(127-64)
#define TESTINT_CONFIG1_USE_PORT6_FOR_IO   _BN(105-64)
#define TESTINT_CONFIG1_USE_PORT7_FOR_IO   _BN(106-64)



//! \todo TODO make array in my_context to hold SYS_REC_FIFO_ID and 2
//#define SYS_SUBGROUPID   66  
//#define SYS_SUBGROUPID2   67 /*16*4 + 3--4 subgroups per subgroup, use the fourth subgroup in the 17th group */
//#define SYS_REC_FIFO_ID   264 /* 4 * SYS_GROUPID + 0 */
//#define SYS_REC_FIFO_ID2  268 /* 4 * SYS_GROUPID2 + 0 */

uint32_t init_subgroup_id[2] = {
  66,     /*16*4 + 2  4 subgroups per group, use the third subgroup in the 17th group */
  67      /*16*4 + 3  4 subgroups per group, use the fourth subgroup in the 17th group */
};
uint32_t init_sys_rec_fifo_id[2] = {
  264,     /* 4 * 66 + 0  first in subgroup id 66*/
  268      /* 4 * 67 + 0  first in subgroup id 67*/
};

uint32_t get_init_sys_rec_fifo_id(int i) {return init_sys_rec_fifo_id[i];};

uint64_t free_MU_inj_region(struct mudm_memory_region * memregion,struct my_context * mcontext, void * args){
  //$$$$
  struct MU_subgroup_memory * subgroup_mem = (struct MU_subgroup_memory *)args;
  MUSPI_InjFifoSubGroup_t    * ififo_subgroup = &subgroup_mem->ififo_subgroup;
  int rc=0;  
  uint32_t nfifos = 1;
  uint32_t * ififoid = &subgroup_mem->ififoid;
ENTER; 
    PRINT("%s: deactivate and deallocate injection fifos for subgrpid \n",__FUNCTION__);

    rc = Kernel_InjFifoActivate (ififo_subgroup, nfifos, ififoid, KERNEL_INJ_FIFO_DEACTIVATE);    
    if (rc != 0)
      {
        HERE;
        PRINT("Kernel_InjFifoActivate--KERNEL_INJ_FIFO_DEACTIVATE Inj failed with rc=%d\n",rc);
	return -EFAULT;
      }

    PRINT("%s: deactivated injection fifos  \n",__FUNCTION__);
    
    rc= Kernel_DeallocateInjFifos (ififo_subgroup,nfifos,ififoid);
    if ( rc != 0)
    {
      HERE;
      PRINT("Kernel_DeallocateInjFifos failed with rc=%d\n",rc);
      return -EFAULT;
    }
    rc = free_mem(memregion,mcontext,args); 
EXIT;
  return rc;
}


/**
 * \brief Activate Injection FIFO
 *
 * For the subgroup, activate the MUDM system injection FIFOs at the given physical storage.
 *
 * \param[in] inj_memory_fifo
 * \param[in] ififo_subgroup
 * \param[in] inj_mem_region points to a struct of virtual and physical address of given memory size
 *            for injecting memFIFO and putdirect descriptors 
 * \param[in] injrget_mem_region points to a struct of virtual and physical address of given memory size
 * 
 *
 * \retval 0  Success
 * \retval -1 Error
 * \todo Check on error values
 *
 */

int activate_injfifos(struct MU_subgroup_memory * musm){

    /* Kernel_InjFifoInterrupts_t  injFifoInterrupts[2]; */
    MUSPI_InjFifoSubGroup_t    * ififo_subgroup = &musm->ififo_subgroup;
    struct mudm_memory_region * inj_mem_region =  &musm->injRegion;
    int rc=0;
    int i = 0;
    uint32_t subgrpid = musm->subgroupID;
    uint32_t nfifos = 1;
    uint32_t * ififoid = &musm->ififoid; 
    //uint64_t injFifoNormalThreshold    = 0; //NOT USING INTERRUPTS for threshold
    //int64_t injFifoRemoteGetThreshold = 0; //NOT USING INTERRUPTS  for threshold

    Kernel_InjFifoInterrupts_t  injFifoInterrupts[nfifos]; 
    Kernel_InjFifoAttributes_t injFifoAttrs[nfifos];

    for (i=0;i<nfifos;i++){/*BEAM*//*loop doesn't iterate*/
      ififoid[i]=i;
      injFifoAttrs[i].RemoteGet = 0;
      injFifoAttrs[i].System    = 1;    
       injFifoInterrupts[i].Threshold_Crossing = 0;  // An interrupt is NOT FIRED OFF
    }

    rc= Kernel_AllocateInjFifos (subgrpid, ififo_subgroup, nfifos, ififoid, injFifoAttrs);
    if ( unlikely(rc != 0) )
    {
      //MPRINT("Kernel_AllocateInjFifos failed with rc=%d\n",rc);
      MUDM_RAS_INITERR(EFAULT);
      return -EFAULT;
    }

    /* inj fifo for memfifo injections */
    /* spi/include/kernel/MU.h requires size of the injection fifo minus 1 in bytes */
    /* fifoid[i] is 0 to BGQ_MU_NUM_INJ_FIFOS_PER_SUBGROUP-1 */
    for (i=0;i<nfifos;i++){/*BEAM*//*loop doesn't iterate*/
       rc=Kernel_InjFifoInit (ififo_subgroup, ififoid[i], (Kernel_MemoryRegion_t *)inj_mem_region,0,inj_mem_region->length-1); 
       if ( unlikely(rc != 0) )
       {
         //MPRINT("Kernel_InjFifoInit 0 failed with rc=%d\n",rc);
         MUDM_RAS_INITERR(EFAULT);
	 return -EFAULT;
       } 
       inj_mem_region++; //advance to next memregion in the list
    } 
    //MHERE;
    //dump_subgroup_memory_struct(musm); 
  
    //! \note DO NOT do any configuring of InjFifo Interrupts.  Rely on firmware setting
    //! rc = Kernel_ConfigureInjFifoInterrupts (ififo_subgroup, nfifos, fifoid,  injFifoInterrupts); 
    //! \note DO NOT touch INTERRUPTS TO GEA setting.  Rely on firmware setting
    //! rc = Kernel_Configure_MU_GEA(subgrpid , KERNEL_SUBGROUP_INTERRUPTS_GO_TO_GEA); 
    //! rc = Kernel_Configure_MU_GEA(subgrpid , KERNEL_SUBGROUP_INTERRUPTS_DO_NOT_GO_TO_GEA);
    //! \note DO NOT configure injection FIFO threshold as this sets the register (imu_thold) for all injection 
    //!  imFIFO and the register (rget_thold) for all rget injections
    //! Src = Kernel_ConfigureInjFifoThresholds( &injFifoNormalThreshold, &injFifoRemoteGetThreshold); 

    rc = Kernel_InjFifoActivate (ififo_subgroup, nfifos, ififoid, KERNEL_INJ_FIFO_ACTIVATE);    
    if ( unlikely(rc != 0) )
      {
        //MPRINT("Kernel_InjFifoActivate Inj failed with rc=%d\n",rc);
        MUDM_RAS_INITERR(EFAULT);
	return -EFAULT;
      }
    
    // Clear interrupt status for both injection fifos.
    ififo_subgroup->groupStatus->clearInterruptStatus = _BN(0) | _BN(1);

    

    PRINT("%s: activated injection fifos for subgrpid %u\n",__FUNCTION__, subgrpid);
    return 0;   

}

void deactivate_base_address_table(MUSPI_BaseAddressTableSubGroup_t * bat,uint32_t numbats,uint32_t * batids){  
    Kernel_DeallocateBaseAddressTable (bat,numbats,batids);
}

void activate_base_address_table(MUSPI_BaseAddressTableSubGroup_t * bat,uint32_t numbats,uint32_t * batids,uint32_t subgrpid,MUHWI_BaseAddress_t * batval){
    uint8_t i;
    // Set up the base address table 
    // Set base address to 0 and use physical address as offset
    Kernel_AllocateBaseAddressTable ( subgrpid, bat, numbats, batids, 1 /* System use */ );
    
    // base address is the beginning of physical memory, location 0
    for ( i=0; i<numbats; i++ ){
      MUSPI_SetBaseAddress ( bat, i, batval[i] );
      //MPRINT("bat=%p, i=%llu, batval[i]=%llx \n", bat, (LLUS)i, (LLUS)batval[i]);
    }
}


uint64_t free_MU_subgroup_mem(struct mudm_memory_region * memregion,struct my_context * mcontext, void * args){

  int status_free;
  struct MU_subgroup_memory * subgroup_mem = (struct MU_subgroup_memory *)memregion->base_vaddr;
  ENTER;
  //$$$$
  deactivate_base_address_table(&subgroup_mem->bat,BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP,subgroup_mem->batids);
  status_free = free_mem(memregion,mcontext,args); 
  EXIT;
  return status_free;
}

uint64_t deactivate_recfifo(struct my_context * mcontext, struct MU_subgroup_memory * subgroup_mem){
  uint32_t rc=0;
  uint32_t nfifos = 1;
  uint32_t * rfifoids = &subgroup_mem->rfifoids;
  uint32_t groupid  = SYS_GROUPID; 
  uint32_t subgrpid = subgroup_mem->subgroupID;
  
  uint64_t recFifoDisableBits = 0;
  recFifoDisableBits |= ( 0x0000000000000001ULL <<
                       ( 15 - ( (subgrpid % BGQ_MU_NUM_FIFO_SUBGROUPS)*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) - *rfifoids )  );
ENTER;
  rc = Kernel_RecFifoDisable ( groupid, recFifoDisableBits);
  if ( unlikely(rc != 0) )
  {
        
        //MPRINT("Kernel_RecFifoDisable failed with rc=%d\n",rc);
        MUDM_RAS_INITERR(EFAULT);
	return -EFAULT;
  }

  rc = Kernel_DeallocateRecFifos (&subgroup_mem->rfifo_subgroup,nfifos,rfifoids);
  if ( unlikely(rc != 0) )
  {
        
        //MPRINT("Kernel_DeallocateRecFifos failed with rc=%d\n",rc);
        MUDM_RAS_INITERR(EFAULT);
	return -EFAULT;
  }
  return rc;
}

uint64_t free_recfifo(struct mudm_memory_region * memregion,struct my_context * mcontext, void * args){
  uint64_t status_free;
  status_free = deactivate_recfifo(mcontext, (struct MU_subgroup_memory *)args);
  if (status_free) return status_free;
  if (memregion) status_free = free_mem(memregion,mcontext,args); 
  EXIT;
  return status_free;
  //! \todo TODO check cleanup array that it has enough entries for additional memory regions....
}


int32_t activate_recfifos(struct MU_subgroup_memory * musm){ 
    MUSPI_RecFifoSubGroup_t * rfifo_subgroup= &musm->rfifo_subgroup;
    struct mudm_memory_region * rec_mem_region = &musm->recRegion;
    int i = 0;
    int32_t rc=0;
    uint32_t nfifos = 1;
    Kernel_RecFifoInterrupts_t recFifoInterrupts[1];
    Kernel_RecFifoAttributes_t recFifoAttrs[1];
 
    uint32_t * rfifoids = &musm->rfifoids;
    uint32_t groupid  = SYS_GROUPID; 
    uint32_t subgrpid = musm->subgroupID;

    
    uint64_t recFifoEnableBits=0;
    for (i=0;i<nfifos;i++){/*BEAM*//*loop doesn't iterate*/
       rfifoids[i] = i;

       recFifoAttrs[i].System = 1;
       recFifoInterrupts[i].Threshold_Crossing = 0;
       recFifoInterrupts[i].Packet_Arrival     = 1; 
    }
    //MHERE;

    //MPRINT("%s: allocate reception FIFO for group %u, subgroup %u\n", __FUNCTION__, groupid, subgrpid);
    /* from spi/include/kernel/MU.h
     subgrpid  SubGroup being allocated (0 to BGQ_MU_NUM_FIFO_SUBGROUPS_PER_NODE-1).
     int32_t Kernel_AllocateRecFifos (uint32_t                     subgrpid,
				 MUSPI_RecFifoSubGroup_t    * sg_ptr,
				 uint32_t                     nfifos,
				 uint32_t                   * fifoids,
				 Kernel_RecFifoAttributes_t * fifoAttrs);
    */
    rc=Kernel_AllocateRecFifos (subgrpid, rfifo_subgroup, nfifos, rfifoids, recFifoAttrs);
    if ( unlikely(rc != 0) )
    {
        //MPRINT("Kernel_AllocateRecFifos failed with rc=%d\n",rc);
        MUDM_RAS_INITERR(EFAULT);
	return -EFAULT;
    }

    PRINT("%s: init reception FIFO 1\n", __FUNCTION__);

    for (i=0;i<nfifos;i++){/*BEAM*//*loop doesn't iterate*/
      rc=Kernel_RecFifoInit(rfifo_subgroup, rfifoids[i], (Kernel_MemoryRegion_t *)rec_mem_region,0,rec_mem_region->length-1);
      if ( unlikely(rc != 0) )
       {
        //MPRINT("Kernel_RecFifoInit failed with rc=%d\n",rc);
        MUDM_RAS_INITERR(EFAULT);
	return -EFAULT;
       } 
           

       recFifoEnableBits |= ( 0x0000000000000001ULL <<
                         ( 15 - ( (subgrpid % BGQ_MU_NUM_FIFO_SUBGROUPS)*BGQ_MU_NUM_REC_FIFOS_PER_SUBGROUP) - rfifoids[i] )  );
       rec_mem_region++;//advance to next rec fifo memory region
    }    
    musm->clearInterrupts_mask = recFifoEnableBits;  //the bits for enabling the FIFOs and clear mask are the same          
    //DUMPHEXSTR("Kernel_RecFifoInit MUSPI_RecFifoSubGroup_t * rfifo_subgroup", rfifo_subgroup,sizeof(MUSPI_RecFifoSubGroup_t) );
    /////////////////////////////////////////// INTERRUPTS per recFifoInterrupts ////////////////////////////////////////////////
    //MPRINT("rfifo_subgroup=%llx %llu nfifos=%d \n",(LLUS)rfifo_subgroup, (LLUS)rfifo_subgroup,nfifos);
    //MDUMPHEXSTR("rfifoids",rfifoids,sizeof(rfifoids));
    //MDUMPHEXSTR("recFifoInterrupts",recFifoInterrupts,sizeof(recFifoInterrupts));
    rc= Kernel_ConfigureRecFifoInterrupts (rfifo_subgroup,
					   nfifos,
					   rfifoids,
					   recFifoInterrupts);

    /* int32_t Kernel_RecFifoEnable (  uint32_t groupid,
				uint64_t enableBits );
    param[in]  groupid     Id of the fifo group
                          (0 to BGQ_MU_NUM_FIFO_GROUPS-1).
    param[in]  enableBits  Bit mask indicating which fifos
                          to enable.  There are 16 fifos
                          in a group.  The low-order 16 bits
                          are used:
                          - Bit 48 is fifo 0
                          - Bit 49 is fifo 1
                          - and so on...
                          - Bit 63 is fifo 15.
    */
    //MPRINT("groupid=%d recFifoEnableBits=%llx \n",groupid, (LLUS)recFifoEnableBits);
    //MHERE MPRINT("B4 RecFifoEnable DCRReadPv(MU_DCR(RMFIFO_EN) + 16)=%llx \n",(LLUS)DCRReadPriv(MU_DCR(RMFIFO_EN) + 16) );
    Kernel_RecFifoEnable (groupid, recFifoEnableBits );
    //MHERE MPRINT("Post RecFioEnable DCRReadPv(MU_DCR(RMFIFO_EN) + 16)=%llx \n",(LLUS)DCRReadPriv(MU_DCR(RMFIFO_EN) + 16) );

    return 0;

}


void InitCollectiveMemoryFIFODescriptor( MUHWI_Descriptor_t          *desc,
                                               uint32_t source_torus_addr,
                                               uint16_t rec_FIFO_Id
                                             );
//! \note block-wide system collective class route is already set in firmware, firmware/src/fwext_ddr/fw_nd.c:fw_nd_set_collective_class_routes


//! \todo TODO pass &mcontext->MU_subgroup_mem[0] or pass index as an argument to bcast_init ........
int bcast_init(struct my_context * mcontext,int i){
  struct mudm_bcast *mb = &mcontext->system_bcast;

  ENTER;

  // need the physical address for memfifo broadcast payload computation
  mb->RDMA_bcast_control.bcast_rdma_object_pa = mcontext->mudm_context_phys_addr + offsetof(struct my_context,system_bcast) + offsetof(struct mudm_bcast,RDMA_bcast_control);
  //MUSPI_SetBaseAddress ( &mcontext->MU_subgroup_mem[0]->bat, SYS_BATID_BCAST_CTR - SYS_BATID, mcontext->mudm_context_phys_addr);

  mb->IdToInjFifo = mcontext->injfifo_ctls[i].injfifo;
  mb->inj_fifo_lock = &mcontext->injfifo_ctls[i].inj_fifo_lock;
  mb->maincontext = mcontext;
  mb->memfifo_sent = 0;
  
  // for broadcast, reduce, all-reduce collectives
  // inline method against template is common_inlines.h:InjFifoInjectMemFifoBcast
  InitCollectiveMemoryFIFODescriptor( &mb-> mu_iMemoryFifoDescriptor,  mcontext->myTorusAddress.Destination.Destination, mcontext->sys_rec_fifo_id[i] );
  // the counter offset is based on the start of the RDMA counter region base-address
  mb->RDMA_bcast_control.bcast_counter_info.mu_counter_offset =  mcontext->rdma_obj_ctls[i]->rdma_bcast_controls_offset;
  mb->RDMA_bcast_control.mu_counter_bcast_addr = mcontext->rdma_obj_ctls[i]->rdma_bcast_controls;
  //PRINT("mb->RDMA_bcast_control.mu_counter_bcast_addr =%p\n",mb->RDMA_bcast_control.mu_counter_bcast_addr);
  //PRINT("mb->RDMA_bcast_control.bcast_counter_info.mu_counter_offset = %llx \n",(LLUS)mb->RDMA_bcast_control.bcast_counter_info.mu_counter_offset);
  //PRINT("mb->RDMA_bcast_control.bcast_counter_info.mu_counter_offset = %llu \n",(LLUS)mb->RDMA_bcast_control.bcast_counter_info.mu_counter_offset);
  InitBroadcastDirectPut( &mb->RDMA_bcast_control.mu_iDirectPutDescriptor,mb->RDMA_bcast_control.bcast_counter_info.mu_counter_offset,0);
EXIT;
return 0;
}



/**
 * \brief Implementation of mudm_init
 *
 */

int  mudm_init(struct mudm_init_info* init_info, 
               struct mudm_cfg_info* cfg_info, 
               void** mudm_context )
{  
   struct my_context * mcontext=NULL;
   int temp = 0;
   int alloc_status; /* status from allocate callback */
   struct mudm_memory_region core_region;
   uint32_t i;
   
/* struct mudm_cfg_info returned by the data mover into supplied structure */
  PRINT("Entering %s FUNCTION %s LINE %d DATE %s TIME %s \n",__FILE__,__FUNCTION__,__LINE__, __DATE__,__TIME__);
  
  cfg_info->mudm_version = MUDM_VERSION;
  cfg_info->mudm_errno = 0;
  cfg_info->num_io_links = NUM_IO_LINKS;

  if (cfg_info->mudm_version != init_info->callers_version){
    cfg_info->mudm_errno = EBADRQC; /* bad request code for bad version */
    PRINT("Version Mismatch\n");
    return -EBADRQC;
  }
  
  cfg_info->max_connections=init_info->req_inbound_connections; /* maximum torus connections */
  
  if (cfg_info->max_connections < 4) cfg_info->max_connections = 4;    

  PRINT("cfg_info->max_connections=%d \n",cfg_info->max_connections);
  

  if ( (NULL==init_info->allocate)||(NULL==init_info->status)
   ||(NULL==init_info->recv_conn) || (NULL==init_info->recv) 
   ||(NULL==init_info->free)
      )
  {
    PRINT("\n>>>callback function pointer is null\n");
    cfg_info->mudm_errno = EINVAL; /* invalid argument */
    PRINT("Invalid value--NULL callback\n");
    return -EINVAL;
  }

  alloc_status =  (init_info->allocate)(&core_region,sizeof (struct my_context),init_info->callback_context); 
  if (alloc_status){
     (init_info->free)(&core_region,init_info->callback_context); 
     HERE;
     return -ENOMEM;
  }
  *mudm_context=core_region.base_vaddr;
  mcontext=*mudm_context;
  zero_memregion(&core_region);
  memcpy(&mcontext->mmregion,&core_region,sizeof(struct mudm_memory_region));
  mcontext->mudm_context_phys_addr = (uint64_t)core_region.base_paddr;

  mcontext=*mudm_context;
  mcontext -> max_connections = cfg_info->max_connections;

  mcontext->personality=init_info->personality;
  PRINT("personality=%p \n",mcontext->personality);

  mcontext-> my_using_IO_port = getCNportOnIOnode(mcontext->personality);
  PRINT("from getCNportOnIOnode is mcontext-> my_using_IO_port= %x \n",mcontext-> my_using_IO_port);
  /* create the first cleanup table entry */
  alloc_status = add_object(&mcontext->mmregion,mcontext, NULL, free_mem);
  if (alloc_status){
    MUDM_RAS_INITERR(EPERM);
     goto ALLOC_ERROR;  
  }

  
  mcontext->magic=MUDM_MAGIC;
  mcontext->recv = init_info->recv;
  mcontext->recv_conn = init_info->recv_conn;
  mcontext->status = init_info->status;
  mcontext->allocate = init_info->allocate;
  mcontext->free = init_info->free;
  mcontext->callback_context = init_info->callback_context;
//WAKEUP_CONFIG:
  
  //MENTER;
  //$$$ uncomment following line to force use of wakeup unit on compute nodes
  //init_info->wakeupActive = 1;
  //$$$ uncomment following line to force use of mudm "hard" poll on compute nodes
  //init_info->wakeupActive = 0;

   cfg_info->wakeupActive = init_info->wakeupActive;
#ifdef __LINUX__
   if (cfg_info->wakeupActive){
      MPRINT("WAKEUP NOT SUPPORTED ON LINUX");
   }
   cfg_info->wakeupActive = 0;  //no wakeup for Linux supported w/o support infrastructure
#endif

  mcontext->wakeupActive = cfg_info->wakeupActive;


//PERSONALITY:
  mcontext->personality=init_info->personality;

  mcontext -> myTorusAddress.Destination.Destination = get_my_torus_addr(init_info->personality);
  mcontext->mybridgingCNnode.Destination.Destination= get_my_bridging_torus_addr(init_info->personality); 
          
  //Get local information
  mcontext->myUCI = getUCI(mcontext->personality);
  mcontext->myBlockID = get_BlockID(mcontext->personality);
  mcontext->start_clock = GetTimeBase2();

  
  alloc_status = rdma_init(mcontext, cfg_info->max_connections,NUM_IO_LINKS);
  if (alloc_status){
     MUDM_RAS_INITERR(EPERM);
     goto ALLOC_ERROR;  
  }

 for (i=0;i<NUM_IO_LINKS;i++){/*BEAM*//*loop doesn't iterate*/
   int j;
   /* allocate subgroup memory to be used to manage system MU FIFOs */
    alloc_status = alloc_object(sizeof( struct MU_subgroup_memory),&mcontext->MU_subgroup_memregion[i],mcontext,NULL, free_MU_subgroup_mem);
    if (alloc_status){
       MUDM_RAS_INITERR(EPERM);
       goto ALLOC_ERROR;  
    }

    mcontext->MU_subgroup_mem[i] = mcontext->MU_subgroup_memregion[i].base_vaddr;
 
    mcontext->MU_subgroup_mem[i]->subgroupID =init_subgroup_id[i];
    mcontext->MU_subgroup_mem[i]->WU_ArmMU_subgroup_thread_mask =  0x08>>(init_subgroup_id[i]%4);
    mcontext->MU_subgroup_mem[i]->wakeupActive = mcontext->wakeupActive;

    //MPRINT("mcontext->MU_subgroup_mem[i]->WU_ArmMU_subgroup_thread_mask=%llx \n",(LLUS)mcontext->MU_subgroup_mem[i]->WU_ArmMU_subgroup_thread_mask);

    for ( j=0; j<BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP; j++ ){
          mcontext->MU_subgroup_mem[i]->batids[j] = j;
          mcontext->MU_subgroup_mem[i]->batvalues[j]=0;
    }
    mcontext->MU_subgroup_mem[i]->batvalues[MCONTEXT_INDEX] = mcontext->mudm_context_phys_addr;
    mcontext->MU_subgroup_mem[i]->batvalues[RDMA_COUNTER_INDEX] = (uint64_t)mcontext->memblock[i].pa_start;
    mcontext->MU_subgroup_mem[i]->batvalues[MCONTEXT_INDEX_VA] = (uint64_t)mcontext;   
    
    activate_base_address_table(&mcontext->MU_subgroup_mem[i]->bat,BGQ_MU_NUM_DATA_COUNTERS_PER_SUBGROUP,mcontext->MU_subgroup_mem[i]->batids,mcontext->MU_subgroup_mem[i]->subgroupID,mcontext->MU_subgroup_mem[i]->batvalues);
    
  }//endfor i
    

  for (i=0;i<NUM_IO_LINKS;i++){  /*BEAM*//*loop doesn't iterate*/
    /* allocate mudm packet (fullsize) use area according to maximum connections */
    alloc_pkt_list_object( mcontext, cfg_info->max_connections , 512, &mcontext->packet_controls_region[i] );
    if (alloc_status){
       MUDM_RAS_INITERR(EPERM);
       goto ALLOC_ERROR;  
    }
    mcontext -> packetcontrols[i] = (struct pkt_controls *)mcontext ->packet_controls_region[i].base_vaddr;
    
  }
  
  for (i=0;i<NUM_IO_LINKS;i++){/*BEAM*//*loop doesn't iterate*/
    /* allocate mudm small memory use area according to maximum connections */
    alloc_pkt_list_object( mcontext, cfg_info->max_connections , 64, &mcontext->smallpa_obj_ctls_mregion[i]);
    if (alloc_status){
       MUDM_RAS_INITERR(EPERM);
       goto ALLOC_ERROR;  
    }
    mcontext-> smallpa_obj_ctls[i] = mcontext->smallpa_obj_ctls_mregion[i].base_vaddr;
    
  }
 
  #define MUDM_MIN_INJ_REGION 512*64
  temp = BGQ_MU_DESCRIPTOR_SIZE_IN_BYTES * cfg_info->max_connections * 4;
  if (temp < MUDM_MIN_INJ_REGION) temp = MUDM_MIN_INJ_REGION;


  for (i=0;i<NUM_IO_LINKS;i++){ /*BEAM*//*loop doesn't iterate*/
    //! \todo pass index fifoid[i] address to plug into *args instead of NULL
    alloc_status = alloc_object(temp ,&mcontext->MU_subgroup_mem[i]->injRegion,
                    mcontext,mcontext->MU_subgroup_mem[i], free_MU_inj_region);
    if (alloc_status){
       MUDM_RAS_INITERR(EPERM);
       goto ALLOC_ERROR;  
    }
      /* init the io inj fifo lock */
    mcontext->injfifo_ctls[i].inj_fifo_lock.atom = 0;
    activate_injfifos( mcontext->MU_subgroup_mem[i]);
  }
   //TORUS_PACKET_SIZE define is 544
   if (cfg_info->max_connections <= 16)
    temp = TORUS_PACKET_SIZE * 64;  
   else
    temp = TORUS_PACKET_SIZE * cfg_info->max_connections * 4; 
  
  for (i=0;i<NUM_IO_LINKS;i++){ /*BEAM*//*loop doesn't iterate*/
    //! \todo pass index fifoid[i] address to plug into *args instead of NULL
    alloc_status = alloc_object(temp,&mcontext->MU_subgroup_mem[i]->recRegion,mcontext,mcontext->MU_subgroup_mem[i], free_recfifo);
    PRINT("i=%d mcontext->MU_subgroup_mem->ififoid[i]=%d \n ",i,mcontext->MU_subgroup_mem[i]->ififoid);
    DUMPHEXSTR("&mcontext->MU_subgroup_mem->recRegion[i] ",&mcontext->MU_subgroup_mem[i]->recRegion,sizeof(struct mudm_memory_region))
    if (alloc_status){
       MUDM_RAS_INITERR(EPERM);
       goto ALLOC_ERROR;  
    }
    activate_recfifos(mcontext->MU_subgroup_mem[i]);
  }//endfor i

  
  alloc_status = conn_list_init(mcontext, cfg_info->max_connections );
  if (alloc_status) return alloc_status;

  
  for (i=0;i<NUM_IO_LINKS;i++){  /*BEAM*//*loop doesn't iterate*/
    mcontext->sys_rec_fifo_id[i] = init_sys_rec_fifo_id[i];
    mcontext->injfifo_ctls[i].injfifo =  MUSPI_IdToInjFifo(0, &(mcontext->MU_subgroup_mem[i]->ififo_subgroup));
    mcontext->injfifo_ctls[i].state = 1; //ACTIVE
    //! \todo TODO change from pointer to injfifo to ctls 
    update_IdToInjFifo(mcontext->packetcontrols[i],  mcontext->injfifo_ctls[i].injfifo);
    update_IdToInjFifo(mcontext->smallpa_obj_ctls[i], mcontext->injfifo_ctls[i].injfifo);
   }


  //activate bcast info 
  bcast_init( mcontext,0);
  


 // save flight recorder info
  complete_init_mudm_flightlog_fmt((void *)mcontext);
  fillin_FlightLogegistyEntry_hiwrap(&cfg_info->mudm_hi_wrap_flight_recorder);
  fillin_FlightLogegistyEntry_nowrap(&cfg_info->mudm_lo_wrap_flight_recorder);
  fillin_FlightLogegistyEntry_hiwrap(&mcontext->mudm_hi_wrap_flight_recorder);
  fillin_FlightLogegistyEntry_nowrap(&mcontext->mudm_no_wrap_flight_recorder);

  mcontext->rdma_EINVAL_source = -EINVAL;
  mcontext->rdma_GOOD_source = 0;


  mcontext->RAS_pacing_timestamp = GetTimeBase2(); 
  mcontext->RAS_pacing_count = 0;

  {
  SoftwareBytes_t  SoftwareBytes;
  SoftwareBytes.ionet_pkt.torus_source_node=mcontext->myTorusAddress.Destination.Destination; 
  SoftwareBytes.ionet_pkt.options=MUDM_NO_IOLINK;
  InitPt2PtMemoryFIFODescriptor( &mcontext->mu_iMemoryFifoDescriptor,
                                               &SoftwareBytes,
                                               0,
                                               0,
                                               mcontext->myTorusAddress,
                                               NODE2NODE, 
                                               0,
                                               mcontext->sys_rec_fifo_id[0]
                                             );
  }
  mcontext->state = MUDM_ACTIVE;
#if 1
  {
  uint64_t no_wrap_entry_num = MUDM_INIT_DONE(&mcontext->mudm_no_wrap_flight_recorder,__LINE__,MUDM_VERSION,NUM_IO_LINKS,mcontext);
  BG_FlightRecorderRegistry_t* logregistry = &mcontext->mudm_no_wrap_flight_recorder;
  BG_FlightRecorderLog_t* logentry = &logregistry->flightlog[no_wrap_entry_num];
  for (i=0;i<NUM_IO_LINKS;i++){  /*BEAM*//*loop doesn't iterate*/
   MUSPI_RecFifo_t * recfifo = MUSPI_IdToRecFifo (0, &mcontext->MU_subgroup_mem[i]->rfifo_subgroup);
   MUSPI_Fifo_t    * fifo_shadow = &recfifo->_fifo;        /**< Software fifo shadow. */
   MUHWI_Fifo_t      *  hwfifo = fifo_shadow->hwfifo;
   void * start_va             = fifo_shadow->va_start;
    
    MUDM_DB_INJF_INFO(&mcontext->mudm_no_wrap_flight_recorder,mcontext->injfifo_ctls[i].injfifo->_fifo.hwfifo,mcontext->injfifo_ctls[i].injfifo->hw_injfifo, mcontext->injfifo_ctls[i].injfifo->_fifo.va_start, init_subgroup_id[i]);
    logentry++;
    //logentry->timestamp |= 0x8000000000000000;
uint32_t init_subgroup_id[2] = {
  66,     /*16*4 + 2  4 subgroups per group, use the third subgroup in the 17th group */
  67      /*16*4 + 3  4 subgroups per group, use the fourth subgroup in the 17th group */
};
uint32_t init_sys_rec_fifo_id[2] = {
  264,     /* 4 * 66 + 0  first in subgroup id 66*/
  268      /* 4 * 67 + 0  first in subgroup id 67*/
};      

#if 1
    MUDM_DB_RECF_INFO(&mcontext->mudm_no_wrap_flight_recorder,hwfifo, start_va, init_subgroup_id[i],init_sys_rec_fifo_id[i]);                      
    logentry++;
    //logentry->timestamp |= 0x8000000000000000;    
#endif 
  }
  for (i=0;i<NUM_IO_LINKS;i++){  /*BEAM*//*loop doesn't iterate*/
    check4notRight( mcontext -> packetcontrols[i],&mcontext->mudm_no_wrap_flight_recorder);
    check4notRight( mcontext-> smallpa_obj_ctls[i],&mcontext->mudm_no_wrap_flight_recorder);
  }
  //MUDM_DM_PERS_INFO(&mcontext->mudm_no_wrap_flight_recorder,mcontext->personality,NUM_IO_LINKS,mcontext->max_connections,mcontext->myTorusAddress);
  
  }
#endif 

//! \todo remove 
#ifndef __LINUX__ 
#if 0
  //count_CN_in_block(mcontext);
  largest_node_corner_in_block(mcontext);
#endif
#endif


return 0;
 

//! \todo put init error handling here to call cleanup routines, etc.
ALLOC_ERROR:
  MUDM_RAS_INITERR(EPERM);
  /* need to free allocated storage */
  temp = process_cleanup_table(mcontext);
  MPRINT("\n ==== MUDM INIT FAILED====\n");
return -ENOMEM;
}
EXPORT_SYMBOL(mudm_init);

