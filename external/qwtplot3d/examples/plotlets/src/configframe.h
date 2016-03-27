#pragma once
#ifndef configdialog_h__2010_02_18_14_05_33_begin_guarded_code
#define configdialog_h__2010_02_18_14_05_33_begin_guarded_code

#include <qsignalmapper.h>
#include "qwt3d_color_std.h"

#include "ui_configframebase.h"


class ConfigFrame : public QFrame, protected Ui::ConfigFrameBase
{
	Q_OBJECT
    
public:
  ConfigFrame(QWidget *parent=0);
  Qwt3D::RGBA meshColor() const {return meshcolor_;}
  const Qwt3D::StandardColor& dataColor() const {return datacolor_;}
  Qwt3D::PLOTSTYLE plotstyle() const {return plotstyle_;}

signals:
  void meshColorChanged();
  void dataColorChanged();
  void plotStyleChanged();

private slots:
  void slotMeshColor();
  void slotDataColor();
  void slotPlotstyle(const QString& val);

private:
  QSignalMapper *signalmapper_;
  Qwt3D::RGBA meshcolor_;
  Qwt3D::StandardColor datacolor_;
  Qwt3D::PLOTSTYLE plotstyle_;

  bool openColorMap(Qwt3D::ColorVector& cv, QString fname);

};

#endif /* include guarded */
