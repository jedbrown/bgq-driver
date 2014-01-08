
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
# (C) Copyright IBM Corp.  2011, 2012                              
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
package bgutils;

use strict;
use Exporter;
use Carp;
use Data::Dumper;

use vars qw( @ISA @EXPORT );

@ISA    = qw(Exporter);
@EXPORT = qw(&InitBgVars PrintVars ListBlock IsBlock IsFree IsAlloc IsBooted GetBlockInfo RunJobWrapper
             $bgPropFile $ppcfloor $ppclevel %bgProps $bgSysType 
             $bgDbName $bgDbPw $bgDbUser $bgDbSchema $bgDriverDir
             $bgServerLogDir $bgMmcsLogFile
);


our $bgPropFile = "/bgsys/local/etc/bg.properties";
our $ppcfloor;
our $ppclevel;
our %bgProps;
our $bgSysType = "BGQ";
our $bgDbName;
our $bgDbPw;
our $bgDbUser;
our $bgDbSchema;
our $bgDriverDir;
our $bgServerLogDir;
our $bgMmcsLogFile;


my $bgBlock="bgqblock";
my $bgEventLog="bgqeventlog";
my $consoleCmd;
my $blklen="20";
my $blkTbl;
my $jobTbl;
my $jobHistTbl = "bgqjob_history";
my $setUserName = "set_username";
my $username = $ENV{'USER'};


sub ParseProperties # $fileName
{
    my $propFile = shift;
    open(PROPS, $propFile) || die "ERROR: Unable to open \"$propFile\"; $!\n";
    my $line;
    my $curSection = "";
    my $curName = "";
    my $curValue = "";
    while (defined($line = <PROPS>)) {
        if    ($line =~ m/^\s*\#/o) { } # skip
        elsif ($line =~ m/^\s*$/o)  { } # skip
        elsif ($line =~ m/^\s*\[\s*(\S+)\s*\]/o) {
            $curSection = $1;
        }
        elsif ($line =~ m/^\s*(\S+)\s*=\s*(\S+)/o) {
            $curName = $1;
            $curValue = $2;
            $bgProps{$curSection}->{$curName} = $curValue;
        }
    }
    close PROPS;
}



sub ParseDbRecord # <%> $ref = ParseDbRecord($strg)
{
    my @lines = split('\n', shift);
    my $ref = {};
    my ($lbl,$val);
    foreach my $line (@lines) {
        #print $line;
        ($lbl,$val) = ($line =~ m/^(\S+)\s*=\s*(\S*)/o);
        if (defined($lbl)) {
            $lbl =~ s/^_//;        
            if (defined($val)) {
                $$ref{$lbl} = $val;
            }
            else {
                $$ref{$lbl} = "";
            }
        }
    }
    return $ref;
}



sub InitBgVars
{
    $bgPropFile = $ENV{'BG_PROPERTIES_FILE'} if (defined($ENV{'BG_PROPERTIES_FILE'}));
    
    if (-f $bgPropFile) {
        $bgSysType = "BGQ";
        $ENV{'BG_PROPERTIES_FILE'} = $bgPropFile;
    }
    else {
        die "ERROR: Bgq $bgPropFile not found; $!\n";
    }
    
    ParseProperties($bgPropFile);
    
    if (defined($ENV{'DRIVERDIR'})) {
        $bgDriverDir = $ENV{'DRIVERDIR'};
    }
    else {
        $bgDriverDir = "/bgsys/drivers/ppcfloor/bin";
    }
        
    ($ppcfloor = $bgDriverDir) =~ s|/bin||;
    if (! -l $ppcfloor) { die "ERROR: $ppcfloor is not a symbolic link\n"; }
    my $link = readlink($ppcfloor); 
    if (!defined($link)) { die "ERROR: Unable to read sym link $ppcfloor; $!\n"; }
    
    $ppcfloor = $link;
    $ppclevel = $ppcfloor;
    $ppcfloor =~ s|.*/(\S+?)/ppc.*$|$1|;
    $ppclevel =~ s|.*/||g;
        
    $consoleCmd = "bg_console --properties $bgPropFile";

    $bgDbName = defined($bgProps{'database'}->{'name'})   ? $bgProps{'database'}->{'name'} : 'BGDB0';
    $bgDbUser = defined($bgProps{'database'}->{'user'})     ? $bgProps{'database'}->{'user'} : 'bgqsysdb';
    $bgDbPw = defined($bgProps{'database'}->{'password'}) ? $bgProps{'database'}->{'password'} : 'db24bgq';
    $bgDbSchema = $bgProps{'database'}->{'schema_name'};   
    
    $blkTbl = "$bgDbSchema.t$bgBlock";
    $jobTbl = "$bgDbSchema.TBGQJOB";
    
    #print Dumper(\%bgProps);
    
    $bgServerLogDir = $bgProps{'master.agent'}->{'logdir'};
    $bgMmcsLogFile = `ls -1t $bgServerLogDir/*-mmcs_server.log | head -1`; 
    chomp($bgMmcsLogFile);
}



sub PrintVars 
{
    print "\%bgProps: ";
    print Dumper(\%bgProps);
    print "\$bgPropFile         = $bgPropFile\n";
    print "\$ppcfloor           = $ppcfloor\n";
    print "\$ppclevel           = $ppclevel\n";
    print "\$bgSysType          = $bgSysType\n";
    print "\$bgDbName           = $bgDbName\n";
    print "\$bgDbPw             = $bgDbPw\n";
    print "\$bgDbUser           = $bgDbUser\n";
    print "\$bgDbSchema         = $bgDbSchema\n";
    print "\$bgDriverDir        = $bgDriverDir\n";
    print "\$bgServerLogDir     = $bgServerLogDir\n";
    print "\$bgMmcsLogFile      = $bgMmcsLogFile\n";
    print "\$bgBlock            = $bgBlock\n";
    print "\$bgEventLog         = $bgEventLog\n";
    print "\$consoleCmd         = $consoleCmd\n";
    print "\$blklen             = $blklen\n";
    print "\$blkTbl             = $blkTbl\n";
    print "\$jobTbl             = $jobTbl\n";
    print "\$jobHistTbl         = $jobHistTbl\n";
    print "\$setUserName        = $setUserName\n";
    print "\$username           = $username\n";      
}


sub RunConsoleCmd # $block, @lines 
{
    my $block = shift;
    my $tmpfile = "/tmp/$username.$block.tmp";
    my $cmd = "$consoleCmd 2>/dev/null >$tmpfile";
    open(CPIPE,"| $cmd") || die "ERROR: Unable to run \"$cmd\"; $!\n";
    my $line;
    for $line (@_) {
        print CPIPE "$line\n";
    }
    print CPIPE "quit\n";
    close(CPIPE);
    my $result = `cat $tmpfile`;
    unlink $tmpfile;
    return $result;
}



sub IsBlock # $block
{
    my $block = shift;
    my $cmd = "db2 \"connect to $bgDbName user $bgDbUser using $bgDbPw\" > /dev/null; ";
    $cmd .= "db2 \"select substr(blockid,1,16) blockid from $blkTbl where blockid = '$block'\"  > /dev/null";   
    my $rc = system($cmd);
    return ($rc == 0);
}
    


sub IsAlloc # $block
{
    my $block = shift;
    if (IsBlock($block)) {
        my $cmd = "db2 \"connect to $bgDbName user $bgDbUser using $bgDbPw\" > /dev/null; ";
        $cmd .= "db2 \"select substr(blockid,1,16) blockid,substr(status,1,1) "
                ."status,substr(owner, 1,10)owner from $blkTbl where blockid = '$block' "
                ."AND status in ('T','D','I','C','B','A') \" > /dev/null";         
        my $rc = system($cmd);
        return ($rc == 0);
    }
    return 0;
}



sub IsFree # $block
{
    my $block = shift;
    if (IsBlock($block)) {
        my $cmd = "db2 \"connect to $bgDbName user $bgDbUser using $bgDbPw\" > /dev/null; ";
        $cmd .= "db2 \"select substr(blockid,1,16) blockid,substr(status,1,1) "
                ."status,substr(owner, 1,10)owner from $blkTbl where blockid = '$block' "
                ."AND status in ('F') \" > /dev/null";         
        my $rc = system($cmd);
        return ($rc == 0);
    }
    return 0;
}
    


sub IsBooted # $block
{
    my $block = shift;
    if (IsBlock($block)) {
        my $cmd = "db2 \"connect to $bgDbName user $bgDbUser using $bgDbPw\" > /dev/null; ";
        $cmd .= "db2 \"select substr(blockid,1,16) blockid,substr(status,1,1) "
                ."status,substr(owner, 1,10)owner from $blkTbl where blockid = '$block' "
                ."AND status in ('I') \" > /dev/null";         
        my $rc = system($cmd);
        return ($rc == 0);
    }
    return 0;
}



sub ListBlock # $block
{
    my $block = shift;
    my $result = RunConsoleCmd($block, "list $bgBlock $block");
}


sub GetBlockInfo # <%> $ref = GetBlockInfo($block)
{
    my $blockName = shift;
    my $listout = ListBlock($blockName);
    my $ref = ParseDbRecord($listout);
    #print $listout;
    return $ref;
}


sub Locate # $block
{
    my $block = shift;
    my $result = RunConsoleCmd($block, "select_block $block\n{c} locate ras_format\nquit\n");
    return $result;
}


sub WriteFile # $file, $data
{
    my $file = shift;
    my $data = join '', @_;
    open FILE, ">$file" or die "ERROR: Unable to open file \"$file\" for write; $!\n";
    print FILE $data;
    close FILE;
}

sub AppendFile # $file, $data
{
    my $file = shift;
    my $data = join '', @_;
    open FILE, ">>$file" or die "ERROR: Unable to open file \"$file\" for append; $!\n";
    print FILE $data;
    close FILE;
}



sub GetExitStatus # $file
{
    my $file = shift;
    open FILE, $file or die "ERROR: Unable to open file \"$file\"; $!\n";
    my $line;
    my $rc = -1;
    while ($line = <FILE>) {
        if    ($line =~ m/ibm.runjob.client.Job:.*terminated by signal\s+(\d+)/io) {
            $rc = $1 if (($rc == -1) or ($rc == 0));
        }
        elsif ($line =~ m/ibm.runjob.client.Job:.*with status\s+(\d+)/io) {
            $rc = $1 if (($rc == -1) or ($rc == 0));
        }
        elsif ($line =~ m/^EXIT_STATUS:\s+(\d+)/io) {
            $rc = $1 if (($rc == -1) or ($rc == 0));
        }
    }
    close FILE;
    return $rc;
}



sub GetJobId # $file
{
    my $file = shift;
    open FILE, $file or die "ERROR: Unable to open file \"$file\"; $!\n";
    my $line;
    my $jobId;
    while ($line = <FILE>) {
        if  ($line =~ m/ibm.runjob.client.Job: job\s+(\d+)\s+started/io) {
            $jobId = $1;
            last;
        }
     }
    close FILE;
    if (! defined($jobId)) {
         my $now = `date +'%y%m%d_%H%M%S'`; chomp($now);
         $jobId = "unknown_jobid_$now";
    }
    return $jobId;   
}



sub GetRas4Job # $jobId
{
    my $jobId = shift;
    my $cmd = "db2 \"connect to $bgDbName user $bgDbUser using $bgDbPw\" > /dev/null; ";
    $cmd .= "db2 \"select event_time, msg_id, component, category, severity, substr(location,1,20)location ,  "
            ."message from $bgEventLog  where jobid = $jobId order by event_time\"";
    
    #print STDERR "$cmd\n";
    my $data = `$cmd`;
    $data =~ s/  */ /g;
    return $data;
}



sub RunJobWrapper # $runname, $runcmd
{
    my $runName = shift;
    my $runCmd = shift;
    
    my $block;
    if ($runCmd =~ m/--block\s+(\S+)/) {
        $block = $1;
    }
    else { die "ERROR: didn't find block argument in: $runCmd\n"; }
    
    # why do we care for this test???
    if (-f 'HALT') { die "HALT file detected; aborting RunJobWrapper $runName $runCmd\n"; }
    
    if (! IsBlock($block)) { die "ERROR: block $block is not a valid block name\n"; }
    
    my $locateFile = "$block.locate";
    WriteFile($locateFile, Locate($block));
    
    my $outFile = "run.out";
    if (defined($ENV{'QUIETLY'})) {
        $runCmd .= " 2>&1 >> $outFile ";  
    }
    else {
        $runCmd .= " 2>&1 | tee -a $outFile ";
        print STDERR "$runCmd\n";
    }
    
    my $hostname = `hostname`; chomp $hostname;
    system("echo \"# $runCmd\" > $outFile");
    system("echo \"SYSTEM = $hostname\" >> $outFile");
    system("echo \"PPCFLOOR = $ppcfloor\" >> $outFile");
    system($runCmd);
    my $rc = $?;
    print STDERR "\nRun Command RC = $rc\n";
    
    my $exitStatus = GetExitStatus($outFile);
    $exitStatus = $exitStatus ? $exitStatus : $rc;
    print STDERR "EXIT_STATUS: $exitStatus\n";
    
    my $jobId = GetJobId($outFile);
    my $rasEvents = GetRas4Job($jobId);
    $rasEvents = "RAS: $rasEvents";
    AppendFile($outFile, $rasEvents);
    my $numRasEvents = '?';
    if ($rasEvents =~ m/(\d+)\s+record\(s\) selected/i) {
        $numRasEvents = $1;
    }
     
    # ?? MPI Trace Output
    # ?? Core Files
    my @coreFiles = split ' ', `ls core.* 2>/dev/null`;
    my $numCores = scalar(@coreFiles);
    my $coreDir = "run.$jobId.cores";
    if (@coreFiles) {
        if (! -e $coreDir) {
            mkdir $coreDir or die "ERROR: Unable to make dir: $coreDir; $!\n";
        }
        my $moveNum = 0;
        foreach my $file (@coreFiles) {
            if ($moveNum++ < 10) {
                rename $file, "$coreDir/$file" or die "ERROR: Unable to move $file to $coreDir/$file; $!\n";
            }
            else {
                unlink $file;
            }
        }  
       
    }
    
    # Get any recent MMCS Log file messages on failure
    if ($exitStatus) {
        if (! -f $bgMmcsLogFile) { print STDERR "WARNING: Can't find \"$bgMmcsLogFile\"\n"; }
        else {
            system("echo \"MMCS log entries from $bgMmcsLogFile\" >> $outFile ");
            system("tail -1000 $bgMmcsLogFile | grep -e \"\$USER\" -e \"$block\" -e \"$jobId\" >> $outFile");
        }
    }
    
    my $now=`date +"%D %T"`; chomp $now;
    system("echo \"$now Job $jobId on $block ENDED with Exit status = $exitStatus, $numCores corefiles, and $numRasEvents RAS events\" | tee -a $outFile");
    
     
    return ($exitStatus ? $exitStatus : $rc);
}



1;