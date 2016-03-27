#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning (disable : 4305)
#pragma warning (disable : 4786)
#endif

#include <QtCore/QElapsedTimer>
#include <QDebug>

#include "qwt3d_gridplot.h"
#include "qwt3d_enrichment_std.h"

using namespace std;
using namespace Qwt3D;


// GridData class

GridPlot::GridData::GridData()
{
    datatype_p = Qwt3D::GRID;
    setSize(0,0);
    setPeriodic(false,false);
}

GridPlot::GridData::GridData(unsigned int columns, unsigned int rows)
{
    datatype_p = Qwt3D::GRID;
    setSize(columns,rows);
    setPeriodic(false,false);
}

int GridPlot::GridData::columns() const 
{ 
    return (int)vertices.size();
}

int GridPlot::GridData::rows() const 
{ 
    return (empty()) ? 0 : (int)vertices[0].size();
}

void GridPlot::GridData::setSize(unsigned int columns, unsigned int rows)
{
    setHull(ParallelEpiped());
    vertices = std::vector<DataColumn>(columns);
    for (unsigned int i=0; i!=vertices.size(); ++i)
    {
        vertices[i] = DataColumn(rows);
    }
    normals = std::vector<DataColumn>(columns);
    for (unsigned int i=0; i!=normals.size(); ++i)
    {
        normals[i] = DataColumn(rows);
    }
}


// CVertexProcessor class

GridPlot::CVertexProcessor::CVertexProcessor()
{
	m_step = 1;
	m_useColorMap = false;
	m_colorData = NULL;
	m_dataWidth = m_dataLength = 0;
	m_data = NULL;
}

void GridPlot::CVertexProcessor::setup(int dataWidth, int dataLength, const GridPlot::GridData& data, int row, int step,
									   bool useColorMap, const Qwt3D::RGBA& fixedColor, const Color* colorData,
									   bool showMesh, const Qwt3D::RGBA& meshColor)
{
	m_dataWidth = dataWidth;
	m_dataLength = dataLength;
	m_data = &data;
	m_row = row;
	m_step = step;
	m_useColorMap = useColorMap;
	m_drawFill = (useColorMap && colorData != NULL) || !useColorMap;
	m_colorData = colorData;
	m_fixedColor = fixedColor;
	m_drawMesh = showMesh;
	m_meshColor = meshColor;
}

void GridPlot::CVertexProcessor::run()
{
	// prepare draw arrays
	m_draw_normals.clear();
	m_draw_vertices.clear();
	m_draw_colors.clear();
	m_drawList.clear();

	if (m_drawFill && m_dataLength*m_dataWidth)
	{
		// reserve() gives up to 10% speedup
		m_draw_normals.reserve(m_dataLength*m_dataWidth*2);
		m_draw_vertices.reserve(m_dataLength*m_dataWidth*2);
		m_draw_colors.reserve(m_dataLength*m_dataWidth*2);
		m_drawList.reserve(m_dataLength*m_dataWidth*2);

		int index = 0;
		int size = 0;

		for (int r = m_row; r < m_row + m_dataLength + 1 - m_step; r += m_step)
		{
			bool stripStarted = false;

			processVertex(stripStarted, r, 0, index, size);
			processVertex(stripStarted, r+m_step, 0, index, size);

			for (int c = m_step; c < m_dataWidth + 1 - m_step; c += m_step)
			{
				processVertex(stripStarted, r, c, index, size);
				processVertex(stripStarted, r+m_step, c, index, size);
			}

			endVertex(index, size);
		}
	}

	// prepare mesh arrays
	m_mesh_vertices.clear();
	m_mesh_colors.clear();
	m_drawMeshList.clear();
	
	if (m_drawMesh && m_dataLength*m_dataWidth)
	{
		int index = 0;
		int size = 0;

		int i, j;

        for (i = 0; i < m_dataWidth; i += m_step)
        {
			bool stripStarted = false;

            for (j = m_row; j < m_row + m_dataLength + 2 - m_step; j += m_step)
            {
				processLineStripVertex(stripStarted, j, i, index, size);
            }

			endLineVertex(index, size);
        }

        for (j = m_row; j < m_row + m_dataLength + 1 - m_step; j += m_step)
        {
			bool stripStarted = false;

            for (i = 0; i < m_dataWidth; i += m_step)
            {
				processLineStripVertex(stripStarted, j, i, index, size);
            }

			endLineVertex(index, size);
		}		
	}
}

void GridPlot::CVertexProcessor::processVertex(bool& stripStarted, int i, int j, int& index, int& size)
{
	const Triple& v = m_data->vertices[j][i];

    if (IS_NAN(v.z))
	{
		if (stripStarted){
			stripStarted = false;
			endVertex(index, size);
		}
	}
	else{
		if (!stripStarted){
			stripStarted = true;
		}

		m_draw_vertices.push_back(v);
		m_draw_normals.push_back(m_data->normals[j][i]);

		if (m_useColorMap){
			m_draw_colors.push_back(m_colorData->rgba(v));
		}
		else{
			m_draw_colors.push_back(m_fixedColor);
		}

		size++;
	}
}

void GridPlot::CVertexProcessor::endVertex(int& index, int& size)
{
	if (size > 2){
		m_drawList.push_back(QPair<int,int>(index, size));
	}

	// next available index
	index = m_draw_vertices.size();
	size = 0;
}

void GridPlot::CVertexProcessor::processLineStripVertex(bool& stripStarted, int i, int j, int& index, int& size)
{
	const Triple& v = m_data->vertices[j][i];

    if (IS_NAN(v.z))
	{
		if (stripStarted){
			stripStarted = false;
			endLineVertex(index, size);
		}
	}
	else{
		if (!stripStarted){
			stripStarted = true;
		}

		m_mesh_vertices.push_back(v);

		m_mesh_colors.push_back(m_meshColor);

		size++;
	}
}

void GridPlot::CVertexProcessor::endLineVertex(int& index, int& size)
{
	if (size >= 2){
		m_drawMeshList.push_back(QPair<int,int>(index, size));
	}

	// next available index
	index = m_mesh_vertices.size();
	size = 0;
}

void GridPlot::CVertexProcessor::paintGL()
{
 	if (!m_draw_vertices.empty())
	{
		Q_ASSERT(m_draw_vertices.size() == m_draw_normals.size());
		Q_ASSERT(m_draw_vertices.size() == m_draw_colors.size());

		glVertexPointer(3, GL_DOUBLE, 0, &m_draw_vertices[0]);
		glNormalPointer(GL_DOUBLE, 0, &m_draw_normals[0]);
		glColorPointer(4, GL_DOUBLE, 0, &m_draw_colors[0]);

		for (unsigned int i = 0; i < m_drawList.size(); i++)
		{
			const QPair<int,int>& p = m_drawList.at(i);
			glDrawArrays(GL_TRIANGLE_STRIP, p.first, p.second);
		}
	}

	// mesh
	if (!m_drawMeshList.empty())
	{
		Q_ASSERT(m_mesh_vertices.size() == m_mesh_colors.size());

		glVertexPointer(3, GL_DOUBLE, 0, &m_mesh_vertices[0]);
		glColorPointer(4, GL_DOUBLE, 0, &m_mesh_colors[0]);

		for (unsigned int i = 0; i < m_drawMeshList.size(); i++)
		{
			const QPair<int,int>& p = m_drawMeshList.at(i);
			glDrawArrays(GL_LINE_STRIP, p.first, p.second);
		}
	}
}


// GridPlot class

/**
	Initializes with dataNormals()==false, NOFLOOR, resolution() == 1
*/
GridPlot::GridPlot(QWidget* parent, const QGLWidget* shareWidget)
    : SurfacePlot(parent, shareWidget),
	m_threadsCount(0),
	m_useThreads(false)
{
    resolution_p = 1;
    plotlets_p[0].data = ValuePtr<Data>(new GridData);
}

void GridPlot::setColorFromVertex(const Color& colorData, const Triple& vertex, RGBA& lastColor, bool skip) const
{
    if (skip)
        return;

    RGBA col = colorData.rgba(vertex);
	if (lastColor != col)
	{ 
		glColor4d(col.r, col.g, col.b, col.a);
		lastColor = col;
	}
}

void GridPlot::createNormals(const Plotlet& pl)
{
    if (!normals())
        return;

    Arrow arrow;
    arrow.setQuality(normalQuality());

    Triple norm;

    int step = resolution();

    const GridData& data = dynamic_cast<const GridData&>(*pl.data);
    const Color& color = *pl.appearance->dataColor();
    double diag = (data.hull().maxVertex-data.hull().minVertex).length() * normalLength();

    arrow.assign(*this);
    arrow.drawBegin();
    for (int i = 0; i <= data.columns() - step; i += step)
    {
        for (int j = 0; j <= data.rows() - step; j += step)
        {
            norm = data.normals[i][j];//topv-basev;
            norm.normalize();
            norm *= diag;

            const Triple &vert = data.vertices[i][j];
            arrow.setTop(vert+norm);
            arrow.setColor(color.rgba(vert.x, vert.y, vert.z));
            arrow.draw(vert);
        }
    }
    arrow.drawEnd();
}

void GridPlot::readIn(GridData& gdata, Triple** data, unsigned int columns, unsigned int rows)
{
    gdata.setSize(columns, rows);

    ParallelEpiped range(Triple(DBL_MAX,DBL_MAX,DBL_MAX), Triple(-DBL_MAX,-DBL_MAX,-DBL_MAX));

    /* fill out the vertex array for the mesh. */
    for (unsigned i = 0; i != columns; ++i)
    {
        for (unsigned j = 0; j != rows; ++j)
        {
            const Triple& val = data[i][j];

            gdata.vertices[i][j] = val;

            if (val.x > range.maxVertex.x)
                range.maxVertex.x = val.x;
            if (val.y > range.maxVertex.y)
                range.maxVertex.y = val.y;

            if (val.x < range.minVertex.x)
                range.minVertex.x = val.x;
            if (val.y < range.minVertex.y)
                range.minVertex.y = val.y;

            if (!IS_NAN(val.z))
			{
				if (val.z < range.minVertex.z)
					range.minVertex.z = val.z;
				if (val.z > range.maxVertex.z)
					range.maxVertex.z = val.z;
			}
        }
    }

    gdata.setHull(range);
}


void GridPlot::readIn(GridData& gdata, double** data, unsigned int columns, unsigned int rows, 
					  double minx, double maxx, double miny, double maxy)
{
    gdata.setPeriodic(false,false);
    gdata.setSize(columns,rows);

    double dx = (maxx - minx) / (gdata.columns() - 1);
    double dy = (maxy - miny) / (gdata.rows() - 1);

    double tmin = DBL_MAX;
    double tmax = -DBL_MAX;

    /* fill out the vertex array for the mesh. */
    for (unsigned i = 0; i != columns; ++i)
    {
        for (unsigned j = 0; j != rows; ++j)
        {
            Triple& gdata_ij = gdata.vertices[i][j];
            const double& val = data[i][j];

            gdata_ij.x = minx + i*dx;
            gdata_ij.y = miny + j*dy;
			gdata_ij.z = val;

            if (!IS_NAN(val))
			{
				if (val > tmax)
					tmax = val;
				if (val < tmin)
					tmin = val;
			}
        }
    }

    ParallelEpiped hull =
            ParallelEpiped(
                Triple(
                    gdata.vertices[0][0].x,
					gdata.vertices[0][0].y,
					tmin
					),
				Triple(
					gdata.vertices[gdata.columns()-1][gdata.rows()-1].x,
					gdata.vertices[gdata.columns()-1][gdata.rows()-1].y,
					tmax
					)
           );

    gdata.setHull(hull);
}


void GridPlot::calcNormals(GridData& gdata)
{
    int rows = gdata.rows();
    int columns = gdata.columns();

	// fast normals calculation: all the normals = 0,0,1
	if (m_fastNormals)
	{
		for (int i = 0; i < columns; ++i)
		{
			for (int j = 0; j < rows; ++j)
			{
				Triple& n = gdata.normals[i][j];
				n.x = n.y = 0;
				n.z = 1;
			}
		}

		return;
	}


	// precise normals calculation (slowly)
	Triple u, v, c;  // for cross product

	for (int i = 0; i < columns; ++i)
    {
        for (int j = 0; j < rows; ++j)
        {
			Triple& n = gdata.normals[i][j];
			n.reset();

            const Triple& vert_ij = gdata.vertices[i][j];
            if (IS_NAN(vert_ij.z))
			{
				continue;
			}

            if (i<columns-1 && j<rows-1)
            {
				const Triple& vert_ip1_j = gdata.vertices[i+1][j];
				const Triple& vert_i_jp1 = gdata.vertices[i][j+1];

                if (!IS_NAN(vert_i_jp1.z) && !IS_NAN(vert_i_jp1.z))
				{
					u = vert_ip1_j - vert_ij;
					v = vert_i_jp1 - vert_ij;
					c.crossProduct(u, v);
					n += c; // right hand system here !
				}
            }

            if (i>0 && j<rows-1)
            {
				const Triple& vert_i_jp1 = gdata.vertices[i][j+1];
				const Triple& vert_im1_j = gdata.vertices[i-1][j];

                if (!IS_NAN(vert_i_jp1.z) && !IS_NAN(vert_im1_j.z))
				{
					u = vert_i_jp1 - vert_ij;
					v = vert_im1_j - vert_ij;
					c.crossProduct(u, v);
					n += c;
				}
            }

            if (i>0 && j>0)
            {
				const Triple& vert_i_jm1 = gdata.vertices[i][j-1];
				const Triple& vert_im1_j = gdata.vertices[i-1][j];

                if (!IS_NAN(vert_im1_j.z) && !IS_NAN(vert_i_jm1.z))
				{
					u = vert_im1_j - vert_ij;
					v = vert_i_jm1 - vert_ij;
					c.crossProduct(u, v);
					n += c;
				}
            }

            if (i<columns-1 && j>0)
            {
				const Triple& vert_i_jm1 = gdata.vertices[i][j-1];
				const Triple& vert_ip1_j = gdata.vertices[i+1][j];

                if (!IS_NAN(vert_i_jm1.z) && !IS_NAN(vert_ip1_j.z))
				{
					u = vert_i_jm1 - vert_ij;
					v = vert_ip1_j - vert_ij;
					c.crossProduct(u, v);
					n += c;
				}
            }

            n.normalize();
        }
    }
}


void GridPlot::sewPeriodic(GridData& gdata)
{
    // sewing
    Triple n;

    unsigned int columns = gdata.columns();
    unsigned int rows = gdata.rows();

    if (gdata.uperiodic())
    {
        for (unsigned i = 0; i != columns; ++i)
        {
            n = gdata.normals[i][0] + gdata.normals[i][rows-1];
            n.normalize();
            gdata.normals[i][0] = gdata.normals[i][rows-1] = n;
        }
    }

    if (gdata.vperiodic())
    {
        for (unsigned j = 0; j != rows; ++j)
        {
            n = gdata.normals[0][j] + gdata.normals[columns-1][j];
            n.normalize();
            gdata.normals[0][j] = gdata.normals[columns-1][j] = n;
        }
    }
}

/*!
  Convert user grid data to internal vertex structure.
  See also NativeReader::operator() and Function::create()

  \param append For append==true the new dataset will be appended. If false (default), all data  will
  be replaced by the new data. This includes destruction of possible additional datasets/Plotlets.
  \return Index of new entry in dataset array (append == true), 0 (append == false) or -1 for errors
  */
int GridPlot::createDataset(Triple** data, unsigned int columns, unsigned int rows, 
                            bool uperiodic /*=false*/, bool vperiodic /*=false*/, bool append /*= false*/)
{
	// block possible active threads
	m_useThreads = false;

    int ret = prepareDatasetCreation<GridData>(append);
    if (ret < 0)
        return -1;

    GridData& plotdata = dynamic_cast<GridData&>(*plotlets_p[ret].data);
    readIn(plotdata, data, columns, rows);
    calcNormals(plotdata);
    plotdata.setPeriodic(uperiodic,vperiodic);
    sewPeriodic(plotdata);

    updateData();
    updateNormals();
    createCoordinateSystem();

    return ret;
}	

/*! 
  Convert user grid data to internal vertex structure.
  See also NativeReader::operator() and Function::create()

  \param append For append==true the new dataset will be appended. If false (default), all data  will
  be replaced by the new data. This includes destruction of possible additional datasets/Plotlets.
  \return Index of new entry in dataset array (append == true), 0 (append == false) or -1 for errors
  */
int GridPlot::createDataset(double** data, unsigned int columns, unsigned int rows,
                            double minx, double maxx, double miny, double maxy, bool append /*= false*/)
{	
	// block possible active threads
	m_useThreads = false;

    int ret = prepareDatasetCreation<GridData>(append);
    if (ret < 0)
        return -1;

    GridData& plotdata = dynamic_cast<GridData&>(*plotlets_p[ret].data);
    plotdata.setPeriodic(false,false);
    plotdata.setSize(columns,rows);

//	QElapsedTimer timer;
//	timer.start();

    readIn(plotdata,data,columns,rows,minx,maxx,miny,maxy);

//	qDebug() << "GridPlot::readIn(): " << timer.elapsed();
//	timer.restart();

	calcNormals(plotdata);

//	qDebug() << "GridPlot::calcNormals(): " << timer.elapsed();
//	timer.restart();

	updateData();

//	qDebug() << "GridPlot::createDataset()" << timer.elapsed();

	updateNormals();
    createCoordinateSystem();

    return ret;
}	

void GridPlot::data2Floor(const Plotlet& pl)
{
    int step = resolution();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

	RGBA lastColor;
	Color& colorData = *(pl.appearance->dataColor());

    const GridData& data = dynamic_cast<const GridData&>(*pl.data);
    double zshift = data.hull().minVertex.z;
    for (int i = 0; i < data.columns() - step; i += step)
    {
        glBegin(GL_TRIANGLE_STRIP);

        const Triple& vert1 = data.vertices[i][0];
        setColorFromVertex(colorData, vert1, lastColor);
        glVertex3d(vert1.x, vert1.y, zshift);

        const Triple& vert2 = data.vertices[i+step][0];
        setColorFromVertex(colorData, vert2, lastColor);
        glVertex3d(vert2.x, vert2.y, zshift);

        for (int j = 0; j < data.rows() - step; j += step)
        {
            const Triple& vert1 = data.vertices[i][j+step];
            setColorFromVertex(colorData, vert1, lastColor);
            glVertex3d(vert1.x, vert1.y, zshift);

            const Triple& vert2 = data.vertices[i+step][j+step];
            setColorFromVertex(colorData, vert2, lastColor);
            glVertex3d(vert2.x, vert2.y, zshift);
        }

        glEnd();
    }
}

void GridPlot::isolines2Floor(const Plotlet& pl)
{
    if (!isolines())
        return;

    const GridData& data = dynamic_cast<const GridData&>(*pl.data);
    const Color& color = *pl.appearance->dataColor();

    double zshift = data.hull().minVertex.z;
    if (delayisolinecalculation_p)
    {
        double step = (data.hull().maxVertex.z - data.hull().minVertex.z) / isolines();
        for (unsigned k = 0; k != isolines(); ++k)
        {
            isolinesZ_p[k] = zshift + k * step;
        }
    }

    int step = resolution();
    int cols = data.columns();
    int rows = data.rows();

    Triple t[4];
    vector<Triple> intersection;

    double lambda = 0;

    GLStateBewarer sb2(GL_LINE_SMOOTH, false);

    for (unsigned k = 0; k != isolines(); ++k)
    {
        double val = isolinesZ_p[k];
        if (val > data.hull().maxVertex.z || val < data.hull().minVertex.z)
            continue;
        
        for (int i = 0; i < cols-step; i += step)
        {
            for (int j = 0; j < rows-step; j += step)
            {
                t[0] =  data.vertices[i][j];
                t[1] =  data.vertices[i+step][j];
                t[2] =  data.vertices[i+step][j+step];
                t[3] =  data.vertices[i][j+step];

                double diff = 0;
                for (int m = 0; m!=4; ++m)
                {
                    int mm = (m+1)%4;
                    if ((val>=t[m].z && val<=t[mm].z) || (val>=t[mm].z && val<=t[m].z))
                    {
                        diff = t[mm].z - t[m].z;

                        if (isPracticallyZero(diff)) // degenerated
                        {
                            intersection.push_back(t[m]);
                            intersection.push_back(t[mm]);
                            continue;
                        }

                        lambda =  (val - t[m].z) / diff;
                        intersection.push_back(Triple(t[m].x + lambda * (t[mm].x-t[m].x), t[m].y + lambda * (t[mm].y-t[m].y), zshift));
                    }
                }

                drawIntersection(intersection, color.rgba(t[0].x,t[0].y,t[0].z));
                intersection.clear();
            }
        }
    }
}

/*!
  Sets data resolution (res == 1 original resolution) and updates widget
  If res < 1, the function does nothing
*/
void GridPlot::setResolution(int res)
{
    if (resolution_p == res || res < 1)
        return;

    resolution_p = res;
    updateData();
    updateNormals();
    if (initializedGL())
        updateGL();

    emit resolutionChanged(res);
}

void GridPlot::createOpenGlData(const Plotlet& pl)
{
	//QElapsedTimer timer;
	//timer.start();

	m_useThreads = false;

    createFloorOpenGlData();

    const GridData& data = dynamic_cast<const GridData&>(*pl.data);
    const Appearance& app = *pl.appearance;

    if (app.plotStyle() == NOPLOT)
        return;

    for (Appearance::ELCIT it = app.elist.begin(); it!=app.elist.end(); ++it)
    {
        drawEnrichment(pl, **it);
    }

    int i, j;
    int step = resolution();

	Color& colorData = *(pl.appearance->dataColor());
    RGBA col = backgroundRGBAColor();

    if (app.plotStyle() == Qwt3D::POINTS)
    {
        Dot dot;
        drawEnrichment(pl, dot);
        return;
    }
    else if (app.plotStyle() == Qwt3D::USER)
    {
        if (app.userStyle())
            drawEnrichment(pl, *app.userStyle());
        return;
    }

    setLineWidth(app.meshLineWidth());

    GLStateBewarer sb(GL_POLYGON_OFFSET_FILL, true);
    glPolygonOffset(app.polygonOffset(), 1.0);

    GLStateBewarer sb2(GL_LINE_SMOOTH, app.smoothDataMesh());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int lastcol = data.columns();
    int lastrow = data.rows();

	bool drawMesh = (app.plotStyle() == FILLEDMESH || app.plotStyle() == WIREFRAME || app.plotStyle() == HIDDENLINE);
	bool drawFill = (app.plotStyle() != WIREFRAME);
    bool hl = (app.plotStyle() == HIDDENLINE);

	// older list-based render
	if (m_threadsCount <= 0)
	{
		if (drawFill)
		{
	        glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);
	
			if (hl)
				glColor4d(col.r, col.g, col.b, col.a);

            for (i = 0; i < lastcol - step; i += step)
			{
				bool stripStarted = true;
				glBegin(GL_TRIANGLE_STRIP);

				const Triple& norm1 = data.normals[i][0];
				const Triple& vert1 = data.vertices[i][0];
				processVertex(vert1, norm1, colorData, hl, stripStarted, col);

				const Triple& norm2 = data.normals[i+step][0];
				const Triple& vert2 = data.vertices[i+step][0];
				processVertex(vert2, norm2, colorData, hl, stripStarted, col);

				for (j = 0; j < lastrow - step; j += step)
				{
					const Triple& norm1 = data.normals[i][j+step];
					const Triple& vert1 = data.vertices[i][j+step];
					processVertex(vert1, norm1, colorData, hl, stripStarted, col);

					const Triple& norm2 = data.normals[i+step][j+step];
					const Triple& vert2 = data.vertices[i+step][j+step];
					processVertex(vert2, norm2, colorData, hl, stripStarted, col);
				}

				glEnd();
			}
		}

		if (drawMesh)
		{
			glColor4d(app.meshColor().r, app.meshColor().g, app.meshColor().b, app.meshColor().a);

			if (step < data.columns() && step < data.rows())
			{
				bool stripStarted = true;
				glBegin(GL_LINE_LOOP);

				for (i = 0; i < data.columns() - step; i += step)
				{
					processLineLoopVertex(data.vertices[i][0], stripStarted);
				}

				for (j = 0; j < data.rows() - step; j += step)
				{
					processLineLoopVertex(data.vertices[i][j], stripStarted);
				}

				for (; i >= 0; i -= step)
				{
					processLineLoopVertex(data.vertices[i][j], stripStarted);
				}

				for (; j >= 0; j -= step)
				{
					processLineLoopVertex(data.vertices[0][j], stripStarted);
				}

				glEnd();
			}

			// weaving
			for (i = step; i < data.columns() - step; i += step)
			{
				bool stripStarted = true;
				glBegin(GL_LINE_STRIP);

				for (j = 0; j < data.rows(); j += step)
				{
					processLineStripVertex(data.vertices[i][j], stripStarted);
				}

				glEnd();
			}

			for (j = step; j < data.rows() - step; j += step)
			{
				bool stripStarted = true;
				glBegin(GL_LINE_STRIP);

				for (i = 0; i < data.columns(); i += step)
				{
					processLineStripVertex(data.vertices[i][j], stripStarted);
				}

				glEnd();
			}		
		}
	}
	else // new glDrawArrays-based render
	{
		m_useThreads = true;

		int length = lastrow / m_threadsCount;
		int r = 0;

		for (int i = 0; i < m_threadsCount; i++)
		{
			// if last one
			if (i == m_threadsCount-1)
				length += lastrow - (length * m_threadsCount) - 1;

			m_workers[i].setup(lastcol, length, data, r, step, 
				!hl, col, drawFill ? &colorData : NULL, 
				drawMesh, app.meshColor());

			m_workers[i].start(QThread::HighPriority);

			r += length;
		}

		// wait while threads are done
		for (int i = 0; i < m_threadsCount; i++)
			while (m_workers[i].isRunning());
	}

	//qDebug() << "GridPlot::createOpenGlData(): " << timer.elapsed();
}

void GridPlot::drawOpenGlData()
{
	if (m_useThreads)
	{
		glEnable(GL_DEPTH_TEST);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		for (int i = 0; i < m_threadsCount; i++)
			m_workers[i].paintGL();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	SurfacePlot::drawOpenGlData();
}

void GridPlot::setRenderThreadsCount(int count)
{
	if (count < 1)
		m_threadsCount = 0;
	else if (count > 10)
		m_threadsCount = 10;
	else
		m_threadsCount = count;
}

void GridPlot::processVertex(const Triple& vert1, const Triple& norm1, 
							 const Color& colorData, bool hl, bool& stripStarted,
							 RGBA& lastColor) const
{
    static Triple lastVertex;

    if (IS_NAN(vert1.z))
	{
		if (stripStarted){
			stripStarted = false;

			// degenerated triangle
            glVertex3d(lastVertex.x, lastVertex.y, lastVertex.z);
            glVertex3d(lastVertex.x, lastVertex.y, lastVertex.z);
            //glEnd();
		}
	}
	else{
		if (!stripStarted){
			stripStarted = true;
            
            // degenerated triangle
			glVertex3d(vert1.x, vert1.y, vert1.z);
            glVertex3d(vert1.x, vert1.y, vert1.z);
			//glBegin(GL_TRIANGLE_STRIP);
        }

        setColorFromVertex(colorData, vert1, lastColor, hl);

        glNormal3d(norm1.x, norm1.y, norm1.z);

		glVertex3d(vert1.x, vert1.y, vert1.z);

        lastVertex = vert1;
	}
}

void GridPlot::processLineLoopVertex(const Triple& vert1, bool& stripStarted) const
{
    if (IS_NAN(vert1.z))
	{
		if (stripStarted){
			stripStarted = false;
			glEnd();
		}
	}
	else{
		if (!stripStarted){
			stripStarted = true;
			glBegin(GL_LINE_LOOP);
		}

		glVertex3d(vert1.x, vert1.y, vert1.z);
	}
}

void GridPlot::processLineStripVertex(const Triple& vert1, bool& stripStarted) const
{
    if (IS_NAN(vert1.z))
	{
		if (stripStarted){
			stripStarted = false;
			glEnd();
		}
	}
	else{
		if (!stripStarted){
			stripStarted = true;
			glBegin(GL_LINE_STRIP);
		}

		glVertex3d(vert1.x, vert1.y, vert1.z);
	}
}

void GridPlot::drawEnrichment(const Plotlet& pl, Enrichment& p)
{
    switch(p.type())
    {
        case Enrichment::USERENRICHMENT:
        {
            p.assign(*this);
            UserEnrichment* ue = (UserEnrichment*)&p;
            ue->drawBegin();
            ue->draw();
            ue->drawEnd();
        }
			break;

		case Enrichment::VERTEXENRICHMENT:
		{
			p.assign(*this);
			VertexEnrichment* ve = (VertexEnrichment*)&p;
			ve->drawBegin();
			const GridData& data = dynamic_cast<const GridData&>(*pl.data);
			int step = resolution();
			for (int i = 0; i <= data.columns() - step; i += step)
				for (int j = 0; j <= data.rows() - step; j += step)
					ve->draw(data.vertices[i][j]);
			ve->drawEnd();
		}
			break;

		case Enrichment::EDGEENRICHMENT:
			break;

		default:
			break; //todo
    }
}
