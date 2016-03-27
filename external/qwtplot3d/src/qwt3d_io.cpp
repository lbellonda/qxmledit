//#include <time.h>
#include <QImageWriter>

#include "qwt3d_plot3d.h"
#include "qwt3d_io_reader.h"

using namespace Qwt3D;

IO::Entry::Entry() : iofunc(0) 
{
}

IO::Entry::~Entry() 
{
  delete iofunc;
}

IO::Entry::Entry(IO::Entry const& e)
{
  if (this==&e)
    return;

  fmt = e.fmt;
  iofunc = e.iofunc->clone();
}

void IO::Entry::operator=(IO::Entry const& e)
{
  if (this==&e)
    return;

  delete iofunc;
  fmt = e.fmt;
  iofunc = e.iofunc->clone();
}

IO::Entry::Entry(QString const& s, Functor const& f)
  : fmt(s) 
{ 
  iofunc = f.clone();
}

IO::Entry::Entry(QString const& s, Function f)
  : fmt(s) 
{ 
  Wrapper  w(f);
  iofunc = w.clone();
}


IO::FormatCompare::FormatCompare(IO::Entry const& e) 
{
  e_ = e;
}

bool IO::FormatCompare::operator() (IO::Entry const& e)
{
  return ( e.fmt == e_.fmt);
}

IO::FormatCompare2::FormatCompare2(QString s) 
{
  s_ = s;
}

bool IO::FormatCompare2::operator() (IO::Entry const& e)
{
  return( e.fmt == s_);
}




bool IO::add_unique(Container& l, Entry const& e)
{
  FormatCompare comp(e);
  l.erase(std::remove_if(l.begin(), l.end(), comp), l.end());
  l.push_back(e);

  return true;
}

IO::IT IO::find(Container& l, QString const& fmt)
{
  FormatCompare2 comp(fmt);
  return std::find_if(l.begin(), l.end(), comp); 
}

IO::Container& IO::rlist()
{
  static Container rl = Container();
  static bool rfirst = true;
//  bool f = false;
//  f = rfirst;
  if (rfirst)
  {
    rfirst = false;
    setupHandler();
  }
  return rl;
}

IO::Container& IO::wlist()
{
  static Container wl = Container();
  static bool wfirst = true;
//  bool f = false;
//  f = wfirst;
  if (wfirst)
  {
    wfirst = false;
    setupHandler();
  }
  return wl;
}

/*! 
  Registers a new IO::Function for data input.\n
  Every call overwrites a formerly registered handler for the same format string
  (case sensitive).
*/
bool IO::defineInputHandler(QString const& format, IO::Function func)
{
  return add_unique(rlist(), Entry(format, func));
}

/*! 
  Registers a new Functor for data input.\n
  Every call overwrites a formerly registered handler for the same format string
  (case sensitive).
*/
bool IO::defineInputHandler(QString const& format, IO::Functor const& func)
{
  return add_unique(rlist(), Entry(format, func));
}

/*! 
  Registers a new IO::Function for data output.  
  Every call overwrites a formerly registered handler for the same format string
  (case sensitive).
 */
bool IO::defineOutputHandler(QString const& format, IO::Function func)
{
  return add_unique(wlist(), Entry(format, func));
}

/*! 
  Registers a new Functor for data output.\n
  Every call overwrites a formerly registered handler for the same format string
  (case sensitive).
*/
bool IO::defineOutputHandler(QString const& format, IO::Functor const& func)
{
  return add_unique(wlist(), Entry(format, func));
}

/*!
  Applies a reading IO::Function or IO::Functor.
  \param plot Plot with the content that should be loaded
  \param fname File name 
  \param format Input format
  \return The return value from the called Function/Functor. 
  The function returns false, if no registered handler could be found.
*/
bool IO::load(Plot3D* plot, QString const& fname, QString const& format)
{
  IT it = IO::find(rlist(), format);

  if (it == rlist().end())
    return false;

  return (*it->iofunc)(plot, fname);
}

/*!
  Applies a writing IO::Function or IO::Functor.
  \param plot Plot with the content that should be saved
  \param fname File name
  \param format Output format
  \return The return value from the called Function/Functor.
  The function returns false, if no registered handler could be found.
*/
bool IO::save(Plot3D* plot, QString const& fname, QString const& format)
{
  IT it = IO::find(wlist(), format);

  if (it == wlist().end())
    return false;

  return (*it->iofunc)(plot, fname);
}

/*!
  Returns a list of currently registered input formats. 
*/
QStringList IO::inputFormatList()
{
  QStringList list;
  for ( IT it = rlist().begin(); it!=rlist().end(); ++it )
    list.append(it->fmt);
  
  return list;
}

/*!
  Returns a list of currently registered output formats. 
*/
QStringList IO::outputFormatList()
{
  QStringList list;  
  for ( IT it = wlist().begin(); it!=wlist().end(); ++it )
    list.append(it->fmt);
  
  return list;
}

/*! 
  Returns the input functor in charge for format and 0 if non-existent. 
*/
IO::Functor* IO::inputHandler(QString const& format)
{
  IO::IT it = IO::find(rlist(), format);
  
  if (it==rlist().end())
    return 0;

  return it->iofunc;
}

/*! 
  Returns the output functor in charge for format and 0 if non-existent. 
*/
IO::Functor* IO::outputHandler(QString const& format)
{
  IO::IT it = IO::find(wlist(), format);
  
  if (it==wlist().end())
    return 0;

  return it->iofunc;
}

bool PixmapWriter::operator()(Plot3D* plot, QString const& fname, bool /*= unused */)
{
//  QImage im = plot->grabFrameBuffer(true);
  
  QImage im = plot->renderPixmap().toImage();
  QImageWriter iio;
  iio.setFormat(QWT3DLOCAL8BIT(fmt_));
  iio.setQuality(quality_);
  iio.setFileName(fname);

  return iio.write(im);
}

//! Sets quality (the way QImage::save() uses it).
void PixmapWriter::setQuality(int val)
{
  quality_ = val;  
}

void IO::setupHandler()
{
  QList<QByteArray> list = QImageWriter::supportedImageFormats();
  QList<QByteArray>::Iterator it = list.begin();
  PixmapWriter qtw;
  while( it != list.end() ) 
  {
    qtw.fmt_ = *it;
    defineOutputHandler(*it, qtw);
    ++it;
  }

  defineInputHandler("mes", NativeReader());
  defineInputHandler("MES", NativeReader());
}

/*! 
  Saves content in one of the registered output formats. To modify the 
  behavior for more complex output handling use IO::outputHandler.
*/
bool Plot3D::save(QString const& fileName, QString const& format)
{
  return IO::save(this, fileName, format);
}

