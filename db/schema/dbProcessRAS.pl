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
# (C) Copyright IBM Corp.  2007, 2011                              
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

package DbPopulate;

#____________________________
#
#  Include needed modules
use strict;
use Getopt::Long;
use DBI;
use DBD::DB2;
use Config::IniFiles;
use XML::Simple;
#____________________________
# Globals
use vars
    qw($dbHandle $dbPropHash $debug);

#_____________________________________________________________
#
#       MAIN
#



#____________________________
# 
#  Input Variables
my $verify;
my %options;
my $dbprop=undef;
my $logdir=undef;
#____________________________

#_______________________
#
#  Process Command Line Options
#
#
GetOptions(
	\%options,'help','h','logdir=s' => \$logdir, 'properties=s' => \$dbprop);

if (defined $options{"h"} || defined $options{"help"}) {
  usage();
}



if ( !defined($logdir)) {
  print "\n\tERROR -  Specify -logdir \n";
  usage();
}

#_____________________________________
#
#       CONNECT TO DATABASE
# connect to the database

my $dbp = "/bgsys/local/etc/bg.properties";
if ($dbprop) {
    $dbp = $dbprop;
}
setupDb($dbp);


#
#_____________________________________


processRAS();


#_____________________________________________________________



#_______________________
#
#  Usage
#
#  Displays the usage of this script if -h is specified or if the wrong
#  arguments are passed in
#
sub usage {

system("perldoc $0");
exit(0);

}
#_______________________




##########################################
# setupDb()
# Make connection to the DB
##########################################
sub setupDb
{
    my $dbPropertiesFile = shift();
    #make sure we have the DB2 driver
    my @drivers = DBI->available_drivers();
    die "No perl DBI drivers found!\n" unless @drivers;

    #check if the DB2 driver is available
    my $foundDB2Driver = 0;
    foreach my $driver (@drivers)
    {
        if ($driver eq "DB2")
        {
	    $foundDB2Driver = 1;
	    last;
        } # if ($driver eq "DB2")
    } # foreach my $driver (@drivers)

    die "Could not find DBI::DB2 database driver, make sure it's installed!"
        if ($foundDB2Driver == 0);

    #open the db.properties file
    $dbPropHash = parseProperties($dbPropertiesFile, $dbPropHash);

    #check if we have a database name and schema
    die "Please specify --properties\n"
        unless defined $dbPropHash->{"name"} and defined $dbPropHash->{"schema_name"};
}

##########################################
# getDbHandle()
# Returns a handle to the database
##########################################
sub getDbHandle
{
    #connect to the database if we're not already
    if (!defined($dbHandle))
    {
        my $dsn = "dbi:DB2:" . $dbPropHash->{"name"};
        $dbHandle =
            DBI->connect($dsn, $dbPropHash->{"user"}, $dbPropHash->{"password"});

	# set schema based on the properties file
        my $schemaStmt = "SET SCHEMA " . $dbPropHash->{'schema_name'};

	print "\n" . $schemaStmt . "\n" if($debug);
        my $schemaStmtHandle = $dbHandle->prepare($schemaStmt);
        if (!$schemaStmtHandle->execute())
        {
            print "Result is " . getDbHandle()->errstr . ". EXITING\n";
            die "Could not execute $schemaStmtHandle";
        } # if (!$schemaStmtHandle->execute())
    } # if (!defined($dbHandle))

    return $dbHandle;

} # sub getDbHandle


##########################################
# parseProperties()
# Parses the database properties file for schema, uid, password
##########################################
sub parseProperties
{
    my $fileName = shift();
    my $db       = shift();
    my $prop = new Config::IniFiles( -file => $fileName );
    if (!defined($prop))
    {
       print "Could not read $fileName\n";
       foreach (@Config::IniFiles::errors) {
          print "$_\n";
       }
       exit(1);
    }

    # get database section
    my @section = $prop->Parameters( "database" );
    foreach my $key (@section)
    {
        my $val = $prop->val( "database", $key );
        print "adding $key=$val to hash\n" if ($debug);
        $db->{$key} = $val;
    }

    return $db;
} # sub parseProperties




sub processRAS {
      
    my $db_schema = $dbPropHash->{'schema_name'};
    
    my $sql = qq(select  runid, logdir  from bgqdiagruns  where insertedRAS = 'F' and logdir like '$logdir%');
    my $sth_diags = getDbHandle()->prepare($sql);
    $sth_diags->execute();
    
#    print $sql;
    my $diags;
    my $diagruns = 0;
    
    while ($diags = $sth_diags->fetchrow_hashref) {
        
        my $runid = $diags->{RUNID};
        my $logdir_inner = $diags->{LOGDIR};
        
        open (LOG, "find $logdir_inner -name rasFile.log  | ");
    
        while (<LOG>){   
            my $saveln = $_;
            print "Processing:  $saveln";

            open (RAS, "cat $saveln  | ");
    
            my $rascount=0;
            while (<RAS>){   
                my $rasln = $_;

                if (substr($_,0,9) eq "<RasEvent") {
                    $rascount++;
                    my $ras = XMLin($rasln);

                    $sql = qq(insert into tbgqdiagseventlog (msg_id,severity,event_time,location,message,runid)  values('$ras->{BG_ID}' ,'$ras->{BG_SEV}','$ras->{time}','$ras->{BG_LOC}','$ras->{BG_MSG}',$runid));
                    # print " $sql  \n";
                    my $sth_insert = getDbHandle()->prepare($sql);
                    $sth_insert->execute();

                }
                # print "Processing RAS:  $rasln";
            }
            print "-Inserted $rascount RAS events into $dbPropHash->{'schema_name'}.TBGQDiagsEventLog  \n";
        }

        $sql = qq( update bgqdiagruns set insertedras= 'T' where runid = $runid );
        my $sth_update = getDbHandle()->prepare($sql);
        # Run the SQL against the db engine
        $sth_update->execute();
        
        $diagruns++;
    }
    
    print "Found $diagruns directories to search \n";
}
    

__END__


=pod

=head1 NAME

dbProcessRAS.pl  (Processes diagnostics log files and inserts the RAS into the BGQDiagsEventLog table)

=head1 SYNOPSIS

B<dbProcessRAS.pl> [options]

=head1 DESCRIPTION

dbProcessRAS.pl will connect to the BGQ database and insert the RAS events from diagnostics logs into the database

=head1 OPTIONS

=over 5

=item B<-h, --help>

This help text

=item B<-properties> <properties-file-name>

DB properties file, defaults to /bgsys/local/etc/bg.properties

=item B<-logdir> I<directory-name>

The directory containing the diagnostics logs to process.  The directory, and all subdirectories, will be processed.

=back

NOTE: This utility requires the perl DBI module and DB2 database driver.  

=head1 AUTHOR

International Business Machines (IBM)

=cut  
