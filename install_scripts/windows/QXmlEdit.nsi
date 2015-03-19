; define here the location of the sources
!define BuildDir "yyyyyyyyyyyyy"
!define SourceDir "xxxxxxxxx"
!define Version "0.0.0"
!define QtDir "zzzzzz"

Name "QXmlEdit"


OutFile "QXmlEditInstaller-${Version}.exe"

InstallDir $PROGRAMFILES\QXmlEdit

RequestExecutionLevel admin

Page directory
Page instfiles
;UninstPage uninstConfirm
;UninstPage instfiles


Section ""

        SetOutPath $INSTDIR

        File "${BuildDir}\QXmlEdit.exe"
        File "${BuildDir}\QXmlEditWidget.dll"
        File "${BuildDir}\QXmlEditSessions.dll"
        File "${SourceDir}\README"
        File "${SourceDir}\COPYING"
        File "${SourceDir}\GPLV3.txt"
        File "${SourceDir}\LGPLV3.txt"
        File "${QtDir}\bin\mingwm10.dll"
        File "${QtDir}\bin\libgcc_s_dw2-1.dll"
        File "${QtDir}\bin\QtCore4.dll"
        File "${QtDir}\bin\QtXml4.dll"
        File "${QtDir}\bin\QtGui4.dll"
        File "${QtDir}\bin\QtSvg4.dll"
        File "${QtDir}\bin\QtSql4.dll"
        File "${QtDir}\bin\QtXmlPatterns4.dll"
        File "${QtDir}\bin\QtNetwork4.dll"
        File "${SourceDir}\doc\QXmlEdit_manual.pdf"
        File "${SourceDir}\src\data\sample.style"
        ;File "${SourceDir}\another.sample.style"
        SetOutPath $INSTDIR\sqldrivers
        File "${QtDir}\plugins\sqldrivers\qsqlite4.dll"
        SetOutPath $INSTDIR\translations
        File "${SourceDir}\src\languages_app\QXmlEdit_cs.qm"
        File "${SourceDir}\src\languages_lib\QXmlEditWidget_cs.qm"
        File "${SourceDir}\src\sessions\languages\QXmlEditSessions_cs.qm"
        SetOutPath $INSTDIR\snippets
        File "${SourceDir}\src\data\snippets\{6d11b8d1-e285-46b2-8375-79e17cab9862}.xml"
        File "${SourceDir}\src\data\snippets\{b3303cb3-8537-46d0-943b-cf898cc93b3f}.xml"
        File "${SourceDir}\src\data\snippets\{bd4984f5-107e-48ca-9780-c263ff1aa58d}.xml"
        File "${SourceDir}\src\data\snippets\{d78f4e09-4003-4178-adec-134a7b996d6e}.xml"
        File "${SourceDir}\src\data\snippets\{e40792fb-16de-4418-9320-4feaa92c28a3}.xml"
        File "${SourceDir}\src\data\snippets\{edc60671-0ae0-401b-989d-5d0b82976bce}.xml"
        File "${SourceDir}\src\data\snippets\{f6d7ebdd-c505-4a6a-8a16-f7c99581a995}.xml"

        WriteRegStr HKLM SOFTWARE\QXmlEdit "Install_Dir" "$INSTDIR"

  ; uninstaller
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QXmlEdit" "DisplayName" "QXmlEdit"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QXmlEdit" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QXmlEdit" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QXmlEdit" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd

Section "Start Menu Shortcuts"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\QXmlEdit"
  CreateDirectory "$SMPROGRAMS\QXmlEdit\Uninstall"
  CreateShortCut "$SMPROGRAMS\QXmlEdit\Uninstall\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\QXmlEdit\QXmlEdit.lnk" "$INSTDIR\QXmlEdit.exe" "" "$INSTDIR\QXmlEdit.exe" 0
  CreateShortCut "$SMPROGRAMS\QXmlEdit\QXmlEditManual.lnk" "$INSTDIR\QXmlEdit_manual.pdf" "" "$INSTDIR\QXmlEdit_manual.pdf" 0

SectionEnd


Section "Uninstall"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\QXmlEdit"
  DeleteRegKey HKLM "SOFTWARE\QXmlEdit"

  SetShellVarContext all
  Delete "$SMPROGRAMS\QXmlEdit\Uninstall\*.*"
  RMDir "$SMPROGRAMS\QXmlEdit\Uninstall"
  Delete "$SMPROGRAMS\QXmlEdit\*.*"
  RMDir "$SMPROGRAMS\QXmlEdit"
  Delete "$INSTDIR\sqldrivers\*.*"
  RMDir "$INSTDIR\sqldrivers"
  Delete "$INSTDIR\translations\*.*"
  RMDir "$INSTDIR\translations"
  Delete "$INSTDIR\snippets\*.*"
  RMDir "$INSTDIR\snippets"
  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR"
SectionEnd
