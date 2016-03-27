#include <qapplication.h>
#include <qframe.h>
#include <qgridlayout.h>
#include <qmenu.h>
#include <qevent.h>
#include <qcursor.h>
#include <qfiledialog.h>
#include "freemesh.h"
#include "femreader.h"
  
using namespace Qwt3D;

void createTetrahedron(TripleVector& pos, CellVector& poly)
{
  pos.clear(); poly.clear();
  Cell cell(3);
  
  pos.push_back(Triple(0,0,0));
  pos.push_back(Triple(1,0,0));
  pos.push_back(Triple(0.5,1,0));
  pos.push_back(Triple(0.5,0.7,1.5));

  cell[0] = 0; 
  cell[1] = 1; 
  cell[2] = 2; 
  poly.push_back(cell);
  
  cell[0] = 0; 
  cell[1] = 1; 
  cell[2] = 3; 
  poly.push_back(cell);

  cell[0] = 1; 
  cell[1] = 2; 
  cell[2] = 3; 
  poly.push_back(cell);

  cell[0] = 2; 
  cell[1] = 0; 
  cell[2] = 3; 
  poly.push_back(cell); 
}

void createCone(TripleVector& conepos, CellVector& conecell)
{
  conepos.clear(); conecell.clear();
  Cell cell;

  conepos.push_back(Triple(0,0,0));

  const unsigned int count = 17;
  double r1 = 0.7;
  double r2 = 0.9 * r1;

  double h1 = 2;
  double h2 = 1.03 * h1;

  unsigned i;

  Cell c1;
  // outer top border
  for (i=0; i<count; ++i)
  {
    conepos.push_back(Triple(r1*cos(2*i*Qwt3D::PI/count), r1*sin(2*i*Qwt3D::PI/count),h1));
  }
  // inner top border
  for (i=0; i<count; ++i)
  {
    conepos.push_back(Triple(r2*cos(2*i*Qwt3D::PI/count), r2*sin(2*i*Qwt3D::PI/count),h2));
    c1.push_back(count+i+1);
  }
  // top polygon
  conecell.push_back(c1);

  // triangles
  Cell c2(3);
  for (i=1; i<count+1; ++i)
  {
    c2[0] = 0;
    c2[1] = i%count +1;
    c2[2] = i;
    conecell.push_back(c2);
  }
  // quadrangles
  c2 = Cell(4);
  for (i=1; i<count+1; ++i)
  {
    c2[0] = i;
    c2[1] = i%count +1;
    c2[2] = count+i%count +1;
    c2[3] = count+i;
    conecell.push_back(c2);
  }
}

Plot::Plot(QWidget* parent) : MPlot(parent)
{
  setTitle("Check context menu ...");
  showTetrahedron();
}

void Plot::contextMenuEvent( QContextMenuEvent * )
{
  QMenu* contextMenu = new QMenu( this );
  contextMenu->addAction( "&Tetrahedron",  this, SLOT(showTetrahedron()));
  contextMenu->addAction( "&Cone", this, SLOT(showCone()));
  contextMenu->addAction( "&FEM Data", this, SLOT(openMesh()));
  contextMenu->exec( QCursor::pos() );
  delete contextMenu;
}

void Plot::showCone()
{
  TripleVector pos;
  CellVector poly;
  createCone(pos, poly);
  createDataset(pos, poly);
  setSomeThings();
}
void Plot::showTetrahedron()
{
  TripleVector pos;
  CellVector poly;
  createTetrahedron(pos, poly);
  createDataset(pos, poly);    
  setSomeThings();
}

void Plot::setSomeThings()
{
  setZoom(0.8);
  setScale(1,1,1);
  setRotation(30,0,15);
  for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
  {
    coordinates()->axes[i].setMajors(4);
    coordinates()->axes[i].setMinors(5);
  }
}

void Plot::openMesh()
{
  QString data( QFileDialog::getOpenFileName( this, "", "../../data", "nodes (*.nod)") );
  QString edges( QFileDialog::getOpenFileName( this, "", "../../data", "connectivities (*.cel)") );

  if ( data.isEmpty() || edges.isEmpty())
    return;


  TripleVector vdata;
  CellVector vpoly;

  readNodes(vdata, QWT3DLOCAL8BIT(data), NodeFilter());
  readConnections(vpoly, QWT3DLOCAL8BIT(edges), CellFilter());

  createDataset(vdata, vpoly);

  for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
  {
    coordinates()->axes[i].setMajors(4);
    coordinates()->axes[i].setMinors(5);
    coordinates()->axes[i].setLabelString(QString(""));
  }
  setScale(1,1,5);
}


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  QFrame frame;
  QGridLayout* layout = new QGridLayout(&frame);
   
  Plot plot(&frame);
  layout->addWidget(&plot,0,0);
  frame.resize(800,600);
  frame.show();
  return a.exec();
}
