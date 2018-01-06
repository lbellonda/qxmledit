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

#include "xincludeeditormanager.h"
#include "regola.h"
#include "modules/specialized/xinclude/xincludedialog.h"
#include "xmlutils.h"
#include "utils.h"

//------
XIncludeEditorProvider::XIncludeEditorProvider() {}
XIncludeEditorProvider::~XIncludeEditorProvider() {}
//------

static const QString XINCLUDE_TAG = "include" ;
static const QString XFALLBACK_TAG = "fallback" ;
const QString XIncludeEditorManager::XIncLocaAttrNS = "http://www.w3.org/2001/XInclude/local-attributes";
const QString XIncludeEditorManager::XIncludeItemCode("XInclude");
const QString XIncludeEditorManager::XIncludeFallbackItemCode("Fallback");

XIncludeEditorManager::XIncludeEditorManager() : NamespaceHandlerForEdit()
{
    _provider = this ;
}

XIncludeEditorManager::~XIncludeEditorManager()
{
}

bool XIncludeEditorManager::handleEdit(QWidget *parent, XmlEditWidget * /*editor*/, QTreeWidget * tree, Regola *regola, Element *element)
{
    QXName qName ;
    element->qName(&qName);
    if((qName.ns == NamespaceManager::XIncludeNamespace) && (qName.name == XINCLUDE_TAG)) {
        Element *newElement = new Element(NULL);
        if(NULL != newElement) {
            element->copyTo(*newElement, false);
            XIncludeDialog dlg(parent, newElement);
            if(dlg.exec() == QDialog::Accepted) {
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
    return false;
}

HandlerForInsert *XIncludeEditorManager::handlerForInsert(XmlEditWidget *, Regola *, Element *element, const bool isChild)
{
    bool isFallback = false;
    if(NULL != element) {
        Element *theParent = element ;
        if(!isChild && (NULL != element)) {
            theParent = element->parent();
        }
        if(NULL != theParent) {
            QXName qName ;
            element->qName(&qName);
            if((qName.ns == NamespaceManager::XIncludeNamespace) && (qName.name == XINCLUDE_TAG)) {
                isFallback = true;
            }
        }
    }

    HandlerForInsert *category = new HandlerForInsert();
    const QString XInclude = "XInclude";
    const QString Fallback = "Fallback";
    category->name = XInclude;
    category->handler = this;
    category->nameSpace = NamespaceManager::XIncludeNamespace ;
    SingleHandlerForInsert *s = new SingleHandlerForInsert();
    s->name = XInclude ;
    s->id = XIncludeItemCode ;
    s->description = QObject::tr("%1 element, including content").arg(XInclude) ;
    category->elements.append(s);
    if(isFallback) {
        SingleHandlerForInsert *sf = new SingleHandlerForInsert();
        sf->name = Fallback ;
        sf->id = XIncludeFallbackItemCode ;
        sf->description = QObject::tr("%1 element, fallback for including content").arg(Fallback) ;
        category->elements.append(sf);
    }
    return category ;
}

bool XIncludeEditorManager::handleInsert(XmlEditWidget *, QTreeWidget *tree, Regola *regola, Element *element, const bool isChild, const QString & itemCode)
{
    Element *newElement = new Element(NULL);
    bool goAhead = false;
    if(itemCode == XIncludeItemCode) {
        newElement->setTag(QString(XmlUtils::makeQualifiedName(NamespaceManager::XIncludePrefix, XINCLUDE_TAG)));
        newElement->setAttribute(XmlUtils::makeNSDeclaration(NamespaceManager::XIncludePrefix), NamespaceManager::XIncludeNamespace);
        if(_provider->provideXIncludeEditor(tree->window(), newElement)) {
            goAhead = true ;
        }
    } else {
        QString prefix;
        QXName qName ;
        element->qName(&qName);
        bool foundPrefix = false;
        if((qName.ns == NamespaceManager::XIncludeNamespace) && (qName.name == XINCLUDE_TAG)) {
            QString localName;
            XmlUtils::decodeQualifiedName(element->tag(), prefix, localName);
            foundPrefix = true ;
        }
        if(!foundPrefix) {
            prefix = NamespaceManager::XIncludePrefix ;
            newElement->setAttribute(XmlUtils::makeNSDeclaration(NamespaceManager::XIncludePrefix), NamespaceManager::XIncludeNamespace);
        }
        newElement->setTag(QString(XmlUtils::makeQualifiedName(prefix, XFALLBACK_TAG)));
        // the namespace is already declared in the parent.
        goAhead = true ;
    }

    if(goAhead) {
        return insertAction(tree, regola, element, newElement, isChild);
    }
    delete newElement ;
    return false ;
}

bool XIncludeEditorManager::provideXIncludeEditor(QWidget *parent, Element *element)
{
    XIncludeDialog dlg(parent, element);
    return dlg.exec() == QDialog::Accepted;
}

void XIncludeEditorManager::setProviderXIncludeEditor(XIncludeEditorProvider *newProvider)
{
    _provider = newProvider ;
}
