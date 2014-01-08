#!/usr/bin/perl
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
# Licensed Materials - Property of IBM                             
# Blue Gene/Q 5765-PER 5765-PRP                                    
#                                                                  
# (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           
# US Government Users Restricted Rights -                          
# Use, duplication, or disclosure restricted                       
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog

# This is a quick script to generate mpixl* from mpi{cc|cxx|f77} scripts and a single config file.
# Arguments are path-to-mpich2-install-dir
#
# It is run in the MPI-level makefile

use warnings;
use File::Copy;

$xlcomp_conf="mpixl.conf";

($bin_path = shift) or (die "Must provide location of existing mpixxx\n");
($target      = shift) or (die "Must provide target name (e.g. BGP or BGQ)\n");

(-e "$bin_path/mpicc") or (die "Must provide location of existing mpixxx\n");
(-e $xlcomp_conf) or (die "Couldn't find $xlcomp_conf\n");


$mpicc="$bin_path/mpicc";
$mpif77="$bin_path/mpif77";
$mpif90="$bin_path/mpif90";
$mpicxx="$bin_path/mpicxx";

$mpixlc="$bin_path/mpixlc";
$mpixlf77="$bin_path/mpixlf77";
$mpixlcxx="$bin_path/mpixlcxx";
$mpixlf90="$bin_path/mpixlf90";
$mpixlf95="$bin_path/mpixlf95";
$mpixlf2003="$bin_path/mpixlf2003";
$mpixlf2008="$bin_path/mpixlf2008";

$mpixlc_r="$bin_path/mpixlc_r";
$mpixlf77_r="$bin_path/mpixlf77_r";
$mpixlcxx_r="$bin_path/mpixlcxx_r";
$mpixlf90_r="$bin_path/mpixlf90_r";
$mpixlf95_r="$bin_path/mpixlf95_r";
$mpixlf2003_r="$bin_path/mpixlf2003_r";
$mpixlf2008_r="$bin_path/mpixlf2008_r";

# something to start with
copy($mpicc,  $mpixlc);     chmod(0755,$mpixlc);
copy($mpicxx, $mpixlcxx);   chmod(0755,$mpixlcxx);
copy($mpif77, $mpixlf77);   chmod(0755,$mpixlf77);
copy($mpif90, $mpixlf90);   chmod(0755,$mpixlf90);
copy($mpif90, $mpixlf95);   chmod(0755,$mpixlf95);
copy($mpif90, $mpixlf2003); chmod(0755,$mpixlf2003);
copy($mpif90, $mpixlf2008); chmod(0755,$mpixlf2008);

copy($mpicc,  $mpixlc_r);     chmod(0755,$mpixlc_r);
copy($mpicxx, $mpixlcxx_r);   chmod(0755,$mpixlcxx_r);
copy($mpif77, $mpixlf77_r);   chmod(0755,$mpixlf77_r);
copy($mpif90, $mpixlf90_r);   chmod(0755,$mpixlf90_r);
copy($mpif90, $mpixlf95_r);   chmod(0755,$mpixlf95_r);
copy($mpif90, $mpixlf2003_r); chmod(0755,$mpixlf2003_r);
copy($mpif90, $mpixlf2008_r); chmod(0755,$mpixlf2008_r);


# read in config variables
open(CONF, '<', $xlcomp_conf) or die;
%vars = map { (/^ *(\w+) *= *(\S+)/) ? ($1 => $2) : () } (<CONF>);
close CONF;


# start replacing variables in the mpixl* scripts with variables read in from $xlcomp
open MPICC,   "<$mpicc"    || die;
open MPIXLC,  ">$mpixlc"   || die;
open MPIXLCR, ">$mpixlc_r" || die;
while(<MPICC>)
{
    if (/^CC=/)
    {
        print MPIXLC  "CC=$vars{$target.'_C'}\n";
        print MPIXLCR "CC=$vars{$target.'_CR'}\n";
    }
    else {
        if (/^MPI_OTHERLIBS=/ && defined($vars{'MPI_OTHERLIBS'})) {
            $_="MPI_OTHERLIBS=$vars{'MPI_OTHERLIBS'}\n";
        }
        elsif (/^MPI_CFLAGS=/&& defined($vars{'MPI_CFLAGS'})) {
            $_="MPI_CFLAGS=$vars{'MPI_CFLAGS'}\n";
        }
        elsif (/^MPI_LDFLAGS=/ && defined($vars{'MPI_LDFLAGS'})) {
            $_="MPI_LDFLAGS=$vars{'MPI_LDFLAGS'}\n";
        }
        print MPIXLC  $_;
        print MPIXLCR $_;
    }
}
close MPICC;
close MPIXLC;
close MPIXLCR;


open MPICXX,    "<$mpicxx"     || die;
open MPIXLCXX,  ">$mpixlcxx"   || die;
open MPIXLCXXR, ">$mpixlcxx_r" || die;
while(<MPICXX>)
{
    if (/^CXX=/)
    {
        print MPIXLCXX  "CXX=$vars{$target.'_CXX'}\n";
        print MPIXLCXXR "CXX=$vars{$target.'_CXXR'}\n";
    }
    else {
        if (/^MPI_OTHERLIBS=/ && defined($vars{'MPI_OTHERLIBS'})) {
            $_="MPI_OTHERLIBS=$vars{'MPI_OTHERLIBS'}\n";
        }
        elsif (/^MPI_CXXFLAGS=/ && defined($vars{'MPI_CXXFLAGS'})) {
            $_="MPI_CXXFLAGS=$vars{'MPI_CXXFLAGS'}\n";
        }
        elsif (/^MPI_LDFLAGS=/ && defined($vars{'MPI_LDFLAGS'})) {
            $_="MPI_LDFLAGS=$vars{'MPI_LDFLAGS'}\n";
        }
        print MPIXLCXX  $_;
        print MPIXLCXXR $_;
    }

}
close MPICXX;
close MPIXLCXX;
close MPIXLCXXR;


open MPIF77,      "<$mpif77"       || die;
open MPIXLF77,    ">$mpixlf77"     || die;
open MPIXLF77R,   ">$mpixlf77_r"   || die;
while(<MPIF77>)
{
    if (/^F77=/) {
        print MPIXLF77    "F77=$vars{$target.'_F77'}\n";
        print MPIXLF77R   "F77=$vars{$target.'_F77R'}\n";
    }
    else {
        if (/^MPI_OTHERLIBS=/ && defined($vars{'MPI_OTHERLIBS'})) {
            $_="MPI_OTHERLIBS=$vars{'MPI_OTHERLIBS'}\n";
        }
        elsif (/^MPI_LDFLAGS=/ && defined($vars{'MPI_LDFLAGS'})) {
            $_="MPI_LDFLAGS=$vars{'MPI_LDFLAGS'}\n";
        }
        elsif (/^MPI_FFLAGS=/ && defined($vars{'MPI_FFLAGS'})) {
            $_="MPI_FFLAGS=$vars{'MPI_FFLAGS'}\n";
        }
        print MPIXLF77    $_;
        print MPIXLF77R   $_;
   }
}
close MPIF77;
close MPIXLF77;
close MPIXLF77R;


open MPIF90,      "<$mpif90"       || die;
open MPIXLF90,    ">$mpixlf90"     || die;
open MPIXLF95,    ">$mpixlf95"     || die;
open MPIXLF2003,  ">$mpixlf2003"   || die;
open MPIXLF2008,  ">$mpixlf2008"   || die;
open MPIXLF90R,   ">$mpixlf90_r"   || die;
open MPIXLF95R,   ">$mpixlf95_r"   || die;
open MPIXLF2003R, ">$mpixlf2003_r" || die;
open MPIXLF2008R, ">$mpixlf2008_r" || die;
while(<MPIF90>)
{
    if (/^FC=/) {
        print MPIXLF90    "FC=$vars{$target.'_F90'}\n";
        print MPIXLF90R   "FC=$vars{$target.'_F90R'}\n";
        print MPIXLF95    "FC=$vars{$target.'_F95'}\n";
        print MPIXLF95R   "FC=$vars{$target.'_F95R'}\n";
        print MPIXLF2003  "FC=$vars{$target.'_F2003'}\n";
        print MPIXLF2003R "FC=$vars{$target.'_F2003R'}\n";
        print MPIXLF2008  "FC=$vars{$target.'_F2008'}\n";
        print MPIXLF2008R "FC=$vars{$target.'_F2008R'}\n";
    }
    else {
        if (/^MPI_OTHERLIBS=/ && defined($vars{'MPI_OTHERLIBS'})) {
            $_="MPI_OTHERLIBS=$vars{'MPI_OTHERLIBS'}\n";
        }
        elsif (/^MPI_LDFLAGS=/ && defined($vars{'MPI_LDFLAGS'})) {
            $_="MPI_LDFLAGS=$vars{'MPI_LDFLAGS'}\n";
        }
        elsif (/^MPI_FFLAGS=/ && defined($vars{'MPI_FFLAGS'})) {
            $_="MPI_FFLAGS=$vars{'MPI_FFLAGS'}\n";
        }
        print MPIXLF90    $_;
        print MPIXLF95    $_;
        print MPIXLF2003  $_;
        print MPIXLF2008  $_;
        print MPIXLF90R   $_;
        print MPIXLF95R   $_;
        print MPIXLF2003R $_;
        print MPIXLF2008R $_;
   }
}
close MPIF90;
close MPIXLF90;
close MPIXLF95;
close MPIXLF2003;
close MPIXLF2008;
close MPIXLF90R;
close MPIXLF95R;
close MPIXLF2003R;
close MPIXLF2008R;
