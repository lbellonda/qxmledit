/********************************************************************
    created:   2003/09/09
    filename:  main.cpp
	
    author:    Micha Bieber	
*********************************************************************/

#include <qapplication.h>
#include "meshmainwindow.h"

int main( int argc, char **argv )
{
  QApplication::setColorSpec( QApplication::CustomColor );
  QApplication app(argc,argv);

  if ( !QGLFormat::hasOpenGL() ) 
	{
		qWarning( "This system has no OpenGL support. Exiting." );     
		return -1;
  }
    
  MeshMainWindow mainwindow;
  mainwindow.resize(1024,768);
	mainwindow.show();

	return app.exec();
}
