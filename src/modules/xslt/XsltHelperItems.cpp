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

#include "xslthelper.h"
#include "utils.h"
#include "xmleditwidgetprivate.h"

QString XsltHelper::getXslName(Element *el)
{
    Attribute *a = el->getAttribute("name");
    if(NULL != a) {
        return a->value;
    }
    return "" ;
}

//-----------------------------------------------
QSet<QString> XsltHelper::topLevelNames(const QString &nameToSearch)
{
    QSet<QString> result ;
    Element *root = _owner->getRegola()->root();
    QString tagTemplate = xsltQName(nameToSearch);
    foreach(Element * child, *root->getChildItems()) {
        if(child->isElement() && (child->tag() == tagTemplate)) {
            QString name = getXslName(child);
            if(!name.isEmpty()) {
                result.insert(name);
            }
        }
    }
    return result ;
}

QHash<QString, Element*> XsltHelper::topLevelNamesRef(const QString &nameToSearch)
{
    QHash<QString, Element*> result ;
    Element *root = _owner->getRegola()->root();
    QString tagTemplate = xsltQName(nameToSearch);
    if(NULL != root) {
        foreach(Element * child, *root->getChildItems()) {
            if(child->isElement() && (child->tag() == tagTemplate)) {
                QString name = getXslName(child);
                result.insertMulti(name, child);
            }
        }
    }
    return result ;
}

QSet<QString> XsltHelper::templateNames()
{
    return topLevelNames("template");
}

QSet<QString> XsltHelper::functionNames()
{
    return topLevelNames("function");
}

QHash<QString, Element *> XsltHelper::templateNamesMap()
{
    return topLevelNamesRef("template");
}

QHash<QString, Element *> XsltHelper::functionNamesMap()
{
    return topLevelNamesRef("function");
}

Element *XsltHelper::findTemplate(const QString &nameToFind)
{
    Element *element = findRootTag(nameToFind, "template");
    return element;
}

Element *XsltHelper::findFunction(const QString &nameToFind)
{
    Element *element = findRootTag(nameToFind, "function");
    return element;
}

Element *XsltHelper::findRootTag(const QString &nameToFind, const QString &typeName)
{
    Element *root = _owner->getRegola()->root();
    QString prefix = _owner->namespacePrefixXslt();
    QString tagType = typeName;
    if(!prefix.isEmpty()) {
        tagType = prefix + ":" + tagType  ;
    }
    foreach(Element * child, *root->getChildItems()) {
        if(child->isElement() && (child->tag() == tagType)) {
            QString name = getXslName(child);
            if(name == nameToFind) {
                return child;
            }
        }
    }
    return NULL ;
}

QSet<QString> XsltHelper::parameterNames(const QString &templateName)
{
    QSet<QString> result ;
    if(!templateName.isEmpty()) {
        Element *templateObj = findTemplate(templateName);
        if(NULL != templateObj) {
            QString prefix = _owner->namespacePrefixXslt();
            QString tagParam = "param";
            if(!prefix.isEmpty()) {
                tagParam = prefix + ":" + tagParam ;
            }

            foreach(Element * child, *templateObj->getChildItems()) {
                if(child->isElement() && (child->tag() == tagParam)) {
                    QString name = getXslName(child);
                    if(!name.isEmpty()) {
                        result.insert(name);
                    }
                }
            }
        }
    }
    return result ;
}

bool XsltHelper::isXsdElement(Element *referenceElement, const QString &prefix)
{
    if(prefix.isEmpty()) {
        return !referenceElement->tag().contains(":");
    }
    return referenceElement->tag().startsWith(prefix);
}

bool XsltHelper::isXsdElementOfKind(Element *element, const QString &prefix, const QString &kind)
{
    if((NULL != element) && element->isElement() && isXsdElement(element, prefix)) {
        QString tagParam = kind;
        if(!prefix.isEmpty()) {
            tagParam = prefix + ":" + tagParam ;
        }
        return element->tag() == tagParam ;
    }
    return false;
}

/**
 *
 * @brief XsltHelper::allNames
 * @param referenceElement
 * @return all the xsl variable elements with name attributes prefixed by $, starting with the reference and up to the root, and all the element names as found in rule
 */
QSet<QString> XsltHelper::allNames(Element *referenceElement)
{
    QSet<QString> result ;
    QString prefix = _owner->namespacePrefixXslt();

    if(NULL != referenceElement) {
        Element *aParent = referenceElement;
        while(NULL != aParent) {

            // get the names for this and its previous siblings
            int pos = aParent->indexOfSelfAsChild();
            for(int i = pos; i >= 0 ; i --) {
                if(NULL != aParent->parent()) {
                    Element *el = aParent->parent()->getChildAt(i);
                    if(isXsdElementOfKind(el, prefix, "variable")
                            ||  isXsdElementOfKind(el, prefix, "param")
                            ||  isXsdElementOfKind(el, prefix, "with-param")) {
                        result.insert("$" + getXslName(el));
                    }
                }
            }
            aParent = aParent->parent();
        }
    }
    foreach(QString dataName, _namesForAutocompletion.values()) {
        result.insert(dataName);
    }
    return result ;
}

bool XsltHelper::hasAutoCompletionNames()
{
    bool hasNames = _namesForAutocompletion.size() > 0;
    return hasNames ;
}

Element *XsltHelper::getTemplateParent(Element *referenceElement)
{
    if(NULL != referenceElement) {
        QString prefix = _owner->namespacePrefixXslt();
        QString tagParam = "call-template";
        if(!prefix.isEmpty()) {
            tagParam = prefix + ":" + tagParam ;
        }

        Element *aParent = referenceElement;
        while(NULL != aParent) {
            if(aParent->isElement() && (aParent->tag() == tagParam)) {
                return aParent ;
            }
            aParent = aParent->parent();
        }
    }
    return NULL ;
}

XsltElement *XsltHelper::elementFromName(const QString tag)
{
    init();
    if(_elementsByTag.contains(tag)) {
        return _elementsByTag[tag];
    }
    return NULL ;
}

void XsltHelper::addNameForAutocompletion(const QString &newName)
{
    _namesForAutocompletion.insert(newName);
}
