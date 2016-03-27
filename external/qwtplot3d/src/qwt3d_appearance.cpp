#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning ( disable : 4305 )
#pragma warning ( disable : 4786 )
#endif

#include <cassert>
#include "qwt3d_enrichment.h"
#include "qwt3d_color_std.h"
#include "qwt3d_appearance.h"

using namespace std;
using namespace Qwt3D;

Qwt3D::Appearance::AppProxy::AppProxy()
{
    meshcolor = RGBA(0.0,0.0,0.0);
    meshLineWidth = 1;
    smoothdatamesh = false;
    plotstyle = FILLEDMESH;
    shading = GOURAUD;
    polygonOffset = 0.5;
}

/**
Instantiate a Standard Appearance object.

- Mesh color:      black
- Mesh line width: 1
- Smooth mesh:     false
- Plot style:      FILLEDMESH
- Shading:         GOURAUD
- Polygon offset:  0.5
- Data color:      StandardColor(colors) 
*/
Qwt3D::Appearance::Appearance(unsigned colors /* = 100 */)
{
    datacolor_p = ValuePtr<Color>(new StandardColor(colors));
    userplotstyle_p = 0;
}

void Qwt3D::Appearance::copy( const Appearance& val )
{
    appproxy_ = val.appproxy_;
    datacolor_p = val.datacolor_p;
    if (val.userplotstyle_p)
        setPlotStyle(*val.userplotstyle_p);
    else
        userplotstyle_p = 0;
}


Qwt3D::Appearance::Appearance( const Appearance& val )
{
    copy(val);
}

Appearance& Qwt3D::Appearance::operator=( const Appearance& val )
{
    if (this == &val)
        return *this;

    copy(val);
    return *this;
}

Qwt3D::Appearance::~Appearance()
{
    delete userplotstyle_p;
    for (ELIT it = elist.begin(); it!=elist.end(); ++it)
        delete (*it);

    elist.clear();
}

void Qwt3D::Appearance::update(const Plot3D& plot)
{
    datacolor_p->update(plot);
}

void Qwt3D::Appearance::setDataColor( const Color& col )
{
    datacolor_p = ValuePtr<Color>(col.clone());
}

/*!
Set plotstyle for the standard plotting types. An argument of value Qwt3D::USER
is ignored.
*/
void Qwt3D::Appearance::setPlotStyle( PLOTSTYLE val )
{
    if (val == Qwt3D::USER)
        return;
    delete userplotstyle_p;
    userplotstyle_p = 0;
    appproxy_.plotstyle = val;
}

/*!
Set plotstyle to Qwt3D::USER and an associated enrichment object.
*/
Qwt3D::Enrichment* Qwt3D::Appearance::setPlotStyle( const Qwt3D::Enrichment& obj )
{
    if (&obj == userplotstyle_p)
        return userplotstyle_p;

    delete userplotstyle_p;
    userplotstyle_p = obj.clone();
    appproxy_.plotstyle = Qwt3D::USER;
    return userplotstyle_p;
}

void Qwt3D::Appearance::setShading( Qwt3D::SHADINGSTYLE val )
{
    if (val == appproxy_.shading)
        return;

    appproxy_.shading = val;

    switch (appproxy_.shading)
    {
    case FLAT:
        glShadeModel(GL_FLAT);
        break;
    case GOURAUD:
        glShadeModel(GL_SMOOTH);
        break;
    default:
        break;
    }
}

Enrichment* Qwt3D::Appearance::addEnrichment(Enrichment const& e)
{
    if ( elist.end() == std::find( elist.begin(), elist.end(), &e ) )
        elist.push_back(e.clone());
    return elist.back();
}

bool Qwt3D::Appearance::degrade(Enrichment* e)
{
    ELIT it = std::find(elist.begin(), elist.end(), e);

    if ( it != elist.end() )
    {
        delete (*it);
        elist.erase(it);
        return true;
    }
    return false;
}
