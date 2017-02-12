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

#include "regoladefinitions.h"
#include "undo/elupdateelementcommand.h"
#include "undo/undosimpleeditcommand.h"
#include "undo/undodtd.h"
#include "xmlsavecontext.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/xml/elmpath.h"

//-----

bool RegolaSettings::isUseIndent() const
{
    return useIndent;
}

void RegolaSettings::setUseIndent(bool value)
{
    useIndent = value;
}

int RegolaSettings::getIndentation() const
{
    return indentation;
}

void RegolaSettings::setIndentation(int value)
{
    indentation = value;
}

//-----

DocumentDeviceProvider::~DocumentDeviceProvider()
{
}

//-----

QBrush Regola::hiliteBkBrush(QColor(0xFF, 0xC0, 0x40));

const QString Regola::XsltNameSpace = "http://www.w3.org/1999/XSL/Transform" ;
const QString Regola::XSDNameSpace = "http://www.w3.org/2001/XMLSchema" ;
const QString Regola::XSDSchemaInstance = "http://www.w3.org/2001/XMLSchema-instance";

Regola::Regola(QDomDocument &document, const QString &name, const bool useMixedContent)
{
    housework();
    _useMixedContent = useMixedContent ;
    modified = false;
    _collectSizeData = false ;
    xmlFileName = name ;
    // Assegna i valori.
    processDocument(document);
    rootItem = assegnaValori(document, NULL, &childItems);
    checkEncoding(true);
    checkValidationReference();
    //dumpDocument(document);
}

Regola::Regola(const QString &name, const bool useMixedContent)
{
    housework();
    _useMixedContent = useMixedContent ;
    modified = false;
    _collectSizeData = false ;
    xmlFileName = name ;
    // Assegna i valori.
    rootItem = NULL;
}

Regola::Regola()
{
    housework();
    bookmarks.clear();
    modified = false;
    rootItem = NULL;
    _collectSizeData = false ;
}

Regola::~Regola()
{
    bookmarks.clear();
    clear();
    if(NULL != _docType) {
        delete _docType;
    }
    if(_ownPaintInfo) {
        if(NULL != paintInfo) {
            delete paintInfo;
        }
    }
}

void Regola::housework()
{
    paintInfo = new PaintInfo() ;
    _ownPaintInfo = true ;
    _forceDOM = false;
    _attributesIndentSettings = false;
    _indentAttributes = QXmlEditData::XmlIndentAttributesTypeDefault;
    _indentAttributesColumns = QXmlEditData::XmlIndentAttributesColumnsDefault ;
    _saveAttributesMethod = SaveAttributesUsingDefault ;
    _useNoNamespaceXSD = false;
    _useNamespaceXSD = false ;
    _namespaceManager = NULL ;
    _useIndent = false;
    _indent = QXmlEditData::XmlIndentDefault ;
    _deviceProvider = NULL ;
    _isCrapCacheNSActivated = false;
    _useMixedContent = false ;
    _undoStack.setUndoLimit(UndoLimitCount);
    _editHook = NULL ;
    _editTextHook = NULL ;
    connect(&_undoStack, SIGNAL(canRedoChanged(bool)), this, SIGNAL(undoStateChanged()));
    connect(&_undoStack, SIGNAL(canUndoChanged(bool)), this, SIGNAL(undoStateChanged()));
    _docType = new DocumentType();
    _originalEncoding = DefaultEncoding ;
}

void Regola::clear()
{
    clearUndo();
    selection.clear();
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        delete it.next();
    }
    rootItem = NULL ;
    modified = false;
}

void Regola::setDeviceProvider(DocumentDeviceProvider * value)
{
    _deviceProvider = value ;
}

Regola *Regola::loadFromFile(const QString &fileName, QString *errorMessage)
{
    Regola *regola = NULL ;
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        regola  = Regola::loadFromDevice(&file, fileName, errorMessage);
        file.close();
    } else {
        *errorMessage = QString(tr("Unable to load file '%1'. Error code is '%2'")).arg(fileName).arg(file.error());
    }
    return regola;
}

Regola *Regola::loadFromDevice(QIODevice *ioDevice, const QString &fileName, QString *errorMessage)
{
    Regola *regola = NULL ;
    QDomDocument document;
    QString errorMsg ;
    int errorLine = 0, errorColumn = 0;
    if(document.setContent(ioDevice, &errorMsg, &errorLine, &errorColumn)) {
        regola = new Regola(document, fileName);
    } else {
        *errorMessage = QString("Error loading file '%4', msg:'%1', line '%2', column:'%3'").arg(errorMsg).arg(errorLine).arg(errorColumn).arg(fileName);
    }
    return regola;
}


bool Regola::isUseMixedContent()
{
    return _useMixedContent ;
}

void Regola::setUseMixedContent(const bool value)
{
    _useMixedContent = value ;
}

void Regola::clearUndo()
{
    _undoStack.clear();
}

int Regola::indentation() const
{
    return _indent;
}

void Regola::setIndentationForce(const int indent)
{
    _indent = indent;
    emitIndentationChange();
}

bool Regola::useIndentation() const
{
    return _useIndent;
}

void Regola::setUseIndentation(const bool value)
{
    _useIndent = value;
}

void Regola::setIndentation(const int value)
{
    if(!_useIndent) {
        _indent = value;
        emitIndentationChange();
    }
}

void Regola::emitIndentationChange()
{
    emit indentationChanged((_indent >= 0), _indent);
}

void Regola::caricaValori(QTreeWidget *pTree)
{
    pTree->clear();
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element *el = it.next();
        el->registerState();
        el->caricaFigli(pTree, NULL, paintInfo, true);
    }
}

void Regola::redisplay()
{
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element *el = it.next();
        el->displayRecursive(paintInfo);
    }
}

void Regola::markSaved()
{
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element *el = it.next();
        el->markSavedRecursive();
    }
}

void Regola::markEdited()
{
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element *el = it.next();
        el->markEditedRecursive();
    }
}

void Regola::assignMixedContentText(Element *parent, const QString &text, const bool isCData, QVector<Element*> *collection)
{
    Element *textElement = new Element(this, Element::ET_TEXT, parent) ;
    textElement->setTextOfTextNode(text, isCData);
    collection->append(textElement);
}

bool Regola::write(const QString &filePath)
{
    return write(filePath, true);
}

QDomDocument Regola::createNewDocument()
{
    if(_docType->hasDocType()) {
        QDomDocument  document = _docType->createDocument();
        return document;
    } else {
        QDomDocument    document;
        return document;
    }
}

bool Regola::isEncodingCompatibleWithStream()
{
    QString theEncoding = encoding();
    bool isEncoding8bitNotASCII = Utils::isEncoding8bitNotASCII(theEncoding);
    if(isEncoding8bitNotASCII) {
        bool is8BitEncodingHonored = Utils::is8BitEncodingHonoredForStreamWriter(theEncoding);
        return is8BitEncodingHonored ;
    } // if honored
    return true ;
}

bool Regola::isUseStreamForSaving()
{
    if(!isForceDOM()) {
        return true ;
    }
    return false;
}

bool Regola::write(QIODevice *device, const bool isMarkSaved)
{
    if(isUseStreamForSaving()) {
        return writeStream(device, isMarkSaved);
    }

    //QDomImplementation::setInvalidDataPolicy(QDomImplementation::AcceptInvalidChars);
    QDomDocument    document = createNewDocument();
    //document.setInvalidDataPolicy(QDomImplementation::ReturnNullNode); TODO
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        if(!it.next()->generateDom(document, document)) {
            return false;
        }
    }

    if(!device->open(QIODevice::WriteOnly | (Utils::isEncoding8bitNotASCII(encoding()) ? (QIODevice::OpenModeFlag)0 : QIODevice::Text))) {
        Utils::error(tr("Error writing data: %1").arg(device->errorString()));
        return false;
    }

    QString theEncoding = encoding();

    QTextStream streamOut(device);
    streamOut.setCodec(theEncoding.toLatin1().data());
    streamOut << document.toString(_indent);
    streamOut.flush();
    device->close();
    if(isMarkSaved) {
        foreach(Element * ep, childItems) {
            ep->markSavedRecursive();
        }
    }
    // TODO: repaint only if modified, and only if visible
    redisplay();

    return true;
}

bool Regola::writeStreamInternal(QIODevice *device, const bool useEncoding, ElementLoadInfoMap *map)
{
    if(!device->isOpen()) {
        // open the stream in binary, since the codec will take care of everything.
        if(!device->open(QIODevice::WriteOnly)) {
            Utils::error(tr("Error writing data: %1").arg(device->errorString()));
            return false;
        }
    }

    QXmlStreamWriter outputStream(device);
    XMLSaveContext context;
    context.setIndentation(_indent);
    context.setIsSortAttributesAlpha(isSavingSortingAttributes());
    context.setAttributesMaxColumns(xmlIndentAttributesColumns());
    context.setIsAttributesColumns(xmlIndentAttributesType() == QXmlEditData::AttributesIndentationMaxCols);

    outputStream.setAutoFormatting(false);
    outputStream.setAutoFormattingIndent(_indent);
    QString theEncoding = encoding();
    if(useEncoding) {
        outputStream.setCodec(theEncoding.toLatin1().data());
    } else {
        outputStream.setCodec("UTF-8"); // this should be the default anyway
    }
    context.setCodec(outputStream.codec());
    //Se monobyte, usa text mode e crlf, se multibyte, solo \n. Questo a causa di un bug nello streamwritew.
    // il mio codice inserisce crlf, quello di libreria, no.
    device->setTextModeEnabled(context.canUseTextMode());
    int index = 0 ;
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element* child = it.next();
        if(!child->writeStream(&context, outputStream, map)) {
            return false;
        }
        if((0 == index) && (child->getType() == Element::ET_PROCESSING_INSTRUCTION) && (child->getPITarget() == "xml")) {
            if(!_docType->dtd().isEmpty()) {
                outputStream.writeDTD(_docType->dtd());
                context.setIsDTDWritten(true);
            }

        }
        index ++ ;
    }
    if((_indent >= 0) && !childItems.isEmpty()) {
        outputStream.writeCharacters("\n");
    }
    device->close();

    return true;
}

bool Regola::writeStream(QIODevice *device, const bool isMarkSaved, ElementLoadInfoMap *map)
{
    if(!device->open(QIODevice::WriteOnly)) {
        Utils::error(tr("Error writing data: %1").arg(device->errorString()));
        return false;
    }

    Utils::TODO_NEXT_RELEASE("fare doctype");
    bool result = writeStreamInternal(device, true, map);
    if(result) {
        if(isMarkSaved) {
            foreach(Element * ep, childItems) {
                ep->markSavedRecursive();
            }
        }
        // TODO: repaint only if modified, and only if visible
        redisplay();
    }
    return result;
}

bool Regola::exportElement(const QString &filePath, EExportOptions options, Element *selected)
{
    QFile   file(filePath);
    QIODevice *outDevice = NULL ;
    if(NULL != _deviceProvider) {
        outDevice = _deviceProvider->newDeviceForWrite(filePath);
    } else {
        outDevice = &file ;
    }
    return writeStreamElement(outDevice, options, selected);
}

bool Regola::exportElement(QIODevice *outDevice, EExportOptions options, Element *selected)
{
    return writeStreamElement(outDevice, options, selected);
}

bool Regola::writeStreamElement(QIODevice *device, EExportOptions options, Element *selected)
{
    if(!device->open(QIODevice::WriteOnly | QIODevice::Text)) {
        Utils::error(tr("Error writing data: %1").arg(device->errorString()));
        return false;
    }
    bool result = writeStreamInternalElement(device, options, selected);
    return result;
}

bool Regola::writeStreamInternalElement(QIODevice *device, EExportOptions options, Element *selected)
{
    if(!device->isOpen()) {
        if(!device->open(QIODevice::WriteOnly | QIODevice::Text)) {
            Utils::error(tr("Error writing data: %1").arg(device->errorString()));
            return false;
        }
    }

    QXmlStreamWriter outputStream(device);
    XMLSaveContext context;
    context.setIndentation(_indent);
    context.setIsSortAttributesAlpha(isSavingSortingAttributes());
    context.setAttributesMaxColumns(xmlIndentAttributesColumns());
    context.setIsAttributesColumns(xmlIndentAttributesType() == QXmlEditData::AttributesIndentationMaxCols);

    outputStream.setAutoFormatting(false);
    outputStream.setAutoFormattingIndent(_indent);
    QString theEncoding = encoding();
    outputStream.setCodec(theEncoding.toLatin1().data());

    Element *first = childItems.size() > 0 ? childItems.at(0) : NULL;
    if(options.testFlag(ExportOptionUseDeclaration)
            && (NULL != first)
            && (first->getType() == Element::ET_PROCESSING_INSTRUCTION) && (first->getPITarget() == "xml")) {
        if(!first->writeStream(&context, outputStream, NULL)) {
            return false;
        }
    }
    if(options.testFlag(ExportOptionUseNamespace)) {
        // build namespace map
        ElmPathResolver resolver;
        resolver.collectParentNamespaces(selected, context.namespaceDeclarationsReference());
        // set map
        // assign it to the element
    }

    if(!selected->writeStream(&context, outputStream, NULL)) {
        return false;
    }
    if((_indent >= 0) && !childItems.isEmpty()) {
        outputStream.writeCharacters("\n");
    }

    device->close();

    return true;
}

bool Regola::write(const QString &filePath, const bool isMarkSaved)
{
    QFile   file(filePath);
    QIODevice *outDevice = NULL ;
    if(NULL != _deviceProvider) {
        outDevice = _deviceProvider->newDeviceForWrite(filePath);
    } else {
        outDevice = &file ;
    }
    return write(outDevice, isMarkSaved);
}

QByteArray Regola::writeMemory()
{
    QByteArray byteArray ;
    QBuffer buffer(&byteArray);
    write(&buffer, false);
    return byteArray;
}

QString Regola::getAsText()
{
    return getAsText(NULL);
}

QString Regola::getAsText(ElementLoadInfoMap *map)
{
    return getAsTextStream(map);
}

QString Regola::getAsTextStream(ElementLoadInfoMap *map)
{
    QBuffer buffer ;
    bool result = writeStreamInternal(&buffer, true, map);
    if(result) {
        return QString::fromUtf8(buffer.data());
    } else {
        return "";
    }
}


bool Regola::isEmpty(const bool isRealElement)
{
    if(isRealElement) {
        return (NULL == rootItem) ? true : false ;
    } else {
        //any element?
        return (childItems.isEmpty()) ? true : false ;
    }
}

Element  *Regola::root() const
{
    return rootItem;
}

Element *Regola::firstChild() const
{
    if(childItems.isEmpty()) {
        return NULL ;
    }
    return childItems.first();
}

Element *Regola::lastChild() const
{
    if(childItems.isEmpty()) {
        return NULL ;
    }
    return childItems.last();
}

Element *Regola::firstChildRecursive() const
{
    if(childItems.isEmpty()) {
        return NULL ;
    }
    return childItems.first()->firstChildRecursiveOrThis();
}

Element *Regola::lastChildRecursive() const
{
    if(childItems.isEmpty()) {
        return NULL ;
    }
    return childItems.last()->lastChildRecursiveOrThis();
}

Element* Regola::topElement(const int pos) const
{
    if((pos >= 0) && (pos < childItems.size())) {
        return childItems.at(pos);
    }
    return NULL ;
}


void Regola::setRootElement(Element *newRoot)
{
    bookmarks.clear();
    clear();
    addTopElement(newRoot);
}

bool Regola::addTopElement(Element *theNewElement, const int position)
{
    bool wasExisted = false;
    bool newIsElement = (theNewElement->getType() == Element::ET_ELEMENT) ;
#ifndef QT_NO_DEBUG
    bool isOutOfSync = (NULL != rootItem) && (rootItem->getType() == Element::ET_ELEMENT) && newIsElement ;

    Q_ASSERT_X(!isOutOfSync, "setFirstChild", "rootItem is out of sync");
#endif
    if(newIsElement) {
        rootItem = theNewElement ;
    }
    bool isCheckEncoding = false ;
    if(-1 == position) {
        if(childItems.isEmpty()) {
            isCheckEncoding = true ;
        }
        childItems.append(theNewElement);
    } else {
        childItems.insert(position, theNewElement);
        if(0 == position) {
            isCheckEncoding = true ;
        }
    }
    if(collectSizeData()) {
        theNewElement->recalcSize(true);
    }
    if(isCheckEncoding) {
        checkEncoding();
    }
    setModified(true);
    return wasExisted ;
    Utils::TODO_NEXT_RELEASE("check refactor con void Regola::addTopElement(Element *theNewElement)");
}

Element *Regola::newElement()
{
    Element *newElement = new Element(this);
    return newElement;
}

Element *Regola::newElement(const QString &tagName)
{
    Element *newElement = new Element(this);
    newElement->_tag = addNameToPool(tagName);
    return newElement;
}

Element *Regola::newElement(const Element::ElType newType)
{
    Element *newElement = new Element(this, newType, NULL);
    return newElement;
}

void Regola::setModified(const bool state)
{
    bool stateChanged = false;
    if(state != modified) {
        stateChanged = true ;
        modified = state ;
        bookmarks.setModified();
        checkValidationReference();
    }
    if(state || stateChanged) {
        emit wasModified();
    }
}

bool Regola::isModified() const
{
    return modified ;
}

void Regola::setEmptyRoot()
{
    clear();
}

QString &Regola::fileName()
{
    return xmlFileName;
}

void Regola::setFileName(const QString &newFileName)
{
    xmlFileName = newFileName;
    setModified(true);
}

bool Regola::editNodeElementAsXML(const bool isBase64Coded, Element *pElement, UIDelegate *uiDelegate)
{
    bool isOk = false;
    bool isCDataOut = false;
    if(NULL == uiDelegate) {
        return false;
    }
    QString text = uiDelegate->editNodeElementAsXML(isBase64Coded, pElement, pElement->getAsSimpleTextXml(isBase64Coded), pElement->isCDATA(), isCDataOut, isOk);
    if(isOk) {
        pElement->setAsSingleTextNode(text, isBase64Coded, isCDataOut) ;
    }
    return isOk;
}

bool Regola::editTextNodeElement(QWidget *const parentWindow, const bool isBase64Coded, Element *pElement)
{
    EditTextNode editDialog(isBase64Coded, fileName(), parentWindow);
    editDialog.setWindowModality(Qt::WindowModal);
    editDialog.setText(pElement->getAsSimpleText(isBase64Coded));
    if(editDialog.exec() == QDialog::Accepted) {
        pElement->setAsSingleTextNode(editDialog.getText(), isBase64Coded, pElement->isCDATA()) ;
        return true;
    }
    return false;
}

bool Regola::editNodeElement(QWidget *const parentWindow, Element *pElement, Element *parentElement, const bool enableAllControls)
{
    EditElement element(parentWindow);
    element.setWindowModality(Qt::WindowModal);
    element.setWindowModality(Qt::WindowModal);
    element.setTarget(pElement, parentElement);
    element.setNamespaceManager(_namespaceManager);
    element.setEnableAllControls(enableAllControls);
    if(element.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

bool Regola::editNodeComment(QWidget * const parentWindow, Element *pElement)
{
    return EditCommentNode(parentWindow, pElement);
}


bool Regola::editInnerXMLElement(QTreeWidgetItem *item, UIDelegate *uiDelegate)
{
    Element *pElement = Element::fromItemData(item);
    switch(pElement->getType()) {
    case Element::ET_ELEMENT:
        if(pElement->isMixedContent()) {
            if(NULL != uiDelegate) {
                uiDelegate->error(textOfCantEditMixedContentElementText());
            }
            return false;
        }
    // pass through
    case Element::ET_TEXT: {
        UndoEditCommand *undoCommand = new UndoEditCommand(item->treeWidget(), this, pElement->indexPath());
        if(NULL == undoCommand) {
            Utils::errorOutOfMem(uiDelegate->getMainWidget());
            throw new std::exception();
            return false;
        }
        undoCommand->setOriginalElement(pElement);
        bool result = false;
        if(NULL != _editTextHook) {
            uiDelegate->error(tr("Error editing the element.")); // this stmt in test will be discarded
            result = _editTextHook(item->treeWidget()->window(), item, uiDelegate, pElement);
        } else {
            result = editNodeElementAsXML(false, pElement, uiDelegate);
        }
        if(result) {
            // Aggiorna la visualizzazione
            //se nuovo figlio, o nuovo fratello
            //setta il nuovo valore
            pElement->updateSizeInfo();
            pElement->display(item, paintInfo);
            setModified(true);
            undoCommand->setModifiedElement(pElement);
            _undoStack.push(undoCommand);
            return true ;
        } else {
            delete undoCommand;
            return false ;
        }
    }
    break;
    default:
        return false;
    }
    return false;
}

bool Regola::editInnerXMLBase64Element(QTreeWidgetItem *item, UIDelegate *uiDelegate)
{
    Element *pElement = Element::fromItemData(item);
    switch(pElement->getType()) {
    case Element::ET_ELEMENT:
        if(pElement->isMixedContent()) {
            if(NULL == uiDelegate) {
                uiDelegate->error(textOfCantEditMixedContentElementText());
            }
            return false;
        }
    // pass through
    case Element::ET_TEXT: {
        UndoEditCommand *undoCommand = new UndoEditCommand(item->treeWidget(), this, pElement->indexPath());
        if(NULL == undoCommand) {
            throw new std::exception();
            return false;
        }
        undoCommand->setOriginalElement(pElement);
        bool result = false;
        if(NULL != _editTextHook) {
            uiDelegate->error(tr("Error editing the element.")); // this stmt in test will be discarded
            result = _editTextHook(item->treeWidget()->window(), item, uiDelegate, pElement);
        } else {
            result = editNodeElementAsXML(true, pElement, uiDelegate);
        }
        if(result) {
            // Aggiorna la visualizzazione
            //se nuovo figlio, o nuovo fratello
            //setta il nuovo valore
            pElement->updateSizeInfo();
            pElement->display(item, paintInfo);
            setModified(true);
            undoCommand->setModifiedElement(pElement);
            _undoStack.push(undoCommand);
            return true ;
        } else {
            delete undoCommand;
            return false ;
        }
    }
    break;
    default:
        return false;
    }
    return false;
}

bool Regola::editTextNodeElementBase64(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate)
{
    Element *pElement = Element::fromItemData(item);
    switch(pElement->getType()) {
    case Element::ET_ELEMENT:
        if(pElement->isMixedContent()) {
            if(NULL != uiDelegate) {
                uiDelegate->error(textOfCantEditMixedContentElementText());
            }
            return false;
        }
    // pass through
    case Element::ET_TEXT: {
        UndoEditCommand *undoCommand = new UndoEditCommand(item->treeWidget(), this, pElement->indexPath());
        if(NULL == undoCommand) {
            throw new std::exception();
            return false;
        }
        undoCommand->setOriginalElement(pElement);
        bool result = false;
        if(NULL != _editTextHook) {
            uiDelegate->error(tr("Error editing the element.")); // this stmt in test will be discarded
            result = _editTextHook(parentWindow, item, uiDelegate, pElement);
        } else {
            result = editTextNodeElement(parentWindow, true, pElement);
        }
        if(result) {
            pElement->updateSizeInfo();
            pElement->display(item, paintInfo);
            setModified(true);
            undoCommand->setModifiedElement(pElement);
            _undoStack.push(undoCommand);
            return true ;
        } else {
            delete undoCommand;
            return false ;
        }
    }
    break;
    default:
        return false;
    }
    return false;
}


/**
  * substitute all the text of an element
  */
bool Regola::editAndSubstituteTextInNodeElement(QWidget *const parentWindow, Element *pElement, UIDelegate *uiDelegate)
{
    UndoEditCommand *undoCommand = new UndoEditCommand(pElement->getUI()->treeWidget(), this, pElement->indexPath());
    if(NULL == undoCommand) {
        throw new std::exception();
        return false;
    }
    undoCommand->setOriginalElement(pElement);
    bool result = false;
    if(NULL != _editTextHook) {
        uiDelegate->error(tr("Error editing the element.")); // this stmt in test will be discarded
        result = _editTextHook(parentWindow, pElement->getUI(), uiDelegate, pElement);
    } else {
        result = editAndSubstituteTextInNodeElementInternal(parentWindow, pElement, uiDelegate) ;
    }
    if(result) {
        undoCommand->setModifiedElement(pElement);
        _undoStack.push(undoCommand);
        return true ;
    } else {
        delete undoCommand;
        return false ;
    }
}

bool Regola::editAndSubstituteTextInNodeElementInternal(QWidget *const parentWindow, Element *pElement, UIDelegate *uiDelegate)
{
    switch(pElement->getType()) {
    case Element::ET_ELEMENT:
        if(pElement->isMixedContent()) {
            if(NULL != uiDelegate) {
                uiDelegate->error(textOfCantEditMixedContentElementText());
            }
            return false;
        }
    // pass through
    case Element::ET_TEXT:
        if(editTextNodeElement(parentWindow, false, pElement)) {
            pElement->updateSizeInfo();
            pElement->display(pElement->getUI(), paintInfo);
            setModified(true);
            return true;
        }
        break;
    default:
        return false;
    }
    return false;
}


bool Regola::editElement(QWidget *const parentWindow, QTreeWidgetItem *item, UIDelegate *uiDelegate)
{
    try {
        bool result = false;
        bool updateInfo = false;
        Element *pElement = Element::fromItemData(item);
        int beforeAttributesCount = pElement->getAttributesList().size();
        UndoEditCommand *undoCommand = new UndoEditCommand(item->treeWidget(), this, pElement->indexPath());
        if(NULL == undoCommand) {
            Utils::errorOutOfMem(uiDelegate->getMainWidget());
            throw new std::exception();
            return false;
        }
        undoCommand->setOriginalElement(pElement);

        if(NULL != _editHook) {
            uiDelegate->error(tr("Error editing the element.")); // this stmt in test will be discarded
            result = _editHook(parentWindow, item, uiDelegate, pElement);
            if(result) {
                updateInfo = true ;
            }
        } else {

            switch(pElement->getType()) {
            case Element::ET_TEXT:
                result = editAndSubstituteTextInNodeElementInternal(parentWindow, pElement, uiDelegate);
                break;

            case Element::ET_ELEMENT:
                result = editNodeElement(parentWindow, pElement, pElement->parent());
                if(result) {
                    updateInfo = true ;
                }
                break;

            case Element::ET_PROCESSING_INSTRUCTION:
                result = ::editProcessingInstruction(parentWindow, pElement);
                if(result) {
                    updateInfo = true ;
                }
                break;
            case Element::ET_COMMENT:
                result = editNodeComment(parentWindow, pElement);
                if(result) {
                    updateInfo = true ;
                }
                break;

            default:
                Utils::error(parentWindow, tr("Unknown node type"));
                break;
            }
        }
        if(result) {
            if(updateInfo) {
                pElement->updateSizeInfo();
                pElement->display(item, paintInfo);
                int nowAttributesCount = pElement->getAttributesList().size();
                if(pElement->isElement() && (nowAttributesCount != beforeAttributesCount)) {
                    pElement->forceUpdateGui(true);
                }
                setModified(true);
            }
            if(NULL == pElement->parent()) {
                checkEncoding();
            }
            undoCommand->setModifiedElement(pElement);
            _undoStack.push(undoCommand);
        } else {
            delete undoCommand;
        }
        return result ;
    } catch(...) {
        Utils::error(parentWindow, tr("A fatal error occurred"));
    }
    return false ;
}

bool Regola::editEntry(QWidget * const parentWindow, const QString &title, const QString &label, const QString &actualText, QString &result)
{
    bool ok = false ;
    QString text = QInputDialog::getText(parentWindow, title, label, QLineEdit::Normal, actualText, &ok);
    if(ok && !text.isEmpty()) {
        result = text ;
    }
    return ok;
}

void Regola::addChild(QWidget *window, QTreeWidget *tree)
{
    addChild(window, tree, NULL);
}

// if empty, add, else add only to element
void Regola::addChild(QWidget *window, QTreeWidget *tree, Element *preElement)
{
    QTreeWidgetItem *currItem = getSelItem(tree);
    bool isEmptyE = isEmpty(true);
    Element *parentElement = NULL ;
    if(NULL != currItem) {
        parentElement = Element::fromItemData(currItem);
        if(parentElement->getType() != Element::ET_ELEMENT) {
            //special case: selection is at top and no root element exists
            if(isEmptyE && (NULL == parentElement->parent())) {
                addBrother(window, tree);
                return ;
            }
            return ;
        }
    } else {
        if(!isEmptyE) {
            Utils::errorNoSel(window);
            return;
        }
    }
    Element *theNewElement = preElement ;
    if(NULL == theNewElement) {
        theNewElement = newElement();
        if(!editNodeElement(window, theNewElement, isEmptyE ? NULL : parentElement)) {
            delete theNewElement;
            theNewElement = NULL ;
        }
    }
    if(NULL != theNewElement) {
        insertElementInternal(theNewElement, isEmptyE ? NULL : parentElement, tree);
    }
}

// TODO: check for the root item
Element* Regola::addChildToElement(QWidget *window, QTreeWidget *tree, Element *parentElement, const QString &elementTag, const bool useUndo)
{
    bool isEmptyE = isEmpty(true);
    if(NULL != parentElement) {
        if(parentElement->getType() != Element::ET_ELEMENT) {
            return NULL ;
        }
    } else {
        if(!isEmptyE) {
            Utils::errorNoSel(window);
            return NULL ;
        }
    }
    Element *theNewElement = new Element(addNameToPool(elementTag), "", this, parentElement);
    if(NULL != theNewElement) {
        insertElementInternal(theNewElement, isEmptyE ? NULL : parentElement, tree, useUndo);
    }
    return theNewElement ;
}

QTreeWidgetItem *Regola::getSelItem(QTreeWidget *tree)
{
    QList<QTreeWidgetItem *>selItems =  tree->selectedItems();
    if(selItems.count() < 1) {
        return NULL;
    }
    return selItems.at(0);
}

void Regola::addBrother(QWidget *window, QTreeWidget *tree)
{
    addBrother(window, tree, NULL);
}

// added for testability
void Regola::addBrother(QWidget *window, QTreeWidget *tree, Element* aNewElement)
{
    QTreeWidgetItem *currItem = getSelItem(tree);
    bool isEmptyE = isEmpty(true);
    // Regola vuota, aggiungo il primo ed ultimo
    if(!isEmptyE && (NULL == currItem)) {
        Utils::errorNoSel(window);
        return;
    }
    // Check the root of the tree.
    Element *brotherElement = Element::fromItemData(currItem);

    if(!isEmptyE && (NULL != currItem)) {
        if((currItem->parent() == NULL) && (NULL != root())) {
            Utils::error(window, tr("Only one root item in permitted in XML syntax."));
            return;
        }
    }
    bool canGo = false;
    Element *theNewElement = NULL;
    if(NULL == aNewElement) {
        theNewElement = newElement();
        if(editNodeElement(window, theNewElement, (NULL == brotherElement) ? NULL : brotherElement->parent())) {
            canGo = true;
        }
    } else {
        canGo = true ;
        theNewElement = aNewElement ;
    }
    if(canGo) {
        appendElementInternal(theNewElement, brotherElement, tree);
    } else {
        if(NULL != theNewElement) {
            delete theNewElement;
        }
    }
}

void Regola::paste(QTreeWidget *tree, QList<Element*> &pasteElements)
{
    // lo inserisce come figlio dell'item corrente
    // else as top element if NULL
    QTreeWidgetItem *currItem = getSelItem(tree);
    Element *parentElement = Element::fromItemData(currItem);
    foreach(Element * pasteElement, pasteElements) {
        pasteInternals(tree, parentElement, pasteElement, -1);
    }
}

void Regola::paste(QTreeWidget *tree, Element *pasteElement)
{
    QList<Element*> list ;
    list << pasteElement ;
    paste(tree, list);
}

void Regola::pasteAsSibling(QWidget *window, QTreeWidget *tree, Element *pasteElement)
{
    QList<Element*> list ;
    list << pasteElement ;
    pasteAsSibling(window, tree, list);
}

void Regola::pasteAsSibling(QWidget *window, QTreeWidget *tree, QList<Element*> &pasteElements)
{
    // lo inserisce come figlio dell'item corrente
    QTreeWidgetItem *currItem = getSelItem(tree);
    if(NULL == currItem) {
        Utils::errorNoSel(window);
        return;
    }
    Element *currentElement = Element::fromItemData(currItem);
    if(NULL == currentElement) {
        return ;
    }
    Element *parentElement = currentElement->parent();
    int position = currentElement->indexOfSelfAsChild();
    foreach(Element * pasteElement, pasteElements) {
        if(NULL != pasteElement) {
            if(NULL == parentElement) {
                if((NULL != root()) && pasteElement->isElement()) {
                    Utils::error(tr("It is not possible to place another element as root."));
                    return;
                }
            }
            pasteInternals(tree, parentElement, pasteElement, position + 1);
            position++;
        }
    }
}

void Regola::pasteInternals(QTreeWidget *tree, Element *parentElement, Element *pasteElement, const int position)
{
    //if not sel, at the root iif rule is empty
    //sse extists an item
    //append l'item ( o lo inserisce, con tutti i figli e poi ricarica la lista)
    Element *theNewElement = NULL ;
    if(NULL == pasteElement) {
        return ;
    }
    if(isEmpty(true) || (NULL == parentElement)) {
        theNewElement = pasteElement->copyTo(*new Element(this));
        addTopElement(theNewElement, position);
        theNewElement->caricaFigli(tree, NULL, paintInfo, true, -1);
    } else {
        theNewElement = pasteElement->copyTo(*new Element(this));
        if(!parentElement->isElement()) {
            return ;
        }
        if(-1 == position) {
            parentElement->addChild(theNewElement);
        } else {
            parentElement->addChildAt(theNewElement, position);
        }
        theNewElement->caricaFigli(tree, parentElement->getUI(), paintInfo, true, position);
    }
    theNewElement->markEditedRecursive();
    addUndoInsert(tree, theNewElement);
    setModified(true);
}

Element *Regola::attachElementAt(QTreeWidget *tree, Element *parentElement, Element *attachedElement, const int position)
{
    //if not sel, at the root iif rule is empty
    //sse extists an item
    //append l'item ( o lo inserisce, con tutti i figli e poi ricarica la lista)
    if(NULL == attachedElement) {
        return NULL ;
    }
    attachedElement->setRegola(this, true);
    if(NULL == parentElement) {
        addTopElement(attachedElement, position);
        attachedElement->caricaFigli(tree, NULL, paintInfo, true, -1);
    } else {
        // serve???
        if(!parentElement->isElement()) {
            delete attachedElement ;
            return NULL ;
        }
        if(-1 == position) {
            parentElement->addChild(attachedElement);
        } else {
            parentElement->addChildAt(attachedElement, position);
        }
        attachedElement->caricaFigli(tree, parentElement->getUI(), paintInfo, true, position);
    }
    attachedElement->markEditedRecursive();
    setModified(true);
    return attachedElement ;
}

void Regola::pasteNoUI(Element *pasteElement, Element *pasteTo)
{
    Element *theNewElement = NULL ;
    if(NULL == pasteElement) {
        return ;
    }
    if(NULL == pasteTo) {
        theNewElement = pasteElement->copyTo(*new Element(this));
        addTopElement(theNewElement);
        theNewElement->caricaFigli(NULL, NULL, paintInfo, false, -1);
    } else {
        // lo inserisce come figlio dell'item corrente
        theNewElement = pasteElement->copyTo(*new Element(this));
        Element *parentElement = pasteTo;
        if(!parentElement->isElement()) {
            return ;
        }
        parentElement->addChild(theNewElement);
        theNewElement->caricaFigli(NULL, parentElement->getUI(), paintInfo, false, -1);
    }
    _undoStack.clear();
    Utils::TODO_NEXT_RELEASE("undo");
    setModified(true);
}


void Regola::pasteAttributes(QWidget *window, QTreeWidget *tree, CopyAttributesSession *attributesCopied)
{
    if(NULL == attributesCopied) {
        Utils::error(tr("No attributes to paste."));
        return;
    }
    QTreeWidgetItem *currItem = getSelItem(tree);
    if(NULL == currItem) {
        Utils::errorNoSel(window);
        return;
    }
    Element *element = Element::fromItemData(currItem);
    pasteAttributesInternals(tree, element, attributesCopied);
}


void Regola::pasteAttributes(QTreeWidget *tree, Element *element, CopyAttributesSession *attributesCopied)
{
    pasteAttributesInternals(tree, element, attributesCopied);
}

void Regola::pasteAttributesInternals(QTreeWidget *tree, Element *element, CopyAttributesSession *attributesCopied)
{
    if(element->getType() != Element::ET_ELEMENT) {
        Utils::error(tr("The attributes are valid only for an element."));
        return;
    }

    UndoPasteAttributesCommand *undoPasteAttributesCommand = new UndoPasteAttributesCommand(tree, this, element->indexPath());
    bool isModified = false;
    if(element->pasteAttributes(attributesCopied->attributes(), undoPasteAttributesCommand, isModified)) {
        if(isModified) {
            element->markEdited();
            _undoStack.push(undoPasteAttributesCommand);
            element->updateSizeInfo();
            element->refreshUI();
            setModified(true);
        } else {
            delete undoPasteAttributesCommand;
        }
    } else {
        Utils::error(tr("Error while pasting attributes."));
        delete undoPasteAttributesCommand;
    }
}

void Regola::pasteClearAttributesInternals(QTreeWidget *tree, Element *element, CopyAttributesSession *attributesCopied)
{
    if(element->getType() != Element::ET_ELEMENT) {
        Utils::error(tr("The attributes are valid only for an element."));
        return;
    }

    UndoPasteAttributesCommand *undoPasteAttributesCommand = new UndoPasteAttributesCommand(tree, this, element->indexPath());
    bool isModified = false;
    if(element->pasteClearAttributes(attributesCopied->attributes(), undoPasteAttributesCommand, isModified)) {
        if(isModified) {
            element->markEdited();
            _undoStack.push(undoPasteAttributesCommand);
            element->updateSizeInfo();
            element->refreshUI();
            setModified(true);
        } else {
            delete undoPasteAttributesCommand;
        }
    } else {
        Utils::error(tr("Error while pasting attributes."));
        delete undoPasteAttributesCommand;
    }
}

Element *Regola::insertInternal(QTreeWidget *tree, Element *parentElement, Element *pasteElement, const int position, const bool isRecursive)
{
    //if not sel, at the root iif rule is empty
    //sse extists an item
    //append l'item ( o lo inserisce, con tutti i figli e poi ricarica la lista)
    Element *theNewElement = NULL ;
    if(NULL == pasteElement) {
        return NULL ;
    }
    if(NULL == parentElement) {
        theNewElement = pasteElement->copyTo(*new Element(this), isRecursive);
        addTopElement(theNewElement, position);
        theNewElement->caricaFigli(tree, NULL, paintInfo, true, -1);
    } else {
        theNewElement = pasteElement->copyTo(*new Element(this), isRecursive);
        // serve???
        if(!parentElement->isElement()) {
            delete theNewElement ;
            return NULL ;
        }
        if(-1 == position) {
            parentElement->addChild(theNewElement);
        } else {
            parentElement->addChildAt(theNewElement, position);
        }
        theNewElement->caricaFigli(tree, parentElement->getUI(), paintInfo, true, position);
    }
    theNewElement->markEditedRecursive();
    setModified(true);
    return theNewElement ;
}

Element *Regola::updateElementUI(Element *element, const bool isRecursive)
{
    //if not sel, at the root iif rule is empty
    //sse extists an item
    //append l'item ( o lo inserisce, con tutti i figli e poi ricarica la lista)
    if(NULL == element) {
        return NULL ;
    }
    element->display(element->getUI(), paintInfo, true);
    if(isRecursive) {
        element->markEditedRecursive();
    } else {
        element->markEdited();
    }
    setModified(true);
    return element ;
}

void Regola::expand(QTreeWidget *tree)
{
    if(NULL != rootItem) {
        rootItem->expand(tree);
    }
}

void Regola::notifyDeletionTopElement(Element *pEl)
{
    //if (pEl->getType() == Element::ET_ELEMENT) {
    //Q_ASSERT_X(rootItem == pEl, "notifyDeletionTopElement", "rootItem is out of sync");
    // take out element from vector
    int elementIndex = childItems.indexOf(pEl);
    if(-1 != elementIndex) {
        childItems.remove(elementIndex);
        if(0 == elementIndex) {
            checkEncoding();
        }
    }
    if(pEl->getType() == Element::ET_ELEMENT) {
        rootItem = NULL ;
    }
}

// empty-> top, else append to element
void Regola::addComment(QWidget *window, QTreeWidget *tree)
{
    addComment(window, tree, NULL);
}
// empty-> top, else append to element
void Regola::addComment(QWidget *window, QTreeWidget *tree, Element *newComment)
{
    QTreeWidgetItem *currItem = getSelItem(tree);
    bool isEmptyE = isEmpty(false);
    Element *parentElement = NULL ;
    if(NULL != currItem) {
        parentElement = Element::fromItemData(currItem);
        if(NULL != parentElement) {
            if(parentElement->getType() != Element::ET_ELEMENT) {
                Utils::error(tr("Comments can be added only to elements, consider appending, instead"));
                return ;
            }
        }
    } else {
        if(!isEmptyE) {
            Utils::error(tr("Please, select an element to append the comment"));
            return ;
        }
    }

    if(NULL == newComment) {
        Element *theNewElement = newElement(Element::ET_COMMENT);
        if(editNodeComment(window, theNewElement)) {
            newComment = theNewElement;
        } else {
            delete theNewElement;
            theNewElement = NULL ;
        }
    }
    if(NULL != newComment) {
        insertElementInternal(newComment, parentElement, tree);
    }
}

void Regola::appendComment(QWidget *window, QTreeWidget *tree)
{
    appendComment(window, tree, NULL);
}

void Regola::appendComment(QWidget *window, QTreeWidget *tree, Element *newComment)
{
    QTreeWidgetItem *currItem = getSelItem(tree);
    bool isEmptyE = isEmpty(false);
    Element *parentElement = NULL ;
    Element *brotherElement  = NULL;
    if(NULL != currItem) {
        brotherElement  = Element::fromItemData(currItem);
        parentElement = brotherElement->parent();
        if(NULL != parentElement) {
            if(parentElement->getType() != Element::ET_ELEMENT) {
                Utils::error(tr("Comments can be added only to elements, consider appending, instead"));
                return ;
            }
        }
    } else {
        if(!isEmptyE) {
            Utils::errorNoSel(window);
            return ;
        }
    }
    // Regola vuota, aggiungo il primo ed ultimo
    Element *theNewElement = NULL;
    if(NULL == newComment) {
        theNewElement = newElement(Element::ET_COMMENT);
        if(!editNodeComment(window, theNewElement)) {
            delete theNewElement;
            theNewElement = NULL;
        }
    } else {
        theNewElement = newComment;
    }

    if(NULL != theNewElement) {
        appendElementInternal(theNewElement, brotherElement, tree);
    } else {
        delete theNewElement;
    }
}

void Regola::appendProcessingInstruction(QWidget *window, QTreeWidget *tree)
{
    appendProcessingInstruction(window, tree, NULL);
}

void Regola::appendProcessingInstruction(QWidget *window, QTreeWidget *tree, Element *newPI)
{
    QTreeWidgetItem *currItem = getSelItem(tree);
    bool isEmptyE = isEmpty(false);
    Element *parentElement = NULL ;
    Element *brotherElement  = NULL;
    if(NULL != currItem) {
        brotherElement  = Element::fromItemData(currItem);
        parentElement = brotherElement->parent();
        if(NULL != parentElement) {
            if(parentElement->getType() != Element::ET_ELEMENT) {
                Utils::error(tr("Processing instructions can be added only to elements, consider appending, instead"));
                return ;
            }
        }
    } else {
        if(!isEmptyE) {
            Utils::errorNoSel(window);
            return ;
        }
    }
    // Regola vuota, aggiungo il primo ed ultimo
    Element *piToIns = NULL ;
    if(NULL == newPI) {
        Element *theNewElement = newElement(Element::ET_PROCESSING_INSTRUCTION);
        if(::editProcessingInstruction(window, theNewElement)) {
            piToIns = theNewElement ;
        } else {
            delete theNewElement ;
        }
    } else {
        piToIns = newPI ;
    }
    if(NULL != piToIns) {
        appendElementInternal(piToIns, brotherElement, tree);
    }
}

void Regola::addProcessingInstruction(QWidget *window, QTreeWidget *tree)
{
    addProcessingInstruction(window, tree, NULL);
}

void Regola::addProcessingInstruction(QWidget *window, QTreeWidget *tree, Element *newPI)
{
    QTreeWidgetItem *currItem = getSelItem(tree);
    bool isEmptyE = isEmpty(false);
    Element *parentElement = NULL ;
    if(NULL != currItem) {
        parentElement = Element::fromItemData(currItem);
        if(NULL != parentElement) {
            if(parentElement->getType() != Element::ET_ELEMENT) {
                Utils::error(tr("Processing instructions can be added only to elements, consider appending, instead"));
                return ;
            }
        }
    } else {
        if(!isEmptyE) {
            Utils::error(tr("Please, select an element to append the processing instruction"));
            return ;
        }
    }
    Element *piToIns = NULL ;
    if(NULL == newPI) {
        Element *theNewElement = newElement(Element::ET_PROCESSING_INSTRUCTION);
        if(::editProcessingInstruction(window, theNewElement)) {
            piToIns = theNewElement ;
        } else {
            delete theNewElement ;
        }
    } else {
        piToIns = newPI ;
    }
    if(NULL != piToIns) {
        insertElementInternal(piToIns, parentElement, tree);
    }
}

void Regola::searchWithXQuery(FindTextParams &findArgs, Element *selectedItem)
{
    SearchXQuery xQuery;
    xQuery.search(this, selectedItem, findArgs);
}

void Regola::unhiliteAll()
{
    QHashIterator<int, Element*> sel(selection);
    while(sel.hasNext()) {
        sel.next();
        Element *element = sel.value() ;
        element->unhilite();
    }
    selection.clear();
    D(printf("cleared\n"));
}


void Regola::addHilite(Element *element)
{
    selection.insert(element->getInstanceId(), element);
    D(printf("new element, total is %d \n", selection.size()));
}


void Regola::takeOutElement(Element* element)
{
    D(printf("before %d elements\n", selection.size()));
    int id = element->getInstanceId();
    if(selection.contains(id)) {
        selection.remove(id);
    }
    D(printf("after %d elements\n", selection.size()));
}


void Regola::clearBookmarks()
{
    bookmarks.clear();
}

bool Regola::addBookmark(Element* element)
{
    if(NULL != element) {
        return bookmarks.add(element);
    }
    return false;
}

void Regola::removeBookmark(Element* element)
{
    if(NULL != element) {
        bookmarks.remove(element, false);
    }
}

void Regola::toggleBookmark(Element* element)
{
    if(NULL != element) {
        bookmarks.toggle(element);
    }
}

bool Regola::isBookmarked(Element* element)
{
    return bookmarks.isBookmarked(element);
}


void Regola::setPaintInfo(PaintInfo *newValue)
{
    if(_ownPaintInfo) {
        if(NULL != paintInfo) {
            delete paintInfo;
        }
    }
    paintInfo = newValue ;
    _ownPaintInfo = false;
}

PaintInfo *Regola::getPaintInfo()
{
    if(NULL == paintInfo) {
        paintInfo = new PaintInfo();
        _ownPaintInfo = true ;
    }
    return paintInfo;
}

void Regola::setEditHook(EditHook theEditHook)
{
    _editHook = theEditHook ;
}

void Regola::setEditTextHook(EditTextHook theEditTextHook)
{
    _editTextHook = theEditTextHook ;
}

void Regola::redisplayElement(Element *element)
{
    element->displayWithPaintInfo(paintInfo);
}

Element *Regola::gotoPreviousBookmark()
{
    return bookmarks.gotoPrevious();
}

Element *Regola::gotoNextBookmark()
{
    return bookmarks.gotoNext();
}

int Regola::bookmarkSize()
{
    return bookmarks.bookmarkSize();
}

int Regola::nextBookmark()
{
    return bookmarks.nextBookmark();
}

int Regola::previousBookmark()
{
    return bookmarks.previousBookmark();
}

void Regola::handleSelection(Element *element)
{
    bookmarks.newSelection(element);
}

void Regola::notifyUIDeleted(Element *element)
{
    //emit uiDeleted(element);
    bookmarks.remove(element, true);
}

void Regola::takeoutFormBookmarks(Element *element)
{
    bookmarks.remove(element, true);
}

void Regola::setUserDefinedXsd(const QString &path)
{
    _userDefinedXsd = path ;
}

void Regola::checkValidationReference()
{
    _useNoNamespaceXSD = false;
    _useNamespaceXSD = false;
    _noNameSpaceXsd = "" ;
    _documentXsd = "" ;
    _namespacesByPrefixAndName.clear();
    _namespacesByNameAndPrefix.clear();
    _schemaLocationsByNamespace.clear();
    _schemaLocationsList.clear();
    Element *theRoot = root();
    if(NULL != theRoot) {
        QString attrNSName ;
        QString attrNSNoNamespace ;
        QList<Attribute*> attrs = theRoot->getAttributesList();
        Attribute *schemaURIAttr = NULL ;
        QString namespaceXsi = "" ;
        // collect all namespaces along with declarations
        foreach(Attribute * attr, attrs) {
            if(attr->name.startsWith("xmlns:") || (attr->name == "xmlns")) {
                // add a namespace
                int pos = attr->name.indexOf(":");
                QString ns ;
                if(pos >= 0) {
                    if(pos < attr->name.length()) {
                        ns = attr->name.mid(pos + 1);
                    }
                }
                // ns can be empty
                _namespacesByPrefixAndName.insert(ns, attr->value);
                _namespacesByNameAndPrefix.insert(attr->value, ns);
                // look for special value to identify schema namespace
                if(attr->value == Regola::XSDSchemaInstance) {
                    schemaURIAttr = attr ;
                    namespaceXsi = ns ;
                }
            }
        } // end of scan for namespaces

        // found the namespace of xsi, lookup for locations
        if(NULL != schemaURIAttr) {
            attrNSName = XmlUtils::makeQualifiedName(namespaceXsi, SCHEMA_LOCATION_LITERAL);
            attrNSNoNamespace = XmlUtils::makeQualifiedName(namespaceXsi, SCHEMA_NONS_LOCATION_LITERAL);

            Attribute * attrNoNs = theRoot->getAttribute(attrNSNoNamespace);
            if(NULL != attrNoNs) {
                _noNameSpaceXsd = attrNoNs->value;
                _documentXsd = _noNameSpaceXsd ;
                _useNoNamespaceXSD = true ;
            }
            // ---------------------------------------------------
            // get all the attributes
            Attribute * attr = theRoot->getAttribute(attrNSName);
            if(NULL != attr) {
                _useNamespaceXSD = true ;
                QString value = attr->value;
                decodeASchema(value);
                // set the first
                if(_schemaLocationsByNamespace.size() > 0) {
                    if(_documentXsd.isEmpty()) {
                        if(_namespacesByPrefixAndName.contains("")) {
                            QString nsDocument = _namespacesByPrefixAndName[""];
                            _documentXsd = _schemaLocationsByNamespace[nsDocument];
                        }
                    }
                }
            }
        } else {
            // we not consider using xsi:etc alone, because it is off specifications.
            /****************************************************
                        Attribute * attrNs = theRoot->getAttribute(ATTRIBUTE_SCHEMA_LOCATION);
                        if(NULL != attrNs) {
                            decodeASchema(attrNs->value);
                            if(_namespacesLocationXsd.size() > 0) {
                                if( _documentXsd.isEmpty() ) {
                                    _documentXsd = _namespacesLocationXsd.values().at(0);
                                }
                                _namespacesNamesXsd.insert(XSI_NAMESPACE, _documentXsd);
                                _namespacesNsNamesXsd.insert(_documentXsd, XSI_NAMESPACE);
                            }
                        }

                        Attribute * attrNoNs = theRoot->getAttribute(ATTRIBUTE_SCHEMA_LOCATION);
                        if(NULL != attrNoNs) {
                            decodeASchema(attrNs->value);
                            if(_namespacesLocationXsd.size() > 0) {
                                if( _documentXsd.isEmpty() ) {
                                    _documentXsd = _namespacesLocationXsd.values().at(0);
                                }
                                _noNameSpaceXsd = _documentXsd ;
                                _namespacesNamesXsd.insert(XSI_NAMESPACE, _documentXsd);
                                _namespacesNsNamesXsd.insert(_documentXsd, XSI_NAMESPACE);
                            }
                        }
            **************************************************/
        }
    }
} // checkValidationReference()

void Regola::decodeASchema(const QString &value)
{
    // this grummy piece of code follows w3c specifications
    QString normalizedValue = Utils::normalizeStringXML(value);

    //if(value.indexOf(" ")) {
    QStringList schemas = normalizedValue.split(" ");
    int schemasNumber = schemas.size() / 2;
    for(int i = 0 ; i < schemasNumber ; i ++) {
        _schemaLocationsList.append(QPair<QString, QString>(schemas.at(2 * i), schemas.at(2 * i + 1)));
        _schemaLocationsByNamespace.insert(schemas.at(2 * i), schemas.at(2 * i + 1));
    }
    //} else {
    //   _documentXsd = value ;
    //}
}

bool Regola::hasMoreThanOneXsdFile()
{
    if((_schemaLocationsByNamespace.size() > 1) || ((_schemaLocationsByNamespace.size() == 1) && !_noNameSpaceXsd.isEmpty())) {
        return true ;
    }
    return false ;
}

void Regola::setUserAndDocumentXsd(const QString &newXsd)
{
    _documentXsd = newXsd ;
    _userDefinedXsd = newXsd;
}


void Regola::pasteXML(QTreeWidget *tree, Regola *newRegola)
{
    QVectorIterator<Element*> it(newRegola->childItems);
    while(it.hasNext()) {
        Element* element = it.next() ;
        paste(tree, element);
    }
}

void Regola::pasteElements(QTreeWidget *tree, QList<Element*> elements)
{
    foreach(Element * element, elements) {
        paste(tree, element);
    }
}

bool Regola::collectSizeData()
{
    return _collectSizeData;
}

void Regola::setCollectSizeData(const bool isCollect)
{
    _collectSizeData = isCollect ;
}

void Regola::recalcSize()
{
    if(NULL != rootItem) {
        rootItem->recalcSize(true);
    }
}

void Regola::assignCollectSizeDataFlag(const bool isCollect)
{
    bool toRecalc = !collectSizeData();
    setCollectSizeData(isCollect);
    if(isCollect && toRecalc) {
        recalcSize();
    }
}

void Regola::getBookmarkedElements(QVector<Element*> &elementsBookmarked)
{
    foreach(Bookmark * bookmark, bookmarks.getBookmarks()) {
        Element* element = bookmark->getElement();
        if(NULL != element) {
            elementsBookmarked.append(element);
        }
    }
}

void Regola::deleteBookmarked()
{
    clearUndo();
    bool isContinue ;
    int deletedItems = 0;
    bool calcSize = paintInfo->showElementSize() ;
    do {
        isContinue = false ;
        foreach(Bookmark * bookmark, bookmarks.getBookmarks()) {
            Element* element = bookmark->getElement();
            if(NULL != element) {
                Element *parentEl = NULL ;
                if(calcSize) {
                    parentEl = element->parent();
                }
                element->autoDeleteRecursive();
                deletedItems++;
                isContinue = true ;
                if(parentEl != NULL) {
                    parentEl->updateSizeInfo();
                }
                break ;
            }
        }
    } while(isContinue);
    clearBookmarks();
    selection.clear();
}

void Regola::cleanBookmarkedContents()
{
    clearUndo();
    QVector<Element*> elementsBookmarked ;
    getBookmarkedElements(elementsBookmarked);
    foreach(Element * element, elementsBookmarked) {
        element->clearTextNodes();
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

bool Regola::loadXplore(const QString &fileName)
{
    modified = false;
    _collectSizeData = false ;
    xmlFileName = fileName ;

    XSaxHandler handler(this);
    QXmlSimpleReader reader;
    reader.setFeature("http://xml.org/sax/features/namespaces", false);
    reader.setFeature("http://xml.org/sax/features/namespace-prefixes", true);
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }
    bool isOk = true ;
    QXmlInputSource xmlInput(&file);
    if(!reader.parse(xmlInput)) {
        isOk = false  ;
    }
    file.close();
    return isOk ;
}

bool Regola::isValidXsd()
{
    if(NULL == rootItem) {
        return false;
    }
    // Search in attributes.
    QString elNamespace;
    QString elLocalName;
    rootItem->namespaceOfElement(elNamespace, elLocalName);
    Attribute *attribute = NULL ;
    if(!elNamespace.isEmpty()) {
        attribute = rootItem->getAttribute(QString("%1:%2").arg(XMLNS_PREFIX).arg(elNamespace));
    } else {
        attribute = rootItem->getAttribute(QString(XMLNS_PREFIX));
    }
    if(NULL == attribute) {
        return false ;
    }
    return XmlUtils::IsXsdValid(attribute->value, elLocalName);
}

int Regola::indexOfTopLevelItem(Element *element)
{
    return childItems.indexOf(element);
}


int Regola::setTopLevelItemLike(Element *newElement, Element* oldElement)
{
    int pos = childItems.indexOf(oldElement);
    if(pos >= 0) {
        childItems.insert(pos, newElement);
    }
    return pos ;
}


int Regola::swapAndDeleteElement(Element *newElement, Element* oldElement)
{
    int pos = 1 ;
    Element *parentElement = oldElement->parent();
    if(NULL == parentElement) {
        pos = setTopLevelItemLike(newElement, oldElement);
    } else {
        pos = parentElement->setItemLike(newElement, oldElement);
    }
    oldElement->autoDeleteRecursive();
    return pos ;
}


// sostituisce element con il commento
void Regola::transformInComment(QWidget *window, QTreeWidget *tree, Element *elementToTransform)
{
    if(NULL == elementToTransform) {
        Utils::errorNoSel(window);
        return ;
    }
    Element *parentElement = elementToTransform->parent();
    Element *comment = new Element(this, Element::ET_COMMENT, parentElement) ;
    comment->setComment(elementToTransform->transformToText());
    int pos = swapAndDeleteElement(comment, elementToTransform);

    QTreeWidgetItem *parentUI = NULL ;
    if(NULL != parentElement) {
        parentUI = parentElement->getUI();
    }
    comment->caricaFigli(tree, parentUI, paintInfo, true, pos);
    comment->markEditedRecursive();
    clearUndo();
    setModified(true);
    return ;
}


// creates xml elements from a comment, if possible

/**
  * if  ! is possible to convert from string -> warn the user
  * if the root element exists -> error
  */
bool Regola::generateFromComment(QTreeWidget *tree, UIDelegate *uiDelegate, Element *elementToTransform)
{
    if(NULL == elementToTransform) {
        if(NULL != uiDelegate) {
            uiDelegate->error(Utils::errorNoSelString());
        }
        return false;
    }
    Element *parentElement = elementToTransform->parent();
    if((NULL == parentElement) && (rootItem != NULL)) {
        if(NULL != uiDelegate) {
            uiDelegate->error(tr("A root item exists already, cannot proceed."));
        }
        return false ;
    }

    if(elementToTransform->getType() != Element::ET_COMMENT) {
        if(NULL != uiDelegate) {
            uiDelegate->error(tr("The selected elmement is not a comment, cannot proceed."));
        }
        return false ;
    }
    int position = elementToTransform->indexOfSelfAsChild();
    QDomDocument document;
    if(document.setContent(elementToTransform->getComment())) {
        Regola newModel(document, "");
        // reverse copy to have the elements inserted in the correct order
        QVectorIterator<Element*> reverseElementIterator(newModel.childItems);
        reverseElementIterator.toBack();
        while(reverseElementIterator.hasPrevious()) {
            Element * toCopyElement = reverseElementIterator.previous();
            pasteInternals(tree, parentElement, toCopyElement, position);
        }
        // note that this operation cannot be done if a valid root already exists
        elementToTransform->autoDeleteRecursive();
        clearUndo();
        return true ;
    } else {
        if(NULL != uiDelegate) {
            uiDelegate->error(tr("The content of the comment is not valid XML. Cannot proceed."));
        }
        return false;
    }
}

Element *Regola::findElementByArray(QList<int> &selection)
{
    if(selection.isEmpty()) {
        return NULL ;
    }
    int pos = selection.at(0);
    if(pos >= childItems.size()) {
        return NULL ;
    }

    Element *element = childItems.at(pos);
    if(selection.size() == 1) {
        return element ;
    }
    return findChildElementByArray(element, selection, 1);
}

Element *Regola::findChildElementByArray(Element *element, QList<int> &selection, const int listPos)
{
    int pos = selection.at(listPos);
    QVector<Element*>& chs = element->getItems();
    if(pos >= chs.size()) {
        return NULL ;
    }
    Element *thisElement = chs.at(pos);
    if(selection.size() <= (listPos + 1)) {
        return thisElement ;
    }
    return findChildElementByArray(thisElement, selection, listPos + 1);
}

bool Regola::findElement(Element *toFind)
{
    foreach(Element * ep, childItems) {
        if(ep == toFind) {
            return true ;
        }
        if(ep->findElement(toFind)) {
            return true ;
        }
    }
    return false;
}

QString Regola::textOfCantEditMixedContentElementText()
{
    return tr("When element has mixed contents you must edit text nodes.");
}

void Regola::hideLeafNodes()
{
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element *el = it.next();
        el->hideChildrenLeaves();
    }
}

void Regola::showLeafNodes()
{
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element *el = it.next();
        el->showChildrenLeaves();
    }
}

//-------------------------------------------------------------------------------------------------------------------------------------------------

/**
  this is an hack to collect strings in a future to lower memory consumption
  */
QString Regola::addNameToPool(const QString &inputString)
{
    QSet<QString>::const_iterator it = _namesPool.insert(inputString);
    return *it ;
}

QString Regola::getAttributeString(const QString &attributeString)
{
    QSet<QString>::const_iterator it = _attributeValuesPool.insert(attributeString);
    return *it ;
}

QString Regola::getAttributeNameString(const QString &attributeString)
{
    QSet<QString>::const_iterator it = _attributeNamesPool.insert(attributeString);
    return *it ;
}

QSet<QString> *Regola::namesPool()
{
    return &_namesPool;
}

QSet<QString> *Regola::attributeNamesPool()
{
    return &_attributeNamesPool;
}

QSet<QString> Regola::attributeNamesPoolByValue()
{
    return _attributeNamesPool;
}

//-----------------------------------------------------------------------------------------------------------------------------------------

const int Regola::ModelName = 0 ;
const int Regola::ModelValue = 1 ;
const int Regola::ModelColumns = 2 ;

QVariant Regola::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    Element *element = static_cast<Element*>(index.internalPointer());
    if(NULL != element) {
        switch(role) {
        case Qt::ToolTipRole:
            return element->columnViewTooltipData();
            break;
        case Qt::DisplayRole:
            switch(index.column()) {
            case ModelValue: return QVariant(element->text);
            case ModelName: {
                return element->columnViewDisplayData(paintInfo, index.row() + 1);
            } // if name
            } // switch column
            break;
        case Qt::FontRole:
            return element->styleElementTagFont(paintInfo);
        case Qt::ForegroundRole:
            return element->styleElementTagColor(paintInfo);
        case Qt::DecorationRole:
            return element->styleElementTagIcon();
        default:
            break;
        } // switch displayRole
    }
    return QVariant();
}

Qt::ItemFlags Regola::flags(const QModelIndex &index) const
{
    if(!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled ;
}

QVariant Regola::headerData(int section, Qt::Orientation /*orientation*/, int role) const
{
    if(Qt::DisplayRole == role) {
        switch(section) {
        case ModelName:
            return tr("Element");
        case ModelValue:
            return tr("Attributes");
        default:
            return QVariant("");
        }
    }
    return QVariant();
}

QModelIndex Regola::index(int row, int column, const QModelIndex &parent) const
{
    if((row < 0) || (column < 0) || (column >= ModelColumns) || (parent.isValid() && (parent.column() > 0))) {
        return QModelIndex();
    }

    if(!parent.isValid()) {
//        printf("Parent ROOT NOT VALID created index: %d %d\n", row, column);
//        fflush(stdout);
        if(row < childItems.size()) {
            return createIndex(row, column, childItems.at(row));
        }
    } else {
        Element* parentElement = static_cast<Element*>(parent.internalPointer());
        Element* element = parentElement->getChildAt(row);
        if(NULL != element) {
//           printf("Element %s, created index: %d %d\n", element->tag().toLatin1().data(), row, column);
//          fflush(stdout);
            return createIndex(row, column, element);
        }
    }
    return QModelIndex();
}

int Regola::rowCount(const QModelIndex &parent) const
{
    if(parent.column() > 0) {
        return 0;
    }

    int resRowCount = 0 ;
    Element *parentElement;
    if(!parent.isValid()) {
        resRowCount = childItems.size();
    } else {
        parentElement = static_cast<Element*>(parent.internalPointer());
        if(NULL != parentElement) {
            resRowCount = parentElement->getChildItemsCount();
        } else {
            resRowCount = childItems.size();
        }
    }
    return resRowCount ;
}

int Regola::columnCount(const QModelIndex & /*parent*/) const
{
    return ModelColumns ;
}

QModelIndex Regola::parent(const QModelIndex & index) const
{
    if(index.isValid()) {
        Element *element = static_cast<Element*>(index.internalPointer());
        if(NULL != element) {
            Element *parentElement = element->parent() ;
            int row ;
            if(NULL != parentElement) {
                Element *grandParentElement = parentElement->parent() ;
                if(NULL != grandParentElement) {
                    row = grandParentElement->childIndex(parentElement);
                } else {
                    row = childItems.indexOf(parentElement);
                }
                return createIndex(row, 0, parentElement);
            }
        }
    }
    return QModelIndex();
}

void Regola::addUndoInsert(QTreeWidget * tree, Element * element)
{
    UndoInsertCommand *undoInsertCommand = new UndoInsertCommand(tree, this, element->indexPath());
    _undoStack.push(undoInsertCommand);
}

void Regola::insertElementComplete(Element *theNewElement, Element *parentElement, QTreeWidget *tree)
{
    insertElementInternal(theNewElement, parentElement, tree, true) ;
}

void Regola::insertElementInternal(Element *theNewElement, Element *parentElement, QTreeWidget *tree)
{
    insertElementInternal(theNewElement, parentElement, tree, true) ;
}

void Regola::refreshChildrenOfElement(Element *element, QTreeWidget *tree)
{
    element->refreshChildren(paintInfo);
    element->expand(tree);
}

void Regola::insertElementInternal(Element *theNewElement, Element *parentElement, QTreeWidget *tree, const bool useUndo)
{
    if(NULL != parentElement) {
        parentElement->addChild(theNewElement);
        theNewElement->caricaFigli(tree, parentElement->getUI(), paintInfo);
    } else {
        addTopElement(theNewElement);
        theNewElement->caricaFigli(tree, NULL, paintInfo);
    }
    theNewElement->expand(tree);
    afterInsertHousekeeping(theNewElement, tree, useUndo);
}

void Regola::appendElementInternal(Element *theNewElement, Element *brotherElement, QTreeWidget *tree, const bool useUndo)
{
    if(NULL == brotherElement) {
        addTopElement(theNewElement);
        theNewElement->caricaFigli(tree, NULL, paintInfo);
    } else {
        Element *parentElement = brotherElement->parent();
        if(NULL == parentElement) {
            addTopElement(theNewElement);
            theNewElement->caricaFigli(tree, NULL, paintInfo);
        } else {
            int pos = parentElement->addChildAfter(theNewElement, brotherElement);
            theNewElement->caricaFigli(tree, parentElement->getUI(), paintInfo, true, pos);
        }
    }
    theNewElement->expand(tree);
    afterInsertHousekeeping(theNewElement, tree, useUndo);
}

void Regola::appendElementComplete(Element *theNewElement, Element *brotherElement, QTreeWidget *tree, const bool useUndo)
{
    appendElementInternal(theNewElement, brotherElement, tree, useUndo);
}

void Regola::afterInsertHousekeeping(Element *theNewElement, QTreeWidget *tree)
{
    afterInsertHousekeeping(theNewElement, tree, true);
}

void Regola::afterInsertHousekeeping(Element *theNewElement, QTreeWidget *tree, const bool useUndo)
{
    theNewElement->markEditedRecursive();
    QTreeWidgetItem *item = theNewElement->getUI();
    tree->setCurrentItem(item);
    QTreeWidgetItem *parentItem = item->parent();
    if(NULL != parentItem) {
        tree->expandItem(parentItem);
    }
    if(useUndo) {
        addUndoInsert(tree, theNewElement);
    }
    setModified(true);
}

bool Regola::moveUp(Element *element)
{
    return internalMoveUp(element, true);
}

// Move up in parent containment.
bool Regola::moveDown(Element *element)
{
    return internalMoveDown(element, true);
}

bool Regola::internalMoveUp(Element *element, const bool registerInUndo)
{
    if(registerInUndo) {
        UndoMoveUpCommand *undoCmd = new UndoMoveUpCommand(element->getUI()->treeWidget(), this, element->indexPath()) ;
        _undoStack.push(undoCmd);
        return undoCmd->done();
    }
    // Regola vuota, aggiungo il primo ed ultimo
    Element *parentItem = element->parent();
    bool doneOp = false;
    if(NULL == parentItem) {
        doneOp = Element::moveUp(getItems(), element);
    } else {
        doneOp = parentItem->moveUp(element);
    }
    checkEncodingByEl(element);
    return doneOp;
}

// Move up in parent containment.
bool Regola::internalMoveDown(Element *element, const bool registerInUndo)
{
    if(registerInUndo) {
        UndoMoveDownCommand *undoCmd = new UndoMoveDownCommand(element->getUI()->treeWidget(), this, element->indexPath()) ;
        _undoStack.push(undoCmd);
        return undoCmd->done();
    }
    Element *parentItem = NULL ;
    parentItem = element->parent();
    bool doneOp = false;
    if(NULL == parentItem) {
        doneOp = Element::moveDown(getItems(), element);
    } else {
        doneOp = parentItem->moveDown(element);
    }
    checkEncodingByEl(element);
    return doneOp;
}

bool Regola::canUndo()
{
    return _undoStack.canUndo();
}

bool Regola::canRedo()
{
    return _undoStack.canRedo();
}

int Regola::undoCount()
{
    return _undoStack.count();
}

void Regola::undo()
{
    return _undoStack.undo();
}

void Regola::redo()
{
    return _undoStack.redo();
}

//--------------------------------------------------------------------------------

bool Regola::checkIfUIItemsCorrespondToElements(Element *target)
{
    if(NULL == target) {
        return false;
    }
    QTreeWidgetItem *item = target->getUI();
    if(NULL == item) {
        return false;
    }
    Element *parentElement = NULL ;
    parentElement = target->parent();
    QVector<Element*> *elements = NULL ;
    QTreeWidgetItem *parentItem = item->parent();
    int childrenCount ;
    QTreeWidget *tree = NULL ;
    if(NULL == parentElement) {
        tree = item->treeWidget();
        if(NULL == tree) {
            return false;
        }
        elements = &childItems;
        childrenCount = tree->topLevelItemCount();

    } else {
        if(NULL == parentItem) {
            return false;
        }
        elements = parentElement->getChildItems();
        childrenCount = parentItem->childCount() ;
    }
    int count = elements->size();
    if(count != childrenCount) {
        return false;
    }
    for(int i = 0 ; i < count ; i ++) {
        Element *e = elements->at(i);
        QTreeWidgetItem *child = NULL;
        if(NULL != tree) {
            child = tree->topLevelItem(i);
        } else {
            child = parentItem->child(i) ;
        }
        if(e->getUI() != child) {
            return false;
        }
    }
    return true;
}

bool Regola::addUndoDeleteElement(Element* element)
{
    if(NULL != element) {
        UndoDeleteCommand *undoCmd = new UndoDeleteCommand(element->getUI()->treeWidget(), this, element->indexPath());
        _undoStack.push(undoCmd);
        emit undoStateChanged();
        return true;
    }
    return false;
}

bool Regola::deleteElement(Element* element, const bool isAutoDelete)
{
    if(NULL == element) {
        return false;
    }
    addUndoDeleteElement(element) ;
    element->autoDelete(isAutoDelete);
    return true;
}

bool Regola::takeElement(Element* element, const bool isUseUndo)
{
    if(NULL == element) {
        return false;
    }
    if(isUseUndo) {
        addUndoDeleteElement(element) ;
    } else {
        _undoStack.clear();
    }
    if(element == rootItem) {
        rootItem = NULL ;
    }
    if(childItems.contains(element)) {
        int index = childItems.indexOf(element);
        if(index >= 0) {
            childItems.remove(index);
        }
    }
    element->detachFromParent();
    return true;
}

void Regola::removeElementReference(Element *element)
{
    takeoutFormBookmarks(element);
    takeOutElement(element);
}


bool Regola::deleteElement(Element* element)
{
    return deleteElement(element, true);
}

void Regola::emptyUndoStack()
{
    _undoStack.clear();
}

void Regola::dumpDocument(QDomDocument &document)
{
    QString s;
    QDomDocumentType dtype = document.doctype();

    s += QString("internal subset:%1\n").arg(dtype.internalSubset());
    s += QString("name:%1\n").arg(dtype.name());
    s += QString("node type:%1\n").arg(dtype.nodeType());
    s += QString("publicId:%1\n").arg(dtype.publicId());
    s += QString("systemId:%1\n").arg(dtype.systemId());

    QDomNamedNodeMap    docEntities = dtype.entities();

    s += "entities:\n";
    int size = docEntities.size();
    for(int i = 0 ; i < size ; i ++) {
        QDomNode node = docEntities.item(i);
        QDomEntity entity = node.toEntity();
        s += QString("---publicId %1, systemId %2, notationName %3, name %4\n").arg(entity.publicId()).arg(entity.systemId()).arg(entity.notationName()).arg(node.nodeName());
    }
    s += "notations:\n";
    size = dtype.notations().size();
    for(int i = 0 ; i < size ; i ++) {
        QDomNode node = dtype.notations().item(i);
        QDomNotation notation = node.toNotation();
        s += QString("----publicId %1, systemId %2, name %3\n").arg(notation.publicId()).arg(notation.systemId()).arg(node.nodeName());
    }
    Utils::error(s);
    QString result = document.toString(_indent);
    Utils::error(result);
    printf("%s\n", result.toLatin1().data());
    fflush(stdout);

}

QString Regola::docType()
{
    return _docType->docType() ;
}

DocumentType *Regola::docTypeInfo()
{
    return _docType;
}

void Regola::setDocType(const QString& newDocType, const QString &systemId, const QString &publicId, const QString& newDtd)
{
    bool changed = false ;
    if(newDocType != _docType->docType()) {
        changed = true ;
    }
    if(systemId != _docType->systemId()) {
        changed = true ;
    }
    if(publicId != _docType->publicId()) {
        changed = true ;
    }
    if(newDtd != _docType->dtd()) {
        changed = true ;
    }
    // this first!
    _docType->setDtd(newDtd);
    _docType->setDocType(newDocType) ;
    _docType->setPublicId(publicId);
    _docType->setSystemId(systemId);
    if(changed) {
        emit docTypeChanged(_docType->docType());
    }
}

void Regola::processDocument(QDomDocument &document)
{
    QDomDocumentType dtype = document.doctype();
    setDocType(dtype.name(), dtype.systemId(), dtype.publicId(), "");
}

QString Regola::namespacePrefixFor(const QString &ns)
{
    QString prefix ;
    if(NULL != rootItem) {
        foreach(Attribute * attribute, rootItem->getAttributesList()) {
            if(attribute->name.startsWith("xmlns:") && (attribute->value == ns)) {
                prefix = XmlUtils::namespacePrefix(attribute->name);
                break;
            } else if((attribute->name == "xmlns") && (attribute->value == ns)) {
                prefix = "" ;
                break;
            }
        }
    }
    return prefix;
}

QString Regola::namespacePrefixXSD()
{
    return namespacePrefixFor(XSDNameSpace);
}

QSet<QString> Regola::namespacePrefixesXSD(Element *element)
{
    QSet<QString> result;
    QSet<QString> allPrefixes;
    Element *startElement = element ;
    if(NULL == startElement) {
        startElement = rootItem;
    }
    namespacesPrefixFor(XSDNameSpace, startElement, result, allPrefixes);
    return result;
}

/**
 * @brief Regola::namespacesPrefixFor scans the chain of ns declaration from the inner up.
 * Inner declarations take precedence.
 * @param ns
 * @param element
 * @param prefixes
 * @param allPrefixes
 */
void Regola::namespacesPrefixFor(const QString &ns, Element *element, QSet<QString> &prefixes, QSet<QString> &allPrefixes)
{
    if(NULL != element) {
        foreach(Attribute * attribute, element->getAttributesList()) {
            if(attribute->name.startsWith("xmlns:") || (attribute->name == "xmlns")) {
                QString prefix = XmlUtils::namespacePrefix(attribute->name);
                if((attribute->value == ns)) {
                    if(!allPrefixes.contains(prefix)) {
                        prefixes.insert(prefix);
                    }
                }
                allPrefixes.insert(prefix);
            }
        } // foreach
        Element *parent = element->parent();
        namespacesPrefixFor(ns, parent, prefixes, allPrefixes);
    }
}

QString Regola::namespacePrefixInRoot(const QString &namespaceToFind)
{
    QString ns ;
    if(NULL != rootItem) {
        foreach(Attribute * attribute, rootItem->getAttributesList()) {
            if(XmlUtils::isDeclaringNS(attribute->name) && (attribute->value == namespaceToFind)) {
                ns = XmlUtils::namespacePrefix(attribute->name);
                return ns ;
            }
        }
    }
    return ns;
}

QString Regola::namespacePrefixXslt()
{
    return namespacePrefixInRoot(Regola::XsltNameSpace);
}

QString Regola::namespacePrefixXSI()
{
    return namespacePrefixInRoot(Regola::XSDSchemaInstance);
}

QString Regola::namespaceOfPrefix(const QString &prefix)
{
    QMap<QString, QString> mapNS = namespaces();
    if(mapNS.contains(prefix)) {
        return mapNS[prefix];
    }
    return "";
}

QMap<QString, QString> Regola::namespaces()
{
    QMap<QString, QString> result;
    if(NULL != rootItem) {
        foreach(Attribute * attribute, rootItem->getAttributesList()) {
            if(attribute->name.startsWith("xmlns:")) {
                QString name = attribute->name.mid(6);
                if(!name.isEmpty()) {
                    result.insert(name, attribute->value);
                }
            } else if(attribute->name == "xmlns") {
                result.insert("", attribute->value);
            }
        }
    }
    return result;
}

bool Regola::hasXSLTNamespace()
{
    QSet<QString> uris = namespacesURI();
    if(uris.contains(XsltNameSpace)) {
        return true;
    }
    return false;
}

bool Regola::hasSCXMLNamespace()
{
    QSet<QString> uris = namespacesURI();
    if(uris.contains(NamespaceManager::SCXMLNamespace)) {
        return true;
    }
    return false;
}

QSet<QString> Regola::namespacesURI()
{
    QSet<QString> result;
    if(NULL != rootItem) {
        foreach(Attribute * attribute, rootItem->getAttributesList()) {
            if(attribute->name.startsWith("xmlns:")) {
                result.insert(attribute->value);
            } else if(attribute->name == "xmlns") {
                result.insert(attribute->value);
            }

        }
    }
    return result;
}

QString Regola::getCachedPrefixNS(const QString &nsURI)
{
    if(!_isCrapCacheNSActivated) {
        if(NULL != rootItem) {
            _isCrapCacheNSActivated = true ;
            foreach(Attribute * attribute, rootItem->getAttributesList()) {
                if(attribute->name.startsWith("xmlns:")) {
                    _crapCachePrefixNs[attribute->name] = attribute->value;
                } else if(attribute->name == "xmlns") {
                    _crapCachePrefixNs[attribute->name] = attribute->value;
                }
            } // foreach
        }
    }
    QString str = _crapCachePrefixNs[nsURI];
    if(NULL == str) {
        str = "" ;
    }
    return str;
}

// static method
QList<Element*> Regola::decodeXMLFromString(const QString &input, const bool onlyRootElement, const bool onlyElements)
{
    QList<Element*> result;
    Regola *regola = NULL ;
    QDomDocument document;
    if(document.setContent(input)) {
        regola = new Regola(document, "", true);
        if(NULL != regola) {
            if(onlyRootElement) {
                Element * root = regola->root();
                if(NULL != root) {
                    regola->takeElement(root);
                    result << root ;
                }
            } else {
                Element *root = regola->root();
                if(NULL != root) {
                    QList<Element*> lst;
                    foreach(Element * el, *root->getChildItems()) {
                        lst << el;
                    }

                    foreach(Element * el, lst) {
                        if(!onlyElements || (onlyElements && el->isElement())) {
                            result << el ;
                            regola->takeElement(el);
                        }
                    }
                }
            } // else
            delete regola;
        }
    }
    return result ;
}

void Regola::removeAllElements(QTreeWidget *tree)
{
    tree->setUpdatesEnabled(false);
    while(!childItems.isEmpty()) {
        Element *e = childItems.first();
        childItems.remove(0);
        e->autoDeleteRecursive();
    }
    _undoStack.clear();
    tree->setUpdatesEnabled(false);
}

Element * Regola::syncRoot()
{
    rootItem = NULL ;
    foreach(Element * e, childItems) {
        if(e->isElement()) {
            rootItem = e ;
            break;
        }
    }
    return rootItem;
}

void Regola::insertElementForce(Element *element)
{
    childItems.append(element);
    element->setRegola(this, true);
    syncRoot();
}


void Regola::anonymize(AnonAlg *alg)
{
    if(NULL != root()) {
        root()->anonymize(alg);
    }
}

Regola *Regola::lightClone()
{
    Regola *newInstance = new Regola();
    // copy all the children and nothing more.

    foreach(Element * e, childItems) {
        newInstance->pasteNoUI(e, NULL);
    }
    return newInstance ;
}


void Regola::anonymize(AnonContext *context, QTreeWidget *treeWidget, const bool doDirect)
{
    Utils::showWaitCursor();
    QWidget *w = treeWidget->window();
    w->setEnabled(false);
    if(doDirect) {
        if(NULL != root()) {
            root()->anonymize(context);
        }
    } else {
        QUndoCommand *undoCommand = new QUndoCommand();
        Element *element = new Element("", "", NULL, NULL);
        root()->copyTo(*element);
        QList<int> path = root()->indexPath();
        element->anonymize(context);
        new ElUpdateCommand(treeWidget, this, element, path, undoCommand);
        addUndo(undoCommand);
    }
    w->setEnabled(true);
    Utils::restoreCursor();
}


QHash<QString, QString> Regola::mapOfSchemaReferences()
{
    return _schemaLocationsByNamespace;
}

QString Regola::roleForKnownUri(const QString &uri)
{
    if(uri == XsltNameSpace) {
        return tr("XSLT eXtensible Stylesheet Language Transformations");
    }
    if(uri == XSDNameSpace) {
        return tr("XML Schema (XSD)");
    }
    if(uri == XSDSchemaInstance) {
        return tr("XML Schema Instance");
    }
    return "";
}

QString Regola::noNameSpaceXsd()
{
    return _noNameSpaceXsd;
}


QHash<QString, QSet<QString> > Regola::allNamespaces()
{
    QHash<QString, QSet<QString> > result;
    QVectorIterator<Element*> it(childItems);
    while(it.hasNext()) {
        Element* element = it.next() ;
        element->allNamespaces(result);
    }
    return result ;
}


RegolaSettings *Regola::getSettings()
{
    RegolaSettings *newSettings = new RegolaSettings();
    if(NULL != newSettings) {
        newSettings->setIndentation(_indent);
        newSettings->setUseIndent(_useIndent);
    }
    return newSettings ;
}

void Regola::restoreSettings(RegolaSettings *settings)
{
    if(NULL != settings) {
        _indent = settings->getIndentation();
        _useIndent = settings->isUseIndent();
        emitIndentationChange();
    }
}

NamespaceManager *Regola::namespaceManager() const
{
    return _namespaceManager;
}

void Regola::setNamespaceManager(NamespaceManager *namespaceManager)
{
    _namespaceManager = namespaceManager;
}

bool Regola::isSavingSortingAttributes()
{
    if(SaveAttributesUsingDefault == _saveAttributesMethod) {
        return Regola::isSaveSortAlphaAttribute();
    }
    return SaveAttributesSortingAlphabetically == _saveAttributesMethod ;
}

bool Regola::isSaveSortAlphaAttribute()
{
    return Config::getBool(Config::KEY_XML_SAVE_SORTATTRIBUTES, false);
}

Regola::ESaveAttributes Regola::saveAttributesMethod() const
{
    return _saveAttributesMethod;
}

void Regola::setSaveAttributesMethod(const ESaveAttributes saveAttributesMethod)
{
    _saveAttributesMethod = saveAttributesMethod;
}

void Regola::setDtd(const QString &dtd)
{
    // remove - possibly old -data
    _docType->setPublicId("");
    _docType->setSystemId("");
    _docType->setDtd(dtd);
    emit docTypeChanged(_docType->docType());
}

QString Regola::dtd()
{
    return _docType->dtd();
}

bool Regola::setNewDTD(const QString &newDtd)
{
    if(newDtd != dtd()) {
        UndoDtd *undoCommand = new UndoDtd(this, newDtd);
        if(NULL == undoCommand) {
            clearUndo();
            setDtd(newDtd);
        } else {
            _undoStack.push(undoCommand);
        }
        setModified(true);
        return true;
    }
    return false;
}

void Regola::updateElement(Element* pElement, const bool updateGUI)
{
    pElement->updateSizeInfo();
    if(NULL != pElement->getUI()) {
        pElement->display(pElement->getUI(), paintInfo);
    }
    if(updateGUI) {
        pElement->forceUpdateGui(true);
    }
    setModified(true);
}

void Regola::setIndentAttributesSettings(const bool forceSettings, const QXmlEditData::EIndentAttributes valueSetting, const int valueCols)
{
    if(_attributesIndentSettings || forceSettings) {
        _indentAttributes = valueSetting ;
        _indentAttributesColumns = valueCols ;
        emitIndentationChange();
    }
}

int Regola::xmlIndentAttributesColumns()
{
    return _indentAttributesColumns ;
}

void Regola::setXmlIndentAttributesColumns(const int value)
{
    _indentAttributesColumns = value ;
    emitIndentationChange();
}

QXmlEditData::EIndentAttributes Regola::xmlIndentAttributesType()
{
    return _indentAttributes ;
}

void Regola::setXmlIndentAttributesType(const QXmlEditData::EIndentAttributes value)
{
    _indentAttributes = value ;
    emitIndentationChange();
}

bool Regola::isUseXmlIndentAttributesSettings()
{
    return _attributesIndentSettings ;
}

void Regola::setUseXmlIndentAttributesSettings(const bool value)
{
    _attributesIndentSettings = value ;
}

bool Regola::isForceDOM() const
{
    return _forceDOM;
}

void Regola::setForceDOM(bool newValue)
{
    _forceDOM = newValue;
}

bool Regola::editElementWrapper(QTreeWidget *treeWidget, Element *newElement, Element *selectedElement)
{
    UndoSimpleEditCommand* undoCommand = new UndoSimpleEditCommand(treeWidget, this, selectedElement->indexPath(), newElement);
    if(NULL == undoCommand) {
        Utils::errorOutOfMem(treeWidget->window());
        return false;
    }
    _undoStack.push(undoCommand);
    return true ;
}

bool Regola::applyEditAndSwapElement(Element *newElement, Element *swapElement, QList<int> path)
{
    Element *selectedElement = findElementByArray(path);
    if(NULL == selectedElement) {
        return false;
    }
    const int beforeAttributesCount = selectedElement->getAttributesList().size();
    selectedElement->copyTo(*swapElement, false);
    newElement->copyTo(*selectedElement, false);
    selectedElement->updateSizeInfo();
    selectedElement->markEdited();
    selectedElement->display(selectedElement->getUI(), paintInfo);
    const int nowAttributesCount = selectedElement->getAttributesList().size();
    if(selectedElement->isElement() && (nowAttributesCount != beforeAttributesCount)) {
        selectedElement->forceUpdateGui(true);
    }
    setModified(true);
    if(NULL == selectedElement->parent()) {
        checkEncoding();
    }
    return true;
}

QList<int> Regola::elementPathForPath(QList<int> &inputPos)
{
    QList<int> result;
    // Now find the pos into the real data
    QVector<Element*> *elements = getChildItems();
    foreach(const int indexToSearch, inputPos) {
        int thisPosAsChild = 0;
        int thisPosAsElement = 0;
        bool found = false;
        foreach(Element *element, *elements) {
            if(element->isElement()) {
                if(thisPosAsElement == indexToSearch) {
                    result.append(thisPosAsChild);
                    elements = element->getChildItems();
                    found = true;
                    break;
                }
                thisPosAsElement++;
            }
            thisPosAsChild ++;
            // error: a position could not be found.
        } // foreach elements
        if(!found) {
            return QList<int>();
        }
    } // foreach pos
    return result;
}
