#!/bin/bash
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
# Licensed Materials - Property of IBM                             
# Blue Gene/Q 5765-PER 5765-PRP                                    
#                                                                  
# (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           
# US Government Users Restricted Rights -                          
# Use, duplication, or disclosure restricted                       
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               

#-------------------------------------------------------------------------------
# Creates a test execution XML for a particular test run.
#-------------------------------------------------------------------------------

# Make sure we're having safe scripting
set -o nounset
set -o errexit

usage ()
{
    echo "Logs a test execution to an XML file under /bglhome/sst/WWW/test/xml/"
    echo "Format: logXml.sh [options]"
    echo "Options:"
    echo "  -g <arg> | --groupId <arg>       Unique group ID to group tests together on the web"
    echo "                                   Format is <groupName>.<timestamp> (e.g. MPICH.2010-08-05_12.32.14)"
    echo "  -e <arg> | --exe <arg>           Executable name for the test (has to be unique)"
    echo "  -n <arg> | --np <arg>            Number of nodes"
    echo "  -p <arg> | --procsPerNode <arg>  Number of processes per node"
    echo "  -t <arg> | --numThreads <arg>    Total number of threads used per process"
    echo "  -r <arg> | --returnCode <arg>    Return code of the test (0=pass)"
    echo "  -s <arg> | --summary <arg>       Summary message of the test (1000 chars max)"
    echo "  -d <arg> | --driver <arg>        Driver used to run the test"
    echo "  -c <arg> | --compileDriver <arg> Compile driver used to build the test"
    echo "  -u <arg> | --user <arg>          User that ran the test"
    echo "  -m <arg> | --machineName <arg>   Hostname of where the test was run"
    echo "  -o <arg> | --outputFile <arg>    File containing the output from the test"
    echo "  -l <arg> | --platform <arg>      Platform that the test is for (default is BGQ)"
    echo "             --messaging           Log the output to the Messaging team's directory (default is for SST)"
    echo "  -h       | --help                Print out this help information"
}

#-------------------------------------------------------------------------------
# Set default values for all the inputs
#-------------------------------------------------------------------------------
machineName=`hostname | awk -F . '{print $1}'`
user=`whoami`
groupId=
exe=
np=1
procsPerNode=1
numThreads=1
returnCode=0
summary=
driver=""
compileDriver=""
platform="BGQ"
team="test"

if [ -d "/bgsys/drivers/ppcfloor" ]
then
    driver=`readlink -e /bgsys/drivers/ppcfloor`
else
    if [ -d "/bgsys/bgq/drivers/x86_64.floor" ]
    then
        driver=`readlink -e /bgsys/bgq/drivers/x86_64.floor`
    fi
fi
if [ "$driver" != "" ]
then
    driver=${driver%/*} # Get rid of last dir
    driver=${driver##*/} # Only keep newest last dir (driver)
else
    driver="Unknown"
    echo "WARNING: Unknown floor driver location"
fi

outputFile=/this_file_should_not_exist.log

#-------------------------------------------------------------------------------
# Parse the input parameters
#-------------------------------------------------------------------------------
set +o nounset
while [ "$1" != "" ]; do
    case $1 in
        -g | --groupId )        shift
                                groupId=$1
                                ;;
        -e | --exe )            shift
                                exe=$1
                                ;;
        -n | --np )             shift
                                np=$1
                                ;;
        -p | --procsPerNode )   shift
                                procsPerNode=$1
                                ;;
        -t | --numThreads )     shift
                                numThreads=$1
                                ;;
        -r | --returnCode )     shift
                                returnCode=$1
                                ;;
        -s | --summary )        shift
                                summary=$1
                                ;;
        -d | --driver )         shift
                                driver=$1
                                ;;
        -c | --compileDriver )  shift
                                compileDriver=$1
                                ;;
        -u | --user )           shift
                                user=$1
                                ;;
        -m | --machineName )    shift
                                machineName=$1
                                ;;
        -o | --outputFile )     shift
                                outputFile=$1
                                ;;
        -l | --platform )       shift
                                platform=$1
                                ;;
        --messaging )           team="messaging"
                                ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     echo "ERROR: $1 is an invalid option"
                                usage
                                exit 1
    esac
    shift
done
set -o nounset

#-------------------------------------------------------------------------------
# Validate parameters
#-------------------------------------------------------------------------------
if [ "$exe" == "" ]
then
    echo "ERROR: Must specify --exe"
    exit 1
fi

if [ $returnCode -eq 0 ]
then
    pass="pass"
else
    pass="fail"
fi

#-------------------------------------------------------------------------------
# If we're running on Watson or somewhere that we don't have access to log, just
# go ahead and stop.
#-------------------------------------------------------------------------------
logDir="/bglhome/sst/WWW/$team/xml"
if [ ! -d "$logDir" ]
then
    echo "ERROR: Log directory $logDir does not exist!"
    exit 1
fi

#-------------------------------------------------------------------------------
# Generate the XML filename based on the testName and current timestamp.  If
# that file already exists (i.e. this script got called for the same test within
# the same second, then wait and generate a new timestamp.
#-------------------------------------------------------------------------------
timestamp=`date "+%Y-%m-%d %H:%M:%S"`
fileTimestamp=`echo "$timestamp" | sed s/:/./g | sed s/\ /_/`
fileExe=`echo "$exe" | sed -e 's/\//~/g'`
xmlFile="$logDir/$fileExe.$fileTimestamp.xml"
while (!(set -o noclobber; (echo "" > $xmlFile) 2>/dev/null;)); do
    usleep 100000
    timestamp=`date "+%Y-%m-%d %H:%M:%S"`
    fileTimestamp=`echo "$timestamp" | sed s/:/./g | sed s/\ /_/`
    xmlFile="$logDir/$fileExe.$fileTimestamp.xml"
done

# Make sure a groupId is set
if [ "$groupId" == "" ]
then
    groupId="$exe.$timestamp"
fi

#-------------------------------------------------------------------------------
# Create the XML file
#-------------------------------------------------------------------------------
echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" > $xmlFile
echo "<?xml-stylesheet type=\"text/xsl\" href=\"testExecution.xsl\"?>" >> $xmlFile
echo "<TestExecution>" >> $xmlFile
echo "  <GroupId>$groupId</GroupId>" >> $xmlFile
echo "  <TestName>$exe</TestName>" >> $xmlFile
echo "  <Platform>$platform</Platform>" >> $xmlFile
echo "  <Driver>$driver</Driver>" >> $xmlFile
echo "  <CompileDriver>$compileDriver</CompileDriver>" >> $xmlFile
echo "  <Machine>" >> $xmlFile
echo "    <Hostname>$machineName</Hostname>" >> $xmlFile
echo "  </Machine>" >> $xmlFile
echo "  <ProcsPerNode>$procsPerNode</ProcsPerNode>" >> $xmlFile
echo "  <NumNodes>$np</NumNodes>" >> $xmlFile
echo "  <NumThreads>$numThreads</NumThreads>" >> $xmlFile
echo "  <Result pass=\"$pass\" rc=\"$returnCode\">$summary</Result>" >> $xmlFile
echo "  <Timestamp>$timestamp</Timestamp>" >> $xmlFile
echo "  <User>$user</User>" >> $xmlFile
echo -n "  <FullOutput><![CDATA[" >> $xmlFile
if  [ -f "$outputFile" ]
then
    # use tr to remove any non-print characters that cause XML parsing issues (leave '\n' alone)
    tr -cd "[:print:]\n\t" < $outputFile >> $xmlFile
fi
echo "]]></FullOutput>" >> $xmlFile
echo "</TestExecution>" >> $xmlFile

chmod 664 $xmlFile
chown :bgq $xmlFile

echo "INFO: Test execution logged to $xmlFile"

