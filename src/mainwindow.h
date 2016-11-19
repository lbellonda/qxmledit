/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2016 by Luca Bellonda and individual contributors  *
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

#ifndef REGOLA_MAIN_H
#define REGOLA_MAIN_H

#include "xmlEdit.h"
#include "ui_MainWindow.h"
#include "applicationdata.h"
#include "navigation.h"
#include "regola.h"
#include "widgets/navigationmode.h"
#include "qattributedelegate.h"
#include "extraction/extractresults.h"
#include "UIDelegate.h"
#include "xsdeditor/io/xschemaloader.h"

#include "xsdeditor/xschema.h"
#include "xmleditwidget.h"
#include "sessionstatewidget"
#include "widgets/attrfilterstatus.h"
#include "mainwndcontroller.h"


class SnippetManager;
class QXmlEditApplication;

class MainWindow : public QMainWindow, UIDelegate, XMLLoadErrorHandler
{
    Q_OBJECT
    bool    started;
    bool    internalStateOk;
    MainWndController _controller;

    QApplication *application;
    ApplicationData *data;
    bool isSlave; // use and forward
    QEventLoop *eventLoop;
    int _returnCodeAsSlave;
    bool _slaveIsClosed;
    int maxLastFiles;
    int maxPrefDirs;
    QList<QAction*> lastFiles;
    QList<QAction*> preferredDirs;
    QList<QAction*> pluginsCmds;

    QStringList preferredDirsNames;
    NavigationMode *navigationModeIndicator;
    QLabel *labelSchema;
    QLabel *labelMode;
    QLabel *_labelReadOnly;
    QLabel *_labelReadOnlyImg;
    QLabel *_labelIndentation;
    SessionStateWidget *_sessionStateWidget;
    AttrFilterStatus *_attributeFilterWidget;

    ExtractResults *_extractResult; // this one is half and half
    UIDelegate *uiDelegate;
    AttributeFilter *_attrFilter;
    SnippetManager *_snippetManager;
    QActionGroup *_encodingGroup;
    QIcon _windowIcon;
    QToolButton *_xsdButton ;
    int _errorCount;
    QString _exportPath ;
    XMLLoadErrorHandler *_loadErrorHandler;
public:
    enum EWindowOpen {
        OpenUsingDefaultSettings,
        OpenUsingSameWindow,
        OpenUsingNewWindow,
    };

    MainWindow(const bool isSlave, QApplication *application, ApplicationData *data, QMainWindow *parent = 0);
    virtual ~MainWindow();

    ApplicationData *appData();
    virtual bool loadFile(const QString &filePath, const bool activateModes = true, const EWindowOpen useWindow = OpenUsingDefaultSettings, const bool isRegularFile = true);
    MainWindow *loadFileAndReturnWindow(const QString &filePath, const bool activateModes = true,
                                        const EWindowOpen useWindow = OpenUsingDefaultSettings, const bool isRegularFile = true);
    MainWindow *createFromClipboard(const EWindowOpen useWindow = OpenUsingDefaultSettings);
    bool newFromClipboard();
    void setEventLoop(QEventLoop *eventLoop);
    QString getContentAsText();

    Regola *getRegola();
    SessionStateWidget *sessionStateWidget();

    void autoLoadValidation();
    void triggersWelcomeDialog();

    void error(const QString& message) ;
    void error(QWidget *widget, const QString& message) ;
    void warning(const QString& message) ;
    void message(const QString& message) ;
    bool askYN(const QString & message) ;
    bool askYN(QWidget *parent, const QString & message);
    void errorNoSel(QWidget *parent) ;
    void errorOutOfMem(QWidget *parent);
    void resetErrorCount() ;
    int errorCount() ;

    QString msgOutOfMem() ;

    QTreeWidget *getMainTreeWidget();
    XmlEditWidget *getEditor();

    virtual QWidget *getMainWidget();
    virtual QString getAppTitle();
    virtual QString editNodeElementAsXML(const bool isBase64Coded, Element *pElement, const QString &text, const bool isCData, bool &isCDataOut, bool &isOk);

    Ui::MainWindow *getUI();
    void setUIDelegate(UIDelegate* newDelegate);
    void loadVisFile(const QString &fileName);
    QLabel *readOnlyStatusLabel();
    QString readOnlyStateText(const bool isReadOnly);
    QString readOnlyStateTextImage(const bool isReadOnly);
    QString readOnlyTooltip(const bool isReadOnly);
    bool isReadOnly();
    void onAnonymize(AnonContext *context);
    void onAnonymize(AnonAlg *alg);
    MainWndController *controller();

    bool reload();
    QXmlEditApplication* qXmlEditApplication();
    void updateAfterPreferences();
    void setupFirstAccess();
    void taskChooseDetail();

protected:
    //bool eventFilter(QObject *obj, QEvent *event); no need in this version until now.
    void setSnippetManager(SnippetManager *newSnippetManager);
    MainWindow *makeNewWindow();
    bool loadFileInner(const QString &filePath, const bool isRegularFile = true, const bool activateModes = true);
    bool loadFileInnerStream(const QString &filePath, const bool isRegularFile = true, const bool activateModes = true);
    bool loadFileInnerStream(QIODevice *ioDevice, const QString &filePath, const bool isRegularFile, const bool activateModes = true);
    /*!
     * \deprecated
     */
    bool loadFileInnerDom(const QString &filePath, const bool isRegularFile, const bool activateModes);
    bool readData(XMLLoadStatus *status, QXmlStreamReader *reader, const QString &filePath, const bool isSetState, XMLLoadErrorHandler *errorHandler);
    void newUsingXMLSchema();
    Regola::EExportOptions askExportOption();
    QString getExportPath();
    void updateExportFilePath(const QString &newPath);
    void clearExportFilePath();
    void checkForSaveEncoding();

private slots:
    void onRaiseWindow();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionSaveAs_triggered();
    void on_actionAbout_triggered();
    void on_actionPaste_triggered();
    void on_actionCopy_triggered();
    void on_actionCut_triggered();
    void on_actionExpandAll_triggered();
    void on_actionExpandSelectedItem_triggered();
    void on_actionShowAttrLine_triggered();
    void on_actionAppendComment_triggered();
    void on_actionAddComment_triggered();
    void on_actionAppendProcessingInstruction_triggered();
    void on_actionAddProcessingInstruction_triggered();
    void on_actionAddChildElement_triggered();
    void on_actionAppendChildElement_triggered();
    void on_actionEdit_triggered();
    void on_actionFind_triggered();
    void on_actionReplace_triggered();
    void on_actionDelete_triggered();
    void on_actionMoveUp_triggered();
    void on_actionMoveDown_triggered();
    void on_actionGo_To_Parent_triggered();
    void on_actionGo_to_Previous_Brother_triggered();
    void on_actionGo_to_Next_Brother_triggered();
    void on_actionClearBookmarks_triggered();
    void on_actionToggleBookmark_triggered();
    void on_actionGotoNextBookmark_triggered();
    void on_actionGotoPreviousBookmark_triggered();
    void on_actionShowChildIndex_triggered();
    void on_actionCloseThisAllBrothers_triggered();
    void on_actionNewFromClipboard_triggered();
    void on_actionQuit_triggered();
    void on_actionCompactView_triggered();
    void on_actionShowAttributesSorted_triggered();
    void on_actionShowAlwaysFullTextComments_triggered();
    void on_actionHideBrothers_triggered();
    void on_actionCloseSiblings_triggered();
    void on_actionFixedSizeAttributes_triggered();
    void on_actionShowAttributesLength_triggered();
    void on_actionShowCurrentElementTextBase64_triggered();
    void on_actionShowBase64_triggered();
    void on_actionConfigure_triggered();
    void on_actionEditInnerXML_triggered();
    void on_actionEditInnerXMLBase64_triggered();
    void on_actionEditInnerBase64Text_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionCompare_triggered();
    void on_actionReload_triggered();
    void on_actionCopyPathToClipboard_triggered();
    void on_actionShowContainingFolder_triggered();
    void on_actionAddCurrentDirectory_triggered();
    void on_actionEditPreferredDirectories_triggered();
    void on_actionValidate_triggered();
    void on_actionValidateNewFile_triggered();
    void on_actionValidateUsingDocumentReferences_triggered();
    void on_actionInsertSnippet_triggered();
    void on_actionConfigureSnippets_triggered();
    void on_actionExecuteAutoTest_triggered();
    void on_actionTransforminSnippet_triggered();
    void on_actionShowElementTextLength_triggered();
    void on_actionShowElementSize_triggered();
    void on_actionDeleteBookmarked_triggered();
    void on_actionCleanBookmarkedContent_triggered();
    void on_actionXplore_triggered();
    void on_actionHideView_triggered();
    void on_actionSearchInFiles_triggered();
    void on_actionHelpOnQXmlEdit_triggered();
    void on_actionViewAsXsd_triggered();
    void on_actionAllowedSchemaElements_triggered();
    void on_actionPasteAndSubstituteText_triggered();
    void on_actionNewUsingXMLSchema_triggered();
    void on_actionTransformInComment_triggered();
    void on_actionExtractElementsFromComment_triggered();
    void on_actionExtractFragmentsFromFile_triggered();
    void on_actionWelcomeDialog_triggered();
    void on_actionSaveACopyAs_triggered();
    void on_actionHideLeafChildren_triggered();
    void on_actionHideAllLeafChildren_triggered();
    void on_actionShowLeafChildren_triggered();
    void on_actionShowAllLeafChildren_triggered();
    void on_actionColumnView_triggered();
    void on_actionShowCurrentSessionPanel_triggered();

    void on_actionNewSession_triggered();
    void on_actionPauseSession_triggered();
    void on_actionResumeSession_triggered();
    void on_actionCloseSession_triggered();
    void on_actionManageSessions_triggered();
    void on_actionSessionDetails_triggered();

    void on_actionUndo_triggered();
    void on_actionRedo_triggered();

    void on_actionNewWindow_triggered();
    void on_actionViewData_triggered();

    void on_actionBase64Tools_triggered();
    void on_actionEncodingTools_triggered();

    void on_actionCompareXSD_triggered();
    void on_actionExportAttributesSiblingsAsCSVClipboard_triggered();
    void on_actionVisAttrApplyTemporaryProfile_triggered();
    void on_actionVisAttrSaveCurrenteProfile_triggered();
    void on_actionVisAttrManageProfiles_triggered();
    void on_actionVisAttrRemoveCurrentProfile_triggered();
    void on_actionCompareFiles_triggered();
    void on_actionSpring_and_Forces_triggered();
    void on_actionViewGraphic_triggered();
    void on_actionCopyAttributes_triggered();
    void on_actionPasteAttributes_triggered();
    void on_actionPasteLastAttributes_triggered();
    void on_actionCopyElementPathClipboard_triggered();
    void on_actionAutoComplete_triggered();
    void on_actionXMLEditMode_triggered();
    void on_actionXSLTEditMode_triggered();
    void on_actionNewXSLTSheet_triggered();
    void on_actionNewXSDDocument_triggered();
    void on_actionNewXSLFODocument_triggered();
    void on_actionNewMavenPOM_triggered();
    void on_actionNewFromSnippet_triggered();
    void on_actionEditXSLTitem_triggered();
    void on_actionScanXMLTagsAndNamesXSLTAutocompletion_triggered();
    void on_actionShowXSLNavigator_triggered();
    void on_actionSpecificProperties_triggered();
    void on_actionPasteAsSibling_triggered();
    void on_actionTipsOnVisualAppearance_triggered();
    void on_actionBinaryViewer_triggered();
    void on_actionReadOnly_toggled(bool checked);
    void on_actionInsertMetadata_triggered();
    void on_actionInsertCompleteMetadata_triggered();
    void on_actionEditMetadata_triggered();
    void on_actionExportAsJavaJavascriptSource_triggered();
    void on_actionInsertProlog_triggered();
    void onEncodingChanged(QAction* action);
    void onIndentationChanged(const bool indentationEnabled, const int newIndent);
    void onSchemaValidationError(const QString &message, Element *element);
    void onNewXSDSchemaForValidation(const QString &newUrl);
    void on_actionExportAsCSource_triggered();
    void on_actionCopyAllToClipboard_triggered();
    void on_actionCodePages_triggered();
    void on_actionInsertChildContainer_triggered();
    void on_actionInsertParent_triggered();
    void on_actionRemoveParent_triggered();
    void on_actionXSDInsertElement_triggered();
    void on_actionXSDAppendElement_triggered();
    void on_actionXSDInsertType_triggered();
    void on_actionXSDAppendType_triggered();
    void on_actionXSDInsertAttribute_triggered();
    void on_actionXSDAppendAttribute_triggered();
    void on_actionXSDModifyType_triggered() ;
    void on_actionAnonymize_triggered();
    void on_actionQuickReferenceElements_triggered();
    void on_actionAnonymizeFile_triggered();
    void on_actionEditXSDAnnotation_triggered();
    void on_actionEditEnumerationFacet_triggered();
    void on_actionFindNext_triggered();
    void on_actionFindPrevious_triggered();
    void on_actionSetIndent_triggered();
    void on_actionInfo_triggered();
    void on_actionShowMainButtons_triggered();
    void on_actionShowStatusBar_triggered();
    void on_actionRemoveNilAttribute_triggered();
    void on_actionInsertNilAttribute_triggered();
    void on_actionRemoveXSITypeAttribute_triggered();
    void on_actionInsertXSITypeAttribute_triggered();
    void on_actionInsertXmlSchemaReferences_triggered();
    void on_actionOpenSameWindow_triggered();
    void on_actionFillSerie_triggered();
    void on_actionCloneElements_triggered();
    void on_actionClose_triggered();
    void on_actionExportElementToFile_triggered();
    void on_actionSortAttributesElementRecursive_triggered();
    void on_actionSortAttributesElement_triggered();
    void on_actionSortAttributes_triggered();
    void on_actionRemoveAllSiblings_triggered();
    void on_actionRemoveAllSiblingsAfter_triggered();
    void on_actionRemoveAllSiblingsBefore_triggered();
    void on_actionTaskDisplayDetail_triggered();
    void on_actionHelpSetEditorDetail_triggered();
    void on_actionTEST_triggered();
    void on_actionRemovePrefix_triggered();
    void on_actionRemoveNamespace_triggered();
    void on_actionSetPrefix_triggered();
    void on_actionSetNamespace_triggered();
    void on_actionReplacePrefix_triggered();
    void on_actionReplaceNamespace_triggered();
    void on_actionNormalizeNamespace_triggered();

    //----- other slots ------------------

    void onClipboardDataChanged(bool isData);
    void onDocumentIsModified(const bool isModified);
    void onSessionEnablingChanged();
    void onHandleSessionState();
    void onReadOnlyStateChanged();
    void onReadOnlyGo();

    void treeContextMenu(const QPoint& position);

    void onOkClicked();
    void onCancelClicked();

    void onRecentFile();
    void onPreferredDir();
    void onPlugin();
    void navigateToPage(const int page);
    void schemaLoadComplete(const QString &newLabel);
    void onComputeSelectionState();
    void onComputeSelectionStateExperimentalFeatures();
    void onNewMessage(const QString &newMessage);
    void setClipBoardActionsState(const bool isAction);
    void onShowStatusMessage(const QString &message, const bool isLongTimeout);
    bool onSessionfileLoadRequest(const QString& path);
    void onSessionFolderOpenRequest(const QString& path);
    void onShowSessionManagement();
    void onUndoStateUpdated(const bool isUndo, const bool isRedo);
    void onChangeEditorMode();
    void onEditorEncodingChanged(const QString &newEncoding);

    //------------------- slots

private:
    Ui::MainWindow ui;

    void autoTest();

    bool finishSetUpUi();
    bool setupEncoding();
    bool buildPluginsMenu(const char *method, QMenu *parent);

    QString askFileName(const QString &actualName);
    void errorNoRule();

    void setFileTitle();

    void closeEvent(QCloseEvent * event);
    void startUIState();
    void resetTree();
    void calcColumnState();

    void enableZoom();

    // drag and drop
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

    void computeSelectionState();
    void openFileUsingDialog(const QString folderPath, const EWindowOpen useWindow = OpenUsingDefaultSettings);

    Element *getSelectedItem();

    void setDocument(QDomDocument &document, const QString &filePath, const bool isSetState);

    bool checkAbandonChanges(const EWindowOpen useWindow = OpenUsingDefaultSettings, const QString &filePath = "");

    QAction *createAnAction(QMenu *menu, const QString &label, const QString &tag, const QString &tooltip);

    void deleteSchema();
    void activateXSLTonNewFile();

    // repaint of current items
    void repaint();
    // new display
    void display();

    void wheelEvent(QWheelEvent *event);
    bool buildLastObjects(const int maxObjects, QList<QAction*> &cmdList, const char *method, QMenu *parent);
    void updateRecentFilesMenu(const QString &filePath);
    void updatePreferredDirs(QStringList &entries, QList<QAction*>actions);
    void updateMRU(const QString &entry, QList<QAction*>actions);
    void updateEntriesMenu(QStringList &entries, QList<QAction*>actions);

    void loadRecentFilesSettings();
    void loadPreferredDirsSettings();

    void updateEditMode();

    void validateWithFile(const QString &filePath);

    void doLoadFileXplore(const QString &filePath);
    void loadFileXplore(const QString &filePath);

    void setDisplayMode(const qxmledit::EDisplayMode value);
    void doLoadFileBlindMode(QDomDocument &document, const QString &filePath);
    void loadFileBlindMode(const QString &filePath);

    bool isValidXsd();
    bool verifyAbandonChanges();

    void setSchemaLabel(const QString &newLabel);
    void setModeLabel();
    void cleanExtractResults();
    void showNavigationBox();
    void welcomeDialog();
    void findTextOperation(const bool isFindOrCount);
    void evaluateSingleItemLeaves(Element *element);
    void loadCurrentPage(const int page);
    void updateUndoState(const bool isUndo, const bool isRedo);
    void showLoadFileError(const QString &filePath, const QString &errorMsg, const int errorLine, const int errorColumn);
    void removeAttributesFilter();
    void setAttributeProfile(AttributeFilter *filter);
    QString askNameForCurrentAttributeFilter();
    bool createDocumentFromResources(const QString &path);
    void createDocumentFromSnippet(Regola* newRegola);
    //-------region(internal)
    void actionSaveAs_internal(const QString &newFilePath);
    void actionSaveACopyAs_internal(const QString &newFilePath);
    bool actionSave_internal(const QString &newFilePath);
    //-------endregion(internal)
    void updateWindowFilePath();
    void insertXSITypeAttribute(const QString &newValue);
    //--
    void errorOnLoad(QFile &file);
    void errorFileName();
    void markAsAllEdited();

    //-- interface(XMLLoadErrorHandler)
    bool showErrorAndAskUserIfContinue(QWidget *parent, XMLLoadContext *context, QXmlStreamReader *xmlReader) ;

protected:
    virtual void changeEvent(QEvent *e);
    bool openDroppedFile(const QString &filePath);
    bool recentFile(const QString &filePath);
    bool preferredDir(const QString &filePath);
    bool preferredDirLoadFile(const QString &filePath);
    QString askFileNameToOpen(const QString &startFolder = "");

    void onShowHideMainButtons();
    void onShowHideStatusBar();

    friend class MainWndController;
    friend class MainWindowIOHelper;
#ifdef QXMLEDIT_TEST
    friend class TestFilterAttributes;
    friend class TestCopyAttrs;
    friend class TestNewFromTemplate;
    friend class TestMetadata;
    friend class TestEncoding;
    friend class TestXSIType;
    friend class TestLoadFile;
    void setLoadErrorHandler(XMLLoadErrorHandler *newHandler);
#endif
};

void searchInFiles(QWidget * parent);

#endif
