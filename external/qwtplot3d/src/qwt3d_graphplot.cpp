#if defined(_MSC_VER) /* MSVC Compiler */
/* 'identifier' : truncation from 'type1' to 'type2' */
#pragma warning ( disable : 4305 ) 
#endif


#include "qwt3d_graphplot.h"

using namespace std;
using namespace Qwt3D;

// Data class (private)


GraphPlot::GraphData::GraphData() 
{
  datatype_p=Qwt3D::GRAPH;
  setHull(ParallelEpiped());
}

GraphPlot::GraphData::~GraphData()
{
  setHull(ParallelEpiped());
  nodes.clear();
  edges.clear();
}

bool GraphPlot::GraphData::empty() const
{
  return nodes.empty();
}

// Data class end


GraphPlot::GraphPlot( QWidget * parent, const QGLWidget * shareWidget)
: Plot3D( parent, shareWidget) 
{
  plotlets_p[0].data = ValuePtr<Data>(new GraphData);
}

void GraphPlot::createOpenGlData(const Plotlet& pl)
{
  if (pl.appearance->plotStyle() == NOPLOT)
    return;

  const GraphData& data = dynamic_cast<const GraphData&>(*pl.data);

  //todo
  //if (plotStyle() == Qwt3D::USER)
  //{
  //  if (userplotstyle_p)
  //    drawEnrichment(*userplotstyle_p);
  //  return;
  //}

  // 	setDeviceLineWidth(meshLineWidth());
  //   GLStateBewarer sb(GL_POLYGON_OFFSET_FILL,true);
  // 	glPolygonOffset(polygonOffset(),1.0);
  // 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //	int idx = 0;
  // 	if (plotStyle() != WIREFRAME)
  // 	{
  // 		glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
  // 
  // 		bool hl = (plotStyle() == HIDDENLINE);
  // 		if (hl)
  // 		{
  // 			RGBA col = backgroundRGBAColor();
  // 			glColor4d(col.r, col.g, col.b, col.a);
  // 		}

  /*
  RGBA col(0.8,0,0);    
  glColor4d(col.r, col.g, col.b, col.a);
  glBegin(GL_LINES);
  for (unsigned i=0; i!=data_->edges.size(); ++i)
  {
  Triple& beg = data_->nodes[data_->edges[i].first];
  Triple& end = data_->nodes[data_->edges[i].second];
  glVertex3d( beg.x, beg.y, beg.z );
  glVertex3d( end.x, end.y, end.z );
  //glNormal3d( data_->normals[idx].x, data_->normals[idx].y, data_->normals[idx].z );
  }
  glEnd();

  col = RGBA(0,0,0.5);    
  glColor4d(col.r, col.g, col.b, col.a);
  setDevicePointSize( 4 );
  glBegin(GL_POINTS);
  for (unsigned j=0; j!=data_->nodes.size(); ++j)
  {
  Triple& p = data_->nodes[j];
  glVertex3d( p.x, p.y, p.z );
  }
  glEnd();

  */

  glEnable(GL_POLYGON_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  Stick s((hull().maxVertex-hull().minVertex).length() / 150, 16);
  s.setColor(RGBA(0.5,0.5,0.5)) ;
  for (unsigned i=0; i!=data.edges.size(); ++i)
  {
    const Triple& beg = data.nodes[data.edges[i].first];
    const Triple& end = data.nodes[data.edges[i].second];
    s.draw(beg,end);
  }

  Ball b((hull().maxVertex-hull().minVertex).length() / 50, 32);
  //static bool ff = true;
  //if (ff)
  b.setColor(RGBA(0.5,0,0));    
  //else
  //  b.setColor(RGBA(0,0.5,0));
  //ff = !ff;

  for (unsigned j=0; j!=data.nodes.size(); ++j)
  {
    b.draw(data.nodes[j]);
  }

  //	}
}

/*! 
Convert user defined graph data to internal structure.
See also Qwt3D::TripleVector and Qwt3D::EdgeVector

\param append For append==true the new dataset will be appended. If false (default), all data  will
be replaced by the new data. This includes destruction of possible additional datasets/Plotlets.
\return Index of new entry in dataset array (append == true), 0 (append == false) or -1 for errors
*/
int GraphPlot::createDataset(TripleVector const& nodes, EdgeVector const& edges, bool append /*= false*/)
{	

  int ret = prepareDatasetCreation<GraphData>(append);
  if (ret < 0)
    return -1;

  GraphData& data = dynamic_cast<GraphData&>(*plotlets_p[ret].data);

  data.nodes = nodes;
  data.edges = edges;
  data.setHull(Qwt3D::hull(nodes));
  updateData();
  createCoordinateSystem();

  return ret;
}	
