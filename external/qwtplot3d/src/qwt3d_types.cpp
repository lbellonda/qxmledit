#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning ( disable : 4786 )
#endif

#include <stdlib.h> // qsort
#include <algorithm>
#include <float.h>
#include "qwt3d_data.h"

using namespace Qwt3D;

#ifndef QWT3D_NOT_FOR_DOXYGEN

namespace {
// convex hull

typedef double coordinate_type;

int ccw(coordinate_type **P, int i, int j, int k) {
    coordinate_type	a = P[i][0] - P[j][0],
            b = P[i][1] - P[j][1],
            c = P[k][0] - P[j][0],
            d = P[k][1] - P[j][1];
    return a*d - b*c <= 0;	   /* true if points i, j, k counterclockwise */
}


#define CMPM(c,A,B) \
    v = (*(coordinate_type**)A)[c] - (*(coordinate_type**)B)[c];\
    if (v>0) return 1;\
    if (v<0) return -1;

int cmpl(const void *a, const void *b) {
    double v;
    CMPM(0,a,b);
    CMPM(1,b,a);
    return 0;
}

int cmph(const void *a, const void *b) {return cmpl(b,a);}


int make_chain(coordinate_type** V, int n, int (*cmp)(const void*, const void*)) {
    int i, j, s = 1;
    coordinate_type* t;

    qsort(V, n, sizeof(coordinate_type*), cmp);
    for (i=2; i<n; i++) {
        for (j=s; j>=1 && ccw(V, i, j, j-1); j--){}
        s = j+1;
        t = V[s]; V[s] = V[i]; V[i] = t;
    }
    return s;
}

int _ch2d(coordinate_type **P, int n)  {
    int u = make_chain(P, n, cmpl);		/* make lower hull */
    if (!n) return 0;
    P[n] = P[0];
    return u+make_chain(P+u, n-u+1, cmph);	/* make upper hull */
}


} // ns anon


QColor Qwt3D::GL2Qt(GLdouble r, GLdouble g, GLdouble b)
{
    return QColor(round(r * 255), round(g * 255), round(b * 255));
}

RGBA Qwt3D::Qt2GL(QColor col)
{
    QRgb qrgb = col.rgb();
    RGBA rgba;
    rgba.r = qRed(qrgb) / 255.0;
    rgba.g = qGreen(qrgb) / 255.0;
    rgba.b = qBlue(qrgb) / 255.0;
    rgba.a = qAlpha(qrgb) / 255.0;
    return rgba;
}


void Qwt3D::convexhull2d( std::vector<unsigned>& idx, const std::vector<Tuple>& src )
{
    idx.clear();
    if (src.empty())
        return;
    if (src.size()==1)
    {
        idx.push_back(0);
        return;
    }
    coordinate_type** points = new coordinate_type*[src.size()+1] ;
    coordinate_type* P = new coordinate_type[src.size()*2];

    int i;
    for (i=0; i<(int)src.size(); ++i)
    {
        points[i] = &P[2*i];
        points[i][0] = src[i].x;
        points[i][1] = src[i].y;
    }

    coordinate_type* start = points[0];
    int m = _ch2d( points, src.size() );
    idx.resize(m);
    
    for (i=0; i<m; ++i)
    {
        idx[i] = (points[i] - start)/2;
    }
    delete [] points;
    delete [] P;
}

unsigned Qwt3D::tesselationSize(CellVector const& t)
{
    unsigned ret = 0;

    for (unsigned i=0; i!=t.size(); ++i)
        ret += t[i].size();

    return ret;
}

Qwt3D::ParallelEpiped Qwt3D::hull(TripleVector const& data)
{
    ParallelEpiped hull(Triple(DBL_MAX,DBL_MAX,DBL_MAX), Triple(-DBL_MAX,-DBL_MAX,-DBL_MAX));

    for (unsigned i = 0; i < data.size(); ++i)
    {
        const Triple& vert = data[i];

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

    return hull;
}

#endif // QWT3D_NOT_FOR_DOXYGEN
