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
--  DESCRIPTION: creates BGQ database tables
--
CREATE SEQUENCE SEQID AS BIGINT START WITH 1 INCREMENT BY 1 NOMAXVALUE NOCYCLE CACHE 1000 ORDER;

CREATE TABLE TBGQBlock
(
   blockId               char(32)       NOT NULL,
   numCnodes             integer        NOT NULL WITH DEFAULT 0,
   numIOnodes            integer        NOT NULL WITH DEFAULT 0,
        CONSTRAINT BGQBlock_nodechk check ((numcnodes > 0 ) or (numionodes > 0)),
   owner                 char(32)       NOT NULL,
   userName              char(32),
   isTorus               char(5),
   sizeA                 integer        NOT NULL WITH DEFAULT 0,
   sizeB                 integer        NOT NULL WITH DEFAULT 0,
   sizeC                 integer        NOT NULL WITH DEFAULT 0,
   sizeD                 integer        NOT NULL WITH DEFAULT 0,
   sizeE                 integer        NOT NULL WITH DEFAULT 0,
   description           varchar(1024),
   options               char(16)       NOT NULL WITH DEFAULT ' ',
   status                char(1)        NOT NULL WITH DEFAULT 'F',
          CONSTRAINT BGQBlock_Statchk CHECK ( status IN ('F', 'A', 'B', 'I', 'T') ),
     -- (F)ree, (A)llocated, (B)ooting, (I)nitialized, (T)erminating
   action                char(1)        NOT NULL WITH DEFAULT ' ',
          CONSTRAINT BGQBlock_actchk CHECK ( action IN ('B', 'D', 'N', ' ') ),
     -- (B)oot, (D)ellocate, (N)o-check option on boot of I/O block
   statusLastModified    timestamp      DEFAULT current timestamp,
   mloaderImg            varchar(256),
   nodeConfig            CHAR(32)       NOT NULL,     
   bootOptions           varchar(256),
   createDate            timestamp      NOT NULL WITH DEFAULT current timestamp,
   securityKey           char(32)       FOR BIT DATA,
   errtext               varchar(256),
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   creationId            integer  GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1),
   QUALIFIER             char(32),
     -- boot cookie
   CONSTRAINT BGQBlock_pk PRIMARY KEY (blockId)
) DATA CAPTURE CHANGES;
CREATE ALIAS  BGQBlock FOR TBGQBlock;

CREATE VIEW BGQIOBlock AS SELECT blockid, numIOnodes, owner, username,
   description, options, status, action,
   statuslastmodified, mloaderimg,nodeconfig,bootoptions,createdate,
   securitykey,errtext,seqid,creationid
  from TBGQBlock where numCnodes = 0;

CREATE VIEW BGQComputeBlock AS SELECT blockid, numCnodes, owner, username, istorus, 
   sizeA, sizeB, sizeC, sizeD, sizeE, description, options, status,
   statuslastmodified, mloaderimg,nodeconfig,bootoptions,createdate,
   securitykey,errtext,seqid,creationid
  from TBGQBlock where numIOnodes = 0;
     
CREATE TABLE TBGQNodeConfig
(
   NodeConfig   char(32)        NOT NULL,
   RASPolicy    char(32),         
   NodeOptions  varchar(512),     
   CONSTRAINT BGQNodeConfig_pk PRIMARY KEY (NodeConfig)
);
CREATE ALIAS  BGQNodeConfig FOR TBGQNodeConfig;

ALTER TABLE tbgqblock 
   add CONSTRAINT BGQBlock_fk FOREIGN KEY (NodeConfig)   REFERENCES TBGQNodeConfig (NodeConfig) ON DELETE RESTRICT;


   -- How many computes are using the bridge compute node for I/O for the block.
   -- Also see the BGQIoUsage view which includes the I/O node.
CREATE TABLE TBGQIoUsage
(
   blockId CHAR(32) NOT NULL,
   computeNode CHAR(14) NOT NULL,
   count INTEGER NOT NULL,

   CONSTRAINT BGQBlock_pk PRIMARY KEY (blockId,computeNode)
);



CREATE TABLE TBGQDomainMap
(
    NodeConfig   char(32)       NOT NULL,
    DomainId     char(32)       NOT NULL,
    StartCore    integer        NOT NULL,
    EndCore      integer        NOT NULL,
    StartAddr    BIGINT         NOT NULL,
    EndAddr      BIGINT         NOT NULL,
    DomainImg    varchar(1024)  NOT NULL,
    DomainOptions  varchar(512) , 
    CustAddr     BIGINT         NOT NULL WITH DEFAULT 17179803648,

 CONSTRAINT BGQDomainMap_pk PRIMARY KEY (NodeConfig,DomainId),
 CONSTRAINT BGQDomainMap_fk FOREIGN KEY (NodeConfig)
       REFERENCES TBGQNodeConfig (NodeConfig) ON DELETE CASCADE
);
CREATE ALIAS  BGQDomainMap  FOR TBGQDomainMap;

CREATE TABLE TBGQIOBlockMap
(
   blockId               char(32)       NOT NULL,
   location              char(11)       NOT NULL,
   CONSTRAINT BGQIOBlock_pk PRIMARY KEY (blockId,location)
);
CREATE ALIAS BGQIOBlockMap for TBGQIOBlockMap;


CREATE TABLE TBGQBPBlockMap
(
   bpId                  char(16)       NOT NULL,
   blockId               char(32)       NOT NULL,
   Acoord                integer        NOT NULL WITH DEFAULT 0,
   Bcoord                integer        NOT NULL WITH DEFAULT 0,
   Ccoord                integer        NOT NULL WITH DEFAULT 0,
   Dcoord                integer        NOT NULL WITH DEFAULT 0,
   CONSTRAINT BGQBPBlockMap_pk PRIMARY KEY (bpId, blockId)
);
CREATE ALIAS BGQBPBlockMap for TBGQBPBlockMap;


CREATE TABLE TBGQSmallBlock
(
   blockId               char(32)       NOT NULL,
   posInMachine          char(6)        NOT NULL,
   NodeCardPos           char(3)        NOT NULL,
   CONSTRAINT BGQSmBlock_pk PRIMARY KEY (blockId,posinmachine,nodecardpos)
);
CREATE ALIAS BGQSmallBlock for TBGQSmallBlock;



CREATE TABLE TBGQLinkBlockMap
(
   linkId                char(32)       NOT NULL,
   blockId               char(32)       NOT NULL,
   CONSTRAINT BGQLnkBlockMap_pk PRIMARY KEY (linkId, blockId)
);
CREATE ALIAS BGQLinkBlockMap for TBGQLinkBlockMap;

CREATE TABLE TBGQSwitchBlockMap
(
   blockId               char(32)       NOT NULL,
   switchId              char(16)       NOT NULL,
   include               char(1)        NOT NULL WITH DEFAULT 'T',
    CONSTRAINT BGQSwBlock_chk CHECK ( include IN ('T', 'F') ),
   enablePorts           char(1)        NOT NULL WITH DEFAULT 'B',
    CONSTRAINT BGQSwPort_chk  CHECK ( enableports IN ('B', 'I', 'O') ),
--   (B)oth   or  (I)nput Only  or (O)utput Only  (I and O are for mesh blocks)
   CONSTRAINT BGQSwBlockMap_pk PRIMARY KEY (blockId,switchid)
);
CREATE ALIAS BGQSwitchBlockMap for TBGQSwitchBlockMap;


CREATE TABLE TBGQBlock_history
(
   blockId               char(32)       NOT NULL,
   numCnodes             integer        NOT NULL WITH DEFAULT 0,
   numIOnodes            integer        NOT NULL WITH DEFAULT 0,
   owner                 char(32),
   isTorus               char(5),
   sizeA                 integer        NOT NULL WITH DEFAULT 0,
   sizeB                 integer        NOT NULL WITH DEFAULT 0,
   sizeC                 integer        NOT NULL WITH DEFAULT 0,
   sizeD                 integer        NOT NULL WITH DEFAULT 0,
   sizeE                 integer        NOT NULL WITH DEFAULT 0,
   description           varchar(1024),
   options               char(16)       NOT NULL WITH DEFAULT ' ',
   statusLastModified    timestamp      DEFAULT current timestamp,
   mloaderImg            varchar(256),
   nodeConfig            char(32),
   bootOptions           varchar(256),
   createDate            timestamp      NOT NULL WITH DEFAULT current timestamp,
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   errtext               varchar(256),
   creationId            integer        NOT NULL, 
   CONSTRAINT BGQBlockh_pk PRIMARY KEY (blockId, creationId )
);
CREATE ALIAS  BGQBlock_history FOR TBGQBlock_history;


CREATE TABLE TBGQIOBlockMap_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   blockId               char(32)       NOT NULL,
   creationId            integer        NOT NULL, 
   location              char(10)       NOT NULL,
   CONSTRAINT BGQIOBMaph_pk PRIMARY KEY (blockId,creationId,location),
   CONSTRAINT BGQIOBMaph_fk FOREIGN KEY (blockId,creationId)
       REFERENCES TBGQBlock_history (blockId,creationId) ON DELETE CASCADE
);
CREATE ALIAS BGQIOBlockMap_history for TBGQIOBlockMap_history;


CREATE TABLE TBGQBPBlockMap_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   bpId                  char(16)       NOT NULL,
   blockId               char(32)       NOT NULL,
   creationId            integer        NOT NULL, 
   Acoord                integer        NOT NULL WITH DEFAULT 0,
   Bcoord                integer        NOT NULL WITH DEFAULT 0,
   Ccoord                integer        NOT NULL WITH DEFAULT 0,
   Dcoord                integer        NOT NULL WITH DEFAULT 0,
   CONSTRAINT BGQBPBMaph_pk PRIMARY KEY (bpId, blockId,creationId),
   CONSTRAINT BGQBPBMaph_fk FOREIGN KEY (blockId,creationId)
       REFERENCES TBGQBlock_history (blockId,creationId) ON DELETE CASCADE
);
CREATE ALIAS BGQBPBlockMap_history for TBGQBPBlockMap_history;


CREATE TABLE TBGQSmallBlock_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   blockId               char(32)       NOT NULL,
   creationId            integer        NOT NULL, 
   posInMachine          char(6)        NOT NULL,
   NodeCardPos           char(3)        NOT NULL,
    CONSTRAINT BGQSBh_fk FOREIGN KEY (blockId,creationId)
       REFERENCES TBGQBlock_history (blockId,creationId) ON DELETE CASCADE
);
CREATE ALIAS BGQSmallBlock_history for TBGQSmallBlock_history;


CREATE TABLE TBGQLinkBlockMap_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   linkId                char(32)       NOT NULL,
   blockId               char(32)       NOT NULL,
   creationId            integer        NOT NULL, 
   CONSTRAINT BGQLnkBMaph_pk PRIMARY KEY (linkId, blockId,creationId),
   CONSTRAINT BGQLnkBMaph_fk FOREIGN KEY (blockId,creationId)
       REFERENCES TBGQBlock_history(blockId,creationId) ON DELETE CASCADE
);
CREATE ALIAS BGQLinkBlockMap_history for TBGQLinkBlockMap_history;

CREATE TABLE TBGQSwitchBlockMap_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   blockId               char(32)       NOT NULL,
   switchId              char(16)       NOT NULL,
   creationId            integer        NOT NULL, 
   include               char(1)        NOT NULL WITH DEFAULT 'T',
   enablePorts           char(1)        NOT NULL WITH DEFAULT 'B',
   CONSTRAINT BGQSwBMaph_pk PRIMARY KEY (blockId,switchid,creationId),
   CONSTRAINT BGQSwBMaph_fk FOREIGN KEY (blockId,creationId)
       REFERENCES TBGQBlock_history(blockId,creationId) ON DELETE CASCADE
);
CREATE ALIAS BGQSwitchBlockMap_history for TBGQSwitchBlockMap_history;




CREATE TABLE TBGQProductType
(
  productId              char(16)       NOT NULL,
  description            varchar(1024),
  createDate             timestamp      NOT NULL WITH DEFAULT current timestamp,
  CONSTRAINT BGQProductType_pk PRIMARY KEY (productId)
);
CREATE ALIAS BGQProductType for TBGQProductType;



CREATE TABLE TBGQMachine
(
   serialNumber          char(19)       NOT NULL,
   productId             char(16)       NOT NULL,
   description           varchar(1024),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   hasEthernetGateway    char(1)        NOT NULL WITH DEFAULT 'T',
   bgsysIpv4Address      varchar(255),
   bgsysIpv6Address      varchar(255),
   bgsysRemotePath       varchar(255),
   bgsysMountOptions     char(32),     
   fileSystemType        char(16),
   mtu                   integer        NOT NULL WITH DEFAULT 1500,
   memorymodulesize      integer        NOT NULL WITH DEFAULT 1,
   clockHz               integer        NOT NULL WITH DEFAULT -1,
   snIpv4Address         varchar(255),
   snIpv6Address         varchar(255),
   bringupOptions        varchar(256),
   distroIpv4Address     varchar(255),
   distroIpv6Address     varchar(255),
   distroRemotePath      varchar(255),
   distroMountOptions    char(32),     
   CONSTRAINT BGQMachineEthG_chk CHECK ( hasEthernetGateway IN ('T', 'F') ),
   CONSTRAINT BGQMachineSt_chk CHECK ( status IN ('A', 'M','E', 'S') ),
   CONSTRAINT BGQMachine_pk PRIMARY KEY (serialNumber),
   CONSTRAINT BGQMachineType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
);
CREATE ALIAS BGQMachine for TBGQMachine;


CREATE TABLE TBGQMachineSubnet
(
   serialNumber          char(19)       NOT NULL,
   masqueradeAddr        char(15)       NOT NULL,
   netmask               char(15)       NOT NULL,
    CONSTRAINT BGQMSubnet_pk PRIMARY KEY (serialNumber,masqueradeAddr,netmask),
    CONSTRAINT BGQMSubnet_fk FOREIGN KEY (serialNumber)
       REFERENCES TBGQMachine (serialNumber) ON DELETE CASCADE
);
CREATE ALIAS BGQMachineSubnet for TBGQMachineSubnet;


CREATE TABLE TBGQMidplane
(
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   machineSerialNumber   char(19),
   posInMachine          char(6)        NOT NULL,
       CONSTRAINT BGQMidPo_chk CHECK ( posInMachine LIKE 'R__-M_' ),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
       CONSTRAINT BGQMidSt_chk CHECK ( status IN ('A','M','E','S','F') ),
   isMaster              char(1)        NOT NULL WITH DEFAULT 'F',
       CONSTRAINT BGQMidMs_chk CHECK ( isMaster IN ('T', 'F') ),
   vpd                   varchar(4096)  FOR BIT DATA,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   torusA                int            not null with default 0,
   torusB                int            not null with default 0,
   torusC                int            not null with default 0,
   torusD                int            not null with default 0,
   CONSTRAINT BGQMidpplane_pk PRIMARY KEY (posInMachine),
   CONSTRAINT BGQMidMachineId_fk FOREIGN KEY (machineSerialNumber)
       REFERENCES TBGQMachine (serialNumber) ON DELETE RESTRICT,
   CONSTRAINT BGQMidplaneType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
) DATA CAPTURE CHANGES;
CREATE VIEW BGQMidplane as SELECT serialnumber, productid, machineserialnumber, torusA, torusB, torusC, torusD,
                                  status, ismaster, seqId, posinmachine as location  FROM TBGQMidplane;

CREATE VIEW BGQMidplaneAll as SELECT serialnumber, productid, machineserialnumber, torusA, torusB, torusC, torusD,
                                  status, ismaster, vpd, seqId, posinmachine as location  FROM TBGQMidplane;


CREATE TABLE TBGQNodeCard
(
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   midplanePos           char(6)        NOT NULL,
   position              char(3)        NOT NULL,
       CONSTRAINT BGQBoardPos_chk CHECK ( position  IN (
       'N00', 'N01', 'N02', 'N03', 'N04', 
       'N05', 'N06', 'N07', 'N08', 'N09', 
       'N10', 'N11', 'N12', 'N13', 'N14', 'N15')),
           
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
       CONSTRAINT BGQNodeCardSt_chk CHECK ( status IN ('A','M','E','S') ),
     -- (A)vailable, (M)issing, (E)rror, (S)ervice
   vpd                   varchar(4096)  FOR BIT DATA,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   faildata              varchar(32)    FOR BIT DATA,
   CONSTRAINT BGQNodeCard_pk PRIMARY KEY (midplanePos, position),
   CONSTRAINT BGQNodeCard_fk FOREIGN KEY (midplanePos)
        REFERENCES TBGQMidplane(posInMachine) ON DELETE RESTRICT,
   CONSTRAINT BGQCardType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
) DATA CAPTURE CHANGES;
CREATE VIEW BGQNodeCard as SELECT serialnumber, productid, midplanepos, status,  seqId, position,
                                   midplanepos || '-' || position as location from TBGQNodeCard ;

CREATE VIEW BGQNodeCardAll as SELECT serialnumber, productid, midplanepos, status, vpd, seqId, faildata, position,
                                   midplanepos || '-' || position as location from TBGQNodeCard ;


CREATE TABLE TBGQNodeCardDCA
(
   serialNumber         char(19),
   productId            char(16)        NOT NULL,
   midplanePos          char(6)         NOT NULL,
   nodeCardPos          char(3)         NOT NULL, 
   position             char(3)         NOT NULL,
      CONSTRAINT BGQNodeDCAp_chk CHECK ( position  in ('D0' , 'D1' ) ),
   status               char(1)         NOT NULL WITH DEFAULT 'A' ,
      CONSTRAINT BGQNodeDCAs_chk CHECK ( status IN ('A', 'M','E', 'S') ),
   vpd                   varchar(4096)  FOR BIT DATA,
   faildata              varchar(32)    FOR BIT DATA,
   CONSTRAINT BGQNodeDCA_pk PRIMARY KEY (midplanePos, nodeCardPos, position),
   CONSTRAINT BGQNodeDCA_fk FOREIGN KEY (midplanePos, nodeCardPos)
        REFERENCES TBGQNodeCard(midplanePos, position) ON DELETE RESTRICT,
   CONSTRAINT BGQNodeDCAType_fk FOREIGN KEY (productId)
        REFERENCES TBGQProductType (productId)
); 

CREATE VIEW BGQNodeCardDCA as SELECT serialnumber, productid, midplanepos, nodecardpos, status, position,
       midplanepos || '-' || nodecardpos || '-' || position as location from TBGQNodeCardDCA ;

CREATE VIEW BGQNodeCardDCAAll as SELECT serialnumber, productid, midplanepos, nodecardpos, status, vpd, faildata, position,
       midplanepos || '-' || nodecardpos || '-' || position as location from TBGQNodeCardDCA ;

        
                             
CREATE TABLE TBGQNode
(
   serialNumber          char(19),
   productId             char(16),
   -- will be set to the 4 digit CCIN during Install Service Action
   ecid                  char(32)       FOR BIT DATA,
   midplanePos           char(6)        NOT NULL,
   nodeCardPos           char(3)        NOT NULL, 
   position              char(3)        NOT NULL,
       CONSTRAINT BGQNodePos_chk CHECK ( position >= 'J00' and position <= 'J31' ),
     -- IP and MAC addresses are no longer used in this table, see the TBGQNetconfig table
   ipAddress             varchar(255),
   macAddress            char(20),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
      CONSTRAINT BGQNodeSt_chk CHECK ( status IN ('A', 'M','E', 'S', 'F') ),
     -- (A)vailable, (M)issing, (E)rror, (S)ervice, Software (F)ailure
   memoryModuleSize      integer        NOT NULL WITH DEFAULT 1,
   memorySize            integer        NOT NULL WITH DEFAULT 16384,
   psro                  integer        NOT NULL WITH DEFAULT -1,
   vpd                   varchar(4096)  FOR BIT DATA,
   voltage               float, 
   -- this is the memory voltage, the core voltage is derived from the productId
   bitsteering           integer        NOT NULL WITH DEFAULT -1,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   faildata              varchar(32)    FOR BIT DATA,
   CONSTRAINT BGQNode_pk PRIMARY KEY (midplanePos, nodeCardPos, position),
   CONSTRAINT BGQNodeId_fk FOREIGN KEY (midplanePos, nodeCardPos)
        REFERENCES TBGQNodeCard(midplanePos, position) ON DELETE RESTRICT,
   CONSTRAINT BGQNodeType_fk FOREIGN KEY (productId)
        REFERENCES TBGQProductType (productId)
) DATA CAPTURE CHANGES;
CREATE VIEW BGQNode as SELECT serialnumber,productid, midplanepos, nodecardpos, ipaddress, macaddress,
        status, memorymodulesize, memorysize, voltage, bitsteering, seqid,  position,
        midplanepos || '-' || nodecardpos || '-' || position as location  from TBGQNode ;

CREATE VIEW BGQComputeNode as SELECT serialnumber,productid, midplanepos, nodecardpos, ipaddress, macaddress,
        status, memorymodulesize, memorysize, voltage, bitsteering, seqid,  position,
        midplanepos || '-' || nodecardpos || '-' || position as location  from TBGQNode ;

CREATE VIEW BGQNodeAll as SELECT serialnumber,productid, midplanepos, nodecardpos, ipaddress, macaddress,
        status, memorymodulesize, memorysize, psro, ecid, vpd, voltage, bitsteering, seqid, faildata, position,
        midplanepos || '-' || nodecardpos || '-' || position as location  from TBGQNode ;

CREATE INDEX NODEECID on  Tbgqnode (ecid);

-- Service Card  (1/midplane)
CREATE TABLE TBGQServiceCard
(
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   midplanePos           char(6)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
     CONSTRAINT BGQSCardSt_chk CHECK ( status IN ('A', 'M','E', 'S') ),
   vpd                   varchar(4096)  FOR BIT DATA,
   faildata              varchar(32)    FOR BIT DATA,
   CONSTRAINT BGQSCard_pk PRIMARY KEY (midplanePos),
   CONSTRAINT BGQSCard_fk FOREIGN KEY (midplanePos)
        REFERENCES TBGQMidplane(posInMachine) ON DELETE RESTRICT,
   CONSTRAINT BGQSCardType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
);
CREATE VIEW BGQServiceCard AS SELECT serialnumber, productid, midplanePos, status,
            midplanePos || '-S' as location from TBGQServiceCard ;

CREATE VIEW BGQServiceCardAll AS SELECT serialnumber, productid, midplanePos, status, vpd, faildata,
            midplanePos || '-S' as location from TBGQServiceCard ;


CREATE TABLE TBGQClockCard
(
   serialNumber          char(19),
   productId             char(16),
   location              char(6)        NOT NULL,
    CONSTRAINT BGQClock_chk CHECK ( location LIKE 'R__-K ' OR location like 'Q__-K0' OR location like 'Q__-K1' ),
    CONSTRAINT BGQClock_pk PRIMARY KEY (location),
   clockHz               integer        NOT NULL WITH DEFAULT -1,
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
    CONSTRAINT BGQCCardSt_chk CHECK ( status IN ('A', 'M','E', 'S') ),
   vpd                   varchar(4096)  FOR BIT DATA
);
CREATE VIEW BGQClockCard  AS SELECT serialnumber, productid, location, clockhz, status from TBGQClockCard;

CREATE VIEW BGQClockCardAll as select * from  TBGQClockCard;


CREATE TABLE TBGQBulkPowerSupply
(
   serialNumber          char(19),
   productId             char(16),
   location              char(10)       NOT NULL,
    CONSTRAINT BGQBulk_chk CHECK ( location LIKE 'R__-B_-P_ ' or location LIKE 'Q__-B-P_  ' ),
    CONSTRAINT BGQBulk_pk PRIMARY KEY (location),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
    CONSTRAINT BGQBulkSt_chk CHECK ( status IN ('A', 'M','E', 'S') ),
   vpd                   varchar(256)   FOR BIT DATA
);
CREATE ALIAS BGQBulkPowerSupply for TBGQBulkPowerSupply;

CREATE VIEW BGQBulkPowerSupplyAll as select * from  TBGQBulkPowerSupply;


CREATE TABLE TBGQSwitch
(
   switchID              varchar(32)    NOT NULL,
   midplanePos           char(6)        NOT NULL,
   machineSerialNumber   char(19),
   dimension             char(1)        NOT NULL,
       CONSTRAINT BGQSwDi_chk CHECK ( dimension IN ('A', 'B', 'C', 'D') ),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
       CONSTRAINT BGQSwSt_chk CHECK ( status IN ('A', 'M', 'E', 'S') ),
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   CONSTRAINT BGQSwitch_pk PRIMARY KEY (switchID),
   CONSTRAINT BGQSwMidplane_fk FOREIGN KEY (midplanePos)
       REFERENCES TBGQMidplane (posInMachine) ON DELETE RESTRICT
) DATA CAPTURE CHANGES;
CREATE ALIAS BGQSwitch for TBGQSwitch;


-- Cable 
-- torus cables go between switches - IO cables go between a compute node and an IO node
CREATE TABLE TBGQCable
(
   FromLocation          char(16)       NOT NULL,
   ToLocation            char(16)       NOT NULL,
   Status                char(1)        NOT NULL WITH DEFAULT 'A',
       CONSTRAINT BGQCableStatus_chk CHECK ( status IN ('A', 'M', 'E' , 'S') ),
   CreateDate            timestamp      NOT NULL WITH DEFAULT current timestamp,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   badWireMask           bigint         NOT NULL WITH DEFAULT 0,
   CONSTRAINT BGQCable_pk PRIMARY KEY(FromLocation, ToLocation)
) DATA CAPTURE CHANGES;
CREATE ALIAS BGQCable for TBGQCable;


-- Clock Cables
CREATE TABLE TBGQClockCable
(  
   FromLocation          char(16)       NOT NULL,
   ToLocation            char(16)       NOT NULL,
   -- ToLocation can be clock cards or midplanes (or possibly an IO chassis)	
   CONSTRAINT BGQClkCable_pk PRIMARY KEY(FromLocation,ToLocation)
);
CREATE ALIAS BGQClockCable for TBGQClockCable;

-- Link Chip  
CREATE TABLE TBGQLinkChip
(
   serialNumber          char(19),
   productId             char(16),
   ecid                  char(14)       FOR BIT DATA,
   midplanePos           char(6)        NOT NULL,
   nodeCardPos           char(3)        NOT NULL,
   position              char(3)        NOT NULL,
       CONSTRAINT BGQLchipPos_chk CHECK ( position IN (
       'U00', 'U01', 'U02', 'U03','U04','U05', 'U06','U07','U08') ),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   CONSTRAINT BGQLinkChipSt_chk CHECK ( status IN ('A', 'M', 'E', 'S') ),
   CONSTRAINT BGQLChip_pk PRIMARY KEY (midplanePos,nodeCardPos,position),
   CONSTRAINT BGQLChipId_fk FOREIGN KEY (midplanePos,nodeCardPos)
       REFERENCES TBGQNodeCard(midplanePos, position)  ON DELETE RESTRICT,
   CONSTRAINT BGQLChipType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
);
CREATE VIEW BGQLinkChip AS SELECT serialnumber, productid, midplanePos, nodecardpos,status,position,
            midplanepos || '-' || nodecardpos || '-' || position as location from TBGQLinkChip ;

CREATE VIEW BGQLinkChipAll AS SELECT serialnumber, productid, midplanePos, nodecardpos,status,ecid,position,
            midplanepos || '-' || nodecardpos || '-' || position as location from TBGQLinkChip ;


CREATE TABLE TBGQIORack
(
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   machineSerialNumber   char(19),
   location              char(3)        NOT NULL,
       CONSTRAINT BGQIORPo_chk CHECK ( location LIKE 'Q__' ),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
       CONSTRAINT BGQIORSt_chk CHECK ( status IN ('A','M','E', 'S') ),
   CONSTRAINT BGQIORack_pk PRIMARY KEY (location),
   CONSTRAINT BGQIORMachineId_fk FOREIGN KEY (machineSerialNumber)
       REFERENCES TBGQMachine (serialNumber) ON DELETE RESTRICT,
   CONSTRAINT BGQIORType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
);
CREATE ALIAS BGQIORack  for TBGQIORack;


CREATE TABLE TBGQIODrawer
(
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   location              char(6)        NOT NULL,
       CONSTRAINT BGQIODLoc_chk CHECK ( location like 'Q__-I_' or location like 'R__-I_'),            
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
       CONSTRAINT BGQIODSt_chk CHECK ( status IN ('A','M','E','S') ),
     -- (A)vailable, (M)issing, (E)rror, (S)ervice
   vpd                   varchar(4096)  FOR BIT DATA,
   faildata              varchar(32)    FOR BIT DATA,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   CONSTRAINT BGQIOE_pk PRIMARY KEY (location),
   CONSTRAINT BGQIOEType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
) DATA CAPTURE CHANGES;
CREATE VIEW BGQIODrawer AS SELECT serialnumber, productid, location, status, seqId
       from TBGQIODrawer;

CREATE VIEW BGQIODrawerAll AS SELECT serialnumber, productid, location, status, vpd, faildata, seqId
       from TBGQIODrawer;


CREATE TABLE TBGQIONode
(
   serialNumber          char(19),
   productId             char(16),
   -- will be set to the 4 digit CCIN during Install Service Action
   ecid                  char(32)       FOR BIT DATA,
   IOPos                 char(6)        NOT NULL,
   position              char(3)        NOT NULL,
       CONSTRAINT BGQIONodePos_chk CHECK ( position >= 'J00' and position <= 'J07' ),
     -- IP and MAC addresses are no longer used in this table, see the TBGQNetconfig table
   ipAddress             varchar(255),
   macAddress            char(20),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
      CONSTRAINT BGQIONodeSt_chk CHECK ( status IN ('A', 'M','E', 'S', 'F') ),
     -- (A)vailable, (M)issing, (E)rror, (S)ervice, Software (F)ailure
   memoryModuleSize      integer        NOT NULL WITH DEFAULT 1,
   memorySize            integer        NOT NULL WITH DEFAULT 16384,
   psro                  integer        NOT NULL WITH DEFAULT -1,
   vpd                   varchar(4096)  FOR BIT DATA,
   voltage               float, 
   -- this is the memory voltage, the core voltage is derived from the productId
   bitsteering           integer        NOT NULL WITH DEFAULT -1,
   faildata              varchar(32)    FOR BIT DATA,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   CONSTRAINT BGQIONode_pk PRIMARY KEY (IOPos, position),
   CONSTRAINT BGQIONodeId_fk FOREIGN KEY (IOPos)
        REFERENCES TBGQIODrawer(location) ON DELETE RESTRICT,
   CONSTRAINT BGQIONodeType_fk FOREIGN KEY (productId)
        REFERENCES TBGQProductType (productId)
) DATA CAPTURE CHANGES;
CREATE VIEW BGQIONode as SELECT serialnumber,productid, IOpos, ipaddress, macaddress,
        status, memorymodulesize, memorysize, voltage, bitsteering, seqid, position,
        IOpos || '-' || position as location  from TBGQIONode ;

CREATE VIEW BGQIONodeAll as SELECT serialnumber,productid, IOpos, ipaddress, macaddress,
        status, memorymodulesize, memorysize, psro, ecid, vpd, voltage, bitsteering, faildata, seqid,
        position, IOpos || '-' || position as location  from TBGQIONode ;


CREATE TABLE TBGQIOLinkChip
(
   serialNumber          char(19),
   productId             char(16),
   ecid                  char(14)       FOR BIT DATA,
   IOPos                 char(6)        NOT NULL,
   position              char(3)        NOT NULL,
       CONSTRAINT BGQLchipPos_chk CHECK ( position IN (
       'U00', 'U01', 'U02', 'U03','U04','U05') ),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   CONSTRAINT BGQIOLChipSt_chk CHECK ( status IN ('A', 'M', 'E', 'S') ),
   CONSTRAINT BGQIOLChip_pk PRIMARY KEY (IOPos,position),
   CONSTRAINT BGQIOLChipId_fk FOREIGN KEY (IOPos)
       REFERENCES TBGQIODrawer(location)  ON DELETE RESTRICT,
   CONSTRAINT BGQLIOChipType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
);
CREATE VIEW BGQIOLinkChip AS SELECT serialnumber, productid, IOPos, status,position,
            IOPos || '-' || position as location from TBGQIOLinkChip ;

CREATE VIEW BGQIOLinkChipAll AS SELECT serialnumber, productid, IOPos, status,ecid,position,
            IOPos || '-' || position as location from TBGQIOLinkChip ;


CREATE TABLE TBGQICON
(
   licensePlate          char(12)       FOR BIT DATA  NOT NULL,
   containerLocation     char(10)       NOT NULL,  
   ipAddress             varchar(255),
   iConId                integer, 
   iConChecksum          smallint, 
   iConLogicRev          integer,
   iConBuildDate         char(10),
   iConBuildNumber       integer,
   PalominoId            integer,
   PalominoChecksum      smallint,
   PalominoLogicRev      integer,
   PalominoBuildDate     char(10),
   PalominoBuildNumber   integer,
   status                char(1)        NOT NULL WITH DEFAULT 'A',
   CONSTRAINT BGQICON_pk PRIMARY KEY (containerLocation)
);
CREATE ALIAS BGQICon for TBGQICon;


CREATE TABLE TBGQJob
(
   id                    bigint         GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1),
   username              char(32)       NOT NULL,
   blockId               char(32)       NOT NULL,
   executable            varchar(512)   NOT NULL,
   workingdir            varchar(512)   NOT NULL,
   startTime             timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   status                char(1)        NOT NULL WITH DEFAULT 'P',
     CONSTRAINT BGQJobSt_chk CHECK (status IN ('P', 'L', 'S', 'R', 'N', 'D')),
     -- Setu(p), (L)oading, (S)tarting, (R)unning, Clea(n)up, (D)ebug
   statusLastModified    timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   args                  varchar(4096)  NOT NULL WITH DEFAULT '',
   envs                  varchar(8192)  NOT NULL WITH DEFAULT '',
   mapping               varchar(512)   NOT NULL WITH DEFAULT '',
     -- a permutation of ABCDET or a filename
   nodesUsed             integer        NOT NULL,
     -- will be 0 for single-core jobs
   shapeA                integer        NOT NULL,
   shapeB                integer        NOT NULL,
   shapeC                integer        NOT NULL,
   shapeD                integer        NOT NULL,
   shapeE                integer        NOT NULL,
     -- job shape in each dimension
   processesPerNode      integer        NOT NULL,
   schedulerData         varchar(128),
     -- scheduler data blob
   corner                char(18),
      -- sub-block job corner location ex: R00-M0-N00-J00
      -- for single-core jobs, this will be the core ex: R00-M0-N00-J00-C00
   hostname              varchar(128)  NOT NULL,
     -- hostname where runjob was launched
   pid                   integer       NOT NULL,
     -- pid of runjob process
   QUALIFIER             char(32),
     -- boot cookie
   np                    integer       NOT NULL WITH DEFAULT 0,
     -- number of processes participating in the job
   client                integer       NOT NULL WITH DEFAULT 0,
     -- client ID of runjob
   CONSTRAINT BGQJob_pk PRIMARY KEY (id),
   CONSTRAINT BGQJobBlockId_fk FOREIGN KEY (blockId) 
        REFERENCES TBGQBlock (blockId)
) DATA CAPTURE CHANGES;
CREATE ALIAS BGQJob for TBGQJob;



CREATE TABLE TBGQJobMap
(
   id                    bigint         NOT NULL,
   nodeboard             char(3),
   midplane              char(6)        NOT NULL,
   CONSTRAINT BGQJobMapId_fk FOREIGN KEY (id) 
        REFERENCES TBGQJob (id) ON DELETE CASCADE
);
CREATE ALIAS BGQJobMap for TBGQJobMap;

CREATE TABLE TBGQJobTool
(
   id                    integer        NOT NULL,
   job                   bigint         NOT NULL,
   status                char(1)        NOT NULL,
     CONSTRAINT BGQJobSt_chk CHECK (status IN ('S','R','E','T')),
     -- (S)tarting, (R)unning, (E)nding, (T)erminated
   errtext               char(128),
   executable            varchar(512)   NOT NULL,
   args                  varchar(1024)  NOT NULL WITH DEFAULT '',
   entrydate             timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   subset                varchar(4096)  NOT NULL,
   CONSTRAINT BGQJobTool_pk PRIMARY KEY (id,job),
     -- no duplicate tool id for the same job
   CONSTRAINT BGQJobToolId_fk FOREIGN KEY (job) 
        REFERENCES TBGQJob (id) ON DELETE CASCADE
);
CREATE ALIAS BGQJobTool for TBGQJobTool;


CREATE TABLE TBGQEthGateway
(
   serialNumber          char(19)       NOT NULL,
   machineSerialNumber   char(19)       NOT NULL,
   productId             char(16)       NOT NULL,
   ipAddress             varchar(255),
   broadcast             varchar(255),
   mask                  varchar(255),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   ipv6mask              varchar(255),
   CONSTRAINT BGQMidSt_chk CHECK ( status IN ('A', 'M', 'E' , 'S' ) ),
   CONSTRAINT BGQEthGW_pk PRIMARY KEY (serialNumber),
   CONSTRAINT BGQEthGWType_fk FOREIGN KEY (productId)
       REFERENCES TBGQProductType (productId)
);
CREATE ALIAS BGQEthGateway for TBGQEthGateway;


CREATE TABLE TBGQMidplaneSubnet
(
   posInMachine          char(6)        NOT NULL,    
   ipAddress             varchar(255)   NOT NULL,
   broadcast             varchar(255)   NOT NULL,
   nfsIpAddress          varchar(255)   NOT NULL,
   CONSTRAINT BGQMPSub_pk PRIMARY KEY (posInMachine)
);
CREATE ALIAS BGQMidplaneSubnet for TBGQMidplaneSubnet;

CREATE TABLE TBGQNetConfig
(
   location              char(16)       NOT NULL,
   interface             char(16)       NOT NULL,
   itemName              varchar(128)   NOT NULL,
   itemValue             varchar(255)   NOT NULL,
   CONSTRAINT BGQNet_unqitem UNIQUE ( location, interface, itemName )
);
CREATE ALIAS BGQNetConfig for TBGQNetConfig;

CREATE VIEW BGQIONetConfig (location , iopos, interface, itemname, itemvalue) as SELECT
 a.location, iopos, interface, itemname, itemvalue  from BGQIONode a LEFT OUTER JOIN BGQNetConfig b
 on a.location = b.location;
        


CREATE TABLE TBGQServiceAction
(
   Id                           integer     GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1),
   Location                     varchar(32) NOT NULL,
   ServiceAction                varchar(32),
   InfoServiceAction            varchar(256),
   UsernamePrepareForService    varchar(32) NOT NULL,
   UsernameEndServiceAction     varchar(32),
   Status                       char(1)     NOT NULL,
   TsPrepareForService          timestamp   NOT NULL WITH DEFAULT current timestamp,
   TsEndServiceAction           timestamp,
   LogFileNamePrepareForService varchar(256),
   LogFileNameEndServiceAction  varchar(256)
);
CREATE ALIAS BGQServiceAction for TBGQServiceAction;

   
CREATE TABLE TBGQUserPrefs
(
   userName              varchar(32)    NOT NULL,
   preferenceName        varchar(32)    NOT NULL,
   preferenceValue       varchar(1024)  NOT NULL,
   CONSTRAINT TBGQUserPrefs_pk PRIMARY KEY (userName, preferenceName)
);
CREATE ALIAS BGQUSERPREFS for TBGQUserPrefs;

CREATE TABLE TBGQBlockSecurity
(
   authId                char(32)       NOT NULL,
   blockId               char(32)       NOT NULL,
   authority             char(1)        NOT NULL,
   CONSTRAINT BGQBlockSec_pk PRIMARY KEY (authId,blockId,authority),     
   CONSTRAINT BGQBlockSecId_fk FOREIGN KEY (blockid) REFERENCES TBGQBlock (blockid) ON DELETE CASCADE,
   CONSTRAINT BGQSec_chk CHECK (authority IN ('C', 'R', 'U' , 'D',  'E' ) )
);
CREATE ALIAS BGQBlockSecurity for TBGQBlockSecurity;

CREATE TABLE TBGQBlockSecurityLog
(
   authId                char(32)       NOT NULL, -- user name
   blockId               char(32)       NOT NULL,
   authority             char(1)        NOT NULL,
   CONSTRAINT BGQBlockSecLog_authchk CHECK (authority IN ('C', 'R', 'U' , 'D',  'E' ) ),
   action                char(1)        NOT NULL, -- grant or revoke
   CONSTRAINT BGQBlockSecLog_actchk CHECK (action IN ('G', 'R') ),
   changedById           char(32)       NOT NULL, -- user name
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp
);
CREATE ALIAS BGQBlockSecurityLog for TBGQBlockSecurityLog;

CREATE TABLE TBGQJobSecurity
(
   authId                char(32)       NOT NULL,
   jobId                 bigint         NOT NULL,
   authority             char(1)        NOT NULL,
   CONSTRAINT BGQSec_pk PRIMARY KEY (authId,jobId,authority),     
   CONSTRAINT BGQJobSecId_fk FOREIGN KEY (jobid) REFERENCES TBGQJob (id) ON DELETE CASCADE,
   CONSTRAINT BGQSec_chk CHECK (authority IN ('R', 'E' ) )
);
CREATE ALIAS BGQJobSecurity for TBGQJobSecurity;

CREATE TABLE TBGQJobSecurityLog
(
   authId                char(32)       NOT NULL, -- user name
   jobId                 bigint         NOT NULL,
   authority             char(1)        NOT NULL,
   CONSTRAINT BGQJobSecLog_authchk CHECK (authority IN ('R', 'E' ) ),
   action                char(1)        NOT NULL, -- grant or revoke
   CONSTRAINT BGQJobSecLog_actchk CHECK (action IN ('G', 'R') ),
   changedById           char(32)       NOT NULL, -- user name
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp
);
CREATE ALIAS BGQJobSecurityLog for TBGQJobSecurityLog;

-- links 


-- Link view (replaces link table)  this shows all compute cables (torus and IO)

CREATE VIEW  BGQLink (source , destination, status, seqid) AS  select
 case substr(fromlocation,12,3) when 'T06'  then 'A_'|| substr(fromlocation,1,6) when 'T05' then 'B_'|| substr(fromlocation,1,6) when 'T10' then 'C_'|| substr(fromlocation,1,6) when 'T09' then 'D_'|| substr(fromlocation,1,6) 
                                when 'T04'  then 'A_'|| substr(fromlocation,1,6) when 'T07' then 'B_'|| substr(fromlocation,1,6) when 'T08' then 'C_'|| substr(fromlocation,1,6) when 'T11' then 'D_'|| substr(fromlocation,1,6) else 'I_'|| substr(fromlocation,1,14) end  as source,
 case substr(tolocation,12,3)   when 'T04'  then 'A_'|| substr(tolocation,1,6)   when 'T07' then 'B_'|| substr(tolocation,1,6)   when 'T08' then 'C_'|| substr(tolocation,1,6)   when 'T11' then 'D_'|| substr(tolocation,1,6)   
                                when 'T06'  then 'A_'|| substr(tolocation,1,6)   when 'T05' then 'B_'|| substr(tolocation,1,6)   when 'T10' then 'C_'|| substr(tolocation,1,6)   when 'T09' then 'D_'|| substr(tolocation,1,6)   else 'I_'|| substr(tolocation,1,11) end as destination,
max(status) as status, max(seqid) as seqid from tbgqcable 

where (substr(fromlocation,5,1) = 'M' or substr(tolocation,5,1) = 'M' ) and (substr(tolocation,5,1) = 'I' or substr(fromlocation,12,3) in ('T06','T05','T10','T09','T04','T07','T08','T11'))
group by
 case substr(fromlocation,12,3) when 'T06'  then 'A_'|| substr(fromlocation,1,6) when 'T05' then 'B_'|| substr(fromlocation,1,6) when 'T10' then 'C_'|| substr(fromlocation,1,6) when 'T09' then 'D_'|| substr(fromlocation,1,6) 
                                when 'T04'  then 'A_'|| substr(fromlocation,1,6) when 'T07' then 'B_'|| substr(fromlocation,1,6) when 'T08' then 'C_'|| substr(fromlocation,1,6) when 'T11' then 'D_'|| substr(fromlocation,1,6) else 'I_'|| substr(fromlocation,1,14) end,
 case substr(tolocation,12,3)   when 'T04'  then 'A_'|| substr(tolocation,1,6)   when 'T07' then 'B_'|| substr(tolocation,1,6)   when 'T08' then 'C_'|| substr(tolocation,1,6)   when 'T11' then 'D_'|| substr(tolocation,1,6)  
                                when 'T06'  then 'A_'|| substr(tolocation,1,6)   when 'T05' then 'B_'|| substr(tolocation,1,6)   when 'T10' then 'C_'|| substr(tolocation,1,6)   when 'T09' then 'D_'|| substr(tolocation,1,6)   else 'I_'|| substr(tolocation,1,11) end;

-- IO link view   - this shows the IO torus cables

CREATE VIEW  BGQIOLink (source, destination, status, seqid) AS select
 case substr(fromlocation,8,3) 
 when 'T01' then 'A_' when 'T09' then 'A_' when 'T07' then 'B_' when 'T15' then 'B_' when 'T17' then 'C_' when 'T20' then 'C_' else 'D_' end
 || substr(fromlocation,1,6) as source,
 case substr(tolocation,8,3) 
 when 'T05' then 'A_' when 'T13' then 'A_' when 'T03' then 'B_' when 'T11' then 'B_' when 'T21' then 'C_' when 'T16' then 'C_' else 'D_' end
 || substr(tolocation,1,6) as destination,
 max(status) as status, max(seqid) as seqid from tbgqcable where  substr(fromlocation,5,1) = 'I' and substr(tolocation,5,1) = 'I' 
group by
 case substr(fromlocation,8,3) 
 when 'T01' then 'A_' when 'T09' then 'A_' when 'T07' then 'B_' when 'T15' then 'B_' when 'T17' then 'C_' when 'T20' then 'C_' else 'D_' end
 ||  substr(fromlocation,1,6),
 case substr(tolocation,8,3) 
 when 'T05' then 'A_' when 'T13' then 'A_' when 'T03' then 'B_' when 'T11' then 'B_' when 'T21' then 'C_' when 'T16' then 'C_' else 'D_' end
 ||  substr(tolocation,1,6);

-- Compute Torus Links only, and include bad wire info

CREATE VIEW  BGQTorusLink (dim, sourcemid , destinationmid, badwires) AS  select
 case substr(fromlocation,12,3) 
    when 'T06' then 'A' when 'T05' then 'B' when 'T10' then 'C' when 'T09' then 'D'  
    when 'T04' then 'A' when 'T07' then 'B' when 'T08' then 'C' when 'T11' then 'D' else 'I' end  as dim,
 substr(fromlocation,1,6) as sourcemid,
 substr(tolocation,  1,6) as destinationmid,
 max(badwiremask) as badwires from bgqcable
 
where (substr(fromlocation,5,1) = 'M' and  substr(tolocation,5,1) = 'M') 
group by
 case substr(fromlocation,12,3) 
    when 'T06' then 'A' when 'T05' then 'B' when 'T10' then 'C' when 'T09' then 'D'  
    when 'T04' then 'A' when 'T07' then 'B' when 'T08' then 'C' when 'T11' then 'D' else 'I' end,
 substr(fromlocation,1,6), substr(tolocation,  1,6);


-- Compute Nodes to IO Nodes

CREATE VIEW BGQCNIOLink (source, destination, status, seqid, ion, ionstatus, cnconnector, badwires) as select
 substr(fromlocation,1,11)|| CN.node , tolocation, a.status, a.seqid, b.location, b.status, CN.port, badwiremask
 from   tbgqcable a , bgqionode b,

 (values
( 'T00' , 'J06' , 1),
( 'T00' , 'J11' , 2),
( 'T01' , 'J10' , 1),
( 'T01' , 'J07' , 2),
( 'T02' , 'J08' , 1),
( 'T02' , 'J05' , 2),
( 'T03' , 'J04' , 1),
( 'T03' , 'J09' , 2)) as CN (port,node,num),

 (values
( 'T19' , 'J05' , 1),
( 'T19' , 'J07' , 2),
( 'T23' , 'J04' , 1),
( 'T23' , 'J06' , 2),
( 'T22' , 'J01' , 1),
( 'T22' , 'J03' , 2),
( 'T18' , 'J00' , 1),
( 'T18' , 'J02' , 2),
( 'T12' , 'J07' , 1),
( 'T12' , 'J06' , 2),
( 'T14' , 'J05' , 1),
( 'T14' , 'J04' , 2),
( 'T04' , 'J03' , 1),
( 'T04' , 'J02' , 2),
( 'T06' , 'J01' , 1),
( 'T06' , 'J00' , 2)) as IO (port,node,num)

where substr(fromlocation,5,1) = 'M' and substr(tolocation,5,1) = 'I'  and
  substr(tolocation,1,6) = b.iopos and
  substr(tolocation,8,3) = IO.port and
  substr(fromlocation,12,3) = CN.port  and CN.num = IO.num and b.position = IO.node;


  -- Adds the I/O Node to the TBGQIoUsage table by joining in BGQCNIOLink.
CREATE VIEW BGQIoUsage ( blockId, computeNode, count, ioNode ) AS
  SELECT u.blockId, u.computeNode, u.count, l.ion AS ioNode
    FROM TBGQIoUsage AS u
         JOIN
         BGQCNIOLink AS l
         ON u.computeNode = l.source;


  -- Calculates how many compute nodes are using each I/O node.
CREATE VIEW BGQIoUsageSum ( ioNode, count ) AS
  SELECT ioNode, SUM(count) AS count
    FROM BGQIoUsage
    GROUP BY ioNode;


--
--  HISTORY TABLES
--

CREATE TABLE TBGQReplacement_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   type                  char(16),
   location              char(64),
   oldserialNumber       char(19)       NOT NULL,
   newserialNumber       char(19)       NOT NULL,
   oldECID               char(32)       FOR BIT DATA,
   newECID               char(32)       FOR BIT DATA,
   oldStatus             char(1),
   newStatus             char(1)
);
CREATE ALIAS BGQReplacement_history for TBGQReplacement_history ;


CREATE TABLE TBGQMachine_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19)       NOT NULL,
   productId             char(16)       NOT NULL,
   description           varchar(1024),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   hasEthernetGateway    char(1)        NOT NULL WITH DEFAULT 'T',
   bgsysIpv4Address      varchar(255),
   bgsysIpv6Address      varchar(255),
   bgsysRemotePath       varchar(255),
   bgsysMountOptions     char(32),     
   fileSystemType        char(16),
   mtu                   integer        NOT NULL WITH DEFAULT 1500,
   memorymodulesize      integer        NOT NULL WITH DEFAULT 1,
   clockHz               integer        NOT NULL WITH DEFAULT -1,
   snIpv4Address         varchar(255),
   snIpv6Address         varchar(255),
   bringupOptions        varchar(256),
   distroIpv4Address     varchar(255),
   distroIpv6Address     varchar(255),
   distroRemotePath      varchar(255),
   distroMountOptions    char(32)
 );
CREATE ALIAS BGQMachine_history for TBGQMachine_history        ;


CREATE TABLE  TBGQMidplane_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   machineSerialNumber   char(19),
   posInMachine          char(6),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   isMaster              char(1)        NOT NULL WITH DEFAULT 'T' ,
   vpd                   varchar(4096)  FOR BIT DATA
);
CREATE ALIAS BGQMidplane_history for TBGQMidplane_history;

       
CREATE TABLE TBGQIORack_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   machineSerialNumber   char(19),
   location              char(3)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' 
);
CREATE ALIAS BGQIORack_history for TBGQIORack_history;


CREATE TABLE TBGQIODrawer_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   location              char(7)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A',
   vpd                   varchar(4096)  FOR BIT DATA
);
CREATE ALIAS BGQIODrawer_history  for TBGQIODrawer_history;


CREATE TABLE TBGQIOLinkChip_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16),
   ecid                  char(14)       FOR BIT DATA,
   IOPos                 char(7)        NOT NULL,
   position              char(3)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' 
);
CREATE ALIAS BGQIOLinkChip_history for  TBGQIOLinkChip_history;


CREATE TABLE TBGQBulkPowerSupply_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16),
   location              char(10)       NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   vpd                   varchar(256)   FOR BIT DATA
);
CREATE ALIAS BGQBulkPowerSupply_history for TBGQBulkPowerSupply_history;


CREATE TABLE TBGQNodeCard_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   midplanePos           char(6)        NOT NULL,
   position              char(4)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   vpd                   varchar(4096)  FOR BIT DATA
);
CREATE ALIAS BGQNodeCard_history for TBGQNodeCard_history  ;


CREATE TABLE TBGQNode_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16),
   ecid                  char(32)       FOR BIT DATA,
   midplanePos           char(6)        NOT NULL,
   nodeCardPos           char(3)        NOT NULL, 
   position              char(4)        NOT NULL,
   ipAddress             varchar(255),
   macAddress            char(20),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   memoryModuleSize      integer        NOT NULL WITH DEFAULT 1,
   memorySize            integer        NOT NULL WITH DEFAULT 16384,
   psro                  integer        NOT NULL WITH DEFAULT -1,       
   vpd                   varchar(4096)  FOR BIT DATA,
   voltage               float
);
CREATE ALIAS BGQNode_history for TBGQNode_history;

CREATE TABLE TBGQIONode_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16),
   ecid                  char(32)       FOR BIT DATA,
   IOPos                 char(7)        NOT NULL,
   position              char(3)        NOT NULL,
   ipAddress             varchar(255),
   macAddress            char(20),
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   memoryModuleSize      integer        NOT NULL WITH DEFAULT 1,
   memorySize            integer        NOT NULL WITH DEFAULT 16384,
   psro                  integer        NOT NULL WITH DEFAULT -1,
   vpd                   varchar(4096)  FOR BIT DATA,
   voltage               float
);
CREATE ALIAS BGQIONode_history for TBGQIONode_history;


CREATE TABLE TBGQServiceCard_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16)       NOT NULL,
   midplanePos           char(6)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   vpd                   varchar(4096)  FOR BIT DATA
);
CREATE ALIAS BGQServiceCard_history for TBGQServiceCard_history;


CREATE TABLE TBGQClockCard_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16),
   location              char(6),
   clockHz               integer        NOT NULL WITH DEFAULT -1,
   status                char(1)        NOT NULL WITH DEFAULT 'A' ,
   vpd                   varchar(4096)  FOR BIT DATA

);
CREATE ALIAS BGQClockCard_history for TBGQClockCard_history;


CREATE TABLE TBGQLinkChip_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp, 
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   serialNumber          char(19),
   productId             char(16),
   ecid                  char(14)       FOR BIT DATA,
   midplanePos           char(6)        NOT NULL,
   nodeCardPos           char(3)        NOT NULL,
   position              char(3)        NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'A' 
);
CREATE ALIAS BGQLinkChip_history for TBGQLinkChip_history;
          

CREATE TABLE TBGQIcon_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp, 
   action                char(1)        NOT NULL WITH DEFAULT 'U',
   licensePlate          char(12)       FOR BIT DATA  NOT NULL,
   containerLocation     char(10),
   ipAddress             varchar(255),
   iConId                integer, 
   iConChecksum          smallint, 
   iConLogicRev          integer,
   iConBuildDate         char(10),
   iConBuildNumber       integer,
   PalominoId            integer,
   PalominoChecksum      smallint,
   PalominoLogicRev      integer,
   PalominoBuildDate     char(10),
   PalominoBuildNumber   integer,
   status                char(1)        NOT NULL WITH DEFAULT 'A'
);
CREATE ALIAS BGQIcon_history for TBGQIcon_history;


CREATE TABLE TBGQJob_history
(
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   id                    bigint         NOT NULL,
   username              char(32)       NOT NULL,
   blockId               char(32)       NOT NULL,
   creationID            integer        NOT NULL,
   executable            varchar(512)   NOT NULL,
   workingdir            varchar(512)   NOT NULL,
   startTime             timestamp      NOT NULL,
   status                char(1)        NOT NULL,
     CONSTRAINT BGQJobHSt_chk CHECK (status IN ('T', 'E')),
     -- (T)erminated, (E)rror
   seqId                 bigint         NOT NULL,
   args                  varchar(4096)  NOT NULL,
   envs                  varchar(8192)  NOT NULL,
   mapping               varchar(512)   NOT NULL,
     -- a permutation of ABCDET or a filename
   nodesUsed             integer        NOT NULL,
     -- will be 0 for single-core jobs
   shapeA                integer        NOT NULL,
   shapeB                integer        NOT NULL,
   shapeC                integer        NOT NULL,
   shapeD                integer        NOT NULL,
   shapeE                integer        NOT NULL,
     -- job shape in each dimension
   processesPerNode      integer        NOT NULL,
   schedulerData         varchar(128),
     -- scheduler data blob
   corner                char(18),
      -- sub-block job corner location ex: R00-M0-N00-J00
      -- for single-core jobs, this will be the core ex: R00-M0-N00-J00-C00
   hostname              varchar(128)  NOT NULL,
     -- hostname where runjob was launched
   pid                   integer       NOT NULL,
     -- pid of runjob process
   exitstatus            integer,
   errtext               char(128),
   QUALIFIER             char(32),
     -- boot cookie
   np                    integer       NOT NULL WITH DEFAULT 0,
   client                integer       NOT NULL WITH DEFAULT 0
 );
CREATE ALIAS BGQJob_history for TBGQJob_history;

CREATE INDEX JobHistE  on  TbgqJob_History (entrydate desc);
CREATE INDEX JobHistB  on  TbgqJob_History (blockid, entrydate desc);
CREATE INDEX JobHistU  on  TbgqJob_History (username, entrydate desc);
CREATE INDEX JobHistJ  on  TbgqJob_History (id);

CREATE TABLE TBGQCable_history
(
   FromLocation          char(16)       NOT NULL,
   ToLocation            char(16)       NOT NULL,
   Status                char(1)        NOT NULL WITH DEFAULT 'A',
   entrydate             timestamp      NOT NULL WITH DEFAULT current timestamp,
   seqId                 bigint         NOT NULL WITH DEFAULT 0,
   badWireMask           bigint         NOT NULL WITH DEFAULT 0
);
CREATE ALIAS BGQCable_history for TBGQCable_history;


--TABLES FOR STORING ENVIRONMENT INFORMATION--


CREATE TABLE TBGQServiceCardEnvironment
(
   location              char(8)        NOT NULL,          
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   voltageV12P           float,
   voltageV12R5          float,
   voltageV15P           float,
   voltageV25P           float,
   voltageV33P           float,
   voltageV50P           float,
   CONSTRAINT BGQSCardEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQServiceCardEnvironment for TBGQServiceCardEnvironment;

CREATE TABLE TBGQServiceCardTemp
(
   location              char(8)        NOT NULL,          
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   onboardTemp           integer,
   CONSTRAINT BGQSCardTemp_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQServiceCardTemp for TBGQServiceCardTemp;

CREATE TABLE TBGQBULKPOWEREnvironment
(
   location                  char(10)   NOT NULL,      
   time                      timestamp  NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   inputvoltage              float,
   inputcurrent              float,
   outputvoltage             float,
   outputcurrent             float,
   CONSTRAINT BGQBulkEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQBULKPOWEREnvironment for TBGQBULKPOWEREnvironment;


CREATE TABLE TBGQBULKPOWERTemp
(
   location                  char(10)   NOT NULL,      
   time                      timestamp  NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   module1temp               float,
   module2temp               float,
   module3temp               float,
   CONSTRAINT BGQBulkTmp_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQBULKPOWERTemp for TBGQBULKPOWERTemp;

CREATE TABLE TBGQOpticalEnvironment
(
   location                  char(16)   NOT NULL,      
   time                      timestamp  NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   channel                   integer,
   power                     integer,
   CONSTRAINT BGQOptEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQOpticalEnvironment for TBGQOpticalEnvironment;

CREATE TABLE TBGQOpticalData
(
   location                     char(16)   NOT NULL,      
   time                         timestamp  NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   error                        integer,
   status                       char(1),
   los                          integer,
   losmask                      integer,
   faults                       integer,
   faultsmask                   integer,
   alarmstemp                   integer,
   alarmstempmask               integer,
   alarmsvoltage                integer,
   alarmsvoltagemask            integer,
   alarmsbiascurrent            bigint,
   alarmsbiascurrentmask        bigint,
   alarmspower                  bigint,
   alarmspowermask              bigint,
   temp                         integer,
   voltagev25                   integer,        
   voltagev33                   integer,
   eothours                     integer,
   firmwarelevel                integer,
   serialnumber                 char(16),     
   CONSTRAINT BGQOptData_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQOpticalData for TBGQOpticalData;

CREATE TABLE TBGQOpticalChannelData
(
   location                  char(16)   NOT NULL,      
   time                      timestamp  NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   channel                   integer,
   power                     integer,
   biascurrent               integer,   
   CONSTRAINT BGQOptChData_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQOpticalChannelData for TBGQOpticalChannelData;

CREATE TABLE TBGQLinkChipEnvironment
(
   location              char(16)       NOT NULL,
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   ASICTemp              integer,
 
  CONSTRAINT BGQLChipEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQLinkChipEnvironment for TBGQLinkChipEnvironment;


CREATE TABLE TBGQNodeEnvironment
(
   location              char(16)       NOT NULL,
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   ASICTemp              integer,

   CONSTRAINT BGQNodeEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQNodeEnvironment for TBGQNodeEnvironment;

CREATE INDEX NodeEnvT on  Tbgqnodeenvironment (time desc, location);


CREATE TABLE TBGQNodeCardEnvironment
(
   location              char(16)       NOT NULL,
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   voltagev08            float,
   voltagev14            float, 
   voltagev25            float, 
   voltagev33            float, 
   voltagev120p          float, 
   voltagev15            float, 
   voltagev09            float, 
   voltagev10            float, 
   voltagev33p           float, 
   voltagev12a           float, 
   voltagev12b           float, 
   voltagev18            float, 
   voltagev25p           float, 
   voltagev12p           float, 
   voltagev18p           float, 
   tempmonitor0          integer,
   tempmonitor1          integer,
 
 CONSTRAINT BGQNCardEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQNodeCardEnvironment for TBGQNodeCardEnvironment;

CREATE TABLE TBGQIOCardEnvironment
(
   location              char(16)       NOT NULL,
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   voltagev08            float,
   voltagev14            float, 
   voltagev25            float, 
   voltagev33            float, 
   voltagev120           float, 
   voltagev15            float, 
   voltagev09            float, 
   voltagev10            float, 
   voltagev120p          float, 
   voltagev33p           float, 
   voltagev12            float, 
   voltagev18            float, 
   voltagev12p           float,  
   voltagev15p           float,           
   voltagev18p           float,   
   voltagev25p           float,             
   tempmonitor           integer, 
  
 CONSTRAINT BGQIOCardEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQIOCardEnvironment for TBGQIOCardEnvironment;

CREATE TABLE TBGQFanEnvironment
(
   location              char(16)       NOT NULL,
   time                  timestamp      NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   fannumber             integer        NOT NULL,
   RPMs                  integer        NOT NULL,

 CONSTRAINT BGQFanEnv_pk PRIMARY KEY (location, time, fannumber)
);

CREATE ALIAS BGQFanEnvironment for TBGQFanEnvironment;

CREATE TABLE TBGQCoolantEnvironment
(
   location             char(16)        NOT NULL,
   time                 timestamp       NOT NULL WITH DEFAULT CURRENT TIMESTAMP,
   inletFlowRate        INTEGER         NOT NULL,
   outletFlowRate       INTEGER         NOT NULL,
   coolantPressure      INTEGER         NOT NULL,
   inletCoolantTemp     INTEGER         NOT NULL,
   outletCoolantTemp    INTEGER         NOT NULL,
   dewpointTemp         INTEGER         NOT NULL,
   ambientTemp          INTEGER         NOT NULL,
   ambientHumidity      INTEGER         NOT NULL,
   systemPower          INTEGER         NOT NULL,
   diffPressure         INTEGER         NOT NULL,
   shutoffCause         INTEGER         NOT NULL,

   CONSTRAINT BGQCMEnv_pk PRIMARY KEY (location, time)
);

CREATE ALIAS BGQCoolantEnvironment for TBGQCoolantEnvironment;

-- RAS EVENTS

CREATE TABLE   TBGQEVENTLOG 
(
   recid integer  GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1),
   msg_id                char(8),
   category              char(16),
   component             char(16),
   Severity              char(8),
   event_time            timestamp      NOT NULL WITH DEFAULT current timestamp,
   jobid                 bigint,
   block                 char(32), 
   location              char(64),
   serialnumber          char(19),
   ecid                  char(32)       FOR BIT DATA,
   cpu                   int,
   count                 integer,
   ctlaction             varchar(256), 
   Message               varchar(1024),
   RawData               varchar(3072),
   Diags                 char(1)        NOT NULL WITH DEFAULT 'F',
    CONSTRAINT BGQELog_chk CHECK ( Diags IN ('T', 'F') ),
   QUALIFIER             char(32)
     -- boot cookie
) DATA CAPTURE CHANGES;

CREATE view BGQEVENTLOG as select  recid, msg_id, category, component, severity, event_time, jobid,
          block, location,serialnumber, ecid, cpu, count, ctlaction,  message, rawdata, qualifier from TBGQEVENTLOG;

CREATE INDEX EventLogD on  Tbgqeventlog (event_time desc, msg_id, severity);
CREATE INDEX EventLogL on  Tbgqeventlog (location);
CREATE INDEX EventLogR on  Tbgqeventlog (recid desc);
CREATE INDEX EventLogJ on  Tbgqeventlog (jobid, recid desc);
CREATE INDEX EventLogM on  Tbgqeventlog (msg_id, recid desc);
CREATE INDEX EventLogQ on  Tbgqeventlog (qualifier, recid desc);
CREATE INDEX TEALTHRESHOLDEVENTLOG ON TBGQEVENTLOG (MSG_ID ASC, LOCATION ASC, SERIALNUMBER ASC, EVENT_TIME ASC);

CREATE TABLE   TBGQDIAGSEVENTLOG 
(
   recid integer  GENERATED ALWAYS AS IDENTITY (START WITH 1, INCREMENT BY 1),
   msg_id                char(8),
   category              char(16),
   component             char(16),
   Severity              char(8),
   event_time            timestamp      NOT NULL WITH DEFAULT current timestamp,
   jobid                 bigint,
   block                 char(32), 
   location              char(64),
   serialnumber          char(19),
   ecid                  char(32)       FOR BIT DATA,
   cpu                   int,
   count                 integer,
   ctlaction             varchar(256),       
   Message               varchar(1024),
   RawData               varchar(2048),
   Diags                 char(1)        NOT NULL WITH DEFAULT 'T',
    CONSTRAINT BGQDELog_chk CHECK ( Diags IN ('T', 'F') ),
   runId                 bigint          
);

CREATE view BGQDIAGSEVENTLOG as select  recid, msg_id, category, component, severity, event_time, jobid,
          block, location,serialnumber, ecid, cpu, count, ctlaction, message, rawdata from TBGQDIAGSEVENTLOG;

CREATE INDEX DEventLogD on  Tbgqdiagseventlog (event_time desc, msg_id, severity);
CREATE INDEX DEventLogL on  Tbgqdiagseventlog (location);
CREATE INDEX DEventLogR on  Tbgqdiagseventlog (recid desc);
CREATE INDEX DEventLogJ on  Tbgqdiagseventlog (jobid, recid desc);
CREATE INDEX DEventLogM on  Tbgqdiagseventlog (msg_id, recid desc);

CREATE view BGQALLEVENTS as

 select  recid, msg_id, category, component, severity, event_time, jobid,
     block, location,serialnumber, ecid, cpu, count, ctlaction, message, rawdata, diags
  from TBGQEVENTLOG

 UNION ALL 

 select  recid, msg_id, category, component, severity, event_time, jobid,
     block, location,serialnumber, ecid, cpu, count,  ctlaction, message, rawdata, diags
 from TBGQDIAGSEVENTLOG;


-- RAS msg types
--   db2 "import from /db/schema/ddl/event_code_table.txt of DEL insert into tbgqmsgtypes" 
CREATE TABLE TBGQMSGTYPES
(
   msg_id                CHAR(8)        NOT NULL,
   category              CHAR(16)       NOT NULL,
   component             CHAR(16)       NOT NULL,
   Severity              CHAR(8)        NOT NULL,
   Message               varchar(1024),
   Description           varchar(1024),
   SvcAction             varchar(1024),
   Decoder               varchar(64),
   CtlAction             varchar(256),
   Thresholdcount        integer, 
   Thresholdperiod       char(16),
   Relevantdiags         varchar(256),
  CONSTRAINT BGQMsgTypes_pk PRIMARY KEY (msg_id)
);
CREATE ALIAS BGQMSGTYPES for TBGQMSGTYPES;


-- diags tables
CREATE TABLE TBGQDiagRuns
(
   runId                 bigint        NOT NULL,
   startTime             timestamp     NOT NULL WITH DEFAULT current timestamp,
   endTime               timestamp,
   diagStatus            char(16),
   InsertedRAS           char(1)        NOT NULL WITH DEFAULT 'F',
    CONSTRAINT BGQInsRas_chk CHECK ( InsertedRAS  IN ('T', 'F') ),
   logdir                varchar(256)  NOT NULL,
 
   CONSTRAINT BGQDiagRuns_pk PRIMARY KEY (runId)
);
CREATE ALIAS BGQDiagRuns for TBGQDiagRuns ;

CREATE TABLE TBGQDiagBlocks
(
   runId                 bigint        NOT NULL,
   blockID               char(32)      NOT NULL,
   startTime             timestamp,
   endTime               timestamp,
   hardwareStatus        char(16)      NOT NULL WITH DEFAULT 'unknown',
   
   CONSTRAINT BGQDiagBlk_fk FOREIGN KEY (runId)   
        REFERENCES TBGQDiagRuns (runId) ON DELETE CASCADE,

   CONSTRAINT BGQDiagRes_unq UNIQUE ( runId, blockID )  
);
CREATE ALIAS BGQDiagBlocks for TBGQDiagBlocks ;


CREATE TABLE TBGQDiagResults
(
   runId                 bigint        NOT NULL,
   blockID               char(32)      NOT NULL,   
   testcase              char(32)      NOT NULL,
   iteration             int           NOT NULL WITH DEFAULT 1,
   location              char(64)      NOT NULL,
   serialNumber          char(19),
   endTime               timestamp     NOT NULL WITH DEFAULT current timestamp,
   hardwareStatus        char(16)      NOT NULL WITH DEFAULT 'unknown',  
       CONSTRAINT hwSts_chk CHECK(hardwareStatus IN ('unknown', 'failed', 'marginal','success')),
   hardwareReplace       char(1)       NOT NULL WITH DEFAULT 'F',
       CONSTRAINT hwRpl_chk CHECK(hardwareReplace IN ('T', 'F')),
   logfile               varchar(256),
   analysis              varchar(4096),
   CONSTRAINT BGQDiagRes_fk FOREIGN KEY (runId)
       REFERENCES TBGQDiagRuns (runId)  ON DELETE CASCADE,
   CONSTRAINT BGQDiagRes_unq UNIQUE ( runId, blockID, testCase, location, iteration )
);
CREATE ALIAS BGQDiagResults for TBGQDiagResults ;


CREATE TABLE TBGQDiagTests
(
   runId                 bigint        NOT NULL,
   blockID               char(32)      NOT NULL,     
   testCase              char(32)      NOT NULL,
   iteration             int           NOT NULL WITH DEFAULT 1,
   startTime             timestamp,
   endTime               timestamp,
   hardwareStatus        char(16)      NOT NULL WITH DEFAULT 'unknown',
   softwareStatus        char(16)      NOT NULL WITH DEFAULT 'unknown',
   swlogFile             varchar(256),
   swAnalysis            varchar(1024),
   CONSTRAINT BGQDiagTst_fk FOREIGN KEY (runId)
        REFERENCES TBGQDiagRuns (runId) ON DELETE CASCADE,
   CONSTRAINT BGQDiagTst_unq UNIQUE ( runId, blockID, testCase,iteration )  
);
CREATE VIEW BGQDiagTests (runId, blockID, testCase, iteration, startTime, endTime, hardwareStatus, 
                          passed, marginal, unknown, failed) AS
SELECT runId, blockID, testCase, iteration, startTime, endTime, hardwareStatus,
(select count(*) from tbgqdiagresults r 
  where t.runid = r.runid and t.blockid = r.blockid and t.testcase = r.testcase and r.hardwareStatus = 'success'),
(select count(*) from tbgqdiagresults r 
  where t.runid = r.runid and t.blockid = r.blockid and t.testcase = r.testcase and r.hardwareStatus = 'marginal'),
(select count(*) from tbgqdiagresults r 
  where t.runid = r.runid and t.blockid = r.blockid and t.testcase = r.testcase and r.hardwareStatus = 'unknown'),
(select count(*) from tbgqdiagresults r 
  where t.runid = r.runid and t.blockid = r.blockid and t.testcase = r.testcase and r.hardwareStatus = 'failed')

  from  TBGQDiagTests t ;


-- control system performance measurements

CREATE TABLE TBGQComponentPerf
(
   ID                    char(32)      NOT NULL,
   COMPONENT             char(16)      NOT NULL,
   FUNCTION              char(32)      NOT NULL,
   SUBFUNCTION           char(32),
   DURATION              double,
        -- duration of this function/subfunction, in seconds
   DETAIL                char(64),
        -- block size, or other detail for this ID        
   QUALIFIER             char(32),
        -- boot cookie for a boot request, or other unique qualifier for this ID
   ENTRYDATE             timestamp      NOT NULL WITH DEFAULT current timestamp,
   MODE                  char(16) 
 );

CREATE VIEW BGQComponentPerf (id, component, function, subfunction, duration, detail, qualifier, entrydate, mode)
 as select 
    id, component, function, subfunction, duration, detail, qualifier, entrydate, mode
          from TBGQComponentPerf;

CREATE TABLE TBGQBlockAction_history
(
   blockId               char(32)       NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'F',
   action                char(1)        NOT NULL WITH DEFAULT ' ',
   entrydate             timestamp      DEFAULT current timestamp,
   creationId            integer        NOT NULL
);
CREATE ALIAS BGQBlockAction_history for TBGQBlockAction_history;


COMMIT;




