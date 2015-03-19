/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013 by Luca Bellonda and individual contributors       *
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


#include "xqueryelementmodel.h"
#include "utils.h"


#define ZERO_DATA   (0)

XQueryElementModel::XQueryElementModel(Regola *regola, Element *rootElement, QXmlNamePool &namePool) : QSimpleXmlNodeModel(namePool)
{
    _regola = regola;
    if(NULL != rootElement) {
        _rootElement = rootElement;
        _hasExplicitRoot = true ;
    } else {
        _rootElement = regola->root();
        _hasExplicitRoot = false ;
    }
}

XQueryElementModel::~XQueryElementModel()
{
}


Element *XQueryElementModel::indexToElement(const QXmlNodeModelIndex &index) const
{
    return static_cast<Element*>(index.internalPointer());
}

bool XQueryElementModel::indexIsAttribute(const QXmlNodeModelIndex &index) const
{
    if(index.additionalData() > 0) {
        return true;
    }
    return false;
}

bool XQueryElementModel::indexIsText(const QXmlNodeModelIndex &index) const
{
    if(index.additionalData() < 0) {
        return true;
    }
    return false;
}

bool XQueryElementModel::indexIsElement(const QXmlNodeModelIndex &index) const
{
    if(0 == index.additionalData()) {
        return true;
    }
    return true;
}

Element *XQueryElementModel::indexToTrueElement(const QXmlNodeModelIndex &index) const
{
    if(0 == index.additionalData()) {
        return static_cast<Element*>(index.internalPointer());
    }
    return NULL ;
}

//----------------------------------------------------------------------------------------------

QXmlNodeModelIndex::DocumentOrder XQueryElementModel::compareOrder(const QXmlNodeModelIndex & /*index1*/, const QXmlNodeModelIndex & /*index2*/) const
{
    return QXmlNodeModelIndex::Follows;
}

QXmlName XQueryElementModel::name(const QXmlNodeModelIndex &index) const
{
    Element *element = indexToElement(index);
    if(indexIsAttribute(index)) {
        int attributeIndex = index.additionalData() - 1;
        QString nameString = element->attributes.at(attributeIndex)->name;
        //if( !_names.contains(nameString)) {
        QXmlName name = QXmlName(namePool(), nameString);
        //    _names.insert(nameString, name);
        //}
        //QXmlName name = _names[nameString];
        return name;
    } else if(indexIsElement(index)) {
        if(NULL == element) {
            return QXmlName();
        }
        if(element->getType() == Element::ET_ELEMENT) {
            QString tag = element->tag();
            //if( !_names.contains(tag)) {
            QXmlName name = QXmlName(namePool(), tag);
            //    _names.insert(tag, name);
            //}
            //QXmlName name = _names[tag];
            return name;
        }
    }
    return QXmlName();
}

QUrl XQueryElementModel::documentUri(const QXmlNodeModelIndex & /*index*/) const
{
    return _baseURI ;
}

QXmlNodeModelIndex::NodeKind XQueryElementModel::kind(const QXmlNodeModelIndex &index) const
{
    if(indexIsAttribute(index)) {
        return QXmlNodeModelIndex::Attribute;
    } else if(indexIsText(index)) {
        return QXmlNodeModelIndex::Text ;
    }
    Element *element = indexToTrueElement(index);
    if(NULL == element) {
        return QXmlNodeModelIndex::Document;
    }
    switch(element->getType()) {
    case Element::ET_ELEMENT:
        return QXmlNodeModelIndex::Element;
    case Element::ET_PROCESSING_INSTRUCTION:
        return QXmlNodeModelIndex::ProcessingInstruction;
    case Element::ET_COMMENT:
        return QXmlNodeModelIndex::Comment;
    case Element::ET_TEXT:
        return QXmlNodeModelIndex::Text;
    default:
        Q_ASSERT(false);
        // fake return
        Utils::error("Internal error.");
        return QXmlNodeModelIndex::Element;
    }
}

QXmlNodeModelIndex XQueryElementModel::root(const QXmlNodeModelIndex & /*index*/) const
{
    return createIndex((void *)(_hasExplicitRoot ? _rootElement : NULL), ZERO_DATA);
}

QVariant XQueryElementModel::typedValue(const QXmlNodeModelIndex &index) const
{
    Element *element = indexToElement(index);
    if(indexIsAttribute(index)) {
        int attributeIndex = index.additionalData() - 1;
        QString value = element->attributes.at(attributeIndex)->value;
        return value;
    }
    if(indexIsText(index)) {
        int textIndex = -index.additionalData() - 1;
        QString value = element->getTextChunks().at(textIndex)->text;
        return value;
    }

    if(NULL == element) {
        return QVariant();
    }
    switch(Element::ElType(element->getType())) {
    case Element::ET_ELEMENT:
        return QString(element->tag());
    case Element::ET_PROCESSING_INSTRUCTION:
        return QString(element->getPIData());
    case Element::ET_COMMENT:
        return QString(element->getComment());
    case Element::ET_TEXT:
        return QString(element->text);
    default:
        return QVariant();
    }

}

QVector<QXmlNodeModelIndex> XQueryElementModel::attributes(const QXmlNodeModelIndex& index) const
{
    Element *element = indexToTrueElement(index);
    QVector<QXmlNodeModelIndex> result;
    if(NULL == element) {
        return result;
    }

    int indexNum = 1 ;
    int numAttr = element->getAttributesList().size();
    for(int i = 0 ; i < numAttr ; i ++) {
        result.append(createIndex(element, indexNum));
        indexNum ++ ;
    }
    return result;
}


QXmlNodeModelIndex XQueryElementModel::getSibling(Element *element, const bool isNext) const
{
    Element *theParent = element->parent();
    // root item
    if(NULL == theParent) {
        return QXmlNodeModelIndex();
    }

    QVector<Element *>children = element->parent()->getOnlyElementChildren();
    int index = children.indexOf(element);
    if(isNext) {
        index ++ ;
    } else {
        index -- ;
    }

    if((index >= 0) && (index < children.size())) {
        return createIndex(children.at(index), ZERO_DATA);
    }
    return QXmlNodeModelIndex();
}

QXmlNodeModelIndex XQueryElementModel::nextFromSimpleAxis(SimpleAxis axis, const QXmlNodeModelIndex& index) const
{
    if(indexIsAttribute(index)) {
        return QXmlNodeModelIndex();
    }
    if(indexIsText(index)) {
        int textIndex = -index.additionalData() - 1;
        return nextFromSimpleAxisText(axis, index, textIndex);
    }

    Element *element = indexToTrueElement(index);
    if(NULL == element) {
        switch(axis) {
        case FirstChild: {
            if(NULL == _rootElement) {
                return QXmlNodeModelIndex();
            } else {
                return createIndex(_rootElement, ZERO_DATA);
            }
        }
        default:
        case Parent:
        case NextSibling:
        case PreviousSibling:
            return QXmlNodeModelIndex();
        }
    }
    switch(axis) {
    case Parent:
        return createIndex(element->parent(), ZERO_DATA);

    case FirstChild: {
        QVector<Element *> *children = element->getChildItems();
        if(children->isEmpty()) {
            if(element->getTextChunksNumber() > 0) {
                return createIndex(element, -1);
            }
            return QXmlNodeModelIndex();
        } else {
            return createIndex(children->first(), ZERO_DATA);
        }
    }
    case NextSibling:
        return getSibling(element, true);

    case PreviousSibling:
        return getSibling(element, false);

    default:
        return QXmlNodeModelIndex();
    }
}

QXmlNodeModelIndex XQueryElementModel::nextFromSimpleAxisText(SimpleAxis axis, const QXmlNodeModelIndex& index, const int textIndex) const
{
    Element *element = indexToElement(index);
    Q_ASSERT(NULL != element);
    switch(axis) {
    case Parent:
        return createIndex(element, ZERO_DATA);

    case FirstChild:
        return QXmlNodeModelIndex();

    case NextSibling:
        return getTextSibling(element, textIndex + 1);

    case PreviousSibling:
        return getTextSibling(element, textIndex - 1);

    default:
        return QXmlNodeModelIndex();
    }
}


QXmlNodeModelIndex XQueryElementModel::getTextSibling(Element *element, const int newIndex) const
{
    if((newIndex >= 0) && (newIndex < element->getTextChunksNumber())) {
        return createIndex(element, -newIndex - 1);
    }
    return QXmlNodeModelIndex();
}
