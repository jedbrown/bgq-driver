#-*- mode: perl;-*-
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

package map3d;
use Tk;
use bagotricks;
BEGIN
{
    eval
{
    $OPENGL_LIBRARIES_LOADED = 0;
    require OpenGL;
    OpenGL->import(qw(:all));
    $OPENGL_LIBRARIES_LOADED = 1;
}
}

sub build
{
    ($CORE, @nodes) = @_;
    FINIT("3D Map");
    undef $repeatid;
    if($OPENGL_LIBRARIES_LOADED == 0)
    {
	FIN();
	FCTRL("glerror", Text, -width => 60, -height => 15, orient => 'y');
	SetValue("glerror", 'Sorry, the OpenGL module for Perl has not been installed on this machine.  OpenGL is needed for this function.
                 
                 The perl module for OpenGL can be found at www.cpan.org.  This code was tested with this version of OpenGL:
                 http://search.cpan.org/~ilyaz/OpenGL-0.54/
                 
                 (caveat:  the code required a source code change in order to compile.  Delete/comment-out line 82 in glu_const.h: i(GLU_INCOMPATIBLE_GL_VERSION).  Newer OpenGL functions no longer include that function prototype)
                 '
                 );
	FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); });
	FOUT();
	return;
    }
    if(!defined $nodescale)
    {   
        $nodescale = 80;
        $xrot = 30;
        $yrot = 150;
        $zrot = 0;
    }
    
    $w = FIN();
    FIN();
    FCTRL(undef, Label, -text => "X", -side => 'left');
    FCTRL('xrot', Scale, -length => '8c', -from => 0, -to => 360, -orient => 'horizontal', -variable => \$map3d::xrot, -side => 'left');
    FCTRL("autoxrotate", Checkbutton, -variable => \$map3d::autoxrotate, -side => 'left', -command => [\&setupautorotate]);
    FOUT(-fill => 'x');
    FIN();
    FCTRL(undef, Label, -text => 'Y', -side => 'left');
    FCTRL('yrot', Scale, -length => '8c', -from => 0, -to => 360, -orient => 'horizontal', -variable => \$map3d::yrot, -side => 'left');
    FCTRL("autoyrotate", Checkbutton, -variable => \$map3d::autoyrotate, -side => 'left', -command => [\&setupautorotate]);
    FOUT(-fill => 'x');
    FIN();
    FCTRL(undef, Label, -text => 'Z', -side => 'left');
    FCTRL('zrot', Scale, -length => '8c', -from => 0, -to => 360, -orient => 'horizontal', -variable => \$map3d::zrot, -side => 'left');
    FCTRL("autozrotate", Checkbutton, -variable => \$map3d::autozrotate, -side => 'left', -command => [\&setupautorotate]);
    FOUT(-fill => 'x');
    
    FIN();
    FCTRL(undef, Label, -text => 'Node Scale', -side => 'left');
    FCTRL('nodesize', Scale, -from => 0, -to => 100, -orient => 'horizontal', -variable => \$map3d::nodescale, -command => [\&drawcube], -side => 'left');
    FOUT();
    
    FCTRL(undef, Button, -text => "Close", -command => [\&Close]);
    FOUT();
    glut_takeover(@nodes);
}

sub setupautorotate
{
    $repeatid->cancel() if(defined $repeatid);
    
    $repeatid = FGET()->repeat(50, [\&autorotate]) if($autoxrotate||$autoyrotate||$autozrotate);
}


sub Close
{
    glutHideWindow();
    FCLOSE();
}

sub glut_takeover
{
    if($glutInited==0)
    {
	glutInit();
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition (0, 0);
	$glutwin = glutCreateWindow ("coreprocessor 3D Map");
	setup_perspective(300,300);
	setup_lights();
	&drawcube($CORE, @nodes);
	glutDisplayFunc(\&display);
	glutReshapeFunc(undef);
	glutMouseFunc(undef);
	glutIdleFunc(\&glut_tk);
	
	$glutInited = 1;
	glutMainLoop();
    }
    else
    {
	glutShowWindow();
    }
}
sub autorotate
{
    $xrot=(($xrot==360)?0:$xrot+3) if($autoxrotate);
    $yrot=(($yrot==360)?0:$yrot+3) if($autoyrotate);
    $zrot=(($zrot==360)?0:$zrot+3) if($autozrotate);
}
sub glut_tk
{
    FMW()->update();
    glutPostRedisplay();
}

sub setup_perspective
{
    my($width, $height) = @_;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, $width/$height, .1, 1000.0);    
    
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
}

sub setup_lights
{
    my @light0_position = (200.0, 800.0, 200.0, 0.0);
    
    
    my @light_diffuse = (1.0, 1.0, 1.0, 1.0);
    my @light_ambient = (0.15, 0.15, 0.15, 0.15);
    my @light_specular = (1.0, 1.0, 1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLightfv_p(GL_LIGHT0, GL_POSITION, @light0_position);
    glLightfv_p(GL_LIGHT0, GL_DIFFUSE, @light_diffuse);
    glLightfv_p(GL_LIGHT0, GL_AMBIENT, @light_ambient);
    glLightfv_p(GL_LIGHT0, GL_SPECULAR, @light_specular);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

sub setup_material
{
    my($red, $blue, $green, $alpha) = @_;
    glMatrixMode(GL_MODELVIEW);
    
    my @mat_specular    = (1.0, 1.0, 1.0, 1.0);
    my @mat_shininess        = (50.0);
    my @mat_amb_diff_color = ($red, $blue, $green, $alpha);
    glMaterialfv_p(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, @mat_amb_diff_color);
#    glMaterialfv_p(GL_FRONT, GL_SHININESS, @mat_shininess);
}


sub drawcube
{
    $mycube = glGenLists(1);
    glNewList($mycube, GL_COMPILE);
    
    setup_material(1.0, 0.25, 0.25, 0.5);
    foreach $node (@nodes)
    {
        next if($CORE->{$node}{"ionode"} == 1);
	buildcube($CORE->{$node}{"personality"}{"x"}-$CORE->{"maxx"}/2, 
		  $CORE->{$node}{"personality"}{"y"}-$CORE->{"maxy"}/2, 
		  $CORE->{$node}{"personality"}{"z"}-$CORE->{"maxz"}/2);
    }
    
    setup_material(0.5, 1.0, 0.5, 0.5);
    buildbounds($CORE->{"maxx"}, $CORE->{"maxy"}, $CORE->{"maxz"});
    
    glEndList();    
}

sub buildbounds
{
    my($x, $y, $z) = @_;
    glPushMatrix();
    
    glColor4f(1, 1, 1, .25);
    glScalef($x,$y,$z);
    glTranslatef(-.5, -.5, -.5);
    glBegin(GL_LINE_LOOP); glVertex3f(0,0,0);    glVertex3f(1,0,0);    glVertex3f(1,1,0);    glVertex3f(0,1,0);    glEnd();
    glBegin(GL_LINE_LOOP); glVertex3f(0,0,1);    glVertex3f(1,0,1);    glVertex3f(1,1,1);    glVertex3f(0,1,1);    glEnd();
    glBegin(GL_LINE_LOOP); glVertex3f(0,0,0);    glVertex3f(0,1,0);    glVertex3f(0,1,1);    glVertex3f(0,0,1);    glEnd();
    glBegin(GL_LINE_LOOP); glVertex3f(1,0,0);    glVertex3f(1,1,0);    glVertex3f(1,1,1);    glVertex3f(1,0,1);    glEnd();
    glBegin(GL_LINE_LOOP); glVertex3f(0,0,0);    glVertex3f(1,0,0);    glVertex3f(1,0,1);    glVertex3f(0,0,1);    glEnd();
    glBegin(GL_LINE_LOOP); glVertex3f(0,1,0);    glVertex3f(1,1,0);    glVertex3f(1,1,1);    glVertex3f(0,1,1);    glEnd();
    
    my $coneheight = 0.25;
    my $conewidth  = 0.1;
    
    glPushMatrix(); # Y axis
        glRotated(-90,1,0,0);
    glTranslatef(0,0,1);
    glutSolidCone($conewidth, $coneheight, 10, 10);
    glPopMatrix();
    
    glPushMatrix(); # X axis
        glRotated(90,0,1,0);
    glTranslatef(0,0,1);
    glutSolidCone($conewidth, $coneheight, 10, 10);
    glPopMatrix();
    
    glPushMatrix(); # Z axis
        glTranslatef(0,0,1);
    glutSolidCone($conewidth, $coneheight, 10, 10);
    glPopMatrix();
    
    glDisable(GL_LIGHTING);    
    glRasterPos3f(0,0,0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,ord('o'));
    glRasterPos3f(1.0+$coneheight,0,0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,ord('X'));
    glRasterPos3f(0,1.0+$coneheight,0);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,ord('Y'));
    glRasterPos3f(0,0,1.0+$coneheight);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13,ord('Z'));
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    
    
    glPopMatrix();
}

sub buildcube
{
    my($x, $y, $z) = @_;
    glPushMatrix();
    glTranslatef($x, $y, $z);
    glTranslatef(0.5, 0.5, 0.5);
    glutSolidCube($nodescale/100);
    glPopMatrix();
}

sub display
{
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
#    gluLookAt(GetValue("xeye"), GetValue("yeye"), GetValue("zeye"), 
#	      0.0, 0.0, 0.0,   
#	      0.0, 1.0, 0.0);
    
    glPushMatrix();
    glTranslatef(0.0, 0.0, (-$CORE->{"maxz"}-1)*3);
    glRotated($xrot, 1, 0, 0);
    glRotated($yrot, 0, 1, 0);
    glRotated($zrot, 0, 0, 1);
    glCallList($mycube);
    glPopMatrix();
    glutSwapBuffers();
}

1;
