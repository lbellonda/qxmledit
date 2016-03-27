#ifndef freemesh_h__2005_12_23_begin_guarded_code
#define freemesh_h__2005_12_23_begin_guarded_code
	
#include <qevent.h>
#include <qwt3d_meshplot.h>
  

typedef Qwt3D::MeshPlot MPlot;

class Plot : public MPlot
{
  Q_OBJECT

public:
  Plot(QWidget* parent);
  void contextMenuEvent( QContextMenuEvent * );

public slots:
  void showTetrahedron();
  void showCone();
  void openMesh();

private:
  void setSomeThings();
};

#endif /* include guard */
