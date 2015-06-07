/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014 by Luca Bellonda and individual contributors       *
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
#include "xsdhelper.h"
#include "utils.h"
#include "undo/elinsertcommand.h"
#include "undo/elupdateelementcommand.h"
#include "xsdeditor/XSchemaIOContants.h"

#include <QUndoCommand>

//-----
#define ATTRIB_NAME_REF "ref"
#define ATTRIB_NAME_TYPE_NAME   "type"
#define ATTRIB_NAME_NAME   "name"
#define ATTRIB_NAME_BASE   "base"
#define TAG_ANNOTATION     "annotation"

//-----

XSDAnnotationEditor::XSDAnnotationEditor()
{
    //
}

XSDAnnotationEditor::~XSDAnnotationEditor()
{
    //
}

XSDAnnotationEditProvider::XSDAnnotationEditProvider()
{
    //
}

XSDAnnotationEditProvider::~XSDAnnotationEditProvider()
{
    //
}

//-----

XSDHelper::XSDHelper()
{
}

XSDHelper::~XSDHelper()
{
}

// if the target is null, the element must be inserted and the path is ruling
bool XSDHelper::doOperation(const ElementOp::Op op, QTreeWidget *theWidget, Regola *regola, Element * targetElement, QList<int> path, XSDOperationParameters * params)
{
    bool result = false;
    bool isInsert = false;
    QUndoCommand *undoGroup = new QUndoCommand();
    Element *element = NULL ;
    if((ElementOp::EOInsChild == op) || (ElementOp::EOInsSibling == op)) {
        isInsert = true ;
        // full copy, if any
        element = new Element(elementName(params), "", NULL, NULL);
        // add a child to the undo command
        QList<int> destPath = path;
        if(ElementOp::EOInsChild == op) {
            destPath.append(targetElement->getChildItemsCount());
        } else {
            int lastId = destPath.last();
            lastId ++;
            destPath.removeLast();
            destPath.append(lastId);
        }
        new ElInsertCommand(theWidget, regola, element, destPath, undoGroup);
    } else {
        element = new Element(elementName(params), "", NULL, NULL);
        targetElement->copyTo(*element);
        element->setTag(elementName(params));
        new ElUpdateCommand(theWidget, regola, element, path, undoGroup);
    }

    if(NULL != element) {
        if(isInsert) {
            result = doInsert(regola, element, params);
        } else {
            result = doUpdate(regola, element, params);
        }
        regola->addUndo(undoGroup);
    }
    return result;
}


bool XSDHelper::doInsert(Regola *regola, Element *targetElement, XSDOperationParameters * params)
{
    switch(params->objType()) {
    default:
        return false;
    case XSDOperationParameters::EOI_ELEMENT:
        return processElement(regola, targetElement, params);

    case XSDOperationParameters::EOI_TYPE:
        return processType(targetElement, params);

    case XSDOperationParameters::EOI_ATTRIBUTE:
        return processAttribute(targetElement, params);
    }
}

bool XSDHelper::doUpdate(Regola *regola, Element *targetElement, XSDOperationParameters * params)
{
    switch(params->objType()) {
    default:
        return false;
    case XSDOperationParameters::EOI_ELEMENT:
        return processElement(regola, targetElement, params);

    case XSDOperationParameters::EOI_TYPE:
        return processType(targetElement, params);

    case XSDOperationParameters::EOI_ATTRIBUTE:
        return processAttribute(targetElement, params);
    }
}


bool XSDHelper::processElement(Regola *regola, Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeSpec() == XSDOperationParameters::TS_SIMPLE) {
        return elementSimpleType(regola, targetElement, params);
    } else {
        return updateElementComplexType(targetElement, params);
    }
}

bool XSDHelper::processAttribute(Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeSpec() == XSDOperationParameters::TS_SIMPLE) {
        return attributeSimpleType(targetElement, params);
    } else {
        return false;
    }
}

bool XSDHelper::processType(Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeSpec() == XSDOperationParameters::TS_SIMPLE) {
        return processTypeSimpleType(targetElement, params);
    } else {
        return updateTypeComplexType(targetElement, params);
    }
}

bool XSDHelper::elementSimpleType(Regola *regola, Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeTo() == XSDOperationParameters::EXT_REFERENCE_TYPE) {
        return elementSimpleTypeReference(regola, targetElement, params);
    } else {
        switch(params->subOper()) {
        default:
            return false;
        case XSDOperationParameters::TSS_NONE:
            return execOper(targetElement, params, _factory.getSimpleTypeNoneElement());

        case XSDOperationParameters::TSS_RESTRICTION:
            return execOper(targetElement, params, _factory.getSimpleTypeRestrictionElement());

        case XSDOperationParameters::TSS_LIST:
            return execOper(targetElement, params, _factory.getSimpleTypeListElement());

        case XSDOperationParameters::TSS_UNION:
            return execOper(targetElement, params, _factory.getSimpleTypeUnionElement());

        }
    }
}

bool XSDHelper::attributeSimpleType(Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeTo() == XSDOperationParameters::EXT_REFERENCE_TYPE) {
        return execOper(targetElement, params, _factory.getSimpleTypeReferenceAttribute());
    } else {
        switch(params->subOper()) {
        default:
            return false;
        case XSDOperationParameters::TSS_NONE:
            return execOper(targetElement, params, _factory.getSimpleTypeNoneAttribute());

        case XSDOperationParameters::TSS_RESTRICTION:
            return execOper(targetElement, params, _factory.getSimpleTypeRestrictionAttribute());

        case XSDOperationParameters::TSS_LIST:
            return execOper(targetElement, params, _factory.getSimpleTypeListAttribute());

        case XSDOperationParameters::TSS_UNION:
            return execOper(targetElement, params, _factory.getSimpleTypeUnionAttribute());

        }
    }
}

bool XSDHelper::processTypeSimpleType(Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeTo() == XSDOperationParameters::EXT_REFERENCE_TYPE) {
        return execOper(targetElement, params, _factory.getSimpleTypeReference());
    } else {
        switch(params->subOper()) {
        default:
            return false;
        case XSDOperationParameters::TSS_NONE:
            return execOper(targetElement, params, _factory.getSimpleTypeNoneType());

        case XSDOperationParameters::TSS_RESTRICTION:
            return execOper(targetElement, params, _factory.getSimpleTypeRestrictionType());

        case XSDOperationParameters::TSS_LIST:
            return execOper(targetElement, params, _factory.getSimpleTypeListType());

        case XSDOperationParameters::TSS_UNION:
            return execOper(targetElement, params, _factory.getSimpleTypeUnionType());
        }
    }
}

bool XSDHelper::updateElementComplexType(Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeTo() == XSDOperationParameters::EXT_REFERENCE_TYPE) {
        return elementComplexTypeReference(targetElement, params);
    } else {
        switch(params->typeContent()) {
        default:
            return false;
        case XSDOperationParameters::TSC_SIMPLE:
            switch(params->subOper()) {
            default:
                return false;

            case XSDOperationParameters::TSS_EXTENSION:
                return execOper(targetElement, params, _factory.getComplexTypeSimpleContentExtensionElement());

            case XSDOperationParameters::TSS_RESTRICTION:
                return updateSimpleContentRestrictionElement(targetElement, params);

            case XSDOperationParameters::TSS_NONE:
                return execOper(targetElement, params, _factory.getComplexTypeSimpleContentNoneElement());
            }
            break;

        case XSDOperationParameters::TSC_COMPLEX:
            switch(params->subOper()) {
            default:
                return false;

            case XSDOperationParameters::TSS_EXTENSION:
                return execOper(targetElement, params, _factory.getComplexTypeComplexContentExtensionElement());

            case XSDOperationParameters::TSS_RESTRICTION:
                return updateComplexContentRestrictionElement(targetElement, params);

            case XSDOperationParameters::TSS_NONE:
                return updateComplexTypeNoneElement(targetElement, params);
            }
            break;

        case XSDOperationParameters::TSC_NONE:
            switch(params->subOper()) {
            default:
                return false;

            case XSDOperationParameters::TSS_NONE:
                return updateComplexTypeNoneElement(targetElement, params);

            case XSDOperationParameters::TSS_SEQUENCE:
                return updateComplexTypeSequenceElement(targetElement, params);

            case XSDOperationParameters::TSS_CHOICE:
                return updateComplexTypeChoiceElement(targetElement, params);

            case XSDOperationParameters::TSS_GROUP:
                return updateComplexTypeGroupElement(targetElement, params);

            case XSDOperationParameters::TSS_ALL:
                return updateComplexTypeAllElement(targetElement, params);
            }
            break;
        }
    }
}

bool XSDHelper::updateTypeComplexType(Element *targetElement, XSDOperationParameters * params)
{
    if(params->typeTo() == XSDOperationParameters::EXT_REFERENCE_TYPE) {
        // never here
        return false;
    } else {
        switch(params->typeContent()) {
        default:
            return false;
        case XSDOperationParameters::TSC_SIMPLE:
            switch(params->subOper()) {
            default:
                return false;
            case XSDOperationParameters::TSS_EXTENSION:
                return execOper(targetElement, params, _factory.getComplexTypeSimpleContentExtensionType());

            case XSDOperationParameters::TSS_RESTRICTION:
                return execOper(targetElement, params, _factory.getComplexTypeSimpleContentRestrictionType());

            case XSDOperationParameters::TSS_NONE:
                return execOper(targetElement, params, _factory.getComplexTypeSimpleContentNoneType());
            }
            break;

        case XSDOperationParameters::TSC_COMPLEX:
            switch(params->subOper()) {
            default:
                return false;

            case XSDOperationParameters::TSS_EXTENSION:
                return execOper(targetElement, params, _factory.getComplexTypeComplexContentExtensionType());

            case XSDOperationParameters::TSS_RESTRICTION:
                return execOper(targetElement, params, _factory.getComplexTypeComplexContentRestrictionType());

            case XSDOperationParameters::TSS_NONE:
                return execOper(targetElement, params, _factory.getComplexTypeNoneType());
            }
            break;

        case XSDOperationParameters::TSC_NONE:
            switch(params->subOper()) {
            default:
                return false;

            case XSDOperationParameters::TSS_NONE:
                return execOper(targetElement, params, _factory.getComplexTypeNoneType());

            case XSDOperationParameters::TSS_SEQUENCE:
                return execOper(targetElement, params, _factory.getComplexTypeSequenceType());

            case XSDOperationParameters::TSS_CHOICE:
                return execOper(targetElement, params, _factory.getComplexTypeChoiceType());

            case XSDOperationParameters::TSS_GROUP:
                return execOper(targetElement, params, _factory.getComplexTypeGroupType());

            case XSDOperationParameters::TSS_ALL:
                return execOper(targetElement, params, _factory.getComplexTypeAllType());
            }
            break;
        }
    }
}

bool XSDHelper::elementSimpleTypeReference(Regola *regola, Element *element, XSDOperationParameters *params)
{
    removeChildren(regola, element, params->isConserveAnnotations());

    removeRefAttribute(element);
    setNameAttribute(element, params->name());
    setTypeAttribute(element, params->typeName());
    return true ;
}

bool XSDHelper::elementComplexTypeReference(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeReferenceElement();
    if(NULL != oper) {
        applyOperation(element, oper, params);
        delete oper ;
    }
    return true ;
}


bool XSDHelper::insertSimpleContentRestrictionElement(Regola *regola, Element *element, XSDOperationParameters *params)
{
    removeChildren(regola, element, params->isConserveAnnotations());

    removeRefAttribute(element);
    removeRefAttribute(element);
    setNameAttribute(element, params->name());
    Element *simpleType = new Element(makeName(params, "simpleType"), "", NULL, element);
    element->addChild(simpleType);
    Element *restriction = new Element(makeName(params, "restriction"), "", NULL, simpleType);
    restriction->addAttribute(ATTRIB_NAME_BASE, params->typeName());
    simpleType->addChild(restriction);
    return true;
}

bool XSDHelper::insertSimpleContentExtensionElement(Regola *regola, Element *element, XSDOperationParameters *params)
{
    removeChildren(regola, element, params->isConserveAnnotations());

    removeRefAttribute(element);
    setNameAttribute(element, params->name());
    Element *simpleType = new Element(makeName(params, "simpleType"), "", NULL, element);
    element->addChild(simpleType);
    Element *extension = new Element(makeName(params, "extension"), "", NULL, simpleType);
    extension->addAttribute(ATTRIB_NAME_BASE, params->typeName());
    simpleType->addChild(extension);
    return true;
}

bool XSDHelper::execOper(Element *element, XSDOperationParameters *params, XSDOper *oper)
{
    if(NULL != oper) {
        applyOperation(element, oper, params);
        delete oper ;
    }
    return true ;
}

void XSDHelper::removeRefAttribute(Element *element)
{
    element->removeAttribute(ATTRIB_NAME_REF);
}

void XSDHelper::removeTypeAttribute(Element *element)
{
    element->removeAttribute(ATTRIB_NAME_TYPE_NAME);
}

void XSDHelper::setTypeAttribute(Element *element, const QString &typeName)
{
    element->setAttribute(ATTRIB_NAME_TYPE_NAME, typeName);
}

void XSDHelper::setNameAttribute(Element *element, const QString &name)
{
    element->setAttribute(ATTRIB_NAME_NAME, name);
}

void XSDHelper::removeChildren(Regola *regola, Element *newElement, const bool isConserveAnnotations)
{
    QList<int> toDelete ;
    int index = 0;
    foreach(Element * el, *newElement->getChildItems()) {
        QString aPrefix;
        QString aLocalName;
        el->namespaceOfElement(aPrefix, aLocalName);
        if(!((regola->namespaceOfPrefix(aPrefix) == Regola::XSDNameSpace)  && (aLocalName == IO_XSD_ANNOTATION) && isConserveAnnotations)) {
            toDelete.append(index);
        }
        index++;
    }
    foreach(int indexOfEl, toDelete) {
        newElement->getChildItems()->remove(indexOfEl);
    }
}

QString XSDHelper::elementName(XSDOperationParameters *params)
{
    QString newTag;
    QString prefix;
    if(!params->xsdNamespacePrefix().isEmpty()) {
        prefix = params->xsdNamespacePrefix();
        prefix += ":";
    }
    newTag = prefix ;
    switch(params->objType()) {
    case XSDOperationParameters::EOI_ELEMENT:
        newTag += "element";
        break;
    case XSDOperationParameters::EOI_TYPE:
        if(params->typeSpec() == XSDOperationParameters::TS_COMPLEX) {
            newTag += "complexType";
        } else if(params->typeSpec() == XSDOperationParameters::TS_SIMPLE) {
            newTag += "simpleType";
        }
        break;
    case XSDOperationParameters::EOI_ATTRIBUTE:
        newTag += "attribute";
        break;
    }

    return newTag;
}

QString XSDHelper::makeName(XSDOperationParameters *params, const QString &localName)
{
    return params->makeNameForXSDObject(localName);
}

void XSDHelper::removeChildrenFromElement(Element *element, QList<Element*>childrenToDeleteList)
{
    foreach(Element * childrenToDelete, childrenToDeleteList) {
        element->removeChild(childrenToDelete);
    }
}

int XSDHelper::findInChildren(const QString &name, Element *el, const int index, const int maxEl)
{
    for(int i = index ; i < maxEl ; i ++) {
        if(el->isElement()) {
            if(name == el->localName()) {
                return index ;
            }
        }
    }
    return -1 ;
}

//----------------------------------

void XSDHelper::applyOperation(Element *element, XSDOper *oper, XSDOperationParameters *params)
{
    foreach(QString attrToDelete, oper->attributesToRemove) {
        element->removeAttribute(attrToDelete);
    }
    QPair<QString, QString>attrToAdd;
    foreach(attrToAdd, oper->attributesToAdd) {
        QString value = params->decodeToken(attrToAdd.second);
        element->setAttribute(attrToAdd.first, value);
    }
    //
    QList<Element*> childrenToExamine;
    QList<Element*> childrenToDelete;
    foreach(Element * childElm, *element->getChildItems()) {
        if(childElm->isElement()) {
            XSDOper *childOper = oper->findOper(childElm);
            if(NULL == childOper) {
                childrenToDelete.append(childElm);
            } else {
                if(childOper->canStay) {
                    // can stay, no processing
                } else {
                    childrenToExamine.append(childElm);
                }
            }
        }
    }
    removeChildrenFromElement(element, childrenToDelete);
    childrenToDelete.clear();

    //----
    bool childrenFound = false;
    foreach(Element * childElm, childrenToExamine) {
        XSDOper *childOper = oper->findOper(childElm);
        if(childrenFound && oper->childrenMutuallyExclusive) {
            element->removeChild(childElm);
        } else {
            if(childOper->add) {
                childrenFound = true ;
                applyOperation(childElm, childOper, params);
            } else {
                // A very strange state.
            }
        }
    }

    //se non trova figli
    if(!childrenFound) {
        int index = 0 ;
        int maxEl = element->getChildItemsCount();
        //scan da inizo
        // cerca nei figli
        //  se trova se stay aggiorna, se add aggiorna;
        //  se non trova inserisci dove sta.
        foreach(XSDOper * childOper, oper->children) {
            int newIndex = findInChildren(childOper->name, element, index, maxEl);
            if(newIndex >= 0) {
                index = newIndex ;
            } else {
                // not found, insert
                if(childOper->add) {
                    Element *newElement = new Element(makeName(params, childOper->name), "", NULL, element);
                    element->addChild(newElement);
                    applyOperation(newElement, childOper, params);
                    break;
                }
            }
        }
    }
} // applyOperation()


//-----------------------------------------------------------------------------------


Element *XSDHelper::findAnnotation(Element *element, XSDOperationParameters *params)
{
    QSet<QString> qNames ;
    if(!params->usePrefix()) {
        qNames.insert(TAG_ANNOTATION);
    } else {
        foreach(QString key, params->xsdNamespacePrefixes().values()) {
            if(key.isEmpty()) {
                qNames.insert(TAG_ANNOTATION) ;
            } else {
                qNames.insert(QString("%1:%2").arg(key).arg(TAG_ANNOTATION)) ;
            }
        }
    }
    foreach(Element * childElm, *element->getChildItems()) {
        if(childElm->isElement()) {
            foreach(QString completName, qNames) {
                if(childElm->tag() == completName) {
                    return childElm ;
                }
            } // foreach names
        }
    } // for children
    return NULL;
}

bool XSDHelper::doAnnotation(QTreeWidget * theWidget, Regola *regola, Element * targetElement, Element * currentAnnotation, Element *newAnnotation)
{
    if((NULL == targetElement) || (NULL == regola) || ((NULL != currentAnnotation) && (currentAnnotation->parent() != targetElement))) {
        return false;
    }

    QUndoCommand *undoGroup ;
    if(NULL == currentAnnotation) {
        // full copy, if any
        // add a child to the undo command
        QList<int> destPath = targetElement->indexPath() ;
        destPath.append(0);
        ElInsertCommand *cmd = new ElInsertCommand(theWidget, regola, newAnnotation, destPath);
        cmd->setSelectParent(true);
        undoGroup = cmd ;
    } else {
        ElUpdateCommand *cmd = new ElUpdateCommand(theWidget, regola, newAnnotation, currentAnnotation->indexPath());
        cmd->setSelectParent(true);
        undoGroup = cmd ;
    }
    regola->addUndo(undoGroup);
    return true;
}


Element *XSDHelper::makeElementOther(XSchemaOther *other, Element *parent)
{
    Element * newElement = new Element(parent->getParentRule(), other->element()->getType(), parent);
    other->element()->copyTo(*newElement);
    return newElement ;
}

void XSDHelper::copyInnerContent(XInfoBase *doc, Element *element)
{
    QString innerSource = "<root>";
    innerSource += doc->contentString();
    innerSource += "</root>";
    QList<Element*> content = Regola::decodeXMLFromString(innerSource, false, false);
    foreach(Element * child, content) {
        element->addChild(child);
    }
}

Element *XSDHelper::makeElementDocumentation(XDocumentation *doc, Element *parent, XSDOperationParameters *params)
{
    Element * newElement = new Element(params->makeNameForXSDObject(IO_XSD_DOCUMENTATION), "", parent->getParentRule(),  parent);
    QString lang = doc->language();
    if(!lang.isEmpty()) {
        newElement->setAttribute(XML_LANGUAGE, lang);
    }
    QString source = doc->source();
    if(!source.isEmpty()) {
        newElement->setAttribute(IO_DOCUMENTATION_ATTR_SOURCE, source);
    }
    copyInnerContent(doc, newElement);
    return newElement ;
}

Element *XSDHelper::makeElementAppInfo(XAppInfo* appInfo, Element *parent, XSDOperationParameters *params)
{
    Element * newElement = new Element(params->makeNameForXSDObject(IO_XSD_TAGAPPINFO), "", parent->getParentRule(),  parent);
    QString source = appInfo->source();
    if(!source.isEmpty()) {
        newElement->setAttribute(IO_DOCUMENTATION_ATTR_SOURCE, source);
    }
    copyInnerContent(appInfo, newElement);
    return newElement ;
}
