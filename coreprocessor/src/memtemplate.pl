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
# (C) Copyright IBM Corp.  2005, 2011                              
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
#-*- mode: perl;-*-

package template;

use bagotricks;
use Tk;
use snapshot;

sub build
{
    my($CORE, $node) = @_;
    
    &initglobals();
    FINIT("Template");
    FIN();
    
    FIN();
    FCTRL(undef, Label, -text => "Template:", -side => 'left');
    FCTRL("template", Menubutton, -text => "<new template>", -side => 'left', -relief => 'raised', -bd => 2);
    FOUT(-fill => 'x');
    
    FIN();
    FCTRL("node", Entry, -width => 32, -side => 'right');
    FCTRL(undef, Label, -text => "Node:", -side => 'right');
    FOUT(-fill => 'x');
    FIN();
    FCTRL("symbol", Entry, -width => 32, -side => 'right');
    FCTRL(undef, Label, -text => "Base Address:", -side => 'right');
    FOUT(-fill => 'x');
    
    FIN(-relief => 'raised', -bd => 2);
    FCTRL("canvas", Canvas, -width => '5i', -height => '4i', -fill => 'both', -expand => 'yes');
    FOUT(-fill => 'both', -expand => 'yes', -padx => '.25c', -pady => '.25c');
    FIN();
#    FCTRL(undef, Button, -text => "Add Item", -command => sub { &newitem::build(80,(++$LASTY)*32); }, -side => 'left');
    FCTRL(undef, Button, -text => "Apply Changes", -command => [\&change, $CORE], -state => 'disabled', -side => 'left');
    FCTRL(undef, Button, -text => "Refresh", -command => [\&refresh, $CORE], -side => 'left');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();
    FOUT(-fill => 'both', -expand => 'yes');
    
    FGET("canvas")->bind('all', "<ButtonPress-1>" => [\&select]);
    FGET("canvas")->bind('all', "<B1-Motion>" => [\&drag]);
    FGET("canvas")->bind('all', "<ButtonRelease-1>" => [\&release]);
    FGET("canvas")->bind('all', "<Double-Button-1>" => [\&customize]);
    
    FGET("canvas")->createRectangle(0,0,'20i', '20i', -fill => 'wheat3');
    
    SetValue("node", $node);
    SetValue("symbol", "0x00000000");
    &gettemplates();
    FGET("symbol")->focus();
}

sub initglobals
{
    %widgets = ();
    %lastrawvalue = ();
    %ctrl = ();
}

sub gettemplates
{
    my @templates = ();
    $::OPTIONS{"-templates"} = "." if(!exists $::OPTIONS{"-templates"});
    opendir(TMP, $::OPTIONS{"-templates"});
    my @files = readdir(TMP);
    closedir(TMP);
    foreach $file (@files)
    {
	push(@templates, $file) if($file =~ /\.template$/);
    }
    FPOPUP("template", [-tearoff => 0], ["Change Template Directory", "Save Template", "sep", @templates], 'template::chgtemplate');
}

sub change
{
    my($core) = @_;
    foreach $key (keys %ctrl)
    {
	$symbol = GetValue("symbol");
	$addr = $symbol + $ctrl{$key}{"offset"};
	if($ctrl{$key}{"format"} =~ /hexadecimal/i)
	{
	    $str = $widgets{$key}->get();
	}
	elsif($ctrl{$key}{"format"} =~ /decimal/i)
	{
	    $str = $widgets{$key}->get();
	    $str = sprintf("%8.8x", $str);
	}
	setmemory($core, $addr, $ctrl{$key}{"len"}, $str);
    }
    &refresh();
}

sub refresh
{
    my($core) = @_;
    
    foreach $key (keys %ctrl)
    {
	next if($key eq "globals");
	$symbol = GetValue("symbol");
	$addr = hex($symbol) + $ctrl{$key}{"offset"};
        $lastrawvalue{$key} = getmemory($core, $addr, $ctrl{$key}{"len"});
    }
    &drawcanvas();
}

sub setmemory
{
    my($core, $addr, $len, $data) = @_;
}


sub getmemory
{
    my($core, $addr, $len) = @_;
    my $str = "";
    my $node = GetValue("node");
    $core->setaccessmask(1, $node);
    $adjusted_addr = $addr - $addr%4;
    $adjusted_len  = $len  + $addr%4;
    for(; $adjusted_len > 0; $adjusted_addr+=4, $adjusted_len-=4)
    {
	$cmdaddr = sprintf("0x%8.8x", $adjusted_addr);
	$core->touchmemory($cmdaddr);
	$adjusted_data = $core->{$node}{"memory"}{$cmdaddr};
	return "" if($adjusted_data !~ /\S/);
	$str .= $adjusted_data;
    }
    substr($str,0,($addr%4)*2,"");
    substr($str,$len*2, length($str)-$len*2, "");
    $core->setaccessmask(0);
    return $str;
}

sub chgtemplate
{
    my($var, $value) = @_;
    if($value eq "Save Template") 
    {
	my $data = snapshot(%ctrl);
	&savetemplate::build($data); 
    }
    elsif($value eq "Change Template Directory") 
    {
	&changetemplatedir::build();
    }
    else
    {
	initglobals();
	
	SetValue(@_);
	my $frozen = "";
	open(TMP, $::OPTIONS{"-templates"} ."/". $_[1]);
	$frozen .= <TMP>;
	close(TMP);
	chomp($data);
	
	%ctrl = restore($frozen);
	&drawcanvas();
    }
}
sub drawcanvas
{
    FGET("template|canvas")->delete("all");
    %widgets = ();
    
    FGET("canvas")->createRectangle(0,0,'20i', '20i', -fill => 'wheat3');	
    foreach $key (keys %ctrl)
    {
	next if($key eq "globals");
	
	my $fieldlen;
	if($ctrl{$key}{"format"} =~ /^decimal$/i)
	{
	    $fieldlen = length(sprintf("%u", 256**$ctrl{$key}{"len"}-1));
	}
	else
	{
	    $fieldlen = 2*$ctrl{$key}{"len"};
	}
	
	my $w = FGET("template|canvas")->Entry(-width => $fieldlen, -font => 'Courier 12 bold');
	$w->insert(0, 'X' x $fieldlen);
	$widgets{$key} = $w;
	FGET("template|canvas")->createWindow($ctrl{$key}{"x"}, $ctrl{$key}{"y"}, -anchor => 'nw', -window => $w, -tags => ["control", "widget", $key]);
	FGET("template|canvas")->createText($ctrl{$key}{"x"}, $ctrl{$key}{"y"}, -anchor => 'ne', -text => $ctrl{$key}{"label"}, -tags => ["control", $key]);	    
	
	if(exists $lastrawvalue{$key})
	{
	    my $str = $lastrawvalue{$key};
	    if($ctrl{$key}{"format"} =~ /hexadecimal/i)
	    {
		$widgets{$key}->delete(0, 'end');
		$widgets{$key}->insert(0, $str);
	    }
	    elsif($ctrl{$key}{"format"} =~ /decimal/i)
	    {
		$widgets{$key}->delete(0, 'end');
		$widgets{$key}->insert(0, hex($str));
	    }
	}
    }
}

sub select
{
    my($c) = @_;
    my($x, $y) = ($Tk::event->x, $Tk::event->y);
    @ctrls = $c->find('overlapping', $x-8, $y-8, $x+8, $y+8);
    foreach $ctrl (@ctrls)
    {
	foreach $tag ($c->gettags($ctrl))
	{
	    if($tag =~ /INTERNAL_TAG/)
	    {
		$seltag = $tag;
		$ctrl{$tag}{"adjustx"} = $x;
		$ctrl{$tag}{"adjusty"} = $y;
		$c->addtag('selected', 'withtag', $tag);
		return;
	    }
	}
    }

}
sub release
{
    my($c) = @_;
    $c->dtag('all', 'selected');
}

sub drag
{
    my($c) = @_;
    my($x, $y) = ($Tk::event->x, $Tk::event->y);
    $c->move('selected', $x - $ctrl{$seltag}{"adjustx"}, $y-$ctrl{$seltag}{"adjusty"});
    $ctrl{$seltag}{"x"} += $x - $ctrl{$seltag}{"adjustx"};
    $ctrl{$seltag}{"y"} += $y - $ctrl{$seltag}{"adjusty"};
    $ctrl{$seltag}{"adjustx"} = $x;
    $ctrl{$seltag}{"adjusty"} = $y;
}

sub customize
{
    my($c) = @_;
    my($x, $y) = ($Tk::event->x, $Tk::event->y);
    @ctrls = $c->find('overlapping', $x-16, $y-16, $x+16, $y+16);
    foreach $ctrl (@ctrls)
    {
        foreach $tag ($c->gettags($ctrl))
        {
            if($tag =~ /INTERNAL_TAG/)
            {
		&newitem::build($ctrl{$tag}{"x"},$ctrl{$tag}{"y"}, $tag);
		return;
            }
        }
    }
    &newitem::build($x, $y);
}

package newitem;
use Tk;
use bagotricks;

sub build
{
    my($x, $y, $tag) = @_;
    if(!defined $tag)
    {
	FINIT("New Control");
    }
    else
    {
	FINIT("Modify Control");
    }
    FIN();
    FIN();
    FCTRL(undef, Label, -text => "Label: ", -side => 'left');
    FCTRL("label", Entry, -width => 16, -side => 'left');
    FOUT(-fill => 'x');
    
    FIN();
    FCTRL(undef, Label, -text => "Address Offset: ", -side => 'left');
    FCTRL("offset", Entry, -width => 6, -side => 'left');
    FSPACE(-width => '1c', -side => 'left');
    FCTRL(undef, Label, -text => "Bytes: ", -side => 'left');
    FCTRL("len", Entry, -width => 4, -side => 'left');
    FSPACE(-width => '1c', -side => 'left');
    FCTRL("format", Menubutton, -text => "Hexadecimal", -side => 'left', -relief => 'raised', -bd => 2);
    FPOPUP("format", [-tearoff => 0], ["Hexadecimal", "Decimal"]);
    FOUT(-fill => 'x');

    FIN();
    if(defined $tag)
    {
	FCTRL(undef, Button, -text => "Modify", -command => [\&create, $x, $y, $tag], -side => 'left');
	FCTRL(undef, Button, -text => "Delete", -command => [\&delete, $tag], -side => 'left');
    }
    else
    {
	FCTRL(undef, Button, -text => "Create", -command => [\&create, $x, $y, $tag], -side => 'left');
    }
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();
    FOUT();
    
    if(defined $tag)
    {
	SetValue("label", $template::ctrl{$tag}{"label"});
	SetValue("format", $template::ctrl{$tag}{"format"});
	SetValue("offset", $template::ctrl{$tag}{"offset"});
	SetValue("len", $template::ctrl{$tag}{"len"});
    }
    else
    {
	SetValue("label", "variable" . ++$template::ctrl{"globals"}{"TAGID"});
	SetValue("len", 4);
	SetValue("offset", 0);
    }
    FGET("offset")->focus();
}

sub delete
{
    my($tag) = @_;
    delete $template::ctrl{$tag};
    &template::drawcanvas();
    FCLOSE();
}

sub create
{
    my($x, $y, $tag) = @_;
    $tag = "INTERNAL_TAG" . $template::ctrl{"globals"}{"TAGID"} if(!defined $tag);
    $template::ctrl{$tag}{"x"} = $x;
    $template::ctrl{$tag}{"y"} = $y;    
    $template::ctrl{$tag}{"widget"} = 1;
    $template::ctrl{$tag}{"label"} = GetValue("label");
    $template::ctrl{$tag}{"offset"} = GetValue("offset");
    $template::ctrl{$tag}{"len"} = GetValue("len");
    $template::ctrl{$tag}{"format"} = GetValue("format");
    &template::drawcanvas();
    FCLOSE();
}

package savetemplate;
use bagotricks;
use Tk;

sub build
{
    my($data) = @_;
    FINIT("Save Memory Template");
    FIN();
    FIN();
    FCTRL(undef, Label, -text => "Template Directory/Name: ", -side => 'left');
    FCTRL("path", Entry, -width => 60, -side => 'left');
    FCTRL("name", Entry, -width => 20, -side => 'left');
    FCTRL(undef, Label, -text => ".template", -side => 'left');
    FOUT(-fill => 'x');
    FIN();
    FCTRL(undef, Button, -text => "Save", -side => 'left', -command => [\&save, $data]);
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => sub {FCLOSE();});
    FOUT(-fill => 'x');
    FOUT();
    
    SetValue("path", $::OPTIONS{"-templates"});
    SetValue("name", "mytemplate");
}
sub save
{
    my($data) = @_;
    $path = GetValue("path");
    $name = GetValue("name");
    $::OPTIONS{"-templates"} = $path;
    $file = "$path/$name.template";
    open(TMP, ">$file");
    print TMP $data;
    close(TMP);
    FCLOSE();
}

package changetemplatedir;
use Tk;
use bagotricks;

sub build
{
    FINIT("Change Template Directory");
    FIN();
    FCTRL(undef, Label, -text => "Path:", -side => 'left');
    FCTRL("path", Entry, -width => 60, -side => 'left');
    FOUT(-fill => 'x');
    FIN();
    FCTRL(undef, Button, -text => "ChangeDir", -side => 'left', -command => [\&changedir]);
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => sub { FCLOSE(); });
    FOUT();
    FGET("path")->focus();
}
sub changedir
{
    $::OPTIONS{"-templates"} = GetValue("path");
    FCLOSE();
    &template::gettemplates();
}

1;
