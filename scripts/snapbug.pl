#!/usr/bin/perl
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

print "Blue Gene/Q SnapBug\n";
$validLog = 0;
$SIG{"PIPE"} = 'IGNORE';
$ENV{"PATH"} = "/bin:/usr/bin";

use lib "/dbhome/bgqsysdb/sqllib/lib64";
use lib "/dbhome/bgqsysdb/sqllib/lib32";
use Getopt::Long;
use Cwd;
use DBI;
use FileHandle;
use IPC::Open2;
use Config::IniFiles;
use Sys::Hostname;
use List::Util qw(shuffle);

BEGIN
{
    if(($ENV{LD_LIBRARY_PATH} !~ /dbhome/) && (${^TAINT} == 0))
    {
        $ENV{DB2INSTANCE} = "bgqsysdb";
        $ENV{LD_LIBRARY_PATH} = join(":", split(":", $ENV{LD_LIBRARY_PATH}), "/dbhome/bgqsysdb/sqllib/lib64", "/dbhome/bgqsysdb/sqllib/lib32");
        system("$0 @ARGV");
        exit(0);
    }
}

@field = getpwuid($<);
$HOME = $field[7];

$driver    = "/bgsys/drivers/ppcfloor/";
$propertiesFile = "/bgsys/local/etc/bg.properties";
$startParms = join(" ", @ARGV);

$ENV{"SNAPBUG_ROOTPATH"} = $HOME if(!exists $ENV{"SNAPBUG_ROOTPATH"});
$outputdir = "";

$fulldebug = 0;
$MAXDEPTH = 2;
$help = 0;

$rc = GetOptions(
           "block=s"       => \$SEED_BLOCK,
           "location=s"    => \$SEED_BLOCK,
           "jobid=i"       => \$SEED_JOBID,
           "depth=i"       => \$MAXDEPTH,
           "driver=s"      => \$driver,
	   "output=s"      => \$outputdir,
	   "root=s"        => \$ENV{"SNAPBUG_ROOTPATH"},
	   "m=s"           => \$userMessageString,
	   "full!"         => \$fulldebug,
	   "help!"         => \$help,
	   "wait!"         => \$WaitConsole
    );

$help = 1 if($rc == 0);

$outputdir = $ENV{"SNAPBUG_ROOTPATH"} . "/snapbugCapture." . time() if($outputdir eq "");
$outputdir =~ s/\/$//;
if(${^TAINT} == 0)
{
    $outputdir = glob($outputdir);
}

if($help)
{
    print "Snapshots debug information from Blue Gene/Q in order to facilitate first error data capture.\n";
    print "\n";
    print "Usage:\n";
    print "  $0 --block=<blockname>  [-m \"message\" --root=<rootpath> --output=<outputdir>  --driver=<driverpath>]\n";
    print "  $0 --jobid=<jobid>      [-m \"message\" --root=<rootpath> --output=<outputdir>  --driver=<driverpath>]\n";
    print "\n\n";
    print "Environment variables:\n";
    print "  SNAPBUG_ROOTPATH=<path>   If --output is not specified, a directory containing the output will be created at this path.\n";
    print "                            If SNAPBUG_ROOTPATH is not specified, then $HOME is used.\n";
    
    exit(0);
}

if($WaitConsole)
{
    while(! -f "$outputdir/done")
    {
	sleep(1);
    }
    unlink("$outputdir/done");
    exit(0);
}

&gatherKnowledge();

sub makedir
{
    my($path) = @_;
    logCommand("mkdir $path");
    mkdir($path);
    push(@PATHS_CREATED, $path);
}

sub changedir
{
    my($path) = @_;
    logCommand("chdir $path");
    chdir($path);
}

sub createDirectory
{
    if(-d $outputdir)
    {
	opendir(TMP, $outputdir);
	@files = readdir(TMP);
	closedir(TMP);
	if($#files + 1 > 2)
	{
	    printf("Warning: Directory $outputdir is not empty!  Sleeping 15 seconds  *****\n");
	    sleep(15);
	}
    }
    
    makedir($outputdir);
    message("Created output directory at:  $outputdir\n");
    
    if(defined $userMessageString)
    {
	open(TMP, ">$outputdir/README");
	print TMP $userMessageString . "\n";
	close(TMP);
    }
}

sub setDirectoryACLs
{
    foreach $path (@PATHS_CREATED)
    {
	$cmd = logCommand("/bin/chmod +rx $path");
	system($cmd);
	$cmd = logCommand("/bin/chmod +r $path/*");
	system($cmd);
    }
}

sub openLog
{
    open(captureLog, ">$outputdir/snapbug.log");
    $validLog = 1;
    
    message("SnapBug started with parms: $0 $startParms\n");
}

sub closeLog
{
    $validLog = 0;
    close(captureLog);
}

sub message
{
    my($msg) = @_;
    print captureLog "$msg" if($validLog);
    chomp($d = `/bin/date`);
    print "$d: $msg";
}

sub logCommand
{
    my($cmd) = @_;
    if($validLog)
    {
	print captureLog "######################################\n";
	print captureLog "Command: $cmd\n";
	print captureLog "Time   : " . `/bin/date`;
	
	if($cmd =~ /bgconsole.script/)
	{
	    $scr = `/bin/cat $outputdir/bgconsole.script`;
	    $scr =~ s/^/\t/mog;
	    print captureLog "bgconsole.script contains:\n";
	    print captureLog "$scr\n";
	}
    }
    else
    {
	print "Command: $cmd\n";
    }
    return $cmd;
}

sub logError
{
    message "***********************************************\n";
    message "** ERROR\n";
    message "** \n";
    foreach $line (@_)
    {
	message "** $line\n";
    }
    message "***********************************************\n";
}

sub parseProperties
{
    my($fileName, $db) = @_;
    my $prop = new Config::IniFiles( -file => $fileName );
    my @section = $prop->Parameters("database");
    foreach my $key (@section)
    {
        $db->{$key} = $prop->val("database", $key);
    }
    
    my $adminCertFile = $prop->val("security.admin", "key_file");
    my $data = `/bin/cat $adminCertFile`;
    if($data !~ /\S/)
    {
	logError("SnapBug must be run from a userid with administrator permissions.");
	exit(-1);
    }
}

sub db2Connect
{
    exit(-1) if(!-f $propertiesFile);
    %dbPropHash = ();
    parseProperties($propertiesFile, \%dbPropHash);
    my $dsn = "dbi:DB2:" . $dbPropHash{"name"};
    $db2 = DBI->connect($dsn, $dbPropHash{"user"}, $dbPropHash{"password"});
    
    my $schemaStmt = "SET SCHEMA " . $dbPropHash{'schema_name'};
    my $schemaStmtHandle = $db2->prepare($schemaStmt);
    if (!$schemaStmtHandle->execute())
    {
	print "Result is " . getDbHandle()->errstr . ". EXITING\n";
	die "Could not execute $schemaStmtHandle";
    }
}

sub db2Close
{
    $db2->disconnect();
}

sub deallocateBlocks
{
    foreach $blockid (@ALLOCATED_BLOCKS)
    {
        $cmd = logCommand("/bin/echo free $blockid | $driver/bin/bg_console 2>&1");
	$rc = `$cmd`;
	print captureLog $rc;
    }
}

sub addBlock
{
    my($blockid) = @_;
    return -1 if($blockid eq "");
    
    if(!exists $BLOCK{$blockid})
    {
	gatherBlockData($blockid);
	
	if($BLOCK{$blockid}{"STATUS"} !~ /I|A|B/)
	{
	    message("Block '$blockid' is not allocated.  Attempting allocate\n");
	    
	    open(TMP, ">$outputdir/bgconsole.script");
	    print TMP "allocate_block $blockid diags\n";
	    print TMP "boot_block uloader=/bogus domain={id=CNK} steps=monitorMailbox\n";
	    close(TMP);
	    $cmd = logCommand("/bin/cat $outputdir/bgconsole.script | $driver/bin/bg_console 2>&1");
	    open(TMP, "$cmd |");
	    while($line = <TMP>)
	    {
		print captureLog $line;
		if($line =~ /FAIL/)
		{
		    message("Block '$blockid' was unable to be allocated.  Skipping.\n");
		    return -1;
		}
	    }
	    gatherBlockData($blockid, "redo");
	    push(@ALLOCATED_BLOCKS, $blockid);
	}
    }
    if(!exists $SUSPECT_BLOCKS{$blockid})
    {
	message("Block '$blockid' is associated, will gather information about it.\n");
	$SUSPECT_BLOCKS{$blockid} = 1;
	$knowledgeGained = 1;
    }
    
    if($BLOCK{$blockid}{"REDIRECT"})
    {
	logError("Block '$blockid' mailbox messages are being redirected in another bg_console process.",
		 "Please 'redirect_block off' and try snapbug again");
	exit(-1);
    }
    
    return 0;
}

sub addJob
{
    my($jobid) = @_;
    return if(!defined $jobid);
    if(!exists $JOB{$jobid})
    {
	if($jobid > 0)
	{
	    $cmd = "select block.blockid from bgqjob_history as job left outer join bgqblock as block on block.blockid = job.blockid where job.id = $jobid and job.qualifier = block.qualifier";
	    logCommand("db2 $cmd");
	    $sth = $db2->prepare($cmd);
	    $sth->execute();
	    
	    while($job = $sth->fetchrow_hashref())
	    {
		$SEED_BLOCK = $job->{"BLOCKID"};
		$SEED_BLOCK =~ s/\s*$//;
		message "Job $jobid is no longer running but was previously executed under the block '$SEED_BLOCK', which has remained running.\n";
	    }
	    
	    if($SEED_BLOCK eq "")
	    {
		message "Job $jobid was not executed under the active block\n" if($jobid ne "");
		return -1;
	    }
	}
	return 0;
    }
    if(!exists $SUSPECT_JOBS{$jobid})
    {
	message "Job $jobid is associated, will gather information about it.\n";
	$SUSPECT_JOBS{$jobid} = 1;
	$knowledgeGained = 1;
    }
    return 0;
}

sub protected
{
    $func = (caller(1))[3];
    $name = join(",", @_);
    return 1 if(exists $FUNC{$func}{$name});
    $FUNC{$func}{$name} = 1;
    return 0;
}

sub gatherBlockInfo
{
    return if(protected(@_));
    
    $cmd = logCommand("/bin/echo list_blocks | $driver/bin/bg_console 2>&1");
    open(TMP, "$cmd |");
     while($line = <TMP>)
    {
	last if($line =~ /OK/);
    }
    while($line = <TMP>)
    {
	print captureLog $line;
	last if($line =~ /terminating/);
	@field = split(/\s+/, $line);
	$BLOCK{$field[0]}{"STATUS"} = $field[2];
	$BLOCK{$field[0]}{"USER"}   = $field[3];
	$BLOCK{$field[0]}{"REDIRECT"} = 1 if($line =~ /redirecting/);
    }
    close(TMP);
    $knowledgeGained = 1;
}

sub verifyServiceNode
{
    $cmd = logCommand("/bin/ps auxwww | grep bgmaster_server | grep -v grep");
    $psout = `$cmd`;
    if($psout !~ /\sbgmaster_server/so)
    {
	logError("SnapBug must be executed from a service node");
	exit(-1);
    }
}

sub getVersion
{
    return if(protected(@_));
    
    $cmd = logCommand("/bin/echo version | $driver/bin/bg_console 2>&1");
    open(TMP, "$cmd |");
     while($line = <TMP>)
    {
	last if($line =~ /OK/);
    }
    $version = <TMP>;
    open(TMP, ">$outputdir/driver_level");
    print TMP $version;
    close(TMP);
    
    open(TMP, ">$outputdir/servicenode_status");
    print TMP "Hostname: ";
    print TMP `/bin/hostname`;
    print TMP "Time:     ";
    print TMP `/bin/date`;
    print TMP "\n\nMemory:\n";
    print TMP `/bin/cat /proc/meminfo`;
    print TMP "\n\nProcesses:\n";
    print TMP `/bin/ps auxwww`;
    close(TMP);
    
    $cmd = logCommand("/bin/cp /bgsys/local/etc/bg.properties $outputdir/.");
    system($cmd);
    
    $knowledgeGained = 1;
}

sub gatherJobInfo
{
    return if(protected(@_));
    
    $cmd = logCommand("$driver/bin/list_jobs");
    open(TMP, "$cmd |");
    while($line = <TMP>)
    {
	print captureLog $line;
	$line =~ s/^\s*//;
	@field = split(/\s+/, $line);
	
	next if($#field != 4);
	$JOB{$field[0]}{"STATUS"}      = $field[1];
	$JOB{$field[0]}{"EXECUTABLE"}  = $field[2];
	$JOB{$field[0]}{"BLOCK"}       = $field[3];
	$JOB{$field[0]}{"USER"}        = $field[4];
    }
    close(TMP);
    $knowledgeGained = 1;
}

sub gatherJobStatus
{
    return if(protected(@_));
    
    my($jobid) = @_;
    
    $cmd = logCommand("select * from bgqjob where id = $jobid");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    my $hash = $sth->fetchrow_hashref();
    $sth->finish();
    
    foreach $k (sort keys %{$hash})
    {
	$hash->{$k} = "" if(!defined $hash->{$k});
	printf(captureLog "%20s: %s\n", $k, $hash->{$k});
	$JOB{$jobid}{$k} = $hash->{$k};
    }
    
    $cmd = logCommand("$driver/bin/job_status --id $jobid");
    open(TMP, "$cmd |");
    my $record = 0;
    while($line = <TMP>)
    {
	print captureLog $line;
    }
    close(TMP);
    
    $knowledgeGained = 1;
}

sub dumpFlightRecorder
{
    return if(protected(@_));
    my($blockid) = @_;
    if(($BLOCK{$blockid}{"DEPTH"} >= $MAXDEPTH) && (!$fulldebug))
    {
        message sprintf("Skipping CoreProcessor snapshot on $blockid as it is %d connections away from problem\n", $BLOCK{$blockid}{"DEPTH"});
	return;
    }
    
    my $nodecount = $BLOCK{$blockid}{"NUMCNODES"} + $BLOCK{$blockid}{"NUMIONODES"};
    my $initializer = 0;
    my @filternodelist = ();
    @filternodelist = @{$BLOCK{$blockid}{"NODES"}} if(exists $BLOCK{$blockid}{"NODES"});
    $initializer = 1 if($#filternodelist >= 0);
    foreach $id (0..($nodecount-1))
    {
	$FLIGHTLOG_FETCHED{$blockid}[$id] = $initializer;
    }
    foreach $node (@filternodelist)
    {
	my $id = $BLOCK{$blockid}{"NODELOC2ID"}{$node};
	$FLIGHTLOG_FETCHED{$blockid}[$id] = 0;
    }
    
    my @flnodes = ();
    foreach $node (split(" ", $BLOCK{$blockid}{"FLIGHTLOGNODES"}))
    {
	push(@flnodes, $BLOCK{$blockid}{"NODELOC2ID"}{$node});
    }
    push(@flnodes, shuffle(0..($nodecount-1)));
    
    $starttime = time();
    while($#flnodes != -1)
    {
	my @fetchnodes = splice(@flnodes, 0, 24);
	gatherFlightLog($blockid, @fetchnodes);
	$curtime = time();
	last if($curtime - $starttime > (10*60))
    }
    foreach $node (@flnodes)
    {
	next if(exists $FLIGHTLOG_ACQUIRED{"$blockid.rank$node"});
	
	open(TMP, ">$outputdir/flightlogs/$blockid.rank$node.log");
	print TMP "Node not fetched - timeout\n";
	close(TMP);
    }
}

sub gatherFlightLog
{
    my($blockid, @ids) = @_;
    my @qualids = ();
    foreach $id (@ids)
    {
	if($FLIGHTLOG_FETCHED{$blockid}[$id] == 0)
	{
	    push(@qualids, $id);
	    $FLIGHTLOG_FETCHED{$blockid}[$id] = 1;
	}
    }
    
    @qualids = sort { $a <=> $b } @qualids;
    
    return if($#qualids == -1);
    
    message sprintf("Reading kernel flight recorder from %d nodes within block '$blockid'\n", $#qualids + 1);
    
    my @qualids_condensed = ();
    for($x=0; $x <= $#qualids; $x++)
    {
	my $max = $qualids[$x];
	my $maxindex = $x;
	for($y=$x+1; $y <= $#qualids; $y++)
	{
	    if($qualids[$y] == $max+1)
	    {
		$maxindex = $y;
		$max++;
		next;
	    }
	    $y = $#qualids+1;
	}
	if($max == $qualids[$x])
	{
	    push(@qualids_condensed, $qualids[$x]);
	}
	else
	{
	    push(@qualids_condensed, $qualids[$x] . "-$max");
	    $x = $maxindex;
	}
    }
    
    $qual = '{' . join(",", @qualids_condensed) . '} ';
    
    open(TMP, ">$outputdir/bgconsole.script");
    print TMP "select_block $blockid\n";
    print TMP "connect\n";
    print TMP "redirect_block on\n";
    
    my $dofork = "";
    if($BLOCK{$blockid}{"NUMCNODES"} == 0)
    {
	print TMP $qual . "wc cat /proc/meminfo\n";
	print TMP "sleep 1\n";
	print TMP $qual . "wc ps auxwww\n";
	print TMP "sleep 1\n";
	$dofork = " &";
    }
#    print TMP $qual . "wc cat /dev/bglog$dofork\n";
    print TMP $qual . "wc cat /dev/bglog\n";
    print TMP $qual . "sleep 1\n";
    print TMP $qual . "wc echo FINISH_FLIGHT_DUMP\n";
    print TMP "! $driver/scripts/snapbug.pl --wait --output=$outputdir --driver=$driver\n";
    close(TMP);
    
    $cmd = logCommand("/bin/cat $outputdir/bgconsole.script | $driver/bin/bg_console 2>&1");
    open(TMP, "$cmd |");
    
    my %OUTPUT = ();
    $SIG{"ALRM"} = sub { die "timeout"; };
    
    $donecount = 0;
    $startTime = time();
    eval
    {
	while($line = <TMP>)
	{
	    $curTime = time();
	    die "Error:  Unable to fetch flight recorders within 10mins" if($curTime - $startTime > (10*60));
	    alarm(15);
	    next if($line !~ /\{\d+\}/);
	    
	    ($node) = $line =~ /\{(\d+)\}/;
	    if(($line =~ /stdin.*FINISH_FLIGHT_DUMP/) || ($line =~ /:\s+FINISH_FLIGHT_DUMP/))
	    {
		$donecount++;
		last if($donecount == $#qualids+1);
	    }
	    $OUTPUT{$node} .= $line;
	};
	alarm(0);
    };
    alarm(0);
    $evalrc = $@;
    
    system("/bin/touch $outputdir/done");
    close(TMP);
    unlink("$outputdir/bgconsole.script");
    makedir("$outputdir/flightlogs");
    
    my @nodes = keys %OUTPUT;
    foreach $node (@qualids)
    {
	print captureLog "Writing flightlog for $blockid, rank $node\n";
	$FLIGHTLOG_ACQUIRED{"$blockid.rank$node"} = 1;
	open(TMP, ">$outputdir/flightlogs/$blockid.rank$node.log");
	print TMP $OUTPUT{$node} . "\n";
	if($evalrc =~ /error/i)
	{
	    print TMP "****  An error occurred while fetching this flight log, it may not be complete:\n\t'$evalrc'\n";
	    close(TMP);
	}
    }
    $knowledgeGained = 1;
}

sub copyJobExecutable
{
    return if(protected(@_));
    my($jobid) = @_;
    
    changedir($JOB{$jobid}{"WORKINGDIR"});
    makedir("$outputdir/bin");
    my $cmd = logCommand("/bin/cp " . $JOB{$jobid}{"EXECUTABLE"} . " $outputdir/bin/.");
    system($cmd);
    
    my $cmd = logCommand("/bin/cp /bgsys/drivers/ppcfloor/agents/bin/comm.elf $outputdir/bin/.");
    system($cmd);
}

sub gatherAttachedNodes
{
    return if(protected(@_));
    
    my($blockid) = @_;
    if($BLOCK{$blockid}{"NUMCNODES"} > 0)
    {
	my $sth;
        my $cmd = "select ion,ioblock from bgqcnioblockmap where cnblock like '$blockid %' and ioblockstatus = 'I'";
	
	logCommand("db2 $cmd");
	$sth = $db2->prepare($cmd);
	$sth->execute();
	while($hash = $sth->fetchrow_hashref())
	{
	    foreach $k (sort keys %{$hash})
	    {
		printf(captureLog "%20s: %s\n", $k, $hash->{$k});
		$BLOCK{$blockid}{$k} = $hash->{$k};
	    }
	    my $ioblock = $hash->{"IOBLOCK"};
	    $ioblock =~ s/\s+$//;
	    my $ion = $hash->{"ION"};
	    push(@{$BLOCK{$ioblock}{"NODES"}}, $ion);
	}
	$sth->finish();
    }    
}

sub gatherBlockData
{
    return if(protected(@_));
    my($blockid) = @_;
    my $sth;
    my $cmd;
    $cmd = "select * from bgqblock where blockid like '$blockid %'";
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    my $hash  = $sth->fetchrow_hashref();
    
    foreach $k (sort keys %{$hash})
    {
	$hash->{$k} = "" if(!defined $hash->{$k});
	printf(captureLog "%20s: %s\n", $k, $hash->{$k});
	$BLOCK{$blockid}{$k} = $hash->{$k};
    }
    $sth->finish();
}

sub gatherAttachedBlock
{
    return if(protected(@_));
    my($blockid) = @_;
    my $sth;
    my $cmd;
    
    message "Finding blocks associated with '$blockid'\n";
    gatherBlockData($blockid);
    
    if(!exists $BLOCK{$blockid}{"DEPTH"})
    {
	$BLOCK{$blockid}{"DEPTH"} = 0;
    }    
    message(sprintf("Block $blockid has %s compute nodes, %s ionodes\n", $BLOCK{$blockid}{"NUMCNODES"}, $BLOCK{$blockid}{"NUMIONODES"}));
    printf(captureLog "Block $blockid has %s compute nodes, %s ionodes\n", $BLOCK{$blockid}{"NUMCNODES"}, $BLOCK{$blockid}{"NUMIONODES"});
    
    &gatherAttachedNodes($blockid);
    
    if($BLOCK{$blockid}{"NUMCNODES"} > 0)
    {
	$cmd = "select ioblock from bgqcnioblockmap where cnblock like '$blockid %' and ioblockstatus = 'I' group by ioblock";
    }
    else
    {
	my @ionodes = @{$BLOCK{$blockid}{"NODES"}};
	if($#ionodes >= 0)
	{
	    foreach $ion (@ionodes) { $ion = "'$ion'"; }
	    $ionodestr = join(",", @ionodes);
	    $cmd = "select cnblock from bgqcnioblockmap where ioblock like '$blockid %' and cnblockstatus in ('I', 'B') and ion in ($ionodestr) group by cnblock";
	}
	else
	{
	    $cmd = "select cnblock from bgqcnioblockmap where ioblock like '$blockid %' and cnblockstatus in ('I', 'B') group by cnblock";
	}
    }
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    
    my $block;
    my @blocks = ();
    while(($block) = $sth->fetchrow())
    {
	$block =~ s/\s+$//;
	push(@blocks, $block);
	print captureLog "Block '$blockid' is attached to block '$block'\n";
	if(!exists $BLOCK{$block}{"DEPTH"})
	{
	    $BLOCK{$block}{"DEPTH"} = $BLOCK{$blockid}{"DEPTH"} + 1;
	}
    }
    $sth->finish();
    
    $knowledgeGained = 1;
    return @blocks;
}

sub copyBlockFiles
{
    return if(protected(@_));
    my($blockid) = @_;
    
    $cmd = "select mloaderimg, map.domainimg from bgqblock as block left outer join bgqdomainmap as map on map.nodeconfig = block.nodeconfig where block.blockid like '$blockid %'";
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    makedir("$outputdir/bin");
    
    while(($firmware, $domainimgs) = $sth->fetchrow())
    {
	$firmware =~ s/\s+$//;
	$domainimgs =~ s/\s+$//;
	$BLOCK{$blockid}{"IMAGES"} = join(":", ($firmware, split(",", $domainimgs)));
	my $files = join(" ", ($firmware, split(",", $domainimgs)));
	$cmd = logCommand("/bin/cp $files $outputdir/bin/.");
	system($cmd);
    }
    $sth->finish();
    $knowledgeGained = 1;
}

sub gatherRAS
{
    return if(protected(@_));
    my @blocks = @_;
    my @qualifiers = ();
    foreach $block (@blocks)
    {
	my $qual = $BLOCK{$block}{"QUALIFIER"};
	$qual =~ s/\s+$//;
	push(@qualifiers, $qual);
    }
    my $querystr = "";
    if($#qualifiers >= 0)
    {
	$querystr = "and qualifier in (" . join(",", @qualifiers) . ")";
    }
    $cmd = "select event_time, category, msg_id, substr(location,1,20), severity, substr(message,1,120) from bgqeventlog where event_time > '$DB_STARTTIME' $querystr order by event_time";
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    
    open(TMP, ">$outputdir/ras_summary.log");
    while(@row = $sth->fetchrow())
    {
	print TMP join(" ", @row) . "\n";
    }
    close(TMP);
    $sth->finish();
    $knowledgeGained = 1;
}

sub copyLogFiles
{
    return if(protected(@_));
    my @blocks = @_;
    my @query = ();
    foreach $block (@blocks)
    {
	push(@query, "blockid like '$block %'");
    }
    $querystr = join(" or ", @query);
    $cmd = "select MIN(statuslastmodified) - 10 minutes, MIN(statuslastmodified) from bgqblock where $querystr";
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    ($mintimestamp, $lasttimestamp) = $sth->fetchrow();
    $sth->finish();
    
    $DB_STARTTIME = $mintimestamp;
    $DB_LASTTIME = $lasttimestamp;
    $DB_LASTTIME++;
    $mintimestamp =~ s/(\d+)-(\d+)-(\d+)-(.*)/$1-$2-$3 $4/;
    message "Merging hlcs log files starting at $mintimestamp.\n";
    
    $hostname = hostname();
        
    my @filel = ();
    my @logprefix = qw(bgws_server mmcs_server realtime_server runjob_mux runjob_server teal_bg teal_server);
    foreach $file (@logprefix)
    {
	$file = "/bgsys/logs/BGQ/" . $hostname . "-" . $file . ".log";
	push(@filel, $file) if(-f $file);
    }
    
    $files = join(" ", @filel);
    $cmd = logCommand("$driver/hlcs/bin/log_merge --start '$mintimestamp' $files  > $outputdir/hlcs.log");
    system($cmd);
    
    foreach $blockid (@blocks)
    {
	next if($BLOCK{$blockid}{"NUMIONODES"} == 0);
	
	message "Merging ionode log files start at $mintimestamp.\n";
	$cmd = "select location from bgqioblockmap where blockid like '$blockid %'";
	logCommand("db2 $cmd");
	$sth = $db2->prepare($cmd);
	$sth->execute();
	while(($location) = $sth->fetchrow())
	{
	    $location =~ s/\s+$//;
	    print captureLog "Block $blockid contains ionode $location\n";
#	    $cmd = logCommand("$driver/bin/log_merge --start '$mintimestamp' /bgsys/logs/BGQ/$location*.log > $outputdir/$location.log");
	    $cmd = logCommand("/bin/cp /bgsys/logs/BGQ/$location*.log $outputdir/.");
	    system($cmd);
	}
	$sth->finish();
    }
    $knowledgeGained = 1;
}

sub gatherDumpAllInfo
{
    return if(protected(@_));
    my($blockid) = @_;
    if(($BLOCK{$blockid}{"DEPTH"} >= $MAXDEPTH) && (!$fulldebug))
    {
        message sprintf("Skipping gathering dump_all info on $blockid as it is %d connections away from problem\n", $BLOCK{$blockid}{"DEPTH"});
	return;
    }
    
    my $timeout = 120;
    my $starttime = time();
    my @nodes = split(" ", $BLOCK{$blockid}{"FLIGHTLOGNODES"});
    
    message sprintf("Gathering dump_all information from %d nodes in block $blockid\n", $#nodes + 1);
    makedir("$outputdir/dumpall");
    $SIG{"ALRM"} = sub { die "timeout"; };
    eval
    {
	alarm($timeout);
	foreach $node (sort @nodes)
	{
	    my $username = $BLOCK{$blockid}{"USERNAME"};
	    $username =~ s/\s+$//;
	    
	    $cmd = logCommand("$driver/scantools/bin/dump_all --location=$node --user=$username 2>&1 > $outputdir/dumpall/$node.log");
	    system($cmd);
	}
	alarm(0);
    };
    alarm(0);
    $evalrc = $@;
    if($evalrc =~ /timeout/i)
    {
        message "Gathering dump_all took longer than $timeout seconds\n";
    }
}

sub gatherTDumpInfo
{
    return if(protected(@_));
    my($blockid) = @_;
    if(($BLOCK{$blockid}{"DEPTH"} >= $MAXDEPTH) && (!$fulldebug))
    {
        message sprintf("Skipping gathering tdump info on $blockid as it is %d connections away from problem\n", $BLOCK{$blockid}{"DEPTH"});
	return;
    }
    
    my $timeout = 120;
    my $starttime = time();
    my @nodes = split(" ", $BLOCK{$blockid}{"FLIGHTLOGNODES"});
    
    message sprintf("Gathering tdump information from %d nodes in block $blockid\n", $#nodes + 1);
    makedir("$outputdir/tdump");
    $SIG{"ALRM"} = sub { die "timeout"; };
    eval
    {
	alarm($timeout);
	foreach $node (sort @nodes)
	{
            my $username = $BLOCK{$blockid}{"USERNAME"};
            $username =~ s/\s+$//;

	    $cmd = logCommand("$driver/scantools/bin/tdump --location $node --user $username --dump inbox,outbox,l1p,puea,personality 2>&1 > $outputdir/tdump/$node.log");
	    system($cmd);
	}
	alarm(0);
    };
    alarm(0);
    $evalrc = $@;
    if($evalrc =~ /timeout/i)
    {
        message "Gathering tdump took longer than $timeout seconds\n";
    }
}

sub takeCoreprocessorSnapshot
{
    return if(protected(@_));
    my($blockid) = @_;
    if(($BLOCK{$blockid}{"DEPTH"} >= $MAXDEPTH) && (!$fulldebug))
    {
        message sprintf("Skipping coreprocessor dump on $blockid as it is %d connections away from problem\n", $BLOCK{$blockid}{"DEPTH"});
	return;
    }
    
    my @filternodes_list = ();
    @filternodes_list = @{$BLOCK{$blockid}{"NODES"}} if(exists $BLOCK{$blockid}{"NODES"});
    my %filternodes = map { $_ => 1; } @filternodes_list;
    
    open(TMP, ">$outputdir/bgconsole.script");
    print TMP "select_block $blockid\n";
    print TMP "locate summary\n";
    close(TMP);
    $cmd = logCommand("/bin/cat $outputdir/bgconsole.script | $driver/bin/bg_console 2>&1");
    open(TMP, "$cmd |");
    while($line = <TMP>) { last if($line =~ /^OK/); }
    while($line = <TMP>) { last if($line =~ /^OK/); }
    
    my @query = ();
    while($line = <TMP>) 
    {
	($nodes) = $line =~ /(\S+)/;
	next if(!defined $nodes);
	
	if($nodes =~ /J/)
	{
	}
	elsif($nodes =~ /N/)
	{
	    $nodes .= "-J..";
	}
	elsif($nodes =~ /M/)
	{
	    $nodes .= "-N..-J..";
	}
	elsif(($line =~ /location:/) && ($line =~ /\<.*,.*\>/))
	{
	    ($nodes) = $line =~ /location: (\S+)/;
	}
	else
	{
	    next;
	}
	if(($nodes =~ /-J/) && ($#filternodes_list >= 0))
	{
	    next if(!exists $filternodes{$nodes});
	}
	push(@query, $nodes);
    }
    my $querystr = join('|', @query);
    my $images = $BLOCK{$blockid}{"IMAGES"};
    $BLOCK{$blockid}{"QUERYSTR"} = $querystr;
    
    @images_list = split(":", $images);
    foreach $image (@images_list)
    {
	if(${^TAINT} == 0)
	{
	    if($image =~ /boot\/linux/)
	    {
		# New procedure 2012/01/25
		# The linux image is compressed (vmlinuz) and for symbol resolution we should really use the vmlinux image.
		# However, finding the correct vmlinux image that matches the vmlinuz is not particularly straightforward.
		# Below obtains the Linux version from vmlinuz by grepping strings and then plugging that into the distro
		# path.  If Linux changes its directory layout again, this will break.  
		
		print captureLog "Compressed Linux image:  $image\n";
		$image   = Cwd::abs_path($image);
		print captureLog "Compressed Linux image (absolute):  $image\n";
		$linuxverstr = `strings -a $image | grep \"Linux version\"`;
		chomp($linuxverstr);
		($basedir) = $image =~ /(.*)\/boot\/.*/;
		($linuxver) = $linuxverstr =~ /Linux\s+version\s+(\S+)/i;
		$image = "$basedir/usr/lib/debug/lib/modules/$linuxver/vmlinux";
		print captureLog "Uncompressed Linux image:  $image\n";
		
		# End hack
	    }
	    $image = ""	if(! -r $image)
	}
    }
    foreach $jobid (keys %JOB)
    {
	if($JOB{$jobid}{"BLOCK"} eq $blockid)
	{
	    $jobexecutable = $JOB{$jobid}{"EXECUTABLE"};
	    $jobexecutable =~ s/.*\///;
	    $jobexecutable = "$outputdir/bin/$jobexecutable";
	    push(@images_list, $jobexecutable) if(-r $jobexecutable);
	    
	    # Not sure how to handle sub-block jobs in coreprocessor.  punting...
	    last;
	}
    }
    if($BLOCK{$blockid}{"NUMCNODES"} > 0)
    {
	my $agent = "$outputdir/bin/comm.elf";
	push(@images_list, "$outputdir/bin/comm.elf") if(-r $agent);
    }
    
    foreach $image (@images_list)
    {
	my $name = $image;
	$name =~ s/.*\///o;
	$image = "$outputdir/bin/$name"	if(-f "$outputdir/bin/$name");
    }
    $images = join(":", @images_list);
    $images =~ s/:+/:/og;
    
    my $username = $BLOCK{$blockid}{"USERNAME"};
    $username =~ s/\s+$//;
    $cmd = logCommand("$driver/coreprocessor/bin/coreprocessor.pl '-a=$querystr' -nogui -snapshot=$outputdir/coreprocessor_snapshot.$blockid.snap -b=$images -flightlog -user=$username 2>&1 > $outputdir/coreprocessor_snapshot.$blockid.log");
    message "CoreProcessor: $cmd\n";
    system($cmd);
    
    my $output = `/bin/cat $outputdir/coreprocessor_snapshot.$blockid.log`;
    @locations = $output =~ /Flightlog:\s*(\S+)/g;
    $BLOCK{$blockid}{"FLIGHTLOGNODES"} = join(" ", @locations);
    unlink("$outputdir/bgconsole.script");
    $knowledgeGained = 1;
}

sub getBlockLocations
{
    return if(protected(@_));
    my($blockid) = @_;
    
    open(TMP, ">$outputdir/bgconsole.script");
    print TMP "select_block $blockid\n";
    print TMP "locate ras_format\n";
    close(TMP);
    $cmd = logCommand("/bin/cat $outputdir/bgconsole.script | $driver/bin/bg_console 2>&1");
    open(TMP, "$cmd |");
    while($line = <TMP>) { last if($line =~ /^OK/); }
    while($line = <TMP>) { last if($line =~ /^OK/); }
    
    open(LOCATE, ">$outputdir/locations.$blockid");
    while($line = <TMP>)
    {
	next if($line !~ /^\{/);
	print LOCATE $line;
	if($line =~ /location:/)
	{
	    ($id, $location) = $line =~ /\{(\d+)\}.*location:\s+(\S+)/;
	    $BLOCK{$blockid}{"NODELOC2ID"}{$location} = $id;
	}
    }
    close(LOCATE);
    close(TMP);
    unlink("$outputdir/bgconsole.script");
    $knowledgeGained = 1;
}

sub processJobHistoryInfo
{
    my($job) = @_;
    open(TMP, ">>$outputdir/jobhistory");

    print TMP "*******************************************\n";
    printf(TMP "Job %d\n", $job->{ID});
    foreach $k (sort keys %{$job})
    {
	$job->{$k} = "" if(!defined $job->{$k});
	$job->{$k} =~ s/\s+$//;
	printf(TMP "   %20s: %s\n", $k, $job->{$k});
    }
    
    my $id = $job->{ID};
    my $wd = $job->{WORKINGDIR};
    open(CORE, "grep -H 'Job ID' $wd/core.* | grep $id |");
    while($line = <CORE>)
    {
	if($line =~ /Job ID/)
	{
	    ($path) = $line =~ /(\S+):/;
	    makedir("$outputdir/jobcores.$id");
	    $cmd = logCommand("/bin/cp $path $outputdir/jobcores.$id/.");
	    system($cmd);
	    
	    changedir($wd);
	    my $exec = $job->{"EXECUTABLE"};
	    $cmd = logCommand("/bin/cp $exec $outputdir/jobcores.$id/.");
	    system($cmd);
	}
    }
    close(CORE);
    close(TMP);
}

sub gatherJobHistory
{
    return if(protected(@_));
    my($blockid) = @_;
    my $qual = $BLOCK{$blockid}{"QUALIFIER"};
    $qual =~ s/\s+$//;
    message "Gathering job history on block '$blockid' since $mintimestamp.\n";

    $cmd = "select * from bgqjob where qualifier = $qual";
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    while($job = $sth->fetchrow_hashref())
    {
	processJobHistoryInfo($job);
    }
    $sth->finish();
    
    $cmd = "select * from bgqjob_history where qualifier = $qual order by entrydate desc";
    logCommand("db2 $cmd");
    $sth = $db2->prepare($cmd);
    $sth->execute();
    while($job = $sth->fetchrow_hashref())
    {
	processJobHistoryInfo($job);
    }
    $sth->finish();
    
    $knowledgeGained = 1;
}

sub resumeCores
{
    return if(protected(@_));
    my($blockid) = @_;
    
    return if($BLOCK{$blockid}{"QUERYSTR"} eq "");
    
    message "Resuming cores on $blockid\n";
    my $cmd;
    my $childpid = IPC::Open2::open2(*SCANIN, *SCANOUT, "/bgsys/drivers/ppcfloor/scantools/bin/dump_server");
    while($line = <SCANIN>) { print "dump_server_stdout: $line"; last if($line =~ /COMMAND:/); }
    
    my $nodes = $BLOCK{$blockid}{"QUERYSTR"};
    $cmd = "target $nodes\n";
    print captureLog "dump_server_stdin : $cmd";
    print SCANOUT $cmd;
    while($line = <SCANIN>) { print captureLog "dump_server_stdout: $line"; last if($line =~ /COMMAND:/); }
    
    $cmd = "run 0\n";
    print captureLog "dump_server_stdin : $cmd";
    print SCANOUT $cmd;
    while($line = <SCANIN>) { print captureLog "dump_server_stdout: $line"; last if($line =~ /COMMAND:/); }
    
    $cmd = "exit 0\n";
    print captureLog "dump_server_stdin : $cmd";
    print SCANOUT $cmd;
    while($line = <SCANIN>) { print captureLog "dump_server_stdout: $line"; last if($line =~ /COMMAND:/); }
    
    close(SCANOUT);
    close(SCANIN);
    waitpid($childpid, 0);
}

sub gatherKnowledge
{    
    foreach $STEP ("LOGIN", "QUERY", "SEED", "FIND", "SNAPSHOT", "RAS", "COREPROCESSOR",  "FLIGHTLOG", "DUMPALL", "TDUMP", "RESUMECORES", "JOBHISTORY", "SETACLS", "LOGOUT")
    {
	print "\n";
	message "Performing step '$STEP'\n";
	do
	{
	    $knowledgeGained = 0;
	    
	    if($STEP eq "QUERY")
	    {
		&getVersion();
		&gatherBlockInfo();
		&gatherJobInfo();
	    }
	    if($STEP eq "SEED")
	    {
		exit(-1) if((addJob($SEED_JOBID)) && ($SEED_JOBID ne ""));
		exit(-1) if((addBlock($SEED_BLOCK)) && ($SEED_BLOCK ne ""));
	    }
	    if($STEP eq "FIND")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    foreach $jobid (keys %JOB)
		    {
			if($blockid eq $JOB{$jobid}{"BLOCK"})
			{
			    addJob($jobid);
			}
		    }
		    
		    @blocks = &gatherAttachedBlock($blockid);
		    foreach $block (@blocks)
		    {
			addBlock($block);
		    }
		}
		
		foreach $jobid (keys %SUSPECT_JOBS)
		{
		    &gatherJobStatus($jobid);
		    foreach $blockid (keys %BLOCK)
		    {
			if($blockid eq $JOB{$jobid}{"BLOCK"})
			{
			    addBlock($blockid);
			}
		    }
		}
	    }
	    if($STEP eq "SNAPSHOT")
	    {
		my @blocks = keys %SUSPECT_BLOCKS;
		
		&copyLogFiles(@blocks);
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &getBlockLocations($blockid);
		    &copyBlockFiles($blockid);
		}
		foreach $jobid (keys %SUSPECT_JOBS)
		{
		    &copyJobExecutable($jobid);
		}
	    }
	    if($STEP eq "DUMPALL")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &gatherDumpAllInfo($blockid);
		}
	    }
	    if($STEP eq "TDUMP")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &gatherTDumpInfo($blockid);
		}
	    }
	    if($STEP eq "FLIGHTLOG")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &dumpFlightRecorder($blockid);
		}
	    }
	    if($STEP eq "COREPROCESSOR")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &takeCoreprocessorSnapshot($blockid);
		}
	    }
	    if($STEP eq "RAS")
	    {
		&gatherRAS(keys %SUSPECT_BLOCKS);
	    }
	    if($STEP eq "RESUMECORES")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &resumeCores($blockid);
		}
	    }
	    if($STEP eq "JOBHISTORY")
	    {
		foreach $blockid (keys %SUSPECT_BLOCKS)
		{
		    &gatherJobHistory($blockid);
		}
	    }
	    if($STEP eq "SETACLS")
	    {
		&setDirectoryACLs();
	    }
	    if($STEP eq "LOGIN")
	    {
		&verifyServiceNode();
		&createDirectory();
		&openLog();
		&db2Connect();
	    }
	    if($STEP eq "LOGOUT")
	    {
		&deallocateBlocks();
		&db2Close();
		message "\n";
		message "SnapBug data has been captured at: $outputdir\n";
		
		&closeLog();
	    }
	}
	while($knowledgeGained);
    }
}



# todos:
#
# File system / GPFS information???
