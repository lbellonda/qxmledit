/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015-2018 by Luca Bellonda and individual contributors  *
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

#include "modules/xsd/namespacemanager.h"
#include "xmlutils.h"
#include "undo/undoeditcommand.h"
#include "utils.h"
#include "xsdeditor/XSchemaIOContants.h"
#include "modules/namespace/namespacereferenceentry.h"


// all that is needed to insert the xsd reference

bool Regola::insertXSDReference(QTreeWidget *tree, NamespaceManager &namespaceManager,
                                NamespaceReferenceEntry *command)
{
    Element *currentElement = root();
    bool isNamespacePresent = false;
    QString prefixForNamespaceXSI;
    bool isChanged = false;
    const QString XSINamespace = namespaceManager.namespaceUri(NamespaceManager::XSI_NAMESPACE);
    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, currentElement->indexPath());
    if(NULL != undoCommand) {
        undoCommand->setOriginalElement(currentElement);

        Element *cloned = new Element("", "", NULL, NULL);
        if(NULL != cloned) {
            currentElement->copyTo(*cloned);
            // remove any nillable attributes

            QString prefixNS ;
            if(currentElement->findPrefixForNamespace(XSINamespace, prefixNS)) {
                prefixForNamespaceXSI = prefixNS ;
                isNamespacePresent = true ;
            }
            // --- add namespace
            Utils::TODO_NEXT_RELEASE("refactor with similar methods");
            if(command->isUseNoNamespaceSchemaLocation() || !command->references().isEmpty()) {
                if(!isNamespacePresent) {
                    long nsId = 0 ;
                    bool doNextLoop ;
                    do {
                        doNextLoop = false;
                        QString ns ;
                        if(0 == nsId) {
                            ns = "xsi";
                        } else {
                            ns = QString("xsi%1").arg(nsId);
                        }
                        if(currentElement->nsDeclarationForPrefixOwned(ns) != NULL) {
                            doNextLoop = true ;
                            nsId ++ ;
                        } else {
                            currentElement->addAttribute(XmlUtils::makeNSDeclaration(ns), XSINamespace);
                            prefixForNamespaceXSI = ns ;
                            break;
                        }
                    } while(doNextLoop);
                }
            }

            QString noNamespaceSchemaLocationAttrName = XmlUtils::makeQualifiedName(prefixForNamespaceXSI, NamespaceManager::NoNamespaceSchemaLocationAttributeName);
            if(currentElement->setOrClearAttribute(!command->isUseNoNamespaceSchemaLocation(), noNamespaceSchemaLocationAttrName, command->noNamespaceSchemaLocation())) {
                isChanged = true ;
            }
            QString nsLocations ;
            foreach(SingleNamespaceReferenceEntry * entry, command->references()) {
                if(nsLocations.length() > 0) {
                    nsLocations += " ";
                }
                nsLocations += entry->uri();
                nsLocations += " ";
                nsLocations += entry->schemaLocation();
            }
            QString namespaceSchemaLocationAttrName = XmlUtils::makeQualifiedName(prefixForNamespaceXSI, NamespaceManager::SchemaLocationAttributeName);
            if(currentElement->setOrClearAttribute(command->references().isEmpty(), namespaceSchemaLocationAttrName, nsLocations)) {
                isChanged = true ;
            }
        }
        if(isChanged) {
            currentElement->updateSizeInfo();
            currentElement->markEditedRecursive();
            currentElement->display(currentElement->getUI(), paintInfo);
            setModified(true);
            if(NULL == currentElement->parent()) {
                checkEncoding();
            }
            undoCommand->setModifiedElement(currentElement);
            _undoStack.push(undoCommand);
        } else {
            delete undoCommand ;
        }
        if(NULL != cloned) {
            delete cloned ;
        }
    }
    return isChanged ;
}


void Regola::XSDReferences(NamespaceReferenceEntry *entry)
{
    Element *theRoot = root();
    entry->setUseNoNamespaceSchemaLocation(false);
    entry->setNoNamespaceSchemaLocation("");
    entry->setReferences(QList<SingleNamespaceReferenceEntry*>());
    if(NULL == theRoot) {
        return ;
    }
    checkValidationReference();
    entry->setUseNoNamespaceSchemaLocation(_useNoNamespaceXSD);
    entry->setNoNamespaceSchemaLocation(_noNameSpaceXsd);
    //
    QPair<QString, QString> pair;
    foreach(pair, _schemaLocationsList) {
        entry->addSchemaReference(pair.first, pair.second);
    }
}
