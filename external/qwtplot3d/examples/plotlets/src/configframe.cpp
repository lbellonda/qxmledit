#include <fstream>
#include "qcolordialog.h"
#include "qfiledialog.h"
#include "configframe.h"

using namespace std;
using namespace Qwt3D;

ConfigFrame::ConfigFrame(QWidget *parent)
:QFrame(parent)
{
  setupUi(this);

  meshcolor_ = RGBA(0,0,0);
  datacolor_ = StandardColor(100);
  plotstyle_ = FILLEDMESH;


  connect(meshcolorBtn, SIGNAL( clicked() ), this, SLOT( slotMeshColor()));
  connect(datacolorBtn, SIGNAL( clicked() ), this, SLOT( slotDataColor()));

  signalmapper_ = new QSignalMapper(this);

  connect(pointsBtn, SIGNAL(clicked()), signalmapper_, SLOT(map()));
  signalmapper_->setMapping(pointsBtn, "points");

  connect(wireframeBtn, SIGNAL(clicked()), signalmapper_, SLOT(map()));
  signalmapper_->setMapping(wireframeBtn, "wireframe");

  connect(hiddenlineBtn, SIGNAL(clicked()), signalmapper_, SLOT(map()));
  signalmapper_->setMapping(hiddenlineBtn, "hiddenline");

  connect(polygonBtn, SIGNAL(clicked()), signalmapper_, SLOT(map()));
  signalmapper_->setMapping(polygonBtn, "polygons");

  connect(filledmeshBtn, SIGNAL(clicked()), signalmapper_, SLOT(map()));
  signalmapper_->setMapping(filledmeshBtn, "filledmesh");

  connect(nodataBtn, SIGNAL(clicked()), signalmapper_, SLOT(map()));
  signalmapper_->setMapping(nodataBtn, "nodata");

  connect(signalmapper_, SIGNAL(mapped(const QString &)), this, SLOT(slotPlotstyle(const QString &)));
}

void ConfigFrame::slotMeshColor()
{
  QColor c = QColorDialog::getColor( Qt::white, this );
  if ( !c.isValid() )
    return;
  meshcolor_ = Qt2GL(c);
  emit meshColorChanged();
}

void ConfigFrame::slotDataColor()
{
  QString filename =  QFileDialog::getOpenFileName( this, "", ".", "Colormap files (*.map *.MAP)");

  ColorVector cv;
  
  if (!openColorMap(cv, filename))
  	return;
  
  datacolor_.setColorVector(cv);
  emit dataColorChanged();
}

void ConfigFrame::slotPlotstyle( const QString& val )
{
  if (val=="points")
    plotstyle_ = Qwt3D::POINTS;
  else if (val=="wireframe")
    plotstyle_ = Qwt3D::WIREFRAME;
  else if (val=="hiddenline")
    plotstyle_ = Qwt3D::HIDDENLINE;
  else if (val=="polygons")
    plotstyle_ = Qwt3D::FILLED;
  else if (val=="filledmesh")
    plotstyle_ = Qwt3D::FILLEDMESH;
  else if (val=="nodata")
    plotstyle_ = Qwt3D::NOPLOT;

  emit plotStyleChanged();
}

bool ConfigFrame::openColorMap(ColorVector& cv, QString fname)
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