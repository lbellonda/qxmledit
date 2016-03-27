#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning (disable : 4305)
#pragma warning (disable : 4786)
#endif

#include "qwt3d_color_std.h"
#include "qwt3d_plot3d.h"

#include <QtCore/QElapsedTimer>
#include <QDebug>

using namespace Qwt3D;

Qwt3D::Plot3D::Plotlet::Plotlet(Data* d, const Appearance& a)
{
    data = ValuePtr<Data>(d);
    appearance = ValuePtr<Appearance>(new Appearance(a));
}

/*!
  This should be the first call in your derived classes constructors.
*/
Plot3D::Plot3D(QWidget * parent, const QGLWidget * shareWidget)
    : ExtGLWidget(parent, shareWidget),
	m_fastNormals(false)
{  
    plotlets_p.push_back(Plotlet(0));
    renderpixmaprequest_ = false;
    isolinesZ_p.resize(10);
    delayisolinecalculation_p = true;
    displaylegend_ = false;

    setBackgroundColor(RGBA(1.0,1.0,1.0,1.0));

    displaylists_p = std::vector<GLuint>(DisplayListSize);
    for (unsigned k=0; k!=displaylists_p.size(); ++k)
    {
        displaylists_p[k] = 0;
    }

    title_.setFont("Courier", 16, QFont::Bold);
    title_.setString("");

    setTitlePosition(0.95);

    legend_.setLimits(0, 100);
    legend_.setMajors(10);
    legend_.setMinors(2);
    legend_.setOrientation(ColorLegend::BottomTop, ColorLegend::Left);
}

/*!
  Release allocated resources
*/

Plot3D::~Plot3D()
{
    makeCurrent();
    SaveGlDeleteLists(displaylists_p[0], displaylists_p.size());
}

void Plot3D::initializeGL()
{
    ExtGLWidget::initializeGL();
    if (renderpixmaprequest_)
    {
        updateData();
        renderpixmaprequest_ = false;
    }
}

//! Reimplements QGLWidget::renderPixmap
QPixmap Plot3D::renderPixmap(int w/* =0 */, int h/* =0 */, bool useContext/* =false */)
{
    renderpixmaprequest_ = true;
    return QGLWidget::renderPixmap(w,h,useContext);
}

/*!
  Paint the widgets content.
*/
void Plot3D::paintGL()
{
    // debug mode
    //	QElapsedTimer tm;
    //	tm.start();


    glClearColor(bgcolor_.r, bgcolor_.g, bgcolor_.b, bgcolor_.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glRotatef(-90, 1.0, 0.0, 0.0);
    glRotatef(0.0, 0.0, 1.0, 0.0);
    glRotatef(0.0, 0.0, 0.0, 1.0);

    if (displaylegend_)
    {
        legend_.draw();
    }
    title_.setRelPosition(titlerel_, titleanchor_);
    title_.draw();

    Triple beg = coordinates_p.first();
    Triple end = coordinates_p.second();

    Triple center = beg + (end-beg) / 2;
    double radius = (center-beg).length();

    glLoadIdentity();

    glRotatef(xRotation()-90, 1.0, 0.0, 0.0);
    glRotatef(yRotation(), 0.0, 1.0, 0.0);
    glRotatef(zRotation(), 0.0, 0.0, 1.0);

    glScalef(zoom() * xScale(), zoom() * yScale(), zoom() * zScale());

    glTranslatef(xShift()-center.x, yShift()-center.y, zShift()-center.z);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (beg != end)
    {
        if (ortho())
            glOrtho(-radius, +radius, -radius, +radius, 0, 40 * radius);
        else
            glFrustum(-radius, +radius, -radius, +radius, 5 * radius, 400 * radius);
    }
    else
    {
        if (ortho())
            glOrtho(-1.0, 1.0, -1.0, 1.0, 10.0, 100.0);
        else
            glFrustum(-1.0, 1.0, -1.0, 1.0, 10.0, 100.0);
    }

    glTranslatef(xViewportShift() * 2 * radius , yViewportShift() * 2 * radius , -7 * radius);

	applyLights();

	if (lightingEnabled())
        glEnable(GL_NORMALIZE);

	drawOpenGlData();

    coordinates_p.draw();

    if (lightingEnabled())
        glDisable(GL_NORMALIZE);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();


    // debug info
    /*  qint64 time = tm.elapsed();
  glLoadIdentity();

  Qwt3D::Label timerLabel;
  timerLabel.setString(QString("paintGL: %1ms   createData: %2ms").arg(time).arg(m_createTime));
    timerLabel.setPosition(Qwt3D::Triple(-2,2,0), TopLeft);
    timerLabel.draw();*/
}


/*!
  Set up the OpenGL view port
*/
void Plot3D::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    paintGL();
}

/*!
    Calculates the smallest x-y-z parallelepiped enclosing the data.
    It can be accessed by hull();
*/
void Plot3D::calculateHull()
{
    if (plotlets_p.empty())
        return;

    ParallelEpiped hull = plotlets_p[0].data->hull();
    for (unsigned i=1; i!=plotlets_p.size(); ++i)
        hull = sum(hull, plotlets_p[i].data->hull());

    setHull(hull);
}

/*!
    Create a coordinate system with generating corners beg and end
*/
void Plot3D::createCoordinateSystem(Triple beg, Triple end)
{
    if (beg != coordinates_p.first() || end != coordinates_p.second())
        coordinates_p.init(beg, end);
}

/*!
    Create a coordinate system from data
*/
void Plot3D::createCoordinateSystem()
{
    calculateHull();
    Triple beg = hull().minVertex;
    Triple end = hull().maxVertex;
    createCoordinateSystem(beg, end);
}

/*!
  Show a color legend
*/
void Plot3D::showColorLegend(bool show, unsigned idx /* = 0 */)
{
    displaylegend_ = show;

    if (idx >= plotlets_p.size())
        return;
    if (show)
        plotlets_p[idx].appearance->dataColor()->createVector(legend_.colors);
    updateGL();
}

void Plot3D::setMeshColor(RGBA rgba)
{
    appearance(0).setMeshColor(rgba);
}

void Plot3D::setBackgroundColor(RGBA rgba)
{
    bgcolor_ = rgba;
}


/*!
    Assign a new coloring object for the data.
*/
void Plot3D::setDataColor(const Qwt3D::Color& col)
{
    appearance(0).setDataColor(col);
}

/*!
  Set style of coordinate system
*/
void Plot3D::setCoordinateStyle(COORDSTYLE st)
{
    coordinates_p.setStyle(st);
    updateGL();
}

/*!
  Set plotstyle for the standard plotting types. An argument of value Qwt3D::USER
  is ignored.
*/
void Plot3D::setPlotStyle(PLOTSTYLE val)
{
    appearance(0).setPlotStyle(val);
}

/*!
  Set plotstyle to Qwt3D::USER and an associated enrichment object.
*/
Qwt3D::Enrichment* Plot3D::setPlotStyle(Qwt3D::Enrichment const& obj)
{
    return appearance(0).setPlotStyle(obj);
}

/*!
  Set shading style
*/
void Plot3D::setShading(SHADINGSTYLE val)
{
    appearance(0).setShading(val);
    updateGL();
}

/*!
  Set Polygon offset. The function affects the OpenGL rendering process.
    Try different values for surfaces with polygons only and with mesh and polygons
*/
void Plot3D::setPolygonOffset(double val)
{
    appearance(0).setPolygonOffset(val);
}

void Plot3D::setMeshLineWidth(double val)
{
    appearance(0).setMeshLineWidth(val);
}


/*!
Set relative caption position (0.5,0.5) means, the anchor point lies in the center of the screen
*/
void Plot3D::setTitlePosition(double rely, double relx, Qwt3D::ANCHOR anchor)
{
    titlerel_.y = (rely<0 || rely>1) ? 0.5 : rely;
    titlerel_.x = (relx<0 || relx>1) ? 0.5 : relx;

    titleanchor_ = anchor;
}

/*!
Set caption font
*/
void Plot3D::setTitleFont(const QString& family, int pointSize, int weight, bool italic)
{ 
    title_.setFont(family, pointSize, weight, italic);
}

/*!
  Update OpenGL data representation
*/
void Plot3D::updateData()
{
	makeCurrent();
    GLStateBewarer dt(GL_DEPTH_TEST, true);
    GLStateBewarer ls(GL_LINE_SMOOTH, true);

    calculateHull();

    SaveGlDeleteLists(displaylists_p[DataObject], 1); // nur Daten

    this->updateAppearances();

    //QElapsedTimer timer;
    //timer.start();

    displaylists_p[DataObject] = glGenLists(1);
    glNewList(displaylists_p[DataObject], GL_COMPILE);

    createOpenGlData();

    glEndList();

    //m_createTime = timer.elapsed();
}

/** 
The function is called from updateData before a new internal OpenGL representations 
will be recreated, but after the new hull has been calculated. The update() function 
for every Appearance list member is called by this function. 
So - for example - the Appearance's color part can adapt himself to new z ranges.
*/
void Qwt3D::Plot3D::updateAppearances()
{
    for (unsigned i=0; i!=plotlets_p.size(); ++i)
        appearance(i).update(*this);
}

void Qwt3D::Plot3D::createOpenGlData()
{
    for (unsigned i=0; i!= plotlets_p.size(); ++i)
    {
       this->createOpenGlData(plotlets_p[i]);
    }
}

void Qwt3D::Plot3D::drawOpenGlData()
{
	for (unsigned i=0; i!= displaylists_p.size(); ++i)
	{
		if (i!=LegendObject)
			glCallList(displaylists_p[i]);
	}
}

/**
 Removes Plotlet at position idx. All data and the Appearance object are destroyed.
 You cannot remove the last remaining plotlet (the function returns false for this attempt)
 Semantically, removing an entry is like erasing a vector element. Subsequent elements
 will change their respective position/index. Associated internal copies are cheap, though
 (by utilizing smart pointers).
 \return true for success, false else.
*/
bool Qwt3D::Plot3D::removePlotlet(unsigned idx)
{
    if (idx >= plotlets_p.size() || 1 == plotlets_p.size())
        return false;

    plotlets_p.erase(plotlets_p.begin() + idx);
    updateData();
    createCoordinateSystem();
    return true;
}
