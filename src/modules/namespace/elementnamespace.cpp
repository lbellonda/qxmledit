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
#include "xmlutils.h"
#include "utils.h"
#include "modules/namespace/nscontext.h"

/*
The worst case is this:

a:1
b:2
1:1

replace a:1 with b:3

Now:
b:3
b:2
b:3 or 2?


If b2 contains a declaration, b3 can be assigned to ns2 instead of ns3

the behavior is;
if is root and not ex declation ns3 visible then
  1. remove prefix declaration if exists
  2. add ns3 declaration
now the declation of ns3 exists

flag is a flag to nofy that the ns is declared and visible

In children:

If the tag is not to replace:
- if exists other ns same pr. then flag <- false
If the tag is to replace:
- if exists another declation for the prefix -> remove it (THIS CAN LEAD TO INCONSISTENCIES: protect this case before the call)
- if no ns decl (how it is possible?) || !flag || (isRoot && no decl) then
- - add declaration
- - flag <- true
- - now the namspace is visible for children using the informatio from flag

  */
bool Element::replaceNamespace(const QString &replacedNS, const QString &newNS, const QString &newPrefix, TargetSelection::Type targetSelection,
                               ElementUndoObserver *observer, const NSContext *parentContext, const bool isRoot, const bool nsVisible)
{
    NSContext context(parentContext);
    bool nsIsVisible = nsVisible;
    //add namespaces
    handleNamespace(&context);
    // Then, forced to declare here because of this declaration that must be invalidated
    // but only if the ns has to be changed
    if(isRoot) {
        nsIsVisible = false;
        if(context.hasPrefixForNamespace(newNS, newPrefix)) {
            nsIsVisible = true;
        }
    }

    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    QString name, prefix;
    bool tagMatched = false;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    const QString &elementNamespace = context.uriFromPrefix(prefix);
    if(elementNamespace == replacedNS) {
        QString newTag = XmlUtils::makeQualifiedName(newPrefix, name);
        setNewTagWithUndo(newTag, info);
        changed = true ;
        tagMatched = true;
    }
    QString attrDeclarationName = XmlUtils::makeNSDeclaration(newPrefix);
    if(tagMatched) {
        if(!nsIsVisible) {
            serializeAttributesToInfo(info);
            removeAttribute(attrDeclarationName);
            addAttribute(attrDeclarationName, newNS);
            changed = true ;
            nsIsVisible = true ;
        }
    } else {
        Attribute *declaration = getAttribute(attrDeclarationName);
        if(NULL != declaration) {
            if(declaration->value != newNS) {
                nsIsVisible = false ;
            }
        }
    }

    if(changed) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }
    bool recursive = TargetSelection::isRecursive(targetSelection);
    if(recursive) {
        foreach(Element * child, getChildItemsRef()) {
            if(child->isElement()) {
                if(!child->replaceNamespace(replacedNS, newNS, newPrefix, targetSelection, observer, &context, false, nsIsVisible)) {
                    ok = false;
                }
            }
        }
    }
    return ok;
}

bool Element::setNamespace(const QString &newNS, const QString &newPrefix, TargetSelection::Type targetSelection,
                           ElementUndoObserver *observer, const NSContext *parentContext, const bool canDeclare)
{
    NSContext context(parentContext);

    //add namespaces
    handleNamespace(&context);
    bool declareNS = false;
    if(canDeclare && !context.hasPrefixForNamespace(newNS, newPrefix)) {
        declareNS = true;
    }

    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    if(prefix != newPrefix) {
        QString newTag = XmlUtils::makeQualifiedName(newPrefix, name);
        setNewTagWithUndo(newTag, info);
        changed = true ;
    }
    if(declareNS) {
        serializeAttributesToInfo(info);
        QString attrDeclarationName = XmlUtils::makeNSDeclaration(newPrefix);
        removeAttribute(attrDeclarationName);
        addAttribute(attrDeclarationName, newNS);
        changed = true ;
    } else if(!canDeclare) {
        QString attrDeclarationName = XmlUtils::makeNSDeclaration(newPrefix);
        if(hasAttribute(attrDeclarationName)) {
            serializeAttributesToInfo(info);
            removeAttribute(attrDeclarationName);
            changed = true ;
        }
    }

    if(changed) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }
    bool recursive = TargetSelection::isRecursive(targetSelection);
    if(recursive) {
        foreach(Element * child, getChildItemsRef()) {
            if(child->isElement()) {
                if(!child->setNamespace(newNS, newPrefix, targetSelection, observer, &context, false)) {
                    ok = false;
                }
            }
        }
    }
    return ok;
}

bool Element::removeNamespace(const QString &removedNS, TargetSelection::Type targetSelection,
                              const bool isAllNamespaces, const bool removeDeclarations, ElementUndoObserver *observer,
                              const NSContext *parentContext)
{
    NSContext context(parentContext);

    //add namespaces
    handleNamespace(&context);

    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    bool attrChanged = false ;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    const QString &elementNamespace = context.uriFromPrefix(prefix);
    if((isAllNamespaces && !prefix.isEmpty())
            || (!isAllNamespaces && (removedNS == elementNamespace))) {
        setNewTagWithUndo(name, info);
        changed = true ;
    }
    int attrIndex = 0 ;
    serializeAttributesToInfo(info);
    foreach(Attribute * attr, getAttributesList()) {
        if(!XmlUtils::isDeclaringNS(attr->name)) {
            XmlUtils::decodeQualifiedName(attr->name, prefix, name);
            const QString &attributeNamespace = context.uriFromPrefix(prefix);
            if((isAllNamespaces && !prefix.isEmpty())
                    || (!isAllNamespaces && (removedNS == attributeNamespace))) {
                attr->name = name;
                changed = true ;
                attrChanged = true ;
            }
        }
        attrIndex ++ ;
    }

    // if remove declarations
    if(removeDeclarations) {
        QList<int> deleteIndexes;
        int attrIndex = 0 ;
        foreach(Attribute * attr, getAttributesList()) {
            if(XmlUtils::isDeclaringNS(attr->name)) {
                const bool isThisNamespace = attr->value == removedNS ;
                if(isAllNamespaces
                        || (!isAllNamespaces && isThisNamespace)) {
                    deleteIndexes.insert(0, attrIndex);
                    delete attr;
                }
            }
            attrIndex ++ ;
        }
        if(deleteIndexes.size() > 0) {
            changed = true ;
            attrChanged = true ;
            foreach(int index, deleteIndexes) {
                attributes.remove(index);
            }
        }
    }
    if(!attrChanged) {
        info.undoReplaceAttributes();
    }
    if(changed) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }
    bool recursive = TargetSelection::isRecursive(targetSelection);
    if(recursive) {
        foreach(Element * child, getChildItemsRef()) {
            if(child->isElement()) {
                if(!child->removeNamespace(removedNS, targetSelection,
                                           isAllNamespaces, removeDeclarations, observer,
                                           &context)) {
                    ok = false;
                }
            }
        }
    }
    return ok;
}

bool Element::namespaceAvoidClash(NSContext *parentContext, const QString &prefixToAvoid, const QString &legalNS,
                                  QHash<QString, QString> &prefixes, QSet<QString> &allPrefixes, ElementUndoObserver *observer)
{
    if(!isElement()) {
        return true;
    }
    NSContext context(parentContext);

    //add namespaces
    handleNamespace(&context);

    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    bool attrChanged = false ;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    const QString &elementNamespace = context.uriFromPrefix(prefix);
    if((prefix == prefixToAvoid) && (elementNamespace != legalNS)) {
        const QString &newPrefix = XmlUtils::makeNewPrefixForChangePrefix(prefixes, allPrefixes, elementNamespace, prefix);
        QString newTag = XmlUtils::makeQualifiedName(newPrefix, name);
        setNewTagWithUndo(newTag, info);
        changed = true ;
    }
    serializeAttributesToInfo(info);
    foreach(Attribute * attr, getAttributesList()) {
        if(XmlUtils::isDeclaringNS(attr->name)) {
            QString prefix;
            XmlUtils::getNsPrefix(attr->name, prefix);
            if((prefix == prefixToAvoid) && (attr->value != legalNS)) {
                const QString &newPrefix = XmlUtils::makeNewPrefixForChangePrefix(prefixes, allPrefixes, attr->value, prefix);
                attr->name = XmlUtils::makeNSDeclaration(newPrefix);
                attrChanged = true;
            }
        } else {
            QString prefix;
            QString name ;
            XmlUtils::decodeQualifiedName(attr->name, prefix, name);
            if(!prefix.isEmpty()) {
                QString attrNS = context.uriFromPrefix(prefix);
                if((prefix == prefixToAvoid) && (attrNS != legalNS)) {
                    const QString &newPrefix = XmlUtils::makeNewPrefixForChangePrefix(prefixes, allPrefixes, attrNS, prefix);
                    QString newName = XmlUtils::makeQualifiedName(newPrefix, name);
                    attr->name = newName;
                    attrChanged = true ;
                }
            }
        }
    }

    if(!attrChanged) {
        info.undoReplaceAttributes();
    }
    if(changed || attrChanged) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }

    foreach(Element * child, getChildItemsRef()) {
        if(child->isElement()) {
            if(!child->namespaceAvoidClash(&context, prefixToAvoid, legalNS, prefixes, allPrefixes, observer)) {
                ok = false;
            }
        }
    }
    return ok;
}

void Element::setNewTagWithUndo(const QString &newTag, ElementUndoInfo &info)
{
    info.tag = tag();
    setTagAuto(newTag);
}

bool Element::normalizeNamespace(const QString &theNS, const QString &thePrefix,
                                 const bool declareOnlyOnRoot, const bool isRoot,
                                 const bool wasDeclared,
                                 ElementUndoObserver *observer, const NSContext *parentContext)
{
    NSContext context(parentContext);

    //add namespaces
    handleNamespace(&context);

    bool ok = true;
    ElementUndoInfo info;
    bool changed = false ;
    bool attrChanged = false ;
    QString name, prefix;
    XmlUtils::decodeQualifiedName(tag(), prefix, name);
    const QString &thisNamespace = context.uriFromPrefix(prefix);
    const bool elementHasThisNamespace = (thisNamespace == theNS);
    if((prefix != thePrefix) && elementHasThisNamespace) {
        QString newTag = XmlUtils::makeQualifiedName(thePrefix, name);
        setNewTagWithUndo(newTag, info);
        changed = true ;
    }
    bool existsDeclaration = false;
    bool declaredHere = false ;
    serializeAttributesToInfo(info);

    QMutableVectorIterator<Attribute *> attrIterator(attributes);
    while(attrIterator.hasNext()) {
        Attribute * attr = attrIterator.next() ;
        if(XmlUtils::isDeclaringNS(attr->name)) {
            QString prefix;
            XmlUtils::getNsPrefix(attr->name, prefix);
            if((prefix != thePrefix) && (attr->value == theNS)) {
                attrIterator.remove();
                attrChanged = true;
            } else {
                if((prefix == thePrefix) && (attr->value == theNS)) {
                    existsDeclaration = true;
                    if(!isRoot && declareOnlyOnRoot) {
                        //remove this declaration, declarations are legal only on root
                        attrIterator.remove();
                        attrChanged = true;
                    } else {
                        declaredHere = true ;
                    }
                }
            }
        } else {
            if(elementHasThisNamespace) {
                QString prefix;
                QString name ;
                XmlUtils::decodeQualifiedName(attr->name, prefix, name);
                if(!prefix.isEmpty()) {
                    const QString &thisNamespace = context.uriFromPrefix(prefix);
                    if(!prefix.isEmpty() && (thisNamespace == theNS) && (prefix != thePrefix)) {
                        QString newName = XmlUtils::makeQualifiedName(thePrefix, name);
                        attr->name = newName;
                        attrChanged = true ;
                    }
                }
            }
        }
    } // while attr

    if(isRoot && !existsDeclaration && declareOnlyOnRoot) {
        addAttribute(XmlUtils::makeNSDeclaration(thePrefix), theNS);
        attrChanged = true ;
        declaredHere = true ;
    } else if(!existsDeclaration && !wasDeclared && elementHasThisNamespace) {
        // the declaration needs to be here if not existent
        addAttribute(XmlUtils::makeNSDeclaration(thePrefix), theNS);
        attrChanged = true ;
        declaredHere = true ;
    } else {
        declaredHere = existsDeclaration || wasDeclared;
    }

    if(!attrChanged) {
        info.undoReplaceAttributes();
    }
    if(changed || attrChanged) {
        info.path  = indexPath();
        if(!observer->observe(&info)) {
            ok = false;
        }
    }

    // recursive by definition
    foreach(Element * child, getChildItemsRef()) {
        if(child->isElement()) {
            if(!child->normalizeNamespace(theNS, thePrefix, declareOnlyOnRoot, false, declaredHere, observer, &context)) {
                ok = false;
            }
        }
    }
    return ok;
}

void Element::qName(QXName *pQName)
{
    pQName->name = "" ;
    pQName->ns = "" ;
    if(isElement()) {
        QList<NSContext*> contexts;
        NSContext *lastContext = Regola::buildContextInfo(contexts, this);
        NSContext context(lastContext);
        handleNamespace(&context);
        QString name, prefix;
        XmlUtils::decodeQualifiedName(tag(), prefix, name);
        const QString &elementNamespace = context.uriFromPrefix(prefix);
        pQName->ns = elementNamespace ;
        pQName->name = name ;
        EMPTYPTRLIST(contexts, NSContext);
    }
}
