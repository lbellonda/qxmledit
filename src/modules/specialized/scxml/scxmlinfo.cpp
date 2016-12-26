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

#include "scxmlinfo.h"
#include "regola.h"
#include "modules/namespace/nscontext.h"
#include "modules/xsd/namespacemanager.h"
#include "modules/specialized/scxml/scxmltokenmanager.h"
#include "xmlutils.h"
#include "utils.h"

Element *SCXMLState::element() const
{
    return _element;
}

void SCXMLState::setElement(Element *element)
{
    _element = element;
}

QString SCXMLState::id() const
{
    return _id;
}

void SCXMLState::setId(const QString &id)
{
    _id = id;
}

bool SCXMLState::isParallel() const
{
    return _parallel;
}

void SCXMLState::setParallel(bool parallel)
{
    _parallel = parallel;
}

QList<SCXMLState *> SCXMLState::children() const
{
    return _children;
}

SCXMLState::SCXMLState()
{
    _element = NULL ;
    _parallel = false ;
}

SCXMLState::~SCXMLState()
{
    EMPTYPTRLIST(_children, SCXMLState);
}

void SCXMLState::addChild(SCXMLState *child)
{
    _children.append(child);
}

void SCXMLState::allStates(QStringList &result)
{
    result << id();
    foreach(SCXMLState *state, _children) {
        state->allStates(result);
    }
}

//-------------

QList<SCXMLState *> SCXMLInfo::children() const
{
    return _children;
}

SCXMLInfo::SCXMLInfo()
{
}

SCXMLInfo::~SCXMLInfo()
{
    EMPTYPTRLIST(_children, SCXMLState);
}

void SCXMLInfo::addChild(SCXMLState *child)
{
    _children.append(child);
}

QStringList SCXMLInfo::allStates()
{
    Utils::TODO_THIS_RELEASE("fare SCXMLInfo::allStates");
    QStringList result ;
    foreach(SCXMLState *state, _children) {
        state->allStates(result);
    }
    return result;
    /*QStringList x;
    x << "a" << "b" << "c" ;
    return x ;*/
}

//---------------------------------------------------------------

bool SCXMLInfo::findInfoStates(Regola *regola, SCXMLInfo *info)
{
    if(NULL != regola->root()) {
        return findInfoStates(regola->root(), info, NULL, false, NULL);
    }
    return false;
}

SCXMLState *SCXMLInfo::addStateForInfo(SCXMLInfo *info, SCXMLState *currentState, SCXMLState *state)
{
    if(NULL != currentState) {
        currentState->addChild(state);
    } else {
        info->addChild(state);
    }
    return state ;
}

bool SCXMLInfo::findInfoStates(Element *element, SCXMLInfo *info, NSContext *parent, const bool rootFound, SCXMLState *currentState)
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
