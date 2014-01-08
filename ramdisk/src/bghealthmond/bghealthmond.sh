#!/bin/bash
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012 All Rights Reserved                 
# US Government Users Restricted Rights - Use,                     
# duplication or disclosure restricted by GSA ADP                  
# Schedule contract with IBM Corp.                                 
#                                                                  
# This software is available to you under the                      
# GNU General Public License (GPL).                                
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#
#
# bghealthmond for BlueGene/Q Linux Distribution
# 
# File: /usr/sbin/bghealthmond.sh
#
# Author: Jay S. Bryant <jsbryant@us.ibm.com>
#
# Purpose:  This deamon will run on BlueGene Linux nodes to monitor key resources.
#           If a resource is found to be below a configured threshold the monitor throws
#           a RAS event and dumps debug data into /var/log for continued debug.
#
#           It runs, chrooted,  from /bgfs/bghealthmond to shield it from problems when 
#           the nfs root dies.
#

# Need /proc and /sys as well as some devices ...
mount -t proc none /proc
mount -t sysfs sys /sys

mknod /dev/tty c 5 0
mknod /dev/console c 5 1
mknod /dev/bgpers c 10 111
mknod /dev/bgras c 10 112
ln -s /proc/self/fd/0 /dev/stdin
ln -s /proc/self/fd/1 /dev/stdout

. /etc/init.d/functions
. /etc/sysconfig/sysinit

export TERM=linux
CONFIGFILE=/etc/sysconfig/bghealthmond
ITERATIONS=0
TOTALMEM=`cat /proc/meminfo | grep MemTotal: | tr -s " " | cut -d" " -f 2`
MEMFREE=`cat /proc/meminfo | grep MemFree: | tr -s " " | cut -d" " -f 2`
NETERRLOGGED=0
NFSERRLOGGED=0

usage () {

    echo -en "\n\nUsage: $0 [OPTIONS] (NOTE: Must be run as root)\n"
    echo -en "\t[-p <properties file> ] (default: /bgsys/local/etc/bg.properties)\n"
    echo -en "\t[-i <number of iterations to run> ] (default: infinite)"
    echo -en "\t[-h displays this help text ]\n\n"

}

# Check for required arguments and do validation.
while getopts "p:i:h" flags ; do
    case "$flags" in
	p) PROPERTIES=$OPTARG
	   ;;
	i) ITERATIONS=$OPTARG
	   ;;
	h) usage
	   exit 0
	   ;;
	*) usage
	   exit 1
	   ;;
	    
    esac
done


# Function to echo only if verbose is set.
print() {

    if [ $VERBOSE -eq 1 ] ; then

	echo -en "$1"

    fi

}


logEvent() {

    # Get BG RAS IDs
    . /dev/bgras
    DATE=`date +%Y-%m%d-%H:%M:%S`
    LOGFILE=bghealthmond-event-$DATE.log
    

    if [ $LOGPATH = "/var/log" ] ; then

	LOGPATHARR=( `mount | grep var/log | tr " " "\n" ` )
	RASLOGPATH=${LOGPATHARR[0]}/$LOGFILE

	LOG=$LOGPATH/$LOGFILE

    elif [ $LOGPATH = "/dev/console" ] ; then

	RASLOGPATH=/bgsys/logs/BGQ/$BG_LOCATION.log
	# Leave LOG blank so output is just echoed to the console.
	LOG=/dev/console
 
    else

	RASLOGPATH=$LOG
	LOG=$LOGPATH/$LOGFILE
	
    fi

    echo
    bgras $BGRAS_ID_NODE_HEALTH_MONITOR_WARNING \
	  "$1 - Debug data logged to $RASLOGPATH"

    print "\n\t\t$1 - Debug data logged to $RASLOGPATH\n"

    echo -en "#################################################################\n" >> $LOG
    echo -en "Start BG/Q Node Health Monitor Event at: $DATE\n" >> $LOG
    echo -en "#################################################################\n\n" >> $LOG

    case $2 in

	"processInfo" )

	    echo -en "/proc/meminfo output:\n\n" >> $LOG
	    cat /proc/meminfo >> $LOG

	    echo -en "\n\n\n#################################################################\n" >> $LOG
	    echo -en "Top output:\n" >> $LOG
	    echo -en "NOTE: The following system processes have been excluded to reduce output: \n" >> $LOG
	    echo -en "\tksoftirqd, migration, watchdog, events, kintegrityd, kblockd, md, md_misc, rpciod\n" >> $LOG
	    echo -en "\taio, crypto, infiniband, ib_cm, ktrotld\n\n" >> $LOG
	    top -b -n 1 | grep -v ksoftirqd | grep -v migration | grep -v watchdog | grep -v events | grep -v kintegrityd | grep -v kblockd | grep -v md | grep -v rpciod | grep -v aio | grep -v crypto | grep -v infiniband | grep -v ib_cm | grep -v grep | grep -v kthrotld  >> $LOG

	    echo -en "\n\n\n#################################################################\n" >> $LOG
	    echo -en "Number of processes: " >> $LOG
	    ps aux | wc -l >> $LOG

            # Do not want to do the following if we think NFS is gone as it can block on a filesystem
            # operation.
	    if [ $NFSERRLOGGED -eq 0 ] ; then
		echo -en "\n\n\n#################################################################\n" >> $LOG
		echo -en "Number of open file descriptors: " >> $LOG
		lsof -w | wc -l >> $LOG

	    fi

	    ;;
	
	"networkInfo" )

	    echo -en "\n\n\n#################################################################\n" >> $LOG
	    echo -en "Current network status:\n\n" >> $LOG
	    ip -s link show >> $LOG

	    echo -en "\n\n\n#################################################################\n" >> $LOG
	    echo -en "NFS Statistics: " >> $LOG
	    nfsstat -c >> $LOG

	    ;;

	* )
	    echo -en "\n\nERROR:  Unknown event requsted from bghealthmond's logEvent function.\n\n"
	    ;;

    esac

    echo -en "\n\n\n#################################################################\n" >> $LOG    
    echo -en "End BG/Q Node Health Monitor Event Log\n" >> $LOG
    echo -en "#################################################################\n" >> $LOG   


}

# Source the config file.  If it doesn't exist, throw an error.
if [ -e $CONFIGFILE ] ; then
    . $CONFIGFILE
else
    echo -en "\n\nERROR: BG Node Health Monitor cannot find the required config file: $CONFIGFILE\n\n"
    echo -en "Exiting ...\n\n"
    exit 1
fi


if [ $ENABLED -eq 1 ] ; then

    # Flag we are running by putting our pid out in /var/run
    echo "$$" > /var/run/bghealthmond.pid

    print "\n\n###################################################"
    print "\n# Starting BG/Q Node Health Monitor Daemon"
    print "\n#####################################################\n"

    print "\n\tUsing config file: $CONFIGFILE \n"
    print "\tHealth sampling frequency: $FREQUENCY seconds.\n"
    print "\tLogging to: $LOGPATH \n"
    print "\tMinimum Memory Threshold: $MEMTHRESHOLD kb\n\n"


    ############################################################
    # Validate the config values sent and do some setup.

    if [ $FREQUENCY -lt 1 ] ; then
	echo -en "\n\tERROR: bghealthmond_frequency must be greather than 0.\n\n"
	echo -en "\tExiting BG/Q Health Monitor...\n\n"
	rm -rf /var/run/bghealthmond.pid
	exit 1
    fi

    if [ ! -d $LOGPATH ] && [ $LOGPATH != "/dev/console" ] ; then

	RC=$(mkdir -p $LOGPATH 2>&1)
	
	if [ ! -z $RC ] ; then
	    echo -en "\n\tERROR: Unable to create the requested bghealthmod_log_path.\n"
	    echo -en "\tmkdir returned: $rc\n\n"
	    echo -en "\tExiting BG/Q Health Monitor...\n\n"
	    rm -rf /var/run/bghealthmond.pid
	    exit 1
	fi
    fi

    if [ $MEMFREE -lt $MEMTHRESHOLD ] ; then
	echo -en "\n\tERROR: The bghealthmod_memory_threshold $MEMTHRESHOLD kb is more than the\n"
	echo -en "\tfree memory available at start-up time ($MEMFREE kb).\n\n"
	echo -en "\tExiting BG/Q Health Monitor...\n\n"
	rm -rf /var/run/bghealthmond.pid
	exit 1
    fi

    # We are running in the background.  We want to wait for a bit and let the system finish booting before finishing
    # the set-up in case there are additional interfaces, etc that get configured later in boot.
    print "\n\tSleeping for 30 seconds while the system completes boot...\n"
    sleep 30 
    print "\n\tbghealthmond completing set-up and starting monitoring...\n"

    NUMADAPTERS=0

    # Check the list of possible adapters and build an array with what we have.  If we don't do this
    # and we lose our nfs root it causes the ip command to go out to lunch for some reason. Note that we
    # are only going to check the adapters that are configured in the personality as.  We consider those
    # to be the adapters that should be working.  Note  that we need to search different
    # fields if bonding is enabled as the ip output is slightly different.
    for ADAPTER in "ib0" "eth0" "eth1" "tor0" "bond0" ; do
	ip addr show $ADAPTER &> /dev/null
	if [ $? -ne 255 ] ; then
	    
    		if [ ! -z $BG_INTF0_NAME ] && [ $BG_INTF0_NAME == $ADAPTER ] ; then
			ADAPTERS[$NUMADAPTERS]="$ADAPTER"
			let NUMADAPTERS=$NUMADAPTERS+1
    		fi

    		if [ ! -z $BG_INTF1_NAME ] && [ $BG_INTF1_NAME == $ADAPTER ] ; then
			ADAPTERS[$NUMADAPTERS]="$ADAPTER"
			let NUMADAPTERS=$NUMADAPTERS+1
    		fi

		if [ $ADAPTER == "bond0" ] ; then
			ADAPTERS[$NUMADAPTERS]="$ADAPTER"
			let NUMADAPTERS=$NUMADAPTERS+1
			ADAPTERS[$NUMADAPTERS]="eth1"
			let NUMADAPTERS=$NUMADAPTERS+1
		fi
	fi
    done	

    CURRITER=0

    while [ $CURRITER -lt $ITERATIONS ] || [ $ITERATIONS -eq 0 ] ; do

	# Check the current memory free
	print "\n\t#################################################"
	print "\n\tStarting interation $CURRITER at: " 
	if [ $VERBOSE -eq 1 ] ; then date ; fi
	print "\n\tSampling free memory"
	
	CURRMEM=`cat /proc/meminfo | grep MemFree: | tr -s " " | cut -d" " -f 2`
	
	if [ $CURRMEM -lt $MEMTHRESHOLD ] ; then
	    
	    logEvent "Low memory threshold exceeded - Threshold: $MEMTHRESHOLD, Actual: $CURRMEM" processInfo

	fi

	# Only log a network threshold being exceeded once to avoid spamming the system
	if [ $NETERRLOGGED -eq 0 ] ; then
	    print "\n\tChecking network status - errors and dropped packets"
	    DEVICE=0
	    # Check to see if ssh is running.  If it is we can assume boot has completed.
	    # We do this to avoid a misleading message with regards to the number of dropped packets that happens if
	    # we haven't booted into the NFS space.
	    ps aux | grep "sshd" | grep -v "grep" &> /dev/null
	    NOTBOOTED=$?

	    while [ $DEVICE -lt $NUMADAPTERS ] ; do
		print "\n\t\tChecking device ${ADAPTERS[$DEVICE]} ... "

		#Pull the data out of sysfs if possible.  If not, throw a RAS event.
		if [ ! -e /sys/class/net/${ADAPTERS[$DEVICE]}/statistics ] ; then
		     bgras $BGRAS_ID_NODE_HEALTH_MONITOR_WARNING \
			   "Unable to find statistics for device ${ADAPTER[$DEVICE]} in /sys/class/net/${ADAPTERS[$DEVICE]}/statistics."
		else
		    RX_BYTES=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/rx_bytes `
		    RX_PACKETS=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/rx_packets `
		    RX_DROPPED=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/rx_dropped `
		    RX_ERRORS=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/rx_errors `
		    TX_BYTES=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/tx_bytes `
		    TX_PACKETS=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/tx_packets `
		    TX_DROPPED=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/tx_dropped `
		    TX_ERRORS=`cat /sys/class/net/${ADAPTERS[$DEVICE]}/statistics/tx_errors `
		    

		    if [ $RX_ERRORS -gt $NETERRTHRESHOLD ] ; then
			logEvent "Network RX error threshold exceeded on ${ADAPTERS[$DEVICE]} - Threshold: $NETERRTHRESHOLD, Actual: $RX_ERRORS " networkInfo
			NETERRLOGGED=1
		    fi

		    # Need to calculate a percentage of the total packets RXd for the drop threshold.
		    if [ $RX_DROPPED -gt 0 ] && [ $RX_PACKETS -gt 0 ] && [ $NOTBOOTED -eq 0 ] ; then
			let DROPPEDPERC=`echo "scale=0; $RX_DROPPED*100/$RX_PACKETS " | bc`
			if [ $DROPPEDPERC -ge $NETDRPTHRESHOLD ] ; then
			    logEvent "Network RX dropped packets threshold exceeded on ${ADAPTERS[$DEVICE]} - Threshold $NETDRPTHRESHOLD: %, Actual: $DROPPEDPERC %" networkInfo
			    NETERRLOGGED=1
			fi
		    elif [ $RX_DROPPED -gt 0 ] && [$RX_PACKETS -eq 0 ] ; then
			logEvent "All RX packets are being dropped on ${ADAPTERS[$DEVICE]} - Dropped: $RX_DROPPED, Received: $RX_PACKETS " networkInfo
			NETERRLOGGED=1
		    fi

	            if [ $TX_ERRORS -gt $NETERRTHRESHOLD ] ; then
			logEvent "Network TX error threshold exceeded on ${ADAPTERS[$DEVICE]} - Threshold: $NETERRTHRESHOLD, Actual: $TX_ERRORS " networkInfo
			NETERRLOGGED=1
		    fi

		    # Reset, just to be safe.
		    DROPPEDPERC=""

	            # Need to calculate a percentage of the total packets RXd for the drop threshold.
		    if [ $TX_DROPPED -gt 0 ] && [ $TX_PACKETS -gt 0 ] && [ $NOTBOOTED -eq 0 ] ; then
			let DROPPEDPERC=`echo "scale=0; $TX_DROPPED*100/$TX_PACKETS" | bc`
			if [ $DROPPEDPERC -ge $NETDRPTHRESHOLD ] ; then
			    logEvent "Network TX dropped packets threshold exceeded on ${ADAPTERS[$DEVICE]} - Threshold: $NETDRPTHRESHOLD %, Actual: $DROPPEDPERC %" networkInfo
			    NETERRLOGGED=1
			fi
		    # If there are dropped packets but none transmitted we need to log an event.
		    elif [ $TX_DROPPED -gt 0 ] && [ $TX_PACKETS -eq 0 ] ; then
			logEvent "All TX packets are being dropped on ${ADAPTERS[$DEVICE]} - Dropped: $TX_DROPPED, Transmitted: $TX_PACKETS " networkInfo
			NETERRLOGGED=1
		    fi

		fi

		let DEVICE=$DEVICE+1

	    done
	    
	else
	    print "\n\tNetwork error already logged, skipping further samples."
	fi

	print "\n\tChecking network status - links"
	DEVICE=0
	while [ $DEVICE -lt $NUMADAPTERS ] ; do

	    # Need to make sure we have not lost our ib link.  The config file has a number of times
	    # to retry specified and a period of time to try over. If that time is exceeded we then
	    # throw a RAS event.  We don't bother to log a healthmon event given that the 
	    # associated RAS event is fatal.
	    if [ ${ADAPTERS[$DEVICE]} == "ib0" ] ; then
		# Only check if we haven't already found a lost link.
		if [[ ! $LINKLOST =~ ${ADAPTERS[$DEVICE]} ]] ; then
			ACTIVE=0
			TRIES=0
			print "\n\t\tChecking interface ${ADAPTERS[$DEVICE]} ... "
			while [ $TRIES -lt $LINKRETRIES ] && [ $ACTIVE -eq 0 ] ; do
		    		cat /sys/class/infiniband/mlx4_0/ports/1/state | grep ACTIVE &> /dev/null
		    		if [ $? -eq 0  ] ; then
					ACTIVE=1
		    		else
					let TRIES=$TRIES+1
					sleep $LINKRETRYSLEEP
		    		fi
			done
		    
			if [ ! $TRIES -lt $LINKRETRIES ] ; then
		    		let SECONDS=$LINKRETRIES*$LINKRETRYSLEEP
                                # Check to see if we ahve lost access to the system serving /bgsys.  If so, the loss of the link is
                                # fatal.  Otherwise, just log an event.  This way we won't free blocks using bonding or that have
                                # lost a secondary network connection.
		    		ping -c 1 $BG_BGSYS_IPV4 &> /dev/null
		    		if [ $? -ne 0 ] ; then
		    			bgras $BGRAS_ID_IB_LINK_LOST "Link lost for $SECONDS or more seconds on interface ${ADAPTERS[$DEVICE]}."
		    		else
					logEvent "Link lost for $SECONDS or more seconds on IB interface ${ADAPTERS[$DEVICE]}." networkInfo
					# Record that we know we have list this link given that it is not fatal, we don't want
					# to start spamming the logs with output.
					LINKLOST="$LINKLOST ${ADAPTERS[$DEVICE]}"
		    		fi
			fi
	    	fi
	    fi
	    # Need to make sure we have not lost our ehternet link.  The config file has a number of times
	    # to retry specified and a period of time to try over. If that time is exceeded we then
	    # throw a RAS event.  We don't bother to log a healthmon event if the bgsys server can't be pinged.
	    # We throw a fatal RAS event in that case.  Otherwise, just log an event.
	    if [ ${ADAPTERS[$DEVICE]} == "eth0" ] || [ ${ADAPTERS[$DEVICE]} == "eth1" ] ; then
                # Only check if we haven't already found a lost link.
                if [[ ! $LINKLOST =~ ${ADAPTERS[$DEVICE]} ]] ; then
			ACTIVE=0
			TRIES=0
			print "\n\t\tChecking interface ${ADAPTERS[$DEVICE]} ... "
			while [ $TRIES -lt $LINKRETRIES ] && [ $ACTIVE -eq 0 ] ; do
		    		cat /sys/class/net/${ADAPTERS[$DEVICE]}/operstate | grep up &> /dev/null
		    		if [ $? -eq 0 ] ; then
					ACTIVE=1
		    		else
					let TRIES=$TRIES+1
					sleep $LINKRETRYSLEEP
		    		fi

			done

			if [ ! $TRIES -lt $LINKRETRIES ] ; then
		    		let SECONDS=$LINKRETRIES*$LINKRETRYSLEEP
		    		# Check to see if we ahve lost access to the system serving /bgsys.  If so, the loss of the link is
		    		# fatal.  Otherwise, just log an event.  This way we won't free blocks using bonding or that have
		    		# lost a secondary network connection.
		    		ping -c 1 $BG_BGSYS_IPV4 
		    		if [ $? -ne 0 ] ; then
		    			bgras $BGRAS_ID_ETHERNET_LINK_LOST "Link lost for $SECONDS or more seconds on interface ${ADAPTERS[$DEVICE]} "
		    		else
					logEvent "Link lost for $SECONDS or more seconds on ethernet interface ${ADAPTERS[$DEVICE]}" networkInfo
                                        # Record that we know we have list this link given that it is not fatal, we don't want
                                        # to start spamming the logs with output.
                                        LINKLOST="$LINKLOST ${ADAPTERS[$DEVICE]}"
		    		fi
			fi
		    
	    	fi	    

	    fi

	    let DEVICE=$DEVICE+1

	done

	print "\n\tSampling the 5 minute system load average"
	LOADARR=( `cat /proc/loadavg ` )
	FIVEMINLOADARR=( `echo "${LOADARR[1]}" | tr "." "\n" ` )
	if [ ${FIVEMINLOADARR[0]} -gt $LOADAVGTHRESHOLD ] ; then
	    logEvent "The 5 minute load average threshold exceeded - Threshold: $LOADAVGTHRESHOLD, Actual: ${FIVEMINLOADARR[0]}" processInfo
	fi

	let CURRITER=$CURRITER+1

	print "\n\tSampling the number of open file descriptors"
	OFARR=( `cat /proc/sys/fs/file-nr | tr "\t" "\n" ` )
	if [ ${OFARR[0]} -gt 0 ] && [ ${OFARR[2]} -gt 0 ] ; then
	    let OFPERC=`echo "scale=0; ${OFARR[0]}*100/${OFARR[2]}" | bc`
	    if [ $OFPERC -gt $OFTHRESHOLD ] ; then
		logEvent "The number of open file descriptors exceeds the threshold - Threshold: $OFTHRESHOLD%, Actual: $OFPERC% of total possible file descriptors" processInfo
	    fi
	fi

	if [ $NFSERRLOGGED -eq 0 ] ; then
	    print "\n\tSampling the number of NFS retransmissions"
	    NFSSTATARR=( `nfsstat -c | tr -s " " | tr "\t" " "` )
	    if [ ${NFSSTATARR[7]} -gt $RETRANSTHRESHOLD ] ; then
		# Set this first to avoid blocking on any FS ops.
		NFSERRLOGGED=1
		logEvent "The nfs retransmission threshold has been exceeded - Threshold: $RETRANSTHRESHOLD, Actual: ${NFSSTATARR[7]}" networkInfo
	    fi
	else
	    print "\n\tNFS retransmission error already logged.  Skipping further samples."
	fi


	print "\n\tChecking readability of $BG_OSDIR"
	ls $BG_OSDIR &> /dev/null &
	LS_PID=$!
	sleep $ROOTFSCHECKSLEEP
	ps aux | grep $LS_PID | grep -v grep | grep ls
	if [ $? -eq 0 ] ; then
	    bgras $BGRAS_ID_ROOT_FS_UNRESPONSIVE "Unable to read from $BG_OSDIR for 30 seconds."
	fi

	print "\n\tChecking for duplicate IP addresses"
	if [ ! -z $BG_INTF0_IPV4 ] && [ $BG_INTF0_IPV4 != "0.0.0.0" ] ; then

	    # If we are using bonding we really want to check the bonding device.
	    if [ ! -z "$BONDINGOPTS" ] ; then
		INTERFACE0="bond0"
	    else
		INTERFACE0="$BG_INTF0_NAME"
	    fi

	    print "\n\t\tChecking interface $INTERFACE0 ... "
	    arping -D -c 1 -I $INTERFACE0 $BG_INTF0_IPV4 &> /dev/null
	    RC=$?
	    # If it is non-zero somewthing went wrong.  An RC of 2 means the device is down so we don't want to throw an error.
	    if [ $RC -ne 0 ] && [ $RC -ne 2 ] ; then
		RC=$(arping -D -c 1 -I $INTERFACE0 $BG_INTF0_IPV4)
		logEvent "Possible duplicate IP address found for interface: $INTERFACE0, ip: $BG_INTF0_IPV4.  Consult the following arping output for conflicting MAC address. - arping returned: $RC" networkInfo
	    fi
	fi

	if [ ! -z $BG_INTF1_IPV4 ] && [ $BG_INTF1_IPV4 != "0.0.0.0" ] ; then
	    print "\n\t\tChecking interface $BG_INTF1_NAME ... "
	    arping -D -c 1 -I $BG_INTF1_NAME $BG_INTF1_IPV4 &> /dev/null
	    RC=$?
	    # If it is non-zero somewthing went wrong.  An RC of 2 means the device is down so we don't want to throw an error.
	    if [ $RC -ne 0 ] && [ $RC -ne 2 ] ; then
		RC=$(arping -D -c 1 -I $BG_INTF1_NAME $BG_INTF1_IPV4)
		logEvent "Possible duplicate IP address found for interface: $BG_INTF1_NAME, ip: $BG_INTF1_IPV4.  Consult the following arping output for conflicting MAC address. - arping returned: $RC" networkInfo
	    fi
	fi
	

	# Don't need to sleep if this is the last iteration.
	if [ $CURRITER -ne $ITERATIONS ] ; then
	    print "\n\tSleeping for $FREQUENCY seconds\n\n"
	    sleep $FREQUENCY
	fi
	    

    done

    if [ $VERBOSE -eq 1 ] ; then
	echo -en "\n\nBG/Q Node Health Monitor completed $ITERATIONS iterations. \n"
	echo -en "\nExiting ...\n\n"
	rm -rf /var/run/bghealthmond.pid
    fi


else

    print "\n\n\nBG/Q Node Health Monitor Daemon is not enabled.\n"
    print "\nStart-up skipped!\n\n\n"
    exit 0
fi

exit 0
