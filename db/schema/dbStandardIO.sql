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
-- (C) Copyright IBM Corp.  2011, 2011                              --
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
