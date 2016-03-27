/*************************************************************
 * VolumePlot extension
 *
 * (c) Sintegrial Technologies 2013-2014
 *             http://www.sintegrial.com
 ************************************************************/

#include "qwt3d_volumeplot.h"


namespace Qwt3D
{

// VolumePlot

VolumePlot::VolumePlot(QWidget *parent, const QGLWidget *shareWidget)
    : Plot3D(parent, shareWidget)
{
    plotlets_p[0].data = ValuePtr<Data>(new GraphData);
}

int VolumePlot::createDataset(const Voxel::Array &nodes, bool append)
{
    int ret = prepareDatasetCreation<GraphData>(append);
    if (ret < 0)
        return -1;

    GraphData& data = dynamic_cast<GraphData&>(*plotlets_p[ret].data);

    data.nodes = nodes;

    ParallelEpiped hull(Triple(DBL_MAX,DBL_MAX,DBL_MAX), Triple(-DBL_MAX,-DBL_MAX,-DBL_MAX));

    for (int i = 0; i < nodes.size(); ++i)
    {
        const Voxel& vert = nodes.at(i);
		if (vert.isValid())
		{
			if (vert.x < hull.minVertex.x)
				hull.minVertex.x = vert.x;
			if (vert.y < hull.minVertex.y)
				hull.minVertex.y = vert.y;
			if (vert.z < hull.minVertex.z)
				hull.minVertex.z = vert.z;

			if (vert.x > hull.maxVertex.x)
				hull.maxVertex.x = vert.x;
			if (vert.y > hull.maxVertex.y)
				hull.maxVertex.y = vert.y;
			if (vert.z > hull.maxVertex.z)
				hull.maxVertex.z = vert.z;
		}
    }

    data.setHull(hull);

    updateData();

    createCoordinateSystem();

    return ret;
}

void VolumePlot::createOpenGlData(const Plot3D::Plotlet &pl)
{
    if (pl.appearance->plotStyle() == NOPLOT)
        return;

    const GraphData& data = dynamic_cast<const GraphData&>(*pl.data);

    glBegin(GL_POINTS);

    for (int i = 0; i < data.nodes.size(); ++i)
    {
        const Voxel& vert = data.nodes.at(i);
		if (vert.isValid())
		{
			glColor4ub(vert.r, vert.g, vert.b, vert.a);
			glVertex3f(vert.x, vert.y, vert.z);
		}
    }

    glEnd();
}


// VolumePlot::GraphData

VolumePlot::GraphData::GraphData()
{
    datatype_p = Qwt3D::VOXEL;
    setHull(ParallelEpiped());
}

VolumePlot::GraphData::~GraphData()
{
    setHull(ParallelEpiped());
    nodes.clear();
}

bool VolumePlot::GraphData::empty() const
{
    return nodes.empty();
}

}
