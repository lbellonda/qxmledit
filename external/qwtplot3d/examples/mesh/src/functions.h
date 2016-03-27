#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

#include <math.h>
#include <qwt3d_parametricsurface.h>
#include <qwt3d_function.h>

using namespace Qwt3D;


class Rosenbrock : public Function
{
public:

	Rosenbrock(GridPlot& pw)
	:Function(pw)
	{
	}

	double operator()(double x, double y)
	{
    return 0.7 * log10((1-x)*(1-x) + 10 * (y - x*x)*(y - x*x));
	}
//	QString name() const {return "Almost {\\it Rosenbrock}\\,:\\quad$\\frac{\\ln((1-x)^2 + 100(y-x^2)^2)}{8}$";}
};

class Hat : public Function
{
public:

	Hat(GridPlot& pw)
	:Function(pw)
	{
		//setMinZ(0.3);     
    setDomain(0,10,0,10);
	}
	
	double operator()(double x, double y)
	{
    return 1.0 / (x*x+y*y+0.5);
    //return x*x*y/(x*x*x*x+y*y);
	}
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

class Saddle : public Function
{
public:

	Saddle()
	:Function()
	{
	//	setMaxZ(0.8);     
	}
	
	double operator()(double x, double y)
	{
		return x*x - y*y;
	}
//	QString name() const {return "$x^2-y^2$";}
};

class Mex : public Function
{
public:

	Mex()
	:Function()
	{
	//	setMaxZ(0.8);
	}
	
	double operator()(double x, double y)
	{
		double n = sqrt(x*x+y*y); 

		if (n < DBL_MIN)
			return 20;
		
		return 20 * sin(sqrt(x*x+y*y)) / n;
	}
//	QString name() const {return "$\\frac{20\\sin\\sqrt{x^2+y^2}}{\\sqrt{x^2+y^2}}$";}
};

class Torus : public ParametricSurface
{
public:

  Torus(GridPlot& pw)
  :ParametricSurface(pw)
  {
    setMesh(41,31);
    setDomain(-2*Qwt3D::PI, 0,-2*Qwt3D::PI,0);
    setPeriodic(true,true);
  }


  Triple operator()(double u, double v)
  {
    double x,y,z;
    double c = 1.9;
    x = (c + cos(v)) * cos(u);
    y = (c + cos(v)) * sin(u);
    z = sin(v) + cos(v);
    return Triple(x,y,z);
  }
};

class Seashell : public ParametricSurface
{
public:

  Seashell(GridPlot& pw)
  :ParametricSurface(pw)
  {
    setMesh(41,131);
    setDomain(0,2*Qwt3D::PI,0,2*Qwt3D::PI);
    setPeriodic(true,true);
  }


  Triple operator()(double u, double v)
  {
    double x,y,z;
    double a = 1;
    double b = 6;
    double c = 0.5;
    int n = 3;

    double f = v/(2*Qwt3D::PI);

    x = a*(1-f)*cos(n*v)*(1+cos(u)) + c*cos(n*v) ;
    y = a*(1-f)*sin(n*v)*(1+cos(u)) + c*sin(n*v) ;
    z = b*f + a*(1-f)*sin(u);
    return Triple(x,y,z);
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

class Dini : public ParametricSurface
{
public:

  Dini(GridPlot& pw)
  :ParametricSurface(pw)
  {
    setMesh(141,35);
    setDomain(0,5*Qwt3D::PI,0.001, 2);
    setPeriodic(true,true);
  }


  Triple operator()(double u, double v)
  {
    double x,y,z;
    double a = 5;
    double b = 1;

    
    x=a*cos(u)*sin(v);
    y=a*sin(u)*sin(v); 
    z=a*(cos(v)+log(tan(v/2)))+b*u;

    return Triple(x,y,z);
  }
};

#endif
