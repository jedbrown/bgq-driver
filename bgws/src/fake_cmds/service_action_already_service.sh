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

cat <<EOF
2011-04-07-07:17:23
2011-04-07-07:17:23  Start /usr/lib/jvm/java-1.6.0-ibm.ppc64/bin/java -classpath ../lib/baremetal.jar:../../xml/lib/mcserver.jar:/baremetal/lib/baremetal.jar:/xml/lib/mcserver.jar:/diags/lib/diags.jar:/xml/lib/jdom.jar:/dbhome/bgqsysdb/sqllib/java/db2java.zip:/dbhome/bgqsysdb/sqllib/java/db2jcc.jar:/dbhome/bgqsysdb/sqllib/java/sqlj.zip:/dbhome/bgqsysdb/sqllib/function:/dbhome/bgqsysdb/sqllib/java/db2jcc_license_cu.jar:. -Dcom.ibm.bluegene.baremetal.DbProperties=/bgsys/local/etc/bg.properties com.ibm.bluegene.baremetal.ServiceBulkPowerModule R00-B0-P8 prepare --logfile /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B0-P8-2011-0407-07:17:23.log
2011-04-07-07:17:23
2011-04-07 07:17:23.565:   Service location:      R00-B0-P8
2011-04-07 07:17:23.568:   Service action:        PREPARE
2011-04-07 07:17:23.568:   User Name:             bgqadmin
2011-04-07 07:17:23.569:   Output message level:  BASIC
2011-04-07 07:17:23.569:   Log file name:         /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B0-P8-2011-0407-07:17:23.log
2011-04-07 07:17:23.569:
2011-04-07 07:17:24.400: ! Prepare BulkPowerModule R00-B0-P8 for service failed with return code -203, There is an open Service Action for R00-B0-P1. Service Action 220 was started at 2011-04-07 07:17:08 by bgqadmin with a state of PREPARE and status of P.
2011-04-07 07:17:24.401:
2011-04-07 07:17:24.401:   ServiceBulkPowerModule ended with return code -203, There is an open Service Action for R00-B0-P1. Service Action 220 was started at 2011-04-07 07:17:08 by bgqadmin with a state of PREPARE and status of P.
2011-04-07-07:17:24
2011-04-07-07:17:24  End ServiceBulkPowerModule R00-B0-P8 prepare    --logfile /bgsys/logs/BGQ/ServiceBulkPowerModule-R00-B0-P8-2011-0407-07:17:23.log
2011-04-07-07:17:24
EOF
