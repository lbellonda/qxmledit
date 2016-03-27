#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

#include <math.h>
#include <qwt3d_parametricsurface.h>
#include <qwt3d_function.h>

using namespace Qwt3D;

class Hat : public Function
{
public:

	Hat(GridPlot& pw, bool positive)
	:Function(pw)
	{
    d_ = (positive) ? 1.0 : -1.0;
    setDomain(0,10,0,10);
	}
	
	double operator()(double x, double y)
	{

    return d_ / (x*x+y*y+0.5);
    //return x*x*y/(x*x*x*x+y*y);
	}

private:
  double d_ ;
};


class Ripple : public Function
{
public:

	Ripple(GridPlot& pw)
	:Function(pw)
	{
		double l = 12; 
    setDomain(-l,l,-l,l);
	}
	
	double operator()(double x, double y)
	{
    return (cos(sqrt(x*x+y*y) + cos(sqrt(((x+.913*2*Qwt3D::PI)*(x+.913*2*Qwt3D::PI))+y*y)) 
      + cos(sqrt(((x-.913*2*Qwt3D::PI)*(x-.913*2*Qwt3D::PI))+(y*y))))*4);
	}
};

class Boy : public ParametricSurface
{
public:

  Boy(GridPlot& pw)
  :ParametricSurface(pw)
  {
    setMesh(141,131);
    setDomain(0,Qwt3D::PI,0,Qwt3D::PI);
    setPeriodic(true,true);
  }


  Triple operator()(double u, double v)
  {
    double x,y,z;
    double a = 2/3.;
    double b = sqrt(2.);
    
    x = a*(cos(u)*cos(2*v)+b*sin(u)*cos(v))*cos(u) / (b-sin(2*u)*sin(3*v));
    y = a*(cos(u)*sin(2*v)-b*sin(u)*sin(v))*cos(u) / (b-sin(2*u)*sin(3*v));
    z = b*cos(u)*cos(u) / (b-sin(2*u)*sin(2*v));
    
    return Triple(x,y,z);
  }
};

#endif
