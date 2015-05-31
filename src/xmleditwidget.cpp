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

#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include "xmleditwidget.h"
#include "ui_xmleditwidget.h"
#include "xsdeditor/io/xschemaloader.h"
#include "validatormessagehandler.h"
#include "schemavalidator.h"
#include "elementitemdelegate.h"
#include "alloweditemsinsert.h"
#include "qattributedelegate.h"
#include "config.h"
#include "findtextparams.h"
#include "regola.h"
#include "utils.h"
#include "xsdeditor/xsdplugin.h"
#include "extraction/extractresults.h"
#include "xmleditwidgetprivate.h"

#define CS_ELEMENT_TEXT "E"
#define CS_ELEMENT_TAG "T"
#define CS_ATTRIBUTE_NAME "N"
#define CS_ATTRIBUTE_VALUE "V"


XmlEditWidget::XmlEditWidget(QWidget *parent) :
    QWidget(parent),
    d(new XmlEditWidgetPrivate(this)),
    ui(new Ui::XmlEditWidget)

{
    ui->setupUi(this);
    if(NULL != d) {
        d->secondStepConstructor();
    }
}

XmlEditWidget::~XmlEditWidget()
{
    if(NULL != d) {
        delete d;
    }
    if(NULL != ui) {
        delete ui;
    }
}


//----- region(emit)
void XmlEditWidget::emitReevaluateSelectionState()
{
    emit reevaluateSelectionState();
}

void XmlEditWidget::emitDocumentIsModified(const bool isModified)
{
    emit documentIsModified(isModified);
}

void XmlEditWidget::emitTreeContextMenuRequested(const QPoint &p)
{
    emit treeContextMenuRequested(p);
}

void XmlEditWidget::emitSignalSetClipBoardActionsState(const bool isAction)
{
    emit signalSetClipBoardActionsState(isAction);
}

void XmlEditWidget::emitOkClicked()
{
    emit okClicked();
}

void XmlEditWidget::emitCancelClicked()
{
    emit cancelClicked();
}

void XmlEditWidget::emitEnableZoom()
{
    emit enableZoom();
}

void XmlEditWidget::emitViewAsXsdRequested()
{
    emit viewAsXsdRequested();
}

void XmlEditWidget::emitSchemaLabelChanged(const QString &newLabel)
{
    emit schemaLabelChanged(newLabel);
}

void XmlEditWidget::emitShowStatusMessage(const QString &message, const bool isLongTimeout)
{
    emit showStatusMessage(message, isLongTimeout);
}

void XmlEditWidget::emitDataReadyMessage(const QString &newLabel)
{
    emit dataReadyMessage(newLabel);
}

void XmlEditWidget::emitLoadCurrentPage(const int page)
{
    emit loadCurrentPage(page);
}

//----- region(emit)

bool XmlEditWidget::isReady()
{
    if(NULL == d) {
        return false;
    }
    return d->isReady();
}

void XmlEditWidget::setData(QApplication *newApplication, QXmlEditData *newData, const bool newIsSlave, UIDelegate *newUiDelegate)
{
    d->setData(newApplication, newData, newIsSlave, newUiDelegate);
}

void XmlEditWidget::setUIDelegate(UIDelegate *newUiDelegate)
{
    d->setUIDelegate(newUiDelegate);
}

bool XmlEditWidget::init()
{
    return d->init();
}

void XmlEditWidget::deleteSchema()
{
    d->deleteSchema();
}

/*
//autotest
void XmlEditWidget::autoTest()
{
#ifdef  _DEBUG_BALSAMIQ
    void testBALSAMIQ(QWidget * parent);
    this->show();
    testBALSAMIQ(this);
#endif
}
*/


void XmlEditWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch(e->type()) {
    case QEvent::StyleChange:
        VStyle::changeDefaultBrush(QApplication::palette().text());
        break;
    default:
        break;
    }
}

qxmledit::EDisplayMode XmlEditWidget::displayMode()
{
    return d->displayMode();
}

void XmlEditWidget::repaint()
{
    d->repaint();
}


PaintInfo* XmlEditWidget::getPaintInfo()
{
    return d->getPaintInfo();
}

void XmlEditWidget::display()
{
    d->display();
}


void XmlEditWidget::computeSelectionState()
{
    d->computeSelectionState();
}

//void XmlEditWidget::enableZoom()
//{
//    ui->actionZoomIn->setEnabled(paintInfo.canZoomIn());
//    ui->actionZoomOut->setEnabled(paintInfo.canZoomOut());
//}

Element *XmlEditWidget::getSelectedItem()
{
    return d->getSelectedItem();
}

QTreeWidgetItem *XmlEditWidget::getSelItem()
{
    return d->getSelItem();
}

void XmlEditWidget::resetTree()
{
    d->resetTree();
}

void XmlEditWidget::showControls(const bool how)
{
    d->showControls(how);
}

void XmlEditWidget::showButtons(const bool how)
{
    d->showButtons(how);
}

void XmlEditWidget::onActionNewFromClipboard()
{
    d->onActionNewFromClipboard();
}

void XmlEditWidget::onActionResizeToContents()
{
    d->onActionResizeToContents();
}

void XmlEditWidget::setEncodingLabelVisible(bool isShow)
{
    d->setEncodingLabelVisible(isShow);
}

bool XmlEditWidget::isEncodingLabelVisible()
{
    return d->isEncodingLabelVisible();
}

void XmlEditWidget::setDocTypeVisible(bool isShow)
{
    d->setDocTypeVisible(isShow);
}

bool XmlEditWidget::isDocTypeVisible()
{
    return d->isDocTypeVisible();
}

void XmlEditWidget::setViewAsXSDButtonVisible(bool isShow)
{
    d->setViewAsXSDButtonVisible(isShow);
}

bool XmlEditWidget::isViewAsXSDButtonVisible()
{
    return d->isViewAsXSDButtonVisible();
}

void XmlEditWidget::setMoveButtonsVisible(bool isShow)
{
    d->setMoveButtonsVisible(isShow);
}

bool XmlEditWidget::areMoveButtonsVisible()
{
    return d->areMoveButtonsVisible();
}

ulong XmlEditWidget::versionNumber()
{
    return d->versionNumber();
}

void XmlEditWidget::resizeTreeColumns()
{
    d->resizeTreeColumns();
}

bool XmlEditWidget::editElement(QTreeWidgetItem *item)
{
    return d->editElement(item);
}


void XmlEditWidget::addChild()
{
    d->addChild();
}

// Can't add a brother to the first child (XML)
void XmlEditWidget::addBrother()
{
    d->addBrother();
}

void XmlEditWidget::errorNoRule()
{
    d->errorNoRule();
}

void XmlEditWidget::onActionAddChildElement()
{
    d->onActionAddChildElement();
}

void XmlEditWidget::onActionAppendChildElement()
{
    d->onActionAppendChildElement();
}

void XmlEditWidget::onActionEdit()
{
    d->onActionEdit();
}

void XmlEditWidget::onActionDelete()
{
    d->onActionDelete();
}

void XmlEditWidget::onActionMoveUp()
{
    d->onActionMoveUp();
}

void XmlEditWidget::onActionMoveDown()
{
    d->onActionMoveDown();
}

void XmlEditWidget::setClipBoardItem(Element *pElement)
{
    d->setClipBoardItem(pElement);
}

void XmlEditWidget::onActionCut()
{
    d->onActionCut();
}

void XmlEditWidget::onActionCopy()
{
    d->onActionCopy();
}

void XmlEditWidget::onActionPaste()
{
    d->onActionPaste();
}

void XmlEditWidget::onActionPasteLastAttributes()
{
    d->onActionPasteLastAttributes();
}

void XmlEditWidget::actionCopyElementPathToClipboard()
{
    d->actionCopyElementPathToClipboard();
}

void XmlEditWidget::onActionExpandAll()
{
    d->onActionExpandAll();
}

void XmlEditWidget::onActionExpandSelectedItem()
{
    d->onActionExpandSelectedItem();
}


void XmlEditWidget::onActionShowAttrLine(const bool state)
{
    d->onActionShowAttrLine(state);
}

void XmlEditWidget::onActionAddComment()
{
    d->onActionAddComment();
}

void XmlEditWidget::onActionAppendComment()
{
    d->onActionAppendComment();
}

void XmlEditWidget::onActionAppendProcessingInstruction()
{
    d->onActionAppendProcessingInstruction();
}

void XmlEditWidget::onActionAddProcessingInstruction()
{
    d->onActionAddProcessingInstruction();
}

/* TODO: I dont think it is relevant anymore
void XmlEditWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}
*/

void XmlEditWidget::onActionFind()
{
    d->onActionFind();
}

void XmlEditWidget::onActionReplace()
{
    d->onActionReplace();
}

void XmlEditWidget::onActionGoToParent()
{
    d->onActionGoToParent();
}

void XmlEditWidget::onActionGoToPreviousBrother()
{
    d->onActionGoToPreviousBrother();
}

void XmlEditWidget::onActionGoToNextBrother()
{
    d->onActionGoToNextBrother();
}

void XmlEditWidget::onActionClearBookmarks()
{
    d->onActionClearBookmarks();
}

void XmlEditWidget::onActionToggleBookmark()
{
    d->onActionToggleBookmark();
}

void XmlEditWidget::onActionGotoNextBookmark()
{
    d->onActionGotoNextBookmark();
}

void XmlEditWidget::onActionGotoPreviousBookmark()
{
    d->onActionGotoPreviousBookmark();
}

void XmlEditWidget::onActionShowChildIndex(const bool isChecked)
{
    d->onActionShowChildIndex(isChecked);
}

void XmlEditWidget::onActionCompactView(const bool isChecked)
{
    d->onActionCompactView(isChecked);
}

void XmlEditWidget::onActionShowAlwaysFullTextComments(const bool isShow)
{
    d->onActionShowAlwaysFullTextComments(isShow);
}

void XmlEditWidget::onActionFixedSizeAttributes(const bool isChecked)
{
    d->onActionFixedSizeAttributes(isChecked);
}

void XmlEditWidget::onActionShowAttributesLength(const bool isChecked)
{
    d->onActionShowAttributesLength(isChecked);
}

void XmlEditWidget::onActionShowElementTextLength(const bool isChecked)
{
    d->onActionShowElementTextLength(isChecked);
}

void XmlEditWidget::onActionCloseThisAllBrothers()
{
    d->onActionCloseThisAllBrothers();
}


void XmlEditWidget::onActionHideBrothers()
{
    d->onActionHideBrothers();
}

void XmlEditWidget::onActionShowCurrentElementTextBase64(const bool isChecked)
{
    d->onActionShowCurrentElementTextBase64(isChecked);
}

void XmlEditWidget::onActionShowBase64(const bool isChecked)
{
    d->onActionShowBase64(isChecked);
}

void XmlEditWidget::onActionShowElementSize(const bool isChecked)
{
    d->onActionShowElementSize(isChecked);
}

void XmlEditWidget::onActionEditInnerXML()
{
    d->onActionEditInnerXML();
}

void XmlEditWidget::onActionEditInnerXMLBase64()
{
    d->onActionEditInnerXMLBase64();
}

void XmlEditWidget::onActionEditInnerBase64Text()
{
    d->onActionEditInnerBase64Text();
}

QString XmlEditWidget::getContentAsText()
{
    return d->getContentAsText();
}

bool XmlEditWidget::isCDATA()
{
    return d->isCDATA();
}

void XmlEditWidget::setCDATA(const bool isCData)
{
    d->setCDATA(isCData);
}

void XmlEditWidget::onActionZoomIn()
{
    d->onActionZoomIn();
}

void XmlEditWidget::onActionZoomOut()
{
    d->onActionZoomOut();
}
//da decidere se emette o torna un flag

void XmlEditWidget::onActionCopyPathToClipboard()
{
    d->onActionCopyPathToClipboard();
}

void XmlEditWidget::onActionValidate()
{
    d->onActionValidate();
}

void XmlEditWidget::validateWithFile(const QString &filePath)
{
    d->validateWithFile(filePath);
}

bool XmlEditWidget::isUndoPossible()
{
    return d->isUndoPossible();
}

bool XmlEditWidget::isRedoPossible()
{
    return d->isRedoPossible();
}

void XmlEditWidget::undo()
{
    d->undo();
}

void XmlEditWidget::redo()
{
    d->redo();
}

XSDSchema* XmlEditWidget::schema()
{
    return d->schema();
}

void XmlEditWidget::onActionValidateNewFile()
{
    d->onActionValidateNewFile();
}

void XmlEditWidget::insertSnippet(Regola *newRegola)
{
    d->insertSnippet(newRegola);
}

void XmlEditWidget::onActionDeleteBookmarked()
{
    d->onActionDeleteBookmarked();
}

void XmlEditWidget::onActionCleanBookmarkedContent()
{
    d->onActionCleanBookmarkedContent();
}

bool XmlEditWidget::isActionMode()
{
    return d->isActionMode();
}

bool XmlEditWidget::isActionNoScanMode()
{
    return d->isActionNoScanMode();
}

void XmlEditWidget::setDisplayMode(const qxmledit::EDisplayMode value)
{
    d->setDisplayMode(value);
}


/**
  changestate if the state is to be reset to normal after loading new document
  */
void XmlEditWidget::loadText(const QString &text, const bool isChangeState, const bool isAskForReview)
{
    d->loadText(text, isChangeState, isAskForReview);

}
/**
 * @brief assignRegola: take ownership of regola
 * @param regola
 * @param isChangeState
 */
void XmlEditWidget::assignRegola(Regola*regola,  const bool isChangeState)
{
    d->assignRegolaAndTakeOwnership(regola,  isChangeState);
}

//---------------------- load data

//TODO: error checking
void XmlEditWidget::doLoadFileXplore(const QString &filePath)
{
    d->doLoadFileXplore(filePath);
}

//TODO: error checking
void XmlEditWidget::setDocument(QDomDocument &document, const QString &filePath, const bool isSetState)
{
    d->setDocument(document, filePath, isSetState);
}

void XmlEditWidget::onActionHideView(const bool isChecked)
{
    d->onActionHideView(isChecked);
}

bool XmlEditWidget::isValidXsd()
{
    return d->isValidXsd();
}

XmlEditWidgetEditMode::EditMode XmlEditWidget::editMode()
{
    return d->editMode();
}

void XmlEditWidget::setEditMode(const XmlEditWidgetEditMode::EditMode newMode)
{
    d->setEditMode(newMode);
}

Regola *XmlEditWidget::getRegola()
{
    return d->getRegola();
}


void XmlEditWidget::onActionViewAsXsd()
{
    d->onActionViewAsXsd();
}

void XmlEditWidget::loadSchema(const QString &schemaURL)
{
    d->loadSchema(schemaURL);
}

void XmlEditWidget::setCurrentItem(Element *newSelection)
{
    d->setCurrentItem(newSelection);
}

void XmlEditWidget::onActionAllowedSchemaElements()
{
    d->onActionAllowedSchemaElements();
}

void XmlEditWidget::insertAllowedElements(Element *element)
{
    d->insertAllowedElements(element);
}

/** Substitute the element text
  */
void XmlEditWidget::onActionPasteAndSubstituteText()
{
    d->onActionPasteAndSubstituteText();
}

void XmlEditWidget::onActionNewUsingXMLSchema(const QString &schemaURL)
{
    d->onActionNewUsingXMLSchema(schemaURL);
}

void XmlEditWidget::doNew()
{
    d->doNew();
}

void XmlEditWidget::onActionTransformInComment()
{
    d->onActionTransformInComment();
}

void XmlEditWidget::onActionExtractElementsFromComment()
{
    d->onActionExtractElementsFromComment();
}

void XmlEditWidget::autoLoadValidation()
{
    d->autoLoadValidation();
}

void XmlEditWidget::onActionInsertNoNamespaceSchemaReferenceAttributes()
{
    d->onActionInsertNoNamespaceSchemaReferenceAttributes();
}

void XmlEditWidget::onActionInsertSchemaReferenceAttributes()
{
    d->onActionInsertSchemaReferenceAttributes();
}

void XmlEditWidget::setNavigationDataAndEnable(const int minFragment, const int maxFragment)
{
    d->setNavigationDataAndEnable(minFragment, maxFragment);
}

void XmlEditWidget::showNavigationBox()
{
    d->showNavigationBox();
}

void XmlEditWidget::error(const QString& message)
{
    Utils::error(this, message);
}

void XmlEditWidget::warning(const QString& message)
{
    Utils::warning(this, message);
}

void XmlEditWidget::message(const QString& message)
{
    Utils::message(this, message);
}

bool XmlEditWidget::askYN(const QString & message)
{
    return Utils::askYN(this, message);
}

QTreeWidget *XmlEditWidget::getMainTreeWidget()
{
    return d->getMainTreeWidget();
}

void XmlEditWidget::onActionHideAllLeafChildren()
{
    d->onActionHideAllLeafChildren();
}

Element* XmlEditWidget::onActionHideLeafChildren()
{
    return d->onActionHideLeafChildren();
}

Element* XmlEditWidget::onActionShowLeafChildren()
{
    return d->onActionShowLeafChildren();
}

void XmlEditWidget::onActionShowAllLeafChildren()
{
    d->onActionShowAllLeafChildren();
}

XmlEditWidgetPrivate *XmlEditWidget::getPrivate()
{
    return d;
}

void XmlEditWidget::onActionCompareXSD(const QString &folderPath)
{
    return d->onActionCompareXSD(folderPath);
}

void XmlEditWidget::onActionExportSiblingsAttributesAsCSVClipboard()
{
    d->onActionExportSiblingsAttributesAsCSVClipboard();
}

void XmlEditWidget::viewNodes()
{
    d->viewNodes();
}

void XmlEditWidget::addXsltMenuItems(QMenu *contextMenu)
{
    d->addXsltMenuItems(contextMenu);
}

void XmlEditWidget::editXSLTElement()
{
    d->editXSLTElement();
}

XsltHelper *XmlEditWidget::XSLTHelper()
{
    return d->XSLTHelper();
}

void XmlEditWidget::scanXMLTagsAndNamesXSLTAutocompletion()
{
    d->scanXMLTagsAndNamesXSLTAutocompletion();
}

void XmlEditWidget::showXSLNavigator()
{
    d->showXSLNavigator();
}

void XmlEditWidget::specificProperties()
{
    d->specificProperties();
}

void XmlEditWidget::setCopyPathAction(QAction* action)
{
    d->setCopyPathAction(action);
}

void XmlEditWidget::pasteAsSibling()
{
    d->pasteAsSibling();
}

void XmlEditWidget::setReadOnly(const bool newState)
{
    d->setReadOnly(newState);
}

bool XmlEditWidget::isReadOnly()
{
    return d->isReadOnly();
}

void XmlEditWidget::onActionInsertMetadata()
{
    d->onActionInsertMetadata();
}

void XmlEditWidget::onActionInsertCompleteMetadata()
{
    d->onActionInsertCompleteMetadata();
}

void XmlEditWidget::onActionEditMetadata()
{
    d->onActionEditMetadata();
}

bool XmlEditWidget::writeData(const QString &filePath)
{
    return d->writeData(filePath);
}

bool XmlEditWidget::insertProlog()
{
    return d->insertProlog();
}

QString XmlEditWidget::labelForEncoding(const QString &encoding)
{
    return tr("encoding:%1").arg(encoding);
}

bool XmlEditWidget::setEncoding(const QString &newEncoding)
{
    return d->setEncoding(newEncoding);
}

void XmlEditWidget::removeAllElements()
{
    d->removeAllElements();
}

void XmlEditWidget::showCodePages(QWidget *parent)
{
    d->showCodePages(parent);
}

void XmlEditWidget::invalidatePaintData()
{
    d->invalidatePaintData();
}

void XmlEditWidget::showToolbar(const bool how)
{
    d->showToolbar(how);
}

void XmlEditWidget::onInsertChildContainer()
{
    d->onInsertChildContainer();
}

void XmlEditWidget::onInsertParent()
{
    d->onInsertParent();
}

void XmlEditWidget::onRemoveParent()
{
    d->onRemoveParent();
}

bool XmlEditWidget::XSDApplyOperation(const ElementOp::Op op, XSDOperationParameters *params)
{
    return d->XSDApplyOperation(op, params);
}

void XmlEditWidget::onXSDInsertElement()
{
    d->onXSDInsertElement();
}

void XmlEditWidget::onXSDAppendElement()
{
    d->onXSDAppendElement();
}

void XmlEditWidget::onXSDInsertType()
{
    d->onXSDInsertType();
}

void XmlEditWidget::onXSDAppendType()
{
    d->onXSDAppendType();
}

void XmlEditWidget::onXSDInsertAttribute()
{
    d->onXSDInsertAttribute();
}

void XmlEditWidget::onXSDAppendAttribute()
{
    d->onXSDAppendAttribute();
}

void XmlEditWidget::onXSDModifyType()
{
    d->onXSDModifyType();
}

bool XmlEditWidget::onEditXSDAnnotation()
{
    return d->onEditXSDAnnotation();
}

void XmlEditWidget::setOrigDataForAnonPreview(QHash<void *, QString> *newOrigData)
{
    d->setOrigDataForAnonPreview(newOrigData);
}

void XmlEditWidget::setXSDAnnotationEditProviderObject(XSDAnnotationEditProvider *newProvider)
{
    d->setXSDAnnotationEditProviderObject(newProvider);
}

bool XmlEditWidget::replaceAll(ReplaceTextParams *params)
{
    return d->replaceAll(params);
}

bool XmlEditWidget::replace(ReplaceTextParams * findArgs)
{
    return d->replace(findArgs);
}

void XmlEditWidget::onFindNext()
{
    d->onFindNext();
}

void XmlEditWidget::onSetIndent()
{
    d->onSetIndent();
}
