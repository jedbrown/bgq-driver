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
use Net::IP;
#____________________________
# Globals
use vars
    qw($dbHandle $dbPropHash $debug);

my $ip;
my $ipAddress=undef;

#____________________________
# 
#  Input Variables
my $verify;
my %options;
my $dbprop=$ENV{BG_PROPERTIES_FILE};
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
	\%options,'help','h','interface=s' => \$interface, 'properties=s' => \$dbprop,'location=s' => \$locparm, 
	   'increment', 'clear','itemname=s' => \$itemname,'itemvalue=s' => \$itemvalue,'list');

if (defined $options{"h"} || defined $options{"help"}) {
  usage();
}

my $incr = 0;
if (defined $options{"increment"}) {
    $incr = 1;
}


if ( !defined($options{"list"}) && !defined($interface)) {
  print "\n\tERROR -  Specify -interface \n";
  usage();
}

if ( !defined($options{"list"}) && !defined($locparm)) {
  print "\n\tERROR -  Specify -location \n";
  usage();
}

if ( !defined($options{"list"}) && !defined($itemname)) {
  print "\n\tERROR -  Specify -itemname \n";
  usage();
}

if ( !defined($itemvalue) && !defined $options{"clear"} && !defined $options{"list"}) {
  print "\n\tERROR -  Specify -itemvalue, -clear, or -list\n";
  usage();
}

if ( defined($itemvalue) && defined $options{"clear"} && defined $options{"list"}) {
  print "\n\tERROR -  Specify -itemvalue, -clear, or -list but not all three\n";
  usage();
}

if ( defined($itemvalue) && defined $options{"clear"}) {
  print "\n\tERROR -  Specify -itemvalue or -clear, but not both\n";
  usage();
}

if ( defined($itemvalue) && defined $options{"list"}) {
  print "\n\tERROR -  Specify -itemvalue or -list, but not both\n";
  usage();
}

if ( defined $options{"clear"} && defined $options{"list"}) {
  print "\n\tERROR -  Specify -clear or -list, but not both\n";
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
} elsif (defined $options{"list"}) {
    listIntf();
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
    
    my $sql = qq(delete from bgqnetconfig where location like '$locparm%'  and interface = '$interface' and itemname = '$itemname');
    my $sth = getDbHandle()->prepare($sql);
    $sth->execute();

    print "Cleared values for interface $interface and itemname $itemname  \n";

}

sub listIntf {
      
    my $db_schema = $dbPropHash->{'schema_name'};
    
    my $sql = qq(select location, interface, itemname, itemvalue from bgqnetconfig where location like '$locparm%');
    my $sth = getDbHandle()->prepare($sql);
    $sth->execute();

    my $rows;

    printf("%-16s %-16s %-16s %-32s\n","Location","Interface","Name","Value");
    printf("%-16s %-16s %-16s %-32s\n","--------","---------","----","-----");
    while ($rows = $sth->fetchrow_hashref) {
        my $loc = $rows->{LOCATION};
        my $interface = $rows->{INTERFACE};
        my $name = $rows->{ITEMNAME};
        my $value = $rows->{ITEMVALUE};
        printf("%-16s %-16s %-16s %-32s\n",$loc,$interface,$name,$value);
    }
}

sub processIntf {
      
    my $db_schema = $dbPropHash->{'schema_name'};
    
    my $sql = qq(select  location  from bgqionode where location like '$locparm%' );
    my $sth = getDbHandle()->prepare($sql);
    $sth->execute();
    
    my $rows;
    my $inserts = 0;
    my $value;

    $value = $itemvalue;
    if ($incr) {
        $ip = new Net::IP($itemvalue);
        $value = $ip->ip();
    }

    while ($rows = $sth->fetchrow_hashref) {
        
        my $loc = $rows->{LOCATION};

        if ($incr) {
            die "IP range exceeded" if (!defined($ip));
            print "$loc $value \n";
        }
        $sql = qq( insert into bgqnetconfig values('$loc', '$interface', '$itemname' , '$value') );
        my $sth_ins = getDbHandle()->prepare($sql);
        $sth_ins->execute() || die "DB error";
        
        if ($incr) {
            $ip++;
            if (defined($ip))  {
                $value = $ip->ip();
            }
        }
        
        $inserts++;
    }
    
    print "Inserted $inserts rows into the BGQNetConfig \n";
}
    

__END__


=pod

=head1 NAME

dbNetConfig.pl  (Establishes network configuration information for Blue Gene nodes)

=head1 SYNOPSIS

B<dbNetConfig.pl> [options]

=head1 DESCRIPTION

dbNetConfig.pl will connect to the BGQ database and insert the network configuration information into the database

=head1 OPTIONS

=over 5

=item B<-h, --help>

This help text

=item B<--properties> properties-file-name

Blue Gene properties file.

=item B<--interface> I<interface-name>

The interface being established, such as external1, external2 or torus.

=item B<--itemname> I<interface-item-name>

The item of information being provided for this interface, such as ipv4address, ipv6address, ipv4netmask, etc.

=item B<--itemvalue> I<item-value>

The value for this item, which will be incremented if -increment is also specified.
For values being incremented, this can be an IP address.
An IP address can be specified in either IPv4 or IPv6 format.  If increment is specified, an IP address must be 
a range large enough to handle the number of values to be assigned, i.e.  195.114.80/24  or 192.1.1.1-192.1.1.22.

=item B<--increment>

If specified, the item value will be incremented for each node.

=item B<--location> I<location>

Designates the locations of the nodes for which the network configuration will be applied, i.e.  Q00, Q00-I0, R, Q, etc.

=item B<--clear> 

If -clear is specified instead of itemvalue, the existing values for the interface and itemname will be cleared.

=item B<--list> 

Display configuration values rather than insert or clear them.

=back

NOTE: This utility requires the perl DBI module and DB2 database driver, as well as the perl Net::IP module.  

NOTE: Database connection information will be obtained from the bg.properties file specified by
BG_PROPERTIES_FILE environment variable. If that environment is not defined, it will use 
/bgsys/local/etc/bg.properties by default.

=head1 AUTHOR

International Business Machines (IBM)

=cut  
