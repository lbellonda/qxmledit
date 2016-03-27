#include <fstream>

#include <qmetaobject.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qfiledialog.h>       
#include <qstatusbar.h>
#include <qfileinfo.h>
#include <qslider.h>
#include <qtimer.h>
#include <qcombobox.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qsignalmapper.h>

#include <qwt3d_io.h>
#include <qwt3d_io_reader.h>

#include "mainwindow.h"

#include "functions.h"
//#include "colormapreader.h"
//#include "configframe.h"

using namespace Qwt3D;
using namespace std;


MainWindow::~MainWindow()      
{
	delete dataWidget;
}

MainWindow::MainWindow( QWidget* parent )       
	: QMainWindow( parent )
{
	setupUi(this);

 // QActionGroup* floorstyle = new QActionGroup(this);
 // floorstyle->addAction(floordata);
 // floorstyle->addAction(flooriso);
 // floorstyle->addAction(floornone);

 QGridLayout *grid = new QGridLayout( frame );

 // col_ = 0;
	//legend_ = false;

 dataWidget = new GridPlot(frame);
 grid->addWidget( dataWidget, 0, 0 );


	//connect( backgroundcolor, SIGNAL( triggered() ), this, SLOT( pickBgColor() ) );
	//connect( floorstyle, SIGNAL( triggered( QAction* ) ), this, SLOT( pickFloorStyle( QAction* ) ) );
	//connect( meshcolor, SIGNAL( triggered() ), this, SLOT( pickMeshColor() ) );
	//connect( numbercolor, SIGNAL( triggered() ), this, SLOT( pickNumberColor() ) );
	//connect( labelcolor, SIGNAL( triggered() ), this, SLOT( pickLabelColor() ) );
	//connect( titlecolor, SIGNAL( triggered() ), this, SLOT( pickTitleColor() ) );
	//connect( datacolor, SIGNAL( triggered() ), this, SLOT( pickDataColor() ) );
	//connect( resetcolor, SIGNAL( triggered() ), this, SLOT( resetColors() ) );
	//connect( openFile, SIGNAL( triggered() ) , this, SLOT( open() ) );
	//

 pmanager["hat"] = PlotletItem(cbHat, cfgHat, 0);
 pmanager["hatn"] = PlotletItem(cbHatn, cfgHatn);
 pmanager["boy"] = PlotletItem(cbBoy, cfgBoy);
 pmanager["ripple"] = PlotletItem(cbRipple, cfgRipple);


 PM_IT i = pmanager.begin();
 while (i != pmanager.end()) 
 {
   connect( i->cbox, SIGNAL( toggled( bool ) ), this, SLOT( togglePlotlet( bool ) ) );
   connect(i->cfgframe, SIGNAL(meshColorChanged()), this, SLOT(setMeshColor()));
   connect(i->cfgframe, SIGNAL(dataColorChanged()), this, SLOT(setDataColor()));
   connect(i->cfgframe, SIGNAL(plotStyleChanged()), this, SLOT(setPlotStyle()));
   ++i;
 }

	//timer = new QTimer( this );
	//connect( timer, SIGNAL(timeout()), this, SLOT(rotate()) );

	//resSlider->setRange(1,70);
	//connect( resSlider, SIGNAL(valueChanged(int)), dataWidget, SLOT(setResolution(int)) );
	//connect( dataWidget, SIGNAL(resolutionChanged(int)), resSlider, SLOT(setValue(int)) );
	//resSlider->setValue(1);             
	//
	//connect( offsSlider, SIGNAL(valueChanged(int)), this, SLOT(setPolygonOffset(int)) );

	connect(normButton, SIGNAL(clicked()), this, SLOT(setStandardView()));  
	//
 // QString qwtstr(" qwtplot3d ");
 // qwtstr += QString::number(QWT3D_MAJOR_VERSION) + ".";
 // qwtstr += QString::number(QWT3D_MINOR_VERSION) + ".";
 // qwtstr += QString::number(QWT3D_PATCH_VERSION) + " ";

	connect(projection, SIGNAL( toggled(bool) ), this, SLOT( toggleProjectionMode(bool)));
	//connect(colorlegend, SIGNAL( toggled(bool) ), this, SLOT( toggleColorLegend(bool)));
	connect(autoscale, SIGNAL( toggled(bool) ), this, SLOT( toggleAutoScale(bool)));
	//connect(normals, SIGNAL( toggled(bool) ), this, SLOT( showNormals(bool)));
	//		
	setStandardView();

	dataWidget->coordinates()->setLineSmooth(true);
	dataWidget->enableMouse(true);
  dataWidget->setKeySpeed(15,20,20);

  dataWidget->setTitleFont( "Arial", 14, QFont::Normal );
  //dataWidget->legend()->setScale(LINEARSCALE);
  for (unsigned i=0; i!=dataWidget->coordinates()->axes.size(); ++i)
  {
    dataWidget->coordinates()->axes[i].setMajors(7);
    dataWidget->coordinates()->axes[i].setMinors(5);
  }
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

  createFunction("hat", false);
  dataWidget->enableLighting(true);
  dataWidget->illuminate(0);
}

int MainWindow::createFunction(QString const& name, bool append /*= true*/)
{
	dataWidget->makeCurrent();
  if (name == QString("hat")) 
  {
    Hat hat(*dataWidget, true);

    hat.setMesh(51,72);
    hat.setDomain(-1.5,1.5,-1.5,1.5);
    hat.create(append);	
  }
  else if (name == QString("hatn")) 
  {
    Hat hat(*dataWidget, false);

    hat.setMesh(51,72);
    hat.setDomain(-1.5,1.5,-1.5,1.5);
    hat.create(append);	
  }
	else if (name == QString("ripple")) 
	{
		Ripple ripple(*dataWidget);
    ripple.setMesh(120,120);
		ripple.create(append);	
	}
  else if (name == QString("boy")) 
  {
    Boy boy(*dataWidget);
    boy.create(append);
  }

  //	updateColorLegend(7,5);

  if (append)
    return (int)(dataWidget->plotlets() - 1);
  
  return 0;
}

void MainWindow::setPlotStyle()
{
  ConfigFrame* w = (ConfigFrame*)sender();
  int pos = findPlotletPosition(w);
  if (pos < 0)
    return;
  dataWidget->appearance(pos).setPlotStyle(w->plotstyle());
  dataWidget->updateData();
  dataWidget->updateGL();
}

void MainWindow::pickFloorStyle( QAction* /*action*/ )
{
	//if (!action || !dataWidget)
	//	return;

	//if (action == floordata)
	//{
	//	dataWidget->setFloorStyle(FLOORDATA);
	//}
	//else if (action == flooriso)
	//{
	//	dataWidget->setFloorStyle(FLOORISO);
	//}
	//else
	//{
	//	dataWidget->setFloorStyle(NOFLOOR);
	//}
	//
	//dataWidget->updateData();
	//dataWidget->updateGL();
}	

void MainWindow::setMeshColor()
{
  ConfigFrame* w = (ConfigFrame*)sender();
  int pos = findPlotletPosition(w);
  if (pos < 0)
    return;
	dataWidget->appearance(pos).setMeshColor(w->meshColor());
	dataWidget->updateData();
	dataWidget->updateGL();
}

void MainWindow::setDataColor()
{
  ConfigFrame* w = (ConfigFrame*)sender();
  int pos = findPlotletPosition(w);
  if (pos < 0)
    return;
  dataWidget->appearance(pos).setDataColor(w->dataColor());
  dataWidget->updateData();
  dataWidget->updateNormals();
  //dataWidget->showColorLegend(legend_);
  dataWidget->updateGL();
}

void MainWindow::setStandardView()
{
	dataWidget->setRotation(30,0,15);
	dataWidget->setViewportShift(0.05,0);
	dataWidget->setScale(1,1,1);
	dataWidget->setZoom(0.95);
}

void MainWindow::toggleProjectionMode(bool val)
{
	dataWidget->setOrtho(val);
}

void MainWindow::toggleColorLegend(bool /*val*/)
{
	//legend_ = val;
	//dataWidget->showColorLegend(val);
}

void MainWindow::toggleAutoScale(bool val)
{
	dataWidget->coordinates()->setAutoScale(val);
	dataWidget->updateGL();
}

void MainWindow::showNormals(bool /*val*/)
{
	//dataWidget->showNormals(val);
	//dataWidget->updateNormals();
	//dataWidget->updateGL();
}

void MainWindow::setNormalLength(int /*val*/)
{
	//dataWidget->setNormalLength(val / 400.);
	//dataWidget->updateNormals();
	//dataWidget->updateGL();
}

void MainWindow::setNormalQuality(int /*val*/)
{
	//dataWidget->setNormalQuality(val);
	//dataWidget->updateNormals();
	//dataWidget->updateGL();
}

void MainWindow::updateColorLegend(int /*majors*/, int /*minors*/)
{
	//dataWidget->legend()->setMajors(majors);
	//dataWidget->legend()->setMinors(minors);
	//double start, stop;
	//dataWidget->coordinates()->axes[Z1].limits(start,stop);
	//dataWidget->legend()->setLimits(start, stop);
}		

void MainWindow::togglePlotlet(bool val)
{
  if (!val) // removing plotlets
  {
    PM_IT i = pmanager.begin();
    if (2 == dataWidget->plotlets()) // we are going to remove the penultimate plotlet...
    {
      // ... so make sure, the last one will be secured
      while (i != pmanager.end()) 
      {
        if (i->cbox->isChecked() && i->cbox != sender())
        {
          i->cbox->setEnabled(false);
          break;
        }       
        ++i;
      }
    }

    // ... remove the plotlet
    int deleted = -1;
    i = pmanager.begin();
    while (i != pmanager.end()) 
    {
      if (sender() == i->cbox)
      {
        dataWidget->removePlotlet(i->pos);
        deleted = i->pos;
        i->pos = -1;
        break;
      }
      ++i;
    }
    
    // ... recalculate indexes
    if (deleted>=0)
    {
      i = pmanager.begin();
      while (i != pmanager.end()) 
      {
        if (i->pos >= deleted)
        {
          --i->pos; // cannot be < 1 prior to decr.
        }      
        ++i;
      }
    }
  }
  else // adding plotlets
  {
    PM_IT i = pmanager.begin();
    while (i != pmanager.end()) 
    {
      // re-enable possible single plotlet checkbox
      if (!i->cbox->isEnabled())
        i->cbox->setEnabled(true);
      if (sender() == i->cbox)
      {
        i->pos = createFunction(i.key());
        if (i->pos>=0)
        {
          dataWidget->appearance(i->pos).setMeshColor(i->cfgframe->meshColor());
          dataWidget->appearance(i->pos).setDataColor(i->cfgframe->dataColor());
          dataWidget->appearance(i->pos).setPlotStyle(i->cfgframe->plotstyle());
          dataWidget->updateData();	
          dataWidget->updateNormals();
       }
      }
      ++i;
    }
  }
  dataWidget->updateGL();
}

int MainWindow::findPlotletPosition( const ConfigFrame* val ) const
{
  PM_CIT i = pmanager.begin();
  while (i != pmanager.end()) 
  {
    if (i->cfgframe == val)
      return i->pos;
    ++i;
  }
  return -1;
}
