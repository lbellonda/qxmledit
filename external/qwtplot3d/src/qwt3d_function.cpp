#include <cstdio>
#include "qwt3d_gridplot.h"
#include "qwt3d_function.h"

using namespace Qwt3D;

Function::Function()
:GridMapping()
{
}

Function::Function(GridPlot& pw)
:GridMapping()
{
  plotwidget_p = &pw;
}

Function::Function(GridPlot* pw)
:GridMapping()
{
  plotwidget_p = pw;
}

void Function::assign(GridPlot& plotWidget)
{
	if (&plotWidget != plotwidget_p)
		plotwidget_p = &plotWidget;
}

void Function::assign(GridPlot* plotWidget)
{
	if (plotWidget != plotwidget_p)
		plotwidget_p = plotWidget;
}

void Function::setMinZ(double val)
{
	range_p.minVertex.z = val;
}

void Function::setMaxZ(double val)
{
	range_p.maxVertex.z = val;
}

bool Function::create(bool append /*= false*/)
{
	if ((umesh_p<=2) || (vmesh_p<=2) || !plotwidget_p)
		return false;
	
	/* allocate some space for the mesh */
 	double** data         = new double* [umesh_p] ;

	unsigned i,j;
	for ( i = 0; i < umesh_p; i++) 
	{
		data[i] = new double [vmesh_p];
	}
	
	/* get the data */

	double dx = (maxu_p - minu_p) / (umesh_p - 1);
	double dy = (maxv_p - minv_p) / (vmesh_p - 1);
	
	for (i = 0; i < umesh_p; ++i) 
	{
		for (j = 0; j < vmesh_p; ++j) 
		{
			data[i][j] = operator()(minu_p + i*dx, minv_p + j*dy);
			
			if (data[i][j] > range_p.maxVertex.z)
				data[i][j] = range_p.maxVertex.z;
			else if (data[i][j] < range_p.minVertex.z)
				data[i][j] = range_p.minVertex.z;
		}
	}

	Q_ASSERT(plotwidget_p);
	if (!plotwidget_p)
	{
		fprintf(stderr,"Function: no valid Plot3D Widget assigned");
	}
	else
	{
		((GridPlot*)plotwidget_p)->createDataset(data, umesh_p, vmesh_p, minu_p, maxu_p, minv_p, maxv_p, append);
	}

	for ( i = 0; i < umesh_p; i++) 
	{
		delete [] data[i];
	}

	delete [] data;

	return true;
}

bool Function::create(GridPlot& pl)
{
  assign(pl);
  return create();
}
