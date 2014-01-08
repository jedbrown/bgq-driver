#!/usr/bin/perl -i

=head1 COPYRIGHT

(C)Copyright IBM Corp.  2007, 2010
IBM CPL License

=cut

use strict;
use warnings;
use File::Basename;
use File::Find;

sub c::filter(@);
sub perl::filter(@);
sub make::filter(@);

undef $/;

my (@c_files, @perl_files, @make_files) = ();
sub wanted() {
    return if (
        ($File::Find::name =~ m/CVS/) or
        ($File::Find::name =~ m@\.git/@) or
        ($File::Find::name =~ m@sys/(?:reference|unused)/@)
        );

# Don't try to parse directories or links
    if (-d $_ or -l $_) {
    } elsif (-f $_) {
        if (m@\.(?:c|cc|h)$@) {
            push(@c_files,    $File::Find::name);
        } elsif (m@\.p[lm]$@) {
            push(@perl_files, $File::Find::name);
        } elsif (m@^(?:configure.in|Build.rules.in|Make.rules.in|GNUmakefile.in)$@) {
            push(@make_files, $File::Find::name);
        }
    } else {
#        warn "$File::Find::name (skipped)\n";
    }
}
find(\&wanted, @ARGV);
die "No files listed\n" unless (scalar(@c_files) + scalar(@perl_files) + scalar(@make_files));

@ARGV=();
#warn join "\n", "-"x33, @c_files, "-"x33, @perl_files, "-"x33, @make_files, '';
c::filter   (@c_files   );
perl::filter(@perl_files);
make::filter(@make_files);






package make;


sub init() {

our $old_copyright_1 = qr"\Q#/*********************************************************************/
#/*                     I.B.M. CONFIDENTIAL                           */
#/*      (c) COPYRIGHT IBM CORP. \E200\d(?: -|,) 200\d\Q ALL RIGHTS RESERVED.     */
#/*                                                                   */
#/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
#/*                                                                   */
#/*********************************************************************/
\E";
our $old_copyright_2 = qr"\Q#/*********************************************************************/
#/*                        I.B.M. CONFIDENTIAL                        */
#/*      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      */
#/*                                                                   */
#/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
#/*                                                                   */
#/*********************************************************************/
\E";

our $small_copyright = qr"\QI.B.M. CONFIDENTIAL\E";
our $new_copyright_core = 'begin_generated_IBM_copyright_prolog                             #
#                                                                  #
#  --------------------------------------------------------------- #
#                                                                  #
# (C) Copyright IBM Corp.  2009, 2010                              #
# IBM CPL License                                                  #
#                                                                  #
#  --------------------------------------------------------------- #
#                                                                  #
# end_generated_IBM_copyright_prolog';
our $new_copyright = "# $new_copyright_core                               #
";


}


sub core() {
    #get the name of the current file
    my $filename = $ARGV;
    $filename =~ s,^\./,,;

    #remove leading/trailing white space
    s@^\n+@@;
    s@\n+$@\n@;

    #remove the old copyright notice
    s@$make::old_copyright_1@@g;
    s@$make::old_copyright_2@@g;
    if (m/$make::small_copyright/) {
        warn sprintf("%-60s: Didn't remove old copyright, but found part of it.\n", $ARGV);
    }

    #find old versions of the copyright prolog
    s{begin_generated_IBM_copyright_prolog.*end_generated_IBM_copyright_prolog}
     {$make::new_copyright_core}s;

    #find the new copyright notice
    unless (m/end_generated_IBM_copyright_prolog/){
        s/^/$make::new_copyright/;
    }

    #Add a blank space after the new copyright notice
    s/(end_generated_IBM_copyright_prolog.*?)\n+/$1\n\n/;
}


sub filter(@) {
    return unless scalar @_;
    init();
    local @main::ARGV = @_;
    while(<>) {
#        warn "$ARGV\n";
        core();
        print;
    }
}




package perl;


sub init() {

}


sub core() {

}


sub filter(@) {
    return unless scalar @_;
    init();
    local @main::ARGV = @_;
    while(<>) {
#        warn "$ARGV\n";
        core();
        print;
    }
}




package c;


sub init() {

our $old_copyright_1 = qr"\Q/*********************************************************************/
/*                     I.B.M. CONFIDENTIAL                           */
/*      (c) COPYRIGHT IBM CORP. \E200\d(?: -|,) 200\d\Q ALL RIGHTS RESERVED.     */
/*                                                                   */
/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
/*                                                                   */
/*********************************************************************/
\E";
our $old_copyright_2 = qr"\Q/*********************************************************************/
/*                        I.B.M. CONFIDENTIAL                        */
/*      (c) COPYRIGHT IBM CORP. 2001, 2003 ALL RIGHTS RESERVED.      */
/*                                                                   */
/*                LICENSED MATERIALS-PROPERTY OF IBM.                */
/*                                                                   */
/*********************************************************************/
\E";

our $small_copyright = qr"\QI.B.M. CONFIDENTIAL\E";
our $end_copyright = "/* end_generated_IBM_copyright_prolog                               */
";
our $end_copyright_x = qr"\Q$end_copyright\E";

our $new_copyright_core = 'begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* ---------------------------------------------------------------- */
/* (C)Copyright IBM Corp.  2009, 2010                               */
/* IBM CPL License                                                  */
/* ---------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog';
our $new_copyright = "/* $new_copyright_core                               */
";

}


sub core() {
    #get the name of the current file
    my $filename = $ARGV;
    $filename =~ s,^\./,,;

    #remove white space at the beginning/end of the file
    s@^\n+@@;
    s@\n*$@\n@; # This actually adds a single LF iff there wasn't one.

    #remove the old copyright notice
    s@$c::old_copyright_1@@g;
    s@$c::old_copyright_2@@g;
    if (m/$c::small_copyright/) {
        warn sprintf("%-60s: Didn't remove old copyright, but found part of it.\n", $ARGV);
    }

    #find old versions of the copyright prolog
    s{begin_generated_IBM_copyright_prolog.*end_generated_IBM_copyright_prolog}
     {$c::new_copyright_core}s;

#At this time, we are not automatically adding the CPL
    #find the new copyright notice
    # unless (m/end_generated_IBM_copyright_prolog/){
    #     s/^/$c::new_copyright/;
    # }

    #Attempt to change the \file tag
    unless (s/\\file(.*)/\\file $filename/g) {
        #If it wasn't found, add the basic doxygen comment
        unless (s@$c::end_copyright_x\n*@${c::end_copyright}/**\n * \\file $filename\n * \\brief ???\n */\n\n@) {
            warn sprintf("%-60s: Didn't add \\file (didn't find good copyright).\n", $ARGV);
        }
    }

    #change javadoc comments to C-style doxygen
    s@/\*!\s*$@/**@gm;
    s@/\*!@/**@g;
    s@/\*!<@/**<@g;

    #I also don't care for CRs
    s/\r//g;

    #I HATE trailing space on lines
    s/[ \t]+$//gm;;

    if ($ARGV =~ m/\.h$/) {
        my $exclude = $ARGV;
        $exclude =~ s@^\./@@g;
        $exclude =~ s@^sys/@@g;
        $exclude =~ tr@_0-9A-Za-z@_@cs;
        $exclude = "__${exclude}__";
        if (s/#ifndef\s+(\w+)\s*\n#define\s+\1\s*?\n/#ifndef $exclude\n#define $exclude\n/m) {
        } else {
            warn sprintf("%-60s: Didn't find #ifndef/#define pair to prevent recursive header inclusion.\n", $ARGV);
        }
    }

return;

    #tabs suck
    1 while s/^( *)\t/$1        /gm;

return;

    #crap in doxygen comments:
    s@^ \* \*{5,}\n(?= +\* \\brief )@@gm;
    s@^( +\* \\brief .*\n) \* \*{5,}\n@$1@gm;
    if (m/\Q*****\E/m) {
        warn sprintf("%-60s: File still has ***** comments\n", $ARGV);
    }
}


sub filter(@) {
    return unless scalar @_;
    init();
    local @main::ARGV = @_;
    while(<>) {
#        warn "$ARGV\n";
        core();
        print;
    }
}
