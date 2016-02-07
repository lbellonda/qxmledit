/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2016 by Luca Bellonda and individual contributors  *
 *    as indicated in the AUTHORS file                                    *
 *  lbellonda _at_ gmail.com                                              *
 *                                                                        *
 * This library is free software; you can redistribute it and/or          *
 * modify it under the terms of the GNU Library General Public            *
 * License as published by the Free Software Foundation; either           *
 * version 2 of the License, or (at your option) any later version.       *
 *                                                                        *
 * This library is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 * Library General Public License for more details.                       *
 *                                                                        *
 * You should have received a copy of the GNU Library General Public      *
 * License along with this library; if not, write to the                  *
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,       *
 * Boston, MA  02110-1301  USA                                            *
 **************************************************************************/


#include "testxsltmode.h"
#include "app.h"
#include "modules/xslt/xsltmode.h"
#include "modules/xslt/xslthelper.h"
#include "modules/xslt/xsltelementdialog.h"
#include "modules/xslt/xsltnavigatordialog.h"
#include "modules/widgets/lineeditwithcompleter.h"
#include "xmleditwidget.h"
#include "comparexml.h"
#include "helpers/comparetrees.h"
#include "modules/copyattr/copiedattribute.h"

#define INSERT_TRUE (true)
#define EDIT_TRUE (false)
#define X_INSERT_ELEMENT    (true)
#define X_EDIT_ELEMENT    (false)


#define FILE_BASE   "../test/data/xslt/base.xslt"
#define FILE_NAMES   "../test/data/xslt/names.xml"
#define FILE_INS_TEMPLATE   "../test/data/xslt/t_instemplate.xslt"
#define FILE_EDIT_TEMPLATE   "../test/data/xslt/t_edittemplate.xslt"
#define FILE_EDIT_TEMPLATE_MATCH   "../test/data/xslt/t_edittemplatematch.xslt"
#define FILE_NAVIGATION   "../test/data/xslt/templatefunctions.xslt"
#define FILE_BASE_XML   "../test/data/xslt/base.xml"
#define FILE_BASE_NOEXT "../test/data/xslt/base.aaa"
#define FILE_BASE_PARAM   "../test/data/xslt/base_param.xslt"
#define FILE_BASE_PARAM1_EXPECTED    "../test/data/xslt/base_param_t1.xml"
#define FILE_BASE_PARAM2_EXPECTED    "../test/data/xslt/base_param_t2.xml"


#define ALL_ITEMS_APPEND   "Aapply-templates", "Aattribute", "Acall-template", "Achoose", "Acomment", "Acopy-of",\
"Aelement", "Afor-each", "Afor-each-group", "Aif" , "Asequence", "Avalue-of", "Avariable"

TestXSLTMode::TestXSLTMode()
{
}

bool TestXSLTMode::testBaseMode()
{
    _testName = "testBaseMode" ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    XmlEditWidgetEditMode::EditMode editMode = mainWindow.getEditor()->editMode();
    if( XmlEditWidgetEditMode::XSLT == editMode ) {
        return error("Editor should not start in xslt edit mode");
    }
    mainWindow.getEditor()->setEditMode(XmlEditWidgetEditMode::XSLT);
    editMode = mainWindow.getEditor()->editMode();
    if( XmlEditWidgetEditMode::XSLT != editMode ) {
        return error(QString("Set editor xslt edit mode, but found %1").arg(editMode));
    }

    return true;
}


bool TestXSLTMode::checkNamespace(MainWindow *mainWindow)
{
    QList<int> selection1;
    selection1.append(1); // root
    selection1.append(6); //template
    selection1.append(1); //fake template
    Element *selectedElement = mainWindow->getRegola()->findElementByArray(selection1);
    if(NULL == selectedElement) {
        return error("checkNamespace select 1");
    }
    mainWindow->getEditor()->setCurrentItem(selectedElement);

    XsltHelper *helper = mainWindow->getEditor()->XSLTHelper();
    XslContext *context = helper->findContext( selectedElement, mainWindow->getRegola()->namespacePrefixXslt()) ;
    if( NULL == context ) {
        return error("context not created");
    }
    if( context->levels.isEmpty() ) {
        delete context;
        return error("context is empty");
    }

    if( context->levels.first()->isXsl ) {
        delete context;
        return error("context is not xsl");
    }
    delete context;
    return true;
}

bool TestXSLTMode::checkMenuChoose()
{
    _testName = "testAdvancedMode/checkMenuChoose";
    QList<int> selection;
    selection.append(1); // root
    selection.append(5); //template
    selection.append(2); //empty choose
    QSet<QString> expected;
    insertItems(&expected, "Itemplate", "Ifunction", "Iwhen" ,"Iotherwise", ALL_ITEMS_APPEND, NULL);

    return checkMenuItemsBase("choose", selection, expected );
}

bool TestXSLTMode::checkMenuCall()
{
    _testName = "testAdvancedMode/checkMenuCall";
    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(4); //call template

    QSet<QString> expected;
    insertItems(&expected, "Itemplate", "Ifunction", "Iwith-param" ,"Isort",  ALL_ITEMS_APPEND, NULL);

    return checkMenuItemsBase("call-template", selection, expected );
}

bool TestXSLTMode::checkMenuItemsBase(const QString &tag, QList<int> &selection, QSet<QString> &itemsToFind )
{
    return checkMenuItems(FILE_BASE, tag, selection, itemsToFind );
}

bool TestXSLTMode::checkMenuItems(const QString &filePath, const QString &tag, QList<int> &selection, QSet<QString> &itemsToFind )
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(QString(filePath)) ) {
        return error(QString("opening test file: '%1'").arg(filePath));
    }

    mainWindow.getEditor()->setEditMode(XmlEditWidgetEditMode::XSLT);

    Element *selectedElement = mainWindow.getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("checkCall select 1");
    }
    mainWindow.getEditor()->setCurrentItem(selectedElement);
    QString simpleTag = selectedElement->localName();
    if(tag != simpleTag ) {
        return error(QString("wrong element, looking for '%1', but found '%2'").arg(tag).arg(simpleTag));
    }

    QMenu contextMenu ;
    mainWindow.getEditor()->addXsltMenuItems(&contextMenu);

    QSet<QString> compareSet ;
    QList<QAction*> actions = contextMenu.findChildren<QAction*>();
    foreach( QAction* action, actions ) {
        if( !action->objectName().isEmpty() ) {
            compareSet.insert( action->objectName() );
        }
    }

    return compareStringSets( _testName, itemsToFind, compareSet);
}


bool TestXSLTMode::testAdvancedMode()
{
    _testName = "testAdvancedMode" ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(QString(FILE_BASE)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE));
    }

    mainWindow.getEditor()->setEditMode(XmlEditWidgetEditMode::XSLT);
    // ready
    if(!checkNamespace(&mainWindow)) {
        return false;
    }
    //----------------------------------------------------------------
    if(!checkMenuCall()) {
        return false;
    }

    if(!checkMenuChoose()) {
        return false;
    }

    //----------------------------------------------------------------

    if(!checkDCallTemplate()) {
        return false;
    }
    if(!checkDChoose()) {
        return false;
    }
    if(!checkDWhen()) {
        return false;
    }
    if(!checkDOtherwise()) {
        return false;
    }
    if(!checkDApplyTemplate()) {
        return false;
    }
    if(!checkDIf()) {
        return false;
    }
    if(!checkDIfTargetDocument()) {
        return false;
    }
    /*if(!checkDParam(&mainWindow)) {
        return false;
    }
    if(!checkDWithParam(&mainWindow)) {
        return false;
    }*/

    if(!checkInsNewTemplate()) {
        return false;
    }
    /*if(!checkInsChoose(&mainWindow)) {
        return false;
    }
    if(!checkInsWhen(&mainWindow)) {
        return false;
    }
    */
    if(!checkEditTemplate()) {
        return false;
    }


/*

        controllo stile applicato;

        controllo elementi suggeriti

controllo nomi template e funzioni in inserimento

seleziona un oggetto
chiama il context menu
controlla se e stata inserita laction
fai un fire action e controlla dialog, controlla i nomi nel combo.
selez call-template
controlla se il combo ha i parametri

select choose controlla se i figli hanno otherwise e when


controlla se new ytemplate e attivo

cerca with param in un if
*/
    return true;
}



bool TestXSLTMode::checkWithDialog(XsltElementDialogParam *param, const bool shouldNameEnabled, const bool shouldSelectEnabled,
                                   QSet<QString> &nameItems, QSet<QString> &selectItems)
{

    XsltElementDialog dlg(param->xsltElement, param->element, param->baseElement, param->helper, param->path, param->parent);
    QComboBox *nameAttrib = dlg.findChild<QComboBox*>("comboName");
    LineEditWithCompleter *selectAttrib= dlg.findChild<LineEditWithCompleter*>("selectAttr");

    //check for activation
    if( nameAttrib->isEnabled() != shouldNameEnabled ) {
        return error(QString("Name combo activation differs. Found:%1").arg(nameAttrib->isEnabled()));
    }

    if( selectAttrib->isEnabled() != shouldSelectEnabled ) {
        return error(QString("Select edit activation differs. Found:%1").arg(selectAttrib->isEnabled()));
    }

    // check for name data
    if( shouldNameEnabled ) {
        QSet<QString> comboItems;
        for( int i = 0 ; i < nameAttrib->count() ; i ++ ) {
            QString key = nameAttrib->itemText(i);
            comboItems.insert(key);
        }

        if(!compareStringSets( "name attribute", nameItems, comboItems)) {
            return false;
        }
    }

    // check for autocompletion values
    if( shouldSelectEnabled ) {
        QSet<QString> data = selectAttrib->data();
        if(!compareStringSets( "select attribute", selectItems, data)) {
            return false;
        }
    }

    return true ;
}


//---- dtests
bool TestXSLTMode::innerDTest( const QString &nameElem, QList<int> &selection, QSet<QString> &nameItems, QSet<QString> &selectItems, const bool namesEnabled, const bool selectEnabled )
{
    return innerDTest( nameElem, "", selection, nameItems, selectItems, namesEnabled, selectEnabled );
}

bool TestXSLTMode::innerDTest( const QString &nameElem, const QString &namesFileAuto, QList<int> &selection, QSet<QString> &nameItems, QSet<QString> &selectItems, const bool namesEnabled, const bool selectEnabled )
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(QString(FILE_BASE)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE));
    }

    mainWindow.getEditor()->setEditMode(XmlEditWidgetEditMode::XSLT);
    // ready

    Element *selectedElement = mainWindow.getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("select 1");
    }
    QStringList lst = selectedElement->tag().split(":");
    if( nameElem != lst.last() ) {
        return error(QString("the selected element is not the right one, expected '%1', found '%2'").arg(nameElem).arg(lst.last()));
    }

    XsltElementDialogParam params;
    XsltHelper *helper = mainWindow.getEditor()->XSLTHelper();
    if( !namesFileAuto.isEmpty()) {
        if( !helper->loadNamesFromFile(namesFileAuto) ) {
            return error(QString("Unable to load names for autocompletion '%1'").arg(namesFileAuto));
        }
    }

    if(!helper->prepareEditElement(selectedElement, &params)) {
        return error("params preparation");
    }

    if( !checkWithDialog(&params, namesEnabled, selectEnabled, nameItems, selectItems) ) {
        return false;
    }
    return true;
}

//--

bool TestXSLTMode::checkDCallTemplate()
{
    _testName = "testAdvancedMode/checkDCallTemplate" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(4); //call-template

    QSet<QString> nameItems ;
    QSet<QString> selectItems;

    insertItems( &nameItems, "one", NULL );
    insertItems( &selectItems, "$var1", "$var2", "$var3", NULL );

    if( !innerDTest("call-template", selection, nameItems, selectItems, true, false ) ) {
        return false;
    }
    return true;
}


bool TestXSLTMode::checkDChoose()
{
    _testName = "testAdvancedMode/checkDChoose" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(3); //choose

    QSet<QString> nameItems ;
    QSet<QString> selectItems;

    if( !innerDTest("choose", selection, nameItems, selectItems, false, false ) ) {
        return false;
    }
    return true;
}

bool TestXSLTMode::checkDWhen()
{
    _testName = "testAdvancedMode/checkDWhen" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(3); //choose
    selection.append(0); //when

    QSet<QString> nameItems ;
    QSet<QString> selectItems;
    insertItems( &selectItems, "$var1", "$var2", "$var3", NULL );

    if( !innerDTest("when", selection, nameItems, selectItems, false, true ) ) {
        return false;
    }
    return true;
}

bool TestXSLTMode::checkDOtherwise()
{
    _testName = "testAdvancedMode/checkDOtherwise" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(3); //choose
    selection.append(2); //otherwise

    QSet<QString> nameItems ;
    QSet<QString> selectItems;

    if( !innerDTest("otherwise", selection, nameItems, selectItems, false, false ) ) {
        return false;
    }
    return true;
}

bool TestXSLTMode::checkDApplyTemplate()
{
    _testName = "testAdvancedMode/checkDApplyTemplate" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(5); //apply-templates

    QSet<QString> nameItems ;
    QSet<QString> selectItems;

    insertItems( &selectItems, "$var1", "$var2", "$var3", NULL );

    if( !innerDTest("apply-templates", selection, nameItems, selectItems, false, true ) ) {
        return false;
    }
    return true;
}

bool TestXSLTMode::checkDIf()
{
    _testName = "testAdvancedMode/checkDIf" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(2); //if

    QSet<QString> nameItems ;
    QSet<QString> selectItems;

    insertItems( &selectItems, "$var1", "$var2", "$var3", NULL );

    if( !innerDTest("if", selection, nameItems, selectItems, false, true ) ) {
        return false;
    }
    return true;
}

bool TestXSLTMode::checkDIfTargetDocument()
{
    _testName = "testAdvancedMode/checkDIfTargetDocument" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(2); //if

    QSet<QString> nameItems ;
    QSet<QString> selectItems;

    insertItems( &selectItems, "$var1", "$var2", "$var3", "root", "a", "b", "c", "d", "@a1", "@a2", NULL );

    if( !innerDTest("if", FILE_NAMES, selection, nameItems, selectItems, false, true ) ) {
        return false;
    }
    return true;
}


/*
bool TestXSLTMode::checkDParam(MainWindow *mainWindow)
{
    return false;
}

bool TestXSLTMode::checkDWithParam(MainWindow *mainWindow)
{
    return false;
}

checkInsNewTemplate(MainWindow *mainWindow)
{
    return false;
}

bool TestXSLTMode::checkInsChoose(MainWindow *mainWindow)
{
    return false;
}

bool TestXSLTMode::checkInsWhen(MainWindow *mainWindow)
{
    return false;
}
*/

bool TestXSLTMode::setDialogValues( const bool isWaitDialog, XsltElementDialogParam *param,
                                   const QString &nameItem, const QString &selectItem, const bool isInsertOrEdit, const bool isInsertOrAppendElement)
{

    XsltElementDialog dlg(param->xsltElement, param->element, param->baseElement, param->helper, param->path, param->parent);
    QComboBox *nameAttrib = dlg.findChild<QComboBox*>("comboName");
    LineEditWithCompleter *selectAttrib= dlg.findChild<LineEditWithCompleter*>("selectAttr");

    //check for activation
    nameAttrib->setEditText(nameItem);
    selectAttrib->setText(selectItem);
    if( isWaitDialog ) {
        dlg.exec();
        param->useParams = dlg.useParam();
    } else {
        dlg.accept();
    }
    CopyAttributesSession *cas = dlg.getAndClearResult();
    if( NULL == cas ) {
        if( NULL != param->element ) {
            delete param->element;
        }
        return error(QObject::tr("attributes edit dialog : result negative") );
    }
    if(isInsertOrEdit) {
        param->helper->insertElement(param, cas, isInsertOrAppendElement);
    } else {
        param->helper->execEditElement(cas, param->element, param->xsltElement->isClearAttributes, false);
    }
    if( NULL != cas ) {
        cas->clear();
        delete cas ;
    }

    return true ;
}

bool TestXSLTMode::innerDEditOp( const QString &selectedElem, const QString &nameElem, const QString &fileOrig, const QString &fileAfterOperation, QList<int> &selection, const QString &nameItem, const QString &selectItem, const bool isInsertOrEdit, const bool isInsertOrAppendElement)
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(fileOrig) ) {
        return error(QString("opening test file: '%1'").arg(fileOrig));
    }
    Regola *regola = mainWindow.getEditor()->getRegola();
    mainWindow.getEditor()->setEditMode(XmlEditWidgetEditMode::XSLT);
    // ready

    Element *selectedElement = mainWindow.getRegola()->findElementByArray(selection);
    if(NULL == selectedElement) {
        return error("select 1");
    }
    QStringList lst = selectedElement->tag().split(":");
    if( selectedElem != lst.last() ) {
        return error(QString("the selected element is not the right one, expected '%1', found '%2'").arg(nameElem).arg(lst.last()));
    }

    XsltElementDialogParam params;
    XsltHelper *helper = mainWindow.getEditor()->XSLTHelper();
    if(isInsertOrEdit) {
        XsltElement *xsltElement  = helper->elementFromName(nameElem);
        if(!helper->prepareInsertElement(&params, isInsertOrAppendElement, xsltElement )) {
            return error("params preparation for insert");
        }
    } else {
        if(!helper->prepareEditElement(selectedElement, &params)) {
            return error("params preparation for edit");
        }
    }

    if( !setDialogValues( false, &params, nameItem, selectItem, isInsertOrEdit, isInsertOrAppendElement) ) {
        return error("setDialogValues");
    }

    // compare with file
    CompareXML compare;
    if(!compare.compareFileWithRegola(regola, fileAfterOperation)) {
        return error(QString("comparing the results 1: %1").arg(compare.errorString()));
    }
    // undo
    regola->undo();
    if(!compare.compareFileWithRegola(regola, fileOrig)) {
        return error(QString("comparing the undo results: %1").arg(compare.errorString()));
    }
    // redo
    regola->redo();
    if(!compare.compareFileWithRegola(regola, fileAfterOperation)) {
        return error(QString("comparing the results after redo: %1").arg(compare.errorString()));
    }
    return true;
}

bool TestXSLTMode::checkInsNewTemplate()
{
    _testName = "testAdvancedMode/checkInsNewTemplate" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(6); //template
    selection.append(2); //if

    if( !innerDEditOp( "if", "template", FILE_BASE, FILE_INS_TEMPLATE, selection, "ttest", "", INSERT_TRUE, X_INSERT_ELEMENT) ) {
        return false;
    }
    return true;
}


bool TestXSLTMode::checkEditTemplate()
{
    _testName = "testAdvancedMode/checkEditTemplateName" ;

    QList<int> selection;
    selection.append(1); // root
    selection.append(5); //template

    if( !innerDEditOp( "template", "template", FILE_BASE, FILE_EDIT_TEMPLATE, selection, "ttest", "", EDIT_TRUE, X_INSERT_ELEMENT) ) {
        return false;
    }
    _testName = "testAdvancedMode/checkEditTemplateSelect" ;
    if( !innerDEditOp( "template", "template", FILE_BASE, FILE_EDIT_TEMPLATE_MATCH, selection, "", "/", EDIT_TRUE, X_INSERT_ELEMENT) ) {
        return false;
    }

    return true;
}

//------------------------------------

bool TestXSLTMode::testNavigation()
{
    _testName = "testNavigation" ;
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(QString(FILE_NAVIGATION)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_NAVIGATION));
    }

    mainWindow.getEditor()->setEditMode(XmlEditWidgetEditMode::XSLT);
    // ready
    if(!checkNavigationSort(&mainWindow)) {
        return false;
    }
    //----------------------------------------------------------------
    if(!checkNavigationEnable(&mainWindow)) {
        return false;
    }

    if(!checkNavigationGoto(&mainWindow)) {
        return false;
    }

    //----------------------------------------------------------------
    return true ;
}

//----------------------------------------------------------------

bool TestXSLTMode::checkNavigationSort(MainWindow *mainWindow) {
    _testName = "testNavigation/checkNavigationSort" ;
    XSLTNavigatorDialog::testToExecute=0;
    if( !fireAction(mainWindow, "actionShowXSLNavigator") ) {
        return error("firing actionShowXSLNavigator");
    }
    if( !XSLTNavigatorDialog::testPassed ) {
        return error(XSLTNavigatorDialog::testMsg);
    }
    return true;
}

bool TestXSLTMode::checkNavigationEnable(MainWindow *mainWindow) {
    _testName = "testNavigation/checkNavigationEnable" ;
    XSLTNavigatorDialog::testToExecute=1;
    if( !fireAction(mainWindow, "actionShowXSLNavigator") ) {
        return error("firing actionShowXSLNavigator");
    }
    if( !XSLTNavigatorDialog::testPassed ) {
        return error(XSLTNavigatorDialog::testMsg);
    }
    return true;
}

bool TestXSLTMode::checkNavigationGoto(MainWindow *mainWindow) {
    _testName = "testNavigation/checkNavigationGoto" ;
    XSLTNavigatorDialog::testToExecute=2;
    if( !fireAction(mainWindow, "actionShowXSLNavigator") ) {
        return error("firing actionShowXSLNavigator");
    }
    if( !XSLTNavigatorDialog::testPassed ) {
        return error(XSLTNavigatorDialog::testMsg);
    }
    // after the accept, the element should be selected
    Element *element = mainWindow->getEditor()->getSelectedItem();
    if( NULL == element ) {
        return error("no selected element");
    }
    QList<int> path = element->indexPath();

    QList<int> selection;
    selection.append(1); // root
    selection.append(7); //template

    if(!compareListInts("selection", selection, path)) {
        return false;
    }
    return true;
}

//----------


bool TestXSLTMode::testPanel()
{
    _testName = "testPanel" ;
    //testa flag di pannello con xml e xslt: mostra, no mostra
    if( !testPanelShowOnOpenFileXml()) {
        return false;
    }
    if( !testPanelShowOnOpenFileXsl()) {
        return false;
    }
    //testa combinazioni flag mostra con flag auto, 4 combinazioni apertura pannello
    if( !testPanelShowFlagsNoHideNoAuto() ) {
        return false;
    }
    if( !testPanelShowFlagsNoHideAuto() ) {
        return false;
    }
    if( !testPanelShowFlagsHideNoAuto() ) {
        return false;
    }
    if( !testPanelShowFlagsHideAuto() ) {
        return false;
    }
    if(!testPanelShowWithAnswers() ) {
        return false;
    }
    //----------------------------------------------------------------
    return true ;
}

bool TestXSLTMode::testPanelShowOnOpenFile(const QString &fileName, const XSLTAutoModeDialog::PrivateTest::Tests testToDo, const bool expected)
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    XSLTAutoModeDialog::PrivateTest::setTestToExecute(testToDo);
    XSLTAutoModeDialog::PrivateTest::setPanelOpen(false);
    if( !mainWindow.loadFile(QString(fileName)) ) {
        return error(QString("opening test file: '%1'").arg(fileName));
    }
    if( expected != XSLTAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("test failed panel shown:%1 with file '%2', expected:%3, found:%4")
                     .arg(XSLTAutoModeDialog::PrivateTest::testMessage()).arg(fileName).arg(expected).arg(XSLTAutoModeDialog::PrivateTest::panelOpen()));
    }

    //----------------------------------------------------------------
    return true ;
}


bool TestXSLTMode::testPanelShowOnOpenFileXml()
{
    _testName = "testPanel/testPanelShowOnOpenFileXml" ;
    return testPanelShowOnOpenFile(FILE_BASE_XML, XSLTAutoModeDialog::PrivateTest::TEST_SHOWPANEL_XML_FILE, false);
}

bool TestXSLTMode::testPanelShowOnOpenFileXsl()
{
    _testName = "testPanel/testPanelShowOnOpenFileXsl" ;
    return testPanelShowOnOpenFile(FILE_BASE, XSLTAutoModeDialog::PrivateTest::TEST_SHOWPANEL_XSL_FILE, true);
}

bool TestXSLTMode::testPanelShowOnOpenFileUsingFlags(const XSLTAutoModeDialog::PrivateTest::Tests testToDo, const bool isShow, const bool isAuto, const bool expected )
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    app.data()->setAutoXSLTMode(isAuto);
    app.data()->setShowXSLTPanel(isShow);
    XSLTAutoModeDialog::PrivateTest::setPanelOpen(false);
    XSLTAutoModeDialog::PrivateTest::setTestToExecute(testToDo);
    if( !mainWindow.loadFile(QString(FILE_BASE)) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE));
    }
    if( expected != XSLTAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("test failed panel shown:%1 with file '%2' flag was show:%3, auto:%4, expected:%5, found:%6")
                     .arg(XSLTAutoModeDialog::PrivateTest::testMessage()).arg(FILE_BASE)
                     .arg(isShow).arg(isAuto).arg(expected).arg(XSLTAutoModeDialog::PrivateTest::panelOpen()));
    }

    //----------------------------------------------------------------
    return true ;
}

bool TestXSLTMode::testPanelShowFlagsNoHideNoAuto()
{
    _testName = "testPanel/testPanelShowFlagsNoHideNoAuto" ;
    return testPanelShowOnOpenFileUsingFlags(XSLTAutoModeDialog::PrivateTest::TEST_SHOW_NOHIDE_NOAUTO, true, false, true);
}

bool TestXSLTMode::testPanelShowFlagsNoHideAuto()
{
    _testName = "testPanel/testPanelShowFlagsNoHideAuto" ;
    return testPanelShowOnOpenFileUsingFlags(XSLTAutoModeDialog::PrivateTest::TEST_SHOW_NOHIDE_AUTO, true, true, false);
}

bool TestXSLTMode::testPanelShowFlagsHideNoAuto()
{
    _testName = "testPanel/testPanelShowFlagsHideNoAuto" ;
    return testPanelShowOnOpenFileUsingFlags(XSLTAutoModeDialog::PrivateTest::TEST_SHOW_HIDE_NOAUTO, false, false, false);
}

bool TestXSLTMode::testPanelShowFlagsHideAuto()
{
    _testName = "testPanel/testPanelShowFlagsHideAuto" ;
    return testPanelShowOnOpenFileUsingFlags(XSLTAutoModeDialog::PrivateTest::TEST_SHOW_HIDE_AUTO, false, false, false);
}

bool TestXSLTMode::testPanelAnswers( const QString &fileName,
                                    const XSLTAutoModeDialog::PrivateTest::Tests testToDo, const bool isShow, const XSLTAutoModeDialog::ERetCode answer,
                                    const bool expectedFirstShot, const bool expectedOpenSecondShot,
                                    const bool expectedXSLAfterFirstShot, const bool expectedXSLAfterSecondShot )
{
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    XSLTAutoModeDialog::PrivateTest::setTestToExecute(testToDo);
    XSLTAutoModeDialog::PrivateTest::setAnswerShowXSLTPanel(isShow);
    XSLTAutoModeDialog::PrivateTest::setAnswer(answer);
    if( !mainWindow.loadFile(QString(fileName)) ) {
        return error(QString("1. opening test file: '%1'").arg(fileName));
    }
    if( expectedFirstShot != XSLTAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("test failed panel with file:'%1'' flag was show:%2, answer:%3, expected open:%4, found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(XSLTAutoModeDialog::PrivateTest::panelOpen()));
    }

    if( !XSLTAutoModeDialog::PrivateTest::testPassed()) {
        return error(QString("test failed with message panel with message '%6' file:'%1' flag was show:%2, answer:%3, expected open:%4, found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(XSLTAutoModeDialog::PrivateTest::panelOpen())
                     .arg(XSLTAutoModeDialog::PrivateTest::testMessage()));
    }

    if( expectedXSLAfterFirstShot != (mainWindow.getEditor()->editMode() == XmlEditWidgetEditMode::XSLT )) {
        return error(QString(" xml edit mode after first shot was not expected file:'%1' flag was show:%2, answer:%3, edit mode exp shot:%4, edit mode found:%5, test:%6")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(mainWindow.getEditor()->editMode())
                     .arg(testToDo));

    }

    XSLTAutoModeDialog::PrivateTest::setPanelOpen(false);
    // second shot
    if( !mainWindow.loadFile(QString(fileName)) ) {
        return error(QString("2. opening test file: '%1'").arg(fileName));
    }
    if( expectedOpenSecondShot != XSLTAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("Second shot test failed shown: with file '%1' flag was show:%2, answer:%3, expected first shot:%4, found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(XSLTAutoModeDialog::PrivateTest::panelOpen()));
    }
    if( expectedXSLAfterSecondShot != (mainWindow.getEditor()->editMode() == XmlEditWidgetEditMode::XSLT )) {
        return error(QString(" xml edit mode after second shot was not expected file:'%1' flag was show:%2, answer:%3, edit mode exp shot:%4, edit mode found:%5")
                     .arg(fileName)
                     .arg(isShow).arg(answer).arg(expectedXSLAfterSecondShot).arg(mainWindow.getEditor()->editMode()));

    }

    // loading a regular xml
    XSLTAutoModeDialog::PrivateTest::setPanelOpen(false);
    if( !mainWindow.loadFile(QString(FILE_BASE_XML)) ) {
        return error(QString("3. opening test file: '%1'").arg(FILE_BASE_XML));
    }
    if( XSLTAutoModeDialog::PrivateTest::panelOpen() ) {
        return error(QString("Loading regular XML with file '%1' flag was show:%2, answer:%3, expected first shot:%4, found:%5")
                     .arg(FILE_BASE_XML)
                     .arg(isShow).arg(answer).arg(expectedFirstShot).arg(XSLTAutoModeDialog::PrivateTest::panelOpen()));
    }
    if( mainWindow.getEditor()->editMode() != XmlEditWidgetEditMode::XML) {
        return error(QString(" regular xml did not trigger xml edit mode file:'%1' flag was show:%2, answer:%3, edit mode found:%4")
                     .arg(FILE_BASE_XML)
                     .arg(isShow).arg(answer).arg(mainWindow.getEditor()->editMode()));

    }
    //----------------------------------------------------------------
    return true ;
}

struct TestInfoData {
    QString testName;
    XSLTAutoModeDialog::PrivateTest::Tests testToDo;
    bool isShow;
    XSLTAutoModeDialog::ERetCode answer;
    bool expectedFirstShot;
    bool expectedOpenSecondShot;
    bool expectedXSLAfterFirstShot;
    bool expectedXSLAfterSecondShot;
};

bool TestXSLTMode::testPanelShowWithAnswers()
{
    TestInfoData testData[] = {
        { "Ys", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, true, XSLTAutoModeDialog::ENTER_XSLMODE, true, true, true, true },
        { "Yn", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, XSLTAutoModeDialog::ENTER_XSLMODE, true, false, true, false },
        { "Ns", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, true, XSLTAutoModeDialog::DONOTENTER_XSLMODE, true, true, false, false },
        { "Nn", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, XSLTAutoModeDialog::DONOTENTER_XSLMODE, true, false, false, false },
        { "As", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, true, XSLTAutoModeDialog::ALWAYS_XSLMODE, true, false, true, true },
        { "An", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, XSLTAutoModeDialog::ALWAYS_XSLMODE, true, false, true, true },
        // guard
        { "", XSLTAutoModeDialog::PrivateTest::TEST_BEHAVIOUR, false, XSLTAutoModeDialog::ENTER_XSLMODE, false, false, true, true },
    };
    /*int size = sizeof(testData)/sizeof(testData[0]); // hoping for the best
    for( int i = 0 ; i < size ; i ++ ) {
        TestInfoData *test = testData[i];
    }*/
    TestInfoData *test = &testData[0];
    while( !test->testName.isEmpty() ) {
        _testName = QString("TestXSLTMode/infoTest/%1").arg(test->testName);
        if( !testPanelAnswers(FILE_BASE, test->testToDo, test->isShow, test->answer, test->expectedFirstShot, test->expectedOpenSecondShot, test->expectedXSLAfterFirstShot, test->expectedXSLAfterSecondShot) ) {
            return false;
        }
        test++;
        //printf("test...\n");
    }
    test = &testData[0];
    if( !testPanelAnswers(FILE_BASE_NOEXT, test->testToDo, test->isShow, test->answer, test->expectedFirstShot, test->expectedOpenSecondShot, test->expectedXSLAfterFirstShot, test->expectedXSLAfterSecondShot) ) {
        return false;
    }
    return true;
}



/*
testa flag always al caricamento con XML e xslt

testa risposta 3 *sinosempre con flag mostra e no mostra
caricamento successivo

start | Y|N |A | <- answer
first |sn|sn|sn| <- show/not show at the first start
---------------
xsl?  |yy|nn|yy|
second|on|on|nn|
xsl?  |yn|nn|yy|
xml   |nn|nn|nn|

with xml on the second run nothing will open

*/

bool TestXSLTMode::testInsParameters()
{
    _testName = "testInsParameters/ins-true" ;
    // load test
    if(!intTestInsParameters(true, FILE_BASE_PARAM1_EXPECTED)) {
        return false;
    }
    _testName = "testInsParameters/ins-false" ;
    if(!intTestInsParameters(false, FILE_BASE_PARAM2_EXPECTED)) {
        return false;
    }
    return true;
}

bool TestXSLTMode::intTestInsParameters(const bool isParameters, const QString &fileReference)
{
    // load test
    App app;
    if(!app.initNoWindow() ) {
        return error("init window");
    }
    MainWindow mainWindow(false, qApp, app.data());
    if( !mainWindow.loadFile(FILE_BASE_PARAM) ) {
        return error(QString("opening test file: '%1'").arg(FILE_BASE_PARAM));
    }
    // insert ----------------------------------------------------------------------------
    // ins params
    QList<int> selection;
    selection << 1;
    selection << 6;
    selection << 1;
    Element *selectedElement = mainWindow.getRegola()->findElementByArray(selection);
    if( NULL == selectedElement ) {
        return error(QString("error selecting element"));
    }
    mainWindow.getEditor()->setCurrentItem(selectedElement);
    if( isParameters) {
        XsltElementDialog::PrivateTest::setCheckParameters(true);
    } else {
        XsltElementDialog::PrivateTest::setGo(true);
    }

    XsltElementDialogParam params;
    XsltHelper *helper = mainWindow.getEditor()->XSLTHelper();
    QString nameElem = "call-template";
    bool isInsertOrEdit = true ;
    bool isInsertOrAppendElement = true;
    if(isInsertOrEdit) {
        XsltElement *xsltElement  = helper->elementFromName(nameElem);
        if(!helper->prepareInsertElement(&params, isInsertOrAppendElement, xsltElement )) {
            return error("params preparation for insert");
        }
    } else {
        if(!helper->prepareEditElement(selectedElement, &params)) {
            return error("params preparation for edit");
        }
    }

    if( !setDialogValues(true, &params, "one", "", isInsertOrEdit, isInsertOrAppendElement) ) {
        return error("setDialogValues");
    }

    // compare xmls
    CompareXML compare;
    //mainWindow.getRegola()->write("/tmp/1.xml", false);
    if( !compare.compareFileWithRegola(mainWindow.getRegola(), fileReference)) {
        compare.dumpErrorCause();
        return error(QString("edited dom differs from file %1 cause:%2").arg(fileReference).arg(compare.errorString()));
    }

    // compare tree
    MainWindow mainWindow2(false, qApp, app.data());
    if( !mainWindow2.loadFile(fileReference) ) {
        return error(QString("opening compare file: '%1'").arg(fileReference));
    }
    CompareTrees ct;
    if( !ct.compareTrees(mainWindow.getEditor()->getMainTreeWidget(), mainWindow2.getEditor()->getMainTreeWidget())) {
        return error(QString("trees differs '%1'").arg(ct.error()));
    }

    return true ;
}
