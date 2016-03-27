//#include <qbitmap.h>
#include "qwt3d_label.h"

using namespace Qwt3D;

Label::Label()
{
  init();
}

Label::Label(const QString & family, int pointSize, int weight, bool italic)
{
  init(family, pointSize, weight, italic);
}


void Label::init(const QString & family, int pointSize, int weight, bool italic)
{
  init();
  font_ = QFont(family, pointSize, weight, italic );
}

void Label::init()
{
  beg_ = Triple(0.0, 0.0, 0.0);
  end_ = beg_;
  pos_ = beg_;
  setColor(0,0,0);
  font_ = QFont();
  anchor_ = BottomLeft;
  gap_ = 0;
  width_ = 0.0;
  height_ = 0.0;
}

void Label::setFont(const QFont& f)
{
  if ( font_ == f )
    return;

  font_ = f;
}
void Label::setFont(const QString & family, int pointSize, int weight, bool italic)
{
  setFont( QFont(family, pointSize, weight, italic ) );
}

void Label::setString(QString const& s)
{
  text_ = s;
}
const QString& Label::string() const
{
  return text_;
}

void Label::setColor(double r, double g, double b, double a)
{
  Drawable::setColor(r,g,b,a);
  flagforupdate_ = true;
}   

void Label::setColor(Qwt3D::RGBA rgba)
{
  Drawable::setColor(rgba);
  flagforupdate_ = true;
}

/**
example:

\verbatim

   Anchor TopCenter (*)  resp. BottomRight(X) 

   +----*----+
   |  Pixmap |
   +---------X

\endverbatim
*/
void Label::setPosition(Triple pos, ANCHOR a)
{
  use_relpos_ = false;
  anchor_ = a;
  pos_ = pos;
}

void Label::setRelPosition(Tuple rpos, ANCHOR a)
{
  double ot = 0.99;

  relpos_ = Triple(rpos.x, rpos.y, ot);
  anchor_ = a;
  use_relpos_ = true;
}

QImage Label::createImage(double angle)
{
  QRect r = QRect(QPoint(0, 0), QFontMetrics(font_).size(Qwt3D::SingleLine, text_));
  int textWidth = r.width();
  int textHeight = r.height();

  double aux_a = angle;
  if (aux_a > 270)
      aux_a -= 270;
  if (aux_a >= 180)
      aux_a -= 180;
  if (aux_a > 90)
      aux_a -= 90;
  
  double rad = aux_a * PI/180.0;

  int w = 0, h = 0;
  if ((angle >= 0 && angle <= 90) || (angle >= 180 && angle <= 270))
  {
      w = qRound(fabs(textWidth*cos(rad) + textHeight*sin(rad)));
      h = qRound(fabs(textWidth*sin(rad) + textHeight*cos(rad)));
  } 
  else 
  {
      w = qRound(fabs(textWidth*sin(rad) + textHeight*cos(rad)));
      h = qRound(fabs(textWidth*cos(rad) + textHeight*sin(rad)));
  }
  
  width_ = w;
  height_ = h;
  QPixmap pm_ = QPixmap(w, h);
  pm_.fill(Qt::transparent);
  QPainter p(&pm_);
  if (angle >= 270)
    p.translate(textHeight*cos(rad), 0.0);
  else if (angle >= 180)
    p.translate(w, textHeight*cos(rad));
  else if (angle > 90)
    p.translate(textWidth*sin(rad), h);
  else
    p.translate(0.0, textWidth*sin(rad));
  p.rotate(-angle);
  p.translate(0.0, textHeight - QFontMetrics(font_).descent());

  p.setFont( font_ );
  p.setPen( Qt::SolidLine );
  p.setPen( GL2Qt(color.r, color.g, color.b) );

  p.drawText(0, 0, text_);
  p.end();

  return QGLWidget::convertToGLFormat(pm_.toImage());
}

/**
Adds an additional shift to the anchor point. This happens in a more or less intelligent manner
depending on the nature of the anchor:
\verbatim
anchor type         shift

left aligned         -->
right aligned        <--
top aligned          top-down            
bottom aligned       bottom-up
\endverbatim
The unit is user space dependent (one pixel on screen - play around to get satisfying results)
*/
void Label::adjust(int gap)
{
  gap_ = gap;
}

void Label::convert2screen()
{
  Triple start = World2ViewPort(pos_);
  
  switch (anchor_)
  {
    case BottomLeft :
      beg_ = pos_;
      break;
    case BottomRight:
      beg_ = ViewPort2World(start - Triple(width() + gap_, 0, 0));
      break;
    case BottomCenter:
      beg_ = ViewPort2World(start - Triple(width() / 2, -gap_, 0));
      break;
    case TopRight:
      beg_ = ViewPort2World(start - Triple(width() + gap_, height(), 0));
      break;
    case TopLeft:
      beg_ = ViewPort2World(start - Triple(-gap_, height(), 0));
      break;
    case TopCenter:
      beg_ = ViewPort2World(start - Triple(width() / 2, height() + gap_, 0));
      break;
    case CenterLeft:
      beg_ = ViewPort2World(start - Triple(-gap_, height() / 2, 0));
      break;
    case CenterRight:
      beg_ = ViewPort2World(start - Triple(width() + gap_, height() / 2, 0));
      break;
    case Center:
      beg_ = ViewPort2World(start - Triple(width() / 2, height() / 2, 0));
      break;
    default:
      break;
  }
  start = World2ViewPort(beg_);
  end_ = ViewPort2World(start + Triple(width(), height(), 0));    
}

const char * Label::fontname()
{
  const char *name = "Helvetica";
  if (font_.family() == "Times New Roman")
  {
    name = "Times";
    if (font_.bold() && font_.italic ())
      name = "Times-BoldItalic";
    else if (font_.italic())
      name = "Times-Italic";
    else if (font_.bold())
      name = "Times-Bold";
  } 
  else if (font_.family() == "Courier" || font_.family() == "Courier New")
  {
    name = "Courier";
    if (font_.bold() && font_.italic ())
      name = "Courier-BoldOblique";
    else if (font_.italic())
      name = "Courier-Oblique";
    else if (font_.bold())
      name = "Courier-Bold";
  } 
  else 
  {
    if (font_.bold() && font_.italic ())
      name = "Helvetica-BoldOblique";
    else if (font_.italic())
      name = "Helvetica-Oblique";
    else if (font_.bold())
      name = "Helvetica-Bold";
  }
	return (const char*) name;
}

void Label::drawLabel(double angle)
{
  if (text_.isEmpty())
    return;
  if ( use_relpos_ ) 
  {
    getMatrices(modelMatrix, projMatrix, viewport);
    beg_ = relativePosition(relpos_);
    setPosition(beg_, anchor_);
    use_relpos_ = true;// reset the flag
  }

      
  GLboolean b;
  GLint func;
  GLdouble v;
  glGetBooleanv(GL_ALPHA_TEST, &b);
  glGetIntegerv(GL_ALPHA_TEST_FUNC, &func);
  glGetDoublev(GL_ALPHA_TEST_REF, &v);
  
  glEnable (GL_ALPHA_TEST);
  glAlphaFunc (GL_NOTEQUAL, 0.0);
  
  convert2screen();
  glRasterPos3d(beg_.x, beg_.y, beg_.z);

  QImage tex_ = createImage(angle);
  glDrawPixels(tex_.width(), tex_.height(), GL_RGBA, GL_UNSIGNED_BYTE, tex_.bits());
  glAlphaFunc(func,v);
  Enable(GL_ALPHA_TEST, b);
}


double Label::width() const 
{ 
    if (width_ > 0.0 && height_ > 0.0)
        return width_;
    return QRect(QPoint(0, 0), QFontMetrics(font_).size(Qwt3D::SingleLine, text_)).width();
}

double Label::height() const 
{ 
    if (width_ > 0.0 && height_ > 0.0)
        return height_;
    return QRect(QPoint(0, 0), QFontMetrics(font_).size(Qwt3D::SingleLine, text_)).height();
}   
double Label::textHeight() const
{
    return QRect(QPoint(0, 0), QFontMetrics(font_).size(Qwt3D::SingleLine, text_)).height();
}
