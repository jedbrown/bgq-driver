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

args=$@

type=$1
shift

location=$1
shift

sa_action=$1
shift


. ~bgqsysdb/sqllib/db2profile

echo "cwd is" `pwd`

db2 "connect to bgq4x4"
db2 "set schema bgqsysdb"

if [ "$sa_action" == "prepare" ]; then

id=`db2 -x "select id from new table ( insert into tbgqserviceaction ( location, ServiceAction, UsernamePrepareForService, Status ) values ( '$location', 'PREPARE', 'bknudson', 'O' ) )"`

cat <<EOF
2011-04-07-07:10:29
2011-04-07-07:10:29  Start /usr/lib/jvm/java-1.6.0-ibm.ppc64/bin/java -classpath ../lib/baremetal.jar:../../xml/lib/mcserver.jar:/baremetal/lib/baremetal.jar:/xml/lib/mcserver.jar:/diags/lib/diags.jar:/xml/lib/jdom.jar:/dbhome/bgqsysdb/sqllib/java/db2java.zip:/dbhome/bgqsysdb/sqllib/java/db2jcc.jar:/dbhome/bgqsysdb/sqllib/java/sqlj.zip:/dbhome/bgqsysdb/sqllib/function:/dbhome/bgqsysdb/sqllib/java/db2jcc_license_cu.jar:. -Dcom.ibm.bluegene.baremetal.DbProperties=/bgsys/local/etc/bg.properties com.ibm.bluegene.baremetal.ServiceBulkPowerModule R00-B2-P0 prepare --logfile /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:10:29.log
2011-04-07-07:10:29
2011-04-07 07:10:30.192:   Service location:      $location
2011-04-07 07:10:30.194:   Service action:        PREPARE
2011-04-07 07:10:30.194:   User Name:             bgqadmin
2011-04-07 07:10:30.195:   Output message level:  BASIC
2011-04-07 07:10:30.195:   Log file name:         /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:10:29.log
2011-04-07 07:10:30.196:
2011-04-07 07:10:31.766:   Service Action $id opened to service BulkPowerModule R00-B2-P0.
2011-04-07 07:10:31.054:   BulkPowerEnclosure R00-B2 provides the power to the following hardware in ACTIVE status:
2011-04-07 07:10:31.055:     [R00-M1-N00, R00-M1-N01, R00-M1-N02, R00-M1-N03, R00-M1-N04, R00-M1-N05, R00-M1-N06, R00-M1-N07, R00-IC]
2011-04-07 07:10:31.605:   BulkPowerModule R00-B2-P0 is functional.
2011-04-07 07:10:31.606:   BulkPowerModule R00-B2-P1 is functional.
2011-04-07 07:10:31.606:   BulkPowerModule R00-B2-P2 is functional.
2011-04-07 07:10:31.607:   BulkPowerModule R00-B2-P3 is functional.
2011-04-07 07:10:31.608:   BulkPowerModule R00-B2-P4 is functional.
2011-04-07 07:10:31.612:   BulkPowerModule R00-B2-P5 is functional.
2011-04-07 07:10:31.613:   BulkPowerModule R00-B2-P6 is functional.
2011-04-07 07:10:31.614:   BulkPowerModule R00-B2-P7 is functional.
2011-04-07 07:10:31.615:   BulkPowerModule R00-B2-P8 is functional.
2011-04-07 07:10:31.673:   No conflicts were found for service action on BulkPowerModule R00-B2-P0.
2011-04-07 07:10:31.679:   There is currently no open sevice action for R00-B2-P0.
EOF

db2 "update tbgqserviceaction set serviceAction='PREPARE', status='A' WHERE id=$id"

cat <<EOF
2011-04-07 07:10:31.842:   Service Action $id entry for BulkPowerModule R00-B2-P0 has been updated to 'PREPARE' state with status 'A' in the database.
2011-04-07 07:10:31.842:   Service action $id started to service BulkPowerModule R00-B2-P0 by bgqadmin.
EOF

db2 "update tbgqserviceaction set status='P' WHERE id=$id"

cat <<EOF
2011-04-07 07:10:32.102:   Service Action $id entry for BulkPowerModule R00-B2-P0 has been updated to 'PREPARE' state with status 'P' in the database.
2011-04-07 07:10:32.103:   Service Action $id on BulkPowerModule R00-B2-P0 is ready to be serviced. Proceed with the service action.
2011-04-07 07:10:32.191:
2011-04-07 07:10:32.191:   ServiceBulkPowerModule ended with return code 0, Service Action $id on BulkPowerModule R00-B2-P0 is ready to be serviced. Proceed with the service action.
2011-04-07-07:10:32
2011-04-07-07:10:32  End ServiceBulkPowerModule R00-B2-P0 prepare    --logfile /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:10:29.log
2011-04-07-07:10:32
EOF

elif [ "$sa_action" == "end" ]; then


# end or force or whatever.

id=`db2 -x "SELECT MIN(id) AS id FROM bgqServiceAction WHERE location='$location' AND serviceAction='PREPARE' AND status='P'"`
id=`echo $id`

if [ $id = "-" ]; then
    echo "No current service action on $location"
    exit 1
fi

cat <<EOF
2011-04-07-07:26:46
2011-04-07-07:26:46  Start /usr/lib/jvm/java-1.6.0-ibm.ppc64/bin/java -classpath ../lib/baremetal.jar:../../xml/lib/mcserver.jar:/baremetal/lib/baremetal.jar:/xml/lib/mcserver.jar:/diags/lib/diags.jar:/xml/lib/jdom.jar:/dbhome/bgqsysdb/sqllib/java/db2java.zip:/dbhome/bgqsysdb/sqllib/java/db2jcc.jar:/dbhome/bgqsysdb/sqllib/java/sqlj.zip:/dbhome/bgqsysdb/sqllib/function:/dbhome/bgqsysdb/sqllib/java/db2jcc_license_cu.jar:. -Dcom.ibm.bluegene.baremetal.DbProperties=/bgsys/local/etc/bg.properties com.ibm.bluegene.baremetal.ServiceBulkPowerModule R00-B2-P0 end --logfile /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:26:46.log
2011-04-07-07:26:46
2011-04-07 07:26:47.111:   Service location:      $location
2011-04-07 07:26:47.113:   Service action:        END
2011-04-07 07:26:47.113:   User Name:             bgqadmin
2011-04-07 07:26:47.114:   Output message level:  BASIC
2011-04-07 07:26:47.115:   Log file name:         /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:26:46.log
2011-04-07 07:26:47.115:
2011-04-07 07:26:48.025:   Service Action $id is prepared to service BulkPowerModule R00-B2-P0. Service Action $id was started at 2011-04-07 07:10:31 by bgqadmin.
2011-04-07 07:26:48.025:   End Service Action $id to service BulkPowerModule R00-B2-P0 started by bgqadmin.
EOF

db2 "update tbgqserviceaction set serviceAction='END', status='A', UsernameEndServiceAction='bknudson', TsEndServiceAction = CURRENT_TIMESTAMP, LogFileNameEndServiceAction='/bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:26:46.log' WHERE id=$id"

cat <<EOF
2011-04-07 07:26:48.041:   Service Action $id entry for BulkPowerModule R00-B2-P0 has been updated to 'END' state with status 'A' in the database.
2011-04-07 07:26:48.816:   Update the database for BulkPowerModule R00-B2-P0 with the status of 'S'.
2011-04-07 07:26:48.900:   No hardware was replaced by this service action.
2011-04-07 07:26:49.482:
2011-12-15 14:54:14.917: ! Replaced hardware is not fully functional. A service action may be required to repair the non-functional hardware.
2011-12-15 14:54:14.919: ! Service Action 489 on IODrawer Q04-I2 failed with error code -218. Replaced hardware is not fully functional. A service action may be required to repair the non-functional hardware.
EOF


sleep 20

db2 "update tbgqserviceaction set serviceAction='END', status='E' WHERE id=$id"

cat <<EOF
2011-12-15 14:54:14.983:   Service Action 489 entry for IODrawer Q04-I2 has been updated to 'END' state with status 'E' in the database.
2011-12-15 14:54:14.989: ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
2011-12-15 14:54:14.989: ! Service action ended in error.
2011-12-15 14:54:14.989: !   Exit code: -218
2011-12-15 14:54:14.989: !   Termination reason: Service Action 489 on IODrawer Q04-I2 failed with error code -218.
2011-12-15 14:54:14.989: ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
2011-12-15 14:54:14.991:
2011-12-15 14:54:14.991:   ServiceIoDrawer ended with return code -218, Service Action 489 on IODrawer Q04-I2 failed with error code -218.
2011-12-15-14:54:15
2011-12-15-14:54:15  End ServiceIoDrawer Q04-I2 end        --diags NO   --logfile /bgsys/logs/BGQ/ServiceIoDrawer-Q04-I2-2011-1215-14:52:46.log
2011-12-15-14:54:15
EOF


elif [ "$sa_action" == "close" ]; then


id=`db2 -x "SELECT MIN(id) AS id FROM bgqServiceAction WHERE location='$location' AND status='E'"`
id=`echo $id`

if [ $id = "-" ]; then
    echo "No current service action on $location"
    exit 1
fi


cat <<EOF
2012-01-09-08:13:51
2012-01-09-08:13:51  Start /usr/lib/jvm/java-1.6.0-ibm.ppc64/bin/java -classpath baremetal.jar:../lib/baremetal.jar:/bgsys/drivers/ppcfloor/baremetal/lib/baremetal.jar:/bgsys/drivers/ppcfloor/xml/lib/mcserver.jar:/bgsys/drivers/ppcfloor/diags/lib/diags.jar:/usr/share/java/jdom.jar:/dbhome/bgqsysdb/sqllib/java/db2java.zip:/dbhome/bgqsysdb/sqllib/java/db2jcc.jar:/dbhome/bgqsysdb/sqllib/java/sqlj.zip:/dbhome/bgqsysdb/sqllib/function:/dbhome/bgqsysdb/sqllib/java/db2jcc_license_cu.jar:. -Dcom.ibm.bluegene.baremetal.DbProperties=/bgsys/local/etc/bg.properties com.ibm.bluegene.baremetal.ServiceClockCard R01-K close --logfile /bgsys/logs/BGQ/ServiceClockCard-R01-K-2012-0109-08:13:51.log
2012-01-09-08:13:51
2012-01-09 08:13:51.558:   Service location:      $location
2012-01-09 08:13:51.561:   Service action:        CLOSE
2012-01-09 08:13:51.561:   User Name:             bgqadmin
2012-01-09 08:13:51.562:   Output message level:  BASIC
2012-01-09 08:13:51.562:   Log file name:         /bgsys/logs/BGQ/ServiceClockCard-R01-K-2012-0109-08:13:51.log
2012-01-09 08:13:51.562:
2012-01-09 08:13:52.123:   Connect to mcserver at 127.0.0.1:1206.
2012-01-09 08:13:52.123:   Attempting SSL connection to mcserver at 127.0.0.1:1206.
2012-01-09 08:13:52.923:   Established SSL connection to mcserver at 127.0.0.1:1206.
2012-01-09 08:13:53.672:   Service Action $id on ClockCards $location was forced closed by bgqadmin.
EOF


db2 "update tbgqserviceaction set serviceAction='CLOSED', status='F', UsernameEndServiceAction='bknudson', TsEndServiceAction = CURRENT_TIMESTAMP, LogFileNameEndServiceAction='/bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B2-P0-2011-0407-07:26:46.log' WHERE id=$id"


cat <<EOF
2012-01-09 08:13:53.770:   Service Action $id entry for ClockCards $location has been updated to 'CLOSED' state with status 'F' in the database.
2012-01-09 08:13:53.784:
2012-01-09 08:13:53.785:   ServiceClockCard ended with return code 0, Service Action 565 on ClockCards R01-K was forced closed by bgqadmin.
2012-01-09-08:13:53
2012-01-09-08:13:53  End ServiceClockCard R01-K close           --logfile /bgsys/logs/BGQ/ServiceClockCard-R01-K-2012-0109-08:13:51.log
2012-01-09-08:13:53
EOF

fi

