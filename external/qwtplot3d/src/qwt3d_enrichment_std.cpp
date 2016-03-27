#include <math.h>
#include "qwt3d_color.h"
#include "qwt3d_plot3d.h"
#include "qwt3d_enrichment_std.h"

using namespace Qwt3D;

/////////////////////////////////////////////////////////////////
//
//   CrossHair
//
/////////////////////////////////////////////////////////////////

CrossHair::CrossHair()
{
    configure(0, 1, false, false);
}

CrossHair::CrossHair(double rad, double linewidth, bool smooth, bool boxed)
{
    configure(rad, linewidth, smooth, boxed);
}

void CrossHair::configure(double rad, double linewidth, bool smooth, bool boxed)
{
    plot_p = 0;
    radius_ = rad;
    linewidth_ = linewidth;
    smooth_ = smooth;
    boxed_ = boxed;
}

void CrossHair::drawBegin()
{
    setLineWidth( linewidth_ );
    oldstate_ = glIsEnabled(GL_LINE_SMOOTH);
    if (smooth_)
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);
    glBegin( GL_LINES );
}

void CrossHair::drawEnd()
{
    glEnd();

    if (oldstate_)
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);
}

void CrossHair::draw(Qwt3D::Triple const& pos)
{
    RGBA rgba = plot_p->dataColor()->rgba(pos);
    glColor4d(rgba.r,rgba.g,rgba.b,rgba.a);

    double diag = (plot_p->hull().maxVertex-plot_p->hull().minVertex).length() * radius_;

    glVertex3d( pos.x - diag, pos.y, pos.z);
    glVertex3d( pos.x + diag, pos.y, pos.z);

    glVertex3d( pos.x, pos.y - diag, pos.z);
    glVertex3d( pos.x, pos.y + diag, pos.z);

    glVertex3d( pos.x, pos.y, pos.z - diag);
    glVertex3d( pos.x, pos.y, pos.z + diag);

    // hull

    if (!boxed_)
        return;

    glVertex3d( pos.x - diag, pos.y - diag, pos.z + diag);
    glVertex3d( pos.x + diag, pos.y - diag, pos.z + diag);
    glVertex3d( pos.x - diag, pos.y - diag, pos.z - diag);
    glVertex3d( pos.x + diag, pos.y - diag, pos.z - diag);

    glVertex3d( pos.x - diag, pos.y + diag, pos.z + diag);
    glVertex3d( pos.x + diag, pos.y + diag, pos.z + diag);
    glVertex3d( pos.x - diag, pos.y + diag, pos.z - diag);
    glVertex3d( pos.x + diag, pos.y + diag, pos.z - diag);

    glVertex3d( pos.x - diag, pos.y - diag, pos.z + diag);
    glVertex3d( pos.x - diag, pos.y + diag, pos.z + diag);
    glVertex3d( pos.x - diag, pos.y - diag, pos.z - diag);
    glVertex3d( pos.x - diag, pos.y + diag, pos.z - diag);

    glVertex3d( pos.x + diag, pos.y - diag, pos.z + diag);
    glVertex3d( pos.x + diag, pos.y + diag, pos.z + diag);
    glVertex3d( pos.x + diag, pos.y - diag, pos.z - diag);
    glVertex3d( pos.x + diag, pos.y + diag, pos.z - diag);

    glVertex3d( pos.x - diag, pos.y - diag, pos.z - diag);
    glVertex3d( pos.x - diag, pos.y - diag, pos.z + diag);
    glVertex3d( pos.x + diag, pos.y - diag, pos.z - diag);
    glVertex3d( pos.x + diag, pos.y - diag, pos.z + diag);

    glVertex3d( pos.x - diag, pos.y + diag, pos.z - diag);
    glVertex3d( pos.x - diag, pos.y + diag, pos.z + diag);
    glVertex3d( pos.x + diag, pos.y + diag, pos.z - diag);
    glVertex3d( pos.x + diag, pos.y + diag, pos.z + diag);
}

/////////////////////////////////////////////////////////////////
//
//   Dot
//
/////////////////////////////////////////////////////////////////

Dot::Dot()
{
    configure(1, false);
}

Dot::Dot(double pointsize, bool smooth)
{
    configure(pointsize, smooth);
}

void Dot::configure(double pointsize, bool smooth)
{
    plot_p = 0;
    pointsize_ = pointsize;
    smooth_ = smooth;
}

void Dot::drawBegin()
{
    GLfloat lw[2];
    glGetFloatv(GL_POINT_SIZE_RANGE, lw);

    if (pointsize_ < lw[0])
        pointsize_ = lw[0];
    else if (pointsize_ > lw[1])
        pointsize_ = lw[1];

    glPointSize(pointsize_);

    oldstate_ = glIsEnabled(GL_POINT_SMOOTH);
    if (smooth_)
        glEnable(GL_POINT_SMOOTH);
    else
        glDisable(GL_POINT_SMOOTH);

    //glPointSize(10);
    glBegin( GL_POINTS );
}

void Dot::drawEnd()
{
    glEnd();

    if (oldstate_)
        glEnable(GL_POINT_SMOOTH);
    else
        glDisable(GL_POINT_SMOOTH);
}

void Dot::draw(Qwt3D::Triple const& pos)
{
    if (!IS_NAN(pos.z))
	{
		RGBA rgba = plot_p->dataColor()->rgba(pos);
		glColor4d(rgba.r,rgba.g,rgba.b,rgba.a);
		glVertex3d(pos.x, pos.y, pos.z);
	}
}

/////////////////////////////////////////////////////////////////
//
//   Ball
//
/////////////////////////////////////////////////////////////////

Ball::Ball()
{
    sphere = gluNewQuadric();
    configure(1, 11);
}

Ball::Ball(double rad, unsigned quality)
{
    sphere = gluNewQuadric();
    configure(rad, quality);
}

Ball::~Ball()
{
    gluDeleteQuadric(sphere);
}

void Ball::configure(double rad, unsigned quality)
{
    plot_p = 0;
    radius_ = rad;
    quality_ = quality;
    oldstate_ = GL_FALSE;

    gluQuadricDrawStyle(sphere,GLU_FILL);
    gluQuadricNormals(sphere,GLU_SMOOTH);
    gluQuadricOrientation(sphere,GLU_OUTSIDE);
}

void Ball::draw(Qwt3D::Triple const& pos)
{  
    //RGBA rgba = (*plot_p->dataColor())(pos);
    glColor4d(rgba_.r,rgba_.g,rgba_.b,rgba_.a);

    GLint mode;
    glGetIntegerv(GL_MATRIX_MODE, &mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    gluSphere(sphere, radius_, quality_, quality_);

    glPopMatrix();
    glMatrixMode(mode);
}

/////////////////////////////////////////////////////////////////
//
//   Cone
//
/////////////////////////////////////////////////////////////////

Cone::Cone()
{
    hat      = gluNewQuadric();
    disk     = gluNewQuadric();

    configure(0, 3);
}

Cone::Cone(double rad, unsigned quality)
{
    hat      = gluNewQuadric();
    disk     = gluNewQuadric();

    configure(rad, quality);
}

Cone::~Cone()
{
    gluDeleteQuadric(hat);
    gluDeleteQuadric(disk);
}

void Cone::configure(double rad, unsigned quality)
{
    plot_p = 0;
    radius_ = rad;
    quality_ = quality;
    oldstate_ = GL_FALSE;

    gluQuadricDrawStyle(hat,GLU_FILL);
    gluQuadricNormals(hat,GLU_SMOOTH);
    gluQuadricOrientation(hat,GLU_OUTSIDE);
    gluQuadricDrawStyle(disk,GLU_FILL);
    gluQuadricNormals(disk,GLU_SMOOTH);
    gluQuadricOrientation(disk,GLU_OUTSIDE);
}

void Cone::draw(Qwt3D::Triple const& pos)
{  
    //RGBA rgba = (*plot_p->dataColor())(pos);
    glColor4d(rgba_.r,rgba_.g,rgba_.b,rgba_.a);

    GLint mode;
    glGetIntegerv(GL_MATRIX_MODE, &mode);
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);

    gluCylinder(hat, 0.0, radius_, radius_*2, quality_, 1);
    glTranslatef(0, 0, radius_*2);
    gluDisk(disk, 0.0, radius_, quality_, 1);

    glPopMatrix();
    glMatrixMode(mode);
}


/////////////////////////////////////////////////////////////////
//
//   Arrow
//
/////////////////////////////////////////////////////////////////

Arrow::Arrow()
{	
    hat      = gluNewQuadric();
    disk     = gluNewQuadric();
    base    = gluNewQuadric();
    bottom   = gluNewQuadric();

    gluQuadricDrawStyle(hat,GLU_FILL);
    gluQuadricNormals(hat,GLU_SMOOTH);
    gluQuadricOrientation(hat,GLU_OUTSIDE);
    gluQuadricDrawStyle(disk,GLU_FILL);
    gluQuadricNormals(disk,GLU_SMOOTH);
    gluQuadricOrientation(disk,GLU_OUTSIDE);
    gluQuadricDrawStyle(base,GLU_FILL);
    gluQuadricNormals(base,GLU_SMOOTH);
    gluQuadricOrientation(base,GLU_OUTSIDE);
    gluQuadricDrawStyle(bottom,GLU_FILL);
    gluQuadricNormals(bottom,GLU_SMOOTH);
    gluQuadricOrientation(bottom,GLU_OUTSIDE);

    configure(3, 0.4, 0.06, 0.02);
}

Arrow::~Arrow()
{
    gluDeleteQuadric(hat);
    gluDeleteQuadric(disk);
    gluDeleteQuadric(base);
    gluDeleteQuadric(bottom);
}

/**
\param segs number of faces for the fields arrows (see the gallery for examples)
\param relconelength see picture
\param relconerad see picture
\param relstemrad see picture
\image html arrowanatomy.png 
*/
void Arrow::configure(int segs, double relconelength, double relconerad, double relstemrad)
{
    plot_p = 0;
    segments_ = segs;
    oldstate_ = GL_FALSE;
    rel_cone_length = relconelength;
    rel_cone_radius = relconerad;
    rel_stem_radius = relstemrad;
}

void Arrow::draw(Qwt3D::Triple const& pos)
{	
    Triple end = top_;
    Triple beg = pos;
    Triple vdiff = end-beg;
    double length = vdiff.length();
    glColor4d(rgba_.r,rgba_.g,rgba_.b,rgba_.a);

    double radius[2];
    radius[0] = rel_cone_radius * length;
    radius[1] = rel_stem_radius * length;

    GLint mode;
    glGetIntegerv(GL_MATRIX_MODE, &mode);

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();


    Triple axis;
    double phi = calcRotation(axis, FreeVector(beg,end));

    glTranslatef(beg.x, beg.y, beg.z);
    glRotatef(phi, axis.x, axis.y, axis.z);

    double baseheight = (1-rel_cone_length) * length;

    glTranslatef(0, 0, baseheight);

    gluCylinder(hat, radius[0], 0.0, rel_cone_length * length, segments_,1);
    gluDisk(disk,radius[1],radius[0], segments_,1);

    glTranslatef(0, 0, -baseheight);

    gluCylinder(base, radius[1],radius[1], baseheight,segments_,1);
    gluDisk(disk,0,radius[1],segments_,1);

    glPopMatrix();
    glMatrixMode(mode);
}


//! transform a vector on the z axis with length |beg-end|, to get them in coincidence with the vector(beg,end)
/**
    \return Angle in degree to rotate
    \param axis   The axis to rotate around
    \param beg    result vector base point
    \param end    result vector top point
*/ 
double Arrow::calcRotation(Triple& axis, FreeVector const& vec)
{

    const Triple& end = vec.top;
    const Triple& beg = vec.base;

    Triple firstbeg(0.0,0.0,0.0);
    Triple firstend(0.0,0.0,(end-beg).length());

    Triple first = firstend - firstbeg;
    first.normalize();

    Triple second = end-beg;
    second.normalize();

    axis = normalizedcross(first,second);
    double cosphi = dotProduct(first,second);

    return 180 * acos(cosphi) / Qwt3D::PI;
}

/////////////////////////////////////////////////////////////////
//
//   Stick
//
/////////////////////////////////////////////////////////////////

Stick::Stick()
{	
    init(3,1,false);
}

Stick::Stick(double rad, int segs, bool open /*=true*/)
{
    init(rad,segs,open);
}

void Stick::init(double rad, int segs, bool open)
{
    base    = gluNewQuadric();
    gluQuadricDrawStyle(base,GLU_FILL);
    gluQuadricNormals(base,GLU_SMOOTH);
    gluQuadricOrientation(base,GLU_OUTSIDE);

    open_ = open;
    if (!open_)
    {
        top   = gluNewQuadric();
        bottom   = gluNewQuadric();
        gluQuadricDrawStyle(top,GLU_FILL);
        gluQuadricNormals(top,GLU_SMOOTH);
        gluQuadricOrientation(top,GLU_OUTSIDE);
        gluQuadricDrawStyle(bottom,GLU_FILL);
        gluQuadricNormals(bottom,GLU_SMOOTH);
        gluQuadricOrientation(bottom,GLU_OUTSIDE);
    }
    configure(rad, segs);
}


Stick::~Stick()
{
    gluDeleteQuadric(base);
    if (!open_)
    {
        gluDeleteQuadric(top);
        gluDeleteQuadric(bottom);
    }
}

/**
\param segs number of faces for the fields arrows (see the gallery for examples)
\param rad cylinder radius
*/
void Stick::configure(double rad, int segs)
{
    plot_p = 0;
    segments_ = segs;
    radius_ = rad;
}

void Stick::draw(Triple const& beg, Triple const& end)
{	
    Triple vdiff = end-beg;
    double length = vdiff.length();
    glColor4d(rgba_.r,rgba_.g,rgba_.b,rgba_.a);

    GLint mode;
    glGetIntegerv(GL_MATRIX_MODE, &mode);

    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();

    Triple axis;
    double phi = calcRotation(axis, FreeVector(beg,end));

    glTranslatef(beg.x, beg.y, beg.z);
    glRotatef(phi, axis.x, axis.y, axis.z);

    gluCylinder(base, radius_, radius_, length, segments_, 1);
	if (!open_)
    	gluDisk(bottom, 0, radius_, segments_, 1);

    glTranslatef(0, 0, length);
	if (!open_)
    	gluDisk(top, 0, radius_, segments_, 1);

    glPopMatrix();
    glMatrixMode(mode);
}


//! transform a vector on the z axis with length |beg-end|, to get them in coincidence with the vector(beg,end)
/**
\return Angle in degree to rotate
\param axis   The axis to rotate around
\param beg    result vector base point
\param end    result vector top point
*/ 
double Stick::calcRotation(Triple& axis, FreeVector const& vec)
{

    const Triple& end = vec.top;
    const Triple& beg = vec.base;

    Triple firstbeg(0.0,0.0,0.0);
    Triple firstend(0.0,0.0,(end-beg).length());

    Triple first = firstend - firstbeg;
    first.normalize();

    Triple second = end-beg;
    second.normalize();

    axis = normalizedcross(first,second);
    double cosphi = dotProduct(first,second);

    return 180 * acos(cosphi) / Qwt3D::PI;
}

