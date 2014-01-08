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
--  DESCRIPTION: creates BGQ views

-------------------------------------------------------------------------------------------------

--
-- ----------------
-- Views
-- ----------------
--


CREATE VIEW BGQNodeCardCount
(
   midplaneSerialNumber,
   numNodeCards
) AS
SELECT
serialNumber,
(select count(*) from TBGQNodeCard where TBGQMidplane.posinmachine=TBGQNodeCard.midplanepos)
FROM TBGQMidplane ;



CREATE VIEW BGQServiceCardCount
(
   midplaneSerialNumber,
   numServiceCards
) AS
SELECT
serialNumber,
(select count(*) from TBGQServiceCard where TBGQMidplane.posinmachine=TBGQServiceCard.midplanepos)
FROM TBGQMidplane
;


CREATE VIEW BGQNodeCount
(
   nodecardSerialNumber,
   numNodes
) AS
SELECT
serialNumber,
(select count(*) from TBGQNode where TBGQNodeCard.midplanepos=TBGQNode.midplanepos and TBGQNodeCard.position=TBGQNode.nodecardpos)
FROM TBGQNodeCard
;

CREATE VIEW BGQBasePartition
(
        BPID,
        status,
        row,
        column,
        midplane,
        machineSerialNumber,
        seqid,  torusA, torusB, torusC, torusD
) AS
SELECT posinmachine,    STATUS,

  case(substr(posinmachine,2,1)) when 'x' then -1 when 'A' then 10 when 'B' then 11 when 'C' then 12 when 'D' then 13 when 'E' then 14 when 'F' then 15 
     when 'G' then 16  when 'H' then 17  when 'I' then 18  when 'J' then 19  when 'K' then 20  when 'L' then 21  when 'M' then 22 
     when 'N' then 23  when 'O' then 24  when 'P' then 25  when 'Q' then 26  when 'R' then 27  when 'S' then 28  when 'T' then 29 
     when 'U' then 30  when 'V' then 31    
     else integer(substr(posinmachine,2,1)) end, 
  case(substr(posinmachine,3,1)) when 'y' then -1 when 'A' then 10 when 'B' then 11 when 'C' then 12 when 'D' then 13 when 'E' then 14 when 'F' then 15  
     when 'G' then 16  when 'H' then 17  when 'I' then 18  when 'J' then 19  when 'K' then 20  when 'L' then 21  when 'M' then 22 
     when 'N' then 23  when 'O' then 24  when 'P' then 25  when 'Q' then 26  when 'R' then 27  when 'S' then 28  when 'T' then 29 
     when 'U' then 30  when 'V' then 31    
     else integer(substr(posinmachine,3,1)) end, 
  case(substr(posinmachine,6,1)) when 'z' then -1  else integer(substr(posinmachine,6,1)) end, 
  machineSerialNumber, seqid, torusA, torusB, torusC, torusD

FROM TBGQMidplane;


CREATE VIEW BGQSmallBlockStatus
(
       midplanepos,
       nodecardpos,
       blockid
) as 
SELECT posinmachine, nodecardpos, a.blockid 
FROM  bgqblock a, bgqsmallblock b where a.blockid = b.blockid and  a.status <> 'F';


CREATE VIEW BGQBPBlockStatus
(
        BPID,
        status,
        seqid,
        row,
        column,
        midplane, torusA, torusB, torusC, torusD,
        machineSerialNumber,
        blockid,
        blockstatus,
        blockseqid
) AS
SELECT BP.BPID, BP.status, BP.seqid, BP.row,BP.column,BP.midplane, BP.torusA, BP.torusB, BP.torusC, BP.torusD, BP.machineSerialNumber,X.blockid,COALESCE(X.status,' '), COALESCE(X.seqid, 0)  
 FROM BGQBasePartition BP LEFT OUTER JOIN
 (SELECT BPID,a.BLOCKID,status,seqid,ROW_NUMBER() OVER (PARTITION BY bpid ORDER BY (case status when 'F' then 1 else 0 end),
statuslastmodified) AS NUMBER FROM tbgqbpblockmap a, tbgqblock b where a.blockid = b.blockid) AS X
ON BP.bpid = X.bpid and X.number=1;


CREATE VIEW BGQIOBlockStatus
(
        drawer,
        status,
        blockid,
        blockstatus,
        blockseqid
) AS
SELECT IO.location, IO.status,X.blockid,COALESCE(X.status,' '), COALESCE(X.seqid, 0)  
 FROM BGQIODrawer IO LEFT OUTER JOIN
 (SELECT location,a.BLOCKID,status,seqid,ROW_NUMBER() OVER (PARTITION BY location ORDER BY (case status when 'F' then 1 else 0 end),
statuslastmodified) AS NUMBER FROM tbgqioblockmap a, tbgqblock b where a.blockid = b.blockid) AS X
ON IO.location = X.location and X.number=1;


CREATE VIEW BGQSwitchLinks
(
 linkId,
 source,
 destination,
 status,
 seqId
)
AS SELECT RTRIM(source) CONCAT RTRIM(destination), source, destination, status, seqId  
 FROM BGQLink WHERE source in (SELECT switchID from TBGQSwitch)
        or  destination in (SELECT switchID from TBGQSwitch);


CREATE VIEW BGQLinkBlockStatus
(
        linkId,
        source,
        destination,
        blockid,
        blockstatus,
        blockseqid,
        linkstatus,
        linkseqid
) AS
SELECT SL.linkId,SL.source,SL.destination,X.blockid,COALESCE(X.status,' '), COALESCE(X.seqid, 0),SL.status,SL.seqid 
  FROM BGQSwitchLinks SL LEFT OUTER JOIN
 (SELECT linkId,a.BLOCKID,status,seqid,ROW_NUMBER() OVER (PARTITION BY linkid 
   ORDER BY (case status when 'F' then 1 else 0 end),
statuslastmodified) AS NUMBER FROM tbgqlinkblockmap a, tbgqblock b where a.blockid = b.blockid) AS X
ON SL.linkid = X.linkid and X.number=1;


CREATE VIEW BGQSwitchBlockStatus
(
        switchId,
        include,
        enableports,
        blockid,
        blockstatus,
        blockseqid,
        switchstatus,
        switchseqid
) AS
SELECT SW.switchId,X.include,X.enableports,X.blockid,COALESCE(X.status,' '), COALESCE(X.seqid, 0),SW.status,SW.seqid 
  FROM BGQSwitch SW LEFT OUTER JOIN
 (SELECT switchId,a.include,a.enableports,a.BLOCKID,status,seqid,ROW_NUMBER() OVER (PARTITION BY switchid 
   ORDER BY (case status when 'F' then 1 else 0 end),
statuslastmodified) AS NUMBER FROM tbgqswitchblockmap a, tbgqblock b where a.blockid = b.blockid) AS X
ON SW.switchid = X.switchid and X.number=1;


CREATE VIEW BGQSwitchBlockInUse
(
        switchId,
        include,
        enableports,
        blockid,
        blockstatus,
        blockseqid,
        switchstatus,
        switchseqid,
        passthru
) AS
SELECT SW.switchId,X.include,X.enableports,X.blockid,COALESCE(X.status,' '), COALESCE(X.seqid, 0),SW.status,SW.seqid , 'F'
  FROM BGQSwitch SW LEFT OUTER JOIN
 (SELECT switchId,a.include,a.enableports,a.BLOCKID,status,seqid  
  FROM tbgqswitchblockmap a, tbgqblock b, tbgqbpblockmap c  where a.blockid = b.blockid  and c.blockid = b.blockid and substr(switchid,3,6) = bpid and status <>'F')
    AS X
ON SW.switchid = X.switchid

UNION ALL 

SELECT SW.switchId,X.include,X.enableports,X.blockid,COALESCE(X.status,' '), COALESCE(X.seqid, 0),SW.status,SW.seqid , 'T'
  FROM BGQSwitch SW LEFT OUTER JOIN
 (SELECT switchid, include, enableports, b.blockid, status, seqid 
  FROM (select  switchId,a.include,a.enableports,a.BLOCKID  from tbgqswitchblockmap a left outer join tbgqbpblockmap b on a.blockid = b.blockid  and substr(switchid,3,6) = bpid  where     bpid is null)
                as X, tbgqblock b where x.blockid = b.blockid and status <> 'F' ) 
 as X
ON SW.switchid = X.switchid;





CREATE VIEW BGQBlockSize
(
        blockid,
        size
) AS
SELECT blockid, numcnodes AS size
  FROM TBGQBlock;
   

create function ncpos (nodecardpos char(3), modulo integer)
returns integer
language sql
deterministic
no external action
return 
 int(substr(nodecardpos,2,2))/modulo ;


CREATE VIEW  BGQCNIOBlockMap
(
  CNBLOCK,
  CNBLOCKSTATUS,
  ION,
  IOBLOCK,
  IOBLOCKSTATUS
) as

SELECT  x.* , blockid as IOBLOCK, status as IOBLOCKSTATUS 
 from (select a.blockid as CNBLOCK, c.status as CNBLOCKSTATUS, ion from bgqsmallblock a , bgqcniolink b , bgqblock c
          where substr(source,1,10) = posinmachine || '-' || nodecardpos and a.blockid = c.blockid
       union 
       select a.blockid as CNBLOCK, c.status as CNBLOCKSTATUS, ion from bgqbpblockmap a , bgqcniolink b , bgqblock c
          where bpid  = substr(source,1,6) and a.blockid = c.blockid) as x  
 left outer join 
      (select d.blockid,location,status from  bgqioblockmap c, bgqblock d 
          where c.blockid = d.blockid and status <> 'F' ) 
     on x.ion=location or substr(x.ion,1,6)=location ;



COMMIT;

