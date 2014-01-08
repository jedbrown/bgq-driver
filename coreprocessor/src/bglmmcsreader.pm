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

package bglmmcsreader;

use warnings;
use mmcs_db;

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: bglmmcsreader.pm,v 1.8 2008/05/19 21:37:28 mtn Exp $';

sub new 
{
  my ($invocant, $options) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  my ($self) = 
  {
      "version" => $VERSION,
      "options" => $options
  };
  @parms = ();
  push(@parms, "host"=>${%{$options}}{"-host"}) 
      if(exists ${%{$options}}{"-host"});
  push(@parms, "user"=>${%{$options}}{"-user"}) 
      if(exists ${%{$options}}{"-user"});
  push(@parms, "port"=>${%{$options}}{"-port"}) 
      if(exists ${%{$options}}{"-port"});
  
  $self->{"options"}{"debugionode"}      = 0;
  $self->{"options"}{"debugcomputenode"} = 1;
  $self->{"options"}{"-numthreads"}      = 1 if(!defined $self->{"options"}{"-numthreads"});
  $self->{"options"}{"-numbadframes"}    = 0 if(!defined $self->{"options"}{"-numbadframes"});
  $self->{"mmcs_link"} = mmcs_db->new(@parms);
  $self->{"mmcs_link"}->connect($self->{"options"}{"-a"}, $self->{"options"}{"-numthreads"});
  
  bless  ($self,$class);
  return ($self);
}

sub touchlocation
{
    my($CORE) = @_;
    return if($CORE->{"called_touchlocation"});
    $CORE->{"called_touchlocation"} = 1;
    
    my $locatedata = $CORE->{"mmcs_link"}->sendCmd("locate ras_format");
    $CORE->{"maxx"} = 0;
    $CORE->{"maxy"} = 0;
    $CORE->{"maxz"} = 0;
    $CORE->{"bgtype"} = "BGL";
    $CORE->{"maxprocs"} = 2;
    
    foreach $line (split("\n", $locatedata))
    {
	next if($line =~ /OK/);
	next if($line =~ /nc_ido/);
	next if($line =~ /\<nc\>/);
	next if($line =~ /lc_ido/);
	next if($line =~ /linkchip/);
	next if(($line !~ /<.*?\d.*?>/)&&($line !~ /ipaddress/));
	die "Unable to issue locate command.  Check the status of the block" if($line =~ /ABORT/);
	die "Unable to issue locate command.  Check the status of the block" if($line =~ /FAIL/);
	
	if($line =~ /target:/)
	{
	    $CORE->{"maxprocs"} = 4;
	    $CORE->{"bgtype"} = "BGP";
	}
	
	my ($node, $x, $y, $z, $location) = $line =~ /\{(\d+)\}\s*<\s*(\S+),\s*(\S+),\s*(\S+)\s*>.*location:\s*(\S+)/;
        for($proc=0; $proc<$CORE->{"maxprocs"}; $proc++)
        {
            $CORE->{"mmcsnode_$node.$proc"}{"ionode"} = 0;
            $CORE->{"mmcsnode_$node.$proc"}{"ionode"} = 1 if($x !~ /\d+/);
            $CORE->{"mmcsnode_$node.$proc"}{"corerawdata"} = "$line\n\n";
            $CORE->{"mmcsnode_$node.$proc"}{"mmcsnodeid"}  = $node;
            $CORE->{"mmcsnode_$node.$proc"}{"mmcsprocid"}  = $proc;
            $CORE->{"mmcsnode_$node.$proc"}{"personality"}{"x"} = $x;
            $CORE->{"mmcsnode_$node.$proc"}{"personality"}{"y"} = $y;
            $CORE->{"mmcsnode_$node.$proc"}{"personality"}{"z"} = $z;
            $CORE->{"mmcsnode_$node.$proc"}{"personality"}{"location"} = $location;
            
            $CORE->{"mmcsnode_$node.$proc"}{"procstatus"} = "?RUN?";
            $CORE->{"mmcsnode_$node.$proc"}{"node_accessible"} = 1;
            $CORE->{"mmcsnode_$node.$proc"}{"stripframes"}     = 1;
            $CORE->{"mmcsnode_$node.$proc"}{"clipthreshold"}   = 0;
            
            if($CORE->{"mmcsnode_$node.$proc"}{"ionode"} == 0)
            {
                $CORE->{"maxx"} = $x+1 	    if($x >= $CORE->{"maxx"});
                $CORE->{"maxy"} = $y+1	    if($y >= $CORE->{"maxy"});
                $CORE->{"maxz"} = $z+1	    if($z >= $CORE->{"maxz"});
            }
        }
    }
}

sub touchiar
{
    my($CORE) = @_;
    $CORE->touchlocation();
    
    my @queuenodes = ();
    foreach $key ($CORE->getnodenames())
    {
        next if(($CORE->{"options"}{"debugionode"}==0) &&
                ($CORE->{$key}{"ionode"}==1));
        next if(($CORE->{"options"}{"debugcomputenode"}==0) &&
                ($CORE->{$key}{"ionode"}==0));
        next if(exists $CORE->{$key}{"coreregs"}{"IAR"});
	next if($CORE->{$key}{"node_accessible"} == 0);
	
        push(@queuenodes, $key);
        $CORE->modifystatus_internal($key, "HALT");
    }
    $CORE->docommand(@queuenodes, "dump_iar");
}

sub touchstackdata
{
    my($CORE, @nodelist) = @_;
    $CORE->touchlocation();
    
    @nodelist = $CORE->getnodenames() if($#nodelist == -1);
    my @queuenodes = ();    
    foreach $key (@nodelist)
    {	
	next if(($CORE->{"options"}{"debugionode"}==0) &&
		($CORE->{$key}{"ionode"}==1));
	next if(($CORE->{"options"}{"debugcomputenode"}==0) &&
		($CORE->{$key}{"ionode"}==0));
	next if(exists $CORE->{$key}{"stacktrace"});
	next if($CORE->{$key}{"node_accessible"} == 0);
	
	# Temporary hack until stacktrace includes IAR
	if(exists $CORE->{$key}{"coreregs"}{"IAR"})
	{
	    push(@{$CORE->{$key}{"stacktrace"}}, $CORE->{$key}{"coreregs"}{"IAR"});
	}
	
	push(@queuenodes, $key);	
	$CORE->modifystatus_internal($key, "HALT");
    }
    $CORE->docommand(@queuenodes, "dump_stacks");
}

sub touchkernelstatus
{
    my($CORE) = @_;
    $CORE->touchlocation();

    my @queuenodes = ();
    foreach $key ($CORE->getnodenames())
    {
        next if(($CORE->{"options"}{"debugionode"}==0) &&
                ($CORE->{$key}{"ionode"}==1));
        next if(($CORE->{"options"}{"debugcomputenode"}==0) &&
                ($CORE->{$key}{"ionode"}==0));
        next if(exists $CORE->{$key}{"kernelstatus"});
	next if($CORE->{$key}{"node_accessible"} == 0);
	
	push(@queuenodes, $key);
    }
    $CORE->docommand(@queuenodes, "read_kernel_status");
}

sub touchregisters
{
    my($CORE) = @_;
    $CORE->touchlocation();
    
    my @queuenodes = ();
    foreach $key ($CORE->getnodenames())
    {
        next if(($CORE->{"options"}{"debugionode"}==0) &&
                ($CORE->{$key}{"ionode"}==1));
        next if(($CORE->{"options"}{"debugcomputenode"}==0) &&
                ($CORE->{$key}{"ionode"}==0));
        next if(exists $CORE->{$key}{"GPR0"});
	next if($CORE->{$key}{"node_accessible"} == 0);
	
        $CORE->modifystatus_internal($key, "HALT");
	push(@queuenodes, $key);
    }
    $CORE->docommand(@queuenodes, "dump_gprs");
}

sub touchdcr
{
    my($CORE, $dcr) = @_;
    $CORE->touchlocation();
    
    my @queuenodes = ();
    foreach $key ($CORE->getnodenames())
    {
        next if(($CORE->{"options"}{"debugionode"}==0) &&
                ($CORE->{$key}{"ionode"}==1));
        next if(($CORE->{"options"}{"debugcomputenode"}==0) &&
                ($CORE->{$key}{"ionode"}==0));
        next if(exists $CORE->{$key}{"dcr"}{$dcr});
	next if($CORE->{$key}{"node_accessible"} == 0);
	
	push(@queuenodes, $key);
    }
    $CORE->docommand(@queuenodes, "read_dcr $dcr");
}

sub touchmemory
{
    my($CORE, $address) = @_;
    my @queuenodes = ();
    foreach $key ($CORE->getnodenames())
    {
        next if(($CORE->{"options"}{"debugionode"}==0) &&
                ($CORE->{$key}{"ionode"}==1));
        next if(($CORE->{"options"}{"debugcomputenode"}==0) &&
                ($CORE->{$key}{"ionode"}==0));
        next if(exists $CORE->{$key}{"memory"}{$address});
        next if($CORE->{$key}{"node_accessible"} == 0);
	
	push(@queuenodes, $key);
    }
    $CORE->docommand(@queuenodes, "dump_memory $address 0x4");
}

##########################################################
##########################################################

sub process_dump_iar
{
    my($CORE, $iarreply, $cmd) = @_;
    foreach $iardata (split("\n", $iarreply))
    {
	die "Unable to fetch the current instruction address.  Check the block status and verify that the nodes have started" if($iardata =~ /ABORT/);
	die "Unable to fetch the current instruction address.  Check the block status and verify that the nodes have started" if($iardata =~ /FAIL/);
	next if($iardata =~ /OK/);
	if($iardata =~ /\{\d+\}\.\d+\s+IAR:\s+\S+\s+\S+/)
	{
	    ($node,$proc,$iar,$lr) = $iardata =~ /\{(\d+)\}\.(\d+)\s+IAR:\s+(\S+)\s+(\S+)/;
	    $CORE->{"mmcsnode_$node.$proc"}{"corerawdata"} .= "$cmd: $iardata\n";
	    $CORE->{"mmcsnode_$node.$proc"}{"coreregs"}{"IAR"} = $iar;
	    $CORE->{"mmcsnode_$node.$proc"}{"coreregs"}{"LR"} = $lr;
	}
    }
}
sub fail_dump_iar
{
    my($CORE, $reply, $cmd, $node, $proc) = @_;
    $CORE->{"mmcsnode_$node.$proc"}{"coreregs"}{"IAR"} = $reply;
    $CORE->{"mmcsnode_$node.$proc"}{"coreregs"}{"LR"}  = $reply;
}
sub process_dump_stacks
{
    my($CORE, $stackreply, $cmd) = @_;
    my $node = 0;
    my $proc = 0;
    my $firstframe = $CORE->{"options"}{"-numbadframes"};
    foreach $line (split("\n", $stackreply))
    {
	die "Unable to fetch stack tracebacks.  Check the status of the block." if($line =~ /ABORT/);
	die "Unable to fetch stack tracebacks.  Check the status of the block." if($line =~ /FAIL/);
	next if($line eq "OK");
	if($line =~ /\{\d+\}/)
	{
	    ($node) = $line =~ /\{(\d+)\}/; 
	    $firstframe = $CORE->{"options"}{"-numbadframes"};
	}
	if($line =~ /Core\s+\d+/i)
	{
	    ($proc) = $line =~ /Core\s+(\d+)/i; 
	    $firstframe = $CORE->{"options"}{"-numbadframes"};
	}
	
	if($line =~ /^0x/)
	{
	    if($firstframe > 0)
	    {
		$firstframe--;
		next;
	    }
	    $CORE->{"mmcsnode_$node.$proc"}{"corerawdata"} .= "$cmd: $line\n";
	    
	    my($address) = $line =~ /0x.*?\s+(0x.*)/;
	    $address = hex($address);
	    if($address == 0)
	    {
		$address = "0xfffffffc";
	    }
	    else
	    {
		$address = sprintf("0x%8.8x", $address-4);
	    }
	    unshift(@{$CORE->{"mmcsnode_$node.$proc"}{"stacktrace"}}, $address);
	}
    }
}
sub fail_dump_stacks
{
    my($CORE, $reply, $cmd, $node, $proc) = @_;
    $CORE->{"mmcsnode_$node.$proc"}{"stacktrace"} = $reply;
}
sub process_read_kernel_status
{
    my($CORE, $reply, $cmd) = @_;
    foreach $statusdata (split("\n", $reply))
    {
	die "Unable to read kernel status.  Check on the status of the block." if($statusdata =~ /ABORT/);
	die "Unable to read kernel status.  Check on the status of the block." if($statusdata =~ /FAIL/);
	next if($statusdata =~ /OK/);
	my ($node, $proc, $status, $count, $iar) = $statusdata =~ 
	    /\{(\d+)\}\.(\d+):\s+status=(\S+)\s+count=(\S+)\s+iar=(\S+)/;
	$CORE->{"mmcsnode_$node.$proc"}{"kernelstatus"} = "$status $iar";
	$CORE->{"mmcsnode_$node.$proc"}{"corerawdata"} .= "$cmd: $statusdata\n";
    }
}
sub fail_read_kernel_status
{
    my($CORE, $reply, $cmd, $node, $proc) = @_;
    $CORE->{"mmcsnode_$node.$proc"}{"kernelstatus"} = $reply;
}
sub process_dump_gprs
{
    my($CORE, $reply, $cmd) = @_;
    my $node = 0;
    my $proc = 0;
    foreach $line (split("\n", $reply))
    {
	($node,$proc) = $line =~ /\{(\d+)\}\.(\d+)/ if($line =~ /GPR/);
	if($line =~ /^r\d/)
	{
	    $CORE->{"mmcsnode_$node.$proc"}{"corerawdata"} .= "$cmd: $line\n";
	    foreach $regval (split(/\s+/, $line))
	    {
		my ($reg, $hexval) = $regval =~ /(\S+)=(\S+)/;
		$reg =~ s/r/GPR/;
		$CORE->{"mmcsnode_$node.$proc"}{"coreregs"}{$reg} = $hexval;
	    }
	}
    }
}
sub fail_dump_gprs
{
    my($CORE, $reply, $cmd, $node, $proc) = @_;
    foreach $reg ($CORE->getnamedregisters())
    {
	$CORE->{"mmcsnode_$node.$proc"}{"coreregs"}{$reg} = $reply;
    }
}
sub process_read_dcr
{
    my($CORE, $reply, $cmd, $dcr) = @_;
    foreach $line (split("\n", $reply))
    {
	die "Unable to read DCR.  Check on the status of the block." if($line =~ /ABORT/);
	die "Unable to read DCR.  Check on the status of the block." if($line =~ /FAIL/);
	next if($line eq "OK");
	if($line =~ /\{\d+\}/)
	{
	    my ($node, $hexval) = $line =~ /\{(\d+)\}\s+(.*)/;
	    # Device control registers are shared between both ppc cores.
	    $CORE->{"mmcsnode_$node.0"}{"corerawdata"} .= "$cmd: $line\n";
	    $CORE->{"mmcsnode_$node.1"}{"corerawdata"} .= "$cmd: $line\n";
	    $CORE->{"mmcsnode_$node.0"}{"dcr"}{$dcr} = $hexval;
	    $CORE->{"mmcsnode_$node.1"}{"dcr"}{$dcr} = $hexval;
	}
    }
}
sub fail_read_dcr
{
    my($CORE, $reply, $cmd, $node, $proc, $dcr) = @_;
    $CORE->{"mmcsnode_$node.$proc"}{"dcr"}{$dcr} = $reply;
}
sub process_dump_memory
{
    my($CORE, $reply, $cmd, $address_len) = @_;
    my ($address, $len) = $address_len =~ /(\S+)\s+(\S+)/;
    my $node;
    my $proc;
    foreach $line (split("\n", $reply))
    {
	die "Unable to dump memory.  Check status on the block." if($line =~ /ABORT/);
	die "Unable to dump memory.  Check status on the block." if($line =~ /FAIL/);
        next if($line eq "OK");
        if($line =~ /\{\d+\}/)
        {
            ($node,$proc) = $line =~ /\{(\d+)\}\.(\d+)/;
	}
        if($line =~ /$address:\s+/i)
	{	  
	    ($hexval) = $line =~ /:\s+(.*)/;
            $CORE->{"mmcsnode_$node.$proc"}{"corerawdata"} .= "$cmd: $line\n";
            $CORE->{"mmcsnode_$node.$proc"}{"memory"}{$address} = $hexval;
        }
    }
}
sub fail_dump_memory
{
    my($CORE, $reply, $cmd, $node, $proc, $address) = @_;
    $CORE->{"mmcsnode_$node.$proc"}{"memory"}{$address} = $reply;
}

################################################
################################################
################################################


sub is_refresh_inprogress
{
    my($CORE) = @_;
    return 1 if($#{@{$CORE->{"corefiles"}}}+1 > 0);
    return 0;
}

sub setaccessmask
{
    my($CORE, $value, @nodes) = @_;
    foreach $node (keys %{$CORE})
    {
        if($node =~ /mmcsnode_\d+/)
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
	push(@list, $key) if(($key =~ /mmcsnode_\d+/) && ((!defined $accessible) || isaccessible($CORE, $key)));
    }
    return sort(@list);
}

sub getconsole
{
    my($CORE) = @_;
    return $CORE->{"mmcs_link"}->{"console"};
}

sub getnameddcrs
{
    my($CORE) = @_;
    my $dcrs =  $CORE->{"mmcs_link"}->sendCmd(sprintf("list_dcrs"));
    my @dcrlist = (); 
    return () if($dcrs !~ /OK/);
    
    foreach $dcr (split("\n", $dcrs))
    {
	$dcr =~ s/\s.*//;
	push(@dcrlist, $dcr) if($dcr !~ /^OK/);
    }
    return @dcrlist;
}

sub getnamedregisters
{
    my($CORE) = @_;
    foreach $reg (0..31)
    {
	push(@regnames, sprintf("GPR%02d", $reg));
    }
    return @regnames;
}

sub modifystatus_internal
{
    my($CORE, $node, $newstate) = @_;
    
    if($newstate ne $CORE->{$node}{"procstatus"})
    {
	$CORE->{$node}{"corerawdata"} .= "Change state to $newstate\n";
	if($newstate =~ /RUN/i)
	{
	    my $savecnt = ++$CORE->{$node}{"historycnt"};
	    foreach $lvl (@{$CORE->{$node}{"stacktrace"}})
	    {
		push(@{$CORE->{$node}{"stackhistory"}[$savecnt]}, hex($lvl));
	    }
	    
	    # wipe brains
	    $CORE->{$node}{"stacktrace"} = ();
	    delete $CORE->{$node}{"stacktrace"};
	    delete $CORE->{$node}{"dcr"};
	    delete $CORE->{$node}{"kernelstatus"};
	    delete $CORE->{$node}{"coreregs"};
	}
	$CORE->{$node}{"procstatus"} = $newstate;
    }
}

sub sethaltstate
{
    my($CORE, $newstate, @nodes) = @_;
    
    foreach $node (@nodes)
    {
	my $cmd = "";
	$cmd = "start_cores" if($newstate =~ /run/i);
	$cmd = "halt_cores"  if($newstate =~ /halt/i);
	$cmd = "step_core"   if($newstate =~ /step/i);
	
	my $mmcsproc = $CORE->{$node}{"mmcsprocid"};
	my $mmcsnode = $CORE->{$node}{"mmcsnodeid"};
	if($cmd ne "")
	{
	    $cmd = sprintf("{$mmcsnode}.$mmcsproc $cmd");
	    $CORE->{"mmcs_link"}->sendCmd($cmd);
	    
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
}

sub docommand
{
    return if($#_+1 < 3);
    my $CORE  = shift @_;
    my $cmd   = pop @_;
    my @nodes = @_;
    my $reply;
    my @ids1 = ();
    my @ids2 = ();
    my @ids3 = ();
    my @sequence = ();
    my @nodelist = ();
    my $modulo = ($#nodes+1)/$CORE->{"options"}{"-numthreads"};
    my $bin = int(($#nodes+1)/$modulo);
    
    my %nodehash = ();
    foreach $key (@nodes)
    {
	$nodehash{$CORE->{$key}{"mmcsnodeid"}}{"count"} += 2**$CORE->{$key}{"mmcsprocid"};
	$nodehash{$CORE->{$key}{"mmcsnodeid"}}{$CORE->{$key}{"mmcsprocid"}} = 1;
    }
    
    $maxmodulo = 63;
    if($cmd =~ /dump_stacks/)
    {
	$maxmodulo = 7 if($CORE->{"mmcs_link"}->{"replyformat"} == 0);
    }
    elsif($cmd =~ /dump_gprs/)
    {
	$maxmodulo = 3 if($CORE->{"mmcs_link"}->{"replyformat"} == 0);
    }
    else
    {
	$maxmodulo = 63 if($CORE->{"mmcs_link"}->{"replyformat"} == 0);
    }
    $modulo = $maxmodulo if($modulo > $maxmodulo);
    
    foreach $node (sort { $a <=> $b; } keys %nodehash)
    {
	if($nodehash{$node}{"count"} == (2**($CORE->{"maxprocs"})-1))
	{
	    push(@fullnode, $node);
	    if($#fullnode >= $modulo)
	    {
		my $tmp = join(",", @fullnode);
		@fullnode = ();
		push(@sequence, sprintf("{$tmp} $cmd"));
		push(@nodelist, $tmp);
	    }
	}
	else
	{
	    foreach $proc (keys %{$nodehash{$node}})
	    {
		next if($proc eq "count");
		push(@{$ids{$proc}}, $node);
		
		if($#{$ids{$proc}} >= $modulo)
		{
		    my $tmp = join(",", @{$ids{$proc}});
		    @{$ids{$proc}} = ();
		    push(@sequence, sprintf("{$tmp}.$proc $cmd"));
		    push(@nodelist, "$tmp proc$proc");
		}
	    }
	}
    }
    foreach $proc (keys %ids)
    {
	if($#{$ids{$proc}} >= 0)
	{
	    my $node = join(",",@{$ids{$proc}});
	    @{$ids{$proc}} = ();
	    push(@sequence, sprintf("{$node}.$proc $cmd"));
	    push(@nodelist, "$node proc$proc");
	}
    }
    if($#fullnode >= 0)
    {
	my $node = join(",",@fullnode);
	@fullnode = ();
	push(@sequence, sprintf("{$node} $cmd"));
	push(@nodelist, $node);
    }
    
    my @replies = $CORE->{"mmcs_link"}->sendCmd(@sequence);
    foreach $reply (@replies)
    {
	my ($func, $data) = $cmd =~ /^(\S+)\s*(.*)/;
	my $nodes = shift @nodelist;
	my $goterror = 1;
	if($reply =~ /^OK/)
	{
	    eval "\$CORE->process_$func(\$reply, \$cmd, \$data);";
	    if($@)
	    {
		$goterror = 1;
	    }
	    else
	    {
		$goterror = 0;
	    }
	}
	if($goterror == 1)
	{
	    $minproc = 0;
	    $maxproc = 1;
	    if($nodes =~ /proc/)
	    {
		($nodes,$proc) = $nodes =~ /(.*)\s+proc(\d+)/;
		$minproc=$maxproc=$proc;
	    }
	    my $errordata;
	    foreach $node (split(",", $nodes))
	    {
		for($proc=$minproc; $proc<=$maxproc; $proc++)
		{
		    $reply = $CORE->{"mmcs_link"}->sendCmd("{$node}.$proc $cmd");
		    eval "\$CORE->process_$func(\$reply, \$cmd, \$data);";
		    if($@)
		    {		
			$errordata = $@;
			eval "\$CORE->fail_$func(\$reply, \$cmd, \$node, \$proc, \$data);";
		    }
		}
	    }
	    die $errordata if($errordata ne "");
	}
    }
}
