# Perl module for loading schedules into perl arrays and hashes.

=head1 COPYRIGHT

(C)Copyright IBM Corp.  2009, 2010
IBM CPL License

=cut

# A set of subroutines to help with processing schedules dumped
# from "sched_test -V".

use Data::Dumper;
sub dump_sched($) {
	my $ref = shift;
	print Dumper($ref);
}

# Return the total number of ranks in schedule info
sub get_nranks() {
	return scalar(@{$SCHED});
}

# Return a reference to the hash for rank index <n>
sub get_rank($) {
	my $n = shift;
	return ${$SCHED}[$n];
}

# Return a reference to the hash for rank id <r>
sub get_rank_rank($) {
	my $r = shift;
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $rh = get_rank($n);
		if ($rh->{rank} == $r) {
			return $rh;
		}
	}
	return undef;
}

# Return a index to the hash for rank id <r>
sub get_index_rank($) {
	my $r = shift;
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $rh = get_rank($n);
		if ($rh->{rank} == $r) {
			return $n;
		}
	}
	return undef;
}

# Returns the number of phases for rank defined in <ref>
sub get_nphases($) {
	my $ref = shift;
	return scalar(@{$ref->{phases}});
}

# Returns a reference to the array for phase <ph> of rank defined
# by (hash) <ref>.
sub get_phase($$) {
	my $ref = shift;
	my $ph = shift;
	return ${$ref->{phases}}[$ph];
}

sub get_nsteps($) {
	my $ref = shift;
	return scalar(@{$ref});
}

sub get_step($$) {
	my $ref = shift;
	my $st = shift;
	return ${$ref}[$st];
}

sub rank2torus($$$$$) {
	my $rank = shift;
	my $xref = shift;
	my $yref = shift;
	my $zref = shift;
	my $tref = shift;
	my $ref = get_rank_rank($rank);
	${$xref} = $ref->{x};
	${$yref} = $ref->{y};
	${$zref} = $ref->{z};
	${$tref} = $ref->{t};
}

# Now, this gets ugly...
sub torus2rank($$$$) {
	my $x = shift;
	my $y = shift;
	my $z = shift;
	my $t = shift;
	for (my $r = 0; $r < get_nranks(); ++$r) {
		my $ref = get_rank($r);
		if ($x == $ref->{x} &&
		    $y == $ref->{y} &&
		    $z == $ref->{z} &&
		    $t == $ref->{t}) {
			return $ref->{rank};
		}
	}
	return undef;
}

# Returns sets of nodes (ref to array if indices) sharing x,y,z
# (i.e. peer cores), excluding self
sub get_peers($) {
	my $rank = shift;
	my $peers = [];
	my ($x1,$y1,$z1);
	my ($x2,$y2,$z2);
	my $t;
	rank2torus($rank, \$x1, \$y1, \$z1, \$t);
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $ref = get_rank($n);
		if ($ref->{rank} == $rank) { next; }
		rank2torus($ref->{rank}, \$x2, \$y2, \$z2, \$t);
		if ($x1 == $x2 && $y1 == $y2 && $z1 == $z2) {
			push(@{$peers}, $n);
		}
	}
	return $peers;
}

# Returns an array ref containing all dest nodes affected by step.
sub get_nodes_sent($) {
	my $step_ref = shift;	# Sender
	my $rank = $step_ref->{rank};
	my $nodes = [];
	if (defined($step_ref->{dst})) {
		if ($step_ref->{op} =~ m/^LINE_BCAST_([XYZ])([PM])/) {
			my $expr = "\$".$1.";";
			if ($2 eq "P") { $expr = "++".$expr; }
			else { $expr = "--".$expr; }
			my ($X, $Y, $Z, $T);
			rank2torus($rank, \$X, \$Y, \$Z, \$T);
			while ($rank != $step_ref->{dst}) {
				eval $expr;
				$rank = torus2rank($X, $Y, $Z, $T);
				push(@{$nodes}, $rank);
			}
		} elsif ($step_ref->{op} =~ m/^TREE_/) {
			# exclulde TREE ops
		} else {
			push(@{$nodes}, $step_ref->{dst});
		}
	}
	return $nodes;
}

%send2recv = (
	'TREE_ALLREDUCE' => [ 'REDUCE_RECV_STORE', 'REDUCE_RECV_NOSTORE' ],
	'TREE_BCAST' => [ 'BCAST_RECV_STORE', 'BCAST_RECV_NOSTORE' ],
);

# Look for a RECV in <dst> steps of phase <ph>, from <src>.
# Returns the number of such receives found (should be only 1).
# Looks in all phases >= $ph.
# Returns ref to array of refs to receives.
sub chk_recv($$) {
	my $ph = shift;
	my $sth = shift;	# Sender step
	my $recvs = [];
	if (exists($send2recv{$sth->{op}})) {
		# Tree op
		my $nref = get_rank_rank($sth->{rank});
		my $pref = get_phase($nref, $ph);
		my $tops = $send2recv{$sth->{op}};
		for (my $t = 0; $t < get_nsteps($pref); ++$t) {
			my $th = get_step($pref, $t);
			foreach my $top (@{$tops}) {
				if ($th->{op} eq $top) {
					push(@{$recvs}, $th);
				}
			}
		}
	} elsif ($sth->{op} =~ m/_BARRIER/) {
		push(@{$recvs}, $sth);
	} else {
		my $srcs = get_nodes_sent($sth);
		foreach my $s (@{$srcs}) {
			my $nref = get_rank_rank($s);
			my $nph = get_nphases($nref);
			for (my $p = $ph; $p < $nph; ++$p) {
				my $pref = get_phase($nref, $p);
				if (!defined($pref)) { next; }
				for (my $t = 0; $t < get_nsteps($pref); ++$t) {
					my $th = get_step($pref, $t);	# Recver
					if (defined($th->{src})) {
						if ($th->{src} == $sth->{rank}) {
							push(@{$recvs}, $th);
						}
					}
				}
			}
		}
	}
	return $recvs;
}

%recv2send = (
	'REDUCE_RECV_STORE' => 'TREE_ALLREDUCE',
	'REDUCE_RECV_NOSTORE' => 'TREE_ALLREDUCE',
	'BCAST_RECV_STORE' => 'TREE_BCAST',
	'BCAST_RECV_NOSTORE' => 'TREE_BCAST',
);

# Look for a SEND in <src> steps of phase <ph>, to <dst>.
# Returns the number of such sends found (should be only 1).
# Look in all phases <= $ph
# Returns ref to array of refs to sends.
sub chk_send($$) {
	my $ph = shift;
	my $dth = shift;
	my $sends = [];
	if (exists($recv2send{$dth->{op}})) {
		# TREE op
		my $nref = get_rank_rank($dth->{rank});
		my $pref = get_phase($nref, $ph);
		my $top = $recv2send{$dth->{op}};
		for (my $t = 0; $t < get_nsteps($pref); ++$t) {
			my $th = get_step($pref, $t);
			if ($th->{op} eq $top) {
				push(@{$sends}, $th);
			}
		}
	} else {
		my $nref = get_rank_rank($dth->{src});
		for (my $p = $ph; $p >= 0; --$p) {
			my $pref = get_phase($nref, $p);
			if (!defined($pref)) { next; }
			for (my $t = 0; $t < get_nsteps($pref); ++$t) {
				my $th = get_step($pref, $t);
				my $dsts = get_nodes_sent($th);
				foreach my $d (@{$dsts}) {
					if ($d == $dth->{rank}) {
						push(@{$sends}, $th);
					}
				}
			}
		}
	}
	return $sends;
}

sub _num {
	return ($a <=> $b);
}

sub chk_node_data($$) {
	my $node = shift;
	my $data = shift;
	my $nr = get_nranks();
	my @list = sort {$a <=> $b} split(/\,/, $data);
	my $c = 0;
	my $d = 0;
	my $e = -1;
	my $m = 0;
	for (my $x = 0; $x < scalar(@list); ++$x) {
		if ($list[$x] == $e) {
			++$d;	# duplicate
			next;
		}
		++$e;
		if ($list[$x] == $e) {
			++$c;	# correct value
			next;
		}
		while ($list[$x] > $e) {
			++$e;
			++$m;	# missed
		}
	}
	if ($OP eq "Barrier") {
		# This is sort of a cheat. We don't really want duplicate receives
		# but some barrier scheds do not lend themselves to detecting a
		# COMBINE_SUBTASK vs. others, so we get duplicate data.
		if ($c != $nr) {
			my $nh = get_rank($node);
			print STDERR "${_title}: Data error for node $node (rank ".$nh->{rank}.")\n";
			return 1;
		}
	} else {
		if ($d > 0 || $m > 0 || $c != $nr) {
			my $nh = get_rank($node);
			print STDERR "${_title}: Data error for node $node (rank ".$nh->{rank}.")\n";
			return 1;
		}
	}
	return 0;
}

sub chk_all_data($) {
	my $ref = shift;
	my $ret = 0;
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $r = chk_node_data($n, ${$ref}[$n]);
		if ($r != 0) {
			$ret = 1;
		}
	}
	return $ret;
}

sub chk_node_bcast($$$) {
	my $root = shift;
	my $node = shift;
	my $data = shift;
	my $c = 0;
	my $e = 0;
	foreach my $x (split(/\,/, $data)) {
		if ($x == $root) { ++$c; }
		else { ++$e; }
	}
	if ($c > 1) {
		print STDERR "${_title}: Warning: node $node received multiple copies of $root\n";
	} elsif ($c < 1) {
		print STDERR "${_title}: Node $node received no copies of $root\n";
	} elsif ($e > 0) {
		print STDERR "${_title}: Node $node received other nodes data\n";
	}
	return ($c == 1 && $e == 0);
}

sub chk_all_bcast($) {
	my $root = shift;
	my $ref = shift;
	my $ret = 0;
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $r = chk_node_bcast($root, $n, ${$ref}[$n]);
		if (!$r) {
			$ret = 1;
		}
	}
	return $ret;
}

# Load schedule from stdin...
sub sched_load {
	my $file = shift;
	my $fh;
	if (!defined($file)) {
		$fh = STDIN;
	} else {
		$fh = FileHandle($file, "r");
	}
	my @text = (<$fh>);
	my $ret = eval join("\n",@text);
	if (length($@)) {
		print STDERR "sched_test output failed to compile: $@";
		exit 1;
	}
	if (defined($TITLE)) {
		$_title = ${TITLE};
	} else {
		$_title = "${NUM_RANKS}-node ${ALGORITHM} ${OP}";
	}
}
1;
