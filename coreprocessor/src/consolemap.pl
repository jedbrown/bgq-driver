#-*- mode: perl;-*-
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
# (C) Copyright IBM Corp.  2005, 2011                              
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

package console;
use objdump;
require "utilities.pl";

sub build
{
    my($CORE) = @_;	
    $ACTIVECORE = $CORE;
    $CORE->{"cnkbin"} = objdump->new($CORE->{"options"}{"-b"}) if(exists $::OPTIONS{"-b"});
    $CORE->{"linbin"} = objdump->new($CORE->{"options"}{"-lb"}) if(exists $::OPTIONS{"-lb"});
    
    if(exists $::OPTIONS{"-snapshot"})
    {
	print "Gathering locations\n";
	$CORE->touchlocation();
	
	print "Gathering instruction addresses\n";
	$CORE->touchiar();
	
	print "Gathering stacks\n";
	$CORE->touchstackdata();
	
#	print "Gathering GPRs\n";
#	$CORE->touchregisters();
	
	print "Saving snapshot\n";
	
	my $data = bglsnapshot->mkfreeze($CORE);
	
	my $fn = $::OPTIONS{"-snapshot"};
	open(TMP, ">$fn");
	print TMP $data;
	close(TMP);	
    }
    
    my $mode = "condensed";
    my $register = "GPR3";
    $mode = $::OPTIONS{"-mode"} if(exists $::OPTIONS{"-mode"});
    $register = $::OPTIONS{"-register"} if(exists $::OPTIONS{"-register"});
    
    @COREMAP = &utilities::calculatemap($CORE, $mode, $register);
    
    for($x=0; $x<$#COREMAP+1; $x++)
    {
	$sig = $COREMAP[$x]{"signature"};
	$depth = sprintf("%-2d:", $COREMAP[$x]{"indent"});
	if($mode =~ /condensed|detailed|survey/i)
	{
	    if($sig =~ /^0x/)
	    {
		$sig = &utilities::lookup($ACTIVECORE, $COREMAP[$x]{"nodes"}[0], $sig);
	    }
	    if($mode =~ /detailed/i)
	    {
		$depth .= " (IAR=".$COREMAP[$x]{"signature"}.")" if(($COREMAP[$x]{"signature"} ne "IO Node") && 
								    ($COREMAP[$x]{"signature"} ne "Compute Node") &&
								    ($COREMAP[$x]{"signature"} ne "<traceback not fetched>") &&
								    ($COREMAP[$x]{"signature"} ne "<over clip depth threshold>"));
	    }
	}

        my @nodes = @{$COREMAP[$x]{"nodes"}};
	print $depth . ' ' x ($COREMAP[$x]{"indent"}*4) . $sig . " (" . ($#nodes+1) . ")\n";
    }
    
    if($::OPTIONS{"-flightlog"})
    {
	my @nodes = $ACTIVECORE->getnodenames();
	my %iarindex;
	foreach my $node (@nodes)
	{
	    ($location, $index) = $node =~ /(\S+):(\d+)/;
	    next if(exists $locdone{$location});
	    $locdone{$location} = 1;
	    
	    my $iarstring = "";
	    foreach $hwthread (0..67)
	    {
		my $name;
		if(1)
		{
		    $name = &utilities::lookup($ACTIVECORE,  
					       $location . ":" .$hwthread, 
					       $ACTIVECORE->{$location . ":" .$hwthread}{"coreregs"}{"IAR"});
		}
		else
		{
		    $name = $ACTIVECORE->{$location . ":" . $hwthread}{"coreregs"}{"IAR"};
		    substr($name, -2, 2, "");
		}
		$iaruniquecnt{$name}++;
		next if(exists $iarunique{$name});
		$iarunique{$name} = $location;
	    }
	}
	
	foreach $unique (keys %iarunique)
	{
	    $iarlocation{$iarunique{$unique}} = 1;
#	    print "Unique IAR fingerprint: " . $iarunique{$unique} . "   count=" . $iaruniquecnt{$unique} . "\n";
	}
	foreach $uniloc (keys %iarlocation)
	{
	    print "Flightlog: $uniloc\n";
	}
    }
    
    if(exists $::OPTIONS{"-snapshot"})
    {    
	print "Resuming nodes\n";
	if($ACTIVECORE->{"options"}{"-b"} =~ /cnk/)
	{
	    print "Detected CNK binary, resuming core 17\n";
	    $ACTIVECORE->setstate("run17", $CORE->getnodenames());
	}
	else
	{
	    $ACTIVECORE->setstate("run", $CORE->getnodenames());
	}
    }
}

1;
