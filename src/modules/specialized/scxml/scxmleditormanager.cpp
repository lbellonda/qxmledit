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

bool SCXMLEditorManager::handleEdit(QWidget *parent, QTreeWidget * tree, Regola *regola, Element *element)
{
    init();
    QXName qName ;
    element->qName(&qName);
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLToken *token = _tokenMakager.tokenForName(qName.name);
    if(NULL == token) {
        return false;
    }
    Element *newElement = new Element(NULL);
    if(NULL != newElement) {
        element->copyTo(*newElement, false);
        Utils::TODO_THIS_RELEASE("fare info");
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

bool SCXMLEditorManager::handleInsert(QTreeWidget *tree, Regola *regola, Element *element, const bool isChild, const QString & itemCode)
{
    Utils::TODO_THIS_RELEASE("fare");
    init();
    Element *newElement = new Element(NULL);
    bool goAhead = false;
    SCXMLToken *token = _tokenMakager.tokenForName(itemCode);
    if(NULL == token) {
        return false;
    }
    Element *theParent = (NULL != element) ? element->parent() : NULL ;
    Utils::TODO_THIS_RELEASE("fare info");
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

HandlerForInsert *SCXMLEditorManager::handlerForInsert(Regola *, Element *element, const bool isChild)
{
    init();
    Element *theParent = element ;
    if(!isChild && (NULL != element)) {
        theParent = element->parent();
    }
    QList<SCXMLTokenChild*> tokens ;
    bool parentIsSCXML = false;
    QList<NSContext*> contexts;
    NSContext *lastContext = NULL;

    if((NULL != theParent) && (NULL != element)) {
        lastContext = Regola::buildContextInfo(contexts, theParent);
        QXName qName ;
        element->qName(&qName);
        if(qName.ns == NamespaceManager::SCXMLNamespace) {
            tokens = _tokenMakager.tokensForParentAsList(qName.name, theParent->getChildItems(), lastContext);
            parentIsSCXML = true;
        }
    }
    // as root?
    if(!parentIsSCXML && (NULL == element)) {
        QVector<Element*> emptyList;
        tokens = _tokenMakager.tokensForParentAsList("", &emptyList, NULL);
    }
    EMPTYPTRLIST(contexts, NSContext);

    const QString SCXML = QObject::tr("SCXML 1.1 state machine");

    HandlerForInsert *category = new HandlerForInsert();
    category->handler = this;
    category->nameSpace = NamespaceManager::XIncludeNamespace ;
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
    Utils::TODO_THIS_RELEASE("fare, poi elimina sotto");
}
/*
bool SCXMLEditorManager::findInfoStates(Regola *regola, SCXMLInfo *info)
{
    if(NULL != regola->root()) {
        return findInfoStates(regola->root(), info, NULL, false, NULL);
    }
    return false;
}

SCXMLState *SCXMLEditorManager::addStateForInfo(SCXMLInfo *info, SCXMLState *currentState, SCXMLState *state)
{
    if(NULL != currentState) {
        currentState->addChild(state);
    } else {
        info->addChild(state);
    }
    return state ;
}

bool SCXMLEditorManager::findInfoStates(Element *element, SCXMLInfo *info, NSContext *parent, const bool rootFound, SCXMLState *currentState)
{
    NSContext thisContext(parent);
    if(element->isElement()) {
        element->handleNamespace(&thisContext);
        QString name, prefix;
        XmlUtils::decodeQualifiedName(element->tag(), prefix, name);
        const QString &elementNamespace = thisContext.uriFromPrefix(prefix);
        bool doChildren = false;
        bool haveRoot = rootFound ;
        if(elementNamespace == NamespaceManager::SCXMLNamespace) {
            if(!rootFound) {
                if(name == SCXMLRootToken::SCXMLToken::Tag_scxml) {
                    doChildren = true;
                    haveRoot = true;
                }
            } else {
                if(name == SCXMLRootToken::SCXMLToken::Tag_state) {
                    SCXMLState *state = new SCXMLState();
                    state->setElement(element);
                    state->setId(element->getAttributeValue(SCXMLstateToken::A_id));
                    currentState = addStateForInfo(info, currentState, state);
                    doChildren = true ;
                } else if(name == SCXMLRootToken::SCXMLToken::Tag_parallel) {
                    SCXMLState *state = new SCXMLState();
                    state->setElement(element);
                    state->setId(element->getAttributeValue(SCXMLparallelToken::A_id));
                    state->setParallel(true);
                    currentState = addStateForInfo(info, currentState, state);
                    doChildren = true;
                }
            }
        } else {
            if(!rootFound) {
                doChildren = true;
            }
        }
        if(doChildren) {
            foreach(Element *child, *element->getChildItems()) {
                findInfoStates(child, info, &thisContext, haveRoot || rootFound, currentState);
            }
        }
    }
    return true;
}
*/
