/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

// all about regola I/O

#include "regoladefinitions.h"
#include "undo/elupdateelementcommand.h"
#include "xmlsavecontext.h"
#include "modules/xml/xmlloadcontext.h"

Element *Regola::assegnaValori(QDomNode &node, Element *parent, QVector<Element*> *collection)
{
    _isCrapCacheNSActivated = false;
    D(printf("sono in assegna \n"));
    int nodi = node.childNodes().count();
    // this it the only legal root item
    Element *elem = NULL ;
    D(printf("sono in assegna con %d nodi\n", nodi));
    bool isMixedContent = false ;
    bool isElement = false ;
    bool isText = false ;
    QDomNodeList childNodes = node.childNodes();

    if(_useMixedContent) {
        isMixedContent = true ;
    } else {
        // look for mixed content
        for(int i = 0 ; i < nodi ; i ++) {
            QDomNode childNode = childNodes.item(i) ;
            if(childNode.isElement()) {
                if(isText) {
                    isMixedContent = true ;
                    break;
                }
                isElement = true ;
            } else if(childNode.isText()) {
                if(isElement) {
                    isMixedContent = true ;
                    break;
                }
                isText = true ;
            } else if(childNode.isCDATASection()) {
                if(isElement) {
                    isMixedContent = true ;
                    break;
                }
                isText = true ;
            }
        }
    }
    for(int i = 0 ; i < nodi ; i ++) {
        QDomNode childNode = childNodes.item(i) ;
        D(printf("trovato %d %s=%s\n", childNode.nodeType(), childNode.nodeName().toAscii().data(), childNode.nodeValue().toAscii().data()));

        if(childNode.isElement()) {
            //printf("trovato testo %s\n", childNode.nodeValue().toAscii().data());
            QDomElement element = childNode.toElement();
            elem = new Element(addNameToPool(element.tagName()), "", this, parent) ;

            if(element.hasAttributes()) {
                QDomNamedNodeMap attrList = element.attributes();
                int attrMax = attrList.count();
                for(int a = 0 ; a < attrMax ; a ++) {
                    QDomAttr attr = attrList.item(a).toAttr();
                    Attribute *attribute = new Attribute(getAttributeString(attr.name()), getAttributeString(attr.value()));
                    elem->attributes.append(attribute);
                }
            }
            D(printf(" add child %d %s\n", i, element.tagName().toAscii().data()));
            collection->append(elem);
            assegnaValori(childNode, elem, elem->getChildItems());
        } else if(childNode.isCDATASection()) {
            QDomCDATASection text = childNode.toCDATASection();
            if(isMixedContent) {
                assignMixedContentText(parent, text.data(), true, collection);
            } else {
                TextChunk *txt = new TextChunk(true, text.data());
                parent->addTextNode(txt);
            }
        } else if(childNode.isText()) {
            QDomText text = childNode.toText();
            if(isMixedContent) {
                assignMixedContentText(parent, text.data(), false, collection);
            } else {
                TextChunk *txt = new TextChunk(false, text.data());
                parent->addTextNode(txt);
            }
        } else if(childNode.isProcessingInstruction()) {
            QDomProcessingInstruction procInstrNode = childNode.toProcessingInstruction();
            Element *procInstr = new Element(this, Element::ET_PROCESSING_INSTRUCTION, parent) ;
            procInstr->setPIData(procInstrNode.data());
            procInstr->setPITarget(procInstrNode.target());
            collection->append(procInstr);
            assegnaValori(childNode, procInstr, procInstr->getChildItems());
        } else if(childNode.isComment()) {
            QDomComment commentNode = childNode.toComment();
            Element *comment = new Element(this, Element::ET_COMMENT, parent) ;
            comment->setText(commentNode.data());
            collection->append(comment);
            assegnaValori(childNode, comment, comment->getChildItems());
        } else if(childNode.isEntity()) {
            Utils::error(tr("This XML contanins an entity. Entities are not supported in editing."));
        } else if(childNode.isEntityReference()) {
            Utils::error(tr("This XML contanins an entity reference. Entity references are not supported in editing."));
            QDomEntityReference reference = childNode.toEntityReference();
            QString refStr = QString("&%1;").arg(reference.nodeName());
            if(isMixedContent) {
                assignMixedContentText(parent, refStr, false, collection);
            } else {
                TextChunk *txt = new TextChunk(false, refStr);
                parent->addTextNode(txt);
            }
        } //if
    }//for
    return elem;
}//assegnaValori()

/*!
 * \brief Regola::setChildrenTree recurse if top level, until the end of stream
 * \param context
 * \param xmlReader
 * \param parent
 * \param collection
 * \return
 */
bool Regola::setChildrenTreeFromStream(XMLLoadContext *context, QXmlStreamReader *xmlReader,
                                       Element *parent, QVector<Element*> *collection, const bool isTopLevel)
{
    _isCrapCacheNSActivated = false;
    // this it the only legal root item
    bool isMixedContent = false ;
    if(_useMixedContent) {
        isMixedContent = true ;
    }
    int nodeIndex = -1 ;
    while(!xmlReader->atEnd()) {
        nodeIndex++;
        xmlReader->readNext();
        if(xmlReader->hasError()) {
            return context->setErrorFromReader(xmlReader);
        }
        D(printf("trovato %d \n", xmlReader->tokenType()));
        switch(xmlReader->tokenType()) {
        default:
        case QXmlStreamReader::Invalid:
            return context->setErrorFromReader(xmlReader);
            break;
        case QXmlStreamReader::DTD:
            setDocType(xmlReader->dtdName().toString(), xmlReader->dtdSystemId().toString(), xmlReader->dtdPublicId().toString(), xmlReader->text().toString());
            context->setIsAfterDTD(true);
            D(printf("DTD:%s", xmlReader->text().toString().toLatin1().data());)
            break;
        case QXmlStreamReader::StartDocument: {
            QString encoding = xmlReader->documentEncoding().toString();
            context->setEncoding(encoding);
        }
        // ignore at the moment
        break;
        case QXmlStreamReader::EndDocument:
            if(!isTopLevel) {
                return context->setError(tr("Unexpected end document"), xmlReader);
            }
            return true;
            break;
        case QXmlStreamReader::EndElement:
            if(!isTopLevel) {
                if(!isMixedContent) {
                    parent->handleMixedContentToInnerText();
                }
                if(xmlReader->hasError()) {
                    context->setErrorFromReader(xmlReader);
                }
                return context->isOk();
            }
            break;
        case QXmlStreamReader::StartElement: {
            context->setFirstElementSeen(true);
            Element *elem = new Element(addNameToPool(xmlReader->qualifiedName().toString()), "", this, parent) ;
            QXmlStreamAttributes streamAttributes = xmlReader->attributes();
            foreach(QXmlStreamAttribute streamAttribute, streamAttributes) {
                Attribute *attribute = new Attribute(
                    getAttributeString(streamAttribute.qualifiedName().toString()),
                    getAttributeString(streamAttribute.value().toString()));
                elem->attributes.append(attribute);
            }
            D(printf(" add child %d %s\n", i, elem.tagName().toAscii().data()));
            collection->append(elem);
            if(!setChildrenTreeFromStream(context, xmlReader, elem, elem->getChildItems(), false)) {
                return false;
            }
            isMixedContent = true ;
        }
        break;
        case QXmlStreamReader::Characters:
            if(!xmlReader->isWhitespace() || xmlReader->isCDATA()) {
                assignMixedContentText(parent, xmlReader->text().toString(), xmlReader->isCDATA(), collection);
            }
            break;
        case QXmlStreamReader::ProcessingInstruction: {
            Element *procInstr = new Element(this, Element::ET_PROCESSING_INSTRUCTION, parent) ;
            procInstr->setPIData(xmlReader->processingInstructionData().toString());
            procInstr->setPITarget(xmlReader->processingInstructionTarget().toString());
            collection->append(procInstr);
            isMixedContent = true ;
        }
        break;
        case QXmlStreamReader::Comment: {
            Element *comment = new Element(this, Element::ET_COMMENT, parent) ;
            comment->setText(xmlReader->text().toString());
            collection->append(comment);
            if(!context->firstElementSeen() && !context->isAfterDTD()) {
                context->addFirstComment(comment);
            }
            isMixedContent = true ;
        }
        break;
        case QXmlStreamReader::EntityReference:
            Utils::error(tr("This XML contanins an entity reference.\nEntity references are not supported at the moment."));
            return false;
            break;
        } //if
    }//for
    // never reached
    return true;
}//setChildrenTreeFromStream()


bool Regola::readFromStream(XMLLoadContext *context, QXmlStreamReader *xmlReader)
{
    xmlReader->setNamespaceProcessing(false);
    bool result = setChildrenTreeFromStream(context, xmlReader, NULL, &childItems, true);
    foreach(Element * child, childItems) {
        if(child->isElement()) {
            rootItem = child;
            break;
        }
    }

    if(result) {

        // try to read the xml entity declaration using the info as stated in appendix F (Non normative) of the specifications.
        if(!decodePreamble(xmlReader, context->encoding())) {
            result = false;
        }
        if(!filterCommentsAfterReading(context)) {
            return false;
        }
        checkEncoding(true);
        checkValidationReference();
    }
    return result;
}

/*!
 * \brief Regola::filterCommentsAfterReading this piece of clumsy code is used
 * to remove comments in dtd send as comments in the XML itself by the buggy Qt parser
 * \param context
 * \return
 */
bool Regola::filterCommentsAfterReading(XMLLoadContext *context)
{
    QList<Element*>toRemove;
    if(!context->firstComments().isEmpty() && context->isAfterDTD() && !_docType->dtd().isEmpty()) {
        QString dtdStripped = _docType->dtd();
        int lastIndex = 0 ;
        foreach(Element * comment, context->firstComments()) {
            QString cmt = "<!--" + comment->getComment();
            int indexOfString = dtdStripped.indexOf(cmt, lastIndex);
            if(indexOfString >= 0) {
                lastIndex = indexOfString + cmt.length() ;
                toRemove.append(comment);
            }
        } // foreach
        // remove the elements
        foreach(Element * comment, toRemove) {
            int pos = childItems.indexOf(comment);
            if(pos >= 0) {
                childItems.remove(pos);
                delete comment ;
            }
        }
    }
    return true ;
}

/*
static bool compareFour(QByteArray &array, const char ich1, const char ich2, const char ich3, const char ich4)
{
    char ch1 = array[0];
    char ch2 = array[1];
    char ch3 = array[2];
    char ch4 = array[3];
    return ((ich1==ch1) && (ich2==ch2) && (ich3 ==ch3)&& (ich4==ch4));
}

const int MIB_UTF8=106;
const int MIB_UTF32BOM=1017;
*/

bool Regola::decodePreamble(QXmlStreamReader *xmlReader, const QString &inputEncoding)
{
    QString encoding = inputEncoding;
    if(encoding.isEmpty()) {
        encoding = "UTF-8";
    }
    QIODevice *ioDevice = xmlReader->device();
    if(!ioDevice->reset()) {
        return false;
    }
    // try to read how much is possible
    QByteArray charsRead;
    charsRead = ioDevice->read(1000);
    qint64 devSize = ioDevice->size();
    qint64 aMin = (devSize < 1000) ? devSize : 1000;
    if(charsRead.size() < aMin) {
        return false;
    }
    if(charsRead.size() < 12) {
        return true;
    }
    QTextCodec *codec = QTextCodec::codecForName(encoding.toLatin1());
    if(NULL == codec) {
        return false;
    }
    QTextDecoder *decoder = codec->makeDecoder();
    if(NULL == decoder) {
        return false;
    }
    QString firstLines = decoder->toUnicode(charsRead) ;
    QString firstLinesUC = firstLines.toUpper();
    int startIndex = firstLinesUC.indexOf("<?XML") ;
    int endIndex = firstLinesUC.indexOf("?>") ;
    if((startIndex < 2) && (startIndex >= 0) && (endIndex >= 0)) {
        QString declaration = firstLines.mid(startIndex, endIndex - startIndex + 2);
        QString target = declaration.mid(2, 3);
        QString data = declaration.mid(5, declaration.length() - 7).trimmed();
        Element *procInstr = new Element(this, Element::ET_PROCESSING_INSTRUCTION, NULL) ;
        procInstr->setPIData(data);
        procInstr->setPITarget(target);
        childItems.insert(0, procInstr);
        //printf("%s\n", data.toLatin1().data());
    }
    //delete codec;
    delete decoder;
    return true;
    // we can follow precisely the specifications, but the problem is that the stream has already been decoded for us
}
/*
if( !ioDevice->reset() ) {
    return false;
}
// try to read how much is possible
QByteArray charsRead;
charsRead = ioDevice->read(1000);
if( ioDevice->isError() ) {
    return false;
}

//test for bom
int mibId = MIB_UTF8;
if( ioBuffer.size() < 4 ) {
    return true ;
}
int skipBytes = 4;
// With a Byte Order Mark:
if( compareFour(charsRead, 0x00, 0x00, 0xFE, 0xFF) ) {
     mibId = MIB_UTF32BOM;
} else if( compareFour(charsRead, 0xFF, 0xFE, 0x00, 0x00 ) ) {
     mibId = MIB_UTF32BOM;
} else if( compareFour(charsRead, 0x00, 0x00, 0xFF, 0xFE ) ) {
     mibId = MIB_UTF32BOM;
} else if( compareFour(charsRead, 0xFE, 0xFF, 0x00, 0x00 ) ) {
     mibId = MIB_UTF32BOM;
} else if( compareTwo(charsRead, 0xFF, 0xFE ) ) {
    skipBytes = 2;
     mibId = MIB_UTF16BE_BOM;
} else if( compareTwo(charsRead, 0xFE, 0xFF ) ) {
    skipBytes = 2;
     mibId = MIB_UTF16LE_BOM;
} else if( compareThree(charsRead, 0xEF, 0xBB, 0xBF ) ) {
    skipBytes = 3;
     mibId = MIB_UTF8_BOM;
}
}
}

*/

bool Element::handleMixedContentToInnerText()
{
    bool isChanged = false;
    foreach(Element * child, childItems) {
        if(!child->isText()) {
            return false;
        }
    }

    foreach(Element * child, childItems) {
        TextChunk *txt = new TextChunk(child->isCDATA(), child->text);
        addTextNode(txt);
        delete child;
        isChanged = true;
    }
    childItems.clear();
    return isChanged ;
}
