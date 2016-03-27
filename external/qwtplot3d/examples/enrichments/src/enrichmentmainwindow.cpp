#include <qlayout.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

#include <qwt3d_function.h>

#include "enrichmentmainwindow.h"
#include "enrichments.h"

using namespace Qwt3D;


class Hat : public Function
{
public:

	Hat(GridPlot& pw)
	:Function(pw)
	{
	}
	
	double operator()(double x, double y)
	{
    double ret = 1.0 / (x*x+y*y+0.5);
    return ret;
	}
	QString name() const {return "$\\frac{1}{x^2+y^2+\\frac{1}{2}}$";}
};

Hat* hat;

EnrichmentMainWindow::EnrichmentMainWindow( QWidget* parent )
	: DummyBase( parent )
{
  setupUi(this);
  QGridLayout *grid = new QGridLayout( frame);
  plot = new GridPlot(frame);
  grid->addWidget( plot, 0, 0 );

  plot->setTitle("Bar Style (Vertex Enrichment)");
  plot->setTitleFont("Arial",12);

	plot->setZoom(0.8);
	plot->setRotation(30,0,15);

	plot->setCoordinateStyle(BOX);
  bar = (Bar*)plot->setPlotStyle(Bar(0.007,0.5));

  hat = new Hat(*plot);
	
	hat->setMesh(23,21);
	hat->setDomain(-1.8,1.7,-1.6,1.7);	

	hat->create();

	for (unsigned i=0; i!=plot->coordinates()->axes.size(); ++i)
  {
    plot->coordinates()->axes[i].setMajors(5);
    plot->coordinates()->axes[i].setMinors(4);
  }


	//plot->setMeshLineWidth(1);
	plot->coordinates()->setGridLinesColor(RGBA(0,0,0.5));
	plot->coordinates()->setLineWidth(1);
	plot->coordinates()->setNumberFont("Courier",8);
  plot->coordinates()->adjustNumbers(5);
 
	setColor();
  plot->updateData();
  plot->updateGL();


  levelSlider->setValue(50);
	level_ = 0.5;
  width_ = 0.007;
  connect( levelSlider, SIGNAL(valueChanged(int)), this, SLOT(setLevel(int)) );

//  connect( barBtn, SIGNAL(clicked()), this, SLOT(barSlot()) );
  //connect( sliceBtn, SIGNAL(clicked()), this, SLOT(sliceSlot()) );

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LINE_SMOOTH);
}

EnrichmentMainWindow::~EnrichmentMainWindow()
{
	delete hat;
}


void EnrichmentMainWindow::setColor()
{	
  Qwt3D::ColorVector cv;
	
	RGBA rgb;
	int i = 252;
	int step = 4;

  while (i>=0)
  {    
    rgb.r = i/255.; rgb.g=(i-60>0) ? (i-60)/255.:0;rgb.b=0;
  //  rgb.a = 0.2;
    cv.push_back(rgb);	
    if (!--step) 
    {
      i-=4;
      step=4;
    }
  }
	StandardColor col;
	col.setColorVector(cv);
	
	plot->setDataColor(col);
}


void EnrichmentMainWindow::setLevel(int i)
{
  level_ = 1 - i / 100.;
  bar->configure(width_,level_);
  plot->updateData();
  plot->updateGL();
}

void EnrichmentMainWindow::setWidth(int i)
{
  width_ = i / 20000.;
  bar->configure(width_,level_);
  plot->updateData();
  plot->updateGL();
}

void EnrichmentMainWindow::barSlot()
{  
  Bar b(width_,level_); 
  bar = (Bar*)plot->setPlotStyle(b);
  plot->updateData();
  plot->updateGL();
}
