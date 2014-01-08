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

# For manipulating MAC addresses
use Math::BigInt lib => 'GMP';
#____________________________
# Globals
use vars
    qw($dbHandle $dbPropHash $debug);



#_____________________________________________________________
#
#       MAIN
#
#_____________________________________________________________





#____________________________
# 
#  Input Variables
my $verify;
my %options;
my $dbprop=undef;
my $interface=undef;
my $locparm=undef;
my $itemname=undef;
my $itemvalue=undef;

#____________________________

#_______________________
#
#  Process Command Line Options
#
#
GetOptions(
           \%options,'help','h', 'properties=s' => \$dbprop,'location=s' => \$locparm, 
           'clear','itemname=s' => \$itemname,'itemvalue=s' => \$itemvalue);

if (defined $options{"h"} || defined $options{"help"}) {
  usage();
}


if ( !defined($locparm)) {
  print "\n\tERROR -  Specify -location \n";
  usage();
}

if ( !defined($itemname)) {
  print "\n\tERROR -  Specify -itemname \n";
  usage();
}

if ( !defined($itemvalue) && !defined $options{"clear"}) {
  print "\n\tERROR -  Specify -itemvalue or -clear \n";
  usage();
}

if ( defined($itemvalue) && defined $options{"clear"}) {
  print "\n\tERROR -  Specify -itemvalue or -clear, but not both \n";
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

if (defined $options{"clear"}) {
    clearIntf();
} else {
    processIntf();
}


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

sub clearIntf {
      
    my $db_schema = $dbPropHash->{'schema_name'};
    
    my $sql = qq(delete from bgqnetconfig where location like '$locparm%'  and interface = '' and itemname = '$itemname');
    my $sth = getDbHandle()->prepare($sql);
    $sth->execute();

    print "Cleared values for itemname $itemname  \n";

}

sub processIntf {
      
    my $db_schema = $dbPropHash->{'schema_name'};
    
    my $sql = qq(select  location  from bgqionode where location like '$locparm%' );
    my $sth = getDbHandle()->prepare($sql);
    $sth->execute();
    
   #  print $sql;
    my $rows;
    my $inserts = 0;
    my $value;

    $value = $itemvalue;
   
    while ($rows = $sth->fetchrow_hashref) {
        
        my $loc = $rows->{LOCATION};

        $sql = qq( insert into bgqnetconfig values('$loc', '', '$itemname' , '$value') );
        my $sth_ins = getDbHandle()->prepare($sql);
        # Run the SQL against the db engine
        $sth_ins->execute() || die "DB error";
      
             
        $inserts++;
    }
    
    print "Inserted $inserts rows into the database \n";
}
    

__END__


=pod

=head1 NAME

dbCustomize.pl  (Provides a node customization for Blue Gene nodes, which may be processed by the kernel during boot)

=head1 SYNOPSIS

B<dbCustomize.pl> [options]

=head1 DESCRIPTION

dbCustomize.pl will connect to the BGQ database and insert the customization values the database

=head1 OPTIONS

=over 5

=item B<-h, --help>

This help text

=item B<-properties> <properties-file-name>

DB properties file, defaults to /bgsys/local/etc/bg.properties

=item B<-itemname> I<item-name>

The item name for the customization being provided for the set of nodes

=item B<-itemvalue> I<item-value>

The value for this item

=item B<-location> I<location>

Designates the locations of the nodes for which the customization will be applied, i.e.  Q00, Q00-I0, R, Q, etc.

=item B<-clear> 

If -clear is specified instead of itemvalue, the existing values for the itemname will be cleared.

=back

NOTE: This utility requires the perl DBI module and DB2 database driver.  

=head1 AUTHOR

International Business Machines (IBM)

=cut  
