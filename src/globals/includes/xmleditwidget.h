/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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

#ifndef XMLEDITWIDGET_H
#define XMLEDITWIDGET_H

#include "libQXmlEdit_global.h"

#include <QWidget>
#include <QDomDocument>

#include "xmlEdit.h"
#include "xmleditglobals.h"


#include "navigation.h"
#include "regola.h"

// Forward declaration : do not remove, it is usefull for third party applications to include only this file
class QXmlEditData;
class XSDSchema;
class Element;
class UIDelegate;
class QAttributeDelegate;
class PaintInfo;

class XsltHelper;
class XSDOperationParameters;
class AnonAlg ;
class XSDAnnotationEditProvider;
class NamespaceReferenceEntry;
class ReplicaCommand;
class ReplicaCloneInfo;

namespace Ui
{
class XmlEditWidget;
}

class XmlEditWidgetPrivate;

class XmlEditWidgetEditMode
{
private:
    XmlEditWidgetEditMode() {}
    ~XmlEditWidgetEditMode() {}
public:
    enum EditMode {
        XML,// xml edit mode
        XSLT// xslt edit mode
    };
};

class LIBQXMLEDITSHARED_EXPORT XmlEditWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool encodingLabelVisible READ isEncodingLabelVisible WRITE setEncodingLabelVisible)
    Q_PROPERTY(bool viewAsXSDButtonVisible READ isViewAsXSDButtonVisible WRITE setViewAsXSDButtonVisible)
    Q_PROPERTY(bool moveButtonsVisible READ areMoveButtonsVisible WRITE setMoveButtonsVisible)
    Q_PROPERTY(ulong versionNumber READ versionNumber)

    friend class XmlEditWidgetPrivate ;

private:
    XmlEditWidgetPrivate *d;


public:
    explicit XmlEditWidget(QWidget *parent = 0);
    ~XmlEditWidget();

    bool isReady();

    ulong versionNumber();
    XmlEditWidgetPrivate *getPrivate();

    bool editElement(QTreeWidgetItem *item);

    bool init();
    void setData(QApplication *newApplication, QXmlEditData *newData, const bool newIsSlave, UIDelegate *newUiDelegate);
    void setUIDelegate(UIDelegate *newUiDelegate);

    Element *getSelectedItem();
    QTreeWidgetItem *getSelItem();
    void deleteSchema();

    void display();
    void repaint();
    void resetTree();
    void computeSelectionState();
    void showControls(const bool how);
    void showButtons(const bool how);

    PaintInfo *getPaintInfo();

    void doLoadFileXplore(const QString &filePath);
    void openProva();
    QString getContentAsText();
    qxmledit::EDisplayMode displayMode();

    bool isCDATA();
    void setCDATA(const bool isCData);
    void doNew();

    Regola *getRegola();

    XmlEditWidgetEditMode::EditMode editMode();
    void setEditMode(const XmlEditWidgetEditMode::EditMode newMode);

    void editXSLTElement();

    void autoLoadValidation();
    //void triggersWelcomeDialog();

    void error(const QString& message) ;
    void warning(const QString& message) ;
    void message(const QString& message) ;
    bool askYN(const QString & message) ;

    QTreeWidget *getMainTreeWidget();
    void addChild();
    void addBrother();

    void errorNoRule();
    void setClipBoardItem(Element *pElement);

    bool isActionMode();
    bool isActionNoScanMode();
    void setDisplayMode(const qxmledit::EDisplayMode value);

    void setDocument(QDomDocument &document, const QString &filePath, const bool isSetState);
    bool readData(QXmlStreamReader *reader, const QString &filePath, const bool isSetState);
    void setNavigationDataAndEnable(const int minFragment, const int maxFragment);
    void showNavigationBox();
    void showToolbar(const bool how);
    void loadText(const QString &text, const bool isChangeState = true, const bool isAskForReview = false);
    void assignRegola(Regola*regola,  const bool isChangeState = true);
    void insertSnippet(Regola *newRegola);
    void insertAllowedElements(Element *element);
    bool validateWithFile(const QString &filePath);
    XSDSchema *schema();
    bool isUndoPossible();
    bool isRedoPossible();
    void undo();
    void redo();
    void onActionCompareXSD(const QString &folderPath);
    void onActionExportSiblingsAttributesAsCSVClipboard();
    void onInsertChildContainer();
    void onInsertParent();
    void onRemoveParent();
    void onXSDInsertElement();
    void onXSDAppendElement();
    void onXSDInsertType();
    void onXSDAppendType();
    void onXSDInsertAttribute();
    void onXSDAppendAttribute();
    void onXSDModifyType();
    bool onEditXSDAnnotation();
    void setOrigDataForAnonPreview(QHash<void *, QString> *newOrigData);
    void setXSDAnnotationEditProviderObject(XSDAnnotationEditProvider *newProvider);
    void onFindNext();
    void onFindPrevious();
    void onSetIndent();
    void removeNilAttribute();
    void insertNilAttribute();
    void removeXSITypeAttribute();
    void insertXSITypeAttribute(const QString &newValue);
    void insertXmlSchemaReferences();
    bool insertXsdReference(NamespaceReferenceEntry *entry);
    bool actionFillSerie();
    bool doReplica(ReplicaCloneInfo *cmd, Element *element);
    void updateAttributeIndentationSettings();

    QMenu* getCopyMenu();

    bool replaceAll(ReplaceTextParams *params);
    bool replace(ReplaceTextParams * findArgs);

private:
    Ui::XmlEditWidget *ui;

    //-----------------------
    void emitReevaluateSelectionState();
    void emitDocumentIsModified(const bool isModified);// TODO: emit this
    void emitTreeContextMenuRequested(const QPoint &);
    void emitSignalSetClipBoardActionsState(const bool isAction);
    void emitOkClicked();
    void emitCancelClicked();
    void emitEnableZoom();
    void emitViewAsXsdRequested();
    void emitSchemaLabelChanged(const QString &newLabel);
    void emitShowStatusMessage(const QString &message, const bool isLongTimeout);
    void emitDataReadyMessage(const QString &newLabel);
    void emitLoadCurrentPage(const int page);
    //-----------------------

protected:
    virtual void changeEvent(QEvent *e);
public:
    // Show/Hide Move buttons
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
    void onActionPaste();
    void onActionPasteLastAttributes();
    void actionCopyElementPathToClipboard();
    void onActionCopy();
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
    void onActionCompactView(const bool isChecked);
    void onActionShowAlwaysFullTextComments(const bool isShow);
    void onActionHideBrothers();
    void closeSiblings();
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
    void onActionValidate();
    void onActionValidateNewFile();
    bool validateUsingDocumentReferences();
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
    void loadSchema(const QString &schemaURL);
    void setCurrentItem(Element *newSelection);

    void viewNodes();
    void addXsltMenuItems(QMenu *contextMenu);

    XsltHelper *XSLTHelper();
    void scanXMLTagsAndNamesXSLTAutocompletion();
    void showXSLNavigator();
    void specificProperties();
    void setCopyPathAction(QAction* action);
    void pasteAsSibling();
    void onActionInsertMetadata();
    void onActionInsertCompleteMetadata();
    void onActionEditMetadata();
    bool writeData(const QString &filePath);
    bool insertProlog();
    bool setEncoding(const QString &newEncoding);
    void removeAllElements();
    void showCodePages(QWidget *parent);
    void invalidatePaintData();

    void setReadOnly(const bool newState);
    bool isReadOnly();
    QString labelForEncoding(const QString &encoding);
    //-----------------------
    bool XSDApplyOperation(const ElementOp::Op op, XSDOperationParameters *params);
    //-----------------------

    //------------------- slots
signals:
    void reevaluateSelectionState();
    void documentIsModified(const bool isModified);// TODO: emit this
    void treeContextMenuRequested(const QPoint &);
    void signalSetClipBoardActionsState(const bool isAction);
    void okClicked();
    void cancelClicked();
    void enableZoom();
    void viewAsXsdRequested();
    void schemaLabelChanged(const QString &newLabel);
    void showStatusMessage(const QString &message, const bool isLongTimeout);
    void dataReadyMessage(const QString &newLabel);
    void loadCurrentPage(const int page);
    void undoStateUpdated(const bool undoPossible, const bool redoPossible);
    void editModeChanged();
    void readOnlyStateChanged();
    void encodingChanged(const QString &newEncoding);
    void indentationChanged(const bool indentationEnabled, const int newIndent);
    void schemaValidationError(const QString &message, Element *element);
    void newXSDSchemaForValidation(const QString &newSchemaUrl);
    void requestInsert();
    void requestDelete();

    //--------------- signals
};

#endif // XMLEDITWIDGET_H
