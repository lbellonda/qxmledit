TEMPLATE     = app
CONFIG      += qt warn_on thread
QT += opengl
UI_DIR = tmp
MOC_DIR      = tmp
OBJECTS_DIR  = tmp
INCLUDEPATH    += ../../include .
DEPENDPATH	= $$INCLUDEPATH
DESTDIR = ../bin

win32{
  !build_pass {
    win32-msvc | win32-msvc2002 {
      error(Unsupported Visual Studio version ( < 2003 ))
    }
  }

  win32-msvc2008 | win32-msvc2010 | win32-msvc2012 | win32-msvc2013 {
    QMAKE_CXXFLAGS += -MP
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_STL
  }

    LIBS += -L../../lib -lqwtplot3d -lopengl32 -lglu32 -lgdi32
}

unix:!macx { LIBS += -L../../lib -lqwtplot3d -lGLU }

macx: LIBS += -L../../lib -lqwtplot3d -framework OpenGL

linux-g++:QMAKE_CXXFLAGS += -fno-exceptions

RC_FILE = ../icon.rc

RESOURCES     = ../images.qrc
