  #include <qapplication.h>
  #include <qwt3d_graphplot.h>
  
  using namespace Qwt3D;

  class Plot : public GraphPlot
  {
  public:
    Plot();
    void create();

    void createCube(double length, const Triple& shift);
    void createCubic1();
    void createCubic2();
    void createCubic3();

    TripleVector nodes;
    EdgeVector edges;
  };

  Plot::Plot()
  {
    setRotation(30,0,15);
    setScale(1,1,1);
    setShift(0.15,0,0);
    setZoom(0.9);
    //setBackgroundColor(RGBA(0,0,0));
    //setTitleColor(RGBA(1,1,1));

    for (unsigned i=0; i!=coordinates()->axes.size(); ++i)
    {
      coordinates()->axes[i].setMajors(7);
      coordinates()->axes[i].setMinors(4);
    }

    setCoordinateStyle(NOCOORD);

    updateData();
    updateGL();
  }

  void Plot::createCube(double length, const Triple& shift)
  {
    
    unsigned m = nodes.size();
    nodes.push_back(Triple(0,0,0)+shift);
    nodes.push_back(Triple(0,0,length)+shift);
    nodes.push_back(Triple(0,length,length)+shift);
    nodes.push_back(Triple(0,length,0)+shift);
    nodes.push_back(Triple(length,0,0)+shift);
    nodes.push_back(Triple(length,0,length)+shift);
    nodes.push_back(Triple(length,length,length)+shift);
    nodes.push_back(Triple(length,length,0)+shift);

    edges.push_back(Edge(m+0, m+1));
    edges.push_back(Edge(m+1, m+2));
    edges.push_back(Edge(m+2, m+3));
    edges.push_back(Edge(m+3, m+0));

    unsigned n = 4; 

    edges.push_back(Edge(m+0+n, m+1+n));
    edges.push_back(Edge(m+1+n, m+2+n));
    edges.push_back(Edge(m+2+n, m+3+n));
    edges.push_back(Edge(m+3+n, m+0+n));

    edges.push_back(Edge(m+0, m+0+n));
    edges.push_back(Edge(m+1, m+1+n));
    edges.push_back(Edge(m+2, m+2+n));
    edges.push_back(Edge(m+3, m+3+n));
  }
  
  void Plot::createCubic1()
  {
    unsigned xs = 2;
    unsigned ys = xs;
    unsigned zs = xs;
    double stretch = 1.3;      

    unsigned i,j,k;
    for (i=0; i!=xs; ++i)
      for (j=0; j!=ys; ++j)
        for (k=0; k!=zs; ++k)
        {
          //if (!(i==1 && j==1 && k==1))
          createCube(1, Triple(i,j,k) * stretch);
        }
        createDataset(nodes, edges);
  }

  void Plot::createCubic2()
  {
    unsigned xs = 2;
    unsigned ys = xs;
    unsigned zs = xs;

    unsigned i,j,k;
    for (i=0; i!=xs; ++i)
      for (j=0; j!=ys; ++j)
        for (k=0; k!=zs; ++k)
          nodes.push_back(Triple(i,j,k));

    for (i=0; i!=nodes.size()-1; ++i)
    {
      edges.push_back(Edge(i,i+1));
    } 
    createDataset(nodes, edges);
  }
  
  void Plot::createCubic3()
  {
    createCube(1, Triple(0,0,0));
    createCube(2, Triple(1,1,1) * (-0.5));
    edges.push_back(Edge(0, 8));
    edges.push_back(Edge(1, 9));
    edges.push_back(Edge(2, 10));
    edges.push_back(Edge(3, 11));
    edges.push_back(Edge(4, 12));
    edges.push_back(Edge(5, 13));
    edges.push_back(Edge(6, 14));
    edges.push_back(Edge(7, 15));
    createDataset(nodes, edges);
  }


  void Plot::create()
  {
    enableLighting();
    //createCubic3();
    //createCubic2();
    createCubic1();
    Triple dim = hull().maxVertex - hull().minVertex;
    setTitle(QString("x length: %1, y length: %2, z length: %3").arg(dim.x).arg(dim.y).arg(dim.z));
    setCoordinateStyle(BOX);
    //setTitle(QString("Nodes: %1, Edges: %2").arg(nodes.size()).arg(edges.size()));
    illuminate(0);
  }

  int main(int argc, char **argv)
  {
    QApplication a(argc, argv);
    Plot plot;

    plot.create();
    plot.resize(800,600);
    plot.show();
    return a.exec();
  }
