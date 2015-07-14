/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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

#include "element.h"
#include "xmlutils.h"

QString Element::namespaceForPrefix(const QString &prefix)
{
    Element *element = this ;
    while(NULL != element) {
        Attribute *declaration = element->nsDeclarationForPrefixOwned(prefix);
        if(NULL != declaration) {
            return declaration->value;
        }
        element = element->parent();
    }
    return NULL ;
}


Attribute *Element::nsDeclarationForPrefixOwned(const QString &prefix)
{
    foreach(Attribute * attribute, attributes) {
        if(XmlUtils::isNamespaceDeclarationForPrefix(attribute->name, prefix)) {
            return attribute;
        }
    }
    return NULL ;
}


bool Element::isUsingPrefixRecursive(const QString &prefix)
{
    if(!isElement()) {
        return false;
    }
    if(XmlUtils::hasPrefix(tag(), prefix)) {
        return true;
    }
    foreach(Attribute * attribute, attributes) {
        if(XmlUtils::hasPrefix(attribute->name, prefix)) {
            return true;
        }
    }
    foreach(Element * child, childItems) {
        if(child->areChildrenUsingPrefix(prefix)) {
            return true;
        }
    }
    return false;
}


bool Element::areChildrenUsingPrefix(const QString &prefix)
{
    if(!isElement()) {
        return false;
    }
    foreach(Element * child, childItems) {
        if(child->isUsingPrefixRecursive(prefix)) {
            return true;
        }
    }
    return false;
}

// look for the given prefix
/**
 * @brief Element::findPrefixForNamespace find the prefix used in the declaration for the given
 *    namespace starting from this and recursing to the root.
 * @param nsToSearch
 * @param prefixToSet
 * @return
 */
bool Element::findPrefixForNamespace(const QString nsToSearch, QString &prefixToSet)
{
    QSet<QString> nss;
    Element *element = this ;
    while(NULL != element) {
        foreach(Attribute * attr, element->attributes) {
            QString prefix ;
            if( XmlUtils::getNsPrefix(attr->name, prefix) ) {
                // found, but only if it is not shadowed by a previous declaration
                if( attr->value == nsToSearch) {
                    if(!nss.contains(prefix)) {
                        prefixToSet = prefix ;
                        return true ;
                    }
                } else {
                    // register the prefix
                    if(!nss.contains(prefix)) {
                        nss.insert(prefix);
                    }
                }
            } // if is declaring
        } // for each attribute

        element = element->parent();
    }
    return false ;
}

