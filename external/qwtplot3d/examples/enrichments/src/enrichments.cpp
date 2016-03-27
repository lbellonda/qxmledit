#include "qbitmap.h"

#include "qwt3d_color.h"
#include "qwt3d_plot3d.h"
#include "enrichments.h"

using namespace Qwt3D;


/////////////////////////////////////////////////////////////////
//
//   Bar
//
/////////////////////////////////////////////////////////////////

Bar::Bar()
{
  configure(0, 1);
}

Bar::Bar(double rad, double level)
{
  configure(rad, level);
}

void Bar::configure(double rad, double level)
{
  plot_p = 0;
  radius_ = rad;
  level_ = level;
}

void Bar::drawBegin()
{  
  diag_ = (plot_p->hull().maxVertex-plot_p->hull().minVertex).length() * radius_;
  glLineWidth( 0 );
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);
}

void Bar::drawEnd()
{
}

void Bar::draw(Qwt3D::Triple const& pos)
{
  
//	GLStateBewarer sb(GL_LINE_SMOOTH, true);
//  sb.turnOn();

  double interval = plot_p->hull().maxVertex.z-plot_p->hull().minVertex.z;
  double numlevel = plot_p->hull().minVertex.z + level_ * interval;
  interval /=100;
  if (pos.z > numlevel - interval && pos.z < numlevel + interval )
  {
    Label3D lb;
    lb.draw(pos, diag_, diag_ * 2);
  }
  
  GLdouble minz = plot_p->hull().minVertex.z;

  RGBA rgbat = plot_p->dataColor()->rgba(pos);
	RGBA rgbab = plot_p->dataColor()->rgba(pos.x, pos.y, minz);
	
  glBegin(GL_QUADS);
    glColor4d(rgbab.r,rgbab.g,rgbab.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz);

    if (pos.z > numlevel - interval && pos.z < numlevel + interval )
      glColor3d(0.7,0,0);
    else
      glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
  glEnd();

    glColor3d(0,0,0);
  glBegin(GL_LINES);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz); glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z); glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z); glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz); glVertex3d(pos.x+diag_,pos.y+diag_,minz);

    glVertex3d(pos.x-diag_,pos.y-diag_,minz); glVertex3d(pos.x-diag_,pos.y+diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz); glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z); glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z); glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);

    glVertex3d(pos.x-diag_,pos.y-diag_,minz); glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz); glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz); glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz); glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);
  glEnd();
}

void Label3D::draw(Qwt3D::Triple const& pos, double w, double h)
{
  double gap = 0.3;
  glColor3d(1,1,1);
  glBegin(GL_QUADS);
    glVertex3d(pos.x-w,pos.y,pos.z + gap);
    glVertex3d(pos.x+w,pos.y,pos.z + gap);
    glVertex3d(pos.x+w,pos.y,pos.z + gap + h);
    glVertex3d(pos.x-w,pos.y,pos.z + gap + h);
  glEnd();
  glColor3d(0.4,0,0);
  glBegin(GL_LINE_LOOP);
    glVertex3d(pos.x-w,pos.y,pos.z + gap);
    glVertex3d(pos.x+w,pos.y,pos.z + gap);
    glVertex3d(pos.x+w,pos.y,pos.z + gap + h);
    glVertex3d(pos.x-w,pos.y,pos.z + gap + h);
  glEnd();
  glBegin(GL_LINES);
    glVertex3d(pos.x,pos.y,pos.z);
    glVertex3d(pos.x,pos.y,pos.z + gap);
  glEnd();
}
