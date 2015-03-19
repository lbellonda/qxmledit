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


#ifndef XQUERYELEMENTMODEL_H
#define XQUERYELEMENTMODEL_H

#include <QSimpleXmlNodeModel>
#include <QSharedDataPointer>
#include "regola.h"

class XQueryElementModel : public QSimpleXmlNodeModel
{
    const QUrl      _baseURI;
    QHash<QString, QXmlName> _names;
    bool _hasExplicitRoot;
    Regola          *_regola;
    Element     *_rootElement;

    QXmlNodeModelIndex getSibling(Element *element, const bool isNext) const ;
    bool indexIsAttribute(const QXmlNodeModelIndex &index)const;
    bool indexIsElement(const QXmlNodeModelIndex &index) const;
    bool indexIsText(const QXmlNodeModelIndex &index) const;

    QXmlNodeModelIndex getTextSibling(Element *element, const int newIndex) const;
    QXmlNodeModelIndex nextFromSimpleAxisText(SimpleAxis axis, const QXmlNodeModelIndex& index, const int textIndex) const;

public:
    explicit XQueryElementModel(Regola *regola, Element *rootElement, QXmlNamePool &namePool);
    virtual ~XQueryElementModel();

    Element *indexToElement(const QXmlNodeModelIndex &index)const;
    Element *indexToTrueElement(const QXmlNodeModelIndex &index)const;

    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex &n1, const QXmlNodeModelIndex &n2) const;
    virtual QXmlName name(const QXmlNodeModelIndex &n) const;
    virtual QUrl documentUri(const QXmlNodeModelIndex &n) const;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &n) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &n) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex &n) const;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex&) const;
    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis, const QXmlNodeModelIndex&) const;


private:
    //QSharedDataPointer<XQueryElementModelData> data;
};

#endif // XQUERYELEMENTMODEL_H
