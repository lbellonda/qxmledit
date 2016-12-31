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

#include "xsleditormanager.h"
#include "regola.h"
#include "modules/xslt/xslthelper.h"
#include "utils.h"

XSLEditorManager::XSLEditorManager()
{

}

XSLEditorManager::~XSLEditorManager()
{

}

bool XSLEditorManager::handleEdit(QWidget * /*parent*/, XmlEditWidget *editor, QTreeWidget * /*tree*/, Regola * /*regola*/, Element *element)
{
    XsltHelper *xsltHelper = editor->XSLTHelper();
    if(xsltHelper->isXSLTElement(element)) {
        xsltHelper->editElement(element);
        return true;
    }
    return false;
}

bool XSLEditorManager::handleInsert(XmlEditWidget *editor, QTreeWidget * /*tree*/, Regola * /*regola*/, Element *element, const bool isChild, const QString & itemCode)
{
    XsltHelper *xsltHelper = editor->XSLTHelper();
    xsltHelper->execOperation(itemCode, isChild, element);
    return true ;
}

HandlerForInsert *XSLEditorManager::handlerForInsert(XmlEditWidget *editor, Regola *regola, Element *element, const bool isChild)
{
    XsltHelper *xsltHelper = editor->XSLTHelper();
    HandlerForInsert *category;
    if(isChild) {
        category = xsltHelper->findElementsForInsert(element, regola->namespacePrefixXslt());
    } else {
        category = xsltHelper->findElementsForAppend(element, regola->namespacePrefixXslt());
    }
    category->handler = this;
    return category ;
}
