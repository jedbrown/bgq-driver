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
# Produce a digraph for use with dot.

use Getopt::Long;

require sched;

Getopt::Long::Configure("no_ignore_case", "bundling");
GetOptions("A" => \$ext,
	"allreduce_digraph" => \$ext,
	"v" => \$recv,
	"recv" => \$recv);

sched_load();

#dump_sched(\@SCHED);

my @colors = (
"red", "blue", "green", "yellow", "purple",
"cyan", "gray", "orange", "magenta4", "brown",
"red4", "blue4", "green4", "yellow4", "violet",
"cyan4", "black", "orange4", "lightblue", "gold"
	);

print "digraph \"${_title}\" {\n";
if ($recv) { $tag = "receives"; } else { $tag = "sends"; }
print "label=\"${_title} ($tag)\";\n";

for (my $n = 0; $n < get_nranks(); ++$n) {
	my $rh = get_rank($n);
	for (my $p = 0; $p < get_nphases($rh); ++$p) {
		my $color = "magenta";
		if ($p < @colors) { $color = $colors[$p]; }
		my $pref = get_phase($rh, $p);
		if (!defined($pref)) {
			print "Undefined phase: $p\n";
			next;
		}
		for (my $t = 0; $t < get_nsteps($pref); ++$t) {
			my $th = get_step($pref, $t);
			if (!defined($th)) {
				# Error?
				next;
			}
			if (defined($th->{dst})) {
				# send
				if (!$recv) {
					my $dsts = get_nodes_sent($th);
					print "$n";
					foreach my $d (@{$dsts}) {
						print " -> ".$d;
					}
					print " [ label=$p,color=\"".
						$color."\" ];\n";
				}
			} elsif (defined($th->{src})) {
				# recv
				if ($recv) {
					print $th->{src}." -> $n".
						" [ label=$p,color=\"".
						$color."\" ];\n";
				}
			} else {
				# bogus?
				print STDERR "Unrecognized step: $p $n ...\n";
			}
		}
	}
}

if ($ext) {
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $rh = get_rank($n);
		my @ranks = ($n);
		my $tag = "[$n]";
		print "subgraph \"$tag\" {\n";
		for (my $p = get_nphases($rh) - 1; $p >= 0; --$p) {
			my $color = "magenta";
			if ($p < @colors) { $color = $colors[$p]; }

			my $i = scalar(@ranks);
			for (my $tx = 0; $tx < $i; ++$tx) {
				my $th = get_rank($ranks[$tx]);
				my $pref = get_phase($th, $p);
				if (!defined($pref)) {
					print STDERR "Undefined phase: $p\n";
					next;
				}
				for (my $t = 0; $t < get_nsteps($pref); ++$t) {
					my $th = get_step($pref, $t);
					if (!defined($th)) {
						# Error?
						next;
					}
					if (defined($th->{src})) {
						# recv
						my $s = $tag.$th->{src};
						my $d = $tag.$ranks[$tx];
						print "\"$s\" -> \"$d\"".
							" [ label=$p,color=\"".
							$color."\" ];\n";
						push(@ranks, $th->{src});
					}
				}
			}
		}
		print "};\n";
	}
}

print "};\n";
