#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning ( disable : 4305 )
#pragma warning ( disable : 4786 )
#endif

#include "qwt3d_meshplot.h"
#include "qwt3d_enrichment_std.h"

using namespace std;
using namespace Qwt3D;

Triple const& MeshPlot::MeshData::operator()(unsigned cellnumber, unsigned vertexnumber)
{
    return nodes[cells[cellnumber][vertexnumber]];
}

MeshPlot::MeshData::~MeshData()
{
    setHull(ParallelEpiped());
    cells.clear();
    nodes.clear();
    normals.clear();
}


/**
Initializes with dataNormals()==false, NOFLOOR, resolution() == 1
*/
MeshPlot::MeshPlot( QWidget * parent, const QGLWidget * shareWidget)
    : SurfacePlot( parent, shareWidget)
{
    plotlets_p[0].data = ValuePtr<Data>(new MeshData);
}

// ci = cell index
// cv = vertex index in cell ci
void MeshPlot::setColorFromVertex(const Plotlet& pl, int node, bool skip)
{
    if (skip)
        return;

    const MeshData& data = dynamic_cast<const MeshData&>(*pl.data);

    RGBA col = pl.appearance->dataColor()->rgba(
                data.nodes[node].x, data.nodes[node].y, data.nodes[node].z);

    glColor4d(col.r, col.g, col.b, col.a);
}


void MeshPlot::data2Floor(const Plotlet& pl)
{	
    const MeshData& data = dynamic_cast<const MeshData&>(*pl.data);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    double zshift = data.hull().minVertex.z;

    int idx;
    for (unsigned i = 0; i!=data.cells.size(); ++i)
    {
        glBegin(GL_POLYGON);
        for (unsigned j=0; j!=data.cells[i].size(); ++j)
        {
            idx = data.cells[i][j];
            setColorFromVertex(pl, idx);

            const Triple &nodes_idx = data.nodes[idx];
            glVertex3d(nodes_idx.x, nodes_idx.y, zshift);
        }
        glEnd();
    }
}

void MeshPlot::isolines2Floor(const Plotlet& pl)
{
    if (isolines() <= 0)
        return;

    const MeshData& data = dynamic_cast<const MeshData&>(*pl.data);

    double zshift = data.hull().minVertex.z;
    if (delayisolinecalculation_p)
    {
        double step = (data.hull().maxVertex.z - data.hull().minVertex.z) / isolines();
        for (unsigned k = 0; k != isolines(); ++k)
        {
            isolinesZ_p[k] = zshift + k * step;
        }
    }

    TripleVector nodes;
    TripleVector intersection;

    double lambda = 0;

    GLStateBewarer sb2(GL_LINE_SMOOTH, false);

    for (unsigned k = 0; k != isolines(); ++k)
    {
        double val = isolinesZ_p[k];
        if (val > data.hull().maxVertex.z || val < data.hull().minVertex.z)
            continue;

        for (unsigned i=0; i!=data.cells.size(); ++i)
        {
            nodes.clear();
            unsigned cellnodes = data.cells[i].size();
            for (unsigned j=0; j!=cellnodes; ++j)
            {
                nodes.push_back(data.nodes[data.cells[i][j]]);
            }

            double diff = 0;
            for (unsigned m = 0; m!=cellnodes; ++m)
            {
                unsigned mm = (m+1)%cellnodes;
                if ((val>=nodes[m].z && val<=nodes[mm].z) || (val>=nodes[mm].z && val<=nodes[m].z))
                {
                    diff = nodes[mm].z - nodes[m].z;

                    if (isPracticallyZero(diff)) // degenerated
                    {
                        intersection.push_back(nodes[m]);
                        intersection.push_back(nodes[mm]);
                        continue;
                    }

                    lambda =  (val - nodes[m].z) / diff;
                    //intersection.push_back(Triple(nodes[m].x + lambda * (nodes[mm].x-nodes[m].x), nodes[m].y + lambda * (nodes[mm].y-nodes[m].y), val));
                    intersection.push_back(Triple(nodes[m].x + lambda * (nodes[mm].x-nodes[m].x), nodes[m].y + lambda * (nodes[mm].y-nodes[m].y), zshift));
                }
            }
            drawIntersection(intersection, pl.appearance->dataColor()->rgba(nodes[0].x,nodes[0].y,nodes[0].z));
            intersection.clear();
        }
    }
}

void MeshPlot::createNormals(const Plotlet& pl)
{
    if (!normals())
        return;

    const MeshData& data = dynamic_cast<const MeshData&>(*pl.data);
    const Color& color = *pl.appearance->dataColor();

    if (data.nodes.size() != data.normals.size())
        return;
    Arrow arrow;
    arrow.setQuality(normalQuality());

    Triple basev, topv, norm;

    double diag = (data.hull().maxVertex-data.hull().minVertex).length() * normalLength();

    RGBA col;
    arrow.assign(*this);
    arrow.drawBegin();
    for (unsigned i = 0; i != data.normals.size(); ++i)
    {
        basev = data.nodes[i];
        topv = basev + data.normals[i];

        norm = topv-basev;
        norm.normalize();
        norm	*= diag;

        arrow.setTop(basev+norm);
        arrow.setColor(color.rgba(basev.x,basev.y,basev.z));
        arrow.draw(basev);
    }
    arrow.drawEnd();
}

/*! 
Convert user (non-rectangular) mesh based data to internal structure.
See also Qwt3D::TripleVector and Qwt3D::CellVector

\param append For append==true the new dataset will be appended. If false (default), all data  will
be replaced by the new data. This includes destruction of possible additional datasets/Plotlets.
\return Index of new entry in dataset array (append == true), 0 (append == false) or -1 for errors
*/
int MeshPlot::createDataset(TripleVector const& data, CellVector const& poly, bool append /*= false*/)
{	
    int ret = prepareDatasetCreation<MeshData>(append);
    if (ret < 0)
        return -1;

    MeshData& plotdata = dynamic_cast<MeshData&>(*plotlets_p[ret].data);
    plotdata.nodes = data;
    plotdata.cells = poly;
    plotdata.normals = TripleVector(plotdata.nodes.size());

    unsigned i;

    //  normals for the moment
    Triple n, u, v;
    for ( i = 0; i < poly.size(); ++i)
    {
        if (poly[i].size() < 3)
            n = Triple(0,0,0);
        else
        {
            for (unsigned j = 0; j < poly[i].size(); ++j)
            {
                unsigned jj = (j+1) % poly[i].size();
                unsigned pjj = (j) ? j-1 : poly[i].size()-1;
                u = plotdata.nodes[poly[i][jj]]-plotdata.nodes[poly[i][j]];
                v = plotdata.nodes[poly[i][pjj]]-plotdata.nodes[poly[i][j]];
                n = normalizedcross(u,v);
                plotdata.normals[poly[i][j]] += n;
            }
        }
    }
    for ( i = 0; i != plotdata.normals.size(); ++i)
    {
        plotdata.normals[i].normalize();
    }

    plotdata.setHull(Qwt3D::hull(data));

    updateData();
    updateNormals();
    createCoordinateSystem();

    return ret;
}	

void MeshPlot::createOpenGlData(const Plotlet& pl)
{
    createFloorOpenGlData();

    const MeshData& data = dynamic_cast<const MeshData&>(*pl.data);
    const Appearance& app = *pl.appearance;

    if (app.plotStyle() == NOPLOT)
        return;

    for (Appearance::ELCIT it = app.elist.begin(); it!=app.elist.end(); ++it)
    {
        drawEnrichment(pl, **it);
    }

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
    GLStateBewarer sb(GL_POLYGON_OFFSET_FILL,true);
    glPolygonOffset(app.polygonOffset(),1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    int idx = 0;
    if (app.plotStyle() != WIREFRAME)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_QUADS);

        bool hl = (app.plotStyle() == HIDDENLINE);
        if (hl)
        {
            RGBA col = backgroundRGBAColor();
            glColor4d(col.r, col.g, col.b, col.a);
        }

        for (unsigned i=0; i!=data.cells.size(); ++i)
        {
            glBegin(GL_POLYGON);
            for (unsigned j=0; j!=data.cells[i].size(); ++j)
            {
                idx = data.cells[i][j];
                setColorFromVertex(pl, idx, hl);

                const Triple &nodes_idx = data.nodes[idx];
                const Triple &normals_idx = data.normals[idx];

                glVertex3d(nodes_idx.x, nodes_idx.y, nodes_idx.z);
                glNormal3d(normals_idx.x, normals_idx.y, normals_idx.z);
            }
            glEnd();
        }
    }

    if (app.plotStyle() == FILLEDMESH || app.plotStyle() == WIREFRAME || app.plotStyle() == HIDDENLINE)
    {
        glColor4d(app.meshColor().r, app.meshColor().g, app.meshColor().b, app.meshColor().a);
        {
            for (unsigned i=0; i!=data.cells.size(); ++i)
            {
                glBegin(GL_LINE_LOOP);
                for (unsigned j=0; j!=data.cells[i].size(); ++j)
                {
                    idx = data.cells[i][j];

                    const Triple &nodes_idx = data.nodes[idx];
                    glVertex3d(nodes_idx.x, nodes_idx.y, nodes_idx.z);
                }
                glEnd();
            }
        }
    }
}

void MeshPlot::drawEnrichment(const Plotlet& pl, Enrichment& p)
{
    switch(p.type()) {
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
        const MeshData& data = dynamic_cast<const MeshData&>(*pl.data);
        for (unsigned i = 0; i != data.normals.size(); ++i)
            ve->draw(data.nodes[i]);
        ve->drawEnd();
    }
        break;
    case Enrichment::EDGEENRICHMENT:
        break;
    default:
        break; //todo
    }
}

/**
\return Number of cells
*/
int MeshPlot::facets(unsigned idx /*=0*/) const
{
    const MeshData& data = dynamic_cast<const MeshData&>(*plotlets_p[idx].data);
    return int(data.cells.size());
}
