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

package rrcorereader;

use     warnings;
use objdump;

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: rrcorereader.pm,v 1.1 2008/10/07 23:42:24 tgooding Exp $';

sub new 
{
  my ($invocant, $options) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  
  my ($self) = {
      "version" => $VERSION,
      "have_refreshed" => 0,
      "options" => $options
  };
  
  $self->{"options"}{"debugionode"}      = 0;
  $self->{"options"}{"debugcomputenode"} = 1;

  $self->{"corepath"} = 0;  
  $self->{"corepath"} = $$options{"-c"}  if(exists $$options{"-c"});
  
  bless  ($self,$class);
  return ($self);
}

sub readcore
{
    my($CORE, $filename) = @_;
    $id = $filename;
    
    my $data = "";
    $CORE->{"maxx"} = 1;
    $CORE->{"maxy"} = 1;
    $CORE->{"maxz"} = 1;
    
    if(! exists $CORE->{"cnkbin"})
    {
	$CORE->{"cnkbin"} = objdump->new();
    }
    
    open(TMP, $filename);
    while($line = <TMP>) 
    {
	if($line =~ /STACK\s+for/)
	{
	    $CORE->{$id}{"corerawdata"}  = $data;
	    $data = $line;
	    ($process, $host, $pid, $cpu) = $line =~ /STACK\s+for\s+(\S+).*host=(\S+),\s+pid=(\d+),\s+cpu=(\d+)/;
	    
	    $id = "core.$host:$pid:$cpu";
	    
	    $CORE->{$id}{"procstatus"} = "core";
	    $CORE->{$id}{"kernelstatus"} = "core";
	    $CORE->{$id}{"ionode"} = 0;
	    
	    $CORE->{$id}{"node_accessible"} = 1;
	    $CORE->{$id}{"stripframes"}     = 0;
	    $CORE->{$id}{"clipthreshold"}   = 0;
	    $CORE->{$id}{"corefilepath"} = $filename;

	    $CORE->{$id}{"personality"}{"x"} = $pid;
	    $CORE->{$id}{"personality"}{"y"} = 0;
	    $CORE->{$id}{"personality"}{"z"} = 0;
	    $CORE->{"maxx"} = $pid+1 	    if($pid+1 >= $CORE->{"maxx"});
	    $CORE->{"maxy"} = 1;
	    $CORE->{"maxz"} = 1;
	}
	if($line =~ /^\s+\d+\s+0x/)
	{
	    $data .= $line;
	    ($address, $func) = $line =~ /^\s+\d+\s+(0x\S+)\s+(\S+)/;
	    
	    unshift(@{$CORE->{$id}{"stacktrace"}}, $address);
	    $CORE->{"cnkbin"}->addsymbol($address, $func, 4);
	    
#	    ($CORE->{$id}{"coreregs"}{"IAR"}) = $data =~ /while executing instruction at\.+(\S+)/;
	}
	
    }    
    
    close(TMP);
    
    my $fcc = 0;
    foreach $line (split("\n", $data))
    {
	if($fcc)
	{
	    unshift(@{$CORE->{$id}{"stacktrace"}}, $line);
	}
	$fcc = 1 if($line =~ /function call chain:/i);
    }
}

sub is_refresh_inprogress
{
    my($CORE) = @_;
    return 0;
}

sub refresh
{
    my($CORE) = @_;
    readcore($CORE, $CORE->{"corepath"});
    $CORE->{"have_refreshed"} = 1;
}

sub touchlocation
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchiar
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchstackdata
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchdcr
{
    my($CORE, $dcr) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchkernelstatus
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchregisters
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchmemory
{
    my($CORE, $address) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub setaccessmask
{
    my($CORE, $value, @nodes) = @_;
    foreach $node (keys %{$CORE})
    {
        if(($node =~ /mmcsnode_\d+/) || ($node =~ /core\.\d+$/))
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

    return 0 if(($CORE->{"options"}{"debugionode"}==0) && ($CORE->{$key}{"ionode"}==1));
    return 0 if(($CORE->{"options"}{"debugcomputenode"}==0) && ($CORE->{$key}{"ionode"}==0));
    return 0 if(! $CORE->{$key}{"node_accessible"});

    return 1;
}

sub getnodenames
{
    my($CORE, $accessible) = @_;
    my @list = ();
    foreach $key (keys %{$CORE})
    {
	if($key =~ /core\./)
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
    return (@regnames);
}
