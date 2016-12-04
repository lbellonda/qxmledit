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

#include "regola.h"
#include "scxmltokenmanager.h"
#include "modules/specialized/scxml/scxmltokenloader.h"
#include "modules/namespace/nscontext.h"
#include "modules/xsd/namespacemanager.h"
#include "xmlutils.h"
#include "utils.h"

#define RESOURCE_SCXML_TOKEN_DATA   ":/SCXML/scxmltokens.xml"

SCXMLTokenManager::SCXMLTokenManager()
{
    Utils::TODO_THIS_RELEASE("fare traduzioni delle descrizioni come ts esterni");
    _inited = false ;
}

SCXMLTokenManager::~SCXMLTokenManager()
{
}

SCXMLToken *SCXMLTokenManager::tokenForName(const QString &itemCode)
{
    if(_tokens.contains(itemCode)) {
        return _tokens[itemCode] ;
    }
    return NULL ;
}

QList<SCXMLTokenChild*> SCXMLTokenManager::tokensForParentAsList(const QString &parentTag, QVector<Element*> *children, NSContext *parentContext)
{
    QList<SCXMLTokenChild*> result;
    QString parentPrefix, parentName;

    if(!parentTag.isEmpty()) {
        XmlUtils::decodeQualifiedName(parentTag, parentPrefix, parentName);
    }
    // get all the items that can have the parent
    SCXMLToken *parent = _tokens[parentName];
    if(NULL != parent) {
        foreach(SCXMLTokenChild *child, parent->children()) {
            if(child->max() == SCXMLTokenChild::Unbounded) {
                result.append(child);
            } else {
                // check children
                int count = 0 ;
                bool failed = false;
                foreach(Element *childElement, *children) {
                    if(childElement->isElement()) {
                        NSContext context(parentContext);
                        childElement->handleNamespace(&context);
                        QString prefix, name;
                        XmlUtils::decodeQualifiedName(childElement->tag(), prefix, name);
                        const QString &elementNamespace = context.uriFromPrefix(prefix);
                        if((NamespaceManager::SCXMLNamespace == elementNamespace) && (name == child->name())) {
                            count ++ ;
                            if(count > child->max()) {
                                failed = true ;
                            }
                        } // if same ns and tag
                    } //if child is elem
                } // foreach child elm
                if(!failed) {
                    result.append(child);
                }
            } // if not unbounded
        } // foreach child
    } // if parent
    return result;
}

void SCXMLTokenManager::init()
{
    if(_inited) {
        return ;
    }
    _inited = true;
    SCXMLTokenLoader loader;
    if(!loader.load(RESOURCE_SCXML_TOKEN_DATA, &_tokens)) {
        Utils::error(QObject::tr("Error in SCXML engine initialization."));
    }
    Utils::TODO_THIS_RELEASE("fare");
}
