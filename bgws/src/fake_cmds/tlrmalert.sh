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
# (C) Copyright IBM Corp.  2011, 2011                              
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

db_name=bgq4x4
db_schema=bgqsysdb

teal_alert_id=1


args=`getopt -n $0 -o "i:" -- "$@"`

if [ $? -ne 0 ]
then
  echo "Usage: $0 [OPTIONS]"
  exit 1
fi

eval set -- "$args"

while true ; do case "$1" in
  -i) teal_alert_id=$2 ; shift 2 ;;
  --) shift ; break ;;
  *)  echo "Internal error!" ; exit 1 ;;
esac
done


echo "Removing alert $teal_alert_id"
echo "TEAL_LOG_DIR=" $TEAL_LOG_DIR

. ~bgqsysdb/sqllib/db2profile

db2 "CONNECT TO $db_name"
db2 "SET SCHEMA $db_schema"

db2 "DELETE FROM x_tealalertlog WHERE \"rec_id\" = $teal_alert_id AND \"state\" = 2"

if [ $? -ne 0 ]; then
    cat <<EOF
Alert '$teal_alert_id' cannot be removed.
        Reason: Alert is not closed

0 unique alerts removed
EOF

#    cat <<EOF
#
#0 unique alerts removed
#EOF

    exit 0
fi

echo "DONE"
