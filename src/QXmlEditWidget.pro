#/**************************************************************************
# *  This file is part of QXmlEdit                                         *
# *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
# *    as indicated in the AUTHORS file                                    *
# *  lbellonda _at_ gmail.com                                              *
# *                                                                        *
# * This library is free software; you can redistribute it and/or          *
# * modify it under the terms of the GNU Library General Public            *
# * License as published by the Free Software Foundation; either           *
# * version 2 of the License, or (at your option) any later version.       *
# *                                                                        *
# * This library is distributed in the hope that it will be useful,        *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
# * Library General Public License for more details.                       *
# *                                                                        *
# * You should have received a copy of the GNU Library General Public      *
# * License along with this library; if not, write to the                  *
# * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
# * Boston, MA  02110-1301  USA                                            *
# **************************************************************************/

############################ BEGIN INSTALLATION FOLDERS DECLARATION ###########################################
INST_DATA_DIR=$$(QXMLEDIT_INST_DATA_DIR)
isEmpty(INST_DATA_DIR) {
    INST_DATA_DIR = /opt/qxmledit
}

INST_DIR=$$(QXMLEDIT_INST_DIR)
isEmpty(INST_DIR) {
    INST_DIR = /opt/qxmledit
}

INST_DOC_DIR=$$(QXMLEDIT_INST_DOC_DIR)
isEmpty(INST_DOC_DIR) {
    INST_DOC_DIR = /opt/qxmledit
}

INST_LIB_DIR=$$(QXMLEDIT_INST_LIB_DIR)
isEmpty(INST_LIB_DIR) {
    INST_LIB_DIR = /opt/qxmledit
}

INST_TRANSLATIONS_DIR=$$(QXMLEDIT_INST_TRANSLATIONS_DIR)
isEmpty(INST_TRANSLATIONS_DIR) {
    INST_TRANSLATIONS_DIR=$$INST_DATA_DIR/translations
}

INST_INCLUDE_DIR=$$(QXMLEDIT_INST_INCLUDE_DIR)
isEmpty(INST_INCLUDE_DIR) {
    INST_INCLUDE_DIR = /opt/qxmledit
}

INST_AVOID_PRECOMP_HEADERS=$$(QXMLEDIT_INST_AVOID_PRECOMP_HEADERS)
isEmpty(INST_AVOID_PRECOMP_HEADERS) {
    INST_AVOID_PRECOMP_HEADERS = ""
}

USE_QWTPLOT="Y"
DONTUSE_QWTPLOT=$$(QXMLEDIT_NO_QWTPLOT)
!isEmpty(DONTUSE_QWTPLOT) {
    USE_QWTPLOT="N"
    message("No QWTPlot")
}

INST_USE_C11=$$(QXMLEDIT_INST_USE_C11)
isEmpty(INST_USE_C11) {
    INST_USE_C11 = ""
}

LIB_VERSIONED=$$(QXMLEDIT_VERSIONED)

############################ END INSTALLATION FOLDERS DECLARATION #############################################

include("version.pri")

#default value for the lib version name
LIB_VERSIONED_DEFAULT=""
unix:!macx: {
   LIB_VERSIONED_DEFAULT="1"
}

equals(LIB_VERSIONED, "") {
    LIB_VERSIONED = $$LIB_VERSIONED_DEFAULT
}

QXMLEDIT_LIB_SUFFIX = ""
!equals(LIB_VERSIONED, "") {
    QXMLEDIT_LIB_SUFFIX = -$$QXMLEDIT_VERSION
}

equals(QXMLEDIT_LIB_SUFFIX, "") {
    VERSION=""
}


QT       += xml xmlpatterns network svg

greaterThan(QT_MAJOR_VERSION, 4) {
  QT       += printsupport gui widgets core
}

greaterThan(QT_MAJOR_VERSION, 4) {
    equals(QT_MAJOR_VERSION, 5) {
        greaterThan(QT_MINOR_VERSION, 6) {
            QT       += scxml
        }
    }
    greaterThan(QT_MAJOR_VERSION, 5) {
        QT       += scxml
    }
}

TARGET = QXmlEditWidget$${QXMLEDIT_LIB_SUFFIX}
os2:TARGET = QXmlEdtW
TEMPLATE = lib

equals(INST_AVOID_PRECOMP_HEADERS, "") {
 CONFIG += precompile_header
 PRECOMPILED_HEADER=precompiled_lib.h
}

greaterThan(QT_MAJOR_VERSION, 4) {
win32 {
	QT += winextras
    DEFINES += "_NO_W32_PSEUDO_MODIFIERS"
    DEFINES += "NOGDI"
}
}


win32-msvc2010 {

} else {
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS +=-Werror
    #QMAKE_CXXFLAGS += -Winvalid-pch
}

equals(INST_USE_C11, "y") {
 QMAKE_CXXFLAGS +=-std=c++11
}

lessThan(QT_MAJOR_VERSION, 5) {
    QMAKE_CXXFLAGS +=-Wno-unused-local-typedefs
}

macx:QMAKE_CXXFLAGS += -Wno-deprecated-declarations

#CONFIG += release

INCLUDEPATH += .
INCLUDEPATH += $$PWD/globals/includes
DEPENDPATH += $$PWD/globals/includes
INCLUDEPATH += ./globals/framework
DEPENDPATH += ./globals/framework

DESTDIR = ../build
UI_DIR = ../build/lib/ui
MOC_DIR = ../build/lib/moc
OBJECTS_DIR = ../build/lib/obj
RCC_DIR = ../build/lib/rcc

TRANSLATIONS = \
   languages_lib/QXmlEditWidget_cs.ts \
   languages_lib/SCXML_cs.ts
TRANSL = \
   languages_lib/QXmlEditWidget_cs.qm \
   languages_lib/SCXML_cs.ts

translationsfiles.path = $$INST_TRANSLATIONS_DIR
translationsfiles.files = $$TRANSL

DEFINES += LIBQXMLEDIT_LIBRARY

libinclude.files=globals/includes/bookmark.h \
        framework/include/log.h \
        framework/include/Notifier.h \
        globals/includes/qxmleditdata.h \
        globals/includes/utils.h \
        globals/includes/element.h \
        globals/includes/navigation.h \
        globals/includes/regola.h \
        globals/includes/xmleditglobals.h \
        globals/includes/paintinfo.h \
        globals/includes/style.h	\
        globals/includes/xmlEdit.h \
        globals/includes/libQXmlEdit_global.h \
        globals/includes/plugin.h \
        framework/include/UIDelegate.h \
        globals/includes/xmleditwidget.h

libinclude.path=$$INST_INCLUDE_DIR


INCLUDEPATH += globals/includes
INCLUDEPATH += framework/include

SOURCES += xmleditwidget.cpp \
  alloweditemsinsert.cpp \
  balsamiqsrc/balsamiqcontrol.cpp \
  balsamiqsrc/balsamiqproxy.cpp \
  balsamiqsrc/balsamiqplugin.cpp \
  balsamiqsrc/balsamiqwork.cpp \
  balsamiqsrc/balsamiqbasedialog.cpp \
  balsamiqsrc/balsamiqtree.cpp \
  bookmark.cpp \
  config.cpp \
  editelement.cpp \
  editcomment.cpp \
  editprocessinginstruction.cpp \
  paintinfo.cpp \
  element.cpp \
  edittextnode.cpp \
  findtextparams.cpp \
  navigationbox.cpp \
  regola.cpp \
  test.cpp \
  texteditutils.cpp \
  operationresult.cpp \
  style.cpp \
  stylepersistence.cpp \
  utils.cpp \
  validationresults.cpp \
  validatormessagehandler.cpp \
  schemavalidator.cpp \
  showtextdialog.cpp \
  widgets/navigationmode.cpp \
  xsdeditor/xsdplugin.cpp \
  xmlutils.cpp \  
  xsaxhandler.cpp \
    xsdeditor/xsdwindow.cpp \
    xsdeditor/XSchemaChoice.cpp \
    xsdeditor/XSDSchemaSequence.cpp \
    xsdeditor/XSDSchema.cpp \
    xsdeditor/XSDScene.cpp \
    xsdeditor/XSchemaObject.cpp \
    xsdeditor/XSchemaElement.cpp \
    xsdeditor/XSchemaAttribute.cpp \
    xsdeditor/widgets/elementeditor.cpp \
    xsdeditor/widgets/widgetutility.cpp \
    xsdeditor/widgets/xsdrooteditor.cpp \
    xsdeditor/widgets/xsdattributeeditor.cpp \
    xsdeditor/widgets/xsdchoiceeditor.cpp \
    xsdeditor/widgets/xsdsequenceeditor.cpp \
    xsdeditor/xsdsctypes.cpp \
    xsdeditor/XSCContent.cpp \
    xsdeditor/items/XSchemaCompositeItems.cpp \
    xsdeditor/items/XSchemaBaseItems.cpp \
    xsdeditor/items/XSchemaDerivedItems.cpp \
    xsdeditor/items/graphicitems.cpp \
    xsdeditor/XsdException.cpp \
    xsdeditor/XAnnotation.cpp \
    xsdeditor/XSchemaTopLevel.cpp \
    xsdeditor/widgets/xsdgenericeditor.cpp \
    xsdeditor/XFactory.cpp \
    xsdeditor/xsdgraphiccontext.cpp \
    xsdeditor/xsdcontroller.cpp \
    xsdeditor/validator/xsingleelementcontent.cpp \
    xsdeditor/validator/xcollect.cpp \
    xsdeditor/validator/xelementcontent.cpp \
    xsdeditor/validator/xvalidationcontext.cpp \
    xsdeditor/navigationtree.cpp \
    qxmleditdata.cpp \
    xmleditwidgetprivate.cpp \
    xsdeditor/validator/schemasearchcontext.cpp \
    xmleditglobals.cpp \
    documenttype.cpp \
    undo/undocommand.cpp \
    undo/undomanager.cpp \
    undo/undomoveupcommand.cpp \
    undo/undomovedowncommand.cpp \
    undo/undoinsertcommand.cpp \
    undo/undodeletecommand.cpp \
    undo/undobasechangeelement.cpp \
    undo/undoeditcommand.cpp \
    xsdeditor/xsdcompare.cpp \
    xsdeditor/xsdcomparedata.cpp \
    xsdeditor/XSchemaCompare.cpp \
    xsdeditor/xsdcomparetools.cpp \
    xsdeditor/xcomparesummary.cpp \
    xsdeditor/xsdgraphicsconfiguration.cpp \
    xsdeditor/xsdbackgroundconfig.cpp \
    attributefilter.cpp \
    attributefilterdialog.cpp \
    modules/compare/comparemodule.cpp \
    modules/compare/diffresult.cpp \
    modules/compare/compareengine.cpp \
    modules/compare/compareresulttextformat.cpp \
    modules/compare/comparesidebysideview.cpp \
    modules/compare/compareexception.cpp \
    modules/compare/diffmapwidget.cpp \
    modules/compare/comparechrome.cpp \
    modules/graph/nodesrelationsdialog.cpp \
    modules/graph/tagnodes.cpp \
    modules/graph/tagmarker.cpp \
    modules/graph/tagspring.cpp \
    modules/graph/nodesrelationscontroller.cpp \
    extraction/extractfragmentsdialog.cpp \
    extraction/extractionfrontend.cpp \
    extraction/extractionoperation.cpp \
    extraction/extractresults.cpp \
    modules/graph/nodessax.cpp \
    modules/graph/numtablewidgetitem.cpp \
    modules/search/xqueryelementmodel.cpp \
    modules/search/searchxquery.cpp \
    modules/copyattr/copyattributesdialog.cpp \
    modules/copyattr/pasteattributesdialog.cpp \
    modules/uiutil/wchecker.cpp \
    modules/copyattr/copiedattribute.cpp \
    undo/undopasteattributescommand.cpp \
    modules/widgets/lineeditwithcompleter.cpp \
    modules/search/searchresultsdialog.cpp \
    modules/search/searchwidget.cpp \
    xmlwidgetutilities.cpp \
    modules/xslt/xslthelper.cpp \
    modules/xslt/xsltelementdialog.cpp \
    modules/xslt/xsltelement.cpp \
    modules/xslt/XsltHelperItems.cpp \
    modules/xslt/xsltmanager.cpp \
    modules/xslt/saxnamesscan.cpp \
    modules/search/searchmanager.cpp \
    modules/xslt/xsltautomodedialog.cpp \
    xsdeditor/XSchemaCollectionUtilities.cpp \
    modules/services/systemservices.cpp \
    modules/binaryviewer/binaryviewerdialog.cpp \
    modules/binaryviewer/binaryviewermodel.cpp \
    modules/binaryviewer/binaryblock.cpp \
    modules/search/searchletdialog.cpp \
    modules/search/searchletmanager.cpp \
    modules/search/editsearchletdialog.cpp \
    modules/metadata/basecomplexvariable.cpp \
    modules/metadata/metadataparser.cpp \
    modules/metadata/metadatainfo.cpp \
    modules/metadata/metadatainfodialog.cpp \
    modules/metadata/prologparser.cpp \
    modules/xml/metadata.cpp \
    modules/xml/export.cpp \
    modules/xml/prolog.cpp \
    modules/metadata/xmlprolog.cpp \
    modules/xml/elementPaint.cpp \
    modules/delegates/elementitemsingledelegate.cpp \
    modules/delegates/elementviewinfo.cpp \
    modules/encoding/codepagedialog.cpp \
    modules/encoding/codepageitemdelegate.cpp \
    modules/delegates/attributecolumnitemdelegate.cpp \
    modules/services/colormanager.cpp \
    modules/services/colorentry.cpp \
    undo/undoaddchildcontainer.cpp \
    modules/xml/regolaContainer.cpp \
    undo/undoaddparentcommand.cpp \
    undo/undoremoveparentcommand.cpp \
    xsdeditor/XSchemaTypeEngine.cpp \
    xsdeditor/XSchemaContainer.cpp \
    xsdeditor/XsdTypes.cpp \
    xsdeditor/xschemaoperationcontext.cpp \
    xsdeditor/io/xschemaloaderhelper.cpp \
    xsdeditor/XSchemaInfoPool.cpp \
    xsdeditor/XSchemaNameResolver.cpp \
    xsdeditor/names/schemaLists.cpp \
    xsdeditor/io/SchemaIO.cpp \
    xsdeditor/items/XSchemaReferenceItems.cpp \
    xsdeditor/io/xschemaloader.cpp \
    xsdeditor/names/schemaSingleObject.cpp \
    modules/services/anotifier.cpp \
    xsdeditor/xsdloadcontext.cpp \
    modules/encoding/unicodehelper.cpp \
    modules/xsd/xsdelementdefinition.cpp \
    modules/xsd/xsdvalidationexecutor.cpp \
    modules/xsd/xsdtypedialog.cpp \
    modules/xsd/xsdelementreferencedialog.cpp \
    modules/xsd/xsdenumdialog.cpp \
    modules/xsd/xsdeditortypes.cpp \
    modules/xsd/xsdhelper.cpp \
    modules/xsd/xsdoperationparameters.cpp \
    undo/elinsertcommand.cpp \
    undo/elupdateelementcommand.cpp \
    undo/elbasecommand.cpp \
    modules/xsd/xsdhelperoperations.cpp \
    modules/xsd/xsdoperationfactory.cpp \
    modules/xsd/xsdmanager.cpp \
    modules/xsd/XSDHelperElement.cpp \
    modules/anonymize/anonElem.cpp \
    modules/anonymize/anoncontext.cpp \
    modules/anonymize/anonbase.cpp \
    modules/anonymize/anonexception.cpp \
    modules/anonymize/anonprofile.cpp \
    modules/anonymize/anonymizeparameters.cpp \
    modules/anonymize/anonfixedalg.cpp \
    modules/anonymize/anoncodealg.cpp \
    modules/anonymize/anonallalg.cpp \
    modules/anonymize/anonseqproducer.cpp \
    modules/anonymize/anonattr.cpp \
    modules/anonymize/anonoperationbatch.cpp \
    modules/anonymize/anonnullalg.cpp \
    modules/xml/elementPath.cpp \
    modules/xml/elmpath.cpp \
    modules/anonymize/xmlanonutils.cpp \
    modules/search/elmfindtext.cpp \
    modules/xsd/xsdsinglecommentdialog.cpp \
    modules/xsd/xsdfullannotationsdialog.cpp \
    modules/xsd/xsddefaultannotationeditor.cpp \
    xsdeditor/XSchemaOther.cpp \
    modules/xsd/xsdannotationmodel.cpp \
    undo/undocommandgroup.cpp \
    modules/xml/xmlindentationdialog.cpp \
    modules/xsd/XSDWidgetValidation.cpp \
    widgets/qlabelwithsignals.cpp \
    widgets/selectionchoosedelegate.cpp \
    xmlsavecontext.cpp \
    modules/xsd/xsdschemainstance.cpp \
    modules/xsd/namespacemanager.cpp \
    modules/xml/elementNS.cpp \
    modules/xsd/xsiinsertattributedialog.cpp \
    modules/namespace/usernamespace.cpp \
    modules/namespace/namespacemanagementdialog.cpp \
    modules/namespace/choosenamespacedialog.cpp \
    modules/namespace/editnamespacedialog.cpp \
    modules/namespace/namespacecommands.cpp \
    modules/namespace/nstableutils.cpp \
    modules/namespace/usernamespaceloader.cpp \
    modules/utils/base64utils.cpp \
    modules/xsd/schemareferencesdialog.cpp \
    modules/namespace/namespacereferenceentry.cpp \
    modules/xml/insertxsdreference.cpp \
    modules/xml/xmlio.cpp \
    modules/xml/xmlloadcontext.cpp \
    undo/undodtd.cpp \
    modules/replica/replicacommand.cpp \
    modules/replica/replicamanager.cpp \
    modules/replica/replicasettingsdialog.cpp \
    undo/elupdateinplacecommand.cpp \
    undo/undoreplicacommand.cpp \
    modules/replica/replicacloneinfo.cpp \
    modules/xml/xmlattributesindentationhelper.cpp \
    modules/xml/sortAttributes.cpp \
    undo/undosortattributes.cpp \
    undo/undodeletesiblings.cpp \
    modules/anonymize/anonoperationbatchoutputfileprovider.cpp \
    xsdeditor/xsdfacet.cpp \
    modules/xsd/choosefacetdialog.cpp \
    undo/editxsdenumcommand.cpp \
    modules/xml/xmlnamespaces.cpp \
    modules/delegates/automitemeditorfactory.cpp \
    undo/prefixremovecommand.cpp \
    modules/namespace/regolaprefixes.cpp \
    undo/writeelementundoobserver.cpp \
    undo/undodatareply.cpp \
    modules/namespace/nscontext.cpp \
    undo/namespaceremovecommand.cpp \
    undo/prefixaddcommand.cpp \
    widgets/elementselectiontypewidget.cpp \
    undo/namespacesetcommand.cpp \
    undo/namespacereplacecommand.cpp \
    undo/namespacenormalizecommand.cpp \
    undo/prefixreplacecommand.cpp \
    modules/namespace/elementnamespace.cpp \
    modules/namespace/elementprefix.cpp \
    undo/namespaceavoidclashcommand.cpp \
    modules/specialized/xinclude/xincludedialog.cpp \
    modules/specialized/scxml/scxmlroot.cpp \
    modules/specialized/specificpropertiesdialog.cpp \
    modules/specialized/xinclude/xincludeeditormanager.cpp \
    undo/undosimpleeditcommand.cpp \
    modules/specialized/scxml/scxmleditormanager.cpp \
    modules/specialized/scxml/scxmltokenmanager.cpp \
    modules/specialized/scxml/scxmltoken.cpp \
    modules/specialized/scxml/scxmltokenloader.cpp \
    modules/specialized/scxml/scxmltokens.cpp \
    modules/specialized/scxml/scxmlinfo.cpp \
    modules/specialized/scxml/dialogs/baseddata.cpp \
    modules/specialized/scxml/dialogs/scxmlstatedialog.cpp \
    modules/specialized/scxml/dialogs/scxmlparalleldialog.cpp \
    modules/specialized/scxml/dialogs/scxmltransitiondialog.cpp \
    modules/specialized/scxml/dialogs/basescxmleditdata.cpp \
    modules/specialized/scxml/dialogs/scxmlfinaldialog.cpp \
    modules/specialized/scxml/dialogs/scxmlhistoryldialog.cpp \
    modules/specialized/scxml/widgets/scxmlnavigatorwidget.cpp \
    modules/specialized/scxml/dialogs/scxmlraisedialog.cpp \
    modules/specialized/scxml/dialogs/scxmlifdialog.cpp \
    modules/specialized/scxml/dialogs/scxmlelseifdialog.cpp \
    modules/specialized/scxml/dialogs/scxmlforeachdialog.cpp \
    modules/specialized/scxml/dialogs/scxmllogdialog.cpp \
    modules/specialized/scxml/dialogs/scxmldatadialog.cpp \
    modules/specialized/scxml/dialogs/scxmlassigndialog.cpp \
    modules/specialized/scxml/dialogs/scxmlcontentdialog.cpp \
    modules/specialized/scxml/dialogs/scxmlparamdialog.cpp \
    modules/specialized/scxml/dialogs/scxmlscriptdialog.cpp \
    modules/specialized/scxml/dialogs/scxmlsenddialog.cpp \
    modules/specialized/scxml/dialogs/scxmlinvokedialog.cpp \
    modules/xslt/xsltnavigatorwidget.cpp \
    modules/specialized/scxml/scxmlautomodedialog.cpp \
    modules/xslt/xsleditormanager.cpp \
    modules/services/uidservices.cpp \
    modules/messages/sourceerror.cpp \
    editelementwithtexteditor.cpp \
    modules/xml/xmlsyntaxh.cpp \
    modules/services/threadservices.cpp \
    modules/metadata/formattinginfo.cpp \
    undo/commandaddformattinginfo.cpp \
    undo/commandremoveformattinginfo.cpp \
    xsdeditor/items/xschemaoutlintitems.cpp \
    xsdeditor/xschemaoutlineelement.cpp \
    xsdeditor/xschemaoutlinecontainer.cpp \
    xsdeditor/choosexsdviewrootitemdialog.cpp \
    xsdeditor/items/xschemabaseitemlayout.cpp \
    visualization/attributessummarydata.cpp \
    xsdeditor/xsdprint.cpp \
    xsdeditor/xsdreport.cpp \
    xsdeditor/choosexsdreporttypedialog.cpp \
    xsdeditor/report/xsdprintinfo.cpp \
    xsdeditor/report/xsdprintinfohtml.cpp \
    modules/uiutil/uidelegatebase.cpp

HEADERS += precompiled_lib.h \
        globals/includes/bookmark.h \
        framework/include/log.h \
        globals/includes/qxmleditdata.h \
        globals/includes/utils.h \
        globals/includes/element.h \
        globals/includes/navigation.h \
        globals/includes/regola.h \
        globals/includes/xmleditglobals.h \
        globals/includes/paintinfo.h \
        globals/includes/style.h	\
        globals/includes/xmlEdit.h \
        globals/includes/libQXmlEdit_global.h \
        globals/includes/plugin.h \
        framework/include/UIDelegate.h \
        globals/includes/xmleditwidget.h \
    alloweditemsinsert.h \
    balsamiqsrc/balsamiqproxy.h \
    balsamiqsrc/balsamiqplugin.h \
    balsamiqsrc/balsamiqwork.h \
    balsamiqsrc/balsamiqcontrol.h \
    balsamiqsrc/balsamiqbasedialog.h \
    balsamiqsrc/balsamiqtree.h \
    qxmleditconfig.h \
    editelement.h \
    edittextnode.h \
    editcomment.h \
    editprocessinginstruction.h \
    navigationbox.h \
    operationresult.h \
    stylepersistence.h \
    showtextdialog.h \
    widgets/navigationmode.h \
    xsdeditor/xsdplugin.h \
    xsdeditor/xschema.h \
    xsdeditor/xsdcontroller.h \
    xsdeditor/navigationtree.h \
    xsdeditor/xsdgraphiccontext.h \
    xsdeditor/widgets/elementeditor.h \
    xsdeditor/xtypes.h \
    xsdeditor/xsdtypes.h \
    xsdeditor/widgets/XEditorInterface.h \
    xsdeditor/xproperties.h \
    xsdeditor/widgets/WidgetUtility.h \
    xsdeditor/widgets/xsdrooteditor.h \
    xsdeditor/widgets/xsdattributeeditor.h \
    xsdeditor/widgets/xsdchoiceeditor.h \
    xsdeditor/widgets/xsdsequenceeditor.h \
    xsdeditor/xsdsctypes.h \
    xsdeditor/xsdcontent.h \
    xsdeditor/xsddefinitions.h \
    xsdeditor/XSchemaIOContants.h \
    xsdeditor/items/xgraphicsitems.h \
    xsdeditor/xsdparseutils.h \
    xsdeditor/widgets/xsdgenericeditor.h \
    xsdeditor/validator/xelementcontent.h \
    xsdeditor/validator/xvalidationcontext.h \
    xsdeditor/validator/xsingleelementcontent.h \
    xsdeditor/xsdwindow.h \
    findtextparams.h \
    UIDelegate.h \
    xmleditwidgetprivate.h \
    xsdeditor/validator/schemasearchcontext.h \
    documenttype.h \
    undo/undocommand.h \
    undo/undomanager.h \
    undo/undomoveupcommand.h \
    undo/undomovedowncommand.h \
    undo/undoinsertcommand.h \
    undo/undodeletecommand.h \
    undo/undobasechangeelement.h \
    undo/undoeditcommand.h \
    xsdeditor/xsdcompare.h \
    xsdeditor/xsdcomparedata.h \
    xsdeditor/xsdcomparetools.h \
    xsdeditor/xcomparesummary.h \
    xsdeditor/xsdgraphicsconfiguration.h \
    xsdeditor/xsdbackgroundconfig.h \
    attributefilterdialog.h \
    modules/compare/comparemodule.h \
    modules/compare/diffresult.h \
    modules/compare/compareengine.h \
    modules/compare/compareresulttextformat.h \
    modules/compare/comparesidebysideview.h \
    modules/compare/compareexception.h \
    modules/compare/diffmapwidget.h \
    modules/compare/comparechrome.h \
    modules/graph/nodesrelationsdialog.h \
    modules/graph/tagnodes.h \
    modules/graph/tagmarker.h \
    modules/graph/tagspring.h \
    modules/graph/nodesrelationscontroller.h \
    extraction/extractfragmentsdialog.h \
    extraction/extractionfrontend.h \
    extraction/extractionoperation.h \
    extraction/extractresults.h \
    modules/graph/nodessax.h \
    modules/graph/numtablewidgetitem.h \
    modules/graph/tagzorder.h \
    modules/search/xqueryelementmodel.h \
    modules/search/searchxquery.h \
    modules/copyattr/copyattributesdialog.h \
    modules/copyattr/pasteattributesdialog.h \
    modules/uiutil/wchecker.h \
    modules/copyattr/copiedattribute.h \
    undo/undopasteattributescommand.h \
    modules/widgets/lineeditwithcompleter.h \
    modules/widgets/ICompleter.h \
    modules/search/searchresultsdialog.h \
    modules/search/searchwidget.h \
    xmlwidgetutilities.h \
    modules/xslt/xsltmode.h \
    modules/xslt/xslthelper.h \
    modules/xslt/xsltelementdialog.h \
    modules/xslt/xsltelement.h \
    globals/includes/qtincludes.h \
    modules/xslt/xsltmanager.h \
    modules/xslt/saxnamesscan.h \
    modules/search/searchmanager.h \
    modules/xslt/xsltautomodedialog.h \
    modules/services/systemservices.h \
    modules/binaryviewer/binaryviewerdialog.h \
    modules/binaryviewer/binaryviewermodel.h \
    modules/binaryviewer/binaryblock.h \
    globals/includes/data/DataInterface.h \
    globals/includes/data/GenericPersistentData.h \
    modules/search/searchletdialog.h \
    modules/search/searchletmanager.h \
    modules/search/editsearchletdialog.h \
    modules/metadata/basecomplexvariable.h \
    modules/metadata/metadataparser.h \
    modules/metadata/metadatainfo.h \
    modules/metadata/metadatainfodialog.h \
    modules/metadata/prologparser.h \
    regoladefinitions.h \
    globals/includes/xmlprolog.h \
    modules/delegates/elementitemsingledelegate.h \
    modules/delegates/elementviewinfo.h \
    modules/encoding/codepagedialog.h \
    modules/encoding/codepageitemdelegate.h \
    modules/delegates/attributecolumnitemdelegate.h \
    modules/services/colormanager.h \
    modules/services/colorentry.h \
    undo/undoaddchildcontainer.h \
    undo/undoaddparentcommand.h \
    undo/undoremoveparentcommand.h \
    xsdeditor/xschemaoperationcontext.h \
    xsdeditor/io/xschemaloaderhelper.h \
    xsdeditor/items/xitemsdefinitions.h \
    xsdeditor/io/xschemaloader.h \
    framework/include/Notifier.h \
    modules/services/anotifier.h \
    xsdeditor/xsdloadcontext.h \
    modules/encoding/unicodehelper.h \
    modules/xsd/xsdelementdefinition.h \
    modules/xsd/xsdtypedialog.h \
    modules/xsd/xsdelementreferencedialog.h \
    modules/xsd/xsdenumdialog.h \
    modules/xsd/xsdhelper.h \
    modules/xsd/xsdoperationparameters.h \
    undo/elinsertcommand.h \
    undo/elupdateelementcommand.h \
    undo/elbasecommand.h \
    modules/xsd/xsdhelperoperations.h \
    modules/xsd/xsdoperationfactory.h \
    modules/xsd/xsdmanager.h \
    modules/anonymize/anoncontext.h \
    modules/anonymize/anonbase.h \
    modules/anonymize/anonexception.h \
    modules/anonymize/anonprofile.h \
    modules/anonymize/anonymizeparameters.h \
    modules/anonymize/anonfixedalg.h \
    modules/anonymize/anoncodealg.h \
    modules/anonymize/anonallalg.h \
    modules/anonymize/anonseqproducer.h \
    modules/anonymize/anonattr.h \
    modules/anonymize/anonoperationbatch.h \
    modules/anonymize/anonnullalg.h \
    modules/xml/elmpath.h \
    modules/anonymize/xmlanonutils.h \
    modules/xsd/xsdsinglecommentdialog.h \
    modules/xsd/xsdfullannotationsdialog.h \
    modules/xsd/xsddefaultannotationeditor.h \
    modules/xsd/xsdannotationmodel.h \
    undo/undocommandgroup.h \
    modules/xml/xmlindentationdialog.h \
    widgets/qlabelwithsignals.h \
    widgets/selectionchoosedelegate.h \
    xmlsavecontext.h \
    modules/xsd/namespacemanager.h \
    modules/xsd/xsiinsertattribute.h \
    modules/namespace/usernamespace.h \
    modules/namespace/namespacemanagementdialog.h \
    modules/namespace/choosenamespacedialog.h \
    modules/namespace/editnamespacedialog.h \
    modules/namespace/namespacecommands.h \
    modules/namespace/nstableutils.h \
    modules/namespace/usernamespaceloader.h \
    modules/utils/base64utils.h \
    modules/namespace/namespaceresult.h \
    modules/xsd/schemareferencesdialog.h \
    modules/namespace/namespacereferenceentry.h \
    modules/xml/xmlloadcontext.h \
    undo/undodtd.h \
    modules/replica/replicacommand.h \
    modules/replica/replicamanager.h \
    modules/replica/replicasettingsdialog.h \
    undo/elupdateinplacecommand.h \
    undo/undoreplicacommand.h \
    modules/replica/replicacloneinfo.h \
    modules/xml/xmlattributesindentationhelper.h \
    undo/undosortattributes.h \
    undo/undodeletesiblings.h \
    modules/xsd/choosefacetdialog.h \
    undo/editxsdenumcommand.h \
    modules/delegates/automitemeditorfactory.h \
    undo/prefixremovecommand.h \
    undo/writeelementundoobserver.h \
    undo/undodatareply.h \
    modules/namespace/nscontext.h \
    undo/namespaceremovecommand.h \
    undo/prefixaddcommand.h \
    widgets/elementselectiontypewidget.h \
    undo/namespacesetcommand.h \
    undo/namespacereplacecommand.h \
    undo/namespacenormalizecommand.h \
    undo/prefixreplacecommand.h \
    undo/namespaceavoidclashcommand.h \
    modules/specialized/xinclude/xincludedialog.h \
    modules/specialized/scxml/scxmlroot.h \
    modules/specialized/specificpropertiesdialog.h \
    modules/specialized/xinclude/xincludeeditormanager.h \
    undo/undosimpleeditcommand.h \
    modules/specialized/scxml/scxmleditormanager.h \
    modules/specialized/scxml/scxmltokenmanager.h \
    modules/specialized/scxml/scxmltoken.h \
    modules/specialized/scxml/scxmltokenloader.h \
    modules/specialized/scxml/scxmlinfo.h \
    modules/specialized/scxml/dialogs/baseddata.h \
    modules/specialized/scxml/dialogs/scxmlstatedialog.h \
    modules/specialized/scxml/dialogs/scxmlparalleldialog.h \
    modules/specialized/scxml/dialogs/scxmltransitiondialog.h \
    modules/specialized/scxml/dialogs/basescxmleditdata.h \
    modules/specialized/scxml/dialogs/scxmlfinaldialog.h \
    modules/specialized/scxml/dialogs/scxmlhistoryldialog.h \
    modules/specialized/scxml/widgets/scxmlnavigatorwidget.h \
    modules/specialized/scxml/dialogs/scxmlraisedialog.h \
    modules/specialized/scxml/dialogs/scxmlifdialog.h \
    modules/specialized/scxml/dialogs/scxmlelseifdialog.h \
    modules/specialized/scxml/dialogs/scxmlforeachdialog.h \
    modules/specialized/scxml/dialogs/scxmllogdialog.h \
    modules/specialized/scxml/dialogs/scxmldatadialog.h \
    modules/specialized/scxml/dialogs/scxmlassigndialog.h \
    modules/specialized/scxml/dialogs/scxmlcontentdialog.h \
    modules/specialized/scxml/dialogs/scxmlparamdialog.h \
    modules/specialized/scxml/dialogs/scxmlscriptdialog.h \
    modules/specialized/scxml/dialogs/scxmlsenddialog.h \
    modules/specialized/scxml/dialogs/scxmlinvokedialog.h \
    modules/xslt/xsltnavigatorwidget.h \
    modules/specialized/scxml/scxmlautomodedialog.h \
    modules/xslt/xsleditormanager.h \
    modules/services/uidservices.h \
    modules/messages/sourceerror.h \
    editelementwithtexteditor.h \
    modules/xml/xmlsyntaxh.h \
    modules/services/threadservices.h \
    undo/commandaddformattinginfo.h \
    undo/commandremoveformattinginfo.h \
    xsdeditor/xschemaoutlineelement.h \
    xsdeditor/choosexsdviewrootitemdialog.h \
    visualization/attributessummarydata.h \
    xsdeditor/xsdreport.h \
    xsdeditor/choosexsdreporttypedialog.h \
    modules/xsd/xsdvalidationexecutor.h


FORMS += xmleditwidget.ui \
    alloweditemsinsert.ui \
    EditElement.ui \
    EditTextNode.ui \
    EditProcessingInstruction.ui \
    balsamiqsrc/balsamiqui.ui \
    EditComment.ui \
    navigationbox.ui \
    showtextdialog.ui \
    widgets/navigationmode.ui \
    xsdeditor/xsdwindow.ui \
    xsdeditor/widgets/elementeditor.ui \
    xsdeditor/widgets/xsdrooteditor.ui \
    xsdeditor/widgets/xsdattributeeditor.ui \
    xsdeditor/widgets/xsdchoiceeditor.ui \
    xsdeditor/widgets/xsdsequenceeditor.ui \
    xsdeditor/widgets/xsdgenericeditor.ui \
    xsdeditor/xsdcomparetools.ui \
    xsdeditor/xcomparesummary.ui \
    xsdeditor/xsdbackgroundconfig.ui \
    attributefilterdialog.ui \
    modules/compare/comparemodule.ui \
    modules/compare/diffmapwidget.ui \
    modules/graph/nodesrelationsdialog.ui \
    extraction/extractfragmentsdialog.ui \
    extraction/extractionfrontend.ui \
    modules/copyattr/copyattributesdialog.ui \
    modules/copyattr/pasteattributesdialog.ui \
    modules/search/searchresultsdialog.ui \
    modules/search/searchwidget.ui \
    modules/xslt/xsltelementdialog.ui \
    modules/xslt/xsltautomodedialog.ui \
    modules/binaryviewer/binaryviewerdialog.ui \
    modules/search/searchletdialog.ui \
    modules/search/editsearchletdialog.ui \
    modules/metadata/metadatainfodialog.ui \
    modules/encoding/codepagedialog.ui \
    modules/xsd/xsdtypedialog.ui \
    modules/xsd/xsdelementreferencedialog.ui \
    modules/xsd/xsdenumdialog.ui \
    modules/xsd/xsdsinglecommentdialog.ui \
    modules/xsd/xsdfullannotationsdialog.ui \
    modules/xml/xmlindentationdialog.ui \
    modules/namespace/namespacemanagementdialog.ui \
    modules/namespace/choosenamespacedialog.ui \
    modules/namespace/editnamespacedialog.ui \
    modules/xsd/schemareferencesdialog.ui \
    modules/replica/replicasettingsdialog.ui \
    modules/xsd/choosefacetdialog.ui \
    widgets/elementselectiontypewidget.ui \
    modules/specialized/xinclude/xincludedialog.ui \
    modules/specialized/scxml/scxmlroot.ui \
    modules/specialized/specificpropertiesdialog.ui \
    modules/specialized/scxml/dialogs/scxmlstatedialog.ui \
    modules/specialized/scxml/dialogs/scxmlparalleldialog.ui \
    modules/specialized/scxml/dialogs/scxmltransitiondialog.ui \
    modules/specialized/scxml/dialogs/scxmlfinaldialog.ui \
    modules/specialized/scxml/dialogs/scxmlhistoryldialog.ui \
    modules/specialized/scxml/widgets/scxmlnavigatorwidget.ui \
    modules/specialized/scxml/dialogs/scxmlraisedialog.ui \
    modules/specialized/scxml/dialogs/scxmlifdialog.ui \
    modules/specialized/scxml/dialogs/scxmlelseifdialog.ui \
    modules/specialized/scxml/dialogs/scxmlforeachdialog.ui \
    modules/specialized/scxml/dialogs/scxmllogdialog.ui \
    modules/specialized/scxml/dialogs/scxmldatadialog.ui \
    modules/specialized/scxml/dialogs/scxmlassigndialog.ui \
    modules/specialized/scxml/dialogs/scxmlcontentdialog.ui \
    modules/specialized/scxml/dialogs/scxmlparamdialog.ui \
    modules/specialized/scxml/dialogs/scxmlscriptdialog.ui \
    modules/specialized/scxml/dialogs/scxmlsenddialog.ui \
    modules/specialized/scxml/dialogs/scxmlinvokedialog.ui \
    modules/xslt/xsltnavigatorwidget.ui \
    modules/specialized/scxml/scxmlautomodedialog.ui \
    editelementwithtexteditor.ui \
    xsdeditor/choosexsdviewrootitemdialog.ui \
    xsdeditor/xsdreport.ui \
    xsdeditor/choosexsdreporttypedialog.ui

symbian {
    #Symbian specific definitions
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE61059D2
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QXmlEditWidget.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
#        target.path = /usr/local/lib
        target.path = $$INST_LIB_DIR
    }
    INSTALLS += target translationsfiles libinclude
}

RESOURCES += risorse.qrc

# resources are defined in paths.h
unix:!macx:DEFINES += UNIX_RESOURCES

DEFINES += UNIX_RESOURCE_PATH=$$INST_DATA_DIR
DEFINES += UNIX_DOC_PATH=$$INST_DOC_DIR

