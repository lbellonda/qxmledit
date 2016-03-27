CONFIG           += qt warn_on thread static
QT               += opengl

win32 {
  !build_pass {
    win32-msvc | win32-msvc2002 {
      error(Unsupported Visual Studio version ( < 2003 ))
    }
  }
  
  #win32-msvc2003 | win32-msvc2005 | win32-msvc2008 {
  #  TEMPLATE    = vclib
  #  CONFIG     += dll exceptions
  #  dll:DEFINES    += QT_DLL QWT3D_DLL QWT3D_MAKEDLL
  #  QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_STL

  #  win32-msvc2008 | win32-msvc2010 {
  #    !build_pass {
  #      message(Ignore warnings regarding parsing problems for /MP switch)
  #    }
  #    QMAKE_CXXFLAGS += /MP
  #  }
  #}

  win32-msvc2003 | win32-msvc2005 | win32-msvc2008 | win32-msvc2010 {
    QMAKE_CXXFLAGS += -MP
    QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_STL
    QMAKE_CXXFLAGS += -fp:fast -arch:SSE2

    #test - asm output
    QMAKE_CXXFLAGS += -FAs
  }
}

linux-g++:QMAKE_CXXFLAGS += -fno-exceptions

	
