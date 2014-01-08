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
# (C) Copyright IBM Corp.  2004, 2011                              
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
#-*- mode: perl;-*-
$_IBMID_ = "Copyright IBM CORP 2005, 2006 LICENSED MATERIAL - PROGRAM PROPERTY OF IBM";

package main;

use Cwd;

BEGIN
{
    $fn = $0;
    my $startdir = $ENV{PWD};
    while(($fn =~ /\//)||(-l $fn))
    {
	if($fn =~ /\//)
	{
	    ($dir,$fn) = $fn =~ /(\S+)\/(\S+)/;
	    chdir($dir);
	}
	if(-l $fn)
	{
	    $fn = readlink($fn) if(-l $fn);
	}
    }
    $ENV{CODEPATH} = getcwd;
    unshift(@INC, $ENV{CODEPATH});
    chdir($startdir);
    
    $machinetype = "BGQ";
    
    $OPTIONVALUEREQ{'-a'}  = 1;
    $OPTIONVALUEREQ{'-j'}  = 1;
    $OPTIONVALUEREQ{'-pid'}  = 1;
    $OPTIONVALUEREQ{'-s'}  = 1;
    $OPTIONVALUEREQ{'-b'}  = 1;
    $OPTIONVALUEREQ{'-templates'} = 1;
    @VALIDOPTIONS = ("-b", "-c", "-a", "-s", "-templates", "-nogui", "-j", "-pid", "-skiphostcheck", "-h");
    @{$ADDOPTIONS{'-c'}} = ("-mincore", "-maxcore", "-coreprefix");
    @{$ADDOPTIONS{'-a'}} = ("-numbadframes", "-user");
    @{$ADDOPTIONS{'-s'}} = ();
    @{$ADDOPTIONS{'-j'}} = ();
    @{$ADDOPTIONS{'-pid'}} = ();
    @{$ADDOPTIONS{'-skiphostcheck'}} = ();
#    @{$ADDOPTIONS{'-m'}} = ();
    @{$ADDOPTIONS{'-nogui'}} = ("-mode", "-register", "-snapshot", "-flightlog");
    
    $OPTIONEXAM{"-b"} = "-b=/bglscratch/username/hello_world.elf:/bgl/BlueLight/ppcfloor/bglsys/bin/rts_hw.rts" if($machinetype eq "BGL");
    $OPTIONEXAM{"-b"} = "-b=/bgusr/username/hello_world.elf:/bgsys/drivers/ppcfloor/boot/cnk" if($machinetype eq "BGP");
    $OPTIONEXAM{"-b"} = "-b=/bgusr/username/hello_world.elf:/bgsys/drivers/ppcfloor/boot/cnk" if($machinetype eq "BGQ");
    $OPTIONDESC{"-b"} = "Specifies the ELF image to load for compute nodes.  Multiple ELF files can be specified by concatenating them with a colon ':'.  Typically the user's application is specified here.";
    $OPTIONEXAM{"-c"} = "-c=/bglscratch/username/." if($machinetype eq "BGL");
    $OPTIONEXAM{"-c"} = "-c=/bgusr/username/." if($machinetype eq "BGP");
    $OPTIONEXAM{"-c"} = "-c=/bgusr/username/." if($machinetype eq "BGQ");
    $OPTIONDESC{"-c"} = "Specifies the path to the directory containing existing core files";
    $OPTIONEXAM{"-a"} = "-a=R00-M0-N00-J..";
    $OPTIONDESC{"-a"} = "Connects to mc_server and attaches to the specified hardware";
    $OPTIONEXAM{"-j"} = "-j=57132";
    $OPTIONDESC{"-j"} = "Connects to a running job via the scalable debugger interface";
    $OPTIONEXAM{"-pid"} = "-pid=3121";
    $OPTIONDESC{"-pid"} = "Connects to a running job using the pid of the runjob process via the scalable debugger interface";
    $OPTIONEXAM{"-skiphostcheck"} = "-skiphostcheck";
    $OPTIONDESC{"-skiphostcheck"} = "Skips check of the hostname when attaching the scalable debugger.";
    $OPTIONEXAM{"-nogui"} = "-nogui";
    $OPTIONDESC{"-nogui"} = "GUI-less mode.  Coreprocessor processes the data and terminates";
    $OPTIONEXAM{"-snapshot"} = "-snapshot=/tmp/hang_issue1234";
    $OPTIONDESC{"-snapshot"} = "Fetch failure data and save to snapshot file";
    $OPTIONDESC{"-user"} = "The userid of the specific hardware";
    $OPTIONEXAM{"-user"} = "-user=bgqadmin";
    $OPTIONEXAM{"-flightlog"} = "-flightlog";
    $OPTIONDESC{"-flightlog"} = "Suggest which nodes to collect flightlog data from based on unique IARs";
    
    $OPTIONEXAM{"-s"} = "-s=/bglscratch/username/linpack.snapshot" if($machinetype eq "BGL");
    $OPTIONEXAM{"-s"} = "-s=/bgusr/username/linpack.snapshot" if($machinetype eq "BGP");
    $OPTIONEXAM{"-s"} = "-s=/bgusr/username/linpack.snapshot" if($machinetype eq "BGQ");
    $OPTIONDESC{"-s"} = "Loads the specified coreprocessor snapshot file for post-failure analysis";
    $OPTIONEXAM{"-templates"} = "-templates=/bglscratch/username/mytemplates/." if($machinetype eq "BGL");
    $OPTIONEXAM{"-templates"} = "-templates=/bgusr/username/mytemplates/." if($machinetype eq "BGP");
    $OPTIONEXAM{"-templates"} = "-templates=/bgusr/username/mytemplates/." if($machinetype eq "BGQ");
    $OPTIONDESC{"-templates"} = "Specifies the directory for saved memory templates";
    $OPTIONEXAM{"-mincore"}   = "-mincore=75";
    $OPTIONDESC{"-mincore"}   = "Specifies the lowest-numbered corefile to load.  Defaults to 0";
    $OPTIONEXAM{"-maxcore"}   = "-maxcore=65536";
    $OPTIONDESC{"-maxcore"}   = "Specifies the highest-numbered corefile to load.  Defaults to 131072";
    $OPTIONEXAM{"-coreprefix"}   = "-coreprefix=core";
    $OPTIONDESC{"-coreprefix"}   = "Specifies the filename prefix for lightweight corefiles.  Defaults to 'core'";
    $OPTIONEXAM{"-numbadframes"} = "-numbadframes=1";
    $OPTIONDESC{"-numbadframes"} = "Number of topmost stack frames to remove from collection.  This is useful if the app under debug is doing some custom assembly.  Under these conditions the application may not strictly adhere to the PowerPC ABI governing stack frame usage.";
    $OPTIONEXAM{"-mode"}      = "-mode=DCR";
    $OPTIONDESC{"-mode"}      = "Specifies the type of analysis to perform when coreprocessor is running in GUI-less mode.  Valid modes: Condensed, Detailed, Survey, Instruction, Kernel, Processor, DCR, PPC, Ungroup_trace, Ungroup, and Neighbor";
    $OPTIONEXAM{"-register"}  = "-register=GPR3";
    $OPTIONDESC{"-register"}  = "Specifies the PPC or DCR register when using the PPC or DCR modes";
    $OPTIONEXAM{"-h"}         = "-h";
    $OPTIONDESC{"-h"}         = "Displays help text";
    
    my $lastarg = "invalid";
    foreach $arg (@ARGV)
    {
	if($arg !~ /\-/)
	{
	    if((! exists $OPTIONS{$lastarg})||($OPTIONS{$lastarg} == 1))
	    {
		$OPTIONS{$lastarg} = $arg;
	    }
	    else
	    {
		$OPTIONS{$lastarg} .= ":$arg";
	    }
	    next;
	}
	$lastarg = $arg;
	if($arg =~ /=/)
	{
	    ($arg, $value) = $arg =~ /(\S+?)=(.*)/;
	    if((exists $OPTIONVALUEREQ{$arg}) && ($value eq ""))
	    {
		print "Option $arg requires a value\n";
		$DISPLAY_HELP = 1;
	    }
	    $OPTIONS{$arg} = $value;
	}
	else
	{
	    if(exists $OPTIONVALUEREQ{$arg})
	    {
		print "Option $arg requires a value\n";
		$DISPLAY_HELP = 1;
	    }
	    $OPTIONS{$arg} = 1;
	}
	push(@VALIDOPTIONS, @{$ADDOPTIONS{$arg}});
    }
    %VALIDOPTIONS_HASH = map { $_, 1} @VALIDOPTIONS;
    foreach $opt (keys %OPTIONS)
    {
	if(! exists $VALIDOPTIONS_HASH{$opt})
	{
	    print "Invalid option: $opt\n";
	    $DISPLAY_HELP = 1;
	}
	elsif($opt eq "-h")
	{
	    $DISPLAY_HELP = 1;
	}
    }
    if($DISPLAY_HELP)
    {
	print "Coreprocessor Usage\n";
	foreach $option (@VALIDOPTIONS)
	{
	    $spc = ' ' x (15 - length($option));
	    $insetspc = ' ' x 15;
	    
	    $desc = $OPTIONDESC{$option};
	    $desc =~ s/(.{70}\S*)\s*/$1\n/mg;
	    $desc =~ s/\n*$//sog;

	    $desc = "$spc$option: " . $desc;
	    $desc =~ s/\n/\n$insetspc  /g;
	    print $desc . "\n";

	    $desc = "example: " . $OPTIONEXAM{$option};
	    $desc =~ s/(.{70}\S*)\s*/$1\n/mg;
	    $desc =~ s/\n*$//sog;

	    $desc = $insetspc . "  " . $desc;
	    $desc =~ s/\n/\n$insetspc  /g;
	    print $desc . "\n\n";
	    
	    if(exists $ADDOPTIONS{$option})
	    {
		my @optlist = @{$ADDOPTIONS{$option}};
		$numopt = $#optlist+1;
		if($numopt > 0)
{
		print ' ' x 17 . "The next $numopt options can also be used when $option is present: \n";
		foreach $suboption (@{$ADDOPTIONS{$option}})
		{
		    $spc = ' ' x (30 - length($suboption));
		    $insetspc = ' ' x 30;
		    
		    $desc = $OPTIONDESC{$suboption};
		    $desc =~ s/(.{70}\S*)\s*/$1\n/mg;
		    $desc =~ s/\n*$//sog;

		    $desc = "$spc$suboption: " . $desc;
		    $desc =~ s/\n/\n$insetspc  /g;
		    print $desc . "\n";

		    $desc = "example: " . $OPTIONEXAM{$suboption};
		    $desc =~ s/(.{70}\S*)\s*/$1\n/mg;
		    $desc =~ s/\n*$//sog;

		    $desc = $insetspc . "  " . $desc;
		    $desc =~ s/\n/\n$insetspc  /g;
		    print $desc . "\n\n";
		}
}
	    }
	}
	exit(0);
    }
}

use bgqjtagreader;
use bglcorereader;
use bgpcorereader;
use rrcorereader;
use bglmmcsreader;
use bglsnapshot;
use sdbgreader;

if($OPTIONS{"-nogui"})
{
    require "consolemap.pl";
}
else
{
    require "stacktracemap.pl";
}

$reader_type = "";
$reader_type = "bgpcorereader" if(exists $OPTIONS{"-c"});
#$reader_type = "bglmmcsreader" if(exists $OPTIONS{"-a"});
$reader_type = "bgqjtagreader" if(exists $OPTIONS{"-a"});
$reader_type = "sdbgreader"    if(exists $OPTIONS{"-j"});
$reader_type = "sdbgreader"    if(exists $OPTIONS{"-pid"});

if($OPTIONS{"-nogui"})
{
    if(exists $OPTIONS{"-s"})
    {
	$CORE = bglsnapshot->mkthaw($OPTIONS{"-s"});
    }
    elsif($reader_type ne "")
    {
	$CORE = $reader_type->new(\%OPTIONS);
    }
    else
    {
	die "Coreprocessor mode was not specified.  -c, -a, -j, or -pid\n";
    }
    &console::build($CORE);
}
else
{
    &gui::build();
}

exit(0);

1;
