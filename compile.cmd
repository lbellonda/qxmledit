rem this file is part of QXmlEdit
if exist src\QXmlEdit.pro lrelease src\QXmlEdit.pro
if exist src\QXmlEditWidget.pro lrelease src\QXmlEditWidget.pro
if exist src\sessions\QXmlEditSessions.pro lrelease src\sessions\QXmlEditSessions.pro
qmake "CONFIG+=release"
mingw32-make

