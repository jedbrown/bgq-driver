#!/usr/bin/perl
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp.  2010, 2012                              
# US Government Users Restricted Rights - Use, duplication or      
#   disclosure restricted by GSA ADP Schedule Contract with IBM    
#   Corp.                                                          
#                                                                  
# This software is available to you under the Eclipse Public       
# License (EPL).                                                   
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               

# Usage:
#### Update XML calibration file ####
# ./hsscal.pl --location=Q00-I0-J04 --numnodes=1 --mode=xml --out=out/bqc_hss_bgqssn0.xml --in=out/bqc_hss_bgqssn0.xml --stock=stock/bqc_hss.xml.stock.bgqssn0
# This will calibrate Q00-I0-J04 and paste the node settings into --in=<filename> and save it in --out=<filename>. The stock file is only used by
# Tom's updateHssXml tool. If you want to use the stock file settings you need to use it as --in=<stockfile> AND make sure mcserver did load it BEFORE you do
# the calibration!
#
#### Generate schmooing file ####
# ./hsscal.pl --location=Q00-I0-J06 --numnodes=1 --mode=schmoo --out=out.schmoo
# This will generate a schmooo file for Q00-I0-J06.
#
#### Update VPD memory ####
# ./hsscal.pl --location=Q00-I0-J04 --numnodes=1 --mode=xml
# NOT IMPLEMENTED YET! This will update the calibration data directly in the node's eeprom. Control system cannot read this information yet!

use strict;
use Getopt::Long;

#use FindBin;
#use lib "$FindBin::Bin/lib/lib/perl5";
#use Bit::Vector::Overload;

#my $DRIVER="$ENV{BGQHOME}/bgq";
my $DRIVER="/bgsys/drivers/ppcfloor";
chomp(my $TIMESTAMP=`date +%m.%d.%y.%H.%M.%S`);
chomp(my $SYSTEM=`hostname -s`);
my $CWD="$ENV{PWD}";

#my $BQC_STOCK="/gsa/yktgsa/home/m/w/mwkaufma/calibration/stock/bqc_hss.xml.stock.$SYSTEM";
#my $BQL_STOCK="/gsa/yktgsa/home/m/w/mwkaufma/calibration/stock/bql_hss.xml.stock.$SYSTEM";
#my $BQC_XML_FILE="/gsa/yktgsa/home/m/w/mwkaufma/calibration/out/bqc_hss_$SYSTEM.xml";
#my $BQL_XML_FILE="/gsa/yktgsa/home/m/w/mwkaufma/calibration/out/bql_hss_$SYSTEM.xml";

my $location;
my $numnodes;
my $mode;
my $infile;
my $outfile;
my $stock;

GetOptions('location=s' => \$location,
           'numnodes=i' => \$numnodes,
           'in=s'       => \$infile,
           'out=s'      => \$outfile,
           'stock=s'    => \$stock,
           'mode=s'     => \$mode);

my $explore_all = ($mode eq "schmoo" || $mode eq "vpd") ? 1 : 0;
my $filename    = hss_rx_cal($location, $numnodes, $explore_all);

if ($filename ne "")  {
    printf "mode = $mode\n";
    if ($mode eq "vpd") {
        gen_vpd($location, $numnodes, $filename);
    } elsif ($mode eq "schmoo") {
        gen_schmoo($filename, $outfile);
    } elsif ($mode eq "xml") {
        gen_xml($location, $numnodes, $filename, $infile, $outfile, $stock);
    }
} else {
    printf "Calibration failed\n";
}

sub gen_vpd {

}

sub gen_schmoo {
    my $filename;
    my $outfile;
    ($filename, $outfile) = @_;

    my $ld;

    open(FILE, "$filename");
    open(OUT, ">$outfile");

    while(my $l = <FILE>) {
        if ($l =~ m/{(.*)}0:0 Link ([ABCDEI])([+-]) lane (\d):(.*)/) {
            $ld  = $2;
            $ld .= ($3 eq "-") ? "M" : "P";
            $ld  = ($2 eq "I") ? "IO" : $ld;
            
            print OUT "$1-$ld-L$4:$5\n";
        }
    }

    close(OUT);
    close(FILE);

    printf "Generated schmooing file $outfile\n";
}

sub gen_xml {
    # generates xml file with updateHssXml and merges this xml file with the old one.
    # Only the node settings are taken from the new xml file and pasted into the old
    # one. Already existing node settings in the old file are being overwritten.
    my $location;
    my $numnodes;
    my $filename;
    my $infile;
    my $outfile;
    my $stock;
    ($location, $numnodes, $filename, $infile, $outfile, $stock) = @_;

    chomp (my $newfile = "bqc_hss_${SYSTEM}_temp.$TIMESTAMP");

    my @args =("$DRIVER/scantools/bin/updateHssXml",
               "--file $stock",
               "--calibration $filename",
               "--raw",
               "> $newfile");

    my $cmd = join (' ', @args);
    
    system($cmd) == 0
        or die "system @args failed: $?";

    printf "Merging $newfile with $infile into $outfile ...";

    my %nodes;

    # Extract the node settings from the current bqc_hss.xml file
    open(FILE, "$infile");
    while (my $l = <FILE>) {
        if ($l =~ m/setting key='(.*)' rxeq=.*txeq=.*txamp=.*rxvosd0=.*rxvosd1=.*rxvose0=.*rxvose1=.*rxvoseq=.*/) {
            chomp($nodes { $1 } = $l);
        }
    }
    close(FILE);

    # Extract the node settings from the newly generated bqc_hss.xml file.
    # Remove all node settings from the old settings if there are new settings.
    open(FILE, "$newfile");
    while (my $l = <FILE>) {
        if ($l =~ m/setting key='(.*)' rxeq=.*txeq=.*txamp=.*rxvosd0=.*rxvosd1=.*rxvose0=.*rxvose1=.*rxvoseq=.*/) {
            chomp($nodes{$1} = $l);
        }
    }
    close(FILE);

    # Use the old file as a skeleton to put in the merged contents into the new file
    open(FILE, "$infile");
    my @in = <FILE>;
    close(FILE);

    open(FILE, ">$outfile");
    foreach my $l (@in) {
        if($l =~ m/^<\/hsssettings>$/) {
            foreach my $key (sort (keys(%nodes))) {
                print FILE "$nodes{$key}\n";
            }
        }
        # filter old settings, they are already in the %nodes hash
        if ($l !~ m/setting key='(.*)' rxeq=.*txeq=.*txamp=.*rxvosd0=.*rxvosd1=.*rxvose0=.*rxvose1=.*rxvoseq=.*/) {
            print FILE $l;
        }
    }
    close(FILE);

    printf "done\n";
}

sub hss_rx_cal {
    my $location;
    my $numnodes;
    my $explore_all;
    ($location, $numnodes, $explore_all) = @_;

    my @args = ("$DRIVER/scripts/runfctest.sh",
                "--ttype fwddr",
                "--program $DRIVER/firmware/tests/hss_rx_cal/hss_rx_cal.elf",
                "--svchost $CWD/hss_rx_cal.svchost",
                "--script runMmcsLite",
                "--timelimit 1080",
                "--location $location",
                "--numnodes $numnodes",
                "--bootoptions tolerate_link_failures,bqc_read_hss_xml_file,bql_read_hss_xml_file");

    if ($explore_all == 1) {
        push(@args, "--envvar HSS_EXPLORE_ALL=1");
    }

    system(@args) == 0 or die "system @args failed: $?";

    chomp(my $logfile=`ls -t FCTest.hss_rx_cal.*/RunMmcsLite.Log1 | head -1`);
    chomp(my $passed=`grep \"Software Test PASS\" $logfile | wc -l`);
    
    if ($passed == $numnodes) {
        printf "PASS: $passed/$numnodes passed calibration.\n";
        return $logfile;
    } else {
        printf "FAIL: $passed/$numnodes passed calibration.\n";
        return "";
    }
}


exit;

#echo -n "Constructing xml file ... "

#${DRIVER}/scantools/bin/updateHssXml \
#  --file ${BQC_STOCK} \
#  --calibration ${LOG} \
#  --raw > ${BQC_XML_FILE}.${TIMESTAMP}
#echo "done"
#echo "cmd: ${DRIVER}/scantools/bin/updateHssXml --file ${BQC_STOCK} --calibration ${LOG} --raw INTO ${BQC_XML_FILE}.${TIMESTAMP}"
