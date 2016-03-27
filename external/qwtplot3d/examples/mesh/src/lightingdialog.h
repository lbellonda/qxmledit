#pragma once
#ifndef lightingdialog_h__2010_02_18_13_55_34_begin_guarded_code
#define lightingdialog_h__2010_02_18_13_55_34_begin_guarded_code

#include <math.h>
//#include <qapplication.h>
#include "qwt3d_parametricsurface.h"
#include "qwt3d_gridplot.h"
#include "qwt3d_enrichment.h"
#include "qwt3d_color.h"

#include "ui_lightingdialogbase.h"

class Pointer : public Qwt3D::UserEnrichment
{
public:
  explicit Pointer(double rad);

  Qwt3D::Enrichment* clone() const {return new Pointer(*this);}
  
  void configure(double rad);
  void draw();
  void setPos(double x, double y, double z) {pos_ = Qwt3D::Triple(x,y,z);}

private:
  double radius_;
  Qwt3D::Triple pos_;
};

struct SColor : public Qwt3D::Color
{
  Qwt3D::RGBA operator()(double, double, double) const {return Qwt3D::RGBA(0.8,0,0,0.5);}
};

typedef Qwt3D::GridPlot SPlot; // moc/VC6 issue in Qt4

class Plot : public SPlot
{
  Q_OBJECT
    
public:
  Plot(QWidget* parent);
  Pointer* stick;
  void reset();
};

class LightingDlg : public QDialog, protected Ui::Dialog
{
	Q_OBJECT
    
public:
  LightingDlg(QWidget *parent=0);
  ~LightingDlg();

  void assign(Qwt3D::Plot3D* pl);
  
  Plot* plot;
  Qwt3D::Plot3D* dataPlot;

public slots:
	void setDistance(int);
	void setEmission(int);
	void setDiff(int);
	void setSpec(int);
	void setShin(int);
  void reset();
  void setRotation(double x, double y, double z);
};

    
#endif /* include guarded */
