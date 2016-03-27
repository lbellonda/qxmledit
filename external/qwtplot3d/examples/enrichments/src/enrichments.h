#ifndef enrichments_h__2004_02_25_13_58_begin_guarded_code
#define enrichments_h__2004_02_25_13_58_begin_guarded_code

#include "qwt3d_plot3d.h"


class Bar : public Qwt3D::VertexEnrichment
{
public:
  Bar();
  Bar(double rad, double level);

  Qwt3D::Enrichment* clone() const {return new Bar(*this);}
  
  void configure(double rad, double level);
  void drawBegin();
  void drawEnd();
  void draw(Qwt3D::Triple const&);

private:
  double level_, radius_;
  double diag_;
};

class  Label3D
{
public:
  void draw(Qwt3D::Triple const&, double w, double h);
};

#endif
