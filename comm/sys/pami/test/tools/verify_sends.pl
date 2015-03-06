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

=head1 COPYRIGHT

(C)Copyright IBM Corp.  2009, 2010
IBM CPL License

=cut

$_name = "Sends/Recvs";	# What sort of test am I?

sub dump_steps {
	my $s = "";
	foreach my $h (@_) {
		$s .= " ".$h->{rank}."[".$h->{phase}."]:";
		if (defined($h->{dst})) {
			$s .= "(".$h->{dst}.",".$h->{op}.")";
		} else {
			$s .= "<".$h->{src}.",".$h->{op}.">";
		}
	}
	$s = substr($s, 1);
	return $s;
}

use Getopt::Long;
require sched;

Getopt::Long::Configure("no_ignore_case", "bundling");
GetOptions("v" => \$verb,
	"verbose" => \$verb,
	"p" => \$pos,
	"positive" => \$pos);

sched_load();

#dump_sched(\@SCHED);

if (scalar(@{$SCHED}) == 0) {
	print STDERR "${_title} ${_name}: Empty schedule\n";
	exit 1;
}

my $ret = 0;
for (my $n = 0; $n < get_nranks(); ++$n) {
	my $rh = get_rank($n);
	for (my $p = 0; $p < get_nphases($rh); ++$p) {
		my $pref = get_phase($rh, $p);
		if (!defined($pref)) {
			print "Undefined phase: $p\n";
			# Internal Error?
			$ret = 1;
			next;
		}
		for (my $t = 0; $t < get_nsteps($pref); ++$t) {
			my $th = get_step($pref, $t);
			if (!defined($th)) {
				# Internal Error?
				$ret = 1;
				next;
			}
			if (defined($th->{dst})) {
				# send
				my $f = chk_recv($p, $th);
				if ($verb) {
					print dump_steps($th)." matches ".
						dump_steps(@{$f})."\n";
				}
				if (scalar(@{$f}) == 0) {
					print STDERR "${_title}: No matching recv(".
						$th->{dst}.
						") for node ".
						$th->{rank}." phase $p (step $t)\n";
					$ret = 1;
##### This may be some sort of "broadcast" op, in which case we just let the
##### recvs sort it all out.
#				} elsif (scalar(@{$f}) > 1) {
#					print STDERR "${_title}: Too many matching recv(".
#						$th->{dst}.
#						") for node ".
#						$th->{rank}." phase $p (step $t)\n";
#					$ret = 1;
				}
			} elsif (defined($th->{src})) {
				# recv
				my $f = chk_send($p, $th);
				if ($verb) {
					print dump_steps($th)." matches ".
						dump_steps(@{$f})."\n";
				}
				if (scalar(@{$f}) == 0) {
					print STDERR "${_title}: No matching send(".
						$th->{src}.
						") for node ".
						$th->{rank}." phase $p (step $t)\n";
					$ret = 1;
				} elsif (scalar(@{$f}) > 1) {
					print STDERR "${_title}: Too many matching send(".
						$th->{src}.
						") for node ".
						$th->{rank}." phase $p (step $t)\n";
					$ret = 1;
				}
			} else {
				# bogus?
				print STDERR "Unrecognized step: $p $n ...\n";
				$ret = 1;
			}
		}
	}
}
if (($pos || $verb) && $ret == 0) { print "${_title} ${_name}: OK\n"; }
exit $ret;
