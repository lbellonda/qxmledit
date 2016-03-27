#ifndef axes_h__2004_06_07_10_38_begin_guarded_code
#define axes_h__2004_06_07_10_38_begin_guarded_code

#include "qwt3d_plot3d.h"
using namespace Qwt3D;

/*****************************
*
*  Examples for user defined 
*  tic labels
*
******************************/

class Letter : public LinearScale
{
public:
  explicit Letter(bool uppercase = true) : uc_(uppercase) {}
  Scale* clone() const {return new Letter(*this);}
  QString ticLabel(unsigned int idx) const
  {
    if (idx<majors_p.size() && idx < 26)
      return (uc_) ? QString(QChar('A'+idx)) : QString(QChar('a'+idx));
    return QString("-");
  }
private:
  bool uc_;
};

class Imaginary : public LinearScale
{
public:
  Scale* clone() const {return new Imaginary;}
  QString ticLabel(unsigned int idx) const
  {
    if (idx<majors_p.size())
    {
      double val = majors_p[idx];
      if (val)
        return QString::number(val) + "*i";
      return QString::number(val);
    }
    return QString("");
  }
};

class TimeItems : public LinearScale
{
public:
  Scale* clone() const {return new TimeItems;}
  QString ticLabel(unsigned int idx) const
  {
    if (idx<majors_p.size())
    {
      QTime t = QTime::currentTime();
      int h = t.hour();
      int m = t.minute();
      if (m+idx > 59)
      {
        if (h==23)
          h=0;
        else
          h+=1;
        m = (m+idx) % 60; 
      }
      else
        m += idx;

      return QTime(h,m).toString("hh:mm")+"h";
    }
    return QString("");
  }
};


#endif /* include guarded */
