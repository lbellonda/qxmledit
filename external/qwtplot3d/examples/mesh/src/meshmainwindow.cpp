#include <fstream>

#include <QFontDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QStatusBar>
#include <QComboBox>
#include <QTimer>
#include <QFileInfo>

#include <qwt3d_io.h>
#include <qwt3d_io_reader.h>

#include "meshmainwindow.h"

#include "functions.h"
#include "colormapreader.h"
#include "lightingdialog.h"

using namespace Qwt3D;
using namespace std;


MeshMainWindow::~MeshMainWindow()      
{
	delete dataWidget;
}

MeshMainWindow::MeshMainWindow( QWidget* parent )       
	: QMainWindow( parent )
{
	setupUi(this);

  QActionGroup* coord = new QActionGroup(this);
  coord->addAction(Box);
  coord->addAction(Frame);
  coord->addAction(None);
  //coord->setExclusive(true);

  grids = new QActionGroup(this);
  grids->addAction(front); 
  grids->addAction(back);
  grids->addAction(right); 
  grids->addAction(left);
  grids->addAction(ceil);
  grids->addAction(floor); 
  grids->setExclusive(false);

  QActionGroup* plotstyle = new QActionGroup(this);
  plotstyle->addAction(pointstyle);
  plotstyle->addAction(wireframe);
  plotstyle->addAction(hiddenline);
  plotstyle->addAction(polygon);
  plotstyle->addAction(filledmesh);
  plotstyle->addAction(nodata);

  QActionGroup* floorstyle = new QActionGroup(this);
  floorstyle->addAction(floordata);
  floorstyle->addAction(flooriso);
  floorstyle->addAction(floornone);

  QActionGroup* color = new QActionGroup(this);
  color->addAction(axescolor);
  color->addAction(backgroundcolor);
  color->addAction(meshcolor);
  color->addAction(numbercolor);
  color->addAction(labelcolor);
  color->addAction(titlecolor);
  color->addAction(datacolor);
  color->addAction(resetcolor);

  QActionGroup* font = new QActionGroup(this);
  font->addAction(numberfont);
  font->addAction(labelfont);
  font->addAction(titlefont);
  font->addAction(resetfont);


  // toolbars

  QComboBox* functionCB = new QComboBox;
  QComboBox* psurfaceCB = new QComboBox;
  functionCB->clear();
  functionCB->addItem( tr( "---" ) );
  functionCB->addItem( tr( "Hat" ) );
  functionCB->addItem( tr( "Rosenbrock" ) );
  functionCB->addItem( tr( "Saddle" ) );
  functionCB->addItem( tr( "Sombrero" ) );
  functionCB->addItem( tr( "Ripple" ) );
  functionCB->setToolTip(tr( "Display function" ) );
  psurfaceCB->clear();
  psurfaceCB->addItem( tr( "---" ) );
  psurfaceCB->addItem( tr( "Torus" ) );
  psurfaceCB->addItem( tr( "Seashell" ) );
  psurfaceCB->addItem( tr( "Boy" ) );
  psurfaceCB->addItem( tr( "Dini" ) );
  psurfaceCB->setToolTip(tr( "Display parametric surface. Turn Lighting on for better quality."));
  mainToolbar->addWidget(functionCB);
  mainToolbar->addWidget(psurfaceCB);

  QObject::connect(Exit, SIGNAL(triggered()), SLOT(close()));

  QGridLayout *grid = new QGridLayout( frame );

  col_ = 0;
	legend_ = false;
	redrawWait = 50;
	activeCoordSystem = None;

  dataWidget = new GridPlot(frame);
  grid->addWidget( dataWidget, 0, 0 );

  dataWidget->setRenderThreadsCount(2);

	connect( coord, SIGNAL( triggered( QAction* ) ), this, SLOT( pickCoordSystem( QAction* ) ) );
	connect( plotstyle, SIGNAL( triggered( QAction* ) ), this, SLOT( pickPlotStyle( QAction* ) ) );
	connect( axescolor, SIGNAL( triggered() ), this, SLOT( pickAxesColor() ) );
	connect( backgroundcolor, SIGNAL( triggered() ), this, SLOT( pickBgColor() ) );
	connect( floorstyle, SIGNAL( triggered( QAction* ) ), this, SLOT( pickFloorStyle( QAction* ) ) );
	connect( meshcolor, SIGNAL( triggered() ), this, SLOT( pickMeshColor() ) );
	connect( numbercolor, SIGNAL( triggered() ), this, SLOT( pickNumberColor() ) );
	connect( labelcolor, SIGNAL( triggered() ), this, SLOT( pickLabelColor() ) );
	connect( titlecolor, SIGNAL( triggered() ), this, SLOT( pickTitleColor() ) );
	connect( datacolor, SIGNAL( triggered() ), this, SLOT( pickDataColor() ) );
	connect( lighting, SIGNAL( clicked() ), this, SLOT( pickLighting() ) );
	connect( resetcolor, SIGNAL( triggered() ), this, SLOT( resetColors() ) );
 	connect( numberfont, SIGNAL( triggered() ), this, SLOT( pickNumberFont() ) );
	connect( labelfont, SIGNAL( triggered() ), this, SLOT( pickLabelFont() ) );
	connect( titlefont, SIGNAL( triggered() ), this, SLOT( pickTitleFont() ) );
	connect( resetfont, SIGNAL( triggered() ), this, SLOT( resetFonts() ) );
	connect( animation, SIGNAL( toggled(bool) ) , this, SLOT( toggleAnimation(bool) ) );
	connect( openFile, SIGNAL( triggered() ) , this, SLOT( open() ) );
	
  // only EXCLUSIVE groups emit selected :-/
  connect( left, SIGNAL( toggled( bool ) ), this, SLOT( setLeftGrid( bool ) ) );
	connect( right, SIGNAL( toggled( bool ) ), this, SLOT( setRightGrid( bool ) ) );
	connect( ceil, SIGNAL( toggled( bool ) ), this, SLOT( setCeilGrid( bool ) ) );
	connect( floor, SIGNAL( toggled( bool ) ), this, SLOT( setFloorGrid( bool ) ) );
	connect( back, SIGNAL( toggled( bool ) ), this, SLOT( setBackGrid( bool ) ) );
	connect( front, SIGNAL( toggled( bool ) ), this, SLOT( setFrontGrid( bool ) ) );

	timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()), this, SLOT(rotate()) );

	resSlider->setRange(1,70);
	connect( resSlider, SIGNAL(valueChanged(int)), dataWidget, SLOT(setResolution(int)) );
	connect( dataWidget, SIGNAL(resolutionChanged(int)), resSlider, SLOT(setValue(int)) );
	resSlider->setValue(1);             
	
	connect( offsSlider, SIGNAL(valueChanged(int)), this, SLOT(setPolygonOffset(int)) );

	connect(normButton, SIGNAL(clicked()), this, SLOT(setStandardView()));  
	
  QString qwtstr(" qwtplot3d ");
  qwtstr += QString::number(QWT3D_MAJOR_VERSION) + ".";
  qwtstr += QString::number(QWT3D_MINOR_VERSION) + ".";
  qwtstr += QString::number(QWT3D_PATCH_VERSION) + " ";

	QLabel* info = new QLabel(qwtstr, statusBar());       
  statusBar()->addWidget(info, 0);
	filenameWidget = new QLabel("                                  ", statusBar());
	statusBar()->addWidget(filenameWidget,0);
	dimWidget = new QLabel("", statusBar());
	statusBar()->addWidget(dimWidget,0);
	rotateLabel = new QLabel("", statusBar());
	statusBar()->addWidget(rotateLabel,0);
	shiftLabel = new QLabel("", statusBar());
	statusBar()->addWidget(shiftLabel,0);
	scaleLabel = new QLabel("", statusBar());
	statusBar()->addWidget(scaleLabel,0);
	zoomLabel = new QLabel("", statusBar());
	statusBar()->addWidget(zoomLabel,0);
	
	connect(dataWidget, SIGNAL(rotationChanged(double,double,double)),this,SLOT(showRotate(double,double,double)));
	connect(dataWidget, SIGNAL(vieportShiftChanged(double,double)),this,SLOT(showShift(double,double)));
	connect(dataWidget, SIGNAL(scaleChanged(double,double,double)),this,SLOT(showScale(double,double,double)));
	connect(dataWidget, SIGNAL(zoomChanged(double)),this,SLOT(showZoom(double)));

	connect(functionCB, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(createFunction(const QString&)));
	connect(psurfaceCB, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(createPSurface(const QString&)));
	connect(projection, SIGNAL( toggled(bool) ), this, SLOT( toggleProjectionMode(bool)));
	connect(colorlegend, SIGNAL( toggled(bool) ), this, SLOT( toggleColorLegend(bool)));
	connect(autoscale, SIGNAL( toggled(bool) ), this, SLOT( toggleAutoScale(bool)));
	connect(shader, SIGNAL( toggled(bool) ), this, SLOT( toggleShader(bool)));
	connect(mouseinput, SIGNAL( toggled(bool) ), dataWidget, SLOT( enableMouse(bool)));
	connect(lightingswitch, SIGNAL( toggled(bool) ), this, SLOT( enableLighting(bool)));
	connect(normals, SIGNAL( toggled(bool) ), this, SLOT( showNormals(bool)));
	connect(normalsquality,  SIGNAL(valueChanged(int)), this, SLOT(setNormalQuality(int)) );
	connect(normalslength,  SIGNAL(valueChanged(int)), this, SLOT(setNormalLength(int)) );
			
	setStandardView();

	dataWidget->coordinates()->setLineSmooth(true);
  dataWidget->coordinates()->setGridLinesColor(RGBA(0.35,0.35,0.35,1));
	dataWidget->enableMouse(true);
  dataWidget->setKeySpeed(15,20,20);

  lightingdlg_ = new LightingDlg( this );
  lightingdlg_->assign( dataWidget);  

  dataWidget->setTitleFont( "Arial", 14, QFont::Normal );

  grids->setEnabled(false);

  functionCB->setCurrentIndex(1);
  Box->setChecked(true);
  pickCoordSystem(Box);
}

void MeshMainWindow::open()
{
  QString s = QFileDialog::getOpenFileName( this, "", "../../data", "GridData Files (*.mes *.MES)");

	if ( s.isEmpty() || !dataWidget)
      return;
	
  QFileInfo fi( s );
	filenameWidget->setToolTip(s);
  QString ext = fi.suffix();
	filenameWidget->setText(fi.fileName());
    qApp->processEvents(); // enforces repaint;  

  if (IO::load(dataWidget, s, ext))
	{
//		double a = dataWidget->facets().first;
//		double b = dataWidget->facets().second;
//
//		dimWidget->setText(QString("Cells ") + QString::number(a*b) 
//			+ " (" + QString::number(a) + "x" + QString::number(b) +")" );
		
		dataWidget->setResolution(3);
	}
 	
	for (unsigned i=0; i!=dataWidget->coordinates()->axes.size(); ++i)
	{
		dataWidget->coordinates()->axes[i].setMajors(4);
		dataWidget->coordinates()->axes[i].setMinors(5);
		dataWidget->coordinates()->axes[i].setLabelString("");
	}
	
	updateColorLegend(4,5);
	pickCoordSystem(activeCoordSystem);
	dataWidget->showColorLegend(legend_);
}

void MeshMainWindow::createFunction(QString const& name)
{
	dataWidget->makeCurrent();
  
  dataWidget->legend()->setScale(LINEARSCALE);
	for (unsigned i=0; i!=dataWidget->coordinates()->axes.size(); ++i)
	{
		dataWidget->coordinates()->axes[i].setMajors(7);
		dataWidget->coordinates()->axes[i].setMinors(5);
	}

  if (name == QString("Rosenbrock")) 
	{
		Rosenbrock rosenbrock(*dataWidget);
		
		rosenbrock.setMesh(50,51);
		rosenbrock.setDomain(-1.73,1.55,-1.5,1.95);
		rosenbrock.setMinZ(-100);
		
		rosenbrock.create();
	  
    dataWidget->coordinates()->axes[Z1].setScale(LOG10SCALE);
    dataWidget->coordinates()->axes[Z2].setScale(LOG10SCALE);
    dataWidget->coordinates()->axes[Z3].setScale(LOG10SCALE);
    dataWidget->coordinates()->axes[Z4].setScale(LOG10SCALE);
    dataWidget->legend()->setScale(LOG10SCALE);
	}
	else if (name == QString("Hat")) 
	{
		Hat hat(*dataWidget);
		
		hat.setMesh(51,72);
		hat.setDomain(-1.5,1.5,-1.5,1.5);
		hat.create();	
	}
	else if (name == QString("Ripple")) 
	{
		Ripple ripple(*dataWidget);
    ripple.setMesh(120,120);
		ripple.create();	
	}
	else if (name == QString("Saddle")) 
	{
		Saddle saddle;
		
		saddle.setMesh(71,71);
		double dom = 2.5;
		saddle.setDomain(-dom, dom, -dom, dom);
		saddle.assign(*dataWidget);
		saddle.create();
	}
	else if (name == QString("Sombrero")) 
	{
		Mex mex;
		
		mex.setMesh(91,91);
		double dom = 15;
		mex.setDomain(-dom, dom, -dom, dom);
		mex.create(*dataWidget);
	}

//	double a = dataWidget->facets().first;
//	double b = dataWidget->facets().second;
//
//	dimWidget->setText(QString("Cells ") + QString::number(a*b) 
//		+ " (" + QString::number(a) + "x" + QString::number(b) +")" );

	updateColorLegend(7,5);

	dataWidget->coordinates()->axes[X1].setLabelString(QString("X1"));
  dataWidget->coordinates()->axes[X2].setLabelString(QString("X2"));
	dataWidget->coordinates()->axes[X3].setLabelString(QString("X3"));
	dataWidget->coordinates()->axes[X4].setLabelString(QString("X4"));

	dataWidget->coordinates()->axes[Y1].setLabelString(QString("Y1"));
	dataWidget->coordinates()->axes[Y2].setLabelString(QString("Y2"));
	dataWidget->coordinates()->axes[Y3].setLabelString(QString("Y3"));
	dataWidget->coordinates()->axes[Y4].setLabelString(QString("Y4"));

	dataWidget->coordinates()->axes[Z1].setLabelString(QString("Z1"));
	dataWidget->coordinates()->axes[Z2].setLabelString(QString("Z2"));
	dataWidget->coordinates()->axes[Z3].setLabelString(QString("Z3"));
	dataWidget->coordinates()->axes[Z4].setLabelString(QString("Z4"));

	pickCoordSystem(activeCoordSystem);
}

void MeshMainWindow::createPSurface(QString const& name)
{
	dataWidget->makeCurrent();
	if (name == QString("Torus")) 
	{
		Torus sf(*dataWidget);
		sf.create();
	}
	else if (name == QString("Seashell")) 
	{
		Seashell ss(*dataWidget);
		ss.create();
	}
	else if (name == QString("Boy")) 
	{
		Boy boy(*dataWidget);
		boy.create();
	}
	else if (name == QString("Dini")) 
	{
		Dini dini(*dataWidget);
		dini.create();
	}
	for (unsigned i=0; i!=dataWidget->coordinates()->axes.size(); ++i)
	{
		dataWidget->coordinates()->axes[i].setMajors(7);
		dataWidget->coordinates()->axes[i].setMinors(5);
	}

//	double a = dataWidget->facets().first;
//	double b = dataWidget->facets().second;
//
//	dimWidget->setText(QString("Cells ") + QString::number(a*b) 
//		+ " (" + QString::number(a) + "x" + QString::number(b) +")" );

	updateColorLegend(7,5);

	dataWidget->coordinates()->axes[X1].setLabelString(QString("X1"));
  dataWidget->coordinates()->axes[X2].setLabelString(QString("X2"));
	dataWidget->coordinates()->axes[X3].setLabelString(QString("X3"));
	dataWidget->coordinates()->axes[X4].setLabelString(QString("X4"));

	dataWidget->coordinates()->axes[Y1].setLabelString(QString("Y1"));
	dataWidget->coordinates()->axes[Y2].setLabelString(QString("Y2"));
	dataWidget->coordinates()->axes[Y3].setLabelString(QString("Y3"));
	dataWidget->coordinates()->axes[Y4].setLabelString(QString("Y4"));

	dataWidget->coordinates()->axes[Z1].setLabelString(QString("Z1"));
	dataWidget->coordinates()->axes[Z2].setLabelString(QString("Z2"));
	dataWidget->coordinates()->axes[Z3].setLabelString(QString("Z3"));
	dataWidget->coordinates()->axes[Z4].setLabelString(QString("Z4"));

	pickCoordSystem(activeCoordSystem);
}


void MeshMainWindow::pickCoordSystem( QAction* action)
{
	if (!action || !dataWidget)
		return;

	activeCoordSystem = action;
	
	dataWidget->setTitle("QwtPlot3D (Use Ctrl-Alt-Shift-LeftBtn-Wheel or keyboard)");

	if (!dataWidget->hasData())
	{
		double l = 0.6;
		dataWidget->createCoordinateSystem(Triple(-l,-l,-l), Triple(l,l,l));
		for (unsigned i=0; i!=dataWidget->coordinates()->axes.size(); ++i)
		{
			dataWidget->coordinates()->axes[i].setMajors(4);
			dataWidget->coordinates()->axes[i].setMinors(5);
		}
	}			

	if (action == Box || action == Frame)
	{
		if (action == Box)
			dataWidget->setCoordinateStyle(BOX);
		if (action == Frame)
			dataWidget->setCoordinateStyle(FRAME);
		grids->setEnabled(true);
	}
	else if (action == None)
	{
	  dataWidget->setTitle("QwtPlot3D (Use Ctrl-Alt-Shift-LeftBtn-Wheel or keyboard)");
		dataWidget->setCoordinateStyle(NOCOORD);
		grids->setEnabled(false);
	}
}

void MeshMainWindow::pickPlotStyle( QAction* action )
{
	if (!action || !dataWidget)
		return;

	if (action == polygon)
	{
		dataWidget->setPlotStyle(FILLED);
	}
	else if (action == filledmesh)
	{
		dataWidget->setPlotStyle(FILLEDMESH);
	}
	else if (action == wireframe)
	{
		dataWidget->setPlotStyle(WIREFRAME);
	}
	else if (action == hiddenline)
	{
		dataWidget->setPlotStyle(HIDDENLINE);
	}
	else if (action == pointstyle)
	{
    dataWidget->setPlotStyle(Qwt3D::POINTS);
	}
	else
	{
		dataWidget->setPlotStyle(NOPLOT);
	}
	dataWidget->updateData();
	dataWidget->updateGL();
}

void
MeshMainWindow::pickFloorStyle( QAction* action )
{
	if (!action || !dataWidget)
		return;

	if (action == floordata)
	{
		dataWidget->setFloorStyle(FLOORDATA);
	}
	else if (action == flooriso)
	{
		dataWidget->setFloorStyle(FLOORISO);
	}
	else
	{
		dataWidget->setFloorStyle(NOFLOOR);
	}
	
	dataWidget->updateData();
	dataWidget->updateGL();
}	

void MeshMainWindow::setLeftGrid(bool b)
{
  setGrid(Qwt3D::LEFT,b);
}
void MeshMainWindow::setRightGrid(bool b)
{
  setGrid(Qwt3D::RIGHT,b);
}
void MeshMainWindow::setCeilGrid(bool b)
{
  setGrid(Qwt3D::CEIL,b);
}
void MeshMainWindow::setFloorGrid(bool b)
{
  setGrid(Qwt3D::FLOOR,b);
}
void MeshMainWindow::setFrontGrid(bool b)
{
  setGrid(Qwt3D::FRONT,b);
}
void MeshMainWindow::setBackGrid(bool b)
{
  setGrid(Qwt3D::BACK,b);
}

void MeshMainWindow::setGrid(Qwt3D::SIDE s, bool b)
{
  if (!dataWidget)
		return;
  
 int sum = dataWidget->coordinates()->grids();

  if (b)
    sum |= s;
  else
    sum &= ~s;

  dataWidget->coordinates()->setGridLines(sum!=Qwt3D::NOSIDEGRID, sum!=Qwt3D::NOSIDEGRID, sum);
  dataWidget->updateGL();
}

void MeshMainWindow::resetColors()
{
	if (!dataWidget)
		return;

	const RGBA axc = RGBA(0,0,0,1);
	const RGBA bgc = RGBA(1.0,1.0,1.0,1.0);
	const RGBA msc = RGBA(0,0,0,1);
	const RGBA nuc = RGBA(0,0,0,1);
	const RGBA lbc = RGBA(0,0,0,1);
	const RGBA tc = RGBA(0,0,0,1);

	dataWidget->coordinates()->setAxesColor(axc);
	dataWidget->setBackgroundColor(bgc);
	dataWidget->setMeshColor(msc);
	dataWidget->updateData();
	dataWidget->coordinates()->setNumberColor(nuc);
	dataWidget->coordinates()->setLabelColor(lbc);
  dataWidget->setTitleColor(tc);

	dataWidget->setDataColor(StandardColor());
	dataWidget->updateData();	
	dataWidget->updateNormals();
	dataWidget->updateGL();
}


void MeshMainWindow::pickAxesColor()
{
  
  QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
		return;
	RGBA rgb = Qt2GL(c);
	dataWidget->coordinates()->setAxesColor(rgb);
	dataWidget->updateGL();
}

void MeshMainWindow::pickBgColor()
{
  
	QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
		return;
	RGBA rgb = Qt2GL(c);
	dataWidget->setBackgroundColor(rgb);
	dataWidget->updateGL();
}

void MeshMainWindow::pickMeshColor()
{
  
	QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
		return;
	RGBA rgb = Qt2GL(c);
	dataWidget->setMeshColor(rgb);
	dataWidget->updateData();
	dataWidget->updateGL();
}

void MeshMainWindow::pickNumberColor()
{
  
	QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
		return;
	RGBA rgb = Qt2GL(c);
	dataWidget->coordinates()->setNumberColor(rgb);
	dataWidget->updateGL();
}

void MeshMainWindow::pickLabelColor()
{
 	QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
		return;
	RGBA rgb = Qt2GL(c);
	dataWidget->coordinates()->setLabelColor(rgb);
	dataWidget->updateGL();
}
void MeshMainWindow::pickTitleColor()
{
 	QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
		return;
	RGBA rgb = Qt2GL(c);
	dataWidget->setTitleColor(rgb);
	dataWidget->updateGL();
}

void MeshMainWindow::pickLighting()
{
	lightingdlg_->show();
}

void MeshMainWindow::pickDataColor()
{
  QString s =  QFileDialog::getOpenFileName( this, "", "./../colormaps", "Colormap files (*.map *.MAP)");
  adaptDataColors(s);
}

void MeshMainWindow::adaptDataColors(const QString& fileName)
{
  ColorVector cv;
	
	if (!openColorMap(cv, fileName))
		return;
	
	StandardColor col_;
	col_.setColorVector(cv);
	
	dataWidget->setDataColor(col_);
	dataWidget->updateData();
	dataWidget->updateNormals();
	dataWidget->showColorLegend(legend_);
  dataWidget->updateGL();
}

void MeshMainWindow::pickNumberFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, this );
	if ( !ok ) 
	{
		return;
	} 
	dataWidget->coordinates()->setNumberFont(font);
	dataWidget->updateGL();
}
void MeshMainWindow::pickLabelFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, this );
	if ( !ok ) 
	{
		return;
	} 
	dataWidget->coordinates()->setLabelFont(font);
	dataWidget->updateGL();
}
void MeshMainWindow::pickTitleFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, this );
	if ( !ok ) 
	{
		return;
	} 
	dataWidget->setTitleFont(font.family(), font.pointSize(), font.weight(), font.italic());
}

void MeshMainWindow::resetFonts()
{
	dataWidget->coordinates()->setNumberFont(QFont("Courier", 12));
	dataWidget->coordinates()->setLabelFont(QFont("Courier", 14, QFont::Bold));
  dataWidget->setTitleFont( "Arial", 14, QFont::Normal );
	dataWidget->updateGL();
}

void MeshMainWindow::setStandardView()
{
	dataWidget->setRotation(30,0,15);
	dataWidget->setViewportShift(0.05,0);
	dataWidget->setScale(1,1,1);
	dataWidget->setZoom(0.95);
}

/*!
  Turns animation on or off
*/
void MeshMainWindow::toggleAnimation(bool val)
{
	if ( val )
	{
		timer->start( redrawWait ); // Wait this many msecs before redraw
	}
	else
	{
		timer->stop();
	}
}

void MeshMainWindow::rotate()
{
	if (!dataWidget)
		return;

	dataWidget->setRotation(
		int(dataWidget->xRotation() + 1) % 360,
		int(dataWidget->yRotation() + 1) % 360,
		int(dataWidget->zRotation() + 1) % 360
		);
}

void
MeshMainWindow::toggleProjectionMode(bool val)
{
	dataWidget->setOrtho(val);
}

void
MeshMainWindow::toggleColorLegend(bool val)
{
	legend_ = val;
	dataWidget->showColorLegend(val);
}

void
MeshMainWindow::toggleAutoScale(bool val)
{
	dataWidget->coordinates()->setAutoScale(val);
	dataWidget->updateGL();
}

void
MeshMainWindow::toggleShader(bool val)
{
	if (val)
		dataWidget->setShading(GOURAUD);
	else
		dataWidget->setShading(FLAT);
}

void
MeshMainWindow::setPolygonOffset(int val)
{
	dataWidget->setPolygonOffset(val / 10.0);
	dataWidget->updateData();
	dataWidget->updateGL();
}

void
MeshMainWindow::showRotate(double x, double y, double z)		
{
	rotateLabel->setText(" Angles ("  + QString::number(x,'g',3) + " ," 
																	+ QString::number(y,'g',3) + " ,"
																	+ QString::number(z,'g',3) + ")");
}
void
MeshMainWindow::showShift(double x, double y)		
{
	shiftLabel->setText(" Shifts (" + QString::number(x,'g',3) + " ," 
																	+ QString::number(y,'g',3) + " )"
																	);
}
void
MeshMainWindow::showScale(double x, double y, double z)		
{
	scaleLabel->setText(" Scales (" + QString::number(x,'g',3) + " ," 
																	+ QString::number(y,'g',3) + " ,"
																	+ QString::number(z,'g',3) + ")");
}
void
MeshMainWindow::showZoom(double z)		
{
	zoomLabel->setText(" Zoom "  + QString::number(z,'g',3)); 
}

void
MeshMainWindow::showNormals(bool val)
{
	dataWidget->showNormals(val);
	dataWidget->updateNormals();
	dataWidget->updateGL();
}

void
MeshMainWindow::setNormalLength(int val)
{
	dataWidget->setNormalLength(val / 400.);
	dataWidget->updateNormals();
	dataWidget->updateGL();
}

void
MeshMainWindow::setNormalQuality(int val)
{
	dataWidget->setNormalQuality(val);
	dataWidget->updateNormals();
	dataWidget->updateGL();
}

bool
MeshMainWindow::openColorMap(ColorVector& cv, QString fname)
{	
  if (fname.isEmpty())
    return false;
  
  ifstream file(QWT3DLOCAL8BIT(fname));

	if (!file)
		return false;
	
	RGBA rgb;
	cv.clear();
	
	while ( file ) 
	{		
		file >> rgb.r >> rgb.g >> rgb.b;
		file.ignore(1000,'\n');
		if (!file.good())
			break;
		else
		{
			rgb.a = 1;
			rgb.r /= 255;
			rgb.g /= 255;
			rgb.b /= 255;
			cv.push_back(rgb);	
		}
	}

	return true;
}

void 
MeshMainWindow::updateColorLegend(int majors, int minors)
{
	dataWidget->legend()->setMajors(majors);
	dataWidget->legend()->setMinors(minors);
	double start, stop;
	dataWidget->coordinates()->axes[Z1].limits(start,stop);
	dataWidget->legend()->setLimits(start, stop);
}		

void MeshMainWindow::enableLighting(bool val)
{
  dataWidget->enableLighting(val);
  dataWidget->illuminate(0);
  dataWidget->updateGL();
}
