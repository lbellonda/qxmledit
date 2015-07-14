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

#include "regola.h"
#include "namespacemanager.h"
#include "xmlutils.h"
#include "undo/undoeditcommand.h"
#include "utils.h"
#include "xsdeditor/XSchemaIOContants.h"

/**
 * @brief Regola::removeNillable
 * General usage:
 * for each attribute whose local name is nillable
 * Identify the namespace looking up to the root.
 *  If found, remove the attribute.
 *   If the attibute is removed, if the namespace is declared at the element level and none of the children is using it in elements
 *   and attributes, remove it.
 * @param tree
 * @param currentElement
 * @param namespaceManager
 * @return
 */
bool Regola::removeNillableAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager)
{
    bool isChanged = false;
    const QString XSINamespace = namespaceManager.namespaceUri(NamespaceManager::XSI_NAMESPACE);
    Utils::TODO_THIS_RELEASE("lavoro su original e uso cloned come copia");
    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, currentElement->indexPath());
    if(NULL != undoCommand) {
        undoCommand->setOriginalElement(currentElement);

        Element *cloned = new Element("", "", NULL, NULL);
        if(NULL != cloned) {
            currentElement->copyTo(*cloned);
            QList<Attribute*> toDelete;
            foreach(Attribute * attribute, cloned->attributes) {
                if(XmlUtils::stripNs(attribute->name) == IO_INSTANCE_NILLABLE_ATTRIB) {
                    QString prefix;
                    QString name;
                    XmlUtils::decodeQualifiedName(attribute->name, prefix, name);
                    if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                        toDelete.append(attribute);
                        currentElement->removeAttribute(attribute->name);
                        isChanged = true ;
                    }
                }
            }
            // --- test for removal
            foreach(Attribute * toDeleteAttribute, toDelete) {
                QString prefix;
                QString name;
                XmlUtils::decodeQualifiedName(toDeleteAttribute->name, prefix, name);
                if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                    Attribute *declaration = currentElement->nsDeclarationForPrefixOwned(prefix);
                    if(NULL != declaration) {
                        if(!currentElement->areChildrenUsingPrefix(prefix)) {
                            currentElement->removeAttribute(declaration->name);
                            isChanged = true ;
                        }
                    }
                }
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

bool Regola::removeNillableAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager)
{
    bool isChanged = false;
    const QString XSINamespace = namespaceManager.namespaceUri(NamespaceManager::XSI_NAMESPACE);
    Utils::TODO_THIS_RELEASE("lavoro su original e uso cloned come copia");
    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, currentElement->indexPath());
    if(NULL != undoCommand) {
        undoCommand->setOriginalElement(currentElement);

        Element *cloned = new Element("", "", NULL, NULL);
        if(NULL != cloned) {
            currentElement->copyTo(*cloned);
            QList<Attribute*> toDelete;
            foreach(Attribute * attribute, cloned->attributes) {
                if(XmlUtils::stripNs(attribute->name) == IO_INSTANCE_NILLABLE_ATTRIB) {
                    QString prefix;
                    QString name;
                    XmlUtils::decodeQualifiedName(attribute->name, prefix, name);
                    if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                        toDelete.append(attribute);
                        currentElement->removeAttribute(attribute->name);
                        isChanged = true ;
                    }
                }
            }
            // --- test for removal
            foreach(Attribute * toDeleteAttribute, toDelete) {
                QString prefix;
                QString name;
                XmlUtils::decodeQualifiedName(toDeleteAttribute->name, prefix, name);
                if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                    Attribute *declaration = currentElement->nsDeclarationForPrefixOwned(prefix);
                    if(NULL != declaration) {
                        if(!currentElement->areChildrenUsingPrefix(prefix)) {
                            currentElement->removeAttribute(declaration->name);
                            isChanged = true ;
                        }
                    }
                }
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

bool Regola::removeXSITypeAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager)
{
    bool isChanged = false;
    const QString XSINamespace = namespaceManager.namespaceUri(NamespaceManager::XSI_NAMESPACE);
    Utils::TODO_THIS_RELEASE("lavoro su original e uso cloned come copia");
    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, currentElement->indexPath());
    if(NULL != undoCommand) {
        undoCommand->setOriginalElement(currentElement);

        Element *cloned = new Element("", "", NULL, NULL);
        if(NULL != cloned) {
            currentElement->copyTo(*cloned);
            QList<Attribute*> toDelete;
            foreach(Attribute * attribute, cloned->attributes) {
                if(XmlUtils::stripNs(attribute->name) == IO_INSTANCE_NILLABLE_ATTRIB) {
                    QString prefix;
                    QString name;
                    XmlUtils::decodeQualifiedName(attribute->name, prefix, name);
                    if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                        toDelete.append(attribute);
                        currentElement->removeAttribute(attribute->name);
                        isChanged = true ;
                    }
                }
            }
            // --- test for removal
            foreach(Attribute * toDeleteAttribute, toDelete) {
                QString prefix;
                QString name;
                XmlUtils::decodeQualifiedName(toDeleteAttribute->name, prefix, name);
                if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                    Attribute *declaration = currentElement->nsDeclarationForPrefixOwned(prefix);
                    if(NULL != declaration) {
                        if(!currentElement->areChildrenUsingPrefix(prefix)) {
                            currentElement->removeAttribute(declaration->name);
                            isChanged = true ;
                        }
                    }
                }
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


bool Regola::insertNillableAttribute(QTreeWidget *tree, Element *currentElement, NamespaceManager &namespaceManager)
{
    bool isNamespacePresent = false;
    QString prefixForNamespaceXSI;
    bool isChanged = false;
    const QString XSINamespace = namespaceManager.namespaceUri(NamespaceManager::XSI_NAMESPACE);
    Utils::TODO_THIS_RELEASE("lavoro su original e uso cloned come copia");
    UndoEditCommand *undoCommand = new UndoEditCommand(tree, this, currentElement->indexPath());
    if(NULL != undoCommand) {
        undoCommand->setOriginalElement(currentElement);

        Element *cloned = new Element("", "", NULL, NULL);
        if(NULL != cloned) {
            currentElement->copyTo(*cloned);
            // remove any nillable attributes
            QList<Attribute*> toDelete;

            QString prefixNS ;
            if( currentElement->findPrefixForNamespace(XSINamespace, prefixNS) ) {
                prefixForNamespaceXSI = prefixNS ;
                isNamespacePresent = true ;
            }
            // --- remove any outstanding
            foreach(Attribute * attribute, cloned->attributes) {
                if(XmlUtils::stripNs(attribute->name) == IO_INSTANCE_NILLABLE_ATTRIB) {
                    QString prefix;
                    QString name;
                    XmlUtils::decodeQualifiedName(attribute->name, prefix, name);
                    if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                        toDelete.append(attribute);
                        currentElement->removeAttribute(attribute->name);
                        isChanged = true ;
                    }
                }
            }
            Utils::TODO_THIS_RELEASE("remove comments");
            /*if(isChanged) {
                // --- test for removal
                foreach(Attribute * toDeleteAttribute, toDelete) {
                    QString prefix;
                    QString name;
                    XmlUtils::decodeQualifiedName(toDeleteAttribute->name, prefix, name);
                    if(currentElement->namespaceForPrefix(prefix) == XSINamespace) {
                        Attribute *declaration = currentElement->nsDeclarationForPrefixOwned(prefix);
                        if(NULL != declaration) {
                            if(!currentElement->areChildrenUsingPrefix(prefix)) {
                                currentElement->removeAttribute(declaration->name);
                                isChanged = true ;
                            }
                        }
                    }
                }
            }*/
            // --- add namespace
            if(!isNamespacePresent) {
                long nsId = 0 ;
                bool doNextLoop ;
                do {
                    doNextLoop = false;
                    QString ns ;
                    if( 0 == nsId ) {
                        ns = "xsi";
                    } else {
                        ns = QString("xsi%1").arg(nsId);
                    }
                    if( currentElement->nsDeclarationForPrefixOwned(ns) != NULL ) {
                        doNextLoop = true ;
                        nsId ++ ;
                    } else {
                        currentElement->addAttribute(XmlUtils::makeNSDeclaration(ns), XSINamespace);
                        prefixForNamespaceXSI = ns ;
                        break;
                    }
                } while(doNextLoop);
            }
            // --- add attribute
            QString completeAttributeName = XmlUtils::makeQualifiedName(prefixForNamespaceXSI, IO_INSTANCE_NILLABLE_ATTRIB);
            currentElement->addAttribute(completeAttributeName,"true");
            isChanged = true ;
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

