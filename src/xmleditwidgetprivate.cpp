/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2017 by Luca Bellonda and individual contributors  *
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

#include "xmlEdit.h"
#include "xmleditwidgetprivate.h"

#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QFileInfo>
#include <QShortcut>
#include "xmleditwidget.h"
#include "xsdeditor/io/xschemaloader.h"
#include "validatormessagehandler.h"
#include "schemavalidator.h"
#include "elementitemdelegate.h"
#include "alloweditemsinsert.h"
#include "qattributedelegate.h"
#include "qxmleditconfig.h"
#include "findtextparams.h"
#include "regola.h"
#include "utils.h"
#include "xsdeditor/xsdplugin.h"
#include "extraction/extractresults.h"
#include "xmleditwidgetprivate.h"
#include "xsdeditor/xsdcompare.h"
#include "modules/graph/nodesrelationsdialog.h"
#include "modules/widgets/lineeditwithcompleter.h"
#include "modules/search/searchresultsdialog.h"
#include "xmlwidgetutilities.h"
#include "modules/metadata/metadatainfodialog.h"
#include "modules/encoding/codepagedialog.h"
#include "modules/delegates/elementitemsingledelegate.h"
#include "modules/xsd/xsdhelper.h"
#include "modules/xsd/xsdtypedialog.h"
#include "modules/xsd/xsdhelper.h"
#include "modules/xsd/xsddefaultannotationeditor.h"
#include "modules/xml/xmlindentationdialog.h"
#include "modules/xsd/schemareferencesdialog.h"
#include "modules/namespace/namespacereferenceentry.h"
#include "modules/xml/xmlloadcontext.h"
#include "modules/replica/replicasettingsdialog.h"
#include "modules/replica/replicamanager.h"
#include "undo/undoreplicacommand.h"
#include "modules/copyattr/copiedattribute.h"
#include "undo/undodeletesiblings.h"
#include "modules/xsd/xsdenumdialog.h"
#include "undo/editxsdenumcommand.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "undo/prefixremovecommand.h"
#include "undo/namespaceremovecommand.h"
#include "undo/prefixaddcommand.h"
#include "undo/prefixreplacecommand.h"
#include "undo/namespacesetcommand.h"
#include "undo/namespacereplacecommand.h"
#include "undo/namespacenormalizecommand.h"
#include "undo/namespaceavoidclashcommand.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/specialized/scxml/scxmlinfo.h"
#include "modules/specialized/scxml/scxmleditormanager.h"

void ShowTextInDialog(QWidget *parent, const QString &text);

#define CS_ELEMENT_TEXT "E"
#define CS_ELEMENT_TAG "T"
#define CS_ATTRIBUTE_NAME "N"
#define CS_ATTRIBUTE_VALUE "V"

XmlEditWidgetPrivate::XmlEditWidgetPrivate(XmlEditWidget *theOwner):
    styleActionGroup(this), _xsltHelper(this)
{
    p = theOwner ;
    _SCXMLNavigator = NULL ;
    _XSLTNavigator = NULL ;
    _readOnly = false ;
    _copyPathAction = NULL ;
    _xsltAction = NULL ;
    _SCXMLAction = NULL ;
    _editMode = XmlEditWidgetEditMode::XML;
    _appData = &_defaultData;
    _XSDAnnotationEditProvider = this ;
    started = false ;
    internalStateOk = false;
    paintInfo.setColorManager(_appData->colorManager());

    _updateTimer.setSingleShot(true);
    _updateTimer.setInterval(1000);
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(updateTimeout()));
    _uiDelegate = NULL ;
    _schemaRoot = NULL ;
    attrDelegate = NULL ;
    _displayMode = qxmledit::NORMAL;
    paintInfo.setExploreMode(false);
    isSlave = false ;

    styleMenu = NULL ;
    copyMenu = NULL ;

    paintInfo.loadState();
    regola = newRegola(false);
    regola->assignCollectSizeDataFlag(paintInfo.showElementSize());
}

Regola *XmlEditWidgetPrivate::newRegola(const bool bind)
{
    Regola *newRule = new Regola();
    houseworkRegola(newRule);
    newRule->setPaintInfo(&paintInfo);
    newRule->setNamespaceManager(_appData->namespaceManager());
    bindRegola(newRule, bind);
    return newRule ;
}

void XmlEditWidgetPrivate::secondStepConstructor()
{
    internalStateOk = finishSetUpUi();
    if(!internalStateOk) {
        Utils::error(tr("Error preparing user interface."));
    }
    onEncodingChanged(regola->encoding());
    init();
    p->setAcceptDrops(true);

    connect(p->ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(elementDoubleClicked(QTreeWidgetItem *, int)));
    QTreeWidget *tree = getMainTreeWidget();
    //--------------- delete -----------
    {
        QShortcut *shortcutDelete = new QShortcut(tree);
        shortcutDelete->setKey(Qt::Key_Delete);
        connect(shortcutDelete, SIGNAL(activated()), this, SLOT(onShortcutDelete()));
    }
    {
        QShortcut *shortcutDelete = new QShortcut(tree);
        shortcutDelete->setKey(Qt::Key_Backspace);
        connect(shortcutDelete, SIGNAL(activated()), this, SLOT(onShortcutDelete()));
    }
    //--------------- insert -----------
    // INS
    {
        QShortcut *shortcutInsert = new QShortcut(tree);
        shortcutInsert->setKey(Qt::Key_Insert);
        connect(shortcutInsert, SIGNAL(activated()), this, SLOT(onShortcutInsert()));
    }
    // I
    {
        QShortcut *shortcutInsert1 = new QShortcut(tree);
        shortcutInsert1->setKey(Qt::Key_I);
        connect(shortcutInsert1, SIGNAL(activated()), this, SLOT(onShortcutInsert()));
    }
    //--------------- insert spec -----------
    // SHIFT INS
    {
        QShortcut *shortcutAppend = new QShortcut(tree);
        shortcutAppend->setKey(Qt::Key_Insert + Qt::SHIFT);
        connect(shortcutAppend, SIGNAL(activated()), this, SLOT(onShortcutInsertSpec()));
    }
    // SHIFT I
    {
        QShortcut *shortcutAppend1 = new QShortcut(tree);
        shortcutAppend1->setKey(Qt::Key_I + Qt::SHIFT);
        connect(shortcutAppend1, SIGNAL(activated()), this, SLOT(onShortcutInsertSpec()));
    }
    //--------------- append -----------
    // ALT INS
    {
        QShortcut *shortcutInsertSpec1 = new QShortcut(tree);
        shortcutInsertSpec1->setKey(Qt::Key_Insert + Qt::ALT);
        connect(shortcutInsertSpec1, SIGNAL(activated()), this, SLOT(onShortcutAppend()));
    }
    // A
    {
        QShortcut *shortcutAppend2 = new QShortcut(tree);
        shortcutAppend2->setKey(Qt::Key_A);
        connect(shortcutAppend2, SIGNAL(activated()), this, SLOT(onShortcutAppend()));
    }
    //--------------- append spec -----------
    // SHIFT A
    {
        QShortcut *shortcutInsertSpec = new QShortcut(tree);
        shortcutInsertSpec->setKey(Qt::Key_A + Qt::SHIFT);
        connect(shortcutInsertSpec, SIGNAL(activated()), this, SLOT(onShortcutAppendSpec()));
    }

    // ALT SHIFT INS
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Insert + Qt::ALT + Qt::SHIFT);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutAppendSpec()));
    }
    //-------------------------------------
    // inspection
    // SHIFT Enter
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Return + Qt::SHIFT);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutShiftEnter()));
    }
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Enter + Qt::SHIFT);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutShiftEnter()));
    }
    // CTRL Enter
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Return + Qt::CTRL);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutCtrlEnter()));
    }
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Enter + Qt::CTRL);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutCtrlEnter()));
    }
    // ALT Enter
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Return + Qt::ALT);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutAltEnter()));
    }
    // ALT Enter
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Enter + Qt::ALT);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutAltEnter()));
    }
    // Enter
    Utils::TODO_THIS_RELEASE("cassato");
    /*{
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_Return);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutEnter()));
    }*/
    // T
    {
        QShortcut *shortcutAppendSpec = new QShortcut(tree);
        shortcutAppendSpec->setKey(Qt::Key_T);
        connect(shortcutAppendSpec, SIGNAL(activated()), this, SLOT(onShortcutT()));
    }
    // end inspection

    recalcRowHeightClass();

    started = true ;
    internalStateOk = true ;

}

XmlEditWidgetPrivate::~XmlEditWidgetPrivate()
{
    _updateTimer.stop();
    resetStyleMenu();
    deleteRegola();
    deleteSchema();
    if(NULL != _XSDAnnotationEditProvider) {
        _XSDAnnotationEditProvider->autoDelete();
    }
}

bool XmlEditWidgetPrivate::isReady()
{
    return started && internalStateOk;
}

XmlEditWidgetEditMode::EditMode XmlEditWidgetPrivate::editMode()
{
    return _editMode;
}

void XmlEditWidgetPrivate::setEditMode(const XmlEditWidgetEditMode::EditMode newMode)
{
    _editMode = newMode ;
    if(XmlEditWidgetEditMode::XSLT == _editMode) {
        if(NULL != _xsltAction) {
            _xsltAction->setChecked(true);
            _xsltAction->trigger();
        }
    } else if(XmlEditWidgetEditMode::SCXML == _editMode) {
        if(NULL != _SCXMLAction) {
            _SCXMLAction->setChecked(true);
            _SCXMLAction->trigger();
        }
    }
    emit p->editModeChanged();
}

void XmlEditWidgetPrivate::deleteSchema()
{
    if(NULL != _schemaRoot) {
//        disconnect(_schemaRoot, SIGNAL(loadComplete(XSDSchema*)), this, SLOT(schemaLoadComplete(XSDSchema*, const bool)));
        delete _schemaRoot ;
        _schemaRoot = NULL ;
    }
}

/*
//autotest
void XmlEditWidgetPrivate::autoTest()
{
#ifdef  _DEBUG_BALSAMIQ
    void testBALSAMIQ(QWidget * parent);
    this->show();
    testBALSAMIQ(this);
#endif
}
*/

void XmlEditWidgetPrivate::setData(QXmlEditData *newData, const bool newIsSlave, UIDelegate *newUiDelegate)
{
    if(NULL != newData) {
        _appData = newData ;
        _appData->namespaceManager()->init();
    }
    Regola *regola = getRegola();
    if(NULL != regola) {
        regola->setNamespaceManager(_appData->namespaceManager());
    }
    paintInfo.setColorManager(_appData->colorManager());
    isSlave = newIsSlave;
    _uiDelegate = newUiDelegate ;
    p->ui->searchWidget->setData(_appData);
    init();
}

void XmlEditWidgetPrivate::setUIDelegate(UIDelegate *newUiDelegate)
{
    _uiDelegate = newUiDelegate ;
}


bool XmlEditWidgetPrivate::init()
{
    p->ui->ok->setEnabled(isSlave);
    p->ui->ok->setVisible(isSlave);

    p->ui->cdataElement->setEnabled(isSlave);
    p->ui->cdataElement->setVisible(isSlave);

    p->ui->cancel->setEnabled(isSlave);
    p->ui->cancel->setVisible(isSlave);

    return internalStateOk  ;
}

/*
//autotest
void XmlEditWidgetPrivate::autoTest()
{
#ifdef  _DEBUG_BALSAMIQ
    void testBALSAMIQ(QWidget * parent);
    this->show();
    testBALSAMIQ(this);
#endif
}
*/

void XmlEditWidgetPrivate::deleteRegola()
{
    if(NULL != regola) {
        disconnect(regola, SIGNAL(wasModified()), this, SLOT(regolaIsModified()));
        disconnect(regola, SIGNAL(undoStateChanged()), this, SLOT(regolaUndoChanged()));
        disconnect(regola, SIGNAL(docTypeChanged(const QString &)), this, SLOT(docTypeChanged(const QString &)));
        disconnect(regola, SIGNAL(encodingChanged(const QString &)), this, SLOT(onEncodingChanged(const QString &)));
        disconnect(regola, SIGNAL(indentationChanged(const bool, const int)), this, SLOT(onIndentationChanged(const bool, const int)));
        p->ui->treeWidget->setUpdatesEnabled(false);
        p->ui->treeWidget->clear();
        p->ui->treeWidget->setUpdatesEnabled(true);
        delete regola;
        regola = NULL;
        _SCXMLNavigator->setEnabledInfo(false);
        _XSLTNavigator->setEnabledInfo(false);
    }
}

void XmlEditWidgetPrivate::startUIState()
{
    computeSelectionState();
    showNavigationBox();
}

qxmledit::EDisplayMode XmlEditWidgetPrivate::displayMode()
{
    return _displayMode ;
}

void XmlEditWidgetPrivate::setupSCXMLNavigator()
{
    _SCXMLNavigator = new SCXMLNavigatorWidget(NULL);
    _SCXMLNavigator->setEnabledInfo(false);
    p->ui->verticalLayout->insertWidget(15, _SCXMLNavigator);
    _SCXMLNavigator->setVisible(false);
    _SCXMLNavigator->setObjectName("SCXMLNavigator");
    connect(_SCXMLNavigator, SIGNAL(goToState(const QString &, Element *)), this, SLOT(onSCXMLNavigatorGoToState(const QString &, Element *)));
    connect(_SCXMLNavigator, SIGNAL(editState(const QString &, Element *)), this, SLOT(onSCXMLNavigatorEditState(const QString &, Element *)));
    //-----
    _XSLTNavigator = new XSLTNavigatorWidget(NULL);
    _XSLTNavigator->setEnabledInfo(false);
    p->ui->verticalLayout->insertWidget(16, _XSLTNavigator);
    _XSLTNavigator->setVisible(false);
    _XSLTNavigator->setObjectName("XSLTNavigator");
    connect(_XSLTNavigator, SIGNAL(goTo(Element *)), this, SLOT(onXSLTNavigatorGoTo(Element *)));
    connect(_XSLTNavigator, SIGNAL(edit(Element *)), this, SLOT(onXSLTNavigatorEdit(Element *)));
}

bool XmlEditWidgetPrivate::finishSetUpUi()
{
    setupSCXMLNavigator();
    _helper.setTree(p->ui->treeWidget);
    p->ui->treeWidget->setExpandsOnDoubleClick(false);
    // connect to search widget. In the original code these actions were inseted into the UI
    connect(p->ui->searchWidget, SIGNAL(find()), this, SLOT(findText()));
    connect(p->ui->searchWidget, SIGNAL(count()), this, SLOT(countTextOccurrences()));
    connect(p->ui->searchWidget, SIGNAL(openAdvancedSearch()), this, SLOT(openAdvancedResultPanel()));
    connect(p->ui->searchWidget, SIGNAL(hiding()), this, SLOT(on_closeSearchPanel_clicked()));
    connect(p->ui->searchWidget, SIGNAL(searchNext()), this, SLOT(onSearchNext()));
    connect(p->ui->searchWidget, SIGNAL(searchPrev()), this, SLOT(onSearchPrev()));
    //--
    connect(p->ui->searchWidget, SIGNAL(replaceAll()), this, SLOT(onReplaceAll()));
    connect(p->ui->searchWidget, SIGNAL(replaceSkipAndGotoNext()), this, SLOT(onReplaceSkipAndGotoNext()));
    connect(p->ui->searchWidget, SIGNAL(replaceSkipAndGotoPrevious()), this, SLOT(onReplaceSkipAndGotoPrevious()));
    connect(p->ui->searchWidget, SIGNAL(replaceReplaceAndGotoNext()), this, SLOT(onReplaceReplaceAndGotoNext()));
    connect(p->ui->searchWidget, SIGNAL(replaceReplaceAndGotoPrevious()), this, SLOT(onReplaceReplaceAndGotoPrevious()));
    //---

    connect(p->ui->ok, SIGNAL(clicked()), this, SLOT(on_ok_clicked()));
    connect(p->ui->cancel, SIGNAL(clicked()), this, SLOT(on_cancel_clicked()));
    connect(p->ui->moveUp, SIGNAL(clicked()), this, SLOT(on_moveUp_clicked()));
    connect(p->ui->moveDown, SIGNAL(clicked()), this, SLOT(on_moveDown_clicked()));
    connect(p->ui->addChild, SIGNAL(clicked()), this, SLOT(on_addChild_clicked()));
    connect(p->ui->addBrother, SIGNAL(clicked()), this, SLOT(on_addBrother_clicked()));
    connect(p->ui->editItem, SIGNAL(clicked()), this, SLOT(on_editItem_clicked()));
    connect(p->ui->deleteItem, SIGNAL(clicked()), this, SLOT(on_deleteItem_clicked()));
    connect(p->ui->viewAsXsdCmd, SIGNAL(clicked()), this, SLOT(on_viewAsXsdCmd_clicked()));
    connect(p->ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(on_treeWidget_itemSelectionChanged()));

    // turn off autoscroll
    p->ui->treeWidget->setAutoScroll(_appData->isAutoscroll());

    connect(p->ui->navigation, SIGNAL(navigateTo(const int)), this, SLOT(navigateToPage(const int)));

    mainFontSize = p->ui->treeWidget->font().pointSize();
    p->ui->horizontalLowLayout->setAlignment(Qt::AlignLeft);

    p->ui->testNext->setVisible(false);
    p->ui->testPrev->setVisible(false);

    resetTree();
    startUIState();
    p->ui->treeWidget->setAlternatingRowColors(true);

    p->ui->treeWidget->setUniformRowHeights(false);

    p->ui->searchWidget->setVisible(false);
    p->ui->searchWidget->setAdvancedSearch(true);

    p->ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(p->ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(treeContextMenu(const QPoint &)));
    p->ui->treeWidget->installEventFilter(this);

    /***************************************************************
    p->ui->ok->setEnabled(isSlave);
    p->ui->ok->setVisible(isSlave);
    */
    p->ui->cdataElement->setEnabled(isSlave);
    p->ui->cdataElement->setVisible(isSlave);
    /*
    p->ui->cancel->setEnabled(isSlave);
    p->ui->cancel->setVisible(isSlave);
    ***************************************************************/
    bool isNavigation = (_displayMode == qxmledit::SCAN);
    p->ui->navigation->setVisible(isNavigation);
    p->ui->navigation->setEnabled(isNavigation);
    p->ui->docTypeLabel->setVisible(false);

    connect(p->ui->indent, SIGNAL(clicked()), this, SLOT(onSetIndent()));

    VStyle *style = loadStyleMenu();
    setNewStyle(style);

    bool isOk = true;

    if(!setUpDelegates()) {
        isOk = false ;
    }
    return isOk;
}

bool XmlEditWidgetPrivate::setUpDelegates()
{
    attrDelegate = _helper.setUpTreeXmlDelegates(p, p->ui->treeWidget, &paintInfo);
    return true;
}

void XmlEditWidgetPrivate::onCopySpecial()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(NULL == action) {
        return ;
    }
    Element *element = getSelectedItem();
    if(NULL == element) {
        return ;
    }
    QClipboard *clipboard = QApplication::clipboard();
    QString tag = action->data().toString();
    if(tag == CS_ELEMENT_TEXT) {
        clipboard->setText(element->getAsText());
    } else if(tag == CS_ELEMENT_TAG) {
        clipboard->setText(element->tag());
    } else if(tag.startsWith(CS_ATTRIBUTE_NAME)) {
        clipboard->setText(tag.right(tag.length() - 1));
    } else if(tag.startsWith(CS_ATTRIBUTE_VALUE)) {
        Attribute *attribute = element->getAttribute(tag.right(tag.length() - 1));
        if(NULL != attribute) {
            clipboard->setText(attribute->value);
        }
    }
}

void XmlEditWidgetPrivate::setCopyPathAction(QAction* action)
{
    _copyPathAction = action ;
}

QMenu* XmlEditWidgetPrivate::getCopyMenu()
{
    return copyMenu ;
}

void XmlEditWidgetPrivate::buildCopyMenu()
{
    Element *element = getSelectedItem();
    resetCopyMenu();
    if(NULL == element) {
        return ;
    }
    copyMenu = new QMenu(p);

    if(element->isElement()) {
        if(NULL != _copyPathAction) {
            copyMenu->addAction(_copyPathAction);
        }
        QString text = element->getAsSimpleTextWithLimit(8);
        if(!text.isEmpty()) {
            if(text.length() > 8) {
                text = text.left(8);
                text += "...";
            }
            QAction *elementTextAction = createAnAction(copyMenu, QString(tr("Copy Text '%1'")).arg(text), CS_ELEMENT_TEXT, tr("Copy Element Text"));
            connect(elementTextAction , SIGNAL(triggered()), this, SLOT(onCopySpecial()));
            copyMenu->addAction(elementTextAction);
        }
        QAction *elementTagAction = createAnAction(copyMenu, QString(tr("Copy Tag <%1>")).arg(element->tag()), CS_ELEMENT_TAG, tr("Copy Element Tag"));
        connect(elementTagAction , SIGNAL(triggered()), this, SLOT(onCopySpecial()));
        copyMenu->addAction(elementTagAction);
    }
    QList<Attribute*>attributes = element->getAttributesList();
    QMap<QString, QString> attributesMap;
    foreach(Attribute * attribute, attributes) {
        attributesMap.insert(attribute->name, attribute->value);
    }
    foreach(QString attributeName, attributesMap.keys()) {
        QString attributeValue = attributesMap[attributeName];
        QAction *attributeNameAction = createAnAction(copyMenu, QString(tr("Copy '%1'")).arg(attributeName), CS_ATTRIBUTE_NAME + attributeName, tr("Copy Attribute Name"));
        connect(attributeNameAction , SIGNAL(triggered()), this, SLOT(onCopySpecial()));
        copyMenu->addAction(attributeNameAction);
        QString value = attributeValue;
        if(value.length() > 10) {
            value = value.left(10);
            value += "...";
        }
        QAction *attributeValueAction = createAnAction(copyMenu, QString(tr("Copy value %1='%2'")).arg(attributeName).arg(value), CS_ATTRIBUTE_VALUE + attributeName, tr("Copy Attribute Value"));
        connect(attributeValueAction , SIGNAL(triggered()), this, SLOT(onCopySpecial()));
        copyMenu->addAction(attributeValueAction);

    }
    p->ui->copySpecial->setMenu(copyMenu);
    p->ui->copySpecial->setPopupMode(QToolButton::InstantPopup);
}

// load style menu: NEED data pointer
VStyle* XmlEditWidgetPrivate::loadStyleMenu()
{
    VStyle* selectedStyle = NULL;
    QString selected = Config::getString(Config::KEY_VIEW_STYLE, _appData->predefinedStyleName());
    resetStyleMenu();
    styleMenu = new QMenu(p);
    QAction *defaultAction = createStyleAction(styleMenu, tr("== No style =="), "", tr("don't use a style"));
    connect(defaultAction, SIGNAL(triggered()), this, SLOT(onStyleChanged()));
    if(selected == "") {
        defaultAction->setChecked(true);
        selectedStyle = NULL ;
    }
    styleMenu->addAction(defaultAction);
    styleActionGroup.addAction(defaultAction);
    foreach(VStyle * style, _appData->getStyles()) {
        QString name = style->name();
        QAction *action = createStyleAction(styleMenu, name, name, "");
        connect(action, SIGNAL(triggered()), this, SLOT(onStyleChanged()));
        styleMenu->addAction(action);
        styleActionGroup.addAction(action);
        if(selected == name) {
            action->setChecked(true);
            selectedStyle = style;
        }
    }
    // add special styles
    foreach(VStyle * style, _appData->getPredefinedStyles()) {
        QAction *xsltAction = createStyleAction(styleMenu, style->name(), style->name(), "");
        connect(xsltAction, SIGNAL(triggered()), this, SLOT(onPredefinedStyleChanged()));
        styleMenu->addAction(xsltAction);
        styleActionGroup.addAction(xsltAction);
        if(selected == style->name()) {
            xsltAction->setChecked(true);
            selectedStyle = style;
        }
        if(style->name() == "XSLT") {
            _xsltAction = xsltAction ;
        }
        if(style->name() == "SCXML") {
            _SCXMLAction = xsltAction ;
        }
    }

    p->ui->styleButton->setMenu(styleMenu);
    p->ui->styleButton->setPopupMode(QToolButton::InstantPopup);

    return selectedStyle ;
}

void XmlEditWidgetPrivate::resetStyleMenu()
{
    p->ui->styleButton->setMenu(NULL);
    if(styleMenu != NULL) {
        foreach(QAction * action, styleMenu->actions()) {
            styleActionGroup.removeAction(action);
        }
        styleMenu->deleteLater();
    }
    styleMenu = NULL ;
}

void XmlEditWidgetPrivate::resetCopyMenu()
{
    p->ui->copySpecial->setMenu(NULL);
    if(copyMenu != NULL) {
        copyMenu->deleteLater();
    }
    copyMenu = NULL ;
}


QAction *XmlEditWidgetPrivate::createStyleAction(QMenu *menu, const QString &label, const QString &tag, const QString &tooltip)
{
    QAction *action = new QAction(menu);
    action->setText(label);
    action->setData(tag);
    action->setToolTip(tooltip);
    action->setCheckable(true);
    return action ;
    //TODO -14-
}

QAction *XmlEditWidgetPrivate::createAnAction(QMenu *menu, const QString &label, const QString &tag, const QString &tooltip)
{
    QAction *action = new QAction(menu);
    action->setText(label);
    action->setData(tag);
    action->setToolTip(tooltip);
    return action ;
    //TODO -14-
}

void XmlEditWidgetPrivate::onStyleChanged()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(NULL == action) {
        return ;
    }
    QString tag = action->data().toString();
    VStyle * newStyle = _appData->getStyle(tag);
    setNewStyle(newStyle);
}

void XmlEditWidgetPrivate::onPredefinedStyleChanged()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if(NULL == action) {
        return ;
    }
    QString tag = action->data().toString();
    VStyle * newStyle = _appData->getPredefinedStyle(tag);
    setNewStyle(newStyle);
}



void XmlEditWidgetPrivate::setNewStyle(VStyle *newStyle)
{
    if(NULL == newStyle) {
        Config::saveString(Config::KEY_VIEW_STYLE, "");
        VStyle *style = paintInfo.currentStyle();
        if(NULL != style) {
            style->deactivate();
        }
        paintInfo.setCurrentStyle(NULL) ;
        repaint();
        return ;
    }
    if(newStyle != paintInfo.currentStyle()) {
        Config::saveString(Config::KEY_VIEW_STYLE, newStyle->name());
        if(!newStyle->activate(p->ui->treeWidget->font(), paintInfo.zoom())) {
            Utils::error(tr("Error activating style"));
        }
        paintInfo.setCurrentStyle(newStyle) ;
        repaint();
    }
}

void XmlEditWidgetPrivate::repaint()
{
    if(NULL != regola) {
        p->ui->treeWidget->setUpdatesEnabled(false);
        regola->redisplay();
        p->ui->treeWidget->setUpdatesEnabled(true);
    }
}

PaintInfo* XmlEditWidgetPrivate::getPaintInfo()
{
    return &paintInfo;
}

void XmlEditWidgetPrivate::display()
{
    if(NULL != regola) {
        p->ui->treeWidget->setUpdatesEnabled(false);
        regola->caricaValori(p->ui->treeWidget);
        p->ui->treeWidget->setUpdatesEnabled(true);
    }
}

void XmlEditWidgetPrivate::treeContextMenu(const QPoint& position)
{
    p->emitTreeContextMenuRequested(position);
}

// Solo il salva come...
void XmlEditWidgetPrivate::regolaIsModified()
{
    //TRACEQ(QString("XmlEditWidgetPrivate::regolaIsModified():%1").arg(regola->isModified()));
    const bool isModified = regola->isModified();
    p->emitDocumentIsModified(isModified);
    if(isModified && (_SCXMLNavigator->isEnabledInfo() || _XSLTNavigator->isEnabledInfo())) {
        if(_updateTimer.isActive()) {
            _updateTimer.stop();
        }
        _updateTimer.start();
    }
}

void XmlEditWidgetPrivate::on_treeWidget_itemSelectionChanged()
{
    if(NULL != regola) {
        Element *selection = getSelectedItem();
        regola->handleSelection(selection);
        computeSelectionState();
        _SCXMLNavigator->selectItem(selection);
    }
}


void XmlEditWidgetPrivate::computeSelectionState()
{
    bool isSomeItemSelected = false;
    bool selectSpecials = false;
    bool selectSpecialsUp = false;
    bool selectSpecialsDown = false;
    //bool isNormalViewState = true;
    bool canAddChild = false;
    //bool isShownAsBase64 = false ;
    bool isExplore = (_displayMode != qxmledit::NORMAL) && (_displayMode != qxmledit::SCAN) ;
    bool isModifiable = !(isExplore || _readOnly);
    int numberSelected = p->ui->treeWidget->selectedItems().size();
    if(numberSelected > 0) {
        selectSpecials = true ;
        selectSpecialsUp = true ;
        selectSpecialsDown = true ;
        Element *element = Element::fromItemData(p->ui->treeWidget->selectedItems().at(0));
        selectSpecialsUp = !element->isFirstChild();
        selectSpecialsDown = !element->isLastChild();
        if(element->getType() == Element::ET_ELEMENT) {
            canAddChild = true;
        }
        //isNormalViewState = element->isNormalViewState();
        isSomeItemSelected = true ;
        //isShownAsBase64 = element->isShownBase64();
        //if il primo item
        //disabilita accoda, mvup e down
        //altrimenti se il selected si trova al primo o ultimo posto della catena deselezmv up o down
    } else {
        if(regola->isEmpty(true)) {
            canAddChild = true;
        }
    }
    p->ui->addChild->setEnabled(canAddChild && isModifiable);
    p->ui->addBrother->setEnabled(selectSpecials && isModifiable);
    p->ui->deleteItem->setEnabled(isSomeItemSelected && isModifiable);
    p->ui->editItem->setEnabled(isSomeItemSelected && isModifiable);
    p->ui->moveUp->setEnabled(selectSpecialsUp && isModifiable);
    p->ui->moveDown->setEnabled(selectSpecialsDown && isModifiable);
    p->ui->copySpecial->setEnabled(isSomeItemSelected);
    if(isSomeItemSelected) {
        buildCopyMenu();
    }
    // TODO: test
    //p->ui->testNext->setEnabled((NULL!=regola)?regola->nextBookmark()>=0:false);
    //p->ui->testPrev->setEnabled((NULL!=regola)?regola->previousBookmark()>=0:false);

    p->ui->viewAsXsdCmd->setEnabled(isValidXsd() && !isExplore);
    p->emitReevaluateSelectionState();
}

//void XmlEditWidgetPrivate::enableZoom()
//{
//    p->ui->actionZoomIn->setEnabled(paintInfo.canZoomIn());
//    p->ui->actionZoomOut->setEnabled(paintInfo.canZoomOut());
//}

Element *XmlEditWidgetPrivate::getSelectedItem()
{
    if((NULL != p->ui->treeWidget) && (p->ui->treeWidget->selectedItems().size() > 0)) {
        Element *element = Element::fromItemData(p->ui->treeWidget->selectedItems().at(0));
        return element;
    }
    return NULL;
}

void XmlEditWidgetPrivate::resetTree()
{
    _helper.resetTree(p->ui->treeWidget, &paintInfo);
}

bool XmlEditWidgetPrivate::onActionNewFromClipboard()
{
    const QClipboard *clipBoard = QApplication::clipboard();
    const QMimeData *mimeData = clipBoard->mimeData();
    if(mimeData->hasText()) {
        if(loadText(mimeData->text())) {
            setReadOnly(false);
            regola->markEdited();
            regola->setModified(true);
            return true ;
        }
    }
    return false;
}

bool XmlEditWidgetPrivate::newFromString(const QString &newData)
{
    if(loadText(newData)) {
        setReadOnly(false);
        regola->markEdited();
        regola->setModified(true);
        return true ;
    }
    return false;
}

void XmlEditWidgetPrivate::onActionResizeToContents()
{
    resizeTreeColumns();
}

void XmlEditWidgetPrivate::setEncodingLabelVisible(bool isShow)
{
    p->ui->encoding->setVisible(isShow);
}

bool XmlEditWidgetPrivate::isEncodingLabelVisible()
{
    return p->ui->encoding->isVisible();
}

void XmlEditWidgetPrivate::setDocTypeVisible(bool isShow)
{
    p->ui->docTypeLabel->setVisible(isShow);
}

bool XmlEditWidgetPrivate::isDocTypeVisible()
{
    return p->ui->docTypeLabel->isVisible();
}

void XmlEditWidgetPrivate::setViewAsXSDButtonVisible(bool isShow)
{
    if(p->ui->viewAsXsdCmd) {
        p->ui->viewAsXsdCmd->setVisible(isShow);
    }
}

bool XmlEditWidgetPrivate::isViewAsXSDButtonVisible()
{
    return p->ui->viewAsXsdCmd->isVisible();
}

void XmlEditWidgetPrivate::setMoveButtonsVisible(bool isShow)
{
    p->ui->moveDown->setVisible(isShow);
    p->ui->moveUp->setVisible(isShow);
}

bool XmlEditWidgetPrivate::areMoveButtonsVisible()
{
    return p->ui->moveDown->isVisible();
}

ulong XmlEditWidgetPrivate::versionNumber()
{
    return VERSION_NUMBER ;
}

void XmlEditWidgetPrivate::resizeTreeColumns()
{
    _helper.resizeTreeColumns(p->ui->treeWidget);
}

bool XmlEditWidgetPrivate::editElement(QTreeWidgetItem *item, const bool isByMouse, const bool forceTextual)
{
    if(!isActionMode()) {
        return false ;
    }
    if(NULL == regola) {
        errorNoRule();
        return false ;
    }
    Utils::TODO_THIS_RELEASE("caso da fare sposta a monte");
    QPoint pt = getEditor()->mapFromGlobal(QCursor::pos());
    QRect itemRect = getEditor()->visualItemRect(item);
    ElementDisplayInfo *elementDisplayInfo = Element::displayInfoFromItemData(item);
    if(forceTextual) {
        Utils::TODO_THIS_RELEASE("undo deve essere solo di livello piu alto");
        regola->editElementWithTextEditor(p, getEditor(), item);
        bool result = false;
        computeSelectionState();
        return result ;
    } else {
        if(isByMouse) {
            Utils::TODO_THIS_RELEASE("decidere, magari opzione trade memory for speed");
            if(NULL != elementDisplayInfo) {
                if(itemRect.contains(pt, false)) {
                    if(_appData->areExperimentalFeaturesEnabled()) {
                        ElementDisplayInfo thisElementDisplayInfo;
                        ElementItemSingleDelegate::findRects(getEditor(), item, itemRect, Element::fromItemData(item),
                                                             &thisElementDisplayInfo);
                        if(thisElementDisplayInfo._textRect.contains(pt, false)) {
                            return regola->editAndSubstituteTextInNodeElement(p, Element::fromItemData(item), _uiDelegate);
                        }
                    } else {
                        Utils::TODO_THIS_RELEASE("commento");
                        //pt.setX(pt.x()-itemRect.left());
                        //pt.setY(pt.y()-itemRect.top());
                        if(elementDisplayInfo->_textRect.contains(pt, false)) {
                            return regola->editAndSubstituteTextInNodeElement(p, Element::fromItemData(item), _uiDelegate);
                        }
                    }
                }
                if((QApplication::keyboardModifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
                    regola->editElementWithTextEditor(p, getEditor(), item);
                    bool result = false;
                    computeSelectionState();
                    return result ;
                }
                Utils::TODO_THIS_RELEASE("finire");
                /*else if (attributi) {
                    regola->editElement(p, item, _uiDelegate, XXXX );
                    mah
                }*/
            }
        }
    }
    regola->editElement(p, item, _uiDelegate);
    bool result = false;
    computeSelectionState();
    return result ;
}

void XmlEditWidgetPrivate::elementDoubleClicked(QTreeWidgetItem * item, int /*column*/)
{
    const bool isAlt = 0 != (Qt::AltModifier & QApplication::keyboardModifiers());
    const bool isCtrl = 0 != (Qt::ControlModifier & QApplication::keyboardModifiers());
    const bool isShift = 0 != (Qt::ShiftModifier & QApplication::keyboardModifiers());
    EEditMode editMode = XmlEditWidgetPrivate::EditModeDetail ;
    if(isShift) {
        editMode = EditModeTextualDependingOnMouse ;
    } else if(isCtrl || isAlt) {
        editMode = EditModeSpecific ;
    }
    specificPropertiesItem(item, editMode);
}

QTreeWidgetItem *XmlEditWidgetPrivate::getSelItem()
{
    QList<QTreeWidgetItem *>selItems =  p->ui->treeWidget->selectedItems();
    if(selItems.count() < 1) {
        return NULL;
    }
    return selItems.at(0);
}

void XmlEditWidgetPrivate::on_editItem_clicked()
{
    editItem();
}

void XmlEditWidgetPrivate::editItem()
{
    if(!isActionMode()) {
        return ;
    }
    QTreeWidgetItem *itemSel = getSelItem();
    if(NULL == itemSel) {
        Utils::errorNoSel(p);
        return;
    }
    editElement(itemSel, false) ;
}


void XmlEditWidgetPrivate::on_addChild_clicked()
{
    addChild();
}

void XmlEditWidgetPrivate::addChild()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->addChild(p->window(), p->ui->treeWidget);
}

// Can't add a brother to the first child (XML)
void XmlEditWidgetPrivate::addBrother()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->addBrother(p->window(), p->ui->treeWidget);
}

void XmlEditWidgetPrivate::on_addBrother_clicked()
{
    addBrother();
}

void XmlEditWidgetPrivate::errorNoRule()
{
    error(tr("No main structure"));
}

void XmlEditWidgetPrivate::onActionAddChildElement()
{
    addChild();
}

void XmlEditWidgetPrivate::onActionAppendChildElement()
{
    addBrother();
}

void XmlEditWidgetPrivate::onActionEdit()
{
    editSelection(EditModeDetail);
}

void XmlEditWidgetPrivate::editSelection(const EEditMode editMode)
{
    if(!isActionMode()) {
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    specificPropertiesItem(currItem, editMode);
}

void XmlEditWidgetPrivate::onActionDelete()
{
    deleteItem();
}
void XmlEditWidgetPrivate::onActionMoveUp()
{
    moveUp();
}

void XmlEditWidgetPrivate::onActionMoveDown()
{
    moveDown();
}

void XmlEditWidgetPrivate::deleteItem()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    if(!Utils::askYN(p,
                     tr("This operation will destroy the element and all his children. Do you really want to continue?"))) {
        return ;
    }
    regola->deleteElement(Element::fromItemData(currItem));
}

void XmlEditWidgetPrivate::on_deleteItem_clicked()
{
    deleteItem();
}

void XmlEditWidgetPrivate::on_moveUp_clicked()
{
    moveUp();
}

// Move up in parent containment.
void XmlEditWidgetPrivate::moveUp()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *element = Element::fromItemData(currItem);
    if(NULL == element) {
        return ;
    }
    regola->moveUp(element);
}

// Move up in parent containment.
void XmlEditWidgetPrivate::moveDown()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *element = Element::fromItemData(currItem);
    if(NULL == element) {
        return ;
    }
    regola->moveDown(element);
}

void XmlEditWidgetPrivate::on_moveDown_clicked()
{
    moveDown();
}

/**
 * @brief setClipBoardItem get the ownership of an element for the clipboard
 * @param pElement
 */
void XmlEditWidgetPrivate::setClipBoardItem(Element *pElement)
{
    QString clipboardText = pElement->getStringRepresentationForClipboard();
    _appData->setClipBoardItem(pElement, clipboardText);
}

void XmlEditWidgetPrivate::onActionCut()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    setClipBoardItem(Element::fromItemData(currItem)->cutToClipboard());
    regola->setModified(true);
    QTreeWidgetItem *newSel = getSelItem();
    if(NULL != newSel) {
        p->ui->treeWidget->scrollToItem(newSel, QAbstractItemView::EnsureVisible);
    }
}

void XmlEditWidgetPrivate::onActionCopy()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *element = Element::fromItemData(currItem);
    setClipBoardItem(element->copyToClipboard());
}

void XmlEditWidgetPrivate::onActionCopyElementOnly()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *element = Element::fromItemData(currItem);
    setClipBoardItem(element->copyToClipboard(true));
}

void XmlEditWidgetPrivate::onActionPaste()
{
    if(isActionMode()) {
        ClipboardElementList *list = getClipBoardItemList();
        bool dataExists = false;
        if(NULL != list) {
            if(list->elements().count() > 0) {
                regola->paste(p->ui->treeWidget, list->elements());
                dataExists = true ;
            }
            delete list;
        }
        if(!dataExists) {
            _uiDelegate->error(tr("No data can be pasted"));
        }
    }
}

void XmlEditWidgetPrivate::pasteAsSibling()
{
    if(isActionMode()) {
        ClipboardElementList *list = getClipBoardItemList();
        bool dataExists = false;
        if(NULL != list) {
            if(list->elements().count() > 0) {
                regola->pasteAsSibling(p, p->ui->treeWidget, list->elements());
                dataExists = true ;
            }
            delete list;
        }
        if(!dataExists) {
            _uiDelegate->error(tr("No data can be pasted"));
        }
    }
}


void XmlEditWidgetPrivate::onActionPasteLastAttributes()
{
    if(isActionMode()) {
        regola->pasteAttributes(p, p->ui->treeWidget, _appData->copyAttributesManager()->first());
    }
}

void XmlEditWidgetPrivate::actionCopyElementPathToClipboard()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *element = Element::fromItemData(currItem);
    if(NULL == element) {
        return ;
    }
    element->copyPathToClipboard();
}

ClipboardElementList* XmlEditWidgetPrivate::getClipBoardItemList(const bool onlyElements) const
{
    return _appData->clipBoardItemList(onlyElements);
}

void XmlEditWidgetPrivate::onActionExpandAll()
{
    if(regola != NULL) {
        regola->expand(p->ui->treeWidget);
    }
}

void XmlEditWidgetPrivate::onActionExpandSelectedItem()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        element->expand(p->ui->treeWidget);
    }
}

void XmlEditWidgetPrivate::onActionShowAttrLine(const bool state)
{
    paintInfo.setOneAttrPerLine(state);
    recalcRowHeightClass();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QAbstractItemModel *model = p->ui->treeWidget->model();
    emit model->layoutAboutToBeChanged();
    emit model->layoutChanged();
#endif
    repaint();
}

void XmlEditWidgetPrivate::recalcRowHeightClass()
{
    bool sameHeight = false ;
    if(!paintInfo.oneAttrPerLine() && paintInfo.compactView()) {
        sameHeight = true ;
    }
    if(getMainTreeWidget()->uniformRowHeights() != sameHeight) {
        getMainTreeWidget()->setUniformRowHeights(sameHeight);
    }
    // this function only for Qt 4.3 compatibility, for qt5 model->layoutAboutToBeChanged is enough
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    getMainTreeWidget()->doItemsLayout();
#endif
}

void XmlEditWidgetPrivate::invalidatePaintData(const bool full)
{
    ElementItemSingleDelegate *delegate = _helper.tagDelegate();
    if(NULL != delegate) {
        delegate->reset();
    }
    if(full) {
        recalcRowHeightClass();
    }
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QAbstractItemModel *model = getMainTreeWidget()->model();
    emit model->layoutAboutToBeChanged();
    emit model->layoutChanged();
#endif
    repaint();
}

void XmlEditWidgetPrivate::onActionAddComment()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->addComment(p, p->ui->treeWidget);
}


void XmlEditWidgetPrivate::onActionAppendComment()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->appendComment(p, p->ui->treeWidget);
}

void XmlEditWidgetPrivate::onActionAppendProcessingInstruction()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->appendProcessingInstruction(p, p->ui->treeWidget);
}

void XmlEditWidgetPrivate::onActionAddProcessingInstruction()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->addProcessingInstruction(p, p->ui->treeWidget);
}

/* TODO: I dont think it is relevant anymore
void XmlEditWidgetPrivate::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}
*/

/*
void XmlEditWidgetPrivate::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list")) {
        QString filePath = "" ;
        event->acceptProposedAction();
        if(event->mimeData()->hasUrls()) {
            foreach(QUrl url, event->mimeData()->urls()) {
                filePath = url.toLocalFile();
                break;
            }
        }
        if(filePath.length() > 0) {
            if(!XmlEditWidgetPrivate::checkAbandonChanges()) {
                event->ignore();
                return ;
            }
            loadFile(filePath);
        }
        event->acceptProposedAction();
    }
}
*/

void XmlEditWidgetPrivate::onActionReplace()
{
    if(!isActionMode()) {
        return ;
    }
    SearchWidget *searchWidget = p->ui->searchWidget;
    searchWidget->setManager(_appData->searchManager());
    searchWidget->setIsReplace(true);
    searchWidget->setVisible(true);
    if(NULL != regola) {
        searchWidget->setDataForCompletion(regola->namesPool());
    }
    searchWidget->regainFocus();
}


void XmlEditWidgetPrivate::onActionFind()
{
    SearchWidget *searchWidget = p->ui->searchWidget;
    searchWidget->setManager(_appData->searchManager());
    searchWidget->setIsReplace(false);
    searchWidget->setVisible(true);
    if(NULL != regola) {
        searchWidget->setDataForCompletion(regola->namesPool());
    }
    searchWidget->regainFocus();
}

void XmlEditWidgetPrivate::on_closeSearchPanel_clicked()
{
    if(NULL != regola) {
        regola->unhiliteAll();
    }
    p->ui->searchWidget->setVisible(false);
    p->setFocus(Qt::OtherFocusReason);
}

void XmlEditWidgetPrivate::findText()
{
    findTextOperation(true);
}

void XmlEditWidgetPrivate::countTextOccurrences()
{
    findTextOperation(false);
}

Element * XmlEditWidgetPrivate::findTextOperation(const bool isFindOrCount, const FindTextParams::EFindType findType)
{
    Element *foundElement = NULL;
    Element *firstOccurrence = NULL ;
    bool isGlobalSearch = FindTextParams::FindAllOccurrences == findType;
    if(NULL != regola) {

        p->setEnabled(false);
        p->ui->treeWidget->setUpdatesEnabled(false);
        Utils::showWaitCursor();

        bool isErrorShown = false;
        bool isError = false;
        FindTextParams* findArgs = p->ui->searchWidget->getSearchParams(findType, isFindOrCount, NULL);
        if((NULL == findArgs) || ((NULL != findArgs) && !findArgs->checkParams(isErrorShown))) {
            isError = true ;
        }
        if(isError) {
            if(!isErrorShown) {
                Utils::error(p, tr("Impossible to start a search; please, check the parameters."));
            }
        } else {
            findArgs->saveState();
            regola->unhiliteAll();
            findArgs->start();
            foundElement = regola->findText(*findArgs, getSelectedItem());
            firstOccurrence = findArgs->firstMatch();
            p->ui->searchWidget->setSearchResults(findArgs);
            if(isGlobalSearch) {
                p->emitShowStatusMessage(p->ui->searchWidget->messageCount(), true);
            }
            if(!isGlobalSearch && (NULL == foundElement)) {
                _uiDelegate->error(p->window(), tr("No match found."));
            }
        }
        if(NULL != findArgs) {
            delete findArgs;
        }
    }
    p->ui->treeWidget->setUpdatesEnabled(true);
    if(NULL != firstOccurrence) {
        p->ui->treeWidget->scrollToItem(firstOccurrence->getUI(), QAbstractItemView::PositionAtTop);
    }
    p->setEnabled(true);
    Utils::restoreCursor();
    emit p->reevaluateSelectionState();
    return foundElement ;
}

void XmlEditWidgetPrivate::onActionGoToParent()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    QTreeWidgetItem *parentItem = NULL ;
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    if((parentItem = currItem->parent()) != NULL) {
        p->ui->treeWidget->setCurrentItem(parentItem);
    }
}

void XmlEditWidgetPrivate::onActionGoToPreviousBrother()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL != currItem) {
        Element *element = Element::fromItemData(currItem);
        if(NULL != element) {
            QTreeWidgetItem *itemToSelect = element->goToPreviousBrother();
            if(NULL != itemToSelect) {
                p->ui->treeWidget->setCurrentItem(itemToSelect);
            }
        }
    }
}

void XmlEditWidgetPrivate::onActionGoToNextBrother() //onActionGo_to_Next_Brother()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL != currItem) {
        Element *element = Element::fromItemData(currItem);
        if(NULL != element) {
            QTreeWidgetItem *itemToSelect = element->goToNextBrother();
            if(NULL != itemToSelect) {
                p->ui->treeWidget->setCurrentItem(itemToSelect);
            }
        }
    }
}

void XmlEditWidgetPrivate::onActionClearBookmarks()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    regola->clearBookmarks();
    //p->ui->treeWidget->update();
}

void XmlEditWidgetPrivate::onActionToggleBookmark()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    // Regola vuota, aggiungo il primo ed ultimo
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL != currItem) {
        Element *element = Element::fromItemData(currItem);
        if(NULL != element) {
            regola->toggleBookmark(element);
        }
    }
}

void XmlEditWidgetPrivate::onActionGotoNextBookmark()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    Element *current = regola->gotoNextBookmark();
    if(NULL != current) {
        p->ui->treeWidget->setCurrentItem(current->getUI());
    }
}

void XmlEditWidgetPrivate::onActionGotoPreviousBookmark()
{
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    Element *current = regola->gotoPreviousBookmark();
    if(NULL != current) {
        p->ui->treeWidget->setCurrentItem(current->getUI());
    }
}

void XmlEditWidgetPrivate::onActionShowChildIndex(const bool isChecked)
{
    paintInfo.setIndexPerChild(isChecked);
    repaint();
}

void XmlEditWidgetPrivate::onActionCompactView(const bool isChecked)
{
    paintInfo.setCompactView(isChecked);
    _helper.setDataColumnTitle(p->ui->treeWidget, &paintInfo, paintInfo.compactView());
    recalcRowHeightClass();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QAbstractItemModel *model = p->ui->treeWidget->model();
    emit model->layoutAboutToBeChanged();
    emit model->layoutChanged();
#endif
    repaint();
}

void XmlEditWidgetPrivate::onActionShowAttributesSorted(const bool isChecked)
{
    paintInfo.setSortAttributesAlpha(isChecked);
    _helper.setDataColumnTitle(p->ui->treeWidget, &paintInfo, paintInfo.compactView());
    recalcRowHeightClass();
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QAbstractItemModel *model = p->ui->treeWidget->model();
    emit model->layoutAboutToBeChanged();
    emit model->layoutChanged();
#endif
    repaint();
}



void XmlEditWidgetPrivate::onActionShowAlwaysFullTextComments(const bool isShow)
{
    paintInfo.setShowFullComments(isShow);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    QAbstractItemModel *model = p->ui->treeWidget->model();
    emit model->layoutAboutToBeChanged();
    emit model->layoutChanged();
#endif
    repaint();
}


void XmlEditWidgetPrivate::onActionFixedSizeAttributes(const bool isChecked)
{
    paintInfo.setUseFixedLengthFont(isChecked);
    repaint();
}

void XmlEditWidgetPrivate::onActionShowAttributesLength(const bool isChecked)
{
    paintInfo.setShowAttributesLength(isChecked);
    repaint();
}

void XmlEditWidgetPrivate::onActionShowElementTextLength(const bool isChecked)
{
    paintInfo.setShowElementTextLength(isChecked);
    repaint();
}

void XmlEditWidgetPrivate::onActionCloseThisAllBrothers()
{
    QTreeWidgetItem *current = getSelItem();
    if(NULL != current) {
        QTreeWidgetItem *parent = current->parent();
        if(NULL != parent) {
            int numChildren = parent->childCount();
            for(int i = 0 ; i < numChildren ; i ++) {
                QTreeWidgetItem *child = parent->child(i);
                child->setExpanded(false);
            }
        }
    }
}

void XmlEditWidgetPrivate::onActionHideBrothers()
{
    QTreeWidgetItem *current = getSelItem();
    if(NULL != current) {
        Element *element = Element::fromItemData(current);
        if(NULL != element) {
            if(element->isNormalViewState()) {
                element->hideBrothers();
            } else {
                element->showBrothers();
            }
        }
    }
}

void XmlEditWidgetPrivate::closeSiblings()
{
    QTreeWidgetItem *current = getSelItem();
    if(NULL != current) {
        Element *element = Element::fromItemData(current);
        if((NULL != element) && (NULL != element->getUI())) {
            QVector<Element*> *elems = NULL ;
            if(NULL == element->parent()) {
                elems = element->getParentRule()->getChildItems();
            } else {
                elems = element->parent()->getChildItems();
            }
            QVectorIterator<Element*> it(*elems);
            while(it.hasNext()) {
                Element *elm = it.next();
                if((elm != element) && (NULL != elm->getUI())) {
                    if(elm->getUI()->isExpanded()) {
                        elm->getUI()->setExpanded(false);
                    }
                }
            }
            getEditor()->scrollToItem(element->getUI());
        }
    }
}

void XmlEditWidgetPrivate::onActionShowCurrentElementTextBase64(const bool isChecked)
{
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL != currItem) {
        Element *element = Element::fromItemData(currItem);
        if(NULL != element) {
            element->setShownBase64(isChecked);
            element->refreshUI();
        }
    }
}

void XmlEditWidgetPrivate::onActionShowBase64(const bool isChecked)
{
    paintInfo.setShowUnBase64(isChecked);
    repaint();
}

void XmlEditWidgetPrivate::onActionShowElementSize(const bool isChecked)
{
    paintInfo.setShowElementSize(isChecked);
    _helper.calcColumnState(p->ui->treeWidget, &paintInfo);
    if(NULL != regola) {
        regola->assignCollectSizeDataFlag(isChecked);
    }
    repaint();
}

//TODO: send an event
void XmlEditWidgetPrivate::on_ok_clicked()
{
    p->emitOkClicked();
}

void XmlEditWidgetPrivate::on_cancel_clicked()
{
    p->emitCancelClicked();
}

void XmlEditWidgetPrivate::onActionEditInnerXML()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *item = getSelItem();
    if(NULL != item) {
        regola->editInnerXMLElement(item, _uiDelegate);
    }
}

void XmlEditWidgetPrivate::onActionEditInnerXMLBase64()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }

    QTreeWidgetItem *item = getSelItem();
    if(NULL != item) {
        regola->editInnerXMLBase64Element(item, _uiDelegate);
    }
}

void XmlEditWidgetPrivate::onActionEditInnerBase64Text()
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL == regola) {
        errorNoRule();
        return ;
    }

    QTreeWidgetItem *item = getSelItem();
    if(NULL != item) {
        regola->editTextNodeElementBase64(p, item, _uiDelegate);
    }
}

QString XmlEditWidgetPrivate::getContentAsText()
{
    if(NULL != regola) {
        return regola->getAsText();
    }
    return "";
}

bool XmlEditWidgetPrivate::isCDATA()
{
    return p->ui->cdataElement->isChecked();
}

void XmlEditWidgetPrivate::setCDATA(const bool isCData)
{
    return p->ui->cdataElement->setChecked(isCData);
}

void XmlEditWidgetPrivate::onActionZoomIn()
{
    if(paintInfo.zoomIn()) {
        Element::setZoomFactor(paintInfo.zoom());
        QFont font = p->ui->treeWidget->font();
        int newSize = VStyle::getZoomFontSize(mainFontSize, paintInfo.zoom());
        if(newSize > 0) {
            font.setPointSize(newSize);
            p->ui->treeWidget->setFont(font);
        }
        p->emitEnableZoom();
        repaint();
    }
}

void XmlEditWidgetPrivate::onActionZoomOut()
{
    if(paintInfo.zoomOut()) {
        Element::setZoomFactor(paintInfo.zoom());
        QFont font = p->ui->treeWidget->font();
        int newSize = VStyle::getZoomFontSize(mainFontSize, paintInfo.zoom());
        if(newSize > 0) {
            font.setPointSize(VStyle::getZoomFontSize(mainFontSize, paintInfo.zoom()));
            p->ui->treeWidget->setFont(font);
        }
        p->emitEnableZoom();
        repaint();
    }
}

//da decidere se emette o torna un flag
void XmlEditWidgetPrivate::onActionCopyPathToClipboard()
{
    if((NULL != regola) && !regola->fileName().isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        if(NULL != clipboard) {
            QFileInfo info(regola->fileName());
            clipboard->setText(QDir::toNativeSeparators(info.absoluteFilePath()));
        }
    }
}

void XmlEditWidgetPrivate::insertSnippet(Regola *newRegola)
{
    if(NULL != newRegola) {
        regola->pasteXML(p->ui->treeWidget, newRegola) ;
        computeSelectionState();
    }
}

void XmlEditWidgetPrivate::onActionDeleteBookmarked()
{
    if(NULL != regola) {
        Utils::showWaitCursor();
        p->ui->treeWidget->setUpdatesEnabled(false);
        regola->deleteBookmarked();
        p->ui->treeWidget->setUpdatesEnabled(true);
        Utils::restoreCursor();
    }
}

void XmlEditWidgetPrivate::onActionCleanBookmarkedContent()
{
    if(NULL != regola) {
        Utils::showWaitCursor();
        p->ui->treeWidget->setUpdatesEnabled(false);
        regola->cleanBookmarkedContents();
        p->ui->treeWidget->setUpdatesEnabled(true);
        Utils::restoreCursor();
    }
}

bool XmlEditWidgetPrivate::isActionMode()
{
    return ((_displayMode == qxmledit::NORMAL) || (_displayMode == qxmledit::SCAN)) && !_readOnly ;
}

bool XmlEditWidgetPrivate::isActionNoScanMode()
{
    return _displayMode == qxmledit::NORMAL;
}

void XmlEditWidgetPrivate::setDisplayMode(const qxmledit::EDisplayMode value)
{
    _displayMode = value ;
    //navigationModeIndicator->setExploreMode(value);
    p->ui->treeWidget->setUniformRowHeights(value == qxmledit::EXPLORE);
    p->ui->treeWidget->setVisible(value != qxmledit::BLIND);
}


/**
  changestate if the state is to be reset to normal after loading new document
  */
bool XmlEditWidgetPrivate::loadText(const QString &text, const bool isChangeState, const bool isAskForReview)
{
    if(!text.isEmpty()) {
        QDomDocument document;
        if(document.setContent(text)) {
            setDocument(document, QString(""), isChangeState);
            autoLoadValidation();
            return true;
        } else {
            if(isAskForReview) {
                if(Utils::askYN(tr("Unable to parse XML. Sometimes this is caused by parser informations.\n Do you want to examine data as text?"))) {
                    ShowTextInDialog(p, text);
                }
            } else {
                Utils::error(tr("Unable to parse XML"));
            }
        }
    } else {
        Utils::error(tr("Data empty. Unable to load."));
    }
    return false;
}

void XmlEditWidgetPrivate::assignRegolaAndTakeOwnership(Regola *newRegola, const bool isChangeState)
{
    if(NULL != newRegola) {
        assignRegola(newRegola, isChangeState);
        autoLoadValidation();
    } else {
        Utils::error(tr("Data empty. Unable to load."));
    }
}

//---------------------- load data

//TODO: error checking
void XmlEditWidgetPrivate::doLoadFileXplore(const QString &filePath)
{
    Regola *newModel = newRegola();
    if(!newModel->loadXplore(filePath)) {
        Utils::error(tr("Unable to load XML"));
    }
    setDisplayMode(qxmledit::EXPLORE);
    p->emitDataReadyMessage(tr("Data loaded"));
    deleteRegola();
    regola = newModel;
    regola->setModified(false);
    resetTree();
    p->ui->treeWidget->setUniformRowHeights(true);
    display();
    startUIState();
    regolaIsModified();
    if(isExpandTreeOnLoad()) {
        onActionExpandAll();
    }
    resizeTreeColumns();
    showControls(true);
    setEditMode(XmlEditWidgetEditMode::XML);
}

void XmlEditWidgetPrivate::setDocument(QDomDocument &document, const QString &filePath, const bool isSetState)
{
    Regola *newModel = new Regola(document, filePath);
    assignRegola(newModel, isSetState);
}

bool XmlEditWidgetPrivate::readData(XMLLoadStatus *status, QXmlStreamReader *xmlReader, const QString &filePath, const bool isSetState, XMLLoadErrorHandler *errorHandler)
{
    XMLLoadContext context;
    status->clearErrors();
    Regola *newModel = new Regola(filePath);
    houseworkRegola(newModel);
    if(!newModel->readFromStream(&context, xmlReader)) {
        if(showLoadError(context.errorMessage(), errorHandler, &context, xmlReader)) {
            status->setErrorsPresent();
        } else {
            delete newModel;
            return false;
        }
    }
    assignRegola(newModel, isSetState);
    return true;
}

void XmlEditWidgetPrivate::houseworkRegola(Regola *regola)
{
    regola->setIndentation(_appData->xmlIndent());
    regola->setIndentAttributesSettings(true, _appData->xmlIndentAttributesType(), _appData->xmlIndentAttributes());
}

void XmlEditWidgetPrivate::assignRegola(Regola *newModel, const bool isSetState)
{
    newModel->setPaintInfo(&paintInfo);
    newModel->setNamespaceManager(_appData->namespaceManager());
    houseworkRegola(newModel);
    p->emitDataReadyMessage(tr("Data loaded"));
    deleteRegola();
    regola = newModel;
    regola->assignCollectSizeDataFlag(paintInfo.showElementSize());
    docTypeChanged(regola->docType());
    if(isSetState) {
        setDisplayMode(qxmledit::NORMAL);
        setReadOnly(false);
    }
    bindRegola(regola);

    _SCXMLNavigator->setEnabledInfo(false);
    _XSLTNavigator->setEnabledInfo(false);
    resetTree();
    display();
    startUIState();
    regolaIsModified();
    if(isExpandTreeOnLoad()) {
        onActionExpandAll();
    }
    resizeTreeColumns();
    showControls(true);
    setEditMode(XmlEditWidgetEditMode::XML);
}

void XmlEditWidgetPrivate::bindRegola(Regola *regola, const bool bind)
{
    connect(regola, SIGNAL(wasModified()), this, SLOT(regolaIsModified()));
    connect(regola, SIGNAL(undoStateChanged()), this, SLOT(regolaUndoChanged()));
    connect(regola, SIGNAL(docTypeChanged(const QString &)), this, SLOT(docTypeChanged(const QString &)));
    connect(regola, SIGNAL(encodingChanged(const QString &)), this, SLOT(onEncodingChanged(const QString &)));
    connect(regola, SIGNAL(indentationChanged(const bool, const int)), this, SLOT(onIndentationChanged(const bool, const int)));
    if(bind) {
        onEncodingChanged(regola->encoding());
        regola->emitIndentationChange();
    }
}

void XmlEditWidgetPrivate::onIndentationChanged(const bool indentationEnabled, const int newIndentation)
{
    emit p->indentationChanged(indentationEnabled, newIndentation);
}

void XmlEditWidgetPrivate::onEncodingChanged(const QString &newEncoding)
{
    p->ui->encoding->setText(p->labelForEncoding(newEncoding));
    emit p->encodingChanged(newEncoding);
}

void XmlEditWidgetPrivate::docTypeChanged(const QString &docType)
{
    if(docType.isEmpty()) {
        p->ui->docTypeLabel->setVisible(false);
        p->ui->docTypeLabel->setText("");
    } else {
        p->ui->docTypeLabel->setText(tr("doc type: %1").arg(docType));
        p->ui->docTypeLabel->setVisible(true);
    }
}

void XmlEditWidgetPrivate::showControls(const bool how)
{
    p->ui->treeWidget->setVisible(how);
}


void XmlEditWidgetPrivate::showButtons(const bool how)
{
    p->ui->addBrother->setVisible(how);
    p->ui->addChild->setVisible(how);
    p->ui->deleteItem->setVisible(how);
    p->ui->editItem->setVisible(how);
    p->ui->moveUp->setVisible(how);
    p->ui->moveDown->setVisible(how);
    p->ui->viewAsXsdCmd->setVisible(how);
}

void XmlEditWidgetPrivate::showToolbar(const bool how)
{
    p->ui->copySpecial->setVisible(how);
    p->ui->styleButton->setVisible(how);
    p->ui->indent->setVisible(how);
}

void XmlEditWidgetPrivate::onActionHideView(const bool isChecked)
{
    Utils::TODO_NEXT_RELEASE("consider remove");
    paintInfo.setHideView(isChecked);
    showControls(!paintInfo.hideView());
    if(!paintInfo.hideView()) {
        display();
    }
}

bool XmlEditWidgetPrivate::isValidXsd()
{
    if(NULL != regola) {
        return regola->isValidXsd();
    }
    return false;
}

void XmlEditWidgetPrivate::on_viewAsXsdCmd_clicked()
{
    p->emitViewAsXsdRequested();
}


Regola *XmlEditWidgetPrivate::getRegola()
{
    return regola;
}


void XmlEditWidgetPrivate::onActionViewAsXsd()
{
    XsdPlugin plugin ;
    Element *out = NULL ;
    plugin.go(p->parentWidget(), _appData, regola, getSelectedItem(), &out);
    if(out != NULL) {
        p->setCurrentItem(out);
    }
}

void XmlEditWidgetPrivate::loadSchema(const QString &schemaURL)
{
    if(NULL == _appData) {
        return ;
    }
    if(schemaURL.isEmpty()) {
        return ;
    }

    deleteSchema();
    XSchemaLoader *loader = NULL ;
    if((loader = new XSchemaLoader()) != NULL)  {
        try {
            connect(loader, SIGNAL(finished(XSchemaLoader*, const XSchemaLoader::Code)), this, SLOT(schemaLoadComplete(XSchemaLoader*, const XSchemaLoader::Code)));

            // finds the file path if any
            QString fileName ;
            if(NULL != regola) {
                fileName = regola->fileName();
            }
            QFile file(fileName);
            QString dirPath = QFileInfo(file).absolutePath();

            loader->load(loader->loadContext(), schemaURL, true, dirPath, _appData->xsdNetworkAccessManager());

        } catch(XsdException *ex) {
            Utils::error(tr("Error loading schema.\n%1").arg(ex->cause()));
        } catch(...) {
            Utils::error(tr("Unknown exception."));
        }
    } else {
        Utils::error(tr("No root item"));
    }
}

void XmlEditWidgetPrivate::schemaLoadComplete(XSchemaLoader *loader, const XSchemaLoader::Code code)
{
    if(NULL != loader) {
        loader->deleteLater();
    }
    if(code != XSchemaLoader::SCHEMA_READY) {
        p->emitSchemaLabelChanged(tr("error loading schema"));
    } else {
        _schemaRoot = loader->getSchemaAndTakeOwnership() ;
        if(NULL == regola) {
            p->emitSchemaLabelChanged(tr("schema: ?"));
        } else {
            p->emitSchemaLabelChanged(tr("schema: %1").arg(regola->documentXsd()));
        }
    }
}

void XmlEditWidgetPrivate::onActionAllowedSchemaElements()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }

    insertAllowedElements(getSelectedItem());
}

bool XmlEditWidgetPrivate::findAllowedItemsElement(XElementContent *content, Element *element, bool &startAsRoot)
{
    SchemaValidator validator;
    bool go = false;
    startAsRoot = false;

    if(NULL == regola->root()) {
        go = true ;
        startAsRoot = true ;
    } else if((NULL != element) && (Element::ET_ELEMENT == element->getType())) {
        go = true ;
    }

    if(go) {
        if(validator.getInsertionPointAllowedItems(content, _schemaRoot, element, startAsRoot)) {
            return true ;
        }
    }
    return false;
}

void XmlEditWidgetPrivate::insertAllowedElements(Element *element)
{
    XElementContent content;
    bool startAsRoot = false ;
    if(findAllowedItemsElement(&content, element, startAsRoot)) {

        bool isModified = false;
        regola->emptyUndoStack();
        QList<XSchemaObject*> result;
        QList< QPair<QString, QString> > attributes;
        Element *theParent = element;
        if(ChooseItemsBySchema(p, &content, &result, &attributes)) {
            foreach(XSchemaObject * object, result) {
                if(startAsRoot) {
                    theParent = regola->root();
                    regola->addChildToElement(_uiDelegate->getMainWidget(), p->getMainTreeWidget(), NULL, object->name(), false);
                    isModified = true ;
                    break;
                }
                regola->addChildToElement(p, p->ui->treeWidget, element, object->name(), false);
                isModified = true ;
            }
            QPair<QString, QString> attribute;
            foreach(attribute, attributes) {
                theParent->setAttribute(attribute.first, attribute.second);
            }
            if(isModified) {
                p->emitDocumentIsModified(regola->isModified());
            }
        }
    }
}

/** Substitute the element text
  */
void XmlEditWidgetPrivate::onActionPasteAndSubstituteText()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    Element *element = getSelectedItem();
    if(NULL != element) {
        regola->editAndSubstituteTextInNodeElement(p, element, _uiDelegate);
    }
}

void XmlEditWidgetPrivate::onActionNewUsingXMLSchema(const QString &schemaURL)
{
    doNew();
    regola->setUserAndDocumentXsd(schemaURL);
    loadSchema(schemaURL);
}

void XmlEditWidgetPrivate::doNew()
{
    deleteRegola();
    regola = newRegola();
    setDisplayMode(qxmledit::NORMAL);
    p->ui->treeWidget->clear();
    if(_appData->isAutoInsertProlog()) {
        regola->insertProlog(getMainTreeWidget(), _appData->autoInsertPrologEncoding(), false);
    }
    _SCXMLNavigator->setEnabledInfo(false);
    _XSLTNavigator->setEnabledInfo(false);
    setReadOnly(false);
    regola->assignCollectSizeDataFlag(paintInfo.showElementSize());
    startUIState();
    p->emitDocumentIsModified(regola->isModified());
    p->emitSchemaLabelChanged(tr(""));
}

void XmlEditWidgetPrivate::onActionTransformInComment()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *elementToTransform = Element::fromItemData(currItem);
    regola->transformInComment(p, p->ui->treeWidget, elementToTransform);
}

void XmlEditWidgetPrivate::onActionExtractElementsFromComment()
{
    if(!isActionMode()) {
        return ;
    }
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    QTreeWidgetItem *currItem = getSelItem();
    if(NULL == currItem) {
        Utils::errorNoSel(p);
        return;
    }
    Element *elementToRetransform = Element::fromItemData(currItem);
    regola->generateFromComment(p->ui->treeWidget, _uiDelegate, elementToRetransform);
}

void XmlEditWidgetPrivate::autoLoadValidation()
{
    if(_appData->isAutovalidationOn()) {
        if(!regola->documentXsd().isEmpty()) {
            p->emitSchemaLabelChanged(tr("schema: loading %1").arg(regola->documentXsd()));
            loadSchema(regola->documentXsd());
        } else {
            p->emitSchemaLabelChanged(tr("No schema"));
        }
    } else {
        p->emitSchemaLabelChanged(tr(""));
    }
}

void XmlEditWidgetPrivate::setNavigationDataAndEnable(const int minFragment, const int maxFragment)
{
    p->ui->navigation->setRange(minFragment, maxFragment);
    setDisplayMode(qxmledit::SCAN);
}

void XmlEditWidgetPrivate::showNavigationBox()
{
    bool isNavigation = (_displayMode == qxmledit::SCAN);
    p->ui->navigation->setVisible(isNavigation);
    p->ui->navigation->setEnabled(isNavigation);
}

void XmlEditWidgetPrivate::navigateToPage(const int page)
{
    /********************************** test code **************************
    Utils::todo(QString("Navigate to %1").arg(page));
    setWindowTitle(QString::number(page));
    ***********************************************************************/
    //loadCurrentPage(page);
    p->emitLoadCurrentPage(page);
}

void XmlEditWidgetPrivate::error(const QString& message)
{
    Utils::error(p, message);
}

void XmlEditWidgetPrivate::warning(const QString& message)
{
    Utils::warning(p, message);
}

void XmlEditWidgetPrivate::message(const QString& message)
{
    Utils::message(p, message);
}

bool XmlEditWidgetPrivate::askYN(const QString & message)
{
    return Utils::askYN(p, message);
}

QTreeWidget *XmlEditWidgetPrivate::getMainTreeWidget()
{
    return p->ui->treeWidget;
}

void XmlEditWidgetPrivate::onActionHideAllLeafChildren()
{
    if(NULL != regola) {
        p->ui->treeWidget->setUpdatesEnabled(false);
        Utils::showWaitCursor();
        regola->hideLeafNodes();
        p->ui->treeWidget->setUpdatesEnabled(true);
        Utils::restoreCursor();
    }
}

Element* XmlEditWidgetPrivate::onActionHideLeafChildren()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        p->ui->treeWidget->setUpdatesEnabled(false);
        Utils::showWaitCursor();
        element->hideChildrenLeaves();
        //        evaluateSingleItemLeaves(element);
        p->ui->treeWidget->setUpdatesEnabled(true);
        Utils::restoreCursor();
    }
    return element;
}

Element* XmlEditWidgetPrivate::onActionShowLeafChildren()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        p->ui->treeWidget->setUpdatesEnabled(false);
        Utils::showWaitCursor();
        element->showChildrenLeaves();
        //        evaluateSingleItemLeaves(element);
        p->ui->treeWidget->setUpdatesEnabled(true);
        Utils::restoreCursor();
    }
    return element;
}

void XmlEditWidgetPrivate::onActionShowAllLeafChildren()
{
    if(NULL != regola) {
        p->ui->treeWidget->setUpdatesEnabled(false);
        Utils::showWaitCursor();
        regola->showLeafNodes();
        p->ui->treeWidget->setUpdatesEnabled(true);
        Utils::restoreCursor();
    }
}


XSDSchema *XmlEditWidgetPrivate::schema()
{
    return _schemaRoot;
}

bool XmlEditWidgetPrivate::isExpandTreeOnLoad()
{
    return Config::getBool(Config::KEY_MAIN_EXPANDONLOAD, true);
}

bool XmlEditWidgetPrivate::isUndoPossible()
{
    return regola->canUndo();
}

bool XmlEditWidgetPrivate::isRedoPossible()
{
    return regola->canRedo();
}

void XmlEditWidgetPrivate::undo()
{
    regola->undo();
}

void XmlEditWidgetPrivate::redo()
{
    regola->redo();
}

void XmlEditWidgetPrivate::notifyUndo()
{
    emit p->undoStateUpdated(isUndoPossible(), isRedoPossible());
}

void XmlEditWidgetPrivate::regolaUndoChanged()
{
    notifyUndo();
}

void XmlEditWidgetPrivate::selectAndShowItem(Element *newSelection)
{
    if(NULL != newSelection) {
        p->ui->treeWidget->setCurrentItem(newSelection->getUI());
        p->ui->treeWidget->scrollToItem(newSelection->getUI(), QAbstractItemView::PositionAtTop);
    }
}

void XmlEditWidgetPrivate::setCurrentItem(Element *newSelection)
{
    if(NULL != newSelection) {
        p->ui->treeWidget->setCurrentItem(newSelection->getUI());
    }
}

void XmlEditWidgetPrivate::onActionCompareXSD(const QString &folderPath)
{
    bool isAllowed = false;
    if(NULL != regola) {
        if(regola->isValidXsd()) {
            XSDCompare xsdCompare;
            xsdCompare.compare(p, _appData, regola, folderPath);
            isAllowed = true ;
        }
    }
    if(!isAllowed) {
        Utils::error(tr("No XSD is present in the editor."));
    }
}

// copy the selected element in the clipboard
void XmlEditWidgetPrivate::onActionExportSiblingsAttributesAsCSVClipboard()
{
    // copy the selected element in the clipboard
    Element *element = getSelectedItem();
    if(NULL == element) {
        return ;
    }
    QClipboard *clipboard = QApplication::clipboard();
    QString text = element->makeCSVFromSiblings();
    clipboard->setText(text);
    Utils::message(tr("Attributes copied"));
}

void XmlEditWidgetPrivate::viewNodes()
{
    if(NULL != regola) {
        QByteArray regolaData = regola->writeMemory();
        QBuffer regolaBuffer(&regolaData);
        QList<TagNode*> nodesList ;
        NodesRelationsDialog dialog(false, nodesList);
        dialog.loadNodesFromFile(&regolaBuffer, regola->fileName());
        dialog.exec();
    }
}


void XmlEditWidgetPrivate::openAdvancedResultPanel()
{
    QList<Element*> emptyList;
    FindTextParams* settings = this->p->ui->searchWidget->getSearchParams(FindTextParams::FindAllOccurrences, true, NULL);

    SearchResultsDialog::go(this->p, _appData, emptyList, p->getRegola(), getSelectedItem(), this->p->ui->searchWidget->currentText(), settings);
    if(NULL != settings) {
        delete settings ;
    }
}

void XmlEditWidgetPrivate::addXsltMenuItems(QMenu *contextMenu)
{
    _xsltHelper.addMenuItems(getSelectedItem(), contextMenu, regola->namespacePrefixXslt());
}

QString XmlEditWidgetPrivate::namespacePrefixXslt()
{
    if(NULL != regola) {
        return regola->namespacePrefixXslt();
    }
    return "";
}

QTreeWidget *XmlEditWidgetPrivate::getEditor()
{
    return p->ui->treeWidget ;
}

void XmlEditWidgetPrivate::appendElementComplete(Element *theNewElement, Element *brotherElement)
{
    regola->appendElementComplete(theNewElement, brotherElement, p->ui->treeWidget, true);
}

void XmlEditWidgetPrivate::insertElementComplete(Element *theNewElement, Element *parentElement)
{
    regola->insertElementComplete(theNewElement, parentElement, p->ui->treeWidget) ;
}

QXmlEditData *XmlEditWidgetPrivate::appData()
{
    return _appData;
}

void XmlEditWidgetPrivate::editXSLTElement(QTreeWidgetItem *item)
{
    if(NULL != item) {
        Element *selectedElement = Element::fromItemData(item);
        if((NULL != selectedElement) && selectedElement->isElement()) {
            _xsltHelper.editElement(selectedElement);
        }
    }
}

void XmlEditWidgetPrivate::editXSLTElement()
{
    QTreeWidgetItem *item = p->getSelItem();
    editXSLTElement(item);
}


XsltHelper *XmlEditWidgetPrivate::XSLTHelper()
{
    return &_xsltHelper;
}

void XmlEditWidgetPrivate::scanXMLTagsAndNamesXSLTAutocompletion()
{
    if(editMode() == XmlEditWidgetEditMode::XSLT) {
        _xsltHelper.loadNames();
    }
}

void XmlEditWidgetPrivate::showXSLNavigator(const bool how)
{
    bool shouldApplyNewInfo = false ;
    if(how && !_XSLTNavigator->isEnabledInfo()) {
        shouldApplyNewInfo = true ;
    }
    _XSLTNavigator->setEnabledInfo(how);
    if(shouldApplyNewInfo) {
        applyXSLT();
    }
    _XSLTNavigator->setVisible(how);
}

void XmlEditWidgetPrivate::specificPropertiesItem(QTreeWidgetItem * item, const EEditMode editModeParam)
{
    if(isActionMode() && (NULL != item)) {
        //specialized mode
        Element *element = Element::fromItemData(item);
        const bool isXSLT = (editMode() == XmlEditWidgetEditMode::XSLT) ;
        const bool isXSLTElement = _xsltHelper.isXSLTElement(element);
        const bool isSCXML = (editMode() == XmlEditWidgetEditMode::SCXML);
        SCXMLEditorManager *theSCXMLEditorManager = NULL ;
        bool isSCXMLElement = false;
        NamespaceManager *namespaceManager = getRegola()->namespaceManager();
        if(NULL != namespaceManager) {
            theSCXMLEditorManager = namespaceManager->scxmlEditorManager() ;
            isSCXMLElement = theSCXMLEditorManager->isElementSCXML(element);
        }
        if(editModeParam == EditModeTextualText) {
            regola->editAndSubstituteTextInNodeElement(p, Element::fromItemData(item), _uiDelegate);
            return ;
        }
        if(editModeParam == EditModeTextual) {
            editElement(item, false, true);
            return ;
        }
        if(editModeParam == EditModeTextualDependingOnMouse) {
            editElement(item, true, false);
            return ;
        }
        if(editModeParam == EditModeForceSpecial) {
            if(isXSLTElement) {
                editXSLTElement(item);
                return ;
            }
            if(isSCXMLElement) {
                theSCXMLEditorManager->handleEdit(p->window(), p, getEditor(), getRegola(), element);
                return ;
            }
        }
        if(EditModeDetail == editModeParam) {
            //common mode
            if(isXSLT && isXSLTElement) {
                editXSLTElement(item);
            } else if(isSCXML && isSCXMLElement) {
                theSCXMLEditorManager->handleEdit(p->window(), p, getEditor(), getRegola(), element);
            } else {
                editElement(item, false);
            }
        } else {
            //alternate EditModeSpecific
            if(isXSLT || isSCXML) {
                // always
                editElement(item, false);
            } else {
                NamespaceManager *namespaceManager = _appData->namespaceManager();
                if(NULL != namespaceManager) {
                    Element *element = Element::fromItemData(item);
                    if(namespaceManager->editElement(p->window(), p, getEditor(), regola, element)) {
                        return ;
                    }
                }
                editElement(item, false);
            }
        }
    }
}

void XmlEditWidgetPrivate::specificProperties()
{
    if(isActionMode()) {
        QTreeWidgetItem *item = getEditor()->currentItem();
        if(NULL != item) {
            specificPropertiesItem(item, EditModeSpecific);
        }
    }
}


bool XmlEditWidgetPrivate::eventFilter(QObject * obj, QEvent * event)
{
    return _helper.filterEvent(obj, event, anonDataForPreview());
}

void XmlEditWidgetPrivate::setReadOnly(const bool newState)
{
    _readOnly = newState;
    computeSelectionState();
    emit p->readOnlyStateChanged();
}

bool XmlEditWidgetPrivate::isReadOnly()
{
    return _readOnly ;
}

void XmlEditWidgetPrivate::onActionInsertMetadata()
{
    if(isActionMode()) {
        regola->insertMetadata(p->ui->treeWidget);
    }
}

void XmlEditWidgetPrivate::onActionInsertCompleteMetadata()
{
    if(isActionMode()) {
        regola->insertCompleteMetadata(p->ui->treeWidget);
    }
}

void XmlEditWidgetPrivate::onActionEditMetadata()
{
    MetadataInfo info;
    if(!regola->parseMetadata(&info)) {
        Utils::message(tr("Updatable metadata not found."));
    }
    MetaDataInfoDialog dlg(isReadOnly(), &info, p->window());
    QDialog::DialogCode code = (QDialog::DialogCode)dlg.exec();
    if(!isReadOnly()) {
        if(code == QDialog::Accepted) {
            // apply changes to data
            if(!regola->applyMetadata(getMainTreeWidget(), &info)) {
                Utils::error(p->window(), tr("Problem applying metadata"));
            }
        }
    }
}

bool XmlEditWidgetPrivate::writeData(const QString &filePath)
{
    if(isActionMode()) {
        if(QXmlEditData::isUpdateMetadata()) {
            regola->updateMetadata(p->ui->treeWidget);
        }
    }
    regola->setIndentation(_appData->xmlIndent());
    return regola->write(filePath);
}

bool XmlEditWidgetPrivate::insertProlog()
{
    if(isActionMode()) {
        return regola->insertProlog(p->ui->treeWidget, _appData->insertPrologEncoding());
    }
    return false;
}


bool XmlEditWidgetPrivate::setEncoding(const QString &newEncoding)
{
    if(isActionMode()) {
        return regola->setEncoding(p->ui->treeWidget, _uiDelegate, newEncoding);
    }
    return false;
}

void XmlEditWidgetPrivate::removeAllElements()
{
    if(isActionMode()) {
        regola->removeAllElements(p->ui->treeWidget);
    }
}

void XmlEditWidgetPrivate::showCodePages(QWidget *parent)
{
    CodePageDialog::showCodePage(appData(), parent);
}


void XmlEditWidgetPrivate::onInsertChildContainer()
{
    if(isActionMode()) {
        regola->insertChildContainer(p->ui->treeWidget, getSelectedItem());
    }
}

void XmlEditWidgetPrivate::onInsertParent()
{
    if(isActionMode()) {
        regola->insertParent(p->ui->treeWidget, getSelectedItem());
    }
}

void XmlEditWidgetPrivate::onRemoveParent()
{
    if(isActionMode()) {
        regola->removeParent(p->ui->treeWidget, getSelectedItem());
    }
}

bool XmlEditWidgetPrivate::XSDApplyOperation(const ElementOp::Op op, XSDOperationParameters *params)
{
    if(!isActionMode()) {
        return false ;
    }

    // get the selected item.
    Element *element = getSelectedItem();
    if(NULL == element) {
        return false ;
    }
    return XSDApplyOperationOnElement(op, params, element);
}


bool XmlEditWidgetPrivate::XSDApplyOperationOnElement(const ElementOp::Op op, XSDOperationParameters *params, Element *element)
{
    if(!isActionMode()) {
        return false ;
    }
    XSDHelper helper;
    // get the selected item.
    if(!params->usePrefix()) {
        QString prefix = regola->namespacePrefixXSD();
        params->setXsdNamespacePrefix(prefix);
        params->setUsePrefix(true);
    }
    return helper.doOperation(op, getEditor(), regola, element, element->indexPath(), params);
}


bool XmlEditWidgetPrivate::onXSDInsertElement()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }

        XSDOperationParameters *params = getXSDParams(true, XSDOperationParameters::EOI_ELEMENT, "", NULL);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOInsChild, params, element);
            delete params ;
        }
    }
    return result;
}

bool XmlEditWidgetPrivate::onXSDAppendElement()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }

        XSDOperationParameters *params = getXSDParams(true, XSDOperationParameters::EOI_ELEMENT, "", NULL);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOInsSibling, params, element);
            delete params ;
        }
    }
    return result;
}

bool XmlEditWidgetPrivate::onXSDInsertType()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }

        XSDOperationParameters *params = getXSDParams(true, XSDOperationParameters::EOI_TYPE, "", NULL);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOInsChild, params, element);
            delete params ;
        }
    }
    return result;
}

bool XmlEditWidgetPrivate::onXSDAppendType()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }

        XSDOperationParameters *params = getXSDParams(true, XSDOperationParameters::EOI_TYPE, "", NULL);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOInsSibling, params, element);
            delete params ;
        }
    }
    return result;
}

bool XmlEditWidgetPrivate::onXSDInsertAttribute()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }

        XSDOperationParameters *params = getXSDParams(true, XSDOperationParameters::EOI_ATTRIBUTE, "", NULL);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOInsChild, params, element);
            delete params ;
        }
    }
    return result;
}

bool XmlEditWidgetPrivate::onXSDAppendAttribute()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }

        XSDOperationParameters *params = getXSDParams(true, XSDOperationParameters::EOI_ATTRIBUTE, "", NULL);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOInsSibling, params, element);
            delete params ;
        }
    }
    return result;
}

XSDOperationParameters *XmlEditWidgetPrivate::getXSDParams(const bool isInsert, XSDOperationParameters::EObjectType entityType, const QString &name, Element *selection)
{
    XSDTypeDialog dlg(isInsert, entityType, name, selection, p->window());
    if(dlg.exec() == QDialog::Accepted) {
        return dlg.getParameters();
    }
    return NULL ;
}

bool XmlEditWidgetPrivate::onXSDModifyType()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }
        XSDOperationParameters *params = getXSDParams(false, XSDOperationParameters::EOI_ATTRIBUTE, "", element);
        if(NULL != params) {
            result = XSDApplyOperationOnElement(ElementOp::EOModify, params, element);
            delete params ;
        }
    }
    return result;
}


void XmlEditWidgetPrivate::setOrigDataForAnonPreview(QHash<void *, QString> *newOrigData)
{
    ElementItemSingleDelegate *delegate = _helper.tagDelegate();
    if(NULL != delegate) {
        delegate->setIsAnonPreview(true);
        delegate->setOrigDataForAnonPreview(newOrigData);
    } else {
        if(NULL != newOrigData) {
            delete newOrigData;
        }
    }
}

QHash<void *, QString> *XmlEditWidgetPrivate::anonDataForPreview()
{
    ElementItemSingleDelegate *delegate = _helper.tagDelegate();
    if(NULL != delegate) {
        if(delegate->isAnonPreview()) {
            return delegate->origDataForAnonPreview();
        }
    }
    return NULL;
}


void XmlEditWidgetPrivate::onSearchNext()
{
    Element *selectedElement = findTextOperation(true, FindTextParams::FindNext);
    if(NULL != selectedElement) {
        QTreeWidgetItem *item = NULL ;
        QTreeWidget *tree = getMainTreeWidget() ;
        item = selectedElement->getUI();
        tree->setCurrentItem(item, 0);
        tree-> scrollToItem(item, QAbstractItemView::PositionAtTop);
    }
}


void XmlEditWidgetPrivate::onSearchPrev()
{
    Element *selectedElement = findTextOperation(true, FindTextParams::FindPrevious);
    if(NULL != selectedElement) {
        QTreeWidgetItem *item = NULL ;
        QTreeWidget *tree = getMainTreeWidget() ;
        item = selectedElement->getUI();
        tree->setCurrentItem(item, 0);
        tree-> scrollToItem(item, QAbstractItemView::PositionAtTop);
    }
}

void XmlEditWidgetPrivate::XSDSetNamespaceToParams(XSDOperationParameters *params, Element *element)
{
    regola->XSDSetNamespaceToParams(params, element);
}


XSDAnnotationEditor *XmlEditWidgetPrivate::newEditor(QWidget *window)
{
    return new XSDDefaultAnnotationEditor(window) ;
}

void XmlEditWidgetPrivate::autoDelete()
{
    // do nothing
}

bool XmlEditWidgetPrivate::onEditXSDAnnotation()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }
        XSDHelper helper;
        XSDOperationParameters params;
        XSDSetNamespaceToParams(&params, element);
        Element *origAnnot = helper.findAnnotation(element, &params);
        XSDAnnotationEditProvider * provider = XSDAnnotationEditProviderObject();
        XSDAnnotationEditor *editor = provider->newEditor(p->window());
        editor->exec(origAnnot, &params);
        if(editor->hasResult()) {
            result = helper.doAnnotation(getEditor(), regola, element, origAnnot, editor->annotation());
        }
        delete editor ;
    }
    return result;
}

XSDAnnotationEditProvider *XmlEditWidgetPrivate::XSDAnnotationEditProviderObject()
{
    return _XSDAnnotationEditProvider;
}

void XmlEditWidgetPrivate::setXSDAnnotationEditProviderObject(XSDAnnotationEditProvider *newProvider)
{
    if(NULL != _XSDAnnotationEditProvider) {
        _XSDAnnotationEditProvider->autoDelete();
    }
    _XSDAnnotationEditProvider = newProvider ;
}

void XmlEditWidgetPrivate::onFindNext()
{
    if(!p->ui->searchWidget->isVisible()) {
        onActionFind();
    } else {
        onSearchNext();
    }
}

void XmlEditWidgetPrivate::onFindPrevious()
{
    if(!p->ui->searchWidget->isVisible()) {
        onActionFind();
    } else {
        onSearchPrev();
    }
}

bool XmlEditWidgetPrivate::replaceAll(ReplaceTextParams * findArgs)
{
    findArgs->setFindType(ReplaceTextParams::FindAllOccurrences);
    return replace(findArgs);
}

bool XmlEditWidgetPrivate::replace(ReplaceTextParams * findArgs)
{
    if(!isActionMode()) {
        return false ;
    }
    Element *foundElement = NULL;
    p->ui->treeWidget->setUpdatesEnabled(false);
    p->setEnabled(false);
    Utils::showWaitCursor();

    bool isGlobalSearch = findArgs->findType() == ReplaceTextParams::FindAllOccurrences ;
    findArgs->saveState();
    regola->unhiliteAll();
    findArgs->start();
    foundElement = regola->replaceText(p->getMainTreeWidget(), *findArgs, getSelectedItem());
    if(isGlobalSearch) {
        int replacements = findArgs->replacementCount();
        int errors = findArgs->replacementErrorsCount();
        QString msg ;
        if(replacements > 0) {
            msg = tr("Replaced %1 items").arg(replacements);
        }
        if(errors > 0) {
            msg += tr(" Found %n error(s)", "", errors);
        }
        p->emitShowStatusMessage(p->ui->searchWidget->messageCount(), true);
    }
    if(!isGlobalSearch && (NULL == foundElement)) {
        _uiDelegate->error(p->window(), tr("No match found."));
    } else {
        if(NULL != foundElement) {
            QTreeWidgetItem *item = NULL ;
            QTreeWidget *tree = getMainTreeWidget() ;
            item = foundElement->getUI();
            tree->setCurrentItem(item, 0);
            tree-> scrollToItem(item, QAbstractItemView::PositionAtTop);
        }
    }
    p->ui->treeWidget->setUpdatesEnabled(true);
    p->setEnabled(true);
    Utils::restoreCursor();
    return true ;
}

void XmlEditWidgetPrivate::onReplaceAll()
{
    onReplace(FindTextParams::FindAllOccurrences);
}

void XmlEditWidgetPrivate::onReplace(const FindTextParams::EFindType replaceType)
{
    if(!isActionMode()) {
        return ;
    }

    if(NULL != regola) {

        bool isErrorShown = false;
        bool isError = false;
        ReplaceTextParams* findArgs = p->ui->searchWidget->getReplaceParams(replaceType, NULL);
        if((NULL == findArgs) || ((NULL != findArgs) && !findArgs->checkParams(isErrorShown))) {
            isError = true ;
        }
        if(isError) {
            if(!isErrorShown) {
                Utils::error(p, tr("Unable to start a replace; please, check the parameters."));
            }
        } else {
            if(findArgs->useXQuery()) {
                Utils::error(p, tr("Replace does not support XQuery syntax, please, check the parameters."));
            } else {
                replace(findArgs);
            }
        }
        if(NULL != findArgs) {
            delete findArgs;
        }
    }
}

void XmlEditWidgetPrivate::onReplaceSkipAndGotoNext()
{
    onReplace(FindTextParams::SkipAndGotoNext);
}

void XmlEditWidgetPrivate::onReplaceSkipAndGotoPrevious()
{
    onReplace(FindTextParams::SkipAndGotoPrev);
}

void XmlEditWidgetPrivate::onReplaceReplaceAndGotoNext()
{
    onReplace(FindTextParams::ReplaceAndGotoNext);
}

void XmlEditWidgetPrivate::onReplaceReplaceAndGotoPrevious()
{
    onReplace(FindTextParams::ReplaceAndGotoPrev);
}

void XmlEditWidgetPrivate::onSetIndent()
{
    if(isActionMode() && (NULL != getRegola())) {
        XmlIndentationDialog dlg(p->window(), getRegola(), _appData);
        dlg.exec();
    }
}

void XmlEditWidgetPrivate::onShortcutDelete()
{
    emit p->requestDelete();
}

void XmlEditWidgetPrivate::onShortcutInsert()
{
    emit p->requestInsert();
}

void XmlEditWidgetPrivate::onShortcutAppend()
{
    emit p->requestAppend();
}

void XmlEditWidgetPrivate::onShortcutAppendSpec()
{
    emit p->requestAppendSpec();
}

void XmlEditWidgetPrivate::onShortcutInsertSpec()
{
    emit p->requestInsertSpec();
}

void XmlEditWidgetPrivate::removeNilAttribute()
{
    if(isActionMode() && (NULL != getRegola())) {
        Element *element = getSelectedItem();
        if(NULL != element) {
            getRegola()->removeNillableAttribute(p->getMainTreeWidget(), element, *appData()->namespaceManager());
        }
    }
}

void XmlEditWidgetPrivate::insertNilAttribute()
{
    if(isActionMode() && (NULL != getRegola())) {
        Element *element = getSelectedItem();
        if(NULL != element) {
            getRegola()->insertNillableAttribute(p->getMainTreeWidget(), element, *appData()->namespaceManager());
        }
    }
}

void XmlEditWidgetPrivate::removeXSITypeAttribute()
{
    if(isActionMode() && (NULL != getRegola())) {
        Element *element = getSelectedItem();
        if(NULL != element) {
            getRegola()->removeXSITypeAttribute(p->getMainTreeWidget(), element, *appData()->namespaceManager());
        }
    }
}

void XmlEditWidgetPrivate::insertXSITypeAttribute(const QString &newValue)
{
    if(isActionMode() && (NULL != getRegola())) {
        Element *element = getSelectedItem();
        if(NULL != element) {
            getRegola()->insertXSITypeAttribute(p->getMainTreeWidget(), element, newValue, *appData()->namespaceManager());
        }
    }
}


void XmlEditWidgetPrivate::insertXmlSchemaReferences()
{
    if(isActionMode() && (NULL != getRegola())) {
        Element *element = getRegola()->root();
        if(NULL == element) {
            Utils::error(p->window(), tr("This operation needs a root element."));
        } else {
            NamespaceReferenceEntry entry;
            getRegola()->XSDReferences(&entry);
            SchemaReferencesDialog dlg(p->window(), appData()->namespaceManager(), &entry);
            dlg.setModal(true);
            if(dlg.exec() == QDialog::Accepted) {
                NamespaceReferenceEntry result;
                dlg.getResults(&result);
                insertXsdReference(&result);
            }
        }
    }
}

bool XmlEditWidgetPrivate::insertXsdReference(NamespaceReferenceEntry *entry)
{
    if(isActionMode() && (NULL != getRegola() && (NULL != getRegola()->root()))) {
        regola->insertXSDReference(p->getMainTreeWidget(), *appData()->namespaceManager(), entry);
        return true ;
    }
    return false;
}

void XmlEditWidgetPrivate::showError(const QString &errorMessage)
{
    if(NULL != _uiDelegate) {
        _uiDelegate->error(errorMessage);
    } else {
        Utils::error(p->window(), errorMessage);
    }
}

bool XmlEditWidgetPrivate::showLoadError(const QString &errorMessage, XMLLoadErrorHandler *handler, XMLLoadContext *context, QXmlStreamReader *xmlReader)
{
    if(NULL != handler) {
        if(NULL != _uiDelegate) {
            _uiDelegate->registerError();
        }
        return handler->showErrorAndAskUserIfContinue(p->window(), context, xmlReader);
    } else {
        if(NULL != _uiDelegate) {
            _uiDelegate->error(errorMessage);
        } else {
            Utils::error(p->window(), errorMessage);
        }
        return false;
    }
}

bool XmlEditWidgetPrivate::actionFillSerie()
{
    if(isActionMode() && (NULL != getRegola())) {
        Element *element = getSelectedItem();
        if(NULL != element) {
            ReplicaSettingsDialog dlg(element, p->window());
            dlg.setModal(true);
            if(dlg.exec() == QDialog::Accepted) {
                bool result = false;
                ReplicaCommand *command = dlg.result();
                if(NULL != command) {
                    ReplicaManager manager ;
                    p->window()->setEnabled(false);
                    result = manager.apply(getEditor(), getRegola(), element, command);
                    p->window()->setEnabled(true);
                } else {
                    Utils::errorOutOfMem(p->window());
                }
                if(NULL != command) {
                    delete command ;
                }
                return result ;
            }
        }
    }
    return false;
}

bool XmlEditWidgetPrivate::doReplica(ReplicaCloneInfo *cmd, Element *element)
{
    if(isActionMode() && (NULL != getRegola())) {
        if((NULL != cmd) && (NULL != element) && (element->isElement() && !element->isTopLevel())) {
            UndoReplicaCommand *undo = new UndoReplicaCommand(cmd, getEditor(), regola, element->indexPath());
            regola->addUndo(undo);
            return true ;
        }
    }
    return false;
}

void XmlEditWidgetPrivate::updateAttributeIndentationSettings()
{
    if(NULL != regola) {
        regola->setIndentAttributesSettings(true, _appData->xmlIndentAttributesType(), _appData->xmlIndentAttributes());
    }
}

void XmlEditWidgetPrivate::sortAttributes()
{
    if(isActionMode() && (NULL != getRegola())) {
        getRegola()->sortAttributes();
        repaint();
    }
}

void XmlEditWidgetPrivate::sortAttributesElement(Element *element, const bool isRecursive)
{
    if(isActionMode() && (NULL != getRegola() && (NULL != element))) {
        getRegola()->sortAttributesOfElement(element, isRecursive);
    }
}

void XmlEditWidgetPrivate::deleteSiblings(const RegolaDeleteSiblings::DeleteOptions option, Element *selectedItem)
{
    if(isActionMode() && (NULL != getRegola()) && (NULL != selectedItem) && (NULL != selectedItem->parent())) {
        DeleteSiblingsCommand *deleteSiblingsCommand = new DeleteSiblingsCommand(option, getEditor(), regola, selectedItem->indexPath());
        regola->addUndo(deleteSiblingsCommand);
    }
}

bool XmlEditWidgetPrivate::onEditEnum()
{
    bool result = false;
    if(isActionMode()) {

        Element *element = getSelectedItem();
        if(NULL == element) {
            return false ;
        }
        if(element->localName() != IO_XSD_RESTRICTION) {
            element = element->parent();
        }
        if(NULL == element) {
            return false ;
        }
        XSDHelper helper;
        XSDOperationParameters params;
        XSDSetNamespaceToParams(&params, element);
        QList<Element*> inputElements = helper.findFacetsElements(element, &params);
        QList<XSDFacet*> inputFacets = helper.fromElementsToFacets(inputElements, &params);
        XSDEnumDialog editEnumDialog(p->window(), inputFacets);
        if(editEnumDialog.exec() == QDialog::Accepted) {
            QList<XSDFacet*> finalFacets = editEnumDialog.result();
            setFacets(element, finalFacets);
            EMPTYPTRLIST(finalFacets, XSDFacet);
        }
        EMPTYPTRLIST(inputFacets, XSDFacet);
    }
    return result;
}

void XmlEditWidgetPrivate::setFacets(Element *selection, QList<XSDFacet*> facets)
{
    if(isActionMode()) {
        XSDHelper helper;
        XSDOperationParameters params;
        XSDSetNamespaceToParams(&params, selection);
        QList<Element*> elements = helper.fromFacetsToElements(facets, &params);
        EditXSDEnumCommand * editEnumCommand = new EditXSDEnumCommand(getEditor(), regola, selection->indexPath(), elements);
        regola->addUndo(editEnumCommand);
    }
}

QList<int> XmlEditWidgetPrivate::pathForElement(Element * element)
{
    if(NULL != element) {
        return element->indexPath() ;
    }
    QList<int> path ;
    return path;
}

//----region(names)

void XmlEditWidgetPrivate::prefixRemove(const QString &removedPrefix, Element * element, const TargetSelection::Type targetSelection, const bool isAllPrefixes)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        PrefixRemoveCommand *command = new PrefixRemoveCommand(p->ui->treeWidget, getRegola(), removedPrefix, path, targetSelection, isAllPrefixes);
        getRegola()->addUndo(command);
    }
}

void XmlEditWidgetPrivate::prefixSet(const QString &setPrefix, Element * element, const TargetSelection::Type targetSelection)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        PrefixSetCommand *command = new PrefixSetCommand(p->ui->treeWidget, getRegola(), setPrefix, path, targetSelection);
        getRegola()->addUndo(command);
    }
}

void XmlEditWidgetPrivate::prefixReplace(const QString &oldPrefix, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool isAllPrefixes)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        PrefixReplaceCommand *command = new PrefixReplaceCommand(p->ui->treeWidget, getRegola(), oldPrefix, newPrefix, path, targetSelection, isAllPrefixes);
        getRegola()->addUndo(command);
    }
}

//----endregion(names)

//---region(namespaces)
void XmlEditWidgetPrivate::namespaceRemove(const QString &removedNS, Element *element, const TargetSelection::Type targetSelection, const bool isAllNS, const bool isRemoveDeclarations)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        NamespaceRemoveCommand *command = new NamespaceRemoveCommand(p->ui->treeWidget, getRegola(), removedNS, path, targetSelection, isAllNS, isRemoveDeclarations);
        getRegola()->addUndo(command);
    }
}

void XmlEditWidgetPrivate::namespaceSet(const QString &newNS, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool avoidClash, NamespacesInfo *info)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        QUndoCommand *parentCommand = new QUndoCommand();
        parentCommand->setText(tr("Assign namespace"));
        if(avoidClash) {
            /*NamespaceAvoidClashCommand *namespaceAvoidClashCommand =*/ new NamespaceAvoidClashCommand(p->ui->treeWidget, getRegola(), newNS, newPrefix, info, parentCommand);
        }
        /*NamespaceSetCommand *command =*/ new NamespaceSetCommand(p->ui->treeWidget, getRegola(), newNS, newPrefix, path, targetSelection, parentCommand);
        getRegola()->addUndo(parentCommand);
    }
}

void XmlEditWidgetPrivate::namespaceReplace(const QString &oldNS, const QString &newNS, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool avoidClash, NamespacesInfo *namespacesInfo)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        QUndoCommand *parentCommand = new QUndoCommand();
        parentCommand->setText(tr("Replace namespace"));
        if(avoidClash) {
            /*NamespaceAvoidClashCommand *namespaceAvoidClashCommand =*/ new NamespaceAvoidClashCommand(p->ui->treeWidget, getRegola(), newNS, newPrefix, namespacesInfo, parentCommand);
        }
        /*NamespaceReplaceCommand *command =*/ new NamespaceReplaceCommand(p->ui->treeWidget, getRegola(), oldNS, newNS, newPrefix, path, targetSelection, parentCommand);
        getRegola()->addUndo(parentCommand);
    }
}

void XmlEditWidgetPrivate::namespaceNormalize(const QString &newNS, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool declareOnRoot, const bool avoidClash, NamespacesInfo *namespacesInfo)
{
    if(isActionMode() && (NULL != getRegola())) {
        QList<int> path = pathForElement(element);
        QUndoCommand *parentCommand = new QUndoCommand();
        parentCommand->setText(tr("Normalize namespace"));
        if(avoidClash) {
            /*NamespaceAvoidClashCommand *namespaceAvoidClashCommand =*/ new NamespaceAvoidClashCommand(p->ui->treeWidget, getRegola(), newNS, newPrefix, namespacesInfo, parentCommand);
        }
        /*NamespaceNormalizeCommand *command =*/ new NamespaceNormalizeCommand(p->ui->treeWidget, getRegola(), newNS, newPrefix, path, targetSelection, declareOnRoot, parentCommand);
        getRegola()->addUndo(parentCommand);
    }
}

void XmlEditWidgetPrivate::namespaceAvoidClash(const QString &newNS, const QString &newPrefix, NamespacesInfo *namespacesInfo)
{
    if(isActionMode() && (NULL != getRegola())) {
        NamespaceAvoidClashCommand *command = new NamespaceAvoidClashCommand(p->ui->treeWidget, getRegola(), newNS, newPrefix, namespacesInfo);
        getRegola()->addUndo(command);
    }
}

//---endregion(namespaces)

void XmlEditWidgetPrivate::insertSpecial()
{
    if(isActionMode() && (NULL != getRegola())) {
        NamespaceManager *namespaceManager = _appData->namespaceManager();
        if(NULL != namespaceManager) {
            Element *element = getSelectedItem();
            if((NULL == element) && (NULL != getRegola()->root())) {
                return ;
            }
            namespaceManager->insertElement(getEditor()->window(), p, getEditor(), getRegola(), element, true) ;
        }
    }
}

void XmlEditWidgetPrivate::appendSpecial()
{
    if(isActionMode() && (NULL != getRegola())) {
        NamespaceManager *namespaceManager = _appData->namespaceManager();
        if(NULL != namespaceManager) {
            Element *element = getSelectedItem();
            if((NULL == element) && (NULL != getRegola()->root())) {
                return ;
            }
            namespaceManager->insertElement(getEditor()->window(), p, getEditor(), getRegola(), element, false) ;
        }
    }
}

void XmlEditWidgetPrivate::showSCXMLNavigator(const bool how)
{
    bool shouldApplyNewInfo = false ;
    if(how && !_SCXMLNavigator->isEnabledInfo()) {
        shouldApplyNewInfo = true ;
    }
    _SCXMLNavigator->setEnabledInfo(how);
    if(shouldApplyNewInfo) {
        applySCXML();
    }
    _SCXMLNavigator->setVisible(how);
}

void XmlEditWidgetPrivate::updateTimeout()
{
    _updateTimer.stop();
    if(_SCXMLNavigator->isEnabledInfo()) {
        applySCXML();
    }
    if(_XSLTNavigator->isEnabledInfo()) {
        applyXSLT();
    }
}

void XmlEditWidgetPrivate::applySCXML()
{
    SCXMLInfo *newInfo = NULL ;
    if(_SCXMLNavigator->isEnabledInfo()) {
        newInfo = new SCXMLInfo();
        SCXMLInfo::findInfoStates(regola, newInfo);
        _SCXMLNavigator->applyNewInfo(newInfo);
    }
}

void XmlEditWidgetPrivate::applyXSLT()
{
    if(_XSLTNavigator->isEnabledInfo()) {
        _XSLTNavigator->applyNewInfo(&_xsltHelper);
    }
}


void XmlEditWidgetPrivate::onSCXMLNavigatorGoToState(const QString & /*stateName*/, Element *element)
{
    if(getRegola()->findElement(element)) {
        selectAndShowItem(element);
    }
}

void XmlEditWidgetPrivate::onSCXMLNavigatorEditState(const QString & /*stateName*/, Element *element)
{
    if(isActionMode() && (NULL != getRegola()) && (NULL != element)) {
        if(getRegola()->findElement(element)) {
            selectAndShowItem(element);
            // now, edit it
            specificPropertiesItem(element->getUI(), EditModeForceSpecial);
            Utils::TODO_THIS_RELEASE("check");
        }
    }
}

void XmlEditWidgetPrivate::onXSLTNavigatorGoTo(Element *element)
{
    if(getRegola()->findElement(element)) {
        selectAndShowItem(element);
    }
}

void XmlEditWidgetPrivate::onXSLTNavigatorEdit(Element *element)
{
    if(isActionMode() && (NULL != getRegola()) && (NULL != element)) {
        if(getRegola()->findElement(element)) {
            selectAndShowItem(element);
            // now, edit it
            specificPropertiesItem(element->getUI(), EditModeForceSpecial);
            Utils::TODO_THIS_RELEASE("check");
        }
    }
}


void XmlEditWidgetPrivate::openSiblingsSameLevel()
{
    Element *selection = getSelectedItem();
    if(NULL != selection) {
        openSiblingsSameLevel(selection);
    }
}

void XmlEditWidgetPrivate::openSiblingsSameLevel(Element *element)
{
    if(isActionMode() && (NULL != getRegola()) && (NULL != element)) {
        getEditor()->setUpdatesEnabled(false);
        Element *parent = element->parent();
        Element *grandParent = NULL ;
        if(NULL != parent) {
            grandParent = parent->parent();
            if(NULL != grandParent) {
                QTreeWidgetItem *grandParentItem = grandParent->getUI();
                if(NULL != grandParentItem) {
                    if(!grandParentItem->isExpanded()) {
                        grandParentItem->setExpanded(true);
                    }
                    //---
                    int level2Max = grandParentItem->childCount();
                    FORINT(child2Index, level2Max) {
                        QTreeWidgetItem *child1 = grandParentItem->child(child2Index);
                        if(!child1->isExpanded()) {
                            child1->setExpanded(true);
                        }
                        int level1Max = child1->childCount();
                        FORINT(child1Index, level1Max) {
                            QTreeWidgetItem *child = child1->child(child1Index);
                            if(child->isExpanded()) {
                                child->setExpanded(false);
                            }
                        }
                    }
                }
            }
        }
        getEditor()->setUpdatesEnabled(true);
    }

}

void XmlEditWidgetPrivate::onShortcutT()
{
    editSelection(XmlEditWidgetPrivate::EditModeTextualText);
}

void XmlEditWidgetPrivate::onShortcutCtrlEnter()
{
    editSelection(XmlEditWidgetPrivate::EditModeDetail);
}

void XmlEditWidgetPrivate::onShortcutAltEnter()
{
    editSelection(XmlEditWidgetPrivate::EditModeSpecific);
}

void XmlEditWidgetPrivate::onShortcutShiftEnter()
{
    editSelection(XmlEditWidgetPrivate::EditModeTextual);
}
