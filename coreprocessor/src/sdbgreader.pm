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

package sdbgreader;

use warnings;
use Socket;
require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: sdbgreader.pm,v 1.2 2008/10/08 19:47:39 tgooding Exp $';
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
  
  $self->{"skiphostcheck"} = 0;
  $self->{"jobid"} = $$options{"-j"}  if(exists $$options{"-j"});
  $self->{"pid"} = $$options{"-pid"}  if(exists $$options{"-pid"});
  $self->{"skiphostcheck"} = $$options{"-skiphostcheck"}  if(exists $$options{"-skiphostcheck"});
  $self->{"options"}{"debugionode"}=0;
  $self->{"options"}{"debugcomputenode"}=1;
  
  if(!exists $self->{"jobid"})
  {
      if(exists $self->{"pid"})
      {
	  $self->{"jobid"} = getJobIDFromPID($self->{"pid"}, $self->{"skiphostcheck"});
	  printf("jobid: %d\n", $self->{"jobid"});
      }
  }
  
  if($self->{"jobid"} == 0)
  {
      die "Invalid PID or JobID was specified";
  }
  
  $driver = `cat $ENV{CODEPATH}/driver_path`;
  chomp($driver);
  print "driver path: $driver\n";
  my $job = $self->{"jobid"};
  my $sdebug_proxy = "$driver/coreprocessor/bin/sdebug_proxy";
  # $sdebug_proxy = "/bgusr/tgooding/sdebug_proxy";
  $self->{"options"}{"dump_server"} = "$driver/coreprocessor/bin/sdebug --id=$job --tool=$sdebug_proxy 2>&1";
  
  $self->{"state"} = "run?";
  $self->{"touchedstacks"} = 0;
  $self->{"touchediar"} = 0;
  $self->{"touchedregisters"} = 0;
  $self->{"touchedlocation"} = 0;

  $cmd = $self->{"options"}{"dump_server"};
  print "command: $cmd\n";
  $childpid = IPC::Open2::open2(*SCANIN, *SCANOUT, $cmd);
  while($line = <SCANIN>)
  {
      print "line: $line\n";
      last if($line =~ /Command:/i);
  }
  
  bless  ($self,$class);
  return ($self);
}

sub getJobIDFromPID
{
    my($pid, $SKIPHOSTCHECK) = @_;
    $hosts = `hostname -IA`;
    %validhosts = map { $_ => 1 } split(/\s+/, $hosts);
    
    print "skipHostCheck=$SKIPHOSTCHECK\n";
    foreach $host (sort keys %validhosts)
    {
	print "hostname: $host\n";
    }
    $validhosts{"localhost"} = 1;
    foreach $line (split("\n", `/sbin/ifconfig 2>&1`))
    {
	next if($line !~ /inet addr:/);
	($ipaddr) = $line =~ /inet addr:(\S+)/;
	$validhosts{$ipaddr} = 1;
    }
    
    open(TMP, "/bgsys/drivers/ppcfloor/bin/list_jobs -a |");
    while($line = <TMP>)
    {
	next if($line =~ /\d+ job/);
	next if($line =~ /ID Status/);
	($id) = $line =~ /(\d+)/;
	if(defined $id)
	{
	    $data = `/bgsys/drivers/ppcfloor/bin/list_jobs $id`;
	    ($jobhost,$jobpid) = $data =~ /Client:\s+(\S+):(\d+)/;
	    my $jobip = "";
	    my $jobip_raw = gethostbyname($jobhost);
	    $jobip = inet_ntoa($jobip_raw) if(defined $jobip_raw);
	    print "jobhost=$jobhost   jobpid=$jobpid  jobip=$jobip\n";
	    if(($pid == $jobpid) && ((exists $validhosts{$jobhost}) ||
				     (exists $validhosts{$jobip}) ||
				     ($SKIPHOSTCHECK)))
	    {
		close(TMP);
		return $id;
	    }
	}
    }
    close(TMP);
    return 0;
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
    return;
    
    my $node = $CORE->{"options"}{"-a"};
    my $done = 0;
    print "command: target $node\n";
    
    print SCANOUT "target $node\n";
    while($line = <SCANIN>)
    {
	print "line: $line";
	if($line =~ /Command:/i) { $done=1; last; }
    }
    die "Unable to access location.  Check location string" if(!$done);
    $CORE->{"touchedlocation"} = 1;
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
#print "output: $line\n";
	if($line =~ /Command:/i) { $done=1; last; }
	chomp($line);
	($node, $iar) = split(/\s+/, $line);
	next if(!defined $iar);
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
    
    sendCommand("stacks 0");
    while($line = <SCANIN>)
    {
#print "output: $line";
	if($line =~ /Command:/i) { $done = 1; last; }
	chomp($line);
	($node, @stack) = split(/\s+/, $line);
	next if((! defined $node) || ($node !~ /^rank/));
	unshift(@{$CORE->{$node}{"stacktrace"}}, reverse(@stack));
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
#print "out: $line";
	if($line =~ /Command:/i) { $done = 1; last; }
	chomp($line);
	($node, @values) = split(/\s+/, $line);
        next if($node !~ /^rank/);

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
#print "out: $line";
	if($line =~ /Command:/i) { $done = 1; last; }
	chomp($line);
	($node, $value) = split(/\s+/, $line);
	next if($node !~ /^rank/);
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
        if(($node =~ /mmcsnode_\d+/) || ($node =~ /^core/))
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
	if($key =~ /^rank/)
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
