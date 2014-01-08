-- begin_generated_IBM_copyright_prolog                             --
--                                                                  --
-- This is an automatically generated copyright prolog.             --
-- After initializing,  DO NOT MODIFY OR MOVE                       --
-- ================================================================ --
--                                                                  --
-- Licensed Materials - Property of IBM                             --
--                                                                  --
-- Blue Gene/Q                                                      --
--                                                                  --
-- (C) Copyright IBM Corp.  2004, 2011                              --
--                                                                  --
-- US Government Users Restricted Rights -                          --
-- Use, duplication or disclosure restricted                        --
-- by GSA ADP Schedule Contract with IBM Corp.                      --
--                                                                  --
-- This software is available to you under the                      --
-- Eclipse Public License (EPL).                                    --
--                                                                  --
-- ================================================================ --
--                                                                  --
-- end_generated_IBM_copyright_prolog                               --
--
--  DESCRIPTION: defines triggers for the BG/Q schema
--
-- -----------------------------------------------------------------------


--   check_status
--   make sure block is free before its deleted
-- -----------------------------------------------------------------------


create trigger check_status 
  no cascade before delete on tbgqblock 
  referencing old as old 
  for each row mode db2sql 
  when (old.status <> 'F') 
 begin atomic 
  SIGNAL SQLSTATE '70001' ('Invalid block state'); 
 end @


--  Trigger to create block history 

create trigger block_history
after delete on tbgqblock
referencing old as old
for each row mode db2sql

 begin atomic 

 insert into tbgqblock_history ( blockId, numCnodes, numIOnodes  ,owner  ,
   isTorus ,sizeA  ,sizeB ,sizeC  ,sizeD , sizeE,
   description, options, 
   statusLastModified  ,mloaderImg ,nodeConfig , bootOptions , 
   createDate , entryDate , errtext, creationId          )
     
   values
    (old.blockId,old.numCnodes,old.numIOnodes,old.owner,old.isTorus,
     old.sizeA,old.sizeB,old.sizeC,old.sizeD,old.sizeE,
     old.description,old.options,old.statusLastModified,
     old.mloaderImg,old.nodeConfig,old.bootOptions,
     old.createDate,CURRENT TIMESTAMP,old.errtext,old.creationId);

 insert into tbgqioblockmap_history (blockId,creationId,location)
 select blockId,old.creationId,location from tbgqioblockmap
 where blockid = old.blockid;

 delete from tbgqioblockmap
 where blockid = old.blockid;


 insert into tbgqbpblockmap_history (bpId,blockId,creationId,Acoord,Bcoord,Ccoord,Dcoord)
 select bpId,blockId,old.creationId,Acoord,Bcoord,Ccoord,Dcoord from tbgqbpblockmap
 where blockid = old.blockid;

 delete from tbgqbpblockmap
 where blockid = old.blockid;

 insert into  TBGQSmallBlock_history (blockId, creationId, posInMachine, NodeCardPos)
 select blockId,old.creationId, posInMachine, NodeCardPos from tbgqsmallblock
 where blockid = old.blockid;

 delete from tbgqsmallblock
 where blockid = old.blockid;
   
 insert into  TBGQLinkBlockMap_history (linkId,blockId,creationId)
 select  linkId,blockId,old.creationId from TBGQLinkBlockMap
  where blockid = old.blockid;

 delete from TBGQLinkBlockMap
  where blockid = old.blockid;

 insert into  TBGQSwitchBlockMap_history (blockId,switchId, creationId,include,enableports ) 
 select blockId,switchId, creationId,include, enableports from TBGQSwitchBlockMap
 where blockid = old.blockid;

 delete from TBGQSwitchBlockMap
 where blockid = old.blockid; 

 end @

--  Trigger to prevent image changes while block is in use

create trigger block_upd_images
no cascade before update on tbgqblock
referencing new as newrow old as oldrow
for each row mode db2sql
when (
     (newrow.nodeconfig <> oldrow.nodeconfig OR
      newrow.mloaderImg <>  oldrow.mloaderImg) AND
      (newrow.status <> 'F' OR oldrow.status <> 'F')
     )

 begin atomic 
  SIGNAL SQLSTATE '70001' ('Invalid block state'); 
 end @

-- trigger to update job timestamp on update


create trigger job_timestamp
  no cascade before update of status on tbgqjob
  referencing new as newrow old as oldrow
  for each row mode db2sql
  when (
    newrow.status <> oldrow.status 
  )

 begin atomic
  set newrow.statusLastModified = current timestamp;
 
 end @

-- trigger to update block timestamp on update

create trigger block_timestamp
  no cascade before update of status on tbgqblock
  referencing new as newrow old as oldrow
  for each row mode db2sql
  when (
    newrow.status <> oldrow.status
  )

 begin atomic
  set newrow.statusLastModified = current timestamp;
 end @

-- trigger to insert block action history on update
create trigger block_action_history
  after update of action on tbgqblock
  referencing new as n old as o
  for each row mode db2sql
  when (
    n.action <> o.action
  )

  begin atomic
    insert into tbgqblockaction_history (blockid,status,action,creationId) values
    (n.blockid,n.status,n.action,n.creationid);
 end @


 
-- trigger to remove I/O usage rows when block goes to Free

create trigger block_io_usage
  after update of status on tbgqblock
  referencing new as newrow old as oldrow
  for each row mode db2sql
  when (
      newrow.status = 'F'
  )
  begin atomic
    delete from TBGQIoUsage where blockId = oldrow.blockId;
  end @


-- history triggers

create trigger machine_history_u    
  after update on tbgqmachine
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 
   insert into tbgqmachine_history 
      (serialNumber,productId,description, status, hasEthernetGateway, 
         bgsysIpv4Address, bgsysIpv6Address,bgsysRemotePath,bgsysMountOptions,fileSystemType,
         mtu,memorymodulesize,clockHz,snIpv4Address,snIpv6Address,bringupOptions,distroIpv4Address,
         distroIpv6Address,distroRemotePath,distroMountOptions)
   values
      (n.serialNumber,n.productId,n.description,n.status, n.hasEthernetGateway, 
       n.bgsysIpv4Address,n.bgsysIpv6Address,n.bgsysRemotePath,n.bgsysMountOptions,n.fileSystemType,
       n.mtu,n.memorymodulesize,n.clockHz,n.snIpv4Address,n.snIpv6Address,n.bringupOptions,n.distroIpv4Address,
       n.distroIpv6Address,n.distroRemotePath,n.distroMountOptions);
  end @

create trigger machine_history_i
  after insert on tbgqmachine
  referencing new as n 
  for each row mode db2sql

  begin atomic 
   insert into tbgqmachine_history 
      (serialNumber,productId,description, status, hasEthernetGateway, 
         bgsysIpv4Address, bgsysIpv6Address,bgsysRemotePath,bgsysMountOptions,fileSystemType,
         mtu,memorymodulesize,clockHz,snIpv4Address,snIpv6Address,bringupOptions,distroIpv4Address,
         distroIpv6Address,distroRemotePath,distroMountOptions, action)
   values
      (n.serialNumber,n.productId,n.description,n.status, n.hasEthernetGateway, 
       n.bgsysIpv4Address,n.bgsysIpv6Address,n.bgsysRemotePath,n.bgsysMountOptions,n.fileSystemType,
       n.mtu,n.memorymodulesize,n.clockHz,n.snIpv4Address,n.snIpv6Address,n.bringupOptions,n.distroIpv4Address,
       n.distroIpv6Address,n.distroRemotePath,n.distroMountOptions, 'I');
  end @


create trigger machine_history_d
  before delete on tbgqmachine
  referencing old as n
  for each row mode db2sql

  begin atomic 

    SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
 end @                                                                                                   

-- -----------------------------------------------------------------------

create trigger midplane_history_u
  after update on tbgqmidplane
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

    if (o.posinmachine = n.posinmachine) then

    if ((n.status = 'F') or (o.status = 'F' and n.status = 'A') or (o.seqid <> n.seqid)) then
      -- omit insertions for Software Failure transitions
      -- or when sequence ID has changed from another trigger
    else
    insert into tbgqmidplane_history 
      (serialNumber, productId, machineSerialNumber, posInMachine, status, ismaster, vpd)
     values
      (n.serialNumber, n.productId, n.machineSerialNumber, n.posInMachine, n.status, n.ismaster, n.vpd);

     if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ('Midplane', n.posInMachine,o.serialnumber,n.serialNumber, o.status, n.status);
        
     end if;
     end if;

    else

     SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

    end if;

   end @


create trigger midplane_history_i
  after insert on tbgqmidplane
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqmidplane_history 
      (serialNumber, productId, machineSerialNumber, posInMachine, status, ismaster, vpd, action)
   values
      (n.serialNumber, n.productId, n.machineSerialNumber, n.posInMachine, n.status, n.ismaster, vpd,  'I');
  end @

create trigger midplane_history_d
  before delete on tbgqmidplane
  referencing old as o
  for each row mode db2sql

  begin atomic 
   SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  end @

-- -----------------------------------------------------------------------

create trigger nc_history_u
  after update on tbgqnodecard
  referencing new as n  old as o
  for each row mode db2sql

  begin atomic 

   if ((o.midplanepos = n.midplanepos) and (o.position = n.position)) then

    insert into tbgqnodecard_history 
      (serialNumber, productId, midplanepos, position, status, vpd)
    values
      (n.serialNumber, n.productId, n.midplanepos, n.position, n.status, n.vpd);

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'NodeCard',  n.midplanepos || '-' || n.position,o.serialnumber,n.serialNumber, o.status, n.status);
        
    end if;

   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;
    
  end @


create trigger nc_history_i
  after insert on tbgqnodecard
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqnodecard_history 
      (serialNumber, productId, midplanepos, position, status, vpd, action)
     values 
      (n.serialNumber, n.productId, n.midplanepos, n.position, n.status, n.vpd, 'I');
  end @

create trigger nc_history_d
  before delete on tbgqnodecard
  referencing old as o
  for each row mode db2sql

  begin atomic 

        SIGNAL SQLSTATE '70003' ('Deletion not permitted');

  end @


-- -----------------------------------------------------------------------

create trigger node_history_u
  after update on tbgqnode
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.midplanepos = n.midplanepos) and (o.nodecardpos = n.nodecardpos) and (o.position = n.position)) then

   if ((n.status = 'F') or (o.status = 'F' and n.status = 'A') or (o.seqid <> n.seqid)) then
     -- omit insertions for Software Failure transitions
     -- or when sequence ID has changed from another trigger
   else
   insert into tbgqnode_history 
      (serialNumber, productId, ecid, midplanepos,nodecardpos,  position, ipAddress, macaddress, status, memoryModuleSize, memorySize, psro, vpd, voltage)
   values
      (n.serialNumber, n.productId, n.ecid, n.midplanepos, n.nodecardpos, n.position, n.ipAddress, n.macaddress, n.status, n.memoryModuleSize, n.memorySize, n.psro, n.vpd, n.voltage) ;

    if ((o.serialnumber <> n.serialnumber) OR (o.ecid <> n.ecid)) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldecid, newecid, oldstatus, newstatus)
     values
      ( 'Node', n.midplanepos || '-' || n.nodecardpos || '-' || n.position,o.serialnumber,n.serialNumber,o.ecid,n.ecid, o.status, n.status);

     if (n.bitsteering <> -1) then    
      update tbgqnode set bitsteering = -1 where  midplanepos = n.midplanepos and nodecardpos = n.nodecardpos and position = n.position;
     end if;
       
    end if;
   end if;
 
   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @


create trigger node_history_i
  after insert on tbgqnode
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqnode_history 
      (serialNumber, productId, ecid, midplanepos,nodecardpos, position, ipAddress, macaddress, status, memoryModuleSize, memorySize, psro, vpd, voltage, action)
   values
      (n.serialNumber,n.productId, n.ecid, n.midplanepos, n.nodecardpos,n.position,n.ipAddress,n.macaddress,n.status,n.memoryModuleSize,n.memorySize, n.psro, n.vpd, n.voltage,  'I');
end @


create trigger node_history_d
  before delete on tbgqnode
  referencing old as o
  for each row mode db2sql

  begin atomic 

  SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
  end @


-- -----------------------------------------------------------------------

create trigger ionode_history_u
  after update on tbgqionode
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.iopos = n.iopos) and (o.position = n.position)) then

   insert into tbgqionode_history 
      (serialNumber, productId, ecid, iopos,  position, ipAddress, macaddress, status, memoryModuleSize, memorySize, psro, vpd, voltage)
   values
      (n.serialNumber, n.productId, n.ecid, n.iopos, n.position, n.ipAddress, n.macaddress, n.status, n.memoryModuleSize, n.memorySize, n.psro, n.vpd, n.voltage) ;

    if ((o.serialnumber <> n.serialnumber) OR (o.ecid <> n.ecid)) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldecid, newecid, oldstatus, newstatus)
     values
      ( 'IONode', n.iopos || '-' || n.position,o.serialnumber,n.serialNumber,o.ecid,n.ecid, o.status, n.status);
       
    end if;
 
   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @


create trigger ionode_history_i
  after insert on tbgqionode
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqionode_history 
      (serialNumber, productId, ecid, iopos, position, ipAddress, macaddress, status, memoryModuleSize, memorySize, psro, vpd, voltage, action)
   values
      (n.serialNumber,n.productId, n.ecid, n.iopos, n.position,n.ipAddress,n.macaddress,n.status,n.memoryModuleSize,n.memorySize, n.psro, n.vpd, n.voltage,  'I');
end @


create trigger ionode_history_d
  before delete on tbgqionode
  referencing old as o
  for each row mode db2sql

  begin atomic 

  SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
  end @



-- -----------------------------------------------------------------------

create trigger iorack_history_u
  after update on tbgqiorack
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if (o.location = n.location) then

   insert into tbgqiorack_history 
      (serialNumber, productId, machineserialnumber, location, status)
   values
      (n.serialNumber, n.productId, n.machineserialnumber, n.location, n.status) ;

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'IORack', n.location,o.serialnumber,n.serialNumber, o.status, n.status);
       
    end if;
 
   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @


create trigger iorack_history_i
  after insert on tbgqiorack
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqiorack_history 
      (serialNumber, productId,  machineserialnumber, location, status, action)
   values
      (n.serialNumber,n.productId,n.machineserialnumber,n.location,n.status,'I');
end @


create trigger iorack_history_d
  before delete on tbgqiorack
  referencing old as o
  for each row mode db2sql

  begin atomic 

  SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
  end @


-- -----------------------------------------------------------------------

create trigger ioenc_history_u
  after update on tbgqiodrawer
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if (o.location = n.location) then

   insert into tbgqiodrawer_history 
      (serialNumber, productId, location, status, vpd)
   values
      (n.serialNumber, n.productId, n.location, n.status, n.vpd) ;

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'IODrawer', n.location,o.serialnumber,n.serialNumber, o.status, n.status);
       
    end if;
 
   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @


create trigger ioenc_history_i
  after insert on tbgqiodrawer
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqiodrawer_history 
      (serialNumber, productId,  location, status, vpd,  action)
   values
      (n.serialNumber,n.productId, n.location,n.status,n.vpd,'I');
end @


create trigger ioenc_history_d
  before delete on tbgqiodrawer
  referencing old as o
  for each row mode db2sql

  begin atomic 

  SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
  end @


-- -----------------------------------------------------------------------

create trigger sc_history_u
  after update on tbgqservicecard
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 
   if (o.midplanepos = n.midplanepos)  then

    insert into tbgqservicecard_history 
      (serialNumber, productId, midplanepos, status, vpd)
    values
      (n.serialNumber, n.productId, n.midplanepos, n.status, n.vpd);

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'ServiceCard',  n.midplanepos || '-S', o.serialnumber, n.serialNumber, o.status, n.status);
        
    end if;

   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @


create trigger sc_history_i
  after insert on tbgqservicecard
  referencing new as n
  for each row mode db2sql

  begin atomic 
 
   insert into tbgqservicecard_history 
      (serialNumber, productId, midplanepos, status,vpd, action)
   values
      (n.serialNumber, n.productId, n.midplanepos, n.status, n.vpd, 'I');
  
  end @


create trigger sc_history_d
  before delete on tbgqservicecard
  referencing old as o
  for each row mode db2sql

  begin atomic 

 SIGNAL SQLSTATE '70003' ('Deletion not permitted');
 
  end @


-- -----------------------------------------------------------------------



create trigger cc_history_u
  after update on tbgqclockcard
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

  if (o.location = n.location) then

   insert into tbgqclockcard_history 
      (serialNumber, productId, location, clockHz, status, vpd)
   values
      (n.serialNumber, n.productId, n.location, n.clockHz, n.status, n.vpd);

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'ClockCard',  n.location,o.serialnumber,n.serialNumber, o.status, n.status);
        
    end if;

  else

   SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

  end if;
  end @

create trigger cc_history_i
  after insert on tbgqclockcard
  referencing new as n
  for each row mode db2sql

  begin atomic 
     insert into tbgqclockcard_history 
        (serialNumber, productId, location, clockhz, status, vpd, action)
      values
        (n.serialNumber, n.productId, n.location, n.clockHz, n.status, n.vpd, 'I');
  end @

create trigger cc_history_d
  before delete on tbgqclockcard
  referencing old as o
  for each row mode db2sql

  begin atomic 
  SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  end @

-- -----------------------------------------------------------------------

create trigger linkchip_history_u
  after update on tbgqlinkchip
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.midplanepos = n.midplanepos) and (o.nodecardpos = n.nodecardpos) and (o.position = n.position)) then

   insert into tbgqlinkchip_history 
      (serialNumber, productId, ecid, midplanepos, nodecardpos, position, status)
   values
      (n.serialNumber, n.productId, n.ecid, n.midplanepos, n.nodecardpos, n.position, n.status);

    if ((o.serialnumber <> n.serialnumber) OR (o.ecid <> n.ecid)) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber,oldecid,newecid, oldstatus, newstatus)
     values
      ( 'LinkChip',   n.midplanepos || '-' || n.nodecardpos || '-' || n.position,o.serialnumber,n.serialNumber,o.ecid,n.ecid, o.status, n.status);
        
    end if;

   else

   SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @

create trigger linkchip_history_i
  after insert on tbgqlinkchip
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqlinkchip_history
      (serialNumber, productId, ecid, midplanepos, nodecardpos, position, status, action)
   values
      (n.serialNumber, n.productId, n.ecid, n.midplanepos, n.nodecardpos, n.position, n.status,'I');
  end @


-- -----------------------------------------------------------------------

create trigger iolc_history_u
  after update on tbgqiolinkchip
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.iopos = n.iopos) and (o.position = n.position)) then

   insert into tbgqiolinkchip_history 
      (serialNumber, productId, ecid, iopos, position, status)
   values
      (n.serialNumber, n.productId, n.ecid, n.iopos, n.position, n.status);

    if ((o.serialnumber <> n.serialnumber) OR (o.ecid <> n.ecid)) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber,oldecid,newecid, oldstatus, newstatus)
     values
      ( 'IOLinkChip',   n.iopos || '-' || n.position,o.serialnumber,n.serialNumber,o.ecid,n.ecid, o.status, n.status);
        
    end if;

   else

   SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @

create trigger iolc_history_i
  after insert on tbgqiolinkchip
  referencing new as n
  for each row mode db2sql

  begin atomic 
   insert into tbgqiolinkchip_history
      (serialNumber, productId, ecid, iopos, position, status, action)
   values
      (n.serialNumber, n.productId, n.ecid, n.iopos, n.position, n.status,'I');
  end @


create trigger iolc_history_d
  before delete on tbgqiolinkchip
  referencing old as o
  for each row mode db2sql

  begin atomic 
 SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
  end @

-- -----------------------------------------------------------------------

create trigger icon_history_u
  after update on tbgqicon
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 
  if (o.containerlocation = n.containerlocation) then
   insert into tbgqicon_history 
       ( licensePlate, containerlocation, ipAddress,
         iConId         ,   iConChecksum   ,   iConLogicRev  ,   iConBuildDate  ,
         iConBuildNumber,   PalominoId      ,   PalominoChecksum  ,   PalominoLogicRev  ,
         PalominoBuildDate  ,   PalominoBuildNumber , status)
   values
       ( n.licensePlate, n.containerlocation, n.ipAddress,
         n.iConId         , n.iConChecksum   , n.iConLogicRev  , n.iConBuildDate  ,
         n.iConBuildNumber, n.PalominoId      , n.PalominoChecksum  , n.PalominoLogicRev  ,
         n.PalominoBuildDate  ,   n.PalominoBuildNumber , n.status)     ;
   else
 SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;
  end @

create trigger icon_history_i
  after insert on tbgqicon
  referencing new as n
  for each row mode db2sql

  begin atomic 
 insert into tbgqicon_history 
       ( licensePlate, containerlocation, ipAddress,
         iConId         ,   iConChecksum   ,   iConLogicRev  ,   iConBuildDate  ,
         iConBuildNumber,   PalominoId      ,   PalominoChecksum  ,   PalominoLogicRev  ,
         PalominoBuildDate  ,   PalominoBuildNumber , status, action)
   values
       ( n.licensePlate, n.containerlocation, n.ipAddress,
         n.iConId         , n.iConChecksum   , n.iConLogicRev  , n.iConBuildDate  ,
         n.iConBuildNumber, n.PalominoId      , n.PalominoChecksum  , n.PalominoLogicRev  ,
         n.PalominoBuildDate  ,   n.PalominoBuildNumber , n.status, 'I')     ;
  
  end @

create trigger icon_history_d
  before delete on tbgqicon
  referencing old as o
  for each row mode db2sql

  begin atomic 

 SIGNAL SQLSTATE '70003' ('Deletion not permitted');

 
  end @

-- -----------------------------------------------------------------------


create trigger bp_history_u
  after update on tbgqbulkpowersupply
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

  if (o.location = n.location) then

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'BulkPower', n.location,o.serialnumber,n.serialNumber, o.status, n.status);
        
    end if;

  else

   SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

  end if;

  end @

create trigger dca_history_u
  after update on tbgqnodecarddca
  referencing new as n old as o
  for each row mode db2sql

  begin atomic

  if ((o.midplanepos = n.midplanepos) and (o.nodecardpos = n.nodecardpos) and (o.position = n.position)) then    

    if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ( 'NodeBoardDCA', n.midplanepos || '-' || n.nodecardpos || '-' || n.position,o.serialnumber,n.serialNumber, o.status, n.status);

    end if;

  else

   SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

  end if;

  end @


create trigger cable_history_u
  after update on tbgqcable
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.fromlocation = n.fromlocation) and (o.tolocation = n.tolocation)) then

     if (o.status <> n.status) then
       insert into tbgqcable_history (fromlocation,tolocation,status,seqid,badwiremask)
       values (n.fromlocation,n.tolocation,n.status,n.seqid,n.badwiremask);
     end if;
   else

     SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end @


create trigger cable_history_d
  before delete on tbgqcable
  referencing old as n
  for each row mode db2sql

  begin atomic 

    SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
 end @                                                                                                   


-- - Triggers for real-time START ----------------------------------------

CREATE TRIGGER RT_BLOCK_ADD
  AFTER INSERT ON TBGQBlock
  REFERENCING NEW AS newrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQBlock SET seqId = nextSeqId WHERE blockId = newrow.blockId;
  END @


CREATE TRIGGER RT_BLOCK_STATE
  AFTER UPDATE OF status ON TBGQBlock
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQBlock SET seqId = nextSeqId WHERE blockId = newrow.blockId;
  END @


CREATE TRIGGER RT_JOB_ADD
  AFTER INSERT ON TBGQJob
  REFERENCING NEW AS newrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQJob SET seqId = nextSeqId WHERE id = newrow.id;
  END @


CREATE TRIGGER RT_JOB_STATE
  AFTER UPDATE OF status ON TBGQJob
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQJob SET seqId = nextSeqId WHERE id = newrow.id;
   END @


CREATE TRIGGER RT_MIDPLANE_STATE
  AFTER UPDATE OF status ON TBGQMidplane
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQMidplane SET seqId = nextSeqId WHERE posInMachine = newrow.posInMachine;
  END @


CREATE TRIGGER RT_NODEBOARD_STATE
  AFTER UPDATE OF status ON TBGQNodeCard
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQNodeCard SET seqId = nextSeqId WHERE position = newrow.position AND midplanePos = newrow.midplanePos;
  END @

CREATE TRIGGER RT_NODE_STATE
  AFTER UPDATE OF status ON TBGQNode
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQNode SET seqId = nextSeqId WHERE position = newrow.position AND nodecardPos = newrow.nodecardPos AND midplanePos = newrow.midplanePos;
  END @

 
CREATE TRIGGER RT_SWITCH_STATE
  AFTER UPDATE OF status ON TBGQSwitch
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQSwitch SET seqId = nextSeqId WHERE switchid = newrow.switchid AND midplanePos = newrow.midplanePos;
  END @
   

CREATE TRIGGER RT_CABLE_STATE
  AFTER UPDATE OF status ON TBGQCable
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQCable SET seqId = nextSeqId
     WHERE FromLocation = newrow.FromLocation AND
           ToLocation   = newrow.ToLocation ;
  END @


CREATE TRIGGER RT_IONODE_STATE
  AFTER UPDATE OF status ON TBGQIONode
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQIONode SET seqId = nextSeqId
     WHERE IOPos = newrow.IOPos AND
           position = newrow.position ;
  END @


CREATE TRIGGER RT_IODRAWER_STATE
  AFTER UPDATE OF status ON TBGQIODrawer
  REFERENCING NEW AS newrow OLD AS oldrow
  FOR EACH ROW MODE DB2SQL
  BEGIN ATOMIC
    DECLARE nextSeqId BIGINT;
    SET nextSeqId = NEXT VALUE FOR SEQID;
    UPDATE TBGQIODrawer SET seqId = nextSeqId
     WHERE location = newrow.location ;
  END @


-- - Triggers for real-time END ------------------------------------------


commit @
