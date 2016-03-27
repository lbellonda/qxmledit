#include "qwt3d_surfaceplot.h"

using namespace std;
using namespace Qwt3D;

/**
Initializes with dataNormals()==false //todo
*/
SurfacePlot::SurfacePlot( QWidget * parent, const QGLWidget * shareWidget)
    : Plot3D( parent, shareWidget)
{  
    floorstyle_ = NOFLOOR;
    datanormals_p = false;
    normalLength_p = 0.02;
    normalQuality_p = 3;
}

void SurfacePlot::showNormals(bool b)
{
    datanormals_p = b;
}

/**
Values < 0 or > 1 are ignored
*/
void SurfacePlot::setNormalLength(double val)
{
    if (val<0 || val>1)
        return;
    normalLength_p = val;
}

/**
Values < 3 are ignored 
*/
void SurfacePlot::setNormalQuality(int val) 
{
    if (val<3)
        return;
    normalQuality_p = val;
}

void SurfacePlot::drawIntersection(const Qwt3D::TripleVector& intersection, Qwt3D::RGBA col)
{
    if (intersection.empty())
        return;

    glColor4d(col.r, col.g, col.b, col.a);
    if (intersection.size()>2)
    {
        glBegin(GL_LINE_STRIP);
        for (unsigned dd = 0; dd!=intersection.size(); ++dd)
        {
            glVertex3d(intersection[dd].x, intersection[dd].y, intersection[0].z);
        }
        glEnd();
        glBegin(GL_POINTS);
        glVertex3d(intersection[0].x,intersection[0].y,intersection[0].z);
        glEnd();
    }
    else if (intersection.size() == 2)
    {
        glBegin(GL_LINES);
        glVertex3d(intersection[0].x,intersection[0].y,intersection[0].z);
        glVertex3d(intersection[1].x,intersection[1].y,intersection[0].z);

        // small pixel gap problem (see OpenGL spec.)
        glVertex3d(intersection[1].x,intersection[1].y,intersection[0].z);
        glVertex3d(intersection[0].x,intersection[0].y,intersection[0].z);
        glEnd();
    }
}

void SurfacePlot::updateNormals()
{
    makeCurrent();
    SaveGlDeleteLists(displaylists_p[NormalObject], 1);

    if (plotStyle() == NOPLOT && !normals())
        return;

    displaylists_p[NormalObject] = glGenLists(1);
    glNewList(displaylists_p[NormalObject], GL_COMPILE);
    for (unsigned k=0; k!=plotlets_p.size(); ++k)
    {
        createNormals(plotlets_p[k]);
    }
    glEndList();
}

void SurfacePlot::createFloorOpenGlData()
{
    for (unsigned k=0; k!=plotlets_p.size(); ++k)
    {
        switch (floorStyle())
        {
        case FLOORDATA:
            this->data2Floor(plotlets_p[k]);
            break;
        case FLOORISO:
            this->isolines2Floor(plotlets_p[k]);
            break;
        default:
            break;
        }
    }
}

bool SurfacePlot::removePlotlet(unsigned idx)
{
    if (!Plot3D::removePlotlet(idx))
        return false;

    updateData();
    return true;
}
