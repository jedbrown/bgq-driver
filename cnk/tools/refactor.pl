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

movefiles("filemove.txt");

@files = getfiles("cnk", "firmware", "hwi");

foreach $file (@files)
{
    print "$file\n";
}
retranslateincludes("filemove.txt");

VerifyConvertList("convertsymbols.txt");
#exit(0);

foreach $file (@files)
{
    print "file: $file\n";
    ApplyConvertList("convertsymbols.txt", $file);    
#    exit(0);
}

sub movefiles
{
    my($filelist) = @_;
    open(TMP, $filelist);
    while($line = <TMP>)
    {
	($oldlocation, $newlocation, $oldinclude, $newinclude) = split(/\s+/, $line);
	system("svn mv $oldlocation $newlocation");
    }
    close(TMP);
}

sub retranslateincludes
{
    my($filelist) = @_;
    foreach $file (@files)
    {
	$data = `cat $file`;
	
	open(MOVE, $filelist);
	while($mov = <MOVE>)
	{
	    ($oldlocation, $newlocation, $oldinclude, $newinclude) = split(/\s+/, $mov);
	    if($oldinclude =~ /\S/)
	    {
		if($data =~ /$oldinclude/)
		{
		    $data =~ s/#include.*$oldinclude.*/#include <$newinclude>/;
		}
	    }
	}
	close(MOVE);
	open(TMP, ">$file");
	print TMP $data;
	close(TMP);
    }
}

sub getfiles
{
    my @dirs = @_;
    my @files = ();
    foreach $dir (@dirs)
    {
	open(DIR, "find $dir |");
	while($file = <DIR>)
	{
	    chomp($file);
	    next if($file =~ /\/\./);
	    if(($file =~ /\.h$/) ||
	       ($file =~ /\.S$/) ||
	       ($file =~ /\.s$/) ||
	       ($file =~ /\.c$/) ||
	       ($file =~ /\.C$/) ||
	       ($file =~ /\.hpp$/) || 
	       ($file =~ /\.pl$/) || 
	       ($file =~ /\.cc$/))
	    {
		push(@files, $file);
	    }
	}
	close(DIR);
    }
    return @files;
}

sub VerifyConvertList
{
    my($convertlist) = @_;
    open(TMP, $convertlist);
    
    my %OLDNAMEHASH = ();
    my %NEWNAMEHASH = ();
    while($line = <TMP>)
    {
	($oldname, $newname, $parn, $type) = split(/\s+/, $line);
	next if($oldname =~ /headers:/);
	
	if(exists $OLDNAMEHASH{$oldname})
	{
	    if($OLDNAMEHASH{$oldname} ne $newname)
	    {
		print "Duplicate/Mismatch: $oldname.   variant1=$OLDNAMEHASH{$oldname}  variant2=$newname\n";
		print "Aborting\n";
		exit(-1);
	    }
	    else
	    {
		print "Duplicate name: $oldname\n";
	    }
	}
	$OLDNAMEHASH{$oldname} = $newname;
    }    
}

sub ApplyConvertList
{
    my($convertlist, $file_to_convert) = @_;

    my $data = `cat $file_to_convert`;
    open(TMP, $convertlist);
    
    while($line = <TMP>)
    {
	($oldname, $newname, $parn, $type) = split(/\s+/, $line);
	next if($oldname =~ /headers:/);
	
	if($parn)
	{
	    $data =~ s/(?<=\W)$oldname\s*\(/$newname\(/g;
	}
	$data =~ s/(?<=\W)$oldname(?=\W)/$newname/g;
    }    
    close(TMP);
    
    open(TMP, ">$file_to_convert");
    print TMP $data;
    close(TMP);
}
