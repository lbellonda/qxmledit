#include "qwt3d_color_std.h"
#include "qwt3d_plot3d.h"

using namespace Qwt3D;

StandardColor::StandardColor(unsigned size)
{
    zmin_ = zmax_ = 0.0;
    reset(size);
}

void StandardColor::reset(unsigned size)
{
    colors_ = ColorVector(size);
    RGBA elem;

    double dsize = size;

    for (unsigned int i=0; i!=size; ++i)
    {
        elem.r = i / dsize;
        elem.g = i / dsize / 4;
        elem.b = 1 - i/dsize;
        elem.a = 1.0;
        colors_[i] = elem;
    }
}

/**
    Assigns a new ColorVector (Also overwrites the constructors size argument)
*/
void StandardColor::setColorVector(ColorVector const& cv)
{
    colors_ = cv;
}

void StandardColor::setAlpha(double a)
{
    if (a<0 || a>1)
        return;

    RGBA elem;

    for (unsigned int i=0; i!=colors_.size(); ++i)
    {
        elem = colors_[i];
        elem.a = a;
        colors_[i] = elem;
    }
}	

RGBA StandardColor::rgba(double, double, double z) const
{
    if (colors_.empty())
        return RGBA(0.0,0.0,0.0);

    unsigned index = 0;
    double fac = (z - zmin_) / (zmax_-zmin_);
    if (fac<0)
        return RGBA(0.0,0.0,0.0);

    index = unsigned((colors_.size()-1) * fac);
    if (index >= colors_.size())
        index = colors_.size()-1;

    return colors_[index];
}

void StandardColor::update(const Qwt3D::Plot3D &val)
{
    zmin_ = val.hull().minVertex.z;
    zmax_ = val.hull().maxVertex.z;
}
