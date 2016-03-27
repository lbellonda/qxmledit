#include <qmainwindow.h>

#include "qwt3d_gridplot.h"
#include "qwt3d_function.h"
#include "qwt3d_plot3d.h"

#include "ui_axesmainwindowbase.h"

//MOC_SKIP_BEGIN
  class DummyBase : public QMainWindow, protected Ui::MainWindow
  {
  public:
    DummyBase(QWidget* parent = 0) 
      : QMainWindow(parent) 
    {
    } 
  };
//MOC_SKIP_END


class AxesMainWindow : public DummyBase
{
	Q_OBJECT

public:
	AxesMainWindow( QWidget* parent = 0);
	~AxesMainWindow();
	Qwt3D::GridPlot* plot;
	Qwt3D::Function *rosenbrock;
  void resetTics();

public slots:
	void setNumberGap(int gap);
	void setLabelGap(int gap);

  void setSmoothLines(bool);
  void setTicLength(int val);
  void setTicNumber(int degree);

  void standardItems();
  void complexItems();
  void letterItems();
  void timeItems();
  void customScale();

private:

  int tics;
};
