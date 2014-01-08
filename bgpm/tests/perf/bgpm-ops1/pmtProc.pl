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
 
use strict;
use Data::Dumper;

my $cwd = `pwd -P`; chomp $cwd;
my $cfgName = shift @ARGV;
my $inFile = "$cfgName.out";
my $outFile = "$cfgName.processed";

print STDERR "cwd=$cwd, cfgName=$cfgName, inFile=$inFile outFile=$outFile\n";

my $curFiles=`ls`;
print STDERR "Files found in $cwd:\n";
print STDERR $curFiles, "\n";

open FILE, "$inFile" or die "ERROR: Unable to open $inFile; $!\n";
open OFILE, ">$outFile" or die "ERROR: Unable to open $outFile; $!\n";


sub AddLabel
{
	my $modRef = shift;
	my $label = shift;
	if (! exists($$modRef{'labels'}{$label})) {
		$$modRef{'labels'}{$label} = 1;
		push @{$$modRef{'labelOrder'}}, $label;
	}
	else {
		$$modRef{'labels'}{$label}++;
	}
}


# read input and keep only interesting data
my @modules;
my $curMod;    
my @inData;
my $line;
my $title;
while (defined($line = <FILE>)) {
	# get title and start an array for data
    if ($line =~ m/ TEST_MODULE /o) {
    	($title = $line) =~ s/.*TEST_MODULE\s+\S+\s+//o; chomp($title);
    	$curMod = {}; 
    	$$curMod{'title'} = $title;
        $$curMod{'labels'} = {};
        $$curMod{'labelOrder'} = [];
    	$$curMod{'data'} = [];
    	push @modules, $curMod;
    }
    elsif ($line =~ m/ \S*METRICS:/o) {
    	my ($name, $vals); 
    	($name, $vals) = ($line =~ m/.* \S*METRICS:\s+(.+?)\s*:\s+(.*)/o);
    	AddLabel($curMod, 'Variation');
    	my $curData = {};
    	$$curData{'Variation'} = $name;
    	
    	my @vals = split ' ', $vals;
    	foreach my $sval (@vals) {
            my ($label, $result);
            ($label, $result) = ($sval =~ m/(\S+)=(\S+)/o);
            $result =~ s/,$//o;
            AddLabel($curMod, $label);
            $$curData{$label} = $result;    	
    	}
    	push @{$$curMod{'data'}}, $curData; 
    }
}
close FILE;

#print Dumper(@modules);


# headers
print OFILE "#number of tables: ", scalar(@modules), "\n";

foreach my $table (@modules) {
	print OFILE "#title: $cfgName : ", $$table{'title'}, "\n";
	my $labels = join '"    "', @{$$table{'labelOrder'}};
	print OFILE "#labels: \"$labels\"\n";
	print OFILE "#\"$labels\"\n";
	foreach my $dataRef (@{$$table{'data'}}) {
		#print Dumper($dataRef);
		foreach my $label ( @{$$table{'labelOrder'}} ) {
			#print "$label\n";
			if (exists($$dataRef{$label}) ) {
				print OFILE '"', $$dataRef{$label}, '"   ';
			}
			else {
				print OFILE '"n/a"   ';
			}
		}
        print OFILE "\n";		
    }
}

close OFILE;


my $plcmd="PMTDraw -f $outFile";
print STDERR "$plcmd\n";
system($plcmd);
my $rc = $?;

$curFiles=`ls`;
$cwd = `pwd -P`; chomp $cwd;
print STDERR "Files found in $cwd:\n";
print STDERR $curFiles, "\n";

exit $rc;