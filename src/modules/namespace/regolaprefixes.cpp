/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2016-2018 by Luca Bellonda and individual contributors  *
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
#include "xmlutils.h"
#include "utils.h"
#include "modules/namespace/nscontext.h"

//If the bookmarks are nested, the result is the list of the top levels.
QList<Element*> Regola::getUniqueBookmarksElements(const TargetSelection::Type type)
{
    QList<Element*> result ;
    foreach(Bookmark * b, bookmarks.getBookmarks()) {
        Element * e = b->getElement();
        if(NULL != e) {
            result.append(e);
        }
    }
    if(type == TargetSelection::BookmarksAndChildren) {
        // ---
        bool isChanged = false ;
        do {
        restartLoop:
            isChanged = false ;
            foreach(Element * e, result) {
                foreach(Element * pe, result) {
                    if(e != pe) {
                        if(e->isChildOf(pe)) {
                            isChanged = true ;
                            result.removeAll(e);
                            goto restartLoop;
                            //break;
                        }
                        if(pe->isChildOf(e)) {
                            isChanged = true ;
                            result.removeAll(pe);
                            goto restartLoop;
                            //break;
                        }
                    }
                }
                if(isChanged) {
                    goto restartLoop;
                    //break;
                }
            }
        } while(isChanged);
    }
    return result;
}

void Regola::collectPrefixes(PrefixInfo & info, Element * selection)
{
    if(NULL != root()) {
        root()->collectPrefixes(info, selection, false, false);
    }
}

void Regola::collectNamespaces(NamespacesInfo &info, Element * selection)
{
    if(NULL != root()) {
        root()->collectNamespaces(info, NULL, selection, false, false);
    }
}

void Element::collectNamespaces(NamespacesInfo & info, NSContext *parentContext, Element * selection, const bool inSelection, const bool inBookmark)
{
    //---
    if(!isElement()) {
        return ;
    }
    NSContext thisContext(parentContext);
    handleNamespace(&thisContext);

    bool thisBookmark = false ;
    bool isSelection = this == selection;
    if(NULL != parentRule) {
        if(parentRule->isBookmarked(this)) {
            thisBookmark = true;
        }
    }
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    const QString &elementNamespace = thisContext.uriFromPrefix(prefix);
    // Il contesto reca tutti i ns utilizzati, quindi quelli visibili anche
    // se non dichiarati nel contesto stesso.
    QSet<QString> namespacesOfContext ;
    namespacesOfContext.unite(thisContext.namespacesOfContext());
    if(!elementNamespace.isEmpty()) {
        namespacesOfContext.insert(elementNamespace);
    }
    foreach(const QString & ns, namespacesOfContext) {
        info.allNamespaces.insert(ns);
        if(isSelection) {
            info.selectionNamespaces.insert(ns);
        }
        if(inSelection || isSelection) {
            info.selectionNamespacesRecursive.insert(ns);
        }
        if(thisBookmark) {
            info.bookmarksNamespacesRecursive.insert(ns);
        }
        if(inBookmark || thisBookmark) {
            info.bookmarksNamespacesRecursive.insert(ns);
        }
        if(!info.prefixesForNamespaces.contains(ns)) {
            QSet<QString> newSet ;
            info.prefixesForNamespaces[ns] = newSet ;
        }
        if(!info.prefixesForNamespaces.contains(ns)) {
            QSet<QString> set;
            info.prefixesForNamespaces.insert(ns, set);
        }
        info.prefixesForNamespaces[ns].unite(thisContext.prefixesForNamespace(ns));
    }
    foreach(Element * child, getChildItemsRef()) {
        if(child->isElement()) {
            child->collectNamespaces(info, &thisContext, selection, inSelection || isSelection, inBookmark || thisBookmark);
        }
    }
}

void Element::collectPrefixes(PrefixInfo & info, Element * selection, const bool inSelection, const bool inBookmark)
{
    QSet<QString> prefixes;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    if(!prefix.isEmpty()) {
        prefixes.insert(prefix);
    }
    foreach(Attribute * attr, getAttributesList()) {
        XmlUtils::decodeQualifiedName(attr->name, prefix, name);
        if(!prefix.isEmpty() && XmlUtils::isDataAttribute(attr->name)) {
            prefixes.insert(prefix);
        }
    }
    bool thisBookmark = false ;
    bool isSelection = this == selection;
    if(NULL != parentRule) {
        if(parentRule->isBookmarked(this)) {
            thisBookmark = true;
        }
    }
    foreach(const QString & prefix, prefixes.values()) {
        info.allPrefixes.insert(prefix);
        if(isSelection) {
            info.selectionPrefixes.insert(prefix);
        }
        if(inSelection || isSelection) {
            info.selectionPrefixesRecursive.insert(prefix);
        }
        if(thisBookmark) {
            info.bookmarksPrefixesRecursive.insert(prefix);
        }
        if(inBookmark || thisBookmark) {
            info.bookmarksPrefixesRecursive.insert(prefix);
        }
    }
    foreach(Element * child, getChildItemsRef()) {
        if(child->isElement()) {
            child->collectPrefixes(info, selection, inSelection || isSelection, inBookmark || thisBookmark);
        }
    }
}

void Element::collectAllPrefixes(PrefixInfo & info, Element * selection, const bool inSelection, const bool inBookmark)
{
    QSet<QString> prefixes;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    if(!prefix.isEmpty()) {
        prefixes.insert(prefix);
    }
    foreach(Attribute * attr, getAttributesList()) {
        XmlUtils::decodeQualifiedName(attr->name, prefix, name);
        if(!prefix.isEmpty() && XmlUtils::isDataAttribute(attr->name)) {
            prefixes.insert(prefix);
        } else if(!prefix.isEmpty() && XmlUtils::isDeclaringNS(attr->name)) {
            // handles xmlns:XXX
            if(!name.isEmpty()) {
                prefixes.insert(name);
            }
        }
    }
    bool thisBookmark = false ;
    bool isSelection = this == selection;
    if(NULL != parentRule) {
        if(parentRule->isBookmarked(this)) {
            thisBookmark = true;
        }
    }
    foreach(const QString & prefix, prefixes.values()) {
        info.allPrefixes.insert(prefix);
        if(isSelection) {
            info.selectionPrefixes.insert(prefix);
        }
        if(inSelection || isSelection) {
            info.selectionPrefixesRecursive.insert(prefix);
        }
        if(thisBookmark) {
            info.bookmarksPrefixesRecursive.insert(prefix);
        }
        if(inBookmark || thisBookmark) {
            info.bookmarksPrefixesRecursive.insert(prefix);
        }
    }
    foreach(Element * child, getChildItemsRef()) {
        if(child->isElement()) {
            child->collectAllPrefixes(info, selection, inSelection || isSelection, inBookmark || thisBookmark);
        }
    }
}

bool Regola::removePrefix(const QString &removedPrefix, QList<Element*> elements, TargetSelection::Type targetSelection,
                          const bool isAllPrefixes, ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {
            if(!element->removePrefix(removedPrefix, targetSelection,
                                      isAllPrefixes, observer)) {
                ok = false;
            }
        }
    }
    return ok;
}



bool Regola::setPrefix(const QString &newPrefix, QList<Element*> elements, TargetSelection::Type targetSelection,
                       ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {
            if(!element->setPrefix(newPrefix, targetSelection, observer)) {
                ok = false;
            }
        }
    }
    return ok;
}

bool Regola::updateElementInplace(ElementUndoInfo *info)
{
    bool ok = false;
    Element *element = findElementByArray(info->path);
    if(NULL != element) {
        if(element->isElement() && !info->tag.isEmpty()) {
            element->setTag(addNameToPool(info->tag));
        }
        if(info->replaceAttributes) {
            element->clearAttributes();
            const int count = info->attributes.size();
            FORINT(index, count) {
                const QPair<QString, QString> &attributeInfo = info->attributes.at(index);
                element->addAttribute(attributeInfo.first, attributeInfo.second);
            }
        }
        ok = true ;
    }
    return ok ;
}

bool Regola::removeNamespace(const QString &removedNS, QList<Element *>elements, TargetSelection::Type targetSelection,
                             const bool isAllNamespaces, const bool removeDeclarations, ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {
            QList<NSContext*> contexts;
            NSContext *lastContext = buildContextInfo(contexts, element);
            if(!element->removeNamespace(removedNS, targetSelection,
                                         isAllNamespaces, removeDeclarations, observer, lastContext)) {
                ok = false;
            }
            EMPTYPTRLIST(contexts, NSContext);
        }
    }
    return ok;
}

bool Regola::setNamespace(const QString &ns, const QString &prefix, QList<Element*> elements, TargetSelection::Type targetSelection, ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {

            QList<NSContext*> contexts;
            NSContext *lastContext = buildContextInfo(contexts, element);
            if(!element->setNamespace(ns, prefix, targetSelection, observer, lastContext, true)) {
                ok = false;
            }
            EMPTYPTRLIST(contexts, NSContext);
        }
    }
    return ok;
}

bool Regola::replaceNamespace(const QString &replacedNS, const QString &newNS, const QString &newPrefix, QList<Element*> elements,
                              TargetSelection::Type targetSelection, ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {

            QList<NSContext*> contexts;
            NSContext *lastContext = buildContextInfo(contexts, element);

            if(!element->replaceNamespace(replacedNS, newNS, newPrefix, targetSelection, observer, lastContext, true, false)) {
                ok = false;
            }
            EMPTYPTRLIST(contexts, NSContext);
        }
    }
    return ok;
}

void Element::handleNamespace(NSContext *context)
{
    foreach(Attribute * attribute, attributes) {
        context->addNamespaceAttribute(attribute->name, attribute->value);
    }
}

void Element::serializeAttributesToInfo(ElementUndoInfo &info)
{
    if(info.replaceAttributes) {
        return ;
    }
    info.replaceAttributes = true;
    foreach(Attribute * attr, getAttributesList()) {
        info.attributes.append(QPair<QString, QString>(attr->name, attr->value));
    }
}


bool Regola::replacePrefix(const QString &oldPrefix, const QString &newPrefix, QList<Element*> elements, TargetSelection::Type targetSelection,
                           const bool isAllPrefixes, ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {
            if(!element->replacePrefix(oldPrefix, newPrefix, targetSelection, isAllPrefixes, observer)) {
                ok = false;
            }
        }
    }
    return ok;
}

bool Regola::namespaceAvoidClash(const QString &prefixToAvoid, const QString &legalNS, NamespacesInfo *namespacesInfo, ElementUndoObserver *observer)
{
    NSContext context(NULL);
    QHash<QString, QString> prefixes;
    QSet<QString> allPrefixes;
    // build prefixes pool, to avoid any clash in the processing
    foreach(const QString & ns, namespacesInfo->prefixesForNamespaces.keys()) {
        foreach(const QString & prf, namespacesInfo->prefixesForNamespaces[ns]) {
            allPrefixes.insert(prf);
        }
    }

    if(NULL != root()) {
        return root()->namespaceAvoidClash(&context, prefixToAvoid, legalNS, prefixes, allPrefixes, observer);
    }
    return true;
}


bool Regola::namespaceNormalize(const QString &thePrefix, const QString &theNS, QList<Element*> elements, const bool declareOnlyOnRoot, ElementUndoObserver *observer)
{
    bool ok = true;
    foreach(Element * element, elements) {
        if((NULL != element) && element->isElement()) {
            QList<NSContext*> contexts;
            NSContext *context = buildContextInfo(contexts, element);
            bool isDeclared = false ;
            if((root() != element) && !contexts.isEmpty()) {
                NSContext *ultimateContext = contexts[contexts.size() - 1];
                isDeclared = ultimateContext->hasPrefixForNamespace(theNS, thePrefix) ;
            }
            if(!element->normalizeNamespace(theNS, thePrefix, declareOnlyOnRoot, true, isDeclared, observer, context)) {
                ok = false;
            }
            EMPTYPTRLIST(contexts, NSContext);
        }
    }
    return ok;
}

NSContext* Regola::buildContextInfo(QList<NSContext*> &contexts, Element *element)
{
    NSContext *lastContext = NULL ;
    Element *parent = element->parent() ;
    // buildup up to the parent namespaces
    QList<Element*> parents;
    while(NULL != parent) {
        parents.insert(0, parent);
        parent = parent->parent();
    }
    foreach(Element * theElement, parents) {
        NSContext *newContext = new NSContext(lastContext);
        contexts.append(newContext);
        lastContext = newContext ;
        theElement->handleNamespace(newContext);
    }
    return lastContext;
}
