/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016 by Luca Bellonda and individual contributors       *
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

#include "scxmleditormanager.h"
#include "regola.h"
#include "undo/elinsertcommand.h"
#include "xmlutils.h"
#include "modules/namespace/nscontext.h"
#include "modules/specialized/scxml/scxmlinfo.h"
#include <QMap>
#include "utils.h"

SCXMLEditorManager::SCXMLEditorManager()
{
    _inited = false;
}

SCXMLEditorManager::~SCXMLEditorManager()
{
}

bool SCXMLEditorManager::handleEdit(QWidget *parent, XmlEditWidget *, QTreeWidget * tree, Regola *regola, Element *element)
{
    init();
    QXName qName ;
    element->qName(&qName);
    SCXMLToken *token = _tokenMakager.tokenForName(qName.name);
    if(NULL == token) {
        return false;
    }
    Element *newElement = new Element(NULL);
    if(NULL != newElement) {
        element->copyTo(*newElement, false);
        SCXMLInfo info;
        SCXMLInfo::findInfoStates(regola, &info);
        if(token->editToken(parent, &info, regola, false, false, newElement, element, element->parent())) {
            if(regola->editElementWrapper(tree, newElement, element)) {
                return true ;
            } else {
                Utils::error(parent, QObject::tr("Error applying the editing."));
            }
        }
        delete newElement ;
    }
    return false;
}

bool SCXMLEditorManager::handleInsert(XmlEditWidget *, QTreeWidget *tree, Regola *regola, Element *element, const bool isChild, const QString & itemCode)
{
    init();
    Element *newElement = new Element(NULL);
    bool goAhead = false;
    SCXMLToken *token = _tokenMakager.tokenForName(itemCode);
    if(NULL == token) {
        return false;
    }
    Element *theParent = (NULL != element) ? element->parent() : NULL ;
    SCXMLInfo info;
    SCXMLInfo::findInfoStates(regola, &info);
    if(token->editToken(tree->window(), &info, regola, true, isChild, newElement, element, theParent)) {
        goAhead = true ;
    }
    if(goAhead) {
        return insertAction(tree, regola, element, newElement, isChild);
    }
    delete newElement ;
    return false ;
}

HandlerForInsert *SCXMLEditorManager::handlerForInsert(XmlEditWidget *, Regola *, Element *element, const bool isChild)
{
    init();
    Element *theParent = element ;
    if(!isChild && (NULL != element)) {
        theParent = element->parent();
    }
    QList<SCXMLTokenChild*> tokens ;
    bool parentIsSCXML = false;
    bool isAtTop = false;
    QList<NSContext*> contexts;
    NSContext *lastContext = NULL;

    if((NULL != theParent) && (NULL != element)) {
        lastContext = Regola::buildContextInfo(contexts, theParent);
        QXName qName ;
        theParent->qName(&qName);
        if(qName.ns == NamespaceManager::SCXMLNamespace) {
            tokens = _tokenMakager.tokensForParentAsList(qName.name, theParent->getChildItems(), lastContext);
            parentIsSCXML = true;
        }
    } else if((NULL == theParent) && (NULL != element)) {
        // at top
        isAtTop = true ;
    }
    // as root?
    if(!parentIsSCXML && ((NULL == element) || isAtTop)) {
        QVector<Element*> emptyList;
        tokens = _tokenMakager.tokensForParentAsList("", &emptyList, NULL);
    }
    EMPTYPTRLIST(contexts, NSContext);

    const QString SCXML = QObject::tr("SCXML 1.1 state machine");

    HandlerForInsert *category = new HandlerForInsert();
    category->handler = this;
    category->nameSpace = NamespaceManager::SCXMLNamespace ;
    category->name = SCXML ;
    QMap<QString, SCXMLTokenChild*> sortedNames ;
    foreach(SCXMLTokenChild *tokenChild, tokens) {
        sortedNames.insert(tokenChild->name(), tokenChild);
    }
    foreach(const QString &tokenKey, sortedNames.keys()) {
        SCXMLTokenChild *tokenChild = sortedNames[tokenKey];
        SingleHandlerForInsert *s = new SingleHandlerForInsert();
        s->name = tokenChild->name();
        s->id = tokenChild->name();
        s->description = tokenChild->description();
        category->elements.append(s);
    }
    return category ;
}

void SCXMLEditorManager::init()
{
    if(_inited) {
        return ;
    }
    _inited = true;
    _tokenMakager.init();
}

bool SCXMLEditorManager::isElementSCXML(Element *element)
{
    init();
    QXName qName ;
    element->qName(&qName);
    SCXMLToken *token = _tokenMakager.tokenForName(qName.name);
    if(NULL == token) {
        return false;
    }
    return true;
}
