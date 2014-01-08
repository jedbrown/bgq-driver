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

package utilities;
use bagotricks qw(ansort);

sub makemap
{
    my($data, $index, @subset) = @_;
    my %signatures = ();
    my @COREMAP = ();
    
    foreach $key (@subset)
    {
	push(@{$signatures{$$data{$key}[$index]}}, $key);
    }
    
    my @sortedsigs = sort { $#{$signatures{$a}} <=> $#{$signatures{$b}} or ($a cmp $b); } (keys %signatures);
    
    foreach $key (@sortedsigs)
    {
	my @cores = @{$signatures{$key}};
	my @nextsubset = ();
	foreach $core (@cores)
	{
	    my @node = @{$$data{$core}};
	    if($#node > $index)
	    {
		push(@nextsubset, $core);
	    }
	}
	$totallvl = $#cores + 1;
	
	my %tmp = ();
	$tmp{"indent"} = $index;
	$tmp{"signature"} = $key;
	@{$tmp{"nodes"}} = @cores;
	push(@COREMAP, {%tmp});
	
	if($#nextsubset+1 > 0)
	{
	    push(@COREMAP, &makemap($data, $index+1, @nextsubset));
	}
    }
    return @COREMAP;
}

sub FindCommonNeighbors
{
    my($data, $bound, @nodes) = @_;
    my @maxord = ($data->{"maxx"}, $data->{"maxy"}, $data->{"maxz"}, 2);
    my %hash_decode = ();
    my @axis = ("X", "Y", "Z", "T");
    
    my %collision_hash = map { (join(",", $data->{$_}{"personality"}{"x"}, $data->{$_}{"personality"}{"y"}, $data->{$_}{"personality"}{"z"}, $data->{$_}{"mmcsprocid"}), 0) } @nodes;
    
    foreach $key (@nodes)
    {
	my @tmpord;
	my @ord =($data->{$key}{"personality"}{"x"}, $data->{$key}{"personality"}{"y"}, $data->{$key}{"personality"}{"z"}, $data->{$key}{"mmcsprocid"});
	$hash_decode{join(",", @ord)} = $key;
	for($i=0; $i<4; $i++)
	{
	    if($bound =~ /$axis[$i]\+/i)
	    {
		@tmpord = @ord;
		$tmpord[$i]+=1;
		$tmpord[$i] -= $maxord[$i] if($tmpord[$i]>=$maxord[$i]);
		$collision_hash{join(",", @tmpord)}++;
	    }
	    if($bound =~ /$axis[$i]\-/i)
	    {
		@tmpord = @ord;
		$tmpord[$i]-=1;
		$tmpord[$i] += $maxord[$i] if($tmpord[$i] < 0);
		$collision_hash{join(",", @tmpord)}++;
	    }
	}
    }
    
    my %output;
    foreach $node ($data->getnodenames(0))
    {
	next if($data->{$node}{"ionode"}==1);
	my $coord = join(",", $data->{$node}{"personality"}{"x"}, $data->{$node}{"personality"}{"y"}, $data->{$node}{"personality"}{"z"}, $data->{$node}{"mmcsprocid"});
	$xlate{$coord} = $node;
    }
    
    foreach $key (keys %collision_hash)
    {
	next if(!exists $xlate{$key});
	if(exists $hash_decode{$key})
	{
	    if($bound =~ /SET/i)
	    {
		$output{$xlate{$key}}{"neighcount"} = $collision_hash{$key};
		$output{$xlate{$key}}{"inset"} = 1;
	    }
	}
	else
	{
		$output{$xlate{$key}}{"neighcount"} = $collision_hash{$key};
		$output{$xlate{$key}}{"inset"} = 0;
	}
    }
    return (%output);
}

sub geniartablename
{
    my($ionode, $iar) = @_;
    $iar = "" if(!defined $iar);
    if($ionode) 
    { 
	return "ION $iar"; 
    }
    return "CNK $iar";
}

sub calculatemap
{
    my($ACTIVECORE, $mode, $register) = @_;
    my %values;
    if($mode =~ /condensed/i)
    {
	$ACTIVECORE->touchiar();
	$ACTIVECORE->touchstackdata();
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    my $type = "Node";
	    if(exists $ACTIVECORE->{$key}{"stacktrace"})
	    {
		@{$values{$key}} = ($type, @{$ACTIVECORE->{$key}{"stacktrace"}});
	    }
	    else
	    {
		@{$values{$key}} = ($type);
	    }
	    $sz = $#{$values{$key}};
	    if($sz == 0)
	    {
		push(@{$values{$key}}, "<traceback not fetched>");
	    }
	    if($ACTIVECORE->{$key}{"stripframes"} > 0)
	    {
		splice(@{$values{$key}}, -$ACTIVECORE->{$key}{"stripframes"}-1, $ACTIVECORE->{$key}{"stripframes"});
		$sz = $#{$values{$key}};
	    }
	    if($sz > 0)
	    {
		$values{$key}[$sz] = &lookup($ACTIVECORE, $key, $values{$key}[$sz]);
	    }
	    if($ACTIVECORE->{$key}{"clipthreshold"} > 0)
	    {
		splice(@{$values{$key}}, -$ACTIVECORE->{$key}{"clipthreshold"}-1);
		push(@{$values{$key}}, "<over clip depth threshold>");	    
	    }
	}
    }
    elsif($mode =~ /detailed/i)
    {
	$ACTIVECORE->touchiar();
	$ACTIVECORE->touchstackdata();
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    my $type = "Node";
	    #$type = "IO Node"      if($ACTIVECORE->{$key}{"ionode"} == 1);
	    if(exists $ACTIVECORE->{$key}{"stacktrace"})
	    {
		@{$values{$key}} = ($type, @{$ACTIVECORE->{$key}{"stacktrace"}});
		if($ACTIVECORE->{$key}{"stripframes"} > 0)
		{
		    splice(@{$values{$key}}, -$ACTIVECORE->{$key}{"stripframes"}-1, $ACTIVECORE->{$key}{"stripframes"});
		    $sz = $#{$values{$key}};
		}
	    }
	    else
	    {
		@{$values{$key}} = ($type);
	    }
	    if($#{$values{$key}}==0)
	    {
		push(@{$values{$key}}, "<traceback not fetched>");
	    }
	    
	    if($ACTIVECORE->{$key}{"clipthreshold"} > 0)
	    {
		splice(@{$values{$key}}, -$ACTIVECORE->{$key}{"clipthreshold"}-1);
		push(@{$values{$key}}, "<over clip depth threshold>");	    
	    }
	}
    }
    elsif($mode =~ /survey/i)
    {
	$ACTIVECORE->touchiar();
	my %iartable = ();
	my @fetchnodes = ();
	my @tmp = ();
	
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    my $name = &geniartablename($ACTIVECORE->{$key}{"ionode"}, $ACTIVECORE->{$key}{"coreregs"}{"IAR"});
	    if(!exists $iartable{$name})
	    {
		push(@fetchnodes, $key);
		$iartable{$name} = $key;
	    }
	}
	$ACTIVECORE->touchstackdata(@fetchnodes);
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    my $name = &geniartablename($ACTIVECORE->{$key}{"ionode"}, $ACTIVECORE->{$key}{"coreregs"}{"IAR"});
            my $type = "Node";
            #$type = "IO Node"      if($ACTIVECORE->{$key}{"ionode"} == 1);
	    
	    my $source_node = $iartable{$name};
	    
	    if(exists $ACTIVECORE->{$source_node}{"stacktrace"})
	    {
		@{$values{$key}} = ($type, @{$ACTIVECORE->{$source_node}{"stacktrace"}});
		if($ACTIVECORE->{$key}{"stripframes"} > 0)
		{
		    splice(@{$values{$key}}, -$ACTIVECORE->{$key}{"stripframes"}-1, $ACTIVECORE->{$key}{"stripframes"});
		}
		if($#{$ACTIVECORE->{$source_node}{"stacktrace"}} >= 0)
		{
		    $sz = $#{$values{$key}};
		    $values{$key}[$sz] = &lookup($ACTIVECORE, $key, $values{$key}[$sz]);
		}
		else
		{
		    push(@{$values{$key}}, "<traceback not fetched>");
		}
	    }
	    else
	    {
		@{$values{$key}} = ($type, "<traceback not fetched>");
	    }
	    
	    if($ACTIVECORE->{$key}{"clipthreshold"} > 0)
	    {
		splice(@{$values{$key}}, -$ACTIVECORE->{$key}{"clipthreshold"}-1);
		push(@{$values{$key}}, "<over clip depth threshold>");	    
	    }
	}
}
    elsif($mode =~ /instruction/i)
    {
	$ACTIVECORE->touchiar();
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = ($ACTIVECORE->{$key}{"coreregs"}{"IAR"} . " (" . &lookup($ACTIVECORE, $key, $ACTIVECORE->{$key}{"coreregs"}{"IAR"}) . ")");
	}
    }
    elsif($mode =~ /kernel/i)
    {
	$ACTIVECORE->touchkernelstatus();
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = ($ACTIVECORE->{$key}{"kernelstatus"});
	}
    }
    elsif($mode =~ /processor/i)
    {
	$ACTIVECORE->touchlocation();
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = ("Status: " . $ACTIVECORE->{$key}{"procstatus"});
	}
    }
    elsif($mode =~ /dcr/i)
    {
	my $dcrname = $register;
	$ACTIVECORE->touchdcr($dcrname);
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = "$dcrname=" . ($ACTIVECORE->{$key}{"dcr"}{$dcrname});
	}
    }
    elsif($mode =~ /PPC/i)
    {
        my $regname = $register;
	$ACTIVECORE->touchregisters()   if($regname =~ /GPR/i);
        $ACTIVECORE->touchspr($regname) if($regname =~ /SPR/i);
	
	foreach $key ($ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = "$regname=" . ($ACTIVECORE->{$key}{"coreregs"}{$regname});
	}
    }
    elsif($mode =~ /ungroup.*trace/i)
    {
        $ACTIVECORE->touchiar();
        $ACTIVECORE->touchstackdata();
	foreach $key (ansort $ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = (sprintf("%20s  (%d, %d, %d)", $key, $ACTIVECORE->{$key}{"personality"}{"x"}, $ACTIVECORE->{$key}{"personality"}{"y"}, $ACTIVECORE->{$key}{"personality"}{"z"}), @{$ACTIVECORE->{$key}{"stacktrace"}});
	}
    }
    elsif($mode =~ /ungroup/i)
    {
        $ACTIVECORE->touchiar();
	foreach $key (ansort $ACTIVECORE->getnodenames(1))
	{
	    @{$values{$key}} = sprintf("%20s  (%s, %s, %s)", $key, $ACTIVECORE->{$key}{"personality"}{"x"}, $ACTIVECORE->{$key}{"personality"}{"y"}, $ACTIVECORE->{$key}{"personality"}{"z"});
	}
    }
    elsif($mode =~ /filters/i)
    {
	# Since multiple nodes may be apart of more than 1 filter, we can't use makemap here:
	my @COREMAP;
	foreach $filter (ansort(keys %{$ACTIVECORE->{"filters"}}))
	{
	    my %tmp = ();
	    $tmp{"indent"} = 0;
	    $tmp{"signature"} = $filter;
	    @{$tmp{"nodes"}} = @{$ACTIVECORE->{"filters"}{$filter}{"nodes"}};;
	    push(@COREMAP, {%tmp});
	}
	return @COREMAP;
    }
    elsif($mode =~ /neighbor/i)
    {
	my (%hash) = &utilities::FindCommonNeighbors($ACTIVECORE, "X+X-Y+Y-Z+Z-SET", $ACTIVECORE->getnodenames(1));
	foreach $key (keys %hash)
	{
	    next if($key !~ /\S/);
	    @{$values{$key}} = sprintf("neighborcount=%d  (contained in set)", $hash{$key}{"neighcount"}) if($hash{$key}{"inset"} == 1);
	    @{$values{$key}} = sprintf("neighborcount=%d  (not in set)", $hash{$key}{"neighcount"}) if($hash{$key}{"inset"} == 0);
	}
    }
    elsif($mode =~ /select/i)
    {
    }
    else
    {
	print "Unknown mode: " . $mode . "\n";
    }
    
    return &makemap(\%values, 0, keys %values);
}

sub location
{
    my($ACTIVECORE, $node, $iar) = @_;

    if($ACTIVECORE->{$node}{"ionode"} == 1)
    {
        return $iar if(!exists $ACTIVECORE->{"linbin"});
        return $ACTIVECORE->{"linbin"}->location($iar);
    }
    return $iar if(!exists $ACTIVECORE->{"cnkbin"});
    return $ACTIVECORE->{"cnkbin"}->location($iar);
}

sub disassemble
{
    my($ACTIVECORE, $node, $iar, $len) = @_;
    if($ACTIVECORE->{$node}{"ionode"} == 1)
    {
        return "Binary not available" if(!exists $ACTIVECORE->{"linbin"});
        return $ACTIVECORE->{"linbin"}->disassemble($iar,$len);
    }
    return "Binary not available" if(!exists $ACTIVECORE->{"cnkbin"});
    return $ACTIVECORE->{"cnkbin"}->disassemble($iar, $len);
}

sub lookup
{
    my($ACTIVECORE, $node, $iar) = @_;
    if($ACTIVECORE->{$node}{"ionode"} == 1)
    {
	return $iar if(!exists $ACTIVECORE->{"linbin"});
	return $ACTIVECORE->{"linbin"}->lookup($iar);
    }
    return $iar if(!exists $ACTIVECORE->{"cnkbin"});
    return $ACTIVECORE->{"cnkbin"}->lookup($iar);
}

1;
