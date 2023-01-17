/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2013-2023 by Luca Bellonda and individual contributors  *
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

#include "modules/search/searchxquery.h"
#include "modules/search/xqueryelementmodel.h"
#include "utils.h"
#include "regola.h"
#include "findtextparams.h"
#include <QSourceLocation>
#include <QXmlResultItems>
#include <QXmlFormatter>

SearchXQuery::SearchXQuery(QObject *parent) :
    QAbstractMessageHandler(parent), _query(_namePool)
{
    _query.setMessageHandler(this);
}

SearchXQuery::~SearchXQuery()
{
}

QString SearchXQuery::composeQueryString(Regola *regola, FindTextParams &searchInfo)
{
    QMap<QString, QString> nss = regola->namespaces();
    QString queryString ;

    foreach(QString ns, nss.keys()) {
        if(ns.isEmpty()) {
            queryString += QString("declare default element namespace \"%1\";\n").arg(nss[ns]);
        }
    }

    foreach(QString ns, nss.keys()) {
        if(!ns.isEmpty()) {
            queryString += QString("declare namespace %1 = \"%2\";\n").arg(ns).arg(nss[ns]);
        }
    }
    const QString textToFind = getTextToFind(searchInfo);
    const QString textToFindAdjusted = adjustTextToFind(textToFind);

    queryString += QString("declare variable $root external;\n$root%1").arg(textToFindAdjusted);

    return queryString ;
}

QString SearchXQuery::adjustTextToFind(const QString &textToFind)
{
    if(textToFind.startsWith("/")) {
        return textToFind;
    }
    return "/" + textToFind;
}

QString SearchXQuery::getTextToFind(FindTextParams &searchInfo)
{
    return searchInfo.getTextToFind().trimmed();
}

void SearchXQuery::search(Regola *regola, Element *element, FindTextParams &searchInfo)
{
    QXmlResultItems result;
    Element *rootElement = element;
    if(!searchInfo.isLookOnlyChildren()) {
        rootElement = NULL ;
    }
    XQueryElementModel model(regola, rootElement, _namePool);
    QXmlItem rootItem = model.root(QXmlNodeModelIndex());
    _query.bindVariable("root", rootItem);

    QString queryString = composeQueryString(regola, searchInfo);

    //Utils::message(queryString);
    _query.setQuery(queryString);
    if(!_query.isValid()) {
        Utils::error(tr("The syntax of the XPath expression is not valid."));
        return ;
    }

    regola->unhiliteAll();
    bool isCloseUnrelated = searchInfo.isCloseUnrelated();
    if(searchInfo.isCountingOnly()) {
        isCloseUnrelated = false;
    }
    if(isCloseUnrelated) {
        Element *topElement = regola->root();
        if(NULL != topElement) {
            topElement->unexpandRecursive();
        }
    }

    _query.evaluateTo(&result);
    QXmlItem item(result.next());
    while(!item.isNull()) {
        if(item.isNode()) {
            QXmlNodeModelIndex index = item.toNodeModelIndex();
            Element *element = model.indexToElement(index);
            if(NULL != element) {
                D(printf(">>'%s'\n", element->tag().toLatin1().data());)
                D(fflush(stdout);)
                searchInfo.newOccurence(element->selfInfo.totalSize + element->childrenInfo.totalSize);
                if(!searchInfo.isCountingOnly()) {
                    searchInfo.addSelection(element);
                    element->hilite();
                    if(searchInfo.isSelToBookmarks()) {
                        regola->addBookmark(element);
                    }
                }
                // Recurse up tp the root.
                if(!searchInfo.isCountingOnly()) {
                    QTreeWidgetItem * ui = element->getUI();
                    if(isCloseUnrelated && (NULL != ui)) {
                        ui = ui->parent();
                        while(NULL != ui) {
                            if(!ui->isExpanded()) {
                                ui->setExpanded(true);
                            }
                            ui = ui->parent();
                        }
                    }
                }
            }
        }
        item = result.next();
    }
}



void SearchXQuery::searchAndDisplay(Regola *regola, const QString expression)
{
    XQueryElementModel model(regola, NULL, _namePool);
    QXmlItem rootItem = model.root(QXmlNodeModelIndex());
    _query.bindVariable("root", rootItem);
    QMap<QString, QString> nss = regola->namespaces();
    QString queryString = QString("($root)%1").arg(expression);
    foreach(QString ns, nss.keys()) {
        queryString = QString("declare namespace %1 = \"%2\";%3").arg(ns).arg(nss[ns]).arg(queryString);
    }
    _query.setQuery(queryString);

    Q_ASSERT(_query.isValid());
    QByteArray output;
    QBuffer buffer(&output);
    buffer.open(QIODevice::WriteOnly);
    QXmlFormatter serializer(_query, &buffer);
    _query.evaluateTo(&serializer);
    buffer.close();
    QString s = QString::fromUtf8(output.constData());
    printf("%s\n", s.toLatin1().data());
    fflush(stdout);
    Utils::message(s);
}

void SearchXQuery::handleMessage(QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation)
{
    if((QtCriticalMsg == type) || (QtFatalMsg == type) || (QtSystemMsg == type)) {
        Utils::error(QString("%1: location: %2-%3 %4").arg(description).arg(sourceLocation.line()).arg(sourceLocation.column()).arg(identifier.toString()));
    }
}
