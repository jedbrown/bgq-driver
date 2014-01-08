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


our ( $thisCmdPath, $thisCmdName, @ldirs );
BEGIN {   
    my $skip;
    ( $thisCmdPath, $skip, $thisCmdName ) = ( $0 =~ m|(([^/]*/)*)(.+)| );
    $thisCmdPath =~ s|/$||;
    unshift(@ldirs, $thisCmdPath);
}
use strict;
use Getopt::Long;
use Data::Dumper;

use lib @ldirs;    # add this dir to module search path
use RunTestUtils; 
use bgutils; 


my $echoEnvVals = 1;

sub DspHelp
{
  print <<EndOfHelpText;
  usage: runtest.pl [--jobtype=]<mmcslite|runjob|runpmt> 
                    [--block=]<block>
                    [--prog=]<program name>
                    # optionals
                    [--args=]['prog arguments']  
                    --nodes=[<nodes>]
                    --ppn=[<proc per node>]
                    --svcfile=[<svc host file>]
                    --jobparms=[<'...'>]   (other parms unique to job type)
                   
  Common Environment Variables:
    PAMID_EAGER=65535  # try to keep xfers in eager mode by default
    PAMI_DEVICE=S     # S if 1 node, or M if multiples 
    TIMELIMIT
    BG_SHAREDMEMSIZE=32
    BOOTOPTIONS
    GOMP_STACKSIZE
    BG_PROCESSESPERNODE=1 or set to passed ppnvalue
    EXPORTENV=<val,val...>  # add to this list of environment variables names to export to runjob
        
    SVCHOST    # RunTest.svchost is default
    OBJDUMP=0  # produce object dump with run
    RUN=0      # don't actually submit, just show final command to run    
    
    For mmcslite, may need to set path to properties file.  For example: 
       export BG_PROPERTIES=/bgsys/local/etc/bg.properties.Q00-I2
                
EndOfHelpText

  exit -1;
}


sub SignalHandler
{
    my $sig = shift;
    print STDERR "RunTest.pl Signal $sig received\n";
}

$SIG{'TSTP'} = \&SignalHandler;
$SIG{'TTIN'} = \&SignalHandler;
$SIG{'TTOU'} = \&SignalHandler;


#------------------------------------------------------
# Recognize and validate the incoming arguments and environment variables
#------------------------------------------------------
my $builddir = $ENV{'BGQ_BUILD_DIR'};
if (length($builddir) == 0) {
    die "ERROR ($thisCmdName): BGQ_BUILD_DIR environment var is not defined - must be exported by caller Makefile\n";
}
my $installdir = $ENV{'BGQ_INSTALL_DIR'};
if (length($builddir) == 0) {
    die "ERROR ($thisCmdName): BGQ_INSTALL_DIR environment var is not defined - must be exported by caller Makefile\n";
}
my $cwd = `pwd`; chomp($cwd);

my $posArg = 0;  # positional argument index.
my $block   = "";
my $jobType = "";
my $nodes   = 1;
my $ppn     = 1;
my $svcfile = SetEnvDef('SVCHOST', FindLeafFile("RunTest.svchost", $cwd), $echoEnvVals);
my $prog    = ""; 
my $args    = "";
my $tlimit  = SetEnvDef('TIMELIMIT', 60, $echoEnvVals);
my $jobParms = "";
my $dspHelp = 0;
my @envLbls = ();

# Set Environment Value as needed
SetEnvDef('PAMID_EAGER', 65536, $echoEnvVals);  # keep operations eager
push @envLbls, 'PAMID_EAGER';

SetEnvDef('BG_SHAREDMEMSIZE', 32, $echoEnvVals);
push @envLbls, 'BG_SHAREDMEMSIZE';

# SetEnvDef('LC_NUMERIC', "en_GB", $echoEnvVals);

my $bootOptions = SetEnvDef('BOOTOPTIONS', "dd2_l2_workaround", 0);
if ($bootOptions !~ m/dd2_l2_workaround/) {
    $bootOptions .= ",dd2_l2_workaround";
}
my $objDump = SetEnvDef('OBJDUMP', 0, $echoEnvVals);
my $run = SetEnvDef('RUN', 1, $echoEnvVals);

if (defined($ENV{'GOMP_STACKSIZE'})) {
    print STDERR "env GOMP_STACKSIZE=", $ENV{'GOMP_STACKSIZE'}, "\n";
    push @envLbls, 'GOMP_STACKSIZE';
}

# lets add any env vars which start with "BGPM"
foreach my $envvar (keys %ENV) {
    if ($envvar =~ m|^BGPM|o) {
        push @envLbls, $envvar;
    }
}

if (defined($ENV{'EXPORTENV'})) {
    push @envLbls, (split(',',$ENV{'EXPORTENV'}));
}




sub PosArgs { 
    my $arg = $_[0];
    if    ($posArg == 0) { $jobType = $arg; }
    elsif ($posArg == 1) { $block = $arg; }
    elsif ($posArg == 2) { $prog = $arg; }
    elsif ($posArg == 3) { $args = $arg; }
    else { die "ERROR ($thisCmdName): Unrecognized argument \"$arg\"\n"; }
    $posArg++;
}

if (!&GetOptions("<>" => \&PosArgs,
                "jobtype=s" => \$jobType, 
                "block=s"   => \$block,
                "prog=s"    => \$prog, 
                "args:s"    => \$args, 
                "nodes:i"   => \$nodes, 
                "tlimit:i"  => \$tlimit, 
                "ppn:i"     => \$ppn, 
                "svcfile:s" => \$svcfile,
                "jobparms:s" => \$jobParms,
                "help"      => \$dspHelp, 
                "h"         => \$dspHelp ))  { 
    &DspHelp();
}
if ($dspHelp) { &DspHelp(); }

if ($block eq "") { die "ERROR ($thisCmdName): No block name given\n"; }
if ($prog eq "")  { die "ERROR ($thisCmdName): No program name given\n"; }

$ppn = 1 if ($ppn < 1);
$nodes = 1 if ($nodes < 1);
$tlimit = 60 if ($tlimit < 1);
SetEnvDef('BG_PROCESSESPERNODE', $ppn, $echoEnvVals);
push @envLbls, 'BG_PROCESSESPERNODE';
#SetEnvDef('BG_MEMSIZE', 4096, $echoEnvVals);

# ----------------------------------------------------------
# Build and run the requested jobType
# ----------------------------------------------------------
my $startTime = GetTime();
my $exitStatus = 0;
if ($jobType eq "mmcslite") {
    $svcfile = "--svchost $svcfile " if ($svcfile ne "");
    $bootOptions = "--bootoptions $bootOptions " if ($bootOptions ne "");
    $objDump = ($objDump == 0) ? "--noobjdump " : ""; 
    $args = "-- $args " if ($args ne "");
    my $cmd = "$installdir/scripts/runfctest.sh --ttype cnk --program $prog "
              . "--location $block $jobParms "
              . "--script runMmcsLite --timelimit $tlimit $svcfile$bootOptions$objDump "
              . "--no-default-listener "
              . "$args";
    print STDERR "$cmd\n";
    if ($run) {          
        system($cmd);
        $exitStatus = EchoSysRC($cmd) if ($? != 0);  
    }     
}

elsif (($jobType eq "runjob") or ($jobType eq "runpmt")) {
        
    # Check if doing multiple or single node
    my ($baseBlock, $node); 
    ($baseBlock, $node) = ($block =~ m/^(.*):(\S+)$/i);
    $baseBlock = $block if (! defined($baseBlock));
    my $cornerBlock = $baseBlock;
    if ($cornerBlock =~ m/^(\S+\-\S+\-\S+)-\S+/) {
        $cornerBlock = $1; 
    }
    
    my $pamiDevice = "";
    my $corner = "";
    my $shape = "";
    if (($nodes > 1) || (! defined($node))) {
        $pamiDevice = 'M';
        if ($baseBlock ne $block) {
            $corner = "--corner $cornerBlock-$node ";            
            $shape = "--shape 1x1x1x1x2 ";
        }
    }
    else {
        $pamiDevice = 'S';
        $corner = "--corner $cornerBlock-$node ";
        
        #$corner = "--corner R00-M0-N07-J18 ";
        
        $shape = "--shape 1x1x1x1x1 ";
    }
    SetEnvDef('PAMI_DEVICE', $pamiDevice, $echoEnvVals);
    push @envLbls, 'PAMI_DEVICE';
    
    InitBgVars();
    
    if (! IsBlock($baseBlock)) {
        die "ERROR ($thisCmdName): $baseBlock is not a valid block name\n";
    }
    
    my $blkInfo = GetBlockInfo($baseBlock);
    #print Dumper($blkInfo);
    
    my $driver = GetEnvDef('DRIVER_NAME', $ppcfloor);
    my $driverNick = $driver;
    $driverNick =~ s/-//g;
    my $topdir=`pwd -P`; chomp($topdir);
    my $exedir="$topdir/.";
    my $tstdir="$topdir/inputs";
    
    # verify existance of executable
    #if (! -f "$exedir/$prog") {
    if (! -f "$prog") {
        die "ERROR ($thisCmdName): \"$prog\" is not found\n";        
    }
    
    # create working directory
    my $host=`hostname -s`; chomp($host);
    my $rundir="$host.$block.$driverNick";
    my $runpath = "";
    if (defined($ENV{'RUNPATH'})) {
        $runpath = $ENV{'RUNPATH'};
        $runpath =~ s|//|/|g;
        $runpath = "$runpath/$rundir";
        if (! -l $rundir) {
            if (-d $rundir) {
                rename $rundir, "$rundir.old" or die "ERROR: unable to rename $rundir to $rundir.old; $!\n";
            }   
            if (! -d $runpath) {     
                mkdir $runpath or die "ERROR: unable to mkdir $runpath; $!\n";
            }
            symlink $runpath, $rundir or die "ERROR: unable to create symlink $rundir to $runpath; $!\n";
        }
        $rundir = $runpath;
    }
    else {
        if (! -d $rundir) {
            mkdir $rundir or die "ERROR: unable to mkdir $rundir; $!\n";
        }
    }
    unlink "$rundir/$prog";
    system("cp $prog $rundir/$prog");
    if ($?) {
        die "ERROR ($thisCmdName): Could not copy $prog to $rundir/$prog\n";                
    }
    
    chdir $rundir || die "ERROR ($thisCmdName): failed to chdir to $rundir; $!\n";   
    print STDERR "CWD=$rundir\n";
    
    my $cmd = "runjob --block $baseBlock "
              ."--exe $prog "
              ."$corner$shape "         # ."--cwd `pwd -P`
              ."--ranks-per-node $ppn "
              ."--np " . $ppn*$nodes . " "
              ."--label long "
              ."--verbose ibm.runjob=INFO "
              ."--exp-env @envLbls "   
              ."--timeout $tlimit "
              ."$jobParms ";
    if ($args ne "") {              
              $cmd .= "--args \"$args\" ";
    }
               
    print STDERR "$cmd\n";
    if ($run) {    
        open STDERR, ">&STDOUT" or die "Can't dup STDOUT: $!";
        select STDOUT; $| = 1; # make unbuffered
                         
        $exitStatus = RunJobWrapper($prog, $cmd); # CallBashFunc($bgutils,"runjob_wrapper", $prog, $cmd);
    }    
}
else {
    die "ERROR ($thisCmdName): Don't recognize jobtype parm value \"$jobType\"\n";
}



if ($jobType eq "runpmt") {
    
}

 

print "Elapsed Run Time: ", ElapsedTimeText($startTime), "\n";

exit ($exitStatus);


