#!/usr/bin/perl -w
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
# (C) Copyright IBM Corp.  2010, 2012                              
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

use strict;

my $testComponant = "bgpm";

my $usePmt = (defined($ENV{'PMT_PATH'}));
my $runJob = "runjob";
#my $runJob = "runmmcslite";

# allow run on mmcslite node
my $nodesInBlock = 32;
if ($runJob ne "runjob") {
    $nodesInBlock = 1;
}


my $tmpdir;
if ($usePmt) { $tmpdir = "pmtTmp"; }
else         { $tmpdir = "tmp"; }


our ( $thisCmdPath, $thisCmdName, @ldirs );
BEGIN {   
    my $skip;
    ( $thisCmdPath, $skip, $thisCmdName ) = ( $0 =~ m|(([^/]*/)*)(.+)| );
    my $cwd = `pwd -P`; chomp($cwd);
    if ($thisCmdPath =~ m|^\./|) { $thisCmdPath = $cwd; }
    else {
        chdir($thisCmdPath) or die "ERROR: unable to chdir \"$thisCmdPath\"; $!\n";
        $thisCmdPath = `pwd -P`; chomp($thisCmdPath);
        chdir($cwd) or die "ERROR: unable to chdir \"$cwd\"; $!\n";
    }
    $thisCmdPath =~ s|/$||;
    $ENV{'PATH'} = "$thisCmdPath:" . $ENV{'PATH'};
}


use Data::Dumper;
use Time::HiRes qw(gettimeofday tv_interval);
use POSIX ":sys_wait_h";

use lib @ldirs;    # add this dir to module search path
use RunTestUtils; 
use bgutils; 

print "current PATH=", $ENV{'PATH'}, "\n\n";
my $cwd = `pwd -P`; chomp($cwd);


my $block = $ARGV[0];
if (@ARGV < 1) {
    die "ERROR: pass R00-M0-N08 or other location on which to run - assumes block of 32 nodes.\n" . 
        "If you wish to run mmcslite on a single node - change $runJob at the top of the scripts\n" .
        "and pass in node name for as (for example):  Q00-I2:J04\n";
}

InitBgVars();
if (!IsBlock($block)) {
    die "ERROR: block name \"$block\" was not found\n";
}


my $startTime     = &GetTime();
my @skipTests     = qw( stress/spi-dcr-slv-op stress/spiapply func/specbkt_sim 
                        mpi/common mpi/minibm-pingpong mpi/spi-nw-pingpong
                        func/consistant_thds/prc63_thd01 
                        internals/globalvar_init/linux-dyn                      
                       );
my @skipGrps      = qw( experiment test_utils se-tm tmp pmtTmp fails-on-purpose );
my @multiNodeGrps = qw( mpi stress );

my $runStampID = `date +"%Y%m%d-%H%M%S"`; chomp $runStampID;
#print "runStampID = $runStampID\n";

# Check whether the given directory should be included in regression test.
sub InclInTest # $dir, @listToSkip
{
    my $dir = shift;
    #print "inclintest: $dir\n";
    my $incl = 1;
    for my $check (@_) {
        if ($dir eq $check) {
            $incl = 0;
        }
    }
    return $incl;
}




###################################################
# Walk dir tree to find testcases to run.
my @tests;
my @groupdirs;
my @dirs = split ' ', `ls`;

foreach my $dir (@dirs) {
    if (-d $dir) {
        if (InclInTest($dir, @skipGrps)) {
            push @groupdirs, $dir;
        }
    }
}
print "TestGroups: @groupdirs\n\n";


foreach my $groupdir (@groupdirs) { 
    my @dirs = split ' ', `ls $groupdir`;
    foreach my $dir (@dirs) {
        my $test = "$groupdir/$dir";
        if (-d $test) {
            if (InclInTest($test, @skipTests)) {
                push @tests, $test;
            }
        }
    }
}
print "Tests: @tests\n\n";


my @expandedTests;
foreach my $testdir (@tests) { 
    my @dirs = split ' ', `ls $testdir`;
    my @vars = ();
    foreach my $dir (@dirs) {
        my $var = "$testdir/$dir/Makefile";
        #if (($dir !~ m/FCTest/o) && (-d $var)) {
        if (-f $var) {
            $var =~ s|/Makefile||o;
            push @vars, $var;
        }
    }
    if (@vars) {
        foreach my $var (@vars) {
            if (InclInTest($var, @skipTests)) {
                push @expandedTests, $var;
            }
        }
    }
    else {
        push @expandedTests, $testdir;
    }
}
@tests = @expandedTests;
print "Incl variations: @tests\n\n";

print "Total Variations: ", scalar(@tests), "\n\n";


# split out list of multinode tests
my @multiNodeTests;
my @singleNodeTests;
foreach my $test (@tests) {
    if (map {$test =~ m|^$_/|} @multiNodeGrps) {
        push @multiNodeTests, $test;
    }
    else {
        push @singleNodeTests, $test;
    }
}

# minimize list for debug
#@multiNodeTests = ();
#@singleNodeTests = ("perf/bgpm-ops1"); 
#@singleNodeTests = ($singleNodeTests[0]); #("func/ovfFastThd"); 


#############################################
# Set up evironment to run selected tests
select STDERR; $| = 1; # make unbuffered
select STDOUT; $| = 1; # make unbuffered

my $logPrint = "runregress-mmcs.print.log";
unlink "$logPrint.old";
rename($logPrint,"$logPrint.old");

open LOG, ">$logPrint" or die "ERROR: Can't open $logPrint\n";

mkdir $tmpdir;
$tmpdir .= '/';



sub TmpLogFile # $test
{
    my $tmp = shift;
    $tmp =~ s|/|--|g;
    return $tmp;
}


sub SubmitTest # $test, $block, $logfile
{
    my $test = shift;
    my $block = shift;
    my $logfile = shift;
    
    #print STDERR "logfile=$logfile\n";
    
    my $pid = fork();
    if ($pid) { # in parent
        #print STDERR "fork $test, $block, pid=$pid, parent=$$\n";
    } 
    elsif ($pid == 0) {  # in child
        unlink $logfile;
        open STDOUT, ">$logfile" or die "ERROR: Can't open $logfile; $!\n"; 
        select STDOUT; $| = 1; # make unbuffered
        open STDERR, ">&STDOUT" or die "ERROR: Can't dup STDOUT; $!";

        if ($usePmt) {
            my $cfg;
            ($cfg = $test) =~ s|/|.|g;
            $cfg = "$testComponant.$cfg";
            my $cwd = `pwd -P`; chomp($cwd);
            my $chkCmd = FindLeafFile("pmtCheck.pl", "$cwd/$test");
            my $postCmd = FindLeafFile("pmtProc.pl", "$cwd/$test"); $postCmd = "-p $postCmd" if ($postCmd ne "");
            chdir $tmpdir or die "ERROR: Can't cd to $tmpdir; $!\n";
            my $cmd = "PMTRun -x \"cd $cwd/$test && make $runJob block=$block\" -f $cfg -e $runStampID -v $chkCmd $postCmd; pwd; ls -l";
            print STDERR "pmtCmd: $cmd\n"; 
            exec("$cmd");
        }
        else {
            exec("cd $test; make runjob block=$block");
        }
    } 
    else {
        die "ERROR: Could not fork: $!\n";
    }
    return $pid; 
}



sub SignalHandler
{
    my $sig = shift;
    print STDERR "runregress Signal $sig received\n";
}

$SIG{'TSTP'} = \&SignalHandler;
$SIG{'TTIN'} = \&SignalHandler;
$SIG{'TTOU'} = \&SignalHandler;

sub SubmitWaitForTests # $@tests, $@nodes, $maxParallel
{
    my @tests = @{$_[0]};
    my @nodes = @{$_[1]};
    my $maxParallel = MIN($_[2], scalar(@nodes));
    
    my $nextSubTest = 0;
    my $nextWaitTest = 0;
    my @childs = ();
    my $maxLoops = 5;
    while (($nextSubTest < @tests) || (@childs > 0)) {
        # more to submit?
        if ((@childs < $maxParallel) && ($nextSubTest < @tests)) {
            my $test = $tests[$nextSubTest];
            my $node = $nodes[$nextSubTest % @nodes];
            my $log = $tmpdir . TmpLogFile($test);
            print STDERR "\n### Submit $test to node=$node.  submits=$nextSubTest, cmplts=$nextWaitTest ...\n";        
            push @childs, SubmitTest($test, $node, $log);
            #print STDERR "child pids=@childs\n";
            $nextSubTest++;  
        } 
        if (@childs > 0) {
            my $retry;
            do {   
                $retry = 0;           
                my $pid = $childs[0];
                #print STDERR "I'm here pid=$pid\n";              
                if (waitpid($pid, WNOHANG)) { 
                    my $rc = $?;
                    print STDERR "\n###   Completed rc=$rc for $tests[$nextWaitTest].  submits=$nextSubTest, cmplts=$nextWaitTest\n";
                    $nextWaitTest++;
                    shift @childs;
                    $retry = 1;
                }
            } while (($retry) && (@childs > 0));
        }
        sleep 1; # allow ctr-C to work
    }
}



my @childs;
my @nodes;
foreach my $nodeNum (0 .. $nodesInBlock-1) {
    my $node = sprintf("$block:J%02d", $nodeNum);
    push @nodes, $node;
}
if ($block =~ m/^(\S+)\:(\S+)/) {
    @nodes = ("$1-$2");
}


#$ENV{'RUN'} = 0;

# @singleNodeTests = @singleNodeTests[0 .. 5];
SubmitWaitForTests(\@singleNodeTests, \@nodes, 10); #$nodesInBlock);
my @blocks = ($block);
if (@nodes > 1) {
    SubmitWaitForTests(\@multiNodeTests, \@blocks, 1);
}
else {
    print "\nSkip multinode tests: @multiNodeTests\n";
}



my $numTests = 0;
my @passed = ();
my @failed = ();
my $host = `hostname`; chomp $host;
$host =~ s|\..*||;
foreach my $test (@tests) {
    $numTests++;
    my $outfile = $tmpdir . TmpLogFile($test);
    my $data = `cat $outfile`;
    if ($data =~ m/Job \d+ on \S+ ENDED with Exit status = 0/sio) {
        push @passed, $test;
        print LOG "\n\n\n\n\n\n-- REGRESSION PASSED ----------\n$test\n-----------------------------\n\n";    
        print LOG $data;
    }
    else {
        push @failed, $test;
        print LOG "\n\n\n\n\n\n-- REGRESSION FAILED ----------\n$test\n-----------------------------\n\n";    
        print LOG $data;       
    }
}



my $doneStrg = "\n\n\ndone Tests: $numTests, Passed: " . scalar(@passed) . ". Failed: " . scalar(@failed) . ".  check $logPrint\n";
if (@passed) {
    $doneStrg .= "\nPassed tests: @passed\n";
}
if (@failed) {
    $doneStrg .= "\nFailed Tests: @failed\n";
}


if ($usePmt) {
    print STDERR "\npushdir: ", PushDir($tmpdir), "\n";    
    my $cwd = `pwd -P`; chomp $cwd;
    
    print STDERR "files in dir $cwd:\n";
    system("ls -l");
    my $metaFile = FindLeafFile("pmtData.metafile", "$cwd");    
    my $commitCmd = "PMTCommit -a $testComponant -e $runStampID";
    system("$commitCmd -m $metaFile");
    EchoSysRC("$commitCmd -m $metaFile");
    system($commitCmd);
    EchoSysRC($commitCmd);
    print "popdir: ", PopDir(), "\n";
}

my $etime = &ElapsedTimeText($startTime);
my $elapsedStrg = "\nOverall Elapsed Time = $etime\n";

print LOG $doneStrg;
print LOG $elapsedStrg;
print STDERR $doneStrg;
print STDERR $elapsedStrg;


exit 0;
