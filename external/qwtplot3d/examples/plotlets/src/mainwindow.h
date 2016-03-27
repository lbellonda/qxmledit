#ifndef mesh2mainwindow_h__2004_03_07_13_38_begin_guarded_code
#define mesh2mainwindow_h__2004_03_07_13_38_begin_guarded_code

#include <qwt3d_gridplot.h>

#include "ui_mainwindowbase.h"

class QLabel;
class QTimer;
class QAction;
class QFileDialog;
class ConfigFrame;
class ColorMapPreview;

class MainWindow : public QMainWindow, protected Ui::MainWindow
{
	Q_OBJECT
public:
	MainWindow( QWidget* parent = 0 );
	~MainWindow();

  Qwt3D::GridPlot* dataWidget;
	
public slots:
	int createFunction(QString const& name, bool append = true);
	void setPlotStyle();
	void pickFloorStyle( QAction* );
	void setMeshColor();
	void setDataColor();
 	void setStandardView();
	void toggleProjectionMode(bool);
	void toggleColorLegend(bool);
	void toggleAutoScale(bool val);

	void showNormals(bool val);
	void setNormalQuality(int);
	void setNormalLength(int);
	void updateColorLegend(int majors, int minors);

  void togglePlotlet(bool val);

private:
  static const int plotlets = 4;

	QLabel *filenameWidget, *dimWidget,
		*rotateLabel, *shiftLabel, *scaleLabel, *zoomLabel;
	
	QTimer* timer;
	int redrawWait;

	QAction* activeCoordSystem;

	bool legend_;
	Qwt3D::StandardColor* col_;

	QFileDialog* datacolordlg_;
  ConfigFrame* lightingdlg_;
  QActionGroup* grids;

  class PlotletItem
  {
  public:
    PlotletItem(QCheckBox* cb = 0, ConfigFrame* cframe = 0, int position = -1) 
      : cbox(cb), cfgframe(cframe), pos(position) 
    {}
    QCheckBox* cbox; // associated checkbox
    ConfigFrame* cfgframe; // config dialog
    int pos; // maintains pos in plotlet vector
  };

  QMap<QString, PlotletItem> pmanager;
  typedef QMap<QString, PlotletItem>::const_iterator PM_CIT;
  typedef QMap<QString, PlotletItem>::iterator PM_IT;

  int findPlotletPosition(const ConfigFrame* val) const;
};

#endif /* include guarded */
