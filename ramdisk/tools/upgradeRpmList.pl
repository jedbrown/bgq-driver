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

$oldlist = "bgqDistrofs-RHEL6.4.rpmlist";
$newlist = "bgqDistrofs-RHEL6.5.rpmlist";
$pkgs    = "/install/redhat/RHEL6.5-ppc/Packages";

opendir(TMP, $pkgs);
@files = readdir(TMP);
closedir(TMP);

foreach $file (@files)
{
    next if($file =~ /^\./);
    $hashed = mkhash($file);
    
    if(exists $AVAIL{$hashed})
    {
	die "duplicate $hashed ($file <=> $AVAIL{$hashed})\n";
    }
    $AVAIL{$hashed} = $file;
    
    $hashed = mkhash_poor($file);
    if(exists $AVAIL_poor{$hashed})
    {
	$POOR_COLLISION{$hashed} = 1;
    }
    $AVAIL_poor{$hashed} = $file;
}

foreach $key (keys %POOR_COLLISION)
{
    print "delete $key\n";
    delete $AVAIL_poor{$key};
}

open(TMP, "$oldlist");
while($file = <TMP>)
{
    chomp($file);
    $hashed = mkhash($file);
    
    if(exists $AVAIL{$hashed})
    {
	push(@NEW, $AVAIL{$hashed});
	if($file eq $AVAIL{$hashed})
	{
	    print "Same   : $file\n";
	}
	else
	{
	    print "Upgrade: $file ---> $AVAIL{$hashed}\n";
	}
    }
    elsif(exists $AVAIL_poor{&mkhash_poor($file)})
    {
	$hashed = mkhash_poor($file);
	push(@NEW, $AVAIL_poor{$hashed});
	if($file eq $AVAIL_poor{$hashed})
	{
	    print "Same   : $file\n";
	}
	else
	{
	    print "Upgrade: $file ---> $AVAIL_poor{$hashed}\n";
	}
    }
    else
    {
	print     "NoMatch: $hashed  ($file)\n";
    }
}
close(TMP);

open(TMP, ">$newlist");
foreach $file (@NEW)
{
    print TMP "$file\n";
}
close(TMP);


sub mkhash
{
    my($file) = @_;
    return $file if($file eq "compat-libstdc++-296-2.96-144.el6.ppc.rpm");
    
    my @parts = reverse split(/\.|\-|_/, $file);
    if($#parts > 3)
    {
	$arch = $parts[1];
	if($file =~ /el6/)
	{
	    while($parts[0] !~ /el6/)
	    {
		splice(@parts,0,1);
	    }
	    splice(@parts,0,1);
	}
	else
	{
	    splice(@parts, 0,2);
	}
	while(($parts[0] !~ /^\D/) && ($parts[0] !~ /dpi/))
	{
	    splice(@parts,0,1);
	}
    }
    return join(".", $arch, @parts);
}

sub mkhash_poor
{
    my @parts = reverse split(/\.|\-|_/, $file);
    my @head = ();
    push(@head, $parts[1]);
    while($piece = pop @parts)
    {
	last if($piece =~ /\d/);
	push(@head, $piece);
    }
    return join(".", @head);
}
