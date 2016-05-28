TARGET            = qwtplot3d
TEMPLATE          = lib

INST_USE_C11=$$(QXMLEDIT_INST_USE_C11)
isEmpty(INST_USE_C11) {
    INST_USE_C11 = ""
}

# object files
#external/qwtplot/src
OBJECTS_DIR = ../../../build/3dplot/obj
MOC_DIR = ../../../build/3dplot/moc
UI_DIR = ../../../build/3dplot/ui
UI_HEADERS_DIR = ../../../build/3dplot/ui/include
UI_SOURCES_DIR = ../../../build/3dplot/ui/src
RCC_DIR = ../../../build/3dplot/rcc
DESTDIR    = ../../../build


CONFIG           += qt warn_on thread static
QT               += opengl

equals(INST_USE_C11, "y") {
 QMAKE_CXXFLAGS +=-std=c++11
}

SOURCES           = *.cpp

# MOCs will be generated from
HEADERS           = ../include/qwt3d_plot3d.h \
                    ../include/qwt3d_extglwidget.h \
                    ../include/qwt3d_graphplot.h \
                    ../include/qwt3d_gridplot.h  \
                    ../include/qwt3d_surfaceplot.h \
                    ../include/qwt3d_volumeplot.h
HEADERS           += ../include/*.h

INCLUDEPATH       = ../include

win32 {
  !build_pass {
    win32-msvc | win32-msvc2002 {
      error(Unsupported Visual Studio version ( < 2003 ))
    }
  }
  
  win32-msvc2008 | win32-msvc2010 | win32-msvc2012 | win32-msvc2013 {
    QMAKE_CXXFLAGS += -MP
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_STL
    QMAKE_CXXFLAGS_RELEASE += /fp:fast /arch:SSE2
  }
}

linux-g++:QMAKE_CXXFLAGS += -fno-exceptions

#unix:VERSION = 0.3.1
