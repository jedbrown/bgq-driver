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

package bgqjtagreader;

use     warnings;
use FileHandle;
use IPC::Open2;

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: bgqjtagreader.pm,v 1.3 2008/05/19 21:42:26 mtn Exp $';
@DCRLIST=();

sub new 
{
  my ($invocant, $options) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  
  my ($self) = {
      "version" => $VERSION,
      "have_refreshed" => 0,
      "options" => $options
  };
  
  $self->{"options"}{"user"} = $ENV{"USER"};
  $self->{"options"}{"user"} = $options->{"-user"} if(exists $options->{"-user"});
  
  $self->{"options"}{"debugionode"}      = 0;
  $self->{"options"}{"debugcomputenode"} = 1;
  $self->{"options"}{"stripframes"}      = 1;
  
  $driver = `cat $ENV{CODEPATH}/driver_path`;
  chomp($driver);
  print "driver path: $driver\n";
  $self->{"options"}{"dump_server"} = "$driver/scantools/bin/dump_server";
  
  $self->{"state"} = "run?";
  $self->{"touchedstacks"} = 0;
  $self->{"touchediar"} = 0;
  $self->{"touchedregisters"} = 0;
  $self->{"touchedlocation"} = 0;
  
  $cmd = $self->{"options"}{"dump_server"} . " --user=" . $self->{"options"}{"user"};
  print "command: $cmd\n";
  $childpid = IPC::Open2::open2(*SCANIN, *SCANOUT, $cmd);
  while($line = <SCANIN>)
  {
      last if($line =~ /Command:/i);
  }
  
  bless  ($self,$class);
  return ($self);
}

sub sendCommand
{
    $cmd = join(" ", @_) . "\n";
    print "command: $cmd";
    print SCANOUT $cmd;
}

sub is_refresh_inprogress
{
    my($CORE) = @_;
    return 1 if($#{@{$CORE->{"corefiles"}}}+1 > 0);
    return 0;
}

sub touchlocation
{
    my($CORE) = @_;
    return if($CORE->{"touchedlocation"} == 1);
    print "touchlocation\n";
    
    my $node = $CORE->{"options"}{"-a"};
    my $done = 0;
    print "command: target $node\n";
    
    if($node ne "")
    {
	print SCANOUT "target $node\n";
	while($line = <SCANIN>)
	{
	    print "line: $line";
	    if($line =~ /Command:/i) { $done=1; last; }
	}
	die "Unable to access location.  Check location string" if(!$done);
	$CORE->{"touchedlocation"} = 1;
    }
    else
    {
	die "Invalid target name";
    }
}

sub touchiar
{
    my($CORE) = @_;
    return if($CORE->{"touchediar"} == 1);
    
    $CORE->touchlocation();
    $CORE->setstate("halt");
    
    my $done = 0;
    print "touchiar\n";
    sendCommand("iar 0");
    while($line = <SCANIN>)
    {
#	print "output: $line\n";
	if($line =~ /Command:/i) { $done=1; last; }
	chomp($line);
	($node, $iar) = split(/\s+/, $line);
	$CORE->{$node}{"coreregs"}{"IAR"} = $iar;
	$CORE->{$node}{"corerawdata"} .= "IAR:  $line\n";
    }
    die "Error fetching IARs" if(!$done);
    $CORE->{"touchediar"} = 1;
}

sub touchstackdata
{
    my($CORE, @nodelist) = @_;
    $CORE->touchlocation();
    $CORE->setstate("halt");
    
    return if($CORE->{"touchedstacks"} == 1);
    print "touchstackdata\n";
    my $done = 0;
    
    sendCommand("stack 0");
    while($line = <SCANIN>)
    {
#	print "output: $line";
	if($line =~ /Command:/i) { $done = 1; last; }
	chomp($line);
	($node, $depth, $iar) = split(/\s+/, $line);
	unshift(@{$CORE->{$node}{"stacktrace"}}, $iar);
	$CORE->{$node}{"corerawdata"} .= "STK:  $line\n";
    }
    die "Error fetching stacks" if(!$done);
    $CORE->{"touchedstacks"} = 1;
}

sub touchkernelstatus
{
    my($CORE) = @_;
}

sub touchregisters
{
    my($CORE, @nodelist) = @_;
    $CORE->touchlocation();
    $CORE->setstate("halt");
    
    return if($CORE->{"touchedregisters"} == 1);
    
    sendCommand("gprs 0");
    while($line = <SCANIN>)
    {
#	print "out: $line";
	if($line =~ /Command:/i) { $done = 1; last; }
	next if(length($line) < 32);  # a long line means GPR data.  Hack to filter extraneous output which caused perl warning messages.
	chomp($line);
	($node, @values) = split(/\s+/, $line);
	foreach $reg (0..31)
	{
	    my $regn = sprintf("GPR%02d", $reg);
	    $CORE->{$node}{"coreregs"}{$regn} = $values[$reg];
	}
	foreach $reg (0..7)
	{
	    $CORE->{$node}{"corerawdata"} .= sprintf("GPR%02d..%02d:  %s %s %s %s\n", $reg*4, $reg*4+3, $values[$reg*4+0], $values[$reg*4+1], $values[$reg*4+2], $values[$reg*4+3]);
	}
    }
    die "Error fetching registers" if(!$done);
    $CORE->{"touchedregisters"} = 1;
}

sub touchspr
{
    my($CORE, $spr) = @_;
    $CORE->touchlocation();
    $CORE->setstate("halt");
    
    sendCommand("$spr 0");
    while($line = <SCANIN>)
    {
#	print "out: $line";
	if($line =~ /Command:/i) { $done = 1; last; }
	chomp($line);
	($node, $value) = split(/\s+/, $line);
	$CORE->{$node}{"coreregs"}{$spr} = $value;
	$CORE->{$node}{"corerawdata"} .= "$spr:  $value\n";
    }
    die "Error fetching special purpose registers" if(!$done);
}

sub touchdcr
{
    my($CORE, $dcr) = @_;
}

sub touchmemory
{
    my($CORE, $address) = @_;
}

sub setaccessmask
{
    my($CORE, $value, @nodes) = @_;
    foreach $node (keys %{$CORE})
    {
        if(($node =~ /^Q/) || ($node =~ /^R/))
	{
            $CORE->{$node}{"node_accessible"} = 1-$value;
	}
    }
    foreach $node (@nodes)
    {
	$CORE->{$node}{"node_accessible"} = $value;
    }
}


sub isaccessible
{
    my($CORE, $key) = @_;

#    return 0 if(($CORE->{"options"}{"debugionode"}==0) && ($CORE->{$key}{"ionode"}==1));
#    return 0 if(($CORE->{"options"}{"debugcomputenode"}==0) && ($CORE->{$key}{"ionode"}==0));
#    return 0 if(! $CORE->{$key}{"node_accessible"});
    return 1;
}

sub getnodenames
{
    my($CORE, $accessible) = @_;
    my @list = ();
    foreach $key (keys %{$CORE})
    {
	if(($key =~ /^Q/) || ($key =~ /^R/))
	{
	    if((!defined $accessible) || isaccessible($CORE, $key))
	    {
		push(@list, $key);
	    }
	}
    }
    return @list;
}

sub getmpiname
{
    my($CORE, $index) = @_;
    return $CORE->{"mpimap"}[$index];
}

sub getnameddcrs
{
    my($CORE) = @_;
    return @DCRLIST;
}

sub getnamedregisters
{
    my($CORE) = @_;
    print "getnamedregisters\n";
    my @regnames = ();
    foreach $reg (0..31)
    {
	push(@regnames, sprintf("GPR%02d", $reg));
    }
    
    sendCommand("listsprs 0");
    while($line = <SCANIN>)
    {
        last if($line =~ /Command:/i);
	if($line =~ /SPRNAME:/)
	{
	    ($sprname) = $line =~ /SPRNAME:\s+(\S+)/;
	    push(@regnames, $sprname);
	}
    }
    return sort @regnames;
}

sub modifystatus_internal
{
    my($CORE, $node, $newstate) = @_;

    $CORE->{$node}{"corerawdata"} .= "Change state to $newstate\n";
    if($newstate =~ /RUN/i)
    {
#            my $savecnt = ++$CORE->{$node}{"historycnt"};
#            foreach $lvl (@{$CORE->{$node}{"stacktrace"}})
#            {
#                push(@{$CORE->{$node}{"stackhistory"}[$savecnt]}, hex($lvl));
#            }
	
	$CORE->{$node}{"stacktrace"} = ();
	delete $CORE->{$node}{"stacktrace"};
	delete $CORE->{$node}{"dcr"};
	delete $CORE->{$node}{"kernelstatus"};
	delete $CORE->{$node}{"coreregs"};
	
	$CORE->{"touchedstacks"} = 0;
	$CORE->{"touchediar"} = 0;
	$CORE->{"touchedregisters"} = 0;	    
    }
    $CORE->{$node}{"procstatus"} = $newstate;
}

sub setstate
{
    my($CORE, $newstate, @nodes) = @_;
    
    return if($CORE->{"state"} eq $newstate);
    
    print "setstate($newstate)\n";
    sendCommand("$newstate 0");
    while($line = <SCANIN>)
    {
	last if($line =~ /Command:/i);
    }
    
    foreach $node (@nodes)
    {
	if($newstate =~ /run/i)
	{
	    $CORE->modifystatus_internal($node, "RUN");
	}
	if($newstate =~ /halt/i)
	{
	    $CORE->modifystatus_internal($node, "HALT");
	}
	if($newstate =~ /step/i)
	{
	    $CORE->modifystatus_internal($node, "RUN");
	    $CORE->modifystatus_internal($node, "HALT");
	}
    }
}

1;
