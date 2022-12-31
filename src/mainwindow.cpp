/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2022 by Luca Bellonda and individual contributors  *
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
/*
Rules to edit:
1) If no items at all, this is the first item
2) Cannot add brother to the first item (1! root item)
*/
extern const char *APP_TITLE ;
#include <xmlEdit.h>
#include "utils.h"
#include "mainwindow.h"
#include "qxmleditconfig.h"
#include "compare.h"
#include "preferreddirs.h"
#include "test.h"
#include "qxmledit.h"
#include "validatormessagehandler.h"
#include "snippet_interface.h"
#include "aboutdialog.h"
#include "schemavalidator.h"
#include "alloweditemsinsert.h"
#include "SchemaInterface.h"
#include "welcomedialog.h"
#include "widgets/columnarview.h"
#include "sessions/sessionrequester.h"
#include "visualization/datavisualization.h"
#include "uiservices.h"
#include "attributefilterdialog.h"
#include "modules/graph/nodesrelationsdialog.h"
#include "modules/copyattr/copyattributesdialog.h"
#include "modules/copyattr/pasteattributesdialog.h"
#include "modules/widgets/ICompleter.h"
#include "modules/xslt/xsltautomodedialog.h"
#include "tipsaboutvisualappearance.h"
#include "modules/binaryviewer/binaryviewerdialog.h"
#include "modules/xsd/xsdmanager.h"
#include "modules/xsd/xsdelementreferencedialog.h"
#include "widgets/qlabelwithsignals.h"
#include "qxmleditapplication.h"
#include "modules/export/exportoptionsdialog.h"
#include "modules/copyattr/copiedattribute.h"
#include "modules/services/anotifier.h"
#include "modules/style/choosestyledialog.h"
#include "widgets/warningswidget.h"
#include "modules/messages/sourcerelatedmessages.h"
#include "modules/specialized/scxml/scxmlautomodedialog.h"
#include "modules/xslt/xsltexecdialog.h"
#include "modules/style/editingtypesdialog.h"
#include "modules/help/guidedvalidationdialog.h"
#include "modules/help/shortcutsdialog.h"
#include "extraction/extractfragmentsdialog.h"
#include "widgets/infoonkeyboardshoertcuts.h"
#include "widgets/infooneditmode.h"
#include "modules/help/tips.h"
#include "modules/help/searchcommanddialog.h"

#define LONG_TIMEOUT    10000
#define SHORT_TIMEOUT    2000

#define ENC_PREFIX  "action_encoding_"

void doOptions(QWidget * const parentWindow, ApplicationData* data);
void executeTests(QWidget *parent);
void ShowTextInDialoog(QWidget *parent, const QString &text);

#define CS_ELEMENT_TEXT "E"
#define CS_ELEMENT_TAG "T"
#define CS_ATTRIBUTE_NAME "N"
#define CS_ATTRIBUTE_VALUE "V"

#define MAX_LAST_FILES  (20)

const QString MainWindow::ActionTagLastFiles("LastFiles");
const QString MainWindow::ActionTagLastFolders("LastFolders");

MainWindow::MainWindow(const bool isAutoDelete, const bool setIsSlave, ApplicationData *newData, QMainWindow *parent)
    : QMainWindow(parent),
      _uiServices(this),
      data(newData),
      uiDelegate(this),
      _windowIcon(":/icon/images/icon.png"),
      _closing(false)
{
    _isAutoDelete = isAutoDelete;
    isDeleted = false ;
    _infoOnEditMode = NULL ;
    _infoOnKeyboardShortcuts = NULL ;
    _scxmlValidationErrors = NULL ;
    _loadErrorHandler = NULL ;
    _slaveIsClosed = false ;
    _errorCount = 0 ;
    _xsdButton = NULL ;
    _controller.setWindow(this);
    _encodingGroup = NULL ;
    _attrFilter = NULL ;
    started = false ;
    internalStateOk = false;
    _sessionStateWidget = NULL;
    _labelIndentation = NULL ;
    _snippetManager = new SnippetManager();

    _returnCodeAsSlave = 0 ;
    labelSchema = NULL;
    navigationModeIndicator = NULL ;
    maxLastFiles = MAX_LAST_FILES;
    maxPrefDirs = QXmlEditParameters::NUM_PREF_DIRS;
    isSlave = setIsSlave ;
    eventLoop = NULL ;
    _extractResult = NULL ;
    ui.setupUi(this);
    ui.loadWarningWidget->setVisible(false);
    ui.actionAbout->setMenuRole(QAction::AboutRole);
    //----------------------------------------------------------------------------------
    internalStateOk = finishSetUpUi();
    if(!internalStateOk) {
        Utils::error(tr("Error preparing user interface."));
    }
    if(!ui.editor->isReady()) {
        Utils::error(tr("Error preparing edit widget."));
    }

    loadRecentFilesSettings();
    loadPreferredDirsSettings();
    setAcceptDrops(true);
    setUnifiedTitleAndToolBarOnMac(true);

    QString windowTitle = APP_TITLE;
    setWindowTitle(windowTitle);

    if(!isSlave) {
        data->addWindow(this);
    }

    autoTest();
    setWindowIcon(_windowIcon);
    started = true ;
    /********************************** test code **************************
    ui.navigation->setRange(1, 200);
    setDisplayMode(qxmledit::SCAN);
    showNavigationBox();
    ************************************************************************/
    ui.editor->setFocus();
}

MainWindow::~MainWindow()
{
    isDeleted = true ;
    ui.sessionTree->setSessionManager(NULL);
    dismissInfoOnKeyboard();
    dismissInfoEditTypes();
    cleanExtractResults(); //TODO
    removeAttributesFilter();
    if(NULL != _snippetManager) {
        delete _snippetManager;
    }
    forgetData();
}

void MainWindow::forgetData()
{
    if(NULL != data) {
        ui.editor->getPaintInfo()->setColorManager(NULL);
        ui.sessionTree->setSessionManager(NULL);
        disconnect(data, SIGNAL(clipboardDataChanged(bool)), this, SLOT(onClipboardDataChanged(bool)));
        disconnect(data, SIGNAL(stateKeyboardShortcutChanged(bool)), this, SLOT(onStateKeyboardShortcutChanged(bool)));
        if(!isSlave) {
            data->removeWindow(this);
        }
        data = NULL ;
    }
}


ApplicationData *MainWindow::appData()
{
    return data;
}

QString MainWindow::editNodeElementAsXML(const bool isBase64Coded, Element *pElement, const QString & /*text*/, const bool /*isCData*/, bool &isCDataOut, bool &isOk)
{
    QString result;
    MainWindow *mainWindow = new MainWindow(true, true, data, this) ;
    mainWindow->setWindowModality(Qt::WindowModal);
    mainWindow->ui.editor->loadText(pElement->getAsSimpleText(isBase64Coded));
    mainWindow->ui.editor->setCDATA(pElement->isCDATA());
    QEventLoop *eventLoop = new QEventLoop();
    mainWindow->setEventLoop(eventLoop);
    mainWindow->show();
    if(eventLoop->exec() > 0) {
        result = mainWindow->getContentAsText();
        isCDataOut = ui.editor->isCDATA();
        isOk = true;
    } else {
        isOk = false;
    }
    delete mainWindow;
    eventLoop->deleteLater();
    return result;
}

void MainWindow::cleanExtractResults()
{
    if(NULL != _extractResult) {
        delete _extractResult ;
        _extractResult = NULL ;
    }
}

void MainWindow::deleteSchema()
{
    ui.editor->deleteSchema(); //TODO: is it private?
}

//autotest
void MainWindow::autoTest()
{
#ifdef  _DEBUG_BALSAMIQ
    void testBALSAMIQ(QWidget * parent);
    this->show();
    testBALSAMIQ(this);
#endif
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch(e->type()) {
    case QEvent::StyleChange:
        VStyle::changeDefaultBrush(QApplication::palette().text());
        break;
    default:
        break;
    }
}

bool MainWindow::event(QEvent *e)
{
    const bool result = QMainWindow::event(e);
    switch(e->type()) {
    case QEvent::WindowActivate:
        if((NULL != data) && !isDeleted)  {
            data->newWindowActivationStatus(this, true);
        }
        break;
    case QEvent::WindowDeactivate:
        if((NULL != data) && !isDeleted)  {
            data->newWindowActivationStatus(this, false);
        }
        break;
    default:
        break;
    }
    return result ;
}


void MainWindow::on_actionNew_triggered()
{
    execNew();
}

MainWindow *MainWindow::execNew()
{
    if(!checkAbandonChanges()) {
        return NULL ;
    }
    MainWindow *theWindow ;
    if(_controller.isOpenInNewWindow()) {
        theWindow = makeNewWindow();
    } else {
        theWindow = this ;
    }
    theWindow->beforeLoadingNewData();
    theWindow->ui.editor->doNew();
    theWindow->removeAttributesFilter();
    theWindow->updateWindowFilePath();
    theWindow->clearExportFilePath();
    return theWindow;
}

void MainWindow::onRaiseWindow()
{
    activateWindow();
    raise();
    show();
}

void MainWindow::startUIState()
{
    ui.actionPaste->setEnabled(false);
    ui.actionPasteAsSibling->setEnabled(false);
}


bool MainWindow::completeToolBar()
{
    bool result = false ;
    QWidget* spacerWidget = new QWidget();
    if(NULL != spacerWidget) {
        spacerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        spacerWidget->setEnabled(false);
        ui.toolBar->addWidget(spacerWidget);
        result = true ;
    }
    ui.toolBar->addAction(ui.actionShowKeyboardShortcuts);
    ui.toolBar->addAction(ui.actionSearchCommand);
    return result ;
}


bool MainWindow::finishSetUpUi()
{
    if(ui.editor->versionNumber() != VERSION_NUMBER) {
        Utils::error(this, tr("The XML Editor component is not correct (%1)").arg(ui.editor->versionNumber()));
        return false;
    }

    completeToolBar();
    onStateKeyboardShortcutChanged(data->keyboardInfoState());

    ui.messagePanel->setVisible(false);
    _scxmlValidationErrors = new SourceRelatedMessages(NULL);
    ui.messagePanel->widget(0)->layout()->addWidget(_scxmlValidationErrors);
    connect(_scxmlValidationErrors, SIGNAL(navigateTo(QList<int>)), this, SLOT(onSourceNavigateTo(QList<int>)));
    connect(ui.messagePanel, SIGNAL(tabCloseRequested(int)), this, SLOT(onMessagesTabCloseRequested(int)));
    // setup function keys modifier for macos
#if defined(ENVIRONMENT_MACOS)
    ui.actionGo_To_Parent->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F11));
    ui.actionGo_to_Previous_Brother->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F9));
    ui.actionGo_to_Next_Brother->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F10));
    ui.actionCloseThisAllBrothers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F12));
#endif

    ui.editor->setData(data, isSlave, this);
    if(!ui.editor->init()) {
        Utils::error(this, tr("XML Editor initialization failed"));
        return false;
    }
    connect(data, SIGNAL(clipboardDataChanged(bool)), this, SLOT(onClipboardDataChanged(bool)));
    connect(data, SIGNAL(stateKeyboardShortcutChanged(bool)), this, SLOT(onStateKeyboardShortcutChanged(bool)));
    ui.editor->setCopyPathAction(ui.actionCopyElementPathClipboard);
    QActionGroup* editModeActionGroup = new QActionGroup(this);   // autorelease
    ui.actionXMLEditMode->setActionGroup(editModeActionGroup);
    ui.actionXSLTEditMode->setActionGroup(editModeActionGroup);
    ui.actionSCXMLEditMode->setActionGroup(editModeActionGroup);
    updateEditMode();
    connect(ui.editor, SIGNAL(documentIsModified(const bool)), this, SLOT(onDocumentIsModified(const bool)));
    connect(ui.editor, SIGNAL(reevaluateSelectionState()), this, SLOT(onComputeSelectionState()));
    connect(ui.editor, SIGNAL(signalSetClipBoardActionsState(const bool)), this, SLOT(setClipBoardActionsState(const bool)));
    connect(ui.editor, SIGNAL(okClicked()), this, SLOT(onOkClicked()));
    connect(ui.editor, SIGNAL(cancelClicked()), this, SLOT(onCancelClicked()));
    connect(ui.editor, SIGNAL(viewAsXsdRequested()), this, SLOT(on_actionViewAsXsd_triggered()));
    connect(ui.editor, SIGNAL(schemaLabelChanged(const QString &)), this, SLOT(schemaLoadComplete(const QString &)));
    connect(ui.editor, SIGNAL(dataReadyMessage(const QString &)), this, SLOT(onNewMessage(const QString &)));
    connect(ui.editor, SIGNAL(loadCurrentPage(const int)), this, SLOT(navigateToPage(const int)));
    connect(ui.editor, SIGNAL(showStatusMessage(const QString &, const bool)), this, SLOT(onShowStatusMessage(const QString &, const bool)));
    connect(ui.editor, SIGNAL(undoStateUpdated(const bool, const bool)), this, SLOT(onUndoStateUpdated(const bool, const bool)));
    connect(ui.editor, SIGNAL(editModeChanged()), this, SLOT(onChangeEditorMode()));
    connect(ui.editor, SIGNAL(readOnlyStateChanged()), this, SLOT(onReadOnlyStateChanged()));
    connect(ui.editor, SIGNAL(encodingChanged(const QString &)), this, SLOT(onEditorEncodingChanged(const QString &)));
    connect(ui.editor, SIGNAL(indentationChanged(const bool, const int)), this, SLOT(onIndentationChanged(const bool, const int)));
    connect(ui.editor, SIGNAL(schemaValidationError(const QString &, Element *)), this, SLOT(onSchemaValidationError(const QString &, Element *)));
    connect(ui.editor, SIGNAL(newXSDSchemaForValidation(const QString &)), this, SLOT(onNewXSDSchemaForValidation(const QString &)));
    connect(ui.editor, SIGNAL(requestInsert()), this, SLOT(on_actionAddChildElement_triggered()));
    connect(ui.editor, SIGNAL(requestAppend()), this, SLOT(on_actionAppendChildElement_triggered()));
    connect(ui.editor, SIGNAL(requestDelete()), this, SLOT(on_actionCut_triggered()));
    connect(ui.editor, SIGNAL(requestInsertSpec()), this, SLOT(on_actionInsertSpecial_triggered()));
    connect(ui.editor, SIGNAL(requestAppendSpec()), this, SLOT(on_actionAppendSpecial_triggered()));
    connect(ui.editor, SIGNAL(elementDoubleClicked(const uint)), this, SLOT(onEditorElementDoubleClicked(const uint)));
    connect(ui.editor, SIGNAL(editElementEvent(const uint, const uint)), this, SLOT(onEditorEditElementEvent(const uint, const uint)));
    connect(ui.editor, SIGNAL(configure(const QPoint&)), this, SLOT(onEditorConfigure(const QPoint&)));
    //-----
    connect(ui.sessionTree, SIGNAL(fileLoadRequest(const QString&)), this, SLOT(onSessionfileLoadRequest(const QString&)));
    connect(ui.sessionTree, SIGNAL(folderOpenRequest(const QString&)), this, SLOT(onSessionFolderOpenRequest(const QString&)));
    connect(ui.sessionTree, SIGNAL(showSessionManagement()), this, SLOT(onShowSessionManagement()));
    connect(ui.sessionTree, SIGNAL(createSession()), this, SLOT(on_actionNewSession_triggered()));

    //------------------------------ status bar widgets ----------------------------------
    // turn off autoscroll
    navigationModeIndicator = new NavigationMode(statusBar());
    if(NULL == navigationModeIndicator) {
        Utils::error(this, tr("Error creating user interface"));
        return false;
    }
    navigationModeIndicator->setExploreMode(ui.editor->displayMode());
    statusBar()->addPermanentWidget(navigationModeIndicator);

    labelSchema = new QLabel(statusBar());
    if(NULL == labelSchema) {
        Utils::error(this, tr("Error creating user interface"));
        return false;
    }
    setSchemaLabel(tr(""));
    statusBar()->addPermanentWidget(labelSchema);

    labelMode = new QLabel(statusBar());
    if(NULL == labelMode) {
        Utils::error(tr("Error creating user interface"));
        return false;
    }
    setModeLabel();
    labelMode->setToolTip(tr("Indicates the edit modality."));
    statusBar()->addPermanentWidget(labelMode);

    _labelReadOnlyImg = new QLabelWithSignals(statusBar());
    if(NULL == _labelReadOnlyImg) {
        Utils::error(tr("Error creating user interface"));
        return false;
    }
    _labelReadOnlyImg->setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(_labelReadOnlyImg);

    _labelReadOnly = new QLabelWithSignals(statusBar());
    if(NULL == _labelReadOnly) {
        Utils::error(tr("Error creating user interface"));
        return false;
    }
    _labelReadOnly->setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(_labelReadOnly);

    connect(_labelReadOnly, SIGNAL(doubleClicked()), this, SLOT(onReadOnlyGo()));
    connect(_labelReadOnlyImg, SIGNAL(doubleClicked()), this, SLOT(onReadOnlyGo()));

    _labelIndentation = new QLabelWithSignals(statusBar());
    if(NULL == _labelIndentation) {
        Utils::error(tr("Error creating user interface"));
        return false;
    }
    _labelIndentation->setTextFormat(Qt::RichText);
    statusBar()->addPermanentWidget(_labelIndentation);
    connect(_labelIndentation, SIGNAL(doubleClicked()), this, SLOT(on_actionSetIndent_triggered()));

    _sessionStateWidget = new SessionStateWidget(statusBar());
    if(NULL == _sessionStateWidget) {
        Utils::error(tr("Error creating user interface"));
        return false;
    }
    connect(_sessionStateWidget, SIGNAL(doubleClicked()), this, SLOT(on_actionManageSessions_triggered()));
    _sessionStateWidget->setSessionManager(data->sessionManager());
    statusBar()->addPermanentWidget(_sessionStateWidget);

    _attributeFilterWidget = new AttrFilterStatus();
    statusBar()->addPermanentWidget(_attributeFilterWidget);

    //------------------------------ status bar widgets ----------------------------------

    startUIState();
    if(_controller.isOpenInNewWindow()) {
        ui.actionOpenSameWindow->setText(tr("&Open in Same Window..."));
        ui.actionOpenSameWindow->setToolTip(tr("Open in same window."));
    } else {
        ui.actionOpenSameWindow->setText(tr("&Open in New Window..."));
        ui.actionOpenSameWindow->setToolTip(tr("Open in new window."));
    }
    ui.actionShowAttrLine-> setCheckable(true);
    ui.actionShowChildIndex->setCheckable(true);
    ui.actionCompactView->setCheckable(true);
    ui.actionShowAlwaysFullTextComments->setCheckable(true);
    ui.actionHideBrothers->setCheckable(true);
    ui.actionShowBase64->setCheckable(true);
    ui.actionShowCurrentElementTextBase64->setCheckable(true);
    ui.actionShowElementSize->setCheckable(true);

    ui.actionFixedSizeAttributes->setCheckable(true);
    ui.actionShowAttributesLength->setCheckable(true);
    ui.actionShowElementTextLength->setCheckable(true);
    ui.actionHideView->setCheckable(true);

    updateUIFromPaintInfo();

    updateUndoState(false, false);

    connect(ui.editor, SIGNAL(treeContextMenuRequested(const QPoint &)), this, SLOT(treeContextMenu(const QPoint &)));
    //TODO connect( ui.searchLocation, SIGNAL(currentIndexChanged(int)), this, SLOT(searchTypeChanged(int)) );
    ui.menu_XML->setTearOffEnabled(true);

    if(!data->sessionManager()->isEnabled() || isSlave) {
        ui.actionShowCurrentSessionPanel->setVisible(false);
        ui.actionShowCurrentSessionPanel->setEnabled(false);
        ui.sessionTree->setVisible(false);
    } else {
        ui.actionShowCurrentSessionPanel->setChecked(true);
        ui.sessionTree->setVisible(true);
    }
    connect(data->sessionManager(), SIGNAL(enablingChanged()), this, SLOT(onSessionEnablingChanged()));
    connect(data->sessionManager(), SIGNAL(sessionStateChanged(Session::SessionState)), this, SLOT(onHandleSessionState()));
    connect(data->sessionManager(), SIGNAL(editFile(const QString &)), this, SLOT(onSessionfileLoadRequest(const QString&)));
    if(!isSlave) {
        ui.sessionTree->setSessionManager(data->sessionManager());
    }

    // insert the edit menu
    bool isOk = true;
    QToolButton *editTextButton = new QToolButton(this);
    QMenu *editTextMenu = new QMenu(this);
    if((NULL == editTextButton) || (editTextMenu == NULL)) {
        isOk = false ;
    } else {
        editTextMenu->addAction(ui.actionPasteAndSubstituteText);
        editTextMenu->addAction(ui.actionEditInnerBase64Text);
        editTextMenu->addAction(ui.actionEditInnerXML);
        editTextMenu->addAction(ui.actionEditInnerXMLBase64);
        editTextButton->setMenu(editTextMenu);
        editTextButton->setPopupMode(QToolButton::InstantPopup);
        editTextButton->setIcon(QIcon(":/commands/modify"));
        ui.toolBar->insertWidget(ui.actionHelpOnQXmlEdit, editTextButton);
    }

    _xsdButton = new QToolButton(this);
    QMenu *xsdMenu = new QMenu(this);
    if((NULL == _xsdButton) || (xsdMenu == NULL)) {
        isOk = false ;
        if(NULL != xsdMenu) {
            delete xsdMenu ;
        }
    } else {
        xsdMenu->addAction(ui.actionXSDModifyType);
        xsdMenu->addSeparator();
        xsdMenu->addAction(ui.actionXSDInsertElement);
        xsdMenu->addAction(ui.actionXSDInsertAttribute);
        xsdMenu->addAction(ui.actionXSDInsertType);
        xsdMenu->addSeparator();
        xsdMenu->addAction(ui.actionEditXSDAnnotation);
        xsdMenu->addAction(ui.actionEditEnumerationFacet);
        _xsdButton->setMenu(xsdMenu);
        _xsdButton->setPopupMode(QToolButton::InstantPopup);
        _xsdButton->setIcon(QIcon(":/actions/xsd-edit"));
        ui.toolBar->insertWidget(ui.actionHelpOnQXmlEdit, _xsdButton);
    }

    ui.toolBar->insertSeparator(ui.actionHelpOnQXmlEdit);

    ui.actionLastFiles->setVisible(false);
    QMenu *recentFiles = new QMenu(this);
    if(NULL == recentFiles) {
        isOk = false ;
    } else {
        recentFiles->setTitle(tr("Recent Files"));
        ui.menuFile->insertMenu(ui.actionLastFiles, recentFiles);

        if(!buildLastObjects(maxLastFiles, lastFiles, SLOT(onRecentFile()), recentFiles, ActionTagLastFiles)) {
            isOk = false ;
        }
    }

    QMenu *preferredDirsMenu = new QMenu(this);
    if(NULL == preferredDirsMenu) {
        isOk = false ;
    } else {
        preferredDirsMenu->setTitle(tr("Preferred Directories"));
        ui.menuFile->insertMenu(ui.actionLastFiles, preferredDirsMenu);
        preferredDirsMenu->addAction(ui.actionEditPreferredDirectories);
        preferredDirsMenu->addAction(ui.actionAddCurrentDirectory);
        preferredDirsMenu->addSeparator();

        if(!buildLastObjects(maxPrefDirs, preferredDirs, SLOT(onPreferredDir()), preferredDirsMenu, ActionTagLastFolders)) {
            isOk = false ;
        }
    }

    QMenu *pluginsMenu = new QMenu(this);
    if(NULL == pluginsMenu) {
        isOk = false ;
    } else {
        ui.actionPlugins->setMenu(pluginsMenu);
        if(!buildPluginsMenu(SLOT(onPlugin()), pluginsMenu)) {
            isOk = false ;
        }
    }

    //----- encoding
    if(!setupEncoding()) {
        uiDelegate->error(this, tr("Error in encoding set up."));
    }
    //-----

    ui.actionXplore->setEnabled(false);
    ui.actionXplore->setVisible(false);

    onHandleSessionState();

    ui.actionXSDAppendAttribute->setEnabled(false);
    ui.actionXSDInsertAttribute->setEnabled(false);
    ui.actionXSDAppendElement->setEnabled(false);
    ui.actionXSDInsertElement->setEnabled(false);
    ui.actionXSDAppendType->setEnabled(false);
    ui.actionXSDInsertType->setEnabled(false);
    ui.actionXSDModifyType->setEnabled(false);

    bool currentButtonState = Config::getBool(Config::KEY_MAIN_SHOWBUTTONS, true);
    ui.actionShowMainButtons->setChecked(currentButtonState);
    onShowHideMainButtons();

    bool currentStatusBarState = Config::getBool(Config::KEY_MAIN_SHOWSTATUSBAR, true);
    ui.actionShowStatusBar->setChecked(currentStatusBarState);
    onShowHideStatusBar();

    //---- region(experimental)
    //---- endregion(experimental)
    onComputeSelectionState();
    onReadOnlyStateChanged();
    if(NULL != getRegola()) {
        getRegola()->emitIndentationChange();
    }
    return isOk;
}

void MainWindow::updateUIFromPaintInfo()
{
    PaintInfo *paintInfo = ui.editor->getPaintInfo();
    ui.actionShowAttrLine->setChecked(paintInfo->oneAttrPerLine());
    ui.actionShowElementTextLength->setChecked(paintInfo->showElementTextLength());
    ui.actionShowChildIndex->setChecked(paintInfo->indexPerChild());
    ui.actionCompactView->setChecked(paintInfo->compactView());
    ui.actionShowAlwaysFullTextComments->setChecked(paintInfo->showFullComments());
    ui.actionFixedSizeAttributes->setChecked(paintInfo->useFixedLengthFont());
    ui.actionShowAttributesLength->setChecked(paintInfo->showAttributesLength());
    ui.actionShowBase64->setChecked(paintInfo->showUnBase64());
    ui.actionShowElementSize->setChecked(paintInfo->showElementSize());
    ui.actionHideView->setChecked(paintInfo->hideView());
    ui.actionShowAttributesSorted->setChecked(paintInfo->isSortAttributesAlpha());
}

bool MainWindow::setupEncoding()
{
    bool status = true;
    //----- encoding
    QList<int> mibsCodec = QTextCodec::availableMibs();
    QMap<QString, QTextCodec*> sortMap;

    foreach(int mib, mibsCodec) {
        QTextCodec *codec = QTextCodec::codecForMib(mib) ;
        sortMap.insert(codec->name().toUpper(), codec);
    }
    QMenu *menu = new QMenu(this);
    ui.actionEncoding->setMenu(menu);
    _encodingGroup = new QActionGroup(this);
    if(NULL != _encodingGroup) {
        foreach(QString key, sortMap.keys()) {
            QTextCodec *codec = sortMap[key] ;
            QAction *encodingAction = new QAction(key, ui.actionEncoding);
            if(NULL != encodingAction) {
                _encodingGroup->addAction(encodingAction);
                encodingAction->setData(codec->mibEnum());
                encodingAction->setCheckable(true);
                QString actionName = ENC_PREFIX ;
                actionName += key;
                encodingAction->setObjectName(actionName);
                menu->addAction(encodingAction);
            } else {
                status = false;
            }
        }
        connect(_encodingGroup, SIGNAL(triggered(QAction*)), this, SLOT(onEncodingChanged(QAction*)));
    } else {
        status = false ;
    }
    return status;
}

//-
void MainWindow::onHandleSessionState()
{
    SessionManager *sessionManager = data->sessionManager();
    bool isSessionEnabled = sessionManager->isEnabled();
    Session::SessionState sessionState = sessionManager->state();
    bool isDefaultSession = sessionManager->isDefaultSession();

    ui.actionNewSession->setEnabled(isSessionEnabled);
    bool isPauseSessionEnabled = isSessionEnabled && (sessionState == Session::Active);
    ui.actionPauseSession->setEnabled(isPauseSessionEnabled);
    ui.actionPauseSession->setVisible(isPauseSessionEnabled);
    bool isResumeSessionEnabled = !isSlave && isSessionEnabled && (sessionState == Session::Paused);
    ui.actionResumeSession->setEnabled(isResumeSessionEnabled);
    ui.actionResumeSession->setVisible(isResumeSessionEnabled);
    ui.actionCloseSession->setEnabled(!isSlave && isSessionEnabled && (sessionState == Session::Active) && !isDefaultSession);
    ui.actionManageSessions->setEnabled(isSessionEnabled);
    ui.actionSessionDetails->setEnabled(isSessionEnabled && (sessionState != Session::NoSession));
}

bool MainWindow::buildLastObjects(const int maxObjects, QList<QAction*> &cmdList, const char *method, QMenu *parent, const QString &tag)
{
    bool isOk = true ;
    //--- last files and dirs
    for(int i = 0 ; i < maxObjects ; i++) {
        QAction *action = new QAction(this);
        if(NULL != action) {
            cmdList.append(action);
            action->setVisible(false);
            action->setData(tag);
            action->setObjectName(DYNAMIC_ACTION_NAME);
            connect(action, SIGNAL(triggered()), this, method);
            parent->addAction(action);
        } else {
            isOk = false;
        }
    }
    return isOk;
}


QAction *MainWindow::createAnAction(QMenu *menu, const QString &label, const QString &tag, const QString &tooltip)
{
    QAction *action = new QAction(menu);
    action->setText(label);
    action->setData(tag);
    action->setToolTip(tooltip);
    action->setObjectName(DYNAMIC_ACTION_NAME);
    return action ;
    //TODO -14-
}

void MainWindow::repaint()
{
    ui.editor->repaint();
}

void MainWindow::display()
{
    ui.editor->display();
}

void MainWindow::treeContextMenu(const QPoint& position)
{
    bool isActionMode = ui.editor->isActionMode();

    QMenu contextMenu(this);
    if(isActionMode) {
        contextMenu.addAction(ui.actionCut);
    }
    contextMenu.addAction(ui.actionCopy);
    Element *element = getSelectedItem();
    bool isElement = false;
    if((NULL != element) && element->isElement()) {
        contextMenu.addAction(ui.actionCopyElementPathClipboard);
        isElement = true ;
    }
    if(isActionMode) {
        contextMenu.addAction(ui.actionPaste);
        contextMenu.addAction(ui.actionPasteAsSibling);
        QMenu *hierarchyMenu = contextMenu.addMenu(tr("Hierarchy"));
        hierarchyMenu->addAction(ui.actionInsertChildContainer);
        hierarchyMenu->addAction(ui.actionInsertParent);
        hierarchyMenu->addAction(ui.actionRemoveParent);
        hierarchyMenu->addSeparator() ;
        hierarchyMenu->addAction(ui.actionInsertDisablingParent);
    }
    //---
    QMenu *advancedCopyMenu = contextMenu.addMenu(tr("Advanced"));
    if((NULL != element) && element->isElement()) {
        advancedCopyMenu->addAction(ui.actionCopyElementOnly);
        advancedCopyMenu->addSeparator();
        advancedCopyMenu->addAction(ui.actionCopyAttributes);
        if(isActionMode) {
            advancedCopyMenu->addAction(ui.actionPasteLastAttributes);
        }
    }
    QMenu *copyMenu = getEditor()->getCopyMenu();
    if(NULL != copyMenu) {
        advancedCopyMenu->addSeparator();
        foreach(QAction * action, copyMenu->actions()) {
            advancedCopyMenu->addAction(action);
        }
    }
    if(isActionMode) {
        contextMenu.addAction(ui.actionEdit);
        contextMenu.addAction(ui.actionSpecificProperties);
    }
    //---
    contextMenu.addSeparator() ;
    if(isActionMode) {
        if(isElement) {
            contextMenu.addAction(ui.actionPasteAndSubstituteText);
            contextMenu.addAction(ui.actionEditInnerBase64Text);
        }
    }
    if(isActionMode) {
        if(isElement) {
            contextMenu.addAction(ui.actionEditInnerXML);
            contextMenu.addAction(ui.actionEditInnerXMLBase64);
        }
        contextMenu.addAction(ui.actionTransforminSnippet);
        contextMenu.addSeparator() ;
        contextMenu.addAction(ui.actionMoveUp);
        contextMenu.addAction(ui.actionMoveDown);
    }

    if(NULL != element) {
        contextMenu.addAction(ui.actionGo_To_Parent);
        contextMenu.addAction(ui.actionGo_to_Previous_Brother);
        contextMenu.addAction(ui.actionGo_to_Next_Brother);
        if(element->areChildrenLeavesHidden(element->getUI())) {
            contextMenu.addAction(ui.actionShowLeafChildren);
        } else {
            contextMenu.addAction(ui.actionHideLeafChildren);
        }
    }

#ifdef XSD_INTEGRATION
    if(NULL != ui.editor->schema()) {
        //SchemaValidator validator;
        // TODO: redo
        /*QStringList options = validator.checkInsertionPoint(_schemaRoot, regola, getSelectedItem());
        QMenu *menu = new QMenu(&contextMenu);
        if(NULL != menu) {
            ui.actionAllowedSchemaElements->setMenu(menu);
            foreach( QString item, options ) {
                QAction *action = new QAction( item, &contextMenu );
                menu->addAction( action );
            }
        }*/
        contextMenu.addSeparator() ;
        contextMenu.addAction(ui.actionAllowedSchemaElements);
    }
#endif

    if(ui.editor->editMode() == XmlEditWidgetEditMode::XSLT) {
        QMenu *xsltMenu = contextMenu.addMenu(tr("XSLT"));
        ui.editor->addXsltMenuItems(xsltMenu);
    }

    contextMenu.exec(ui.editor->getMainTreeWidget()->mapToGlobal(position));
}

// Solo il salva come...
void MainWindow::onDocumentIsModified(const bool isModified)
{
    TRACEQ(QString("MainWindow::onDocumentIsModified(%1)").arg(isModified));
    Regola *regola = ui.editor->getRegola();
    bool regolaIsEmpty = regola->isEmpty(false);
    ui.actionSave->setEnabled(!regolaIsEmpty && ui.editor->isActionNoScanMode() && !isReadOnly());
    ui.actionSaveAs->setEnabled(!regolaIsEmpty);
    ui.actionSaveACopyAs->setEnabled(!regolaIsEmpty);
    setFileTitle();
    setWindowModified(isModified);
}

//cambia la sel clipb.
void MainWindow::setClipBoardActionsState(const bool isAction)
{
    ui.actionPaste->setEnabled(isAction && ui.editor->isActionMode());
    ui.actionPasteAsSibling->setEnabled(isAction && ui.editor->isActionMode());
}

void MainWindow::onComputeSelectionState()
{
    Regola *regola = getRegola();
    bool isSomeItemSelected = false;
    bool selectSpecials = false;
    bool selectSpecialsUp = false;
    bool selectSpecialsDown = false;
    bool isNormalViewState = true;
    bool canAddChild = false;
    bool isShownAsBase64 = false ;
    bool isXSDPresent = isValidXsd();
    bool isElementSelected = false;
    bool isElementRoot = false;
    bool isRegolaReadOnly = isReadOnly();
    bool hasRoot = (NULL != regola) ? (NULL != regola->root()) : false;
    bool isAtTop = false;

    qxmledit::EDisplayMode displayMode = ui.editor->displayMode();
    bool isExplore = (displayMode != qxmledit::NORMAL) && (displayMode != qxmledit::SCAN) ;
    int numberSelected = ui.editor->getMainTreeWidget()->selectedItems().size();
    if(numberSelected > 0) {
        selectSpecials = true ;
        selectSpecialsUp = true ;
        selectSpecialsDown = true ;
        Element *element = Element::fromItemData(ui.editor->getMainTreeWidget()->selectedItems().at(0));
        selectSpecialsUp = !element->isFirstChild();
        selectSpecialsDown = !element->isLastChild();
        if(element->getType() == Element::ET_ELEMENT) {
            canAddChild = true;
            isElementSelected = true ;
            isElementRoot = element->isTopLevel();
        }
        isNormalViewState = element->isNormalViewState();
        isSomeItemSelected = true ;
        isShownAsBase64 = element->isShownBase64();
        if(NULL == element->parent()) {
            isAtTop = true ;
        }
        //if il primo item
        //disabilita accoda, mvup e down
        //altrimenti se il selected si trova al primo o ultimo posto della catena deselezmv up o down
    } else {
        if(regola->isEmpty(true)) {
            canAddChild = true;
        }
    }
    bool enableGoToParent = false;
    bool enableNextBrother = false;
    bool enablePreviousBrother = false;
    bool isComment = false ;
    Element *element = NULL ;
    if(1 == numberSelected) {
        element = getSelectedItem();
        if(NULL != element) {
            if(element->getType() == Element::ET_COMMENT) {
                isComment = true ;
            }
            if(NULL != element->parent()) {
                enableGoToParent = true;
            }
            bool isLast = false;
            bool isFirst = false;
            element->isFirstOrLastChild(isFirst, isLast);
            enableNextBrother = !isLast;
            enablePreviousBrother = !isFirst ;
        }
    }
    ui.actionAddChildElement->setEnabled(canAddChild && !isExplore);
    ui.actionAppendChildElement->setEnabled(selectSpecials && !isExplore);
    ui.actionAddComment->setEnabled(!isExplore);
    ui.actionAppendComment->setEnabled(selectSpecials && !isExplore);
    ui.actionAppendProcessingInstruction->setEnabled(selectSpecials && !isExplore);
    ui.actionAddProcessingInstruction->setEnabled(!isExplore);
    ui.actionDelete->setEnabled(isSomeItemSelected && !isExplore && !getEditor()->isReadOnly());
    ui.actionEditInnerBase64Text->setEnabled(isSomeItemSelected && !isExplore);
    ui.actionEditInnerXML->setEnabled(isSomeItemSelected && !isExplore);
    ui.actionEditInnerXMLBase64->setEnabled(isSomeItemSelected && !isExplore);
    ui.actionEdit->setEnabled(isSomeItemSelected);
    ui.actionEditAsText->setEnabled(isElementSelected && !getEditor()->isReadOnly());
    ui.actionMoveUp->setEnabled(selectSpecialsUp && !isExplore);
    ui.actionMoveDown->setEnabled(selectSpecialsDown && !isExplore);
    ui.actionCut->setEnabled(isSomeItemSelected && !getEditor()->isReadOnly());
    ui.actionCopy->setEnabled(isSomeItemSelected);
    ui.actionCopyElementOnly->setEnabled(isSomeItemSelected);
    ui.actionGo_to_Next_Brother->setEnabled(enableNextBrother);
    ui.actionGo_to_Previous_Brother->setEnabled(enablePreviousBrother);
    ui.actionGo_To_Parent->setEnabled(enableGoToParent);
    ui.actionToggleBookmark->setEnabled(numberSelected > 0);
    ui.actionClearBookmarks->setEnabled((NULL != regola) ? regola->bookmarkSize() > 0 : false);
    ui.actionGotoNextBookmark->setEnabled((NULL != regola) ? regola->nextBookmark() >= 0 : false);
    ui.actionGotoPreviousBookmark->setEnabled((NULL != regola) ? regola->previousBookmark() >= 0 : false);

    ui.actionDeleteBookmarked->setEnabled((NULL != regola) ? (regola->bookmarkSize() >= 0) /*&& !isExplore*/ : false);
    ui.actionCleanBookmarkedContent->setEnabled((NULL != regola) ? (regola->bookmarkSize() >= 0) && !isExplore : false);

    ui.actionHideBrothers->setEnabled(isSomeItemSelected);
    ui.actionHideBrothers->setChecked(!isNormalViewState);
    ui.actionCloseSiblings->setEnabled(isSomeItemSelected);
    ui.actionCloseItemAndChildren->setEnabled(isSomeItemSelected);
    ui.actionShowCurrentElementTextBase64->setEnabled(isSomeItemSelected && !isExplore);
    ui.actionShowCurrentElementTextBase64->setChecked(isShownAsBase64);
    ui.actionReload->setEnabled((NULL != regola) && !regola->fileName().isEmpty());
    enableZoom();
    ui.actionCopyPathToClipboard->setEnabled((NULL != regola) && !regola->fileName().isEmpty());
    ui.actionShowContainingFolder->setEnabled((NULL != regola) && !regola->fileName().isEmpty());
    ui.actionAddCurrentDirectory->setEnabled((NULL != regola) && !regola->fileName().isEmpty());
    // TODO: test
    //ui.testNext->setEnabled((NULL!=regola)?regola->nextBookmark()>=0:false);
    //ui.testPrev->setEnabled((NULL!=regola)?regola->previousBookmark()>=0:false);

    ui.actionValidate->setEnabled((NULL != regola) && !isExplore);
    ui.actionValidateUsingDocumentReferences->setEnabled((NULL != regola) && !isExplore);

    ui.actionValidateNewFile->setEnabled((NULL != regola) && !isExplore);
    ui.actionTransforminSnippet->setEnabled(isSomeItemSelected && !isExplore);

    ui.actionViewAsXsd->setEnabled(isXSDPresent && !isExplore);

    ui.actionTransformInComment->setEnabled(isSomeItemSelected && !isExplore && !isComment);
    ui.actionExtractElementsFromComment->setEnabled(isSomeItemSelected && !isExplore && isComment);

    ui.actionInsertXmlSchemaReferences->setEnabled((NULL != regola) && !isRegolaReadOnly && (NULL != regola->root()) && !isExplore);

    //TODO: always???? ui.actionExtractFragmentsFromFile->setEnabled(XXX);

    ui.actionNewUsingXMLSchema->setEnabled(true);
    ui.actionNewUsingXMLSchema->setVisible(true);

    evaluateSingleItemLeaves(element);

    ui.actionCompareXSD->setEnabled(isXSDPresent && !isExplore);
    ui.actionExportAttributesSiblingsAsCSVClipboard->setEnabled(isSomeItemSelected);

    ui.actionVisAttrApplyTemporaryProfile->setEnabled(isElementSelected);
    ui.actionVisAttrSaveCurrenteProfile->setEnabled((NULL != _attrFilter) && _attrFilter->isTemporary());
    ui.actionVisAttrRemoveCurrentProfile->setEnabled(NULL != _attrFilter);
    ui.actionViewGraphic->setEnabled(!regola->isEmpty(true));

    ui.actionCopyAttributes->setEnabled(isSomeItemSelected);
    ui.actionCopyElementPathClipboard->setEnabled(isElementSelected);
    ui.actionPasteAttributes->setEnabled(isSomeItemSelected && !data->copyAttributesManager()->isEmpty());
    ui.actionPasteLastAttributes->setEnabled(isSomeItemSelected && !data->copyAttributesManager()->isEmpty());
    ui.actionExpandSelectedItem->setEnabled(isSomeItemSelected);
    ui.actionInsertMetadata->setEnabled(!getEditor()->isReadOnly());
    ui.actionInsertCompleteMetadata->setEnabled(!getEditor()->isReadOnly());
    ui.actionExportAsJavaJavascriptSource->setEnabled(!getRegola()->isEmpty(false));
    ui.actionExportAsCSource->setEnabled(!getRegola()->isEmpty(false));
    ui.actionInsertProlog->setEnabled(!getEditor()->isReadOnly());
    ui.actionAddFormattingInfo->setEnabled(!getEditor()->isReadOnly());
    ui.actionRemoveFormattingInfo->setEnabled(!getEditor()->isReadOnly());
    ui.actionEncoding->setEnabled(!getEditor()->isReadOnly());
    ui.actionInsertChildContainer->setEnabled(isElementSelected);
    ui.actionRemoveParent->setEnabled((NULL != element) && element->canRemoveParent());
    ui.actionInsertParent->setEnabled((NULL != element) && element->canInsertParent());
    ui.actionInsertDisablingParent->setEnabled((NULL != element) && element->canInsertParent());

    XSDManager *xsdManager = data->xsdManager();
    QString parentTag;
    QString elmTag ;
    if((NULL != element) && element->isElement()) {
        Element * parent = element->parent();
        elmTag = element->localName();
        if(NULL != parent) {
            parentTag = parent->localName();
        }
    }
    ui.actionXSDAppendAttribute->setEnabled(xsdManager->canInsertAttribute(parentTag) && !isRegolaReadOnly);
    ui.actionXSDInsertAttribute->setEnabled(xsdManager->canInsertAttribute(elmTag) && !isRegolaReadOnly);
    ui.actionXSDAppendElement->setEnabled(xsdManager->canInsertElement(parentTag) && !isRegolaReadOnly);
    ui.actionXSDInsertElement->setEnabled(xsdManager->canInsertElement(elmTag) && !isRegolaReadOnly);
    ui.actionXSDAppendType->setEnabled(xsdManager->canInsertType(parentTag) && !isRegolaReadOnly);
    ui.actionXSDInsertType->setEnabled(xsdManager->canInsertType(elmTag) && !isRegolaReadOnly);
    ui.actionXSDModifyType->setEnabled(xsdManager->canModifyType(elmTag) && !isRegolaReadOnly);
    ui.actionEditXSDAnnotation->setEnabled(xsdManager->canEditAnnotation(elmTag) && !isRegolaReadOnly);
    ui.actionEditEnumerationFacet->setEnabled((xsdManager->canEditFacetAsParent(parentTag) || xsdManager->canEditFacet(elmTag)) && !isRegolaReadOnly);

    ui.actionRemoveNilAttribute->setEnabled(isElementSelected && !getEditor()->isReadOnly());
    ui.actionInsertNilAttribute->setEnabled(isElementSelected && !getEditor()->isReadOnly());
    ui.actionRemoveXSITypeAttribute->setEnabled(isElementSelected && !getEditor()->isReadOnly());
    ui.actionInsertXSITypeAttribute->setEnabled(isElementSelected && !getEditor()->isReadOnly());
    ui.actionInsertXmlSchemaReferences->setEnabled(!getEditor()->isReadOnly());
    ui.actionFillSerie->setEnabled(!getEditor()->isReadOnly() && isElementSelected);
    ui.actionCloneElements->setEnabled(!getEditor()->isReadOnly() && isElementSelected && !isElementRoot);
    ui.actionExportElementToFile->setEnabled(isElementSelected);

    ui.actionSortAttributes->setEnabled(!isRegolaReadOnly);
    ui.actionSortAttributesElement->setEnabled(!isRegolaReadOnly && isElementSelected);
    ui.actionSortAttributesElementRecursive->setEnabled(!isRegolaReadOnly && isElementSelected);

    ui.actionRemoveAllSiblings->setEnabled(!isRegolaReadOnly && isElementSelected);
    ui.actionRemoveAllSiblingsAfter->setEnabled(!isRegolaReadOnly && isElementSelected);
    ui.actionRemoveAllSiblingsBefore->setEnabled(!isRegolaReadOnly && isElementSelected);
    ui.actionInsertSnippet->setEnabled(!getEditor()->isReadOnly());

    ui.actionInsertSpecial->setEnabled(!getEditor()->isReadOnly() && (isElementSelected || (!isSomeItemSelected && !hasRoot)));
    ui.actionAppendSpecial->setEnabled(!getEditor()->isReadOnly() && ((isAtTop && !hasRoot) || (!isAtTop && isSomeItemSelected))) ;
    ui.actionOpenSiblingsAtTheSameLevel->setEnabled(isElementSelected);
    ui.actionGenerateXMLFromXSD->setEnabled(isXSDPresent && !isExplore);

    onComputeSelectionStateExperimentalFeatures();
    data->newSelectionState(this);
}

void MainWindow::onComputeSelectionStateExperimentalFeatures()
{
    if(data->areExperimentalFeaturesEnabled()) {
        // insert here experimental features
    }
}

void MainWindow::evaluateSingleItemLeaves(Element *element)
{
    bool isHiddenLeaves = false;
    bool isVisibleLeaves = false;
    if(NULL != element) {
        if(!element->isALeaf()) {
            if(element->areChildrenLeavesHidden(element->getUI())) {
                isHiddenLeaves = true ;
            } else {
                isVisibleLeaves = true;
            }
        }
    }
    ui.actionHideLeafChildren->setEnabled(isVisibleLeaves);
    ui.actionShowLeafChildren->setEnabled(isHiddenLeaves);
}

void MainWindow::enableZoom()
{
    PaintInfo *paintInfo = ui.editor->getPaintInfo();
    ui.actionZoomIn->setEnabled(paintInfo->canZoomIn());
    ui.actionZoomOut->setEnabled(paintInfo->canZoomOut());
}

Element *MainWindow::getSelectedItem()
{
    return ui.editor->getSelectedItem();
}

void MainWindow::resetTree()
{
    ui.editor->resetTree();
}

void MainWindow::on_actionOpen_triggered()
{
    openFileUsingDialog(getRegola()->fileName());
}

bool MainWindow::openFileUsingDialog(const QString folderPath, const EWindowOpen useWindow)
{
    if(!checkAbandonChanges(useWindow)) {
        return false;
    }
    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Open File"),
                           QXmlEditData::sysFilePathForOperation(folderPath),
                           Utils::getFileFilterForOpenFile()
                       );
    if(!filePath.isEmpty()) {
        return loadFile(filePath, true, useWindow);
    }
    return false ;
}

void MainWindow::on_actionNewFromClipboard_triggered()
{
    if(!checkAbandonChanges()) {
        return ;
    }
    createFromClipboard();
}

bool MainWindow::newFromClipboard()
{
    if(ui.editor->onActionNewFromClipboard()) {
        removeAttributesFilter();
        updateWindowFilePath();
        clearExportFilePath();
        activateModesOnNewFile();
        return true;
    }
    return false ;
}

bool MainWindow::newFromString(const QString &newData)
{
    if(ui.editor->newFromString(newData)) {
        removeAttributesFilter();
        updateWindowFilePath();
        clearExportFilePath();
        activateModesOnNewFile();
        return true;
    }
    return false ;
}

void MainWindow::on_actionSaveAs_triggered()
{
    Regola * regola = getRegola();
    if(NULL == regola) {
        error(tr("No data to save."));
        return ;
    }
    if(regola->isEmpty(false)) {
        error(tr("Cannot write an empty file."));
        return ;
    }

    if(isSlave) {
        on_actionSaveACopyAs_triggered();
        return ;
    }

    QString newFilePath = askFileNameForSaving(regola->fileName());
    if(newFilePath.isEmpty()) {
        return ;
    }
    actionSaveAs_internal(newFilePath);
}

void MainWindow::actionSaveAs_internal(const QString &newFilePath)
{
    Regola * regola = getRegola();
    checkForSaveEncoding();
    if(!getEditor()->writeData(newFilePath)) {
        error(tr("Error saving data. Old file is still intact."));
        return ;
    }
    if(ui.editor->displayMode() != qxmledit::SCAN) {
        regola->setFileName(newFilePath);
    }
    updateRecentFilesMenu(newFilePath);
    regola->setModified(false);
    data->sessionManager()->enrollFile(newFilePath);
    statusBar()->showMessage(fileSavedMessage(newFilePath), SHORT_TIMEOUT);
    updateWindowFilePath();
    ui.loadWarningWidget->setVisible(false);
}

QString MainWindow::fileSavedMessage(const QString &newFilePath)
{
    QString filePathReduced = newFilePath.length() > 20 ? QString("...%1").arg(newFilePath.right(15)) : newFilePath;
    return tr("File saved %1").arg(filePathReduced);
}

void MainWindow::on_actionSaveACopyAs_triggered()
{
    Regola * regola = getRegola();
    if(NULL == regola) {
        error(tr("No data to save."));
        return ;
    }
    if(regola->isEmpty(false)) {
        error(tr("Cannot write an empty file."));
        return ;
    }
    QString newFilePath = askFileNameForSaving(regola->fileName());
    if(newFilePath.isEmpty()) {
        return ;
    }
    actionSaveACopyAs_internal(newFilePath);
}

void MainWindow::actionSaveACopyAs_internal(const QString &newFilePath)
{
    Regola * regola = getRegola();
    bool modifiedStatus = regola->isModified();

    checkForSaveEncoding();
    if(!regola->write(newFilePath, false)) {
        error(tr("Error saving data. Old file is still intact."));
        return ;
    }
    updateRecentFilesMenu(newFilePath);
    regola->setModified(modifiedStatus);
    statusBar()->showMessage(fileSavedMessage(newFilePath), SHORT_TIMEOUT);
}

void MainWindow::on_actionSave_triggered()
{
    Regola * regola = getRegola();
    if(ui.editor->isReadOnly()) {
        return ;
    }

    if(!ui.editor->isActionNoScanMode()) {
        return ;
    }
    if(ui.editor->displayMode() == qxmledit::SCAN) {
        return ;
    }
    if(regola->fileName().isEmpty()) {
        on_actionSaveAs_triggered();
        return ;
    }
    if(regola->isEmpty(false)) {
        error(tr("Cannot write empty file."));
        return ;
    }
    execActionSave();
}

void MainWindow::execActionSave()
{
    Regola * regola = getRegola();

    // scrivi il nuovo con il nome vecchio+estensione
    QString newFilePath = regola->fileName() + ".new_new~"  ;
    if(! actionSave_internal(newFilePath)) {
        error(tr("Error saving data. Old file is still intact."));
        return ;
    }
    // rinomina il vecchio
    QString backupFilePath = regola->fileName() + "~"  ;
    if(QFile::exists(backupFilePath)) {
        if(!QFile::remove(backupFilePath)) {
            error(tr("Error saving data: cleaning backup file."));
            return ;
        }
    }
    if(!QFile::rename(regola->fileName(), backupFilePath)) {
        error(tr("Error renaming old file. You can access written data at file '%1'. Old data are untouched").arg(newFilePath));
        return ;
    }
    // rinomina il nuovo nel nome vecchio
    if(!QFile::rename(newFilePath, regola->fileName())) {
        error(tr("Error renaming new file. You can access written data at file '%1'. You can find old data in the backup file").arg(newFilePath));
        return ;
    }
    regola->setModified(false);
    appData()->notifier()->notify(this, fileSavedMessage(regola->fileName()));
}

bool MainWindow::actionSave_internal(const QString &newFilePath)
{
    checkForSaveEncoding();
    return getEditor()->writeData(newFilePath);
}


MainWindow *MainWindow::getParentMainWindow()
{
    MainWindow *theParent = dynamic_cast<MainWindow*>(parent());
    return theParent;
}

QString MainWindow::fileNameOrFolder(const QString &filePath)
{
    if(filePath.isEmpty()) {
        return "";
    }
    QFileInfo info(filePath);
    return info.path();
}

QString MainWindow::thisOrParentFileName()
{
    QString filePath;
    if(NULL != getRegola()) {
        filePath = getRegola()->fileName();
    }
    if(!filePath.isEmpty()) {
        return fileNameOrFolder(filePath);
    }
    if(isSlave) {
        MainWindow *parentWindow = getParentMainWindow();
        if(NULL != parentWindow) {
            return parentWindow->thisOrParentFileName();
        }
    }
    return "";
}

QString MainWindow::hierarchyFileName(const QString &filePath)
{
    if(!filePath.isEmpty()) {
        return filePath;
    }
    return thisOrParentFileName();
}

QString MainWindow::fileNameForSaving(const QString &actualName)
{
    return QXmlEditData::sysFilePathForOperation(hierarchyFileName(actualName));
}

QString MainWindow::askFileNameForSaving(const QString &actualName)
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Data"),
                       fileNameForSaving(actualName), Utils::getFileFilterForOpenFile());

    if(!filePath.isEmpty()) {
        return filePath;
    }
    return "";
}

void MainWindow::errorNoRule()
{
    error(tr("No main structure"));
}

void MainWindow::on_actionAddChildElement_triggered()
{
    ui.editor->onActionAddChildElement();
}

void MainWindow::on_actionAppendChildElement_triggered()
{
    ui.editor->onActionAppendChildElement();
}

void MainWindow::on_actionEdit_triggered()
{
    ui.editor->onActionEdit();
}

void MainWindow::on_actionDelete_triggered()
{
    ui.editor->onActionDelete();
}
void MainWindow::on_actionMoveUp_triggered()
{
    ui.editor->onActionMoveUp();
}

void MainWindow::on_actionMoveDown_triggered()
{
    ui.editor->onActionMoveDown();
}

void MainWindow::on_actionCut_triggered()
{
    ui.editor->onActionCut();
}

void MainWindow::on_actionCopy_triggered()
{
    ui.editor->onActionCopy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui.editor->onActionPaste();
}

void MainWindow::showAbout(QWidget *theParent, ApplicationData *data)
{
    QList<AuthorInfo*> authors = authorsInfo() ;
    AboutDialog about(theParent, data,
                      AuthorInfo::appName, AuthorInfo::version, AuthorInfo::copyright,
                      AuthorInfo::license,
                      AuthorInfo::other,
                      authors);
    about.setModal(true);
    about.exec() ;
    deleteAuthorsInfo(authors);
}

void MainWindow::on_actionAbout_triggered()
{
    showAbout(this, data);
}


void MainWindow::onShowStatusMessage(const QString &message, const bool isLongTimeout)
{
    statusBar()->showMessage(message, isLongTimeout ? LONG_TIMEOUT : SHORT_TIMEOUT);
}

void MainWindow::setFileTitle()
{
    QString windowTitle = APP_TITLE;
    if(!isSlave) {
        windowTitle.append(" - ");
        if(!getRegola()->fileName().isEmpty()) {
            windowTitle.append(getRegola()->fileName());
        } else {
            windowTitle.append(tr("--No file--"));
        }
        windowTitle.append(" [*]");
    }
    setWindowTitle(windowTitle);
}

void MainWindow::closeEvent(QCloseEvent * event)
{
#ifdef  ENVIRONMENT_MACOS
    // workaround for Qt5 bug, double close message
    if(_closing) {
        event->accept();
        return ;
    }
#endif
    const bool slaveCondition = (isSlave && (0 == _returnCodeAsSlave));
    const bool notSlaveCondition = !isSlave ;
    if(slaveCondition || notSlaveCondition) {
        if(!checkAbandonChanges(OpenUsingSameWindow)) {
            event->ignore();
            return ;
        }
    }
    event->accept();
    _closing = true;
    if(!isSlave) {
        // disable events until destroyed
        forgetData();
        if(_isAutoDelete) {
            deleteLater();
        }
    } else {
        _slaveIsClosed = true ;
        if(NULL != eventLoop) {
            eventLoop->exit(_returnCodeAsSlave);
        }
    }
}

bool MainWindow::askForQuit()
{
    return checkAbandonChanges(OpenUsingSameWindow);
}

void MainWindow::on_actionQuit_triggered()
{
    if(askForQuit()) {
        qApp->quit();
    }
}

void MainWindow::on_actionExpandAll_triggered()
{
    ui.editor->onActionExpandAll();
}

void MainWindow::on_actionExpandSelectedItem_triggered()
{
    ui.editor->onActionExpandSelectedItem();
}

void MainWindow::on_actionShowAttrLine_triggered()
{
    ui.editor->onActionShowAttrLine(ui.actionShowAttrLine->isChecked());
}

void MainWindow::on_actionAddComment_triggered()
{
    ui.editor->onActionAddComment();
}

void MainWindow::on_actionAppendComment_triggered()
{
    ui.editor->onActionAppendComment();
}

void MainWindow::on_actionAppendProcessingInstruction_triggered()
{
    ui.editor->onActionAppendProcessingInstruction();
}


void MainWindow::on_actionAddProcessingInstruction_triggered()
{
    ui.editor->onActionAddProcessingInstruction();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat(Utils::URIDropType)) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat(Utils::URIDropType)) {
        QString filePath = "" ;
        event->acceptProposedAction();
        if(event->mimeData()->hasUrls()) {
            foreach(QUrl url, event->mimeData()->urls()) {
                filePath = url.toLocalFile();
                break;
            }
        }
        if(!openDroppedFile(filePath)) {
            event->ignore();
            return ;
        }
        event->acceptProposedAction();
    }
}

bool MainWindow::openDroppedFile(const QString &filePath)
{
    if(!filePath.isEmpty()) {
        if(!checkAbandonChanges()) {
            return false ;
        }
        return loadFile(filePath);
    }
    return false;
}

void MainWindow::on_actionFind_triggered()
{
    ui.editor->onActionFind();
}

void MainWindow::on_actionReplace_triggered()
{
    ui.editor->onActionReplace();
}

void MainWindow::on_actionGo_To_Parent_triggered()
{
    ui.editor->onActionGoToParent();
}

void MainWindow::on_actionGo_to_Previous_Brother_triggered()
{
    ui.editor->onActionGoToPreviousBrother();
}

void MainWindow::on_actionGo_to_Next_Brother_triggered()
{
    ui.editor->onActionGoToNextBrother();
}

void MainWindow::on_actionClearBookmarks_triggered()
{
    ui.editor->onActionClearBookmarks();
}

void MainWindow::on_actionToggleBookmark_triggered()
{
    ui.editor->onActionToggleBookmark();
}


void MainWindow::on_actionGotoNextBookmark_triggered()
{
    ui.editor->onActionGotoNextBookmark();
}

void MainWindow::on_actionGotoPreviousBookmark_triggered()
{
    ui.editor->onActionGotoPreviousBookmark();
}

void MainWindow::on_actionShowChildIndex_triggered()
{
    ui.editor->onActionShowChildIndex(ui.actionShowChildIndex->isChecked());
}

void MainWindow::on_actionCompactView_triggered()
{
    ui.editor->onActionCompactView(ui.actionCompactView->isChecked());
}

void MainWindow::on_actionShowAttributesSorted_triggered()
{
    ui.editor->onActionShowAttributesSorted(ui.actionShowAttributesSorted->isChecked());
}

void MainWindow::on_actionShowAlwaysFullTextComments_triggered()
{
    ui.editor->onActionShowAlwaysFullTextComments(ui.actionShowAlwaysFullTextComments->isChecked());
}

void MainWindow::on_actionFixedSizeAttributes_triggered()
{
    ui.editor->onActionFixedSizeAttributes(ui.actionFixedSizeAttributes->isChecked());
}

void MainWindow::on_actionShowAttributesLength_triggered()
{
    ui.editor->onActionShowAttributesLength(ui.actionShowAttributesLength->isChecked());
}

void MainWindow::on_actionShowElementTextLength_triggered()
{
    ui.editor->onActionShowElementTextLength(ui.actionShowElementTextLength->isChecked());
}

void MainWindow::on_actionCloseThisAllBrothers_triggered()
{
    ui.editor->onActionCloseThisAllBrothers();
}

void MainWindow::on_actionHideBrothers_triggered()
{
    ui.editor->onActionHideBrothers();
}

void MainWindow::on_actionCloseSiblings_triggered()
{
    ui.editor->closeSiblings();
}

void MainWindow::on_actionCloseItemAndChildren_triggered()
{
    ui.editor->closeItemAndChildren();
}

void MainWindow::on_actionShowCurrentElementTextBase64_triggered()
{
    ui.editor->onActionShowCurrentElementTextBase64(ui.actionShowCurrentElementTextBase64->isChecked());
}

void MainWindow::on_actionShowBase64_triggered()
{
    ui.editor->onActionShowBase64(ui.actionShowBase64->isChecked());
}

void MainWindow::on_actionShowElementSize_triggered()
{
    ui.editor->onActionShowElementSize(ui.actionShowElementSize->isChecked());
}

void MainWindow::on_actionConfigure_triggered()
{
    if(data->preferences(this)) {
        data->updateEditors(true);
    } else {
        data->updateEditors(false);
    }
}

void MainWindow::updateFullAfterPreferences()
{
    getEditor()->getPaintInfo()->loadState();
    // update UI
    updateUIFromPaintInfo();
    getEditor()->invalidatePaintData(true);
}

void MainWindow::updateAfterPreferences()
{
    getEditor()->updateAttributeIndentationSettings();
    bool isChanged = false;
    if(getEditor()->getPaintInfo()->updateAttributeColumnsLimit()) {
        isChanged = true ;
    }
    if(getEditor()->getPaintInfo()->updateShowElementIcon()) {
        isChanged = true ;
    }
    if(getEditor()->getPaintInfo()->updateShowElementTag()) {
        isChanged = true ;
    }
    if(isChanged) {
        getEditor()->getPaintInfo()->loadState();
        // update UI
        updateUIFromPaintInfo();
        getEditor()->invalidatePaintData();
    }
}

void MainWindow::onOkClicked()
{
    if(isSlave && _slaveIsClosed) {
        return ;
    }
    _returnCodeAsSlave = 1 ;
    if(isSlave && (NULL != eventLoop)) {
        close();
    }
}

void MainWindow::onCancelClicked()
{
    if(isSlave && _slaveIsClosed) {
        return ;
    }
    _returnCodeAsSlave = 0 ;
    if(isSlave && (NULL != eventLoop)) {
        close();
    }
}

void MainWindow::setEventLoop(QEventLoop *value)
{
    eventLoop = value ;
}

void MainWindow::on_actionEditInnerXML_triggered()
{
    ui.editor->onActionEditInnerXML();
}

void MainWindow::on_actionEditInnerXMLBase64_triggered()
{
    ui.editor->onActionEditInnerXMLBase64();
}


void MainWindow::on_actionEditInnerBase64Text_triggered()
{
    ui.editor->onActionEditInnerBase64Text();
}

QString MainWindow::getContentAsText()
{
    return ui.editor->getContentAsText();
}

void MainWindow::on_actionZoomIn_triggered()
{
    ui.editor->onActionZoomIn();
    enableZoom();
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui.editor->onActionZoomOut();
    enableZoom();
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
        int numDegrees = event->delta() / 8;
        if(numDegrees > 10) {
            on_actionZoomIn_triggered();
        } else if(numDegrees < -10) {
            on_actionZoomOut_triggered();
        }
        event->accept();
    } else {
        event->ignore();
    }
}



void MainWindow::on_actionCompare_triggered()
{
#ifdef DEBUG_COMPARE
#define FILE_TEST_C   "../src/data/compare_base.xml"
    loadFile(FILE_TEST_C);
#endif
    Regola *regola = getRegola();
    if(NULL == regola) {
        errorNoRule();
        return ;
    }
    CompareBridge::doCompare(this, true, regola, this, data->lastFiles());
}

void MainWindow::on_actionReload_triggered()
{
    reload();
}

bool MainWindow::reload()
{
    Regola *regola = getRegola();
    if(NULL == regola) {
        return false ;
    }
    QString filePath = regola->fileName();
    if(filePath.isEmpty()) {
        return false ;
    }
    if(!checkAbandonChanges(OpenUsingSameWindow)) {
        return false ;
    }
    // save presets
    RegolaSettings *settings = regola->getSettings();
    bool result = loadFile(filePath, false, OpenUsingSameWindow);
    // restore presets
    getRegola()->restoreSettings(settings);
    activateModesOnNewFile();
    delete settings ;
    return result;
}

void MainWindow::onRecentFile()
{
    QAction *actionFile = qobject_cast<QAction*>(sender());
    if(NULL != actionFile) {
        QString filePath = actionFile->data().toString();
        if(!filePath.isEmpty()) {
            recentFile(filePath);
        }
    }
}

bool MainWindow::recentFile(const QString &filePath)
{
    if(!filePath.isEmpty()) {
        if(!checkAbandonChanges()) {
            return false;
        }
        return loadFile(filePath);
    }
    return false ;
}

void MainWindow::onPreferredDir()
{
    QAction *actionFile = qobject_cast<QAction*>(sender());
    if(NULL != actionFile) {
        QString dirPath = actionFile->data().toString();
        preferredDir(dirPath);
    }
}

bool MainWindow::preferredDir(const QString &dirPath)
{
    if(!checkAbandonChanges()) {
        return false;
    }
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                       QXmlEditData::sysFilePathForOperation(dirPath), Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        return preferredDirLoadFile(filePath);
    }
    return false;
}

bool MainWindow::preferredDirLoadFile(const QString &filePath)
{
    if(!filePath.isEmpty()) {
        return loadFile(filePath);
    }
    return false;
}

void MainWindow::updateMRU(const QString &entry, QList<QAction*>actions)
{
    if(entry.isEmpty()) {
        return;
    }
    if(!data->addFileAccess(entry)) {
        Utils::error(tr("Error registering last open files."));
    }
    updateEntriesMenu(data->lastFiles(), actions);
}

void MainWindow::updateEntriesMenu(QStringList &entries, QList<QAction*>actions)
{
    int listSize = entries.size();
    int nMaxActions = actions.size();
    for(int i = 0 ; i < nMaxActions ; i ++) {
        QAction *action = actions.at(i);
        if(i < listSize) {
            QString data = entries.at(i);
            action->setData(data);
            action->setText(data);
            action->setVisible(true);
        } else {
            action->setVisible(false);
        }
    }
}

void MainWindow::updateRecentFilesMenu(const QString &filePath)
{
    updateMRU(filePath, lastFiles);
}

void MainWindow::loadRecentFilesSettings()
{
    data->lastFiles().clear();
    Config::loadStringArray(Config::KEY_MAIN_RECENTFILES, data->lastFiles());
    updateEntriesMenu(data->lastFiles(), lastFiles);
}

void MainWindow::on_actionCopyPathToClipboard_triggered()
{
    ui.editor->onActionCopyPathToClipboard();
}

void MainWindow::on_actionShowContainingFolder_triggered()
{
    Regola *regola = getRegola();
    if((NULL != regola) && !regola->fileName().isEmpty()) {
        QFileInfo info(regola->fileName());
        QDir dir = info.dir();
        if(dir.exists()) {
            if(!QDesktopServices::openUrl(QUrl::fromLocalFile(dir.absolutePath()))) {
                error(tr("Error opening folder."));
            }
        } else {
            error(tr("The folder does not exists"));
        }
    }
}

void MainWindow::on_actionAddCurrentDirectory_triggered()
{
    Regola *regola = getRegola();
    if((NULL != regola) && !regola->fileName().isEmpty()) {
        if(!PreferredDirs::checkNewDir(preferredDirsNames.size(), true)) {
            return;
        }
        QFileInfo fileInfo(regola->fileName());
        QString dirPath = fileInfo.path();
        if(!PreferredDirs::checkDuplicateDir(preferredDirsNames, dirPath, true)) {
            return ;
        }
        preferredDirsNames.append(dirPath);
        updatePreferredDirs(preferredDirsNames, preferredDirs);
    }
}

void MainWindow::on_actionEditPreferredDirectories_triggered()
{
    if(configurePreferredDirs(this, preferredDirsNames)) {
        updatePreferredDirs(preferredDirsNames, preferredDirs);
    }
}

void MainWindow::updatePreferredDirs(QStringList &entries, QList<QAction*>actions)
{
    entries.sort();
    updateEntriesMenu(entries, actions);
    if(!Config::saveStringArray(Config::KEY_MAIN_PREFDIRS, preferredDirsNames)) {
        Utils::error(tr("Error registering preferred directories."));
    }
}
void MainWindow::loadPreferredDirsSettings()
{
    preferredDirsNames.clear();
    Config::loadStringArray(Config::KEY_MAIN_PREFDIRS, preferredDirsNames);
    preferredDirsNames.sort();
    updateEntriesMenu(preferredDirsNames, preferredDirs);
}

void MainWindow::on_actionValidate_triggered()
{
    ui.editor->onActionValidate();
}

void MainWindow::on_actionValidateNewFile_triggered()
{
    ui.editor->onActionValidateNewFile();
}

void MainWindow::on_actionValidateUsingDocumentReferences_triggered()
{
    ui.editor->validateUsingDocumentReferences();
}

void MainWindow::on_actionInsertSnippet_triggered()
{
    if(!isReadOnly()) {
        Regola* regola = getRegola();
        if(NULL == regola) {
            return;
        }
        Regola* newRegola = _snippetManager->chooseSnippets(data, this) ;
        if(NULL != newRegola) {
            ui.editor->insertSnippet(newRegola);
            delete newRegola;
        }
    }
}

void MainWindow::on_actionConfigureSnippets_triggered()
{
    _snippetManager->editSnippets(data, this);
}

void MainWindow::on_actionExecuteAutoTest_triggered()
{
    Test::executeTests();
}

void MainWindow::on_actionTransforminSnippet_triggered()
{
    if(!ui.editor->isActionMode()) {
        return ;
    }

    Element *element = getSelectedItem();
    if(NULL != element) {
        QDomDocument    document;
        if(element->generateDom(document, document)) {
            QString clipboardText = document.toString(4);
            _snippetManager->insertFragmentInSnippets(this, data, clipboardText) ;
        }
    }
}

void MainWindow::onPlugin()
{
    QAction *actionFile = qobject_cast<QAction*>(sender());
    if(NULL != actionFile) {
        QString pluginId = actionFile->data().toString();
        if(!pluginId .isEmpty()) {
            IQXmlEditPlugIn* plugin = data->plugins()[pluginId];
            Element *selectedOut = NULL ;
            if(NULL != plugin) {
                if(plugin->go(this, data, getRegola(), getEditor()->getSelectedItem(), &selectedOut)) {
                    if(NULL != selectedOut) {
                        getEditor()->setCurrentItem(selectedOut);
                    }
                }
            }
        }
    }
}

bool MainWindow::buildPluginsMenu(const char *method, QMenu *parent)
{
    bool isOk = true;
    pluginsCmds.clear();
    QMapIterator<QString, IQXmlEditPlugIn*> pluginsIterator(data->plugins());
    while(pluginsIterator.hasNext()) {
        pluginsIterator.next();
        QAction *action = new QAction(this);
        if(NULL != action) {
            pluginsCmds.append(action);
            connect(action, SIGNAL(triggered()), this, method);
            IQXmlEditPlugIn* plugin = pluginsIterator.value();
            action->setData(pluginsIterator.key());
            action->setText(plugin->name());
            action->setVisible(true);
            parent->addAction(action);
        } else {
            isOk = false ;
        }
    }
    return isOk ;
}

void MainWindow::on_actionDeleteBookmarked_triggered()
{
    ui.editor->onActionDeleteBookmarked();
}

void MainWindow::on_actionCleanBookmarkedContent_triggered()
{
    ui.editor->onActionCleanBookmarkedContent();
}


void MainWindow::setDisplayMode(const qxmledit::EDisplayMode value)
{
    ui.editor->setDisplayMode(value);
    navigationModeIndicator->setExploreMode(value);
}


//---------------------- load trigger

void MainWindow::on_actionXplore_triggered()
{
    if(!checkAbandonChanges()) {
        return ;
    }
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open File"),
                       QXmlEditData::sysFilePathForOperation(getRegola()->fileName()), Utils::getFileFilterForOpenFile());
    if(!filePath.isEmpty()) {
        loadFileXplore(filePath);
    }
}

void MainWindow::loadFileXplore(const QString &filePath)
{
    if(!filePath.isEmpty()) {
        doLoadFileXplore(filePath);
        updateRecentFilesMenu(filePath);
        updateWindowFilePath();
    } else {
        Utils::error(tr("File name empty. Unable to load it."));
    }
}

bool MainWindow::checkActivateModeXSL()
{
    if(!Utils::fileIsXSLT(getEditor()->getRegola())) {
        return false;
    }
    bool switchToXSLTMode = false;
    if(data->isAutoXSLTMode()) {
        switchToXSLTMode = true ;
    } else {
        if(data->isShowXSLTPanel()) {
            if(XSLTAutoModeDialog::showToUser(this, data)) {
                switchToXSLTMode = true ;
            }
        }
    }
    if(switchToXSLTMode) {
        ui.editor->setEditMode(XmlEditWidgetEditMode::XSLT);
        ui.actionXSLTEditMode->setChecked(true);
    }
    return switchToXSLTMode ;
}

bool MainWindow::checkActivateModeSCXML()
{
    if(!Utils::fileIsSCXML(getEditor()->getRegola())) {
        return false;
    }
    bool switchToSXCMLMode = false;
    if(data->isAutoSCXMLMode()) {
        switchToSXCMLMode = true ;
    } else {
        if(data->isShowSCXMLPanel()) {
            if(SCXMLAutoModeDialog::showToUser(this, data)) {
                switchToSXCMLMode = true ;
            }
        }
    }
    if(switchToSXCMLMode) {
        ui.editor->setEditMode(XmlEditWidgetEditMode::SCXML);
        ui.actionSCXMLEditMode->setChecked(true);
    }
    return switchToSXCMLMode ;
}

void MainWindow::activateModesOnNewFile()
{
    if(data->isFormattingInfoEnabled()) {
        getEditor()->getRegola()->readFormattingInfo();
    }
    if(Utils::fileIsXSLT(getEditor()->getRegola())) {
        if(checkActivateModeXSL()) {
            return ;
        }
    }
    checkActivateModeSCXML();
}


void MainWindow::showLoadFileError(const QString &filePath, const QString &errorMsg, const int errorLine, const int errorColumn)
{
    // decode error
    QString msg = Utils::decodeXMLError(errorMsg);
    QString error = tr("Unable to parse XML, file ='%1', at line:%3, column:%4, cause: '%2' ").arg(filePath).arg(msg).arg(errorLine).arg(errorColumn);
    Utils::error(error);
}


//---------------------- load data

//TODO: error checking
void MainWindow::doLoadFileXplore(const QString &filePath)
{
    ui.editor->doLoadFileXplore(filePath);
    statusBar()->showMessage(tr("Data loaded"), SHORT_TIMEOUT);
}

//TODO: error checking
void MainWindow::onNewMessage(const QString &newMessage)
{
    statusBar()->showMessage(newMessage, SHORT_TIMEOUT);
}

//TODO: error checking
void MainWindow::setDocument(QDomDocument &document, const QString &filePath, const bool isSetState)
{
    ui.editor->setDocument(document, filePath, isSetState);
    statusBar()->showMessage(tr("Data loaded"), SHORT_TIMEOUT);
    onReadOnlyStateChanged();
    onNewXSDSchemaForValidation("");
}

bool MainWindow::readData(XMLLoadStatus *status, QXmlStreamReader *reader, const QString &filePath, const bool isSetState, XMLLoadErrorHandler *errorHandler)
{
    bool result = ui.editor->readData(status, reader, filePath, isSetState, errorHandler);
    if(result) {
        if(status->areErrorsPresent()) {
            ui.editor->setReadOnly(true);
            statusBar()->showMessage(tr("Data loaded with errors."), SHORT_TIMEOUT);
        } else {
            statusBar()->showMessage(tr("Data loaded"), SHORT_TIMEOUT);
        }
        onReadOnlyStateChanged();
        onNewXSDSchemaForValidation("");
    } else {
        statusBar()->showMessage(tr("Unable to get data."), LONG_TIMEOUT);
    }
    return result;
}

void MainWindow::on_actionHideView_triggered()
{
    ui.editor->onActionHideView(ui.actionHideView->isChecked());
}

void MainWindow::on_actionSearchInFiles_triggered()
{
    searchInFiles(this);
}

void MainWindow::on_actionHelpOnQXmlEdit_triggered()
{
    data->showUserManual();
}

bool MainWindow::isValidXsd()
{
    Regola * regola = getRegola();
    if(NULL != regola) {
        return regola->isValidXsd();
    }
    return false;
}

void MainWindow::on_actionViewAsXsd_triggered()
{
    ui.editor->onActionViewAsXsd();
}

void MainWindow::viewAsXSD()
{
    ui.editor->onActionViewAsXsd();
}

Regola *MainWindow::getRegola()
{
    return ui.editor->getRegola();
}

SessionStateWidget *MainWindow::sessionStateWidget()
{
    return _sessionStateWidget ;
}

void MainWindow::schemaLoadComplete(const QString &newLabel)
{
    setSchemaLabel(newLabel);
    statusBar()->update();
}

void MainWindow::on_actionAllowedSchemaElements_triggered()
{
    ui.editor->onActionAllowedSchemaElements();
}

/** Substitute the element text
  */
void MainWindow::on_actionPasteAndSubstituteText_triggered()
{
    ui.editor->onActionPasteAndSubstituteText();
}


void MainWindow::on_actionNewUsingXMLSchema_triggered()
{
    MainWindow *theWindow = makeNewWindow();
    theWindow->newUsingXMLSchema();
}

void MainWindow::newUsingXMLSchema()
{
    if(!verifyAbandonChanges()) {
        return ;
    }
    bool isOk = false;
    QString schemaURL;
    schemaURL = chooseSchemaFile(this, isOk) ;
    if(isOk) {
        beforeLoadingNewData();
        ui.editor->onActionNewUsingXMLSchema(schemaURL);
    }
    removeAttributesFilter();
    updateWindowFilePath();
    clearExportFilePath();
}

bool MainWindow::verifyAbandonChanges()
{
    Regola *regola = getRegola();
    if(!regola->isEmpty(false) && regola->isModified()) {
        if(!uiDelegate->askYN(this, tr("Abandon changes?"))) {
            return false ;
        }
    }
    return true ;
}

bool MainWindow::checkAbandonChanges(const EWindowOpen useWindow, const QString &filePath)
{
    Regola *regola = getRegola();
    if(NULL == regola) {
        return true;
    }
    const bool forceSameWindow = (OpenUsingSameWindow == useWindow);
    const bool forceNewWindow = (OpenUsingNewWindow == useWindow);
    if((!_controller.isOpenInNewWindow() && !forceNewWindow) || forceSameWindow) {
        return verifyAbandonChanges();
    } else {
        if(_controller.isOpenInNewWindow() && !filePath.isEmpty()) {
            MainWindow *window = appData()->findWindowByPath(filePath);
            if(NULL != window) {
                return window->verifyAbandonChanges();
            }
        }
        return true ;
    }
}


QWidget *MainWindow::getMainWidget()
{
    return this ;
}

QString MainWindow::getAppTitle()
{
    return APP_TITLE ;
}


void MainWindow::on_actionTransformInComment_triggered()
{
    ui.editor->onActionTransformInComment();
}

void MainWindow::on_actionExtractElementsFromComment_triggered()
{
    ui.editor->onActionExtractElementsFromComment();
}

void MainWindow::autoLoadValidation()
{
    ui.editor->autoLoadValidation();
}

void MainWindow::setSchemaLabel(const QString &newLabel)
{
    labelSchema->setText(newLabel);
}

void MainWindow::setModeLabel()
{
    XmlEditWidgetEditMode::EditMode editMode = getEditor()->editMode();
    QString mode ;
    switch(editMode) {
    case XmlEditWidgetEditMode::XML:
        mode = tr("XML");
        break;
    case XmlEditWidgetEditMode::XSLT:
        mode = tr("XSLT");
        break;
    case XmlEditWidgetEditMode::SCXML:
        mode = tr("SCXML");
        break;
    default:
        mode = tr("unknown");
        break;
    }
    labelMode->setText(QString(tr("mode: %1")).arg(mode));
}

void MainWindow::onChangeEditorMode()
{
    updateEditMode();
    setModeLabel();
}

void MainWindow::on_actionExtractFragmentsFromFile_triggered()
{
    ExtractResults *results = new  ExtractResults();
    if(NULL == results) {
        Utils::errorOutOfMem(this);
        return ;
    }
    extractFragments(results, this, this);
    if(!(results->isError() || results->isAborted())) {
        if(results->numFragments() == 0) {
            Utils::message(tr("No fragments found"));
            delete results ;
            return ;
        }
        MainWindow * newWindow = makeNewWindow();
        newWindow->cleanExtractResults();
        newWindow->_extractResult = results ;
        newWindow->ui.editor->setNavigationDataAndEnable(1, results->numFragments());
        newWindow->showNavigationBox();
    } else {
        delete results ;
    }
}

void MainWindow::showNavigationBox()
{
    ui.editor->showNavigationBox();
}

void MainWindow::navigateToPage(const int page)
{
    /********************************** test code **************************
    Utils::todo(QString("Navigate to %1").arg(page));
    setWindowTitle(QString::number(page));
    ***********************************************************************/
    loadCurrentPage(page);
}

void MainWindow::loadCurrentPage(const int page)
{
    if(NULL != _extractResult) {
        StringOperationResult result;
        _extractResult->loadFragment(page, result);
        if(!result.isError()) {
            ui.editor->loadText(result.result(), false, true);
            // printf("xml is:'%s'\n", result.result().toAscii().data());
        } else {
            Utils::error(this, tr("Error loading data."));
        }
    }
}

void MainWindow::welcomeDialog()
{
    Regola *regola = getRegola() ;
    if(data->shouldShowWelcomeDialog()) {
        WelcomeDialog welcome(data->lastFiles(), this);
        welcome.exec();
        bool welcomeDialogEnabled = true ;
        switch(welcome.result()) {
        default:
            // no action;
            break;
        case WelcomeDialog::E_OPENSCHEMA:
            on_actionOpen_triggered();
            if(NULL != regola) {
                ui.editor->onActionViewAsXsd();
            }
            break;
        case WelcomeDialog::E_RECENTFILECHOOSE: {
            QString filePath = welcome.fileChoosen();
            if(!filePath.isEmpty()) {
                loadFile(filePath);
            }
        }
        break;
        case WelcomeDialog::E_SCANFILE:
            on_actionExtractFragmentsFromFile_triggered();
            break;
        case WelcomeDialog::E_CLOSE:
            break;
        case WelcomeDialog::E_MANAGESESSIONS:
            on_actionManageSessions_triggered();
            break;
        case WelcomeDialog::E_VIEWDATA:
            on_actionViewData_triggered();
            break;
        case WelcomeDialog::E_COMPARE:
            on_actionCompareFiles_triggered();
            break;
        }
        welcomeDialogEnabled = welcome.isShowNextTime();
        data->enableWelcomeDialog(welcomeDialogEnabled);
    }
}

void MainWindow::on_actionWelcomeDialog_triggered()
{
    welcomeDialog();
}

void MainWindow::triggersWelcomeDialog()
{
    ui.actionWelcomeDialog->trigger();
}

void MainWindow::error(const QString& message)
{
    _errorCount++;
    Utils::error(this, message);
}

void MainWindow::error(QWidget *widget, const QString& message)
{
    _errorCount++;
    Utils::error(widget, message);
}

void MainWindow::warning(const QString& message)
{
    Utils::warning(this, message);
}

void MainWindow::message(const QString& message)
{
    Utils::message(this, message);
}

bool MainWindow::askYN(const QString & message)
{
    return Utils::askYN(this, message);
}

bool MainWindow::askYN(QWidget *parent, const QString & message)
{
    return Utils::askYN(parent, message);
}

QString MainWindow::msgOutOfMem()
{
    return Utils::msgOutOfMem();
}

void MainWindow::errorNoSel(QWidget * parent)
{
    Utils::errorNoSel(parent);
    _errorCount++;
}

void MainWindow::errorOutOfMem(QWidget *parent)
{
    Utils::errorOutOfMem(parent);
    _errorCount++;
}

void MainWindow::resetErrorCount()
{
    _errorCount = 0;
}

int MainWindow::errorCount()
{
    return _errorCount;
}

void MainWindow::registerError()
{
    _errorCount++;
}


QTreeWidget *MainWindow::getMainTreeWidget()
{
    return ui.editor->getMainTreeWidget();
}

XmlEditWidget *MainWindow::getEditor()
{
    return ui.editor ;
}

void MainWindow::on_actionHideAllLeafChildren_triggered()
{
    ui.editor->onActionHideAllLeafChildren();
}

void MainWindow::on_actionHideLeafChildren_triggered()
{
    evaluateSingleItemLeaves(ui.editor->onActionHideLeafChildren());
}

void MainWindow::on_actionShowLeafChildren_triggered()
{
    evaluateSingleItemLeaves(ui.editor->onActionShowLeafChildren());
}

void MainWindow::on_actionShowAllLeafChildren_triggered()
{
    ui.editor->onActionShowAllLeafChildren();
}

void MainWindow::on_actionColumnView_triggered()
{
    Regola *regola = getRegola();
    if(NULL != regola) {
        ColumnarView::showModal(this, regola);
    }
}

Ui::MainWindow *MainWindow::getUI()
{
    return &ui;
}

void MainWindow::on_actionShowCurrentSessionPanel_triggered()
{
    if(data->sessionManager()->isEnabled()) {
        ui.sessionTree->setVisible(ui.actionShowCurrentSessionPanel->isChecked());
    }
}

void MainWindow::onSessionEnablingChanged()
{
    if(isSlave) {
        return ;
    }
    bool isEnabled = false ;
    if(data->sessionManager()->isEnabled()) {
        isEnabled = true ;
    }
    ui.actionShowCurrentSessionPanel->setVisible(isEnabled);
    ui.actionShowCurrentSessionPanel->setEnabled(isEnabled);
    if(!isEnabled) {
        ui.sessionTree->setVisible(false);
        ui.actionShowCurrentSessionPanel->setChecked(false);
    }
    ui.actionNewSession->setEnabled(isEnabled);
}

bool MainWindow::onSessionfileLoadRequest(const QString& filePath)
{
    if(!filePath.isEmpty()) {
        if(!checkAbandonChanges()) {
            return false ;
        }
        return loadFile(filePath);
    }
    return false ;
}

void MainWindow::onSessionFolderOpenRequest(const QString& path)
{
    openFileUsingDialog(path);
}

void MainWindow::on_actionNewSession_triggered()
{
    SessionManager *sessionManager = data->sessionManager();
    if(!sessionManager->isEnabled()) {
        return;
    }
    QString newSessionName = SessionRequester::askForNewSessionName(this, this);
    if(!newSessionName.isEmpty()) {
        if(sessionManager->state() != Session::NoSession) {
            sessionManager->closeSession();
        }
        if(NULL == sessionManager->newSession(newSessionName)) {
            Utils::error(this, tr("Error activating sessions."));
            return ;
        }
        data->setActiveSessionCode(sessionManager->currentSession()->id());
        data->setActiveSessionState(sessionManager->currentSession()->state());
    }
}

void MainWindow::on_actionPauseSession_triggered()
{
    SessionManager *sessionManager = data->sessionManager();
    if(!sessionManager->isEnabled()) {
        return;
    }
    sessionManager->pauseSession();
    data->setActiveSessionState(sessionManager->currentSession()->state());
}

void MainWindow::on_actionResumeSession_triggered()
{
    SessionManager *sessionManager = data->sessionManager();
    if(!sessionManager->isEnabled()) {
        return;
    }
    sessionManager->resumeSession();
    data->setActiveSessionState(sessionManager->state());
}

void MainWindow::on_actionCloseSession_triggered()
{
    SessionManager *sessionManager = data->sessionManager();
    if(!sessionManager->isEnabled() || sessionManager->isDefaultSession()) {
        return;
    }
    sessionManager->closeSession();
    data->setActiveSessionCode(0);
    data->setActiveSessionState(sessionManager->state());
    sessionManager->setDefaultSession(data->lastFiles());
}

void MainWindow::on_actionManageSessions_triggered()
{
    SessionManager *sessionManager = data->sessionManager();
    if(!sessionManager->isEnabled()) {
        return;
    }
    sessionManager->manageSessions(this, this, data->lastFiles());
}

void MainWindow::on_actionSessionDetails_triggered()
{
    SessionManager *sessionManager = data->sessionManager();
    if(!sessionManager->isEnabled()) {
        return;
    }
    sessionManager->editCurrentSession(this, this);
}

void MainWindow::onShowSessionManagement()
{
    ui.actionManageSessions->trigger();
}

void MainWindow::on_actionUndo_triggered()
{
    ui.editor->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    ui.editor->redo();
}

void MainWindow::onUndoStateUpdated(const bool isUndo, const bool isRedo)
{
    updateUndoState(isUndo, isRedo);
}

void MainWindow::updateUndoState(const bool isUndo, const bool isRedo)
{
    ui.actionUndo->setEnabled(isUndo) ;
    ui.actionRedo->setEnabled(isRedo) ;
}

void MainWindow::setUIDelegate(UIDelegate* newDelegate)
{
    uiDelegate = newDelegate ;
    if(NULL == uiDelegate) {
        uiDelegate = this ;
    }
    getEditor()->setUIDelegate(uiDelegate);
}

void MainWindow::on_actionNewWindow_triggered()
{
    makeNewWindow();
}

void MainWindow::reposFrame()
{
    const QRect shortcutFrame = data->keyboardInfoGeometry();
    const QRect geom = geometry();
    const QRect intersected = geom.intersected(shortcutFrame);
    if(!intersected.isEmpty()) {
        const int value = geom.height() - intersected.height() ;
        if(value > 0) {
            setGeometry(geom.left(), geom.top(), geom.width(), value);
        }
    }
}

MainWindow *MainWindow::makeNewWindow()
{
#ifndef QXMLEDIT_TEST
    const bool show = true ;
#else
    const bool show = false ;
#endif
    MainWindow *newWindow = MainWindow::newDynamicTopLevelNewWindow(data, true, show);
    return newWindow;
}

MainWindow *MainWindow::newDynamicTopLevelNewWindow(ApplicationData *appData, const bool isRepos, const bool isShow)
{
    MainWindow *newWindow = new MainWindow(true, false, appData);
    if(NULL != newWindow) {
        if(isRepos) {
            newWindow->reposFrame();
        }
        if(isShow) {
            newWindow->show();
        }
    } else {
        Utils::error(tr("Error opening a new window."));
    }
    return newWindow;
}

void MainWindow::on_actionViewData_triggered()
{
    QString fileName = askFileNameToOpen(getRegola()->fileName());
    DataVisualization::viewData(data, this, this, fileName);
}

void MainWindow::loadVisFile(const QString &fileName)
{
    DataVisualization::viewData(data, this, this, fileName);
}

QLabel *MainWindow::readOnlyStatusLabel()
{
    return _labelReadOnly;
}

void MainWindow::setSnippetManager(SnippetManager *newSnippetManager)
{
    if(NULL != _snippetManager) {
        delete _snippetManager;
    }
    // take ownership
    _snippetManager = newSnippetManager;
}

void MainWindow::on_actionBase64Tools_triggered()
{
    data->uiServices()->doBase64Dialog();
}

void MainWindow::on_actionEncodingTools_triggered()
{
    data->uiServices()->doEncodingDialog(data);
}

void MainWindow::on_actionCompareXSD_triggered()
{
    ui.editor->onActionCompareXSD(getRegola()->fileName());
}

void MainWindow::on_actionExportAttributesSiblingsAsCSVClipboard_triggered()
{
    ui.editor->onActionExportSiblingsAttributesAsCSVClipboard();
}

void MainWindow::removeAttributesFilter()
{
    if(NULL != _attrFilter) {
        delete _attrFilter ;
        _attrFilter = NULL ;
    }
}

void MainWindow::on_actionVisAttrApplyTemporaryProfile_triggered()
{
    AttributeFilter * filter = AttributeFilterDialog::display(this, ui.editor->getSelectedItem());
    if(NULL != filter) {
        filter->setTemporary(true);
        setAttributeProfile(filter);
    }
}

QString MainWindow::askNameForCurrentAttributeFilter()
{
    QString result = "" ;
    bool repeat = true ;
    while(repeat) {
        repeat = false ;
        bool isOk = false;
        result = QInputDialog::getText(this, tr("Save the Temporary Profile"), tr("Profile name:"), QLineEdit::Normal, "", &isOk);
        if(!isOk) {
            return "" ;
        }
        if(result.isEmpty()) {
            repeat = true ;
        }
    }
    return result;
}

void MainWindow::on_actionVisAttrSaveCurrenteProfile_triggered()
{
    if((NULL != _attrFilter) && _attrFilter->isTemporary()) {
        QString name = askNameForCurrentAttributeFilter();
        if(data->attributeFilterManagement()->insertFilterAsProfile(name, _attrFilter)) {
            _attrFilter->setTemporary(false);
            onComputeSelectionState();
            Utils::message(this, tr("Filter saved.")) ;
        } else {
            Utils::error(this, tr("Error saving the filter."));
        }
    }
}


void MainWindow::on_actionVisAttrManageProfiles_triggered()
{
    AttributeFilter *newFilter = data->attributeFilterManagement()->showProfiles(this, this);
    if(NULL != newFilter) {
        setAttributeProfile(newFilter);
    }
}

void MainWindow::on_actionVisAttrRemoveCurrentProfile_triggered()
{
    if(NULL != _attrFilter) {
        setAttributeProfile(NULL);
    }
}

void MainWindow::setAttributeProfile(AttributeFilter *filter)
{
    ui.editor->getPaintInfo()->setAttributeFilter(filter);
    removeAttributesFilter();
    _attrFilter = filter;
    ui.editor->getRegola()->redisplay();
    _attributeFilterWidget->setState(NULL != filter);
    onComputeSelectionState();
}

void MainWindow::on_actionCompareFiles_triggered()
{
    Regola dummy;
    CompareBridge::doCompare(this, false, &dummy, this, data->lastFiles());
}

void MainWindow::on_actionSpring_and_Forces_triggered()
{
    QList<TagNode*> dataList;
    NodesRelationsDialog dialog(true, dataList, NULL);
    dialog.exec();
}

void MainWindow::on_actionViewGraphic_triggered()
{
    ui.editor->viewNodes();
}

void MainWindow::on_actionCopyAttributes_triggered()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        CopyAttributesDialog::showDialog(element, data->copyAttributesManager(), this);
        onComputeSelectionState();
    }
}

void MainWindow::on_actionPasteAttributes_triggered()
{
    Element *element = getSelectedItem();
    if(NULL != element) {
        PasteAttributesDialog::showDialog(element, data->copyAttributesManager(), this);
        onComputeSelectionState();
    }
}

void MainWindow::on_actionPasteLastAttributes_triggered()
{
    ui.editor->onActionPasteLastAttributes();
}

void MainWindow::on_actionCopyElementPathClipboard_triggered()
{
    ui.editor->actionCopyElementPathToClipboard();
}


void MainWindow::on_actionAutoComplete_triggered()
{
    QWidget *activeWidget = qApp->focusWidget();
    if(NULL != activeWidget) {
        ICompleter *completer = qobject_cast<ICompleter*>(activeWidget);
        if(NULL == completer) {
            QString widgetName = activeWidget->metaObject()->className();
            if(widgetName == "QComboBox") {
                QComboBox *combo = qobject_cast<QComboBox*>(activeWidget);
                if((NULL != combo) && (NULL != combo->lineEdit())) {
                    completer = qobject_cast<ICompleter*>(combo->lineEdit());
                }
            }
        }
        if(NULL != completer) {
            completer->autocomplete();
        }
    }
}


void MainWindow::updateEditMode()
{
    bool isXMLMode = true;
    {
        bool xsltMode = false;
        if(ui.editor->editMode() == XmlEditWidgetEditMode::XSLT) {
            xsltMode = true ;
            isXMLMode = false;
        }
        ui.actionXSLTEditMode->setChecked(xsltMode);
        ui.actionScanXMLTagsAndNamesXSLTAutocompletion->setEnabled(xsltMode);
        ui.actionShowXSLNavigator->setChecked(xsltMode);
        on_actionShowXSLNavigator_triggered();
    }
    {
        bool scxmlMode = false;
        if(ui.editor->editMode() == XmlEditWidgetEditMode::SCXML) {
            scxmlMode = true ;
            isXMLMode = false;
        }
        ui.actionSCXMLEditMode->setChecked(scxmlMode);
        ui.actionShowSCXMLNavigator->setChecked(scxmlMode);
        on_actionShowSCXMLNavigator_triggered();
    }
    ui.actionXMLEditMode->setChecked(isXMLMode);
}

void MainWindow::on_actionXMLEditMode_triggered()
{
    ui.editor->setEditMode(XmlEditWidgetEditMode::XML);
}

void MainWindow::on_actionXSLTEditMode_triggered()
{
    ui.editor->setEditMode(XmlEditWidgetEditMode::XSLT);
}

void MainWindow::on_actionSCXMLEditMode_triggered()
{
    ui.editor->setEditMode(XmlEditWidgetEditMode::SCXML);
}

void MainWindow::on_actionNewXSLTSheet_triggered()
{
    _controller.createDocumentFromResources(":/templates/templateXsl");
}

void MainWindow::on_actionNewXSDDocument_triggered()
{
    _controller.createDocumentFromResources(":/templates/templateXsd");
}

void MainWindow::on_actionNewXSLFODocument_triggered()
{
    _controller.createDocumentFromResources(":/templates/templateFo");
}

void MainWindow::on_actionNewMavenPOM_triggered()
{
    _controller.createDocumentFromResources(":/templates/templatePOM");
}

void MainWindow::on_actionNewSCXMLDocument_triggered()
{
    _controller.createDocumentFromResources(":/templates/templateSCXML");
}

void MainWindow::on_actionNewFromSnippet_triggered()
{
    Regola* newRegola = _snippetManager->chooseSnippets(data, this) ;
    if(NULL != newRegola) {
        _controller.createDocumentFromSnippet(newRegola);
    }
}

void MainWindow::createDocumentFromSnippet(Regola* newRegola)
{
    beforeLoadingNewData();
    ui.editor->assignRegola(newRegola);
    markAsAllEdited();
    removeAttributesFilter();
    activateModesOnNewFile();
}

void MainWindow::markAsAllEdited()
{
    Regola *regola = getRegola();
    if(NULL != regola) {
        regola->markEdited();
        regola->setModified(true);
    }
}

bool MainWindow::createDocumentFromResources(const QString &path)
{
    return _controller.createDocumentFromResources(path);
}

void MainWindow::on_actionEditXSLTitem_triggered()
{
    ui.editor->editXSLTElement();
}

void MainWindow::on_actionScanXMLTagsAndNamesXSLTAutocompletion_triggered()
{
    ui.editor->scanXMLTagsAndNamesXSLTAutocompletion();
}

void MainWindow::on_actionShowXSLNavigator_triggered()
{
    ui.editor->showXSLNavigator(ui.actionShowXSLNavigator->isChecked());
}

void MainWindow::on_actionSpecificProperties_triggered()
{
    ui.editor->specificProperties();
}

void MainWindow::on_actionPasteAsSibling_triggered()
{
    ui.editor->pasteAsSibling();
}

void MainWindow::onClipboardDataChanged(bool isData)
{
    setClipBoardActionsState(isData);
}

void MainWindow::on_actionTipsOnVisualAppearance_triggered()
{
    TipsAboutVisualAppearance tips(this);
    tips.setModal(true);
    tips.exec();
}

void MainWindow::on_actionBinaryViewer_triggered()
{
    BinaryViewerDialog viewer(data->lastFiles(), this);
    viewer.exec();
}

bool MainWindow::isReadOnly()
{
    return getEditor()->isReadOnly();
}

QString MainWindow::readOnlyStateTextImage(const bool isReadOnly)
{
    QString labelImage ;
    if(isReadOnly) {
        labelImage = ":/status/locked";
    } else {
        labelImage = ":/status/unlocked";
    }
    QString txt = QString("<html><body><div style='vertical-align:middle'><img src=\"%1\"/></div></body></html>").arg(labelImage);
    return txt;
}

QString MainWindow::readOnlyStateText(const bool isReadOnly)
{
    QString labelText ;
    if(isReadOnly) {
        labelText = tr("Read Only");
    } else {
        labelText = tr("Modifiable");
    }
    return labelText;
}

QString MainWindow::readOnlyTooltip(const bool isReadOnly)
{
    if(isReadOnly) {
        return tr("Read Only. Reactivable using Edit menu or by double click.");
    } else {
        return QString("%1. %2").arg(tr("Modifiable")).arg(tr("Double click to change."));
    }
}

void MainWindow::onReadOnlyStateChanged()
{
    bool isReadOnly = ui.editor->isReadOnly();
    QString txt = readOnlyStateText(isReadOnly);
    QString txtImg = readOnlyStateTextImage(isReadOnly);
    _labelReadOnly->setText(txt);
    _labelReadOnly->setToolTip(readOnlyTooltip(isReadOnly));
    _labelReadOnly->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    _labelReadOnlyImg->setText(txtImg);
    _labelReadOnlyImg->setToolTip(readOnlyTooltip(isReadOnly));
    _labelReadOnlyImg->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
    ui.actionReadOnly->setChecked(ui.editor->isReadOnly());
}

void MainWindow::on_actionReadOnly_toggled(bool /*checked*/)
{
    getEditor()->setReadOnly(ui.actionReadOnly->isChecked());
    onReadOnlyStateChanged();
}

void MainWindow::onReadOnlyGo()
{
    ui.actionReadOnly->setChecked(!ui.actionReadOnly->isChecked());
}

void MainWindow::on_actionInsertMetadata_triggered()
{
    ui.editor->onActionInsertMetadata();
}

void MainWindow::on_actionInsertCompleteMetadata_triggered()
{
    ui.editor->onActionInsertCompleteMetadata();
}

void MainWindow::on_actionEditMetadata_triggered()
{
    ui.editor->onActionEditMetadata();
}

void MainWindow::on_actionExportAsJavaJavascriptSource_triggered()
{
    if(NULL == getRegola()) {
        return ;
    }
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Data as Java/Javascript"),
                       QXmlEditData::sysFilePathForOperation(getRegola()->fileName()), Utils::getFileFilterForJavaSourceOpenFile());

    if(!filePath.isEmpty()) {
        getRegola()->writeAsJavaStringInFile(filePath);
    }
}

void MainWindow::on_actionExportAsCSource_triggered()
{
    if(NULL == getRegola()) {
        return ;
    }
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Data as Java/Javascript"),
                       QXmlEditData::sysFilePathForOperation(getRegola()->fileName()), Utils::getFileFilterForJavaSourceOpenFile());

    if(!filePath.isEmpty()) {
        getRegola()->writeAsCStringInFile(filePath);
    }
}

void MainWindow::updateWindowFilePath()
{
    setWindowFilePath(getRegola()->fileName());
}

void MainWindow::on_actionInsertProlog_triggered()
{
    if(!ui.editor->isReadOnly()) {
        ui.editor->insertProlog();
    }
}

void MainWindow::on_actionCopyAllToClipboard_triggered()
{
    if(NULL != getRegola()) {
        setEnabled(false);
        QString str = getRegola()->getAsText();
        QApplication::clipboard()->setText(str);
        setEnabled(true);
    }
}

void MainWindow::onEditorEncodingChanged(const QString & newEncoding)
{
    if(NULL != _encodingGroup) {
        QList<QAction *> encActions = _encodingGroup->actions() ;
        QString actionName = QString(ENC_PREFIX).append(newEncoding);
        foreach(QAction * a, encActions) {
            if(a->objectName() == actionName) {
                a->setChecked(true);
                break;
            }
        }
    }
}

void MainWindow::onEncodingChanged(QAction* action)
{
    if(isReadOnly()) {
        return ;
    }
    if(NULL != action) {
        if(!isReadOnly()) {
            int mib = action->data().toInt();
            QTextCodec *codec = QTextCodec::codecForMib(mib) ;
            if(NULL != codec) {
                if(!getEditor()->setEncoding(codec->name())) {
                    QAction *action = menuBar()->findChild<QAction*>(QString(ENC_PREFIX) + getRegola()->encoding());
                    if(NULL != action) {
                        action->setChecked(true);
                    }
                }
            } else {
                uiDelegate->error(this, tr("No codec for the required encoding"));
            }
        }
    }
}


void MainWindow::onNewXSDSchemaForValidation(const QString &newUrl)
{
    if(newUrl.isEmpty()) {
        ui.actionValidate->setToolTip(tr("Validate using internal XML Schema references."));
    } else {
        ui.actionValidate->setToolTip(tr("Validate using XML Schema:'%1'").arg(newUrl));
    }
}

void MainWindow::onSchemaValidationError(const QString &message, Element * /*element*/)
{
    statusBar()->showMessage(message);
    appData()->notifier()->notify(this, message);
}

void MainWindow::onIndentationChanged(const bool indentationEnabled, const int newIndent)
{
    if(!indentationEnabled) {
        _labelIndentation->setText(tr("I-"));
        _labelIndentation->setToolTip(tr("No Indentation"));
    } else {
        _labelIndentation->setText(tr("I%1").arg(newIndent));
        _labelIndentation->setToolTip(tr("Indentation: %n space(s)", "", newIndent));
    }
}

void MainWindow::on_actionCodePages_triggered()
{
    ui.editor->showCodePages(this);
}

void MainWindow::on_actionInsertChildContainer_triggered()
{
    ui.editor->onInsertChildContainer();
}

void MainWindow::on_actionInsertParent_triggered()
{
    ui.editor->onInsertParent();
}

void MainWindow::on_actionRemoveParent_triggered()
{
    ui.editor->onRemoveParent();
}

void MainWindow::on_actionXSDInsertElement_triggered()
{
    ui.editor->onXSDInsertElement();
}

void MainWindow::on_actionXSDAppendElement_triggered()
{
    ui.editor->onXSDAppendElement();
}

void MainWindow::on_actionXSDInsertType_triggered()
{
    ui.editor->onXSDInsertType();
}

void MainWindow::on_actionXSDAppendType_triggered()
{
    ui.editor->onXSDAppendType();
}

void MainWindow::on_actionXSDInsertAttribute_triggered()
{
    ui.editor->onXSDInsertAttribute();
}

void MainWindow::on_actionXSDAppendAttribute_triggered()
{
    ui.editor->onXSDAppendAttribute();
}

void MainWindow::on_actionXSDModifyType_triggered()
{
    ui.editor->onXSDModifyType();
}

void MainWindow::on_actionAnonymize_triggered()
{
    _controller.onAnonymize();
}

void MainWindow::on_actionQuickReferenceElements_triggered()
{
    XSDElementReferenceDialog dlg(this);
    dlg.setModal(true);
    dlg.exec();
}

void MainWindow::onAnonymize(AnonContext *context)
{
    _controller.anonymize(context);
}

void MainWindow::onAnonymize(AnonAlg *alg)
{
    _controller.anonymize(alg);
}

void MainWindow::on_actionAnonymizeFile_triggered()
{
    _controller.onAnonymizeFile();
}

void MainWindow::on_actionEditXSDAnnotation_triggered()
{
    ui.editor->onEditXSDAnnotation();
}

void MainWindow::on_actionEditEnumerationFacet_triggered()
{
    ui.editor->onEditXSDFacet();
}

void MainWindow::on_actionFindNext_triggered()
{
    ui.editor->onFindNext();
}

void MainWindow::on_actionFindPrevious_triggered()
{
    ui.editor->onFindPrevious();
}

void MainWindow::on_actionSetIndent_triggered()
{
    ui.editor->onSetIndent();
}

void MainWindow::on_actionInfo_triggered()
{
    _controller.actionInfo();
}

void MainWindow::on_actionShowMainButtons_triggered()
{
    onShowHideMainButtons();
    Config::saveBool(Config::KEY_MAIN_SHOWBUTTONS, ui.actionShowMainButtons->isChecked());
}

void MainWindow::onShowHideMainButtons()
{
    bool currentSate = ui.actionShowMainButtons->isChecked();
    XmlEditWidget *editor = ui.editor;
    editor->setEncodingLabelVisible(currentSate);
    editor->setDocTypeVisible(currentSate);
    editor->showButtons(currentSate);
}

void MainWindow::on_actionShowStatusBar_triggered()
{
    onShowHideStatusBar();
    Config::saveBool(Config::KEY_MAIN_SHOWSTATUSBAR, ui.actionShowStatusBar->isChecked());
}

void MainWindow::onShowHideStatusBar()
{
    bool currentSate = ui.actionShowStatusBar->isChecked();
    statusBar()->setVisible(currentSate);
}

void MainWindow::on_actionRemoveNilAttribute_triggered()
{
    ui.editor->removeNilAttribute();
}

void MainWindow::on_actionInsertNilAttribute_triggered()
{
    ui.editor->insertNilAttribute();
}

void MainWindow::on_actionRemoveXSITypeAttribute_triggered()
{
    ui.editor->removeXSITypeAttribute();
}

void MainWindow::on_actionInsertXSITypeAttribute_triggered()
{
    if(!isReadOnly()) {
        QString newType = _controller.askNewXSIType(getEditor()->getSelectedItem());
        if(!newType.isEmpty()) {
            insertXSITypeAttribute(newType);
        }
    }
}

void MainWindow::insertXSITypeAttribute(const QString &newValue)
{
    ui.editor->insertXSITypeAttribute(newValue);
}


void MainWindow::on_actionInsertXmlSchemaReferences_triggered()
{
    if(!ui.editor->isReadOnly()) {
        ui.editor->insertXmlSchemaReferences();
    }
}

void MainWindow::on_actionOpenSameWindow_triggered()
{
    // reverse settings behavior
    openFileUsingDialog(getRegola()->fileName(),
                        _controller.isOpenInNewWindow() ? OpenUsingSameWindow : OpenUsingNewWindow);
}

MainWndController *MainWindow::controller()
{
    return &_controller ;
}

void MainWindow::on_actionFillSerie_triggered()
{
    ui.editor->actionFillSerie();
}

void MainWindow::on_actionCloneElements_triggered()
{
    if(!isReadOnly()) {
        controller()->cloneReplica();
    }
}

void MainWindow::on_actionClose_triggered()
{
    close();
}

void MainWindow::on_actionExportElementToFile_triggered()
{
    Element *selection = getSelectedItem();
    Regola * regola = getRegola();
    if((NULL == regola) || (NULL == selection)
            || ((NULL != selection) && !selection->isElement())) {
        error(tr("No data to save."));
        return ;
    }
    QString newFilePath = askFileNameForSaving(getExportPath());
    if(newFilePath.isEmpty()) {
        return ;
    }
    Regola::EExportOptions options = askExportOption();
    if(!regola->exportElement(newFilePath, options, selection)) {
        error(tr("Error exporting data."));
        return ;
    }
    statusBar()->showMessage(tr("Data exported"), SHORT_TIMEOUT);
    updateExportFilePath(newFilePath);
}

void MainWindow::updateExportFilePath(const QString &newPath)
{
    _exportPath = newPath;
}

void MainWindow::clearExportFilePath()
{
    _exportPath = "" ;
}

QString MainWindow::getExportPath()
{
    if(!_exportPath.isEmpty()) {
        return _exportPath ;
    }
    return getRegola()->fileName();
}

Regola::EExportOptions MainWindow::askExportOption()
{
    ExportOptionsDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted) {
        return dialog.options();
    }
    return Regola::ExportOptionNone ;
}

void MainWindow::on_actionSortAttributes_triggered()
{
    if(!isReadOnly()) {
        getEditor()->sortAttributes();
    }
}

void MainWindow::on_actionSortAttributesElement_triggered()
{
    Element *element = getSelectedItem();
    if(!isReadOnly() && (NULL != element)) {
        getEditor()->sortAttributesElement(element, false);
    }
}

void MainWindow::on_actionSortAttributesElementRecursive_triggered()
{
    Element *element = getSelectedItem();
    if(!isReadOnly() && (NULL != element)) {
        getEditor()->sortAttributesElement(element, true);
    }
}

void MainWindow::on_actionRemoveAllSiblings_triggered()
{
    Element *element = getSelectedItem();
    if(!isReadOnly() && (NULL != element)) {
        getEditor()->deleteSiblings(RegolaDeleteSiblings::DeleteAllSiblings, element);
    }
}

void MainWindow::on_actionRemoveAllSiblingsAfter_triggered()
{
    Element *element = getSelectedItem();
    if(!isReadOnly() && (NULL != element)) {
        getEditor()->deleteSiblings(RegolaDeleteSiblings::DeleteAllSiblingsAfter, element);
    }
}

void MainWindow::on_actionRemoveAllSiblingsBefore_triggered()
{
    Element *element = getSelectedItem();
    if(!isReadOnly() && (NULL != element)) {
        getEditor()->deleteSiblings(RegolaDeleteSiblings::DeleteAllSiblingsBefore, element);
    }
}

QString MainWindow::askFileNameToOpen(const QString &startFolder)
{
    return Utils::askFileNameToOpen(this, startFolder);
}

void MainWindow::taskChooseDetail()
{
    data->chooseVisualDetail();
}

void MainWindow::on_actionTaskDisplayDetail_triggered()
{
    taskChooseDetail();
}

void MainWindow::on_actionHelpSetEditorDetail_triggered()
{
    taskChooseDetail();
}

/**
 * @brief MainWindow::checkForSaveEncoding
 * if the 8 bit encoding triggers a problem in Qt ask the user
 * if DOM Should be used, warning about reduced functionality
 * asking one time only
 */
void MainWindow::checkForSaveEncoding()
{
    Regola *regola = getRegola();
    if(!regola->isForceDOM()) {
        if(!regola->isEncodingCompatibleWithStream()) {
            if(uiDelegate->askYN(tr("QXmlEdit detected that current encoding can lead to a malformed file.\nDo you want to save it using DOM method?\nWarning: DOM does not support attribute alignment and DTD management."))) {
                regola->setForceDOM(true);
            }
        } // if honored
    } // if stream and not dom forced
} // checkForSaveEncoding()

void MainWindow::on_actionRemovePrefix_triggered()
{
    if(!isReadOnly()) {
        _controller.removePrefix();
    }
}

void MainWindow::on_actionRemoveNamespace_triggered()
{
    if(!isReadOnly()) {
        _controller.removeNamespace();
    }
}

void MainWindow::on_actionSetPrefix_triggered()
{
    if(!isReadOnly()) {
        _controller.setPrefix();
    }
}

void MainWindow::on_actionSetNamespace_triggered()
{
    if(!isReadOnly()) {
        _controller.setNamespace();
    }
}

void MainWindow::on_actionReplacePrefix_triggered()
{
    if(!isReadOnly()) {
        _controller.replacePrefix();
    }
}

void MainWindow::on_actionReplaceNamespace_triggered()
{
    if(!isReadOnly()) {
        _controller.replaceNamespace();
    }
}

void MainWindow::on_actionNormalizeNamespace_triggered()
{
    if(!isReadOnly()) {
        _controller.normalizeNamespace();
    }
}

void MainWindow::beforeLoadingNewData()
{
    if(!_controller.isOpenInNewWindow()) {
        ui.loadWarningWidget->setVisible(false);
    }
}

void MainWindow::on_actionInsertSpecial_triggered()
{
    if(!isReadOnly()) {
        getEditor()->insertSpecial();
    }
}

void MainWindow::on_actionAppendSpecial_triggered()
{
    if(!isReadOnly()) {
        getEditor()->appendSpecial();
    }
}

void MainWindow::on_actionShowSCXMLNavigator_triggered()
{
    getEditor()->showSCXMLNavigator(ui.actionShowSCXMLNavigator->isChecked());
}

void MainWindow::on_actionValidateSCXML_triggered()
{
#if QT_VERSION < QT_VERSION_CHECK(5,7,0)
    Utils::error(this, tr("SCXML validation is available only with Qt starting from 5.7."));
#else
    ui.messagePanel->setVisible(false);
    _controller.checkSCXML();
#endif
}

void MainWindow::onSourceNavigateTo(QList<int> path)
{
    if(NULL != getRegola()) {
        Element *selection = getRegola()->findElementByArray(path);
        getEditor()->selectAndShowItem(selection);
    }
}

void MainWindow::onMessagesTabCloseRequested(int)
{
    ui.messagePanel->setVisible(false);
}

SourceRelatedMessages *MainWindow::sourceRelatedMessages()
{
    return _scxmlValidationErrors;
}

void MainWindow::on_actionCopyElementOnly_triggered()
{
    ui.editor->onActionCopyElementOnly();
}

void MainWindow::on_actionXSLExecutor_triggered()
{
    XSLTExecDialog::execDialog(this, data);
}

void MainWindow::on_actionExecuteXSLTAsSheet_triggered()
{
    XSLTExecDialog::execDialog(this, data, NULL, getEditor());
}

void MainWindow::on_actionExecuteXSLTAsSource_triggered()
{
    XSLTExecDialog::execDialog(this, data, getEditor(), NULL);
}

void MainWindow::on_actionOpenSiblingsAtTheSameLevel_triggered()
{
    ui.editor->openSiblingsSameLevel();
}

void MainWindow::on_actionShowPrincipalShortcuts_triggered()
{
    ShortcutsDialog::display(this);
}

void MainWindow::on_actionAddFormattingInfo_triggered()
{
    ui.editor->addFormattingInfo();
}

void MainWindow::on_actionRemoveFormattingInfo_triggered()
{
    ui.editor->removeFormattingInfo();
}

void MainWindow::on_actionPresetApacheFOP_triggered()
{
    ui.editor->presetFormatting(XMLIndentationSettings::PresetApacheFOP);
}

void MainWindow::on_actionPresetNoIndentation_triggered()
{
    ui.editor->presetFormatting(XMLIndentationSettings::PresetNoIndent);
}

void MainWindow::on_actionPresetIndentOneAttributePerLine_triggered()
{
    ui.editor->presetFormatting(XMLIndentationSettings::Preset2SpacesOneAttributePerLine);
}

void MainWindow::onEditorElementDoubleClicked(const uint /*times*/)
{
    const bool isAlreadyOpen = areInfoPanelsVisible();
    if(QXMLEditEnableTips && !isAlreadyOpen && data->evaluateConditionForShowShortcuts()) {
        if(NULL == _infoOnKeyboardShortcuts) {
            _infoOnKeyboardShortcuts = new InfoOnKeyboardShortcuts(this);
            ui.verticalLayoutCentralWidget->insertWidget(0, _infoOnKeyboardShortcuts);
            connect(_infoOnKeyboardShortcuts, SIGNAL(requestDismiss()), this, SLOT(onInfoKeyboardDismiss()));
            connect(_infoOnKeyboardShortcuts, SIGNAL(requestOpenShortcutsPanel()), this, SLOT(onInfoKeyboardRequestOpenShortcutsPanel()));
            data->setShortcutUsedDialogShown();
        }
        _infoOnKeyboardShortcuts->show();
    }
}

void MainWindow::onInfoKeyboardDismiss()
{
    dismissInfoOnKeyboard();
}

void MainWindow::onInfoKeyboardRequestOpenShortcutsPanel()
{
    data->showEditingShortcuts(this);
    dismissInfoOnKeyboard();
}

void MainWindow::dismissInfoOnKeyboard()
{
    if(NULL != _infoOnKeyboardShortcuts) {
        disconnect(_infoOnKeyboardShortcuts, SIGNAL(requestDismiss()), this, SLOT(onInfoKeyboardDismiss()));
        disconnect(_infoOnKeyboardShortcuts, SIGNAL(requestOpenShortcutsPanel()), this, SLOT(onInfoKeyboardRequestOpenShortcutsPanel()));
        ui.verticalLayoutCentralWidget->removeWidget(_infoOnKeyboardShortcuts);
        _infoOnKeyboardShortcuts->deleteLater();
        _infoOnKeyboardShortcuts = NULL ;
    }
}

/**
 * @brief MainWindow::onEditorEditElementEvent
 * activation when (and):
 * 1- the configuration has not been modified
 * 2- the dialog has not been displayed (even as task)
 * 3- The form panel has been used for 10 times without using the text panel
 * @param editElementAsFormUsageCount
 * @param editElementAsTextUsageCount
 */
bool MainWindow::baseEvaluateIfShowEditingTypeDialog(const bool configurationModified, const bool configurationDialogShown, const uint editElementAsFormUsageCount, const uint editElementAsTextUsageCount)
{
    const bool accessValues = (editElementAsFormUsageCount >= MaxTimesElementEditedWithoutText) && (0 == editElementAsTextUsageCount);
    const bool result = !configurationModified && !configurationDialogShown && accessValues;
    return result;
}

bool MainWindow::evaluateIfShowEditingTypeDialog(const uint editElementAsFormUsageCount, const uint editElementAsTextUsageCount)
{
    const bool configurationModified = data->isBaseEditModeFormModified();
    const bool configurationDialogShown = data->isEditTypeDialogShown();
    const bool result = baseEvaluateIfShowEditingTypeDialog(configurationModified, configurationDialogShown, editElementAsFormUsageCount, editElementAsTextUsageCount);
    return result;
}

void MainWindow::onEditorEditElementEvent(const uint editElementAsFormUsageCount, const uint editElementAsTextUsageCount)
{
    const bool isAlreadyOpen = areInfoPanelsVisible();
    if(QXMLEditEnableTips && !isAlreadyOpen && evaluateIfShowEditingTypeDialog(editElementAsFormUsageCount, editElementAsTextUsageCount)) {
        if(NULL == _infoOnEditMode) {
            _infoOnEditMode = new InfoOnEditMode(this);
            ui.verticalLayoutCentralWidget->insertWidget(0, _infoOnEditMode);
            connect(_infoOnEditMode, SIGNAL(requestDismiss()), this, SLOT(onInfoEditTypesDismiss()));
            connect(_infoOnEditMode, SIGNAL(requestOpenPanel()), this, SLOT(onInfoEditTypesOpenShortcutsPanel()));
            data->setEditTypeDialogShown();
        }
        _infoOnEditMode->show();
    }
}

void MainWindow::onInfoEditTypesDismiss()
{
    dismissInfoEditTypes();
}

void MainWindow::onInfoEditTypesOpenShortcutsPanel()
{
    showEditingTypeDialog();
    dismissInfoEditTypes();
}

void MainWindow::dismissInfoEditTypes()
{
    if(NULL != _infoOnEditMode) {
        disconnect(_infoOnEditMode, SIGNAL(requestDismiss()), this, SLOT(onInfoEditTypeDismiss()));
        disconnect(_infoOnEditMode, SIGNAL(requestOpenPanel()), this, SLOT(onInfoEditTypesOpenShortcutsPanel()));
        ui.verticalLayoutCentralWidget->removeWidget(_infoOnEditMode);
        _infoOnEditMode->deleteLater();
        _infoOnEditMode = NULL ;
    }
}

bool MainWindow::areInfoPanelsVisible()
{
    const bool isAlreadyOpen = (NULL != _infoOnEditMode) || (NULL != _infoOnKeyboardShortcuts);
    return isAlreadyOpen ;
}

void MainWindow::on_actionChooseEditType_triggered()
{
    showEditingTypeDialog();
}

void MainWindow::showEditingTypeDialog()
{
    EditingTypesDialog editingTypesDialog(data, this);
    editingTypesDialog.exec();
    data->setEditTypeDialogShown();
}

void MainWindow::requestOpenGuidedPanel()
{
    data->emitOpenUserGuidedPanel();
}

void MainWindow::on_actionValidationDialog_triggered()
{
    GuidedValidationDialog::showValidationDialog(data);
}

void MainWindow::on_actionOpenGuidedOperationPanel_triggered()
{
    requestOpenGuidedPanel();
}

void MainWindow::on_actionChooseUserProfile_triggered()
{
    data->showUserTypePanel();
}

void MainWindow::on_actionInsertDisablingParent_triggered()
{
    ui.editor->onInsertDisabledParent();
}

void MainWindow::onEditorConfigure(const QPoint& position)
{
    QMenu contextMenu(this);
    contextMenu.addAction(ui.actionTaskDisplayDetail);
    contextMenu.addAction(ui.actionChooseEditType);
    contextMenu.exec(ui.editor->getMainTreeWidget()->mapToGlobal(position));
}

void MainWindow::on_actionSearchCommand_triggered()
{
    QList<QAction*> actions = findChildren<QAction*>();
    SearchCommandDialog searchCommandDialog(actions, this);
    if(searchCommandDialog.exec() == QDialog::Accepted) {
        QAction *action = searchCommandDialog.selectedAction();
        fireAction(action);
    }
}

void MainWindow::fireActionByName(const QString &name)
{
    QAction* action = findChild<QAction*>(name);
    fireAction(action);
}

void MainWindow::fireAction(QAction *action)
{
    if((NULL != action) && action->isEnabled()) {
        action->trigger();
    }
}

void MainWindow::onStateKeyboardShortcutChanged(bool newState)
{
    ui.actionShowKeyboardShortcuts->setChecked(newState);
}

void MainWindow::on_actionShowKeyboardShortcuts_triggered()
{
    data->requestShowHideKeyboardInfo();
}

void MainWindow::on_actionEditAsText_triggered()
{
    ui.editor->editSelectionAsText();
}

void MainWindow::on_actionTestXML_triggered()
{
    _controller.testXML();
}

void MainWindow::on_actionLoadSample_triggered()
{
    openSampleUsingDialog(getRegola()->fileName());
}

bool MainWindow::openSampleUsingDialog(const QString folderPath, const EWindowOpen useWindow)
{
    if(!checkAbandonChanges(useWindow)) {
        return false;
    }
    Utils::message(this, tr("Only a sample of data will be loaded."));
    QString filePath = QFileDialog::getOpenFileName(
                           this, tr("Load Sample File"),
                           QXmlEditData::sysFilePathForOperation(folderPath),
                           Utils::getFileFilterForOpenFile()
                       );
    if(!filePath.isEmpty()) {
        return loadSample(filePath, true, useWindow);
    }
    return false ;
}

void MainWindow::on_actionGenerateXSD_triggered()
{
    _controller.generateXSDFromData();
}

void MainWindow::on_actionGenerateXMLFromXSD_triggered()
{
    _controller.generateDataFromXSD();
}
