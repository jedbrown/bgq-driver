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

# Pass up all errors
use strict;
use warnings;

# Let me use more obvious variable names
use English;

# For debug
use Data::Dumper;

# Use the database module
use DBI;

# use the basename module
use File::Basename;

# pod usage
use Pod::Usage;

# For argument parsing
use Getopt::Long;

# For parsing bg.properties
use Config::IniFiles;

# Globals
use vars
    qw($size $doneLinks $doneIO $doneClock $dbHandle $propHash @bgqMidplanes *INFILE $debug $ipAddress $interface);

#initial undef's
my ($size, $propHash, $dbHandle, $optProceed, $bgPropertiesFile) = undef;

#initialize 0's
my ($doneLink, $doneIO, $doneClock) = 0;

my @bgqMidplanes = ();

my $insertdrawer = undef;

my $drawers = 1;

my $debug = 0;

# initial IP address (incremented montonically by $nextIpAddress()
my $ipAddress = "172.16.100.0";

# interface name to use for IP addresses
my $interface = "ib0";
 
# in the spirit of inet_aton(3) convert an address given as a dotted
# quad into an integer
sub inet_atoi($) {
    my $ipaddr = shift;
    my @bytes;
    
    @bytes = (
              $ipaddr =~ /^(25[0-5]|2[0-4][0-9]|[01][0-9][0-9]|[0-9][0-9]|[0-9])\.
              (25[0-5]|2[0-4][0-9]|[01][0-9][0-9]|[0-9][0-9]|[0-9])\.
              (25[0-5]|2[0-4][0-9]|[01][0-9][0-9]|[0-9][0-9]|[0-9])\.
              (25[0-5]|2[0-4][0-9]|[01][0-9][0-9]|[0-9][0-9]|[0-9])$/x
              );
    $ipaddr = 0;
    for (@bytes) {
        $ipaddr <<= 8;
        $ipaddr += $_;
    }
    return $ipaddr;
}

# in the spirit of inet_ntoa(3) convert a network address expressed as
# an integer into the typical dotted-quad representation.
sub inet_itoa($) {
    my $ipaddr = shift;
    my @bytes;
    
    for ( 1 .. 4 ) {
        unshift @bytes, $ipaddr & 0xff;
        $ipaddr >>= 8;
    }
    return sprintf "%u.%u.%u.%u", @bytes;
}

##########################################
# nextIpAddress()
# increment the $ipAddress global variable
# and return it
##########################################
sub nextIpAddress()
{
    my $ip = inet_atoi($ipAddress);
    $ip = $ip + 1;
    $ipAddress = inet_itoa($ip);
    return $ipAddress;
}


##########################################
# createInitialEntries()
# Inserts initial entries into TBGQProductType,  TBGQMachine, TBGQMachineSubnet
#  TBGQEthGateway, and TBGQEGWMachineMap
##########################################
sub createInitialEntries
{
    my @insertStmts = (
                       "INSERT INTO TBGQProductType (productid, description) values('74Y5253','Bulk Power Module')",
                       "INSERT INTO TBGQProductType (productid, description) values('P009',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('MP000000',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('C0000000','Compute Node initial value')",
                       "INSERT INTO TBGQProductType (productid, description) values('P000',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P002',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P004',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P006',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P008',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('BGQ00002',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('NC000000',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P001',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P003',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P005',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('P007',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('BGQ00001',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('NB000000',NULL)",
                       "INSERT INTO TBGQProductType (productid, description) values('I0000000','IO Node initial value')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQCK','BG/Q Clock Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQMP','BG/Q Midplane')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQSV','BG/Q Service Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQDA','BG/Q Node DCA')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQN8','BG/Q Node Board')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQN9','BG/Q Node Board')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQI5','BG/Q I/O Drawer')",
                       "INSERT INTO TBGQProductType (productid, description) values('BQI6','BG/Q I/O Drawer')",
                       "INSERT INTO TBGQProductType (productid, description) values('50Y7928','BG/Q Link Chip')",
                       "INSERT INTO TBGQProductType (productid, description) values('WES1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WFS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WMS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WSS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WUS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('W7S1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('W3S1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WES2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WFS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WMS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WSS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WUS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WED2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WFD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WMD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WSD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WUD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WWD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AES1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AFS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AMS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('ASS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AUS1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('A7S1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('A3S1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AES2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AFS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AMS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('ASS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AUS2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AED2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AFD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AMD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('ASD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AUD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WED1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WFD1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WMD1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WSD1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('WUD1','BG/Q Compute Card')",
                       "INSERT INTO TBGQProductType (productid, description) values('AWD2','BG/Q Compute Card')",
                       "INSERT INTO TBGQMachine  (serialnumber, productid, snipv4address, mtu, clockhz,bringupoptions,bgsysremotepath,bgsysipv4address) values('BGQ','BGQ00001','172.16.1.2',9000,1600,'fischer_connector_clock,phy_delay=15','/bgsys','172.16.1.2')",
                       "INSERT INTO TBGQMachineSubnet values('BGQ','10.255.255.25','255.0.0.0')",
                       "INSERT INTO TBGQEthGateway (serialnumber,machineserialnumber,productid,ipaddress,broadcast,mask) values ('G8T000','BGQ', 'P002', '172.16.3.4','172.16.255.255','255.255.0.0')",
                       "INSERT INTO TBGQNodeConfig (NodeConfig) values('CNKDefault')",
                       "INSERT INTO TBGQDomainMap values ('CNKDefault','CNK',0,-1, 0, -1,'/bgsys/drivers/ppcfloor/boot/cnk',NULL,0)",
                       "INSERT INTO TBGQNodeConfig (NodeConfig) values('IODefault')",
                       "INSERT INTO TBGQDomainMap values ('IODefault','Linux',0,16,0,-1,'/bgsys/drivers/ppcfloor/boot/linux,/bgsys/drivers/ppcfloor/boot/ramdisk',NULL,17179803648)"
                       );

    foreach my $stmt (@insertStmts)
    {
        processSQL($stmt);
    }

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
        my $dsn = "dbi:DB2:" . $propHash->{"name"};
        $dbHandle =
            DBI->connect($dsn, $propHash->{"user"}, $propHash->{"password"});

	# set schema based on the properties file
        my $schemaStmt = "SET SCHEMA " . $propHash->{'schema_name'};

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

    # get database section
    my $rows =  $prop->val( "database", "computeRackRows" );
    my $columns =  $prop->val( "database", "computeRackColumns" );
    if (defined($rows) && defined($columns))
    {
        $db->{"rows"} = $rows;
        $db->{"columns"} = $columns;
        print "rows $rows columns $columns\n" if ($debug);
    }
    else
    {
        print "missing computeRackRows or computeRackColumns keys in database section of properties\n" if ($debug);
    }

    return $db;
} # sub parseProperties

##########################################
# processClocks()
# Prints the help text for this program
##########################################
sub processClocks
{
    my @rows          = @_;
    my %clock_cards;
   
    print "Gathering the clock information for a <$size> system\n";
    # Do something to each row
    foreach my $row (@rows)
    {
        # remove newlines from the row
        $row =~ s/\n//;

        # split rows on ", " so we can find each endpoint of the clock cable
        my @clockCables = split(/\,/, $row);

        # insert clock cards
        if ( scalar(@clockCables) == 2 ) {
            if ( $clockCables[0] =~ /([RQ][[:alnum:]]{2}-K[01]?)/ ) {
                $clock_cards{$clockCables[0]} = undef
            } elsif ( $clockCables[1] =~ /([RQ][[:alnum:]]{2}-K[01]?)/ ) {
                $clock_cards{$clockCables[1]} = undef
            }
            
            # insert clock cable
            my $addStmt = "INSERT INTO TBGQClockCable \(fromlocation,tolocation) values\('$clockCables[0]', '$clockCables[1]'\)";
            processSQL($addStmt);   
        }    

        # add midplanes to array
        if ( scalar(@clockCables) == 1 && $clockCables[0] =~ /(R[[:alnum:]]{2}-M[0-1])/) {
            push(@bgqMidplanes, $clockCables[0]);
        }
    } # foreach my $row (@rows)
 
    # insert clock cards
    foreach my $clock_card (keys %clock_cards) {
        my $addStmt = "INSERT INTO TBGQClockCard \(location) values \('$clock_card'\)";
        processSQL($addStmt);
    }
    processMidplanes();
} # sub processClocks

##########################################
# processLinks()
# Prints the help text for this program
##########################################
sub processLinks
{
    my @rows = @_;

    # Do something to each row
    foreach my $row (@rows)
    {
        # remove newlines from the row
        $row =~ s/\n//;
        
        # split rows on ", " so we can find each endpoint of the clock cable
        my @linkCables = split(/\,/, $row);
        
        if (scalar(@linkCables) == 2) {              
            # insert cable
            my $addStmt = "INSERT INTO TBGQCable \(fromlocation,tolocation) values\('$linkCables[0]', '$linkCables[1]'\)";
            processSQL($addStmt);   
        }     
        
    } # foreach my $row (@rows)
} # sub processLinks

##########################################
# processMidplanes()
# Process the midplane unique data that can be extrapolated from their location
##########################################
sub processMidplanes
{
    my @nodePositions = (
                         "N00", "N01", "N02", "N03", "N04", "N05", "N06", "N07",
                         "N08", "N09", "N10", "N11", "N12", "N13", "N14", "N15"
                         );
    my @nodeCardPositions = (
                             "J00", "J01", "J02", "J03", "J04", "J05", "J06", "J07", "J08", "J09", "J10",
                             "J11", "J12", "J13", "J14", "J15", "J16", "J17", "J18", "J19", "J20",
                             "J21", "J22", "J23", "J24", "J25", "J26", "J27", "J28", "J29", "J30",
                             "J31"
                             );
 
    my @bulkPowerModulePositions = ( "P0", "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8" );
    my @dimensions = ("A",  "B",  "C", "D");

    my @linkPositions = ( "U00", "U01", "U02", "U03", "U04", "U05", "U06", "U07", "U08" );

    print "Processing midplane specific information\n";
    foreach my $mp (@bgqMidplanes)
    {
        my @addStmts = ();
        my $mac;
        die "invalid midplane syntax $mp\n" unless $mp =~ /R([[:alnum:]])([[:alnum:]])-M([0-1])/ ;
        my $row = $1;
        my $col = $2;
        my $mid = $3;

        my $midplaneLong = "R" . $row . $col . "-M" . $mid;
	
        my $lastOctetBase = $mid;
	
	$lastOctetBase = $lastOctetBase << 7;

	my $lastOctectService = $lastOctetBase | 0x10;
	my $ip = "10." . $row . "." . $col. "." . $lastOctectService;


        print "Adding midplane:  $midplaneLong \n";
        push(@addStmts,"INSERT INTO TBGQMidplane (productid, machineserialnumber, posinmachine) values('MP000000','BGQ','$midplaneLong')");
        push(@addStmts,"INSERT INTO TBGQServiceCard (productid, midplanepos) values('P009','$midplaneLong ')");
        push(@addStmts,"INSERT INTO TBGQIcon (licenseplate, containerlocation, ipaddress)  values(x'FFF29F151EF1000D60EAE10E','$midplaneLong-S', '$ip' )");

    
        my $ncOctet = 0x30;
	foreach my $n (@nodePositions)
        {
            my $lastOctetNode = $lastOctetBase | $ncOctet++;
            my $ip = "10." . $row . "." . $col. "." . $lastOctetNode;

            push(@addStmts,"INSERT INTO TBGQNodeCard (productid, midplanepos, position) values('MP000000','$midplaneLong', '$n')");

            push(@addStmts,"INSERT INTO TBGQNodeCardDCA (productid, midplanepos, nodecardpos, position) values('BQDA','$midplaneLong', '$n', 'D0')");
            push(@addStmts,"INSERT INTO TBGQNodeCardDCA (productid, midplanepos, nodecardpos, position) values('BQDA','$midplaneLong', '$n', 'D1')");

            push(@addStmts,"INSERT INTO TBGQIcon (licenseplate,containerlocation, ipaddress)  values(x'FFF29F151EF1000D60EAE10E','$midplaneLong-$n', '$ip' )");
            foreach my $nc (@nodeCardPositions)
            {
                push(@addStmts,"INSERT INTO TBGQNode (productid, midplanepos, nodecardpos, position, voltage) values('C0000000','$midplaneLong', '$n','$nc', 1.2)");
            } # foreach my $nc (@nodeCardPositions)

            foreach my $link (@linkPositions)
            {
                next if ( $drawers == 2 && $link eq "U04" && $n !~ "N(00|02|04|06|08|10|12|14)" );
                next if ( $drawers == 1 && $link eq "U04" && $n !~ "N(00|04|08|12)" );
                push(@addStmts, "INSERT INTO TBGQLinkChip (productid, midplanepos,nodecardpos,position) values('50Y7928', '$midplaneLong', '$n','$link')");
            } #  foreach my $link (@linkPositions)

        } # foreach my $n (@nodePositions)
    

        if ($mid == 0) {
           foreach my $bpm (@bulkPowerModulePositions)
           {
               my $location = "R" . $row . $col . "-B0-" . $bpm;
               push(@addStmts,"INSERT INTO TBGQBulkPowerSupply (productid, location, status) values('74Y5253', '$location', 'A')");
               $location = "R" . $row . $col . "-B1-" . $bpm;
               push(@addStmts,"INSERT INTO TBGQBulkPowerSupply (productid, location, status) values('74Y5253', '$location', 'A')");
               $location = "R" . $row . $col . "-B2-" . $bpm;
               push(@addStmts,"INSERT INTO TBGQBulkPowerSupply (productid, location, status) values('74Y5253', '$location', 'A')");
               $location = "R" . $row . $col . "-B3-" . $bpm;
               push(@addStmts,"INSERT INTO TBGQBulkPowerSupply (productid, location, status) values('74Y5253', '$location', 'A')");
           }
        }

	foreach my $dim (@dimensions)
	{
            my $fullValue = $dim . "_" . $midplaneLong;
            push(@addStmts,"INSERT INTO TBGQSwitch \(switchID, midplanePos, machineSerialNumber, dimension, status\) values \('$fullValue', '$midplaneLong', 'BGQ', '$dim', 'A'\)");
	}

      

	foreach my $sql (@addStmts)
	{
            processSQL($sql);
	}
    } # foreach my $mp (@bgqMidplanes)

    # compute the torus coords for each midplane
    my $mplink = "R00-M0";
    my @torus  = ( 1,1,1,1 );
    my $dest;
    my $sql;
    my $stmt;

    for (my $count=0; $count < 4; $count++) {
        $sql = qq(select destination from bgqlink where source='$dimensions[$count]_$mplink');
        print $sql . "\n" if($debug);
        $stmt = getDbHandle()->prepare($sql);
        $stmt->execute();
        while ($dest = $stmt->fetchrow_hashref) {
            my $newdest = $dest->{DESTINATION};
            $torus[$count]++;
            $sql = qq(select destination from bgqlink where source='$newdest' and destination<>'$dimensions[$count]_$mplink');
            print $sql . "\n" if ($debug);
            $stmt = getDbHandle()->prepare($sql);                                                                    
            $stmt->execute();
        }
        print "Torus size:  $dimensions[$count]:  $torus[$count]  \n";
    }

    for (my $adim=0,my $abase=$mplink; $adim < $torus[0]; $adim++) {
        for (my $bdim=0,my $bbase=$abase; $bdim < $torus[1]; $bdim++) {
            for (my $cdim=0,my $cbase=$bbase; $cdim < $torus[2]; $cdim++) {
                for (my $ddim=0,my $dbase=$cbase; $ddim < $torus[3]; $ddim++) {
                    my $upd=qq(update bgqmidplane set torusa=$adim,torusb=$bdim,torusc=$cdim,torusd=$ddim where location = '$dbase');  
                    processSQL($upd); 
                    $sql = qq(select substr(destination,3) from bgqlink where source='D_$dbase');
                    print $sql . "\n" if($debug);
                    $stmt = getDbHandle()->prepare($sql);
                    $stmt->execute();
                    if ($dest = $stmt->fetchrow_hashref) {
                        $dbase = $dest->{1};
                    }
                }
                $sql = qq(select substr(destination,3) from bgqlink where source='C_$cbase');
                print $sql . "\n" if($debug);
                $stmt = getDbHandle()->prepare($sql);
                $stmt->execute();
                if ($dest = $stmt->fetchrow_hashref) {
                    $cbase = $dest->{1};
                }
            }
            $sql = qq(select substr(destination,3) from bgqlink where source='B_$bbase');
            print $sql . "\n" if($debug);
            $stmt = getDbHandle()->prepare($sql);
            $stmt->execute();
            if ($dest = $stmt->fetchrow_hashref) {
                $bbase = $dest->{1};              
            }    
        }
        $sql = qq(select substr(destination,3) from bgqlink where source='A_$abase');
        print $sql . "\n" if($debug);
        $stmt = getDbHandle()->prepare($sql);
        $stmt->execute();
        if ($dest = $stmt->fetchrow_hashref) {
            $abase = $dest->{1};     
        }
    } 
} # sub processMidplanes


##########################################
# processIO()
# Process the midplane unique data that can be extrapolated from their location
##########################################
sub processIO
{
    my @nodeCardPositions = (
                             "J00", "J01", "J02", "J03", "J04", "J05", "J06", "J07" );

    my @powerModulePositions = ( "P0", "P1", "P2", "P3", "P4", "P5" ); 

    my @linkPositions = ( "U00", "U01", "U02", "U03", "U04", "U05" );
    
    my @rows = @_;
  
    if (defined($size)) {
        print "Gathering the IO information for a <$size> system\n";
    } else {
        print "Adding drawer $rows[0]\n";
    }

    my $rackAlreadyAdded     = undef;
    # Do something to each row
    foreach my $row (@rows)
    {
        # remove newlines from the row
        $row =~ s/\n//;
        
        my @addStmts = ();
    
        
        if ((length($row) == 6) && (substr($row, 3, 2) eq '-I')) {      
            
            if (substr($row,0,1) eq 'Q') {  # is this from an IO rack
                
                my $loc = substr($row,0,3);
                
                # add the IO rack if starts with Q and not already added
                if (defined($rackAlreadyAdded) &&  $rackAlreadyAdded =~ m/$loc/) 
                {
                }
                else 
                {
                    my $stmtHandle = getDbHandle()->prepare("SELECT count(*) from TBGQIORack where location='$loc'");
                    $stmtHandle->execute();
                    my @result = $stmtHandle->fetchrow_array();
                    if ( !@result || $result[0] == 0 ) {
                        print "Adding IO rack:  $loc \n";
                        $rackAlreadyAdded .= "$loc";
                        push(@addStmts,"INSERT INTO TBGQIORack (productid, machineserialnumber, location) values('MP000000','BGQ','$loc')");

                        my $hardware = `cat $bgPropertiesFile | awk '/HardwareToManage/ {printf "%s",\$3}'`;
                        $hardware .=  "," . $loc;
                        `sed -i 's/HardwareToManage.*=.*/HardwareToManage = $hardware/' $bgPropertiesFile`;
                        print "added $loc to HardwareToManage in $bgPropertiesFile\n";

                        foreach my $pm (@powerModulePositions)
                        {
                            push(@addStmts,"INSERT INTO TBGQBulkPowerSupply (productid, location, status) values('74Y5253', '$loc-B-$pm', 'A')");
                        } #  foreach my $pm (@powerModulePositions)
                    } else {
                        print "IO rack $loc already exists\n";
                    }
                }
            }

            my $iconip = "10.5.5.5";

            push(@addStmts,"INSERT INTO TBGQIODrawer (productid, location) values('MP000000','$row')");
            push(@addStmts,"INSERT INTO TBGQIcon (licenseplate, containerlocation, ipaddress)  values(x'FFF29F151EF1000D60EAE10E','$row', '$iconip' )");

        
            foreach my $nc (@nodeCardPositions)
            {
                push(@addStmts,"INSERT INTO TBGQIONode (productid, iopos, position) values('I0000000','$row', '$nc')");
                my $ip = &nextIpAddress();
                push(@addStmts,"INSERT INTO TBGQNetConfig (location, interface, itemname, itemvalue) values('$row' || '-' || '$nc', 'external1', 'ipv4address', '$ip')");
                push(@addStmts,"INSERT INTO TBGQNetConfig (location, interface, itemname, itemvalue) values('$row' || '-' || '$nc', 'external1', 'name', '$interface')");
            } # foreach my $nc (@nodeCardPositions)

            foreach my $link (@linkPositions)
            {
                push(@addStmts, "INSERT INTO TBGQIOLinkChip (productid, iopos,position) values('50Y7928', '$row','$link')");
            } #  foreach my $link (@linkPositions)

        } else {
            my @ioCables = split(/\,/, $row);
            if (scalar(@ioCables) == 2) {              
                # insert cable
                my $addStmt = "INSERT INTO TBGQCable \(fromlocation,tolocation) values\('$ioCables[0]', '$ioCables[1]'\)";
                processSQL($addStmt);   
            }     
        }
      	foreach my $sql (@addStmts)
	{
            processSQL($sql);
	}
    } 
} # sub processIO


##########################################
# processSQL()
# Deals with SQL commands and provides a common interface
##########################################
sub processSQL
{
    my $stmt = shift();
    print "\n" . $stmt . "\n" if($debug);
    my $stmtHandle = getDbHandle()->prepare($stmt);
    if (!$stmtHandle->execute())
    {
	if ($optProceed) 
	{
	    print "\n" . $stmt . "\n";
	    print "FAILED... proceeding  \n";
	    print "Result is " . getDbHandle()->errstr . "\n";
	}
	else
	{
	    print "Result is " . getDbHandle()->errstr . ". EXITING\n";
	    die "Could not execute $stmt";
	}
    } # if (!$schemaStmtHandle->execute())
}


##########################################
# printUsage()
# Prints the help text for this program
##########################################
sub printUsage()
{
    system("perldoc $0");
    exit(0);
} # sub printUsage()


##########################################
# setupDb()
# Make connection to the DB
##########################################
sub setupDb
{
    my $bgPropertiesFile = shift();
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

    #open the properties file
    if (defined($bgPropertiesFile))
    {
        $propHash = parseProperties($bgPropertiesFile, $propHash);
    }

    #check if we have a database name and schema
    die "Please specify at least --database and --schema, or --properties\n"
        unless defined $propHash->{"name"} and defined $propHash->{"schema_name"};
}

##########################################
# worksheet()
# Twig helper for parsing each worksheet
##########################################
sub worksheet
{
    my @worksheet = @_;

    $worksheet[0] =~ /<(.*)>/;
    my $wsName = $1;
   
    # Dump out the name of the current worksheet
    #    print "Looking at: <$wsName>\n";

    # Ignore if we're not dealing with this size
    return if ($wsName !~ /$size/i);
   
    if ($wsName =~ /(\d+[xX]\d+ )([ABCD])$/)
    {
        print "$wsName worksheet\n" if ($debug);
        print "Gathering the $2 link information for a <$size> system\n";
        processLinks(@worksheet);
        $doneLink = 1;
    }
    elsif ($wsName =~ /(((\d+[xX]\d+)|mid) K $drawers)/)
    {
        print "$wsName worksheet\n" if ($debug);
        processClocks(@worksheet);
        $doneClock = 1;
    }
    elsif ($wsName =~ /(((\d+[xX]\d+)|mid) IO $drawers)/)
    {
        print "$wsName worksheet\n" if ($debug);
        processIO(@worksheet);
        $doneIO = 1;
    }
}               
##########################################
# main()
##########################################


# Define local variables
               my $configDir        = dirname($0) . "/configs";
               my $optHelp          = undef;
               my $ipaddrparm       = "172.16.100.0";
               $bgPropertiesFile = $ENV{BG_PROPERTIES_FILE};

# Parse the command line
               GetOptions(
                          'input=s'        => \$configDir,
                          'size=s'         => \$size,
                          'drawers=i'      => \$drawers,
                          'drawer=s'       => \$insertdrawer,
                          'properties=s'   => \$bgPropertiesFile,
                          'help'           => \$optHelp,
                          'ipaddress=s'    => \$ipaddrparm,  
                          'interface=s'    => \$interface,  
                          'proceed'        => \$optProceed,
                          'debug'          => \$debug
                          )
               or printUsage();

               printUsage() if ($optHelp);

               if (!defined($bgPropertiesFile)) {
                  $bgPropertiesFile = "/bgsys/local/etc/bg.properties";
               }
                
               # ensure we can write to bg.properties
               die "Cannot write to $bgPropertiesFile" unless ( -w $bgPropertiesFile );

               if ($ipaddrparm)
               {
                  my $ip = inet_atoi($ipaddrparm);
                  $ip = $ip - 1;
                  $ipAddress = inet_itoa($ip);
               }
             
               setupDb($bgPropertiesFile);

               if (defined($insertdrawer) && $insertdrawer =~ /R[[:alnum:]]{2}-I[CDEF]/ ) {
                   # top hat drawer
                   processIO( $insertdrawer );
                   exit(0);
               } elsif (defined($insertdrawer) && $insertdrawer =~ /Q[[:alnum:]]{2}-I[[:xdigit:]]/ ) {
                   # I/O rack drawer
                   processIO( $insertdrawer );
                   exit(0);
               } elsif (defined($insertdrawer)) {
                   printUsage();
               } elsif ($drawers != 0 && $drawers != 1 && $drawers != 2 && $drawers != 4 ) {
                   printUsage();
               }

               if (!defined($size))
               {
                   if (!defined($propHash->{"rows"}) || !defined($propHash->{"columns"}))
                   {
                       print "please give --size\n";
                       print "or\n";
                       print "a --properties file with computeRackRows and computeRackColumns in the database section\n";
                       printUsage();
                    }
                    else
                    {
                        $size = "$propHash->{'rows'}x$propHash->{'columns'}";
                        print "got size $size from $bgPropertiesFile\n" if ($debug);
                    }
               }
               else
               {
                   print "using --size $size\n" if ($debug);
               }

               if ($size !~ /\d[xX]\d|mid/)
               {
                   print "--size format incorrect expected something like \"2x3\"\n\n";
                   printUsage();
               } # if ($size !~ /\d[xX]\d(-[IiUu]\d)*/)
               
               # open input file
               my $inFile = $configDir . "/" . $size . ".txt";
               print "opening $inFile\n";
               open(INFILE, "< $inFile") || die "Unable to open <$inFile> for reading";

               createInitialEntries();
                
               print "Building a config of size <$size>\n";
               
# some fancy while loop here that grinds on each "section"
               my @slurper = ();
               while(my $line = <INFILE>)
               {
                   #we're at the next section
                   if($line =~ /<.*>/)
                   {
                       
                       #        print $line;
                       #do whatever you do when you reach the end of a section
                       if($#slurper >= 1)
                       {
                           worksheet(@slurper);
                       }
                       @slurper = ();
                   }
                   push(@slurper, $line);
               }
               
# Since worksheet() will be where the program exits on a good path we need to let the
#  user know what went wrong;
# Note that worksheet() is really like the main program since parsefile() will call it and
#  does not have a known facility for breaking out prematurely.  Thus any DB cleanup or
#  other things desired in a graceful exit should occur at the end of worksheet().
               if (!$doneLink && $size ne "mid" )
               {
                   print "Wasn't able to process torus link info for this config\n";
                   exit(255);
               } # if (!$doneLink)
               
               if (!$doneClock)
               {
                   print "Wasn't able to process clock card info for this config\n";
                   exit(255);
               } # if (!$doneClock)
               
               if (!$doneIO)
               {
                   print "Wasn't able to process IO card info for this config\n";
                   exit(255);
               } # if (!$doneIO)
              

               # open properties file so we can update the HardwareToManage key in
               # the machinecontroller.subnet.0 section. This key requires the compute
               # rack locations in a comma separated list, which we can obtain from 
               # the midplane array that was already populated.
               my $prop = new Config::IniFiles( -file => $bgPropertiesFile);
               my %racks;
               foreach my $mp (@bgqMidplanes)
               {
                   my $rack = substr($mp,0,3);
                   $racks{$rack} = undef
               }

               my $hardware = undef;
               foreach my $rack (keys %racks) {
                   if (!defined($hardware)) {
                       $hardware .= $rack;
                   } else {
                      $hardware .= "," . $rack;
                   }
               }

               # update properties with the correct list of hardware to manage
               # this assumes the hardware is managed by a single subnet, multiple subnets
               # will need to be manually updated.
               print "adding '$hardware' to HardwareToManage in $bgPropertiesFile\n";
               `sed -i 's/^HardwareToManage.*=.*/HardwareToManage = $hardware/' $bgPropertiesFile`;
               if ( $? ) {
                   die "sed failed with $?";
               }

               exit(0);
               
__END__
               
=head1 NAME

dbPopulate.pl (utility to populate the DB/2 database with Blue Gene machine configuration data)

=head1 SYNOPSIS

dbPopulate.pl [options]

Options:

=over

--input [path to configs directory ] | Specifies path where configuration files are found.

--properties [properties-file-name] | properties file, used to obtain database name, schema, username, and
password.  Also used to obtain machine size if --size is not given. Your uid will need permission to 
write to this file since it will be updated with the rack locations to manage.

--size [XxY] | Size of system to build. Either "number of rows" x "number of racks per row" or mid for a half
rack system.

--drawers [0,1,2, or 4] | Number of I/O drawers in each compute rack, default is 1. Note that 0 drawers imply
external I/O racks, which are not available for every machine configuration.

--drawer [location] | I/O drawer location to add to an already populated schema.

--ipaddress [ip-addr] | Starting ip address for IO nodes, defaults to 172.16.100.0

--interface [name] | Interface name for the IP address. Ex: ib0 or eth0. Defaults to ib0.

--help | Prints this help text

--proceed | Proceed on errors

--debug | dump extra output

=back

=head1 DESCRIPTION

This utility should be used to populate the BlueGene/Q database records when installing a new machine.  It will rely on the configuration file to determine how to fill out the rows.  For input, it expects config files generated with cable_spreadsheet_parser.pl, dimension, and database connection info which can be specified in a properties file.

NOTE: This utility requires the perl DBI module and DB2 database driver. 

NOTE: The BG_PROPERTIES_FILE environment variable will be used if --properties is not supplied. If that 
environment is not specified, it will use /bgsys/local/etc/bg.properties by default. 

=head1 AUTHOR

International Business Machines (IBM)

=cut

