#include "qwt3d_parametricsurface.h"
#include "qwt3d_gridplot.h"

using namespace Qwt3D;

ParametricSurface::ParametricSurface()
:GridMapping()
{
}

ParametricSurface::ParametricSurface(GridPlot& pw)
:GridMapping()
{
	plotwidget_p = &pw;
  uperiodic_ = false;
  vperiodic_ = false;
}

ParametricSurface::ParametricSurface(GridPlot* pw)
:GridMapping()
{
	plotwidget_p = pw;
  uperiodic_ = false;
  vperiodic_ = false;
}

void ParametricSurface::setPeriodic(bool u, bool v)
{
  uperiodic_ = u;
  vperiodic_ = v;
}

void ParametricSurface::assign(GridPlot& plotWidget)
{
	if (&plotWidget != plotwidget_p)
		plotwidget_p = &plotWidget;
}

void ParametricSurface::assign(GridPlot* plotWidget)
{
	if (plotWidget != plotwidget_p)
		plotwidget_p = plotWidget;
}

/**
For plotWidget != 0 the function permanently assigns her argument (In fact, assign(plotWidget) is called)
*/
bool ParametricSurface::create(bool append /*= false*/)
{
	if ((umesh_p<=2) || (vmesh_p<=2) || !plotwidget_p)
		return false;
	
	/* allocate some space for the mesh */
 	Triple** data         = new Triple* [umesh_p] ;

	unsigned i,j;
	for ( i = 0; i < umesh_p; i++) 
	{
		data[i]         = new Triple [vmesh_p];
	}
	
	/* get the data */

	double du = (maxu_p - minu_p) / (umesh_p - 1);
	double dv = (maxv_p - minv_p) / (vmesh_p - 1);
	
  for (i = 0; i < umesh_p; ++i) 
	{
		for (j = 0; j < vmesh_p; ++j) 
		{
            Triple &value = data[i][j];
            value = operator()(minu_p + i*du, minv_p + j*dv);
			
            if (value.x > range_p.maxVertex.x)
                value.x = range_p.maxVertex.x;
            else if (value.y > range_p.maxVertex.y)
                value.y = range_p.maxVertex.y;
            else if (value.z > range_p.maxVertex.z)
                value.z = range_p.maxVertex.z;
            else if (value.x < range_p.minVertex.x)
                value.x = range_p.minVertex.x;
            else if (value.y < range_p.minVertex.y)
                value.y = range_p.minVertex.y;
            else if (value.z < range_p.minVertex.z)
                value.z = range_p.minVertex.z;
		}
	}

	((GridPlot*)plotwidget_p)->createDataset(data, umesh_p, vmesh_p, uperiodic_, vperiodic_, append);

	for ( i = 0; i < umesh_p; i++) 
	{
		delete [] data[i];
	}

	delete [] data;

	return true;
}

bool ParametricSurface::create(GridPlot& pl)
{
  assign(pl);
  return create();
}
