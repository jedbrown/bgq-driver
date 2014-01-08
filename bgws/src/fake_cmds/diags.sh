#!/bin/bash
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2010, 2011                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

runId=`date +"%s"`$RANDOM

args=$@

MIDPLANES=R00-M0
TESTS=test1

while [ $# -ne 0 ]; do
    if [ "$1" == "--stoponerror" ]; then
      sleep 5
      echo "Failed!"
      db2 "update tbgqdiagruns set endTime = CURRENT_TIMESTAMP where runId=$runId"
      exit 1
    elif [ "$1" == "--midplanes" ]; then
      shift
      MIDPLANES=`echo $1 | sed -e "s/,/ /g"`
    elif [ "$1" == "--tests" ]; then
      shift
      TESTS=`echo $1 | sed -e "s/,/ /g"`
    fi
    shift
done


echo "Blue Gene Diagnostics version 0.1.1 running on" `uname -srm` ", bgqsn.rchland.ibm.com:169.254.2.1, built on 03-06-2011 23:23:01 by clappi, compile 436."
echo "Blue Gene Diagnostics initializing..."

sleep 3

echo "Blue Gene Diagnostics starting..."
echo "Diagnostics run parameters:"
echo "        Run ID:                      $runId"
echo "        Command line arguments:      $args"
echo "        Host name:                  " `hostname`
echo "        Environment:                 FCT"
echo "        Available processors:        8"
echo "        Maximum available memory:    3.906 GiB"
echo "        Process ID:                  30385"
echo "        SN address:                  localhost"
echo "        SN control system port:      32031"
echo "        mcServer port:               1206"
echo "        Control system user:         bgqadmin"
echo "        Database name:               BGDB0"
echo "        Database schema:             bgqsysdb"
echo "        Database user:               bgqsysdb"
echo "        Test properties file:        /bgsys/drivers/ppcfloor/diags/etc/tests.properties"
echo "        BG properties file:          /bgsys/drivers/ppcfloor/diags/etc/fct/bg.nodeboard.properties"
echo "        HUP override enabled:        true"
echo "        Output directory:            /tmp/output/n00"
echo "        User specified output dir:   true"
echo "        Verbosity:                   0"
echo "        Stop on first error:         true"
echo "        Save all output:             true"
echo "        Generate block:              true"
echo "        Force block free:            false"
echo "        Floor directory:             /bgsys/drivers/ppcfloor"
echo "        Poll power regulators:       true"
echo "        Regulator polling interval:  10"
echo "        Regulator cooldown time:     60"
echo "        Check regulator levels:      true"
echo "        DRAM CE flood cancels test:  true"
echo "        FCT mode on:                 true"
echo "        FCT target type:             nodeboard"
echo "        FCT target location:         R00-M0-N00"
echo "        Create results XML file:     true"
echo "        Delete results XML file:     false"
echo "        Display results XML file:    false"
echo "        Expected Hardware level:     DD1.0"
echo "        RAS threshold:               100"
echo "        RAS rate threshold:          10"
echo "        Verify iCon/Pal versions:    false"
echo "        Change clock allowed:        true"
echo "        Tests to run:                envs"
#echo "Running envs, iteration 1, on block FCT-R00-M0-N00. Results stored in /tmp/output/n00/envs_FCT-R00-M0-N00_164410236/"
#echo "envs summary:"
#echo "        ================================================="
#echo "        Run ID:                 1103081644070385"
#echo "        Block ID:               FCT-R00-M0-N00"
#echo "        Start time:             Mar 08 16:44:10"
#echo "        End time:               Mar 08 16:44:11"
#echo "        Testcase:               envs"
#echo "        Iteration:              1"
#echo "        Passed:                 1"
#echo "        Marginal:               0"
#echo "        Failed:                 1"
#echo "           R00-M0-N00-D0 (SN 46K6052YL3210000153):  Domain 1 has a bad status. Expected status 0x90 but actually 0xF2. (0x40) Latched fault is set. (0x20) Dynamic fault is set."
#echo "               1 x  Domain 1 has a bad status. Expected status 0x90 but actually 0xF2. (0x40) Latched fault is set. (0x20) Dynamic fault is set."
#echo "        Unknown:                0"
#echo "        Hardware status:        failed"
#echo "        Internal test failure:  false"
#echo "        Output directory:       /tmp/output/n00/envs_FCT-R00-M0-N00_164410236/"
#echo "        ================================================="
#echo "Ending run since this run is configured to stop on the first error."
#echo "Diagnostics log directory: /tmp/output/n00/"


. ~bgqsysdb/sqllib/db2profile

db2 "connect to bgq4x4"
db2 "set schema bgqsysdb"
db2 "insert into tbgqdiagruns ( runId, diagStatus, logdir ) values ( $runId, 'running', '/bgsys/logs/diags' )"

function cleanup() {
  sleep 10  # it takes a few seconds to actually shut down for some reason.
  db2 "update tbgqdiagruns set endTime = CURRENT_TIMESTAMP, diagStatus = 'cancelled' where runId=$runId"
  exit
}

function cleanup_term() {
  echo "GOT TERM REQUEST TO SHUTDOWN"
  cleanup
}

function cleanup_int() {
  echo "GOT INT REQUEST TO SHUTDOWN"
  cleanup
}

trap "cleanup_term" TERM
trap "cleanup_int" INT


for MIDPLANE in $MIDPLANES; do
    BLOCKID="_DIAGS_$MIDPLANE"

    db2 "insert into tbgqdiagblocks ( runId, blockId ) values ( $runId, '$BLOCKID' )"

done

for MIDPLANE in $MIDPLANES; do
    BLOCKID="_DIAGS_$MIDPLANE"

    for TEST in $TESTS; do
      db2 "insert into tbgqdiagtests ( runId, blockId, testcase ) values ( $runId, '$BLOCKID', '$TEST' )"
    done
done

for MIDPLANE in $MIDPLANES; do
    BLOCKID="_DIAGS_$MIDPLANE"

    db2 "update tbgqdiagblocks set startTime = CURRENT_TIMESTAMP where runId=$runId AND blockId='$BLOCKID'"

    for TEST in $TESTS; do
      sleep 2
      db2 "insert into tbgqdiagresults ( runId, blockID, testcase, location, hardwareStatus ) values ( $runId, '$BLOCKID', '$TEST', '$MIDPLANE-N00', 'success' )"
      db2 "update tbgqdiagtests set endTime = CURRENT_TIMESTAMP where runId=$runId AND blockId='$BLOCKID' AND testcase='$TEST'"
    done

    BLOCKID="_DIAGS_$MIDPLANE"

    db2 "update tbgqdiagblocks set endTime = CURRENT_TIMESTAMP where runId=$runId AND blockId='$BLOCKID'"
done


db2 "update tbgqdiagruns set endTime = CURRENT_TIMESTAMP, diagStatus='completed' where runId=$runId"

echo "Blue Gene diagnostics finished."
