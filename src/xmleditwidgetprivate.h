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


#ifndef XMLEDITWIDGETPRIVATE_H
#define XMLEDITWIDGETPRIVATE_H

#include "xmlEdit.h"
#include "xmleditwidget.h"
#include "ui_xmleditwidget.h"
#include "xmlwidgetutilities.h"
#include "modules/xslt/xslthelper.h"

#include "qxmleditdata.h"
#include "xsdeditor/io/xschemaloader.h"
#include "modules/xsd/xsdoperationparameters.h"
#include "modules/xsd/xsdhelper.h"
#include "modules/specialized/scxml/widgets/scxmlnavigatorwidget.h"
#include "modules/xslt/xsltnavigatorwidget.h"

class XElementContent;
class LineEditWithCompleter;
class AnonAlg ;
class AnonymizeParameters;
class ValidatorMessageHandler;
class FindNodeWithLocationInfo;
class NamespaceReferenceEntry;
class ReplicaCommand;
class ReplicaCloneInfo;

class XmlEditWidgetPrivate : public QObject, XSDAnnotationEditProvider
{
    Q_OBJECT
    friend class XmlEditWidget ;

    //---------------------------------
    XmlEditWidget *p;
    bool    started;
    bool    internalStateOk;
    XmlEditWidgetEditMode::EditMode _editMode;

    Regola *regola;
    PaintInfo   paintInfo;
    QXmlEditData *_appData;
    QXmlEditData _defaultData;
    QMenu *styleMenu;
    QMenu *copyMenu;
    QActionGroup styleActionGroup;
    bool isSlave;
    UIDelegate *_uiDelegate;
    int mainFontSize;

    qxmledit::EDisplayMode _displayMode;
    XSDSchema *_schemaRoot;

    Regola *newRegola(const bool bind = true);
    XmlWidgetUtilities _helper;
    XsltHelper _xsltHelper;
    QAction *_xsltAction;
    QAction *_SCXMLAction;
    QAction *_copyPathAction;
    bool _readOnly;
    XSDAnnotationEditProvider *_XSDAnnotationEditProvider ;
    SCXMLNavigatorWidget *_SCXMLNavigator;
    XSLTNavigatorWidget *_XSLTNavigator;
    QTimer _updateTimer;

public:
    XmlEditWidgetPrivate(XmlEditWidget *theOwner);
    virtual ~XmlEditWidgetPrivate();

    bool isReady();
    void secondStepConstructor();
    void deleteSchema();
    void showControls(const bool how);
    void showButtons(const bool how);
    void showToolbar(const bool how);
    VStyle* loadStyleMenu();
    void resetStyleMenu();
    void deleteRegola();
    bool setUpDelegates();
    void startUIState();
    void setNewStyle(VStyle *newStyle);
    void buildCopyMenu();
    void resetCopyMenu();
    QMenu* getCopyMenu();
    QTreeWidgetItem *getSelItem();

    void moveUp();
    void moveDown();
    void deleteItem();
    void editItem();

    Element *findTextOperation(const bool isFindOrCount, const FindTextParams::EFindType findType = FindTextParams::FindAllOccurrences);

    QAction *createStyleAction(QMenu *menu, const QString &label, const QString &tag, const QString &tooltip);
    QAction *createAnAction(QMenu *menu, const QString &label, const QString &tag, const QString &tooltip);

    void cleanClipBoard();

    void setCopyPathAction(QAction* action);
    void setReadOnly(const bool newState);
    bool isReadOnly();

    //--------------------------------------------------------------------------------------
    ulong versionNumber();

    bool editElement(QTreeWidgetItem *item, const bool isByMouse, const bool forceTextual = false);

    bool init();
    void setData(QXmlEditData *newData, const bool newIsSlave, UIDelegate *newUiDelegate);
    void setUIDelegate(UIDelegate *newUiDelegate);

    Element *getSelectedItem();

    void display();
    void repaint();
    void resetTree();
    void computeSelectionState();

    bool finishSetUpUi();
    void setupSCXMLNavigator();

    PaintInfo *getPaintInfo(); //TODO

    void doLoadFileXplore(const QString &filePath);
    void openProva();
    QString getContentAsText();
    qxmledit::EDisplayMode displayMode();

    bool isCDATA();
    void setCDATA(const bool isCData);
    void doNew();

    Regola *getRegola();

    void autoLoadValidation();

    void error(const QString& message) ;
    void warning(const QString& message) ;
    void message(const QString& message) ;
    bool askYN(const QString & message) ;

    QTreeWidget *getMainTreeWidget();
    void addChild();
    void addBrother();

    void errorNoRule();
    void setClipBoardItem(Element *pElement);
    ClipboardElementList* getClipBoardItemList(const bool onlyElements = true) const;

    bool isActionMode();
    bool isActionNoScanMode();
    void setDisplayMode(const qxmledit::EDisplayMode value);

    void setDocument(QDomDocument &document, const QString &filePath, const bool isSetState);
    bool readData(XMLLoadStatus *status, QXmlStreamReader *reader, const QString &filePath, const bool isSetState, XMLLoadErrorHandler *errorHandler);
    void setNavigationDataAndEnable(const int minFragment, const int maxFragment);
    void showNavigationBox();
    bool loadText(const QString &text, const bool isChangeState = true, const bool isAskForReview = false);
    void assignRegola(Regola *newModel, const bool isSetState);
    void assignRegolaAndTakeOwnership(Regola *newRegola, const bool isChangeState);
    void insertSnippet(Regola *newRegola);
    void insertAllowedElements(Element *element);
    bool validateWithFile(const QString &filePath);
    XSDSchema *schema();
    bool isExpandTreeOnLoad();
    bool isUndoPossible();
    bool isRedoPossible();
    void undo();
    void redo();
    void notifyUndo();
    void setCurrentItem(Element *newSelection);
    void selectAndShowItem(Element *newSelection);

    XSDAnnotationEditProvider *XSDAnnotationEditProviderObject();
    void setXSDAnnotationEditProviderObject(XSDAnnotationEditProvider *newProvider);
    XSDAnnotationEditor *newEditor(QWidget *window);
    void autoDelete();
    void XSDSetNamespaceToParams(XSDOperationParameters *params, Element *element);

    //------------------------------------
    void setMoveButtonsVisible(bool isShow);
    bool areMoveButtonsVisible();
    // Show/Hide ViewAsXsd
    void setViewAsXSDButtonVisible(bool isShow);
    bool isViewAsXSDButtonVisible();
    // Show/Hide encoding label at the top of the right layout
    void setEncodingLabelVisible(bool isShow);
    bool isEncodingLabelVisible();
    // Show/Hide doc type label
    void setDocTypeVisible(bool isShow);
    bool isDocTypeVisible();
    //
    void resizeTreeColumns();
    void onAddBrother();
    void onDeleteItem();
    void onMoveUp();
    void onMoveDown();
    void onActionPaste();
    void onActionPasteLastAttributes();
    void actionCopyElementPathToClipboard();
    void onActionCopy();
    void onActionCopyElementOnly();
    void onActionCut();
    void onActionExpandAll();
    void onActionExpandSelectedItem();
    void onActionShowAttrLine(const bool state);
    void onActionAppendComment();
    void onActionAddComment();
    void onActionAppendProcessingInstruction();
    void onActionAddProcessingInstruction();
    void onActionResizeToContents();
    void onActionAddChildElement();
    void onActionAppendChildElement();
    void onActionEdit();
    void onActionFind();
    void onActionReplace();
    void onActionDelete();
    void onActionMoveUp();
    void onActionMoveDown();
    void onActionGoToParent();
    void onActionGoToPreviousBrother();
    void onActionGoToNextBrother();
    void onActionClearBookmarks();
    void onActionToggleBookmark();
    void onActionGotoNextBookmark();
    void onActionGotoPreviousBookmark();
    void onActionShowChildIndex(const bool isChecked);
    void onActionCloseThisAllBrothers();
    bool onActionNewFromClipboard();
    bool newFromString(const QString &newData);
    void onActionCompactView(const bool isChecked);
    void onActionShowAttributesSorted(const bool isChecked);
    void onActionShowAlwaysFullTextComments(const bool isShow);
    void onActionHideBrothers();
    void closeSiblings();
    void closeItemAndChildren();
    void onActionFixedSizeAttributes(const bool isChecked);
    void onActionShowAttributesLength(const bool isChecked);
    void onActionShowCurrentElementTextBase64(const bool isChecked);
    void onActionShowBase64(const bool isChecked);
    void onActionEditInnerXML();
    void onActionEditInnerXMLBase64();
    void onActionEditInnerBase64Text();
    void onActionZoomIn();
    void onActionZoomOut();
    void onActionCopyPathToClipboard();
    bool onActionValidate();
    bool validateUsingDocumentReferences();
    void onActionValidateNewFile();
    void onActionInsertSnippet();
    void onActionShowElementTextLength(const bool isChecked);
    void onActionShowElementSize(const bool isChecked);
    void onActionDeleteBookmarked();
    void onActionCleanBookmarkedContent();
    void onActionHideView(const bool isChecked);
    void onActionViewAsXsd();
    void onActionAllowedSchemaElements();
    void onActionPasteAndSubstituteText();
    void onActionNewUsingXMLSchema(const QString &schemaURL);
    void onActionTransformInComment();
    void onActionExtractElementsFromComment();
    Element* onActionHideLeafChildren();
    void onActionHideAllLeafChildren();
    Element* onActionShowLeafChildren();
    void onActionShowAllLeafChildren();
    bool isValidXsd();
    void onActionCompareXSD(const QString &folderPath);
    void onActionExportSiblingsAttributesAsCSVClipboard();
    void onActionInsertMetadata();
    void onActionInsertCompleteMetadata();
    void onActionEditMetadata();
    bool writeData(const QString &filePath);
    bool insertProlog();
    bool setEncoding(const QString &newEncoding);

    void loadSchema(const QString &schemaURL);

    bool findAllowedItemsElement(XElementContent *content, Element *element, bool &startAsRoot);
    void viewNodes();

    XmlEditWidgetEditMode::EditMode editMode();
    void setEditMode(const XmlEditWidgetEditMode::EditMode newMode);

    QTreeWidget *getEditor();
    QString namespacePrefixXslt();
    void appendElementComplete(Element *theNewElement, Element *brotherElement);
    void insertElementComplete(Element *theNewElement, Element *parentElement);
    void addXsltMenuItems(QMenu *contextMenu);
    QXmlEditData *appData();
    void editXSLTElement(QTreeWidgetItem *item);
    void editXSLTElement();
    XsltHelper *XSLTHelper();
    void scanXMLTagsAndNamesXSLTAutocompletion();
    void showXSLNavigator(const bool how);

    enum EEditMode {
        EditModeDetail,
        EditModeSpecific,
        EditModeTextual,
        EditModeTextualText,
        EditModeTextualDependingOnMouse,
        EditModeForceSpecial
    };

    void specificPropertiesItem(QTreeWidgetItem * item, const EEditMode editModeParam);
    void specificProperties();
    void pasteAsSibling();
    void removeAllElements();
    void showCodePages(QWidget *parent);
    void invalidatePaintData(const bool full = false);
    void onInsertChildContainer();
    void onInsertParent();
    void onRemoveParent();
    bool onXSDInsertElement();
    bool onXSDAppendElement();
    bool onXSDAppendAttribute();
    bool onXSDInsertAttribute();
    bool onXSDAppendType();
    bool onXSDInsertType();
    bool onXSDModifyType();
    bool onEditXSDAnnotation();
    void setOrigDataForAnonPreview(QHash<void *, QString> *newOrigData);
    //----
    bool replace(ReplaceTextParams *params);
    bool replaceAll(ReplaceTextParams * findArgs);
    void onFindNext();
    void onFindPrevious();
    bool insertXsdReference(NamespaceReferenceEntry *entry);

    //----
    bool XSDApplyOperation(const ElementOp::Op op, XSDOperationParameters *params);
    bool XSDApplyOperationOnElement(const ElementOp::Op op, XSDOperationParameters *params, Element *element);

    void sortAttributes();
    void sortAttributesElement(Element *element, const bool isRecursive);
    void deleteSiblings(const RegolaDeleteSiblings::DeleteOptions option, Element *selectedItem);

    //---region(names)
    void prefixRemove(const QString &removedPrefix, Element *element, const TargetSelection::Type targetSelection, const bool isAllPrefixes);
    void prefixSet(const QString &newPrefix, Element * element, const TargetSelection::Type targetSelection);
    void prefixReplace(const QString &oldPrefix, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool isAllPrefixes);
    //---endregion(names)

    //---region(namespaces)
    void namespaceRemove(const QString &removedNS, Element *element, const TargetSelection::Type targetSelection, const bool isAllNS, const bool isRemoveDeclarations);
    void namespaceSet(const QString &newNS, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool avoidClash, NamespacesInfo *info);
    void namespaceReplace(const QString &oldNS, const QString &newNS, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool avoidClash, NamespacesInfo *info);
    void namespaceNormalize(const QString &ns, const QString &newPrefix, Element *element, const TargetSelection::Type targetSelection, const bool declareOnRoot, const bool avoidClash, NamespacesInfo *info);
    void namespaceAvoidClash(const QString &ns, const QString &newPrefix, NamespacesInfo *namespacesInfo);
    //---endregion(namespaces)

    void insertSpecial();
    void appendSpecial();

    void showSCXMLNavigator(const bool how);
    void applySCXML();
    void applyXSLT();

private:
    QHash<void *, QString> *anonDataForPreview();
    void editSelection(const EEditMode editMode);

private slots:
    void schemaLoadComplete(XSchemaLoader *loader, const XSchemaLoader::Code code);
    void regolaIsModified();
    void regolaUndoChanged();
    void findText();
    void countTextOccurrences();
    void elementDoubleClicked(QTreeWidgetItem * item, int column) ;
    void on_treeWidget_itemSelectionChanged();
    void on_viewAsXsdCmd_clicked();
    void treeContextMenu(const QPoint& position);
    void onStyleChanged();
    void onPredefinedStyleChanged();
    void onCopySpecial();
    void navigateToPage(const int page);
    void docTypeChanged(const QString &docType);

    // buttons of the user interface
    void on_ok_clicked();
    void on_cancel_clicked();
    void on_moveUp_clicked();
    void on_moveDown_clicked();
    void on_addChild_clicked();
    void on_addBrother_clicked();
    void on_editItem_clicked();
    void on_deleteItem_clicked();
    void on_closeSearchPanel_clicked();

    void openAdvancedResultPanel();

    void onEncodingChanged(const QString &newEncoding);
    void onSearchNext();
    void onSearchPrev();

    void onReplace(const FindTextParams::EFindType replaceType);
    void onReplaceAll();
    void onReplaceSkipAndGotoNext();
    void onReplaceSkipAndGotoPrevious();
    void onReplaceReplaceAndGotoNext();
    void onReplaceReplaceAndGotoPrevious();
    void onSetIndent();
    void onIndentationChanged(const bool indentationEnabled, const int newIndentation);
    void onShortcutDelete();
    void onShortcutInsert();
    void onShortcutInsertSpec();
    void onShortcutAppend();
    void onShortcutAppendSpec();
    void removeNilAttribute();
    void insertNilAttribute();
    void removeXSITypeAttribute();
    void insertXSITypeAttribute(const QString &newValue);
    void insertXmlSchemaReferences();
    bool actionFillSerie();
    bool doReplica(ReplicaCloneInfo *cmd, Element *element);
    void updateAttributeIndentationSettings();
    void setFacets(Element *selection, QList<XSDFacet*> facets);
    bool onEditEnum();
    void updateTimeout();
    void onSCXMLNavigatorEditState(const QString &stateName, Element *element);
    void onSCXMLNavigatorGoToState(const QString &stateName, Element *element);
    void onXSLTNavigatorGoTo(Element *element);
    void onXSLTNavigatorEdit(Element *element);
    void openSiblingsSameLevel();
    void onShortcutShiftEnter();
    void onShortcutCtrlEnter();
    void onShortcutAltEnter();
    void onShortcutT();
    void onShortcutX();
    void onShortcutY();
    void addFormattingInfo();
    void removeFormattingInfo();
    bool presetFormatting(const XMLIndentationSettings::ESettings preset);
private:
    void bindRegola(Regola *newModel, const bool bind = true);
    XSDOperationParameters *getXSDParams(const bool isInsert, XSDOperationParameters::EObjectType entityType, const QString &name, Element *selection);
    void showValidationResults(const QString xmlAsText, ValidatorMessageHandler &validator);
    QList<int> makeDomNodePath(QDomNode elementToExamine);
    bool findDomNodeScan(QDomNode node, QDomNode nodeTarget, const int lineSearched, const int columnSearched, FindNodeWithLocationInfo &info);
    void recalcRowHeightClass();
    void showError(const QString &errMessage);
    bool showLoadError(const QString &errorMessage, XMLLoadErrorHandler *handler, XMLLoadContext *context, QXmlStreamReader *xmlReader);
    void houseworkRegola(Regola *regola);
    static QList<int> pathForElement(Element * element);
    void openSiblingsSameLevel(Element *element);
    void closeItemRecursive(QTreeWidgetItem *item);
protected:
    bool eventFilter(QObject *obj, QEvent * event);

};

#endif // XMLEDITWIDGETPRIVATE_H
