/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011 by Luca Bellonda and individual contributors       *
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
//#define LOG_CONSOLE
#include "schemavalidator.h"
#include "xsdeditor/validator/xvalidationcontext.h"
#include "utils.h"


SchemaValidator::SchemaValidator()
{
}

SchemaValidator::~SchemaValidator()
{
}

XElementContent *SchemaValidator::getAvailableContent(XElementContent *content, XSDSchema *root, Element* currentElement, const bool startAsRoot)
{
    if(NULL == root) {
        TRACE("No schema yet");
        return NULL ;
    }
    TRACE("0: starting search\n");
    XSchemaObject* object = root ;
    if(!startAsRoot) {
        if(NULL == currentElement) {
            TRACE("No element");
            return NULL ;
        }

        QList<SchemaSearchContext*> path = getPathFromElement(currentElement);
        foreach(SchemaSearchContext * component, path) {
            object = object->findComponentInValidationPath(component) ;
            if(NULL == object) {
                break;
            }
            if(NULL != object) {
                QString resultString = QString("Found element path:%1").arg(component->targetTag());
                TRACEQ(resultString);
            }
        }
        foreach(SchemaSearchContext * searchItem, path) {
            delete searchItem;
        }
        path.clear();
    } else {
        TRACE("0: starting from root\n");
    }

    if(NULL == object) {
        TRACE("1: path not found\n");
    } else {
        // TODO esamina il contesto
        TRACE2("2: found %s\n", object->name().toAscii().data());
        XValidationContext context(content);
        if(object->findSchemaChildComponents(&context, content)) {
            return content;
        }
    }
    return NULL ;
}

QStringList SchemaValidator::getInsertionPointList(XSDSchema *root, Element* currentElement, const bool startAsRoot)
{
    QStringList result ;
    XElementContent content;
    if(NULL != getAvailableContent(&content, root, currentElement, startAsRoot)) {
        foreach(XSingleElementContent * targetContent, content.allowedItems()) {
            result.append(targetContent->item()->name());
        }
    }
    return result;
}

bool SchemaValidator::getInsertionPointAllowedItems(XElementContent *content, XSDSchema *root, Element* currentElement, const bool startAsRoot)
{
    if(NULL != getAvailableContent(content, root, currentElement, startAsRoot)) {
        return true;
    }
    return false ;
}

#if 0
TODO: old code, albeit interesting for the future
QStringList SchemaValidator::checkInsertionPoint(/*XElementContentDescription *context,*/ XSDSchema *root, Regola *regola, Element* currentElement)
{
    QStringList result ;
    if(NULL == root) {
        TRACE("No schema yet");
        return result;
    }
    if(NULL == currentElement) {
        TRACE("No element");
        return result;
    }
    if(NULL == regola) {
        TRACE("No document");
        return result;
    }
    TRACE("0: starting search\n");
    QList<QString> path = getPathFromElement(currentElement);
    XSchemaObject* object = root ;
    foreach(QString component, path) {
        object = object->findComponentInValidationPath(component) ;
        if(NULL == object) {
            break;
        }
        if(NULL != object) {
            QString result = QString("Found element path:%1").arg(component);
            TRACE2("%s", result.toAscii().data());
        }
    }
    if(NULL == object) {
        TRACE("1: path not found\n");
    } else {
        //XValidationContext context;
        // TODO esamina il contesto
        TRACE2("2: found %s\n", object->name().toAscii().data());
        XValidationContext context;
        XElementContent content;
        if(object->findSchemaChildComponents(&context, &content)) {
            foreach(XSingleElementContent * targetContent, content.items()) {
                result.append(targetContent->item()->name());
            }
        }

    }
    return result ;
}
#endif

QList<SchemaSearchContext*> SchemaValidator::getPathFromElement(Element *element)
{
    QList<SchemaSearchContext*> path;
    Element *theElement = element ;
    TRACEQ(QString("Entering getPathFromElement for %1\n").arg(element->tag()));
    int level = 0;
    while(NULL != theElement) {
        level++;
        TRACE("Element test\n");
        Element *theParent = theElement->parent();
        if(theElement->isElement()) {
            Utils::TODO_NEXT_RELEASE("Do Error checking");
            // find the item position
            int index = 0 ;
            if(NULL != theParent) {
                bool found = false;
                foreach(Element * aChild, theParent->getItems()) {
                    // test if it is an element
                    if(aChild == element) {
                        found = true ;
                        break;
                    } else {
                        // another sibling element
                        if(aChild->getType() == Element::ET_ELEMENT) {
                            index ++;
                        }
                    }
                } // scan all childrens
                if(!found) {
                    TRACEQ(QString("ERROR**** not found at level:%1\n").arg(level));
                }
            } // if exists a parent

            TRACEQ(QString("Found the element at index:%1, level:%2\n").arg(index).arg(level));
            // now we have the correct index
            SchemaSearchContext* newContext = new SchemaSearchContext();
            newContext->setTarget(theParent, theElement, index);
            path.insert(0, newContext);
        } // if it is an element
        theElement = theParent;
    }
    TRACEQ(QString("Exiting getPathFromElement for %1\n").arg(element->tag()));
    return path;
}


/**
  called at the start of a frame
  */
XSchemaObject *XSDSchema::findComponentInValidationPath(const QString &component)
{
    // TODO: unire con findSchemaChildComponents, gestire redefine, import, etc.
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement * element = (XSchemaElement *) child ;
            if(element->name() == component) {
                return element ;
            }
        }
    }
    return NULL;
}

XSchemaObject *XSDSchema::findComponentInValidationPath(SchemaSearchContext *context)
{
    // TODO: unire con findSchemaChildComponents, gestire redefine, import, etc.
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement * element = (XSchemaElement *) child ;
            if(element->name() == context->targetTag()) {
                return element ;
            }
        }
    }
    return NULL;
}



bool XSDSchema::findSchemaChildComponents(XValidationContext *context, XElementContent *content)
{
    bool atLeastOne = false;
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement * element = (XSchemaElement *) child ;
            content->addAllowed(context, element);
            atLeastOne = true ;
        }
    }
    return atLeastOne ;
}


XSchemaObject *XSchemaChoice::findComponentInValidationPath(const QString & /*component*/)
{
    /* TODO: unire con findSchemaChildComponents
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement * element = (XSchemaElement *) child ;
            if( element->name() == component ) {
                return element ;
            }
        }
    }*/
    return NULL;
}

bool XSchemaChoice::findSchemaChildComponents(XValidationContext * /*context*/, XElementContent * /*content*/)
{
    /* TODO
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement * element = (XSchemaElement *) child ;
            if( element->name() == component ) {
                return element ;
            }
        }
    }*/
    return false;
}

#if 0
XSchemaObject *XSchemaElement::examineType(XValidatio XElementContent *content)
{
    XSchemaElement *theType = _root->getType(xsdType());
    if(NULL == theType) {
        return NULL ;
    }
    if(isSimpleType()) {
        return NULL;
    }
    // complex type
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == XSchemaTypeAll) {
            foreach(XSchemaObject * childAll, _children) {
                XSchemaElement * elementAll = (XSchemaElement *) childAll ;
                XSingleElementContent *newContent =  new XSingleElementContent(childAll, childAll->name());
                newContent->addAllowed(content);
            }
        } else if(child->getType() == XSchemaTypeRestriction) {
            se complex...
            foreach(XSchemaObject * child, _children) {
                if()
                    se sequence, prendi i figli della sequence
                    se choice metti un o e prendi figli
                    XSchemaElement * elementAll = (XSchemaElement *) childAll ;
                XSingleElementContent *newContent =  new XSingleElementContent(childAll, childAll->name());
                newContent->addAllowed(content);
            }

        }
    }
}
#endif

/**
  called at the start of a frame
  */

XSchemaObject *XSchemaObject::findComponentInValidationPath(SchemaSearchContext * searchContext)
{
    XElementContent content;
    XValidationContext context(&content);
    TRACEQ(QString("Begin findComponentInValidationPath for%1").arg(description()));
    if(!searchContext->initScan()) {
        TRACEQ(QString("..init failed"));
        return NULL ;
    }

    if(findSchemaChildComponents(&context, &content)) {
        // the content has the allowed childrens
        foreach(XSingleElementContent * child, content.allowedItems()) {

            TRACEQ(QString("..Scanning in %1").arg(child->item()->name()));
            if(SchemaSearchContext::FOUND == child->scanForPosition(searchContext, child->isAll())) {

                TRACEQ(QString("..Found\n"));
                XSingleElementContent* itemFound = searchContext->foundItem() ;
                if(NULL != itemFound) {

                    TRACEQ(QString("..Found element %1").arg(itemFound->item()->name()));
                    return itemFound->item();
                }
            }
        }
    }
    TRACEQ(QString("INVALID findComponentInValidationPath\n"));
    return NULL;
}

bool XSchemaObject::findSchemaChildComponents(XValidationContext * /*context*/, XElementContent * /*content*/)
{
    return false;
}

/*
            }
        } else if(child->getType() == XSchemaTypeSequence ) {
            XSchemaSequence * sequence = (XSchemaSequence *) child;
            if( !sequence->collectAllowed(context, content) ) {
                return false ;
            }
        } else if(child->getType() == XSchemaTypeChoice ) {
            XSchemaChoice * choice = (XSchemaChoice *) child;
            //TODO choice->collectAllowed(context, content);
        } else if(child->getType() == XSchemaTypeGroup ) {
            XSchemaGroup * group = (XSchemaGroup *) child;
            //TODO group->collectAllowed(context, content);
        } //TODO: derivations else if (child->getType() == XSchemaTypeComplexContent ) {
    }
    return true ;
}
*/

/*
sequence
                XSchemaElement * elementAll = (XSchemaElement *) childAll ;
                XSingleElementContent *newContent =  new XSingleElementContent(childAll,childAll->name());
                newContent->addAllowed(content);
            }
        } else if(child->getType() == XSchemaTypeSequence ) {
            collectSequence();
            // collect sequence children
            foreach(XSchemaObject * child, _children) {
                XSchemaElement * elementAll = (XSchemaElement *) childAll ;
                XSingleElementContent *newContent =  new XSingleElementContent(childAll,childAll->name());
                newContent->addAllowed(content);
            }
        } else if(child->getType() == XSchemaTypeChoice ) {
            foreach(XSchemaObject * child, _children) {
            XSingleElementContent *newContent =  new XSingleElementContent(childAll,childAll->name());

        }
}
*/
bool XSchemaElement::examineType(XValidationContext * context, XSingleElementContent * parent)
{
    // complex type
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeAll) {
            XSchemaAll *elementAll = (XSchemaAll *) child ;
            if(!elementAll->collect(context, parent)) {
                return false ;
            }
            /*foreach(XSchemaObject * childAll, child->getChildren()) {
                XSchemaAll *elementAll = (XSchemaAll *) childAll ;
                if(!elementAll->collect(context, parent)) {
                    return false ;
                }
            }*/
        } else if(child->getType() == SchemaTypeSequence) {
            XSchemaSequence * sequence = (XSchemaSequence *) child;
            if(!sequence->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeChoice) {
            XSchemaChoice * choice = (XSchemaChoice *) child;
            if(!choice->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeGroup) {
            XSchemaGroup * group = (XSchemaGroup *) child;
            if(!group->collect(context, parent)) {
                return false ;
            }
        } //TODO: derivations else if (child->getType() == XSchemaTypeComplexContent ) {
    }
    return true ;
}

bool XSchemaComplexContent::examineType(XValidationContext * context, XSingleElementContent * parent)
{
    // complex type
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeAll) {
            XSchemaAll *elementAll = (XSchemaAll *) child ;
            if(!elementAll->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeSequence) {
            XSchemaSequence * sequence = (XSchemaSequence *) child;
            if(!sequence->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeChoice) {
            XSchemaChoice * choice = (XSchemaChoice *) child;
            if(!choice->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeGroup) {
            XSchemaGroup * group = (XSchemaGroup *) child;
            if(!group->collect(context, parent)) {
                return false ;
            }
        }
    }
    return true ;
}


void XSchemaComplexContent::collectAttributes(XSchemaAttributesCollection & attributesCollection)
{
    foreach(XSchemaObject * child, getChildren()) {
        if(child->getType() == SchemaTypeAttribute) {
            XSchemaAttribute *aChild = static_cast<XSchemaAttribute*>(child);
            addAttributeToCollection(aChild->name(), attributesCollection, aChild);
        } else if(child->getType() == SchemaTypeAttributeGroup) {
            XSchemaAttributeGroup *aChild = static_cast<XSchemaAttributeGroup*>(child);
            addAttributeGroupToCollection(attributesCollection, aChild);
        }
    }
}

bool XSchemaComplexContent::hasAReference()
{
    return !_base.isEmpty();
}

QString XSchemaComplexContent::referencedObjectName()
{
    return _base;
}

XReferenceType XSchemaComplexContent::referencedObjectType()
{
    return XRT_TYPE;
}


XSchemaElement *XSchemaComplexContent::getBaseType()
{
    if(_base.isEmpty()) {
        return NULL ;
    }
    return getReferencedType(_base);
}

/*
The type definition corresponding to the <simpleType> or <complexType> element information item
    in the [children],
if either is present, otherwise the type definition ·resolved· to by the ·actual value· of
    the type [attribute], otherwise the {type definition} of the element declaration
    ·resolved· to by the ·actual value· of the substitutionGroup [attribute],
    if present, otherwise the ·ur-type definition·.
    */




//XSchemaObject *XSchemaElement::findComponentInValidationPath(const QString & component) {
//    XElementContent content;
//    XValidationContext context(&content);
//    if(findSchemaChildComponents(&context, &content)) {
//        return content.item(component);
//    }
//    return NULL ;
//#if 0
//    TODO delete ?
//    // gestione riferimento idref()
//    if(isTypeOrElement()) {
//        if(!examineType(&context, &content)) {
//            return NULL;
//        }
//        //TODO examineDerivations();
//    } else {
//        printf("TODO");
//    }
//    /*if( !xsdType.isEmpty() ) {
//           examineType(xsdType());
//           examineDerivations();
//       } else {
//           if()
//       }
//       se tipo semplice nulla
//               se tipo complesso, simplecontent o complex content
//               gruppo, all, choice, sequence
//    vai ricorsivo.
//       se non e' un tipo semplice,
//       cerca fra i figli


//        }
//        foreach(XSchemaObject * child, _children) {
//       if(child->getType() == SchemaTypeElement ) {
//           XSchemaElement * element = (XSchemaElement *) child ;
//           if( element->name() == component ) {
//               return element ;
//           }
//       }
//        }*/
//    return content.item(component);
//#endif
//}

/*
  TODO: questa funzione viene sostituita da una f. virtuale e trasformata nel solo collect di item inteni (da completare)
XSchemaObject *XSchemaElement::findComponentInValidationPath(SchemaSearchContext *context)
{
#if 0
    TODO delete ?
    // gestione riferimento idref()
    if(isTypeOrElement()) {
        if(!examineType(&context, &content)) {
            return NULL;
        }
        //TODO examineDerivations();
    } else {
        printf("TODO");
    }
    / *if( !xsdType.isEmpty() ) {
           examineType(xsdType());
           examineDerivations();
       } else {
           if()
       }
       se tipo semplice nulla
               se tipo complesso, simplecontent o complex content
               gruppo, all, choice, sequence
    vai ricorsivo.
       se non e' un tipo semplice,
       cerca fra i figli


        }
        foreach(XSchemaObject * child, _children) {
       if(child->getType() == SchemaTypeElement ) {
           XSchemaElement * element = (XSchemaElement *) child ;
           if( element->name() == component ) {
               return element ;
           }
       }
        }* /
    return content.item(component);
}
*/

bool XSchemaElement::elabTypeForCollect(XValidationContext * context, XElementContent * /*content*/)
{
    if(!xsdType().isEmpty()) {
        XSchemaElement *baseType = _root->schema()->topLevelType(xsdType());
        // TODO: better checking
        if(NULL == baseType) {
            return false ;
        }
        if(!baseType->examineType(context, NULL)) {
            return false;
        }
    }
    return true ;
}

bool XSchemaElement::findSchemaChildComponents(XValidationContext * context, XElementContent * content)
{
    // gestione riferimento idref()
    if(isTypeOrElement()) {
        if(!examineType(context, NULL)) {
            return false ;
        }
        //TODO examineDerivations();
    } else {
        // TODO: complete handling...
        switch(_elementCategory) {
        default:
            if(!examineType(context, NULL)) {
                return false;
            }
            break;
        case EES_EMPTY:
            if(!elabTypeForCollect(context, content)) {
                return false;
            }
            break;
        case EES_SIMPLETYPE_ONLY:
        case EES_SIMPLETYPE_WITHATTRIBUTES:
            break;
        case EES_REFERENCE: {
            XSchemaElement *referencedElement = _root->schema()->topLevelElement(_ref);
            if(NULL != referencedElement) {
                if(referencedElement->findSchemaChildComponents(context, content)) {
                    return true;
                }
            }
            return false;
        }

        case EES_COMPLEX_DERIVED: { // TODO complexType -  complexContent
            if(isExtension()) {
                // 1- find base type
                if(!elabTypeForCollect(context, content)) {
                    return false;
                }
                // 2- add direct children
                XSchemaComplexContentExtension *extension = (XSchemaComplexContentExtension*)_children.at(0);
                if(!extension->examineType(context, NULL)) {
                    return false;
                }
            } else {
                XSchemaComplexContentRestriction *restriction = (XSchemaComplexContentRestriction*)_children.at(0);
                // restriction, put only children
                if(!restriction->examineType(context, NULL)) {
                    return false;
                }
            }
            break;
        }
        }// switch
    } // if()
    return true ;
}

#if 0
TODO: controllare
XSchemaObject *XSchemaElement::collect(const QString & component)
{
    XValidationContext context;
    XElementContent content;
    // gestione riferimento idref()
    if(isTypeOrElement()) {
        if(!examineType(&context, &content)) {
            return NULL;
        }
        //TODO examineDerivations();
    } else {
        printf("TODO");
    }
    /*if( !xsdType.isEmpty() ) {
           examineType(xsdType());
           examineDerivations();
       } else {
           if()
       }
       se tipo semplice nulla
               se tipo complesso, simplecontent o complex content
               gruppo, all, choice, sequence
    vai ricorsivo.
       se non e' un tipo semplice,
       cerca fra i figli


        }
        foreach(XSchemaObject * child, _children) {
       if(child->getType() == SchemaTypeElement ) {
           XSchemaElement * element = (XSchemaElement *) child ;
           if( element->name() == component ) {
               return element ;
           }
       }
        }*/
    return content.item(component);
}
#endif

/*
XSchemaObject *XSDElement::findNextComponent(const QString &component)
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == XSDSchema::elelento) {
            XSchemaElement * = if name == name
                               return OK;
    }
}
}

XSchemaObject *XSDGroup::findNextComponent(const QString &component)
{
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == XSDSchema::elelento) {
            XSchemaElement * = if name == name
                               return OK;
    }
}
}
*/

//class SchemaSearchPosition {

//    bool isEof;
//    int  currentPos;
//    XSingleElementContent *current;

//    SchemaSearchPosition();
//    ~SchemaSearchPosition();

//    void copyTo(SchemaSearchPosition &copy);
//};

//class SchemaSearchContext
//{

//    enum ESearchState {
//        OK,
//        NOK,
//        FOUND,
//        NOTFOUND,
//        ERROR
//    };

//    // we explore more branches returning to starting point on failure.
//    // it is a FSA similar to those of a compiler, but we have already the
//    // grammar: it is the schema itself.
//    XSingleElementContent *_current;
//    XSingleElementContent *_lastMatched;
//    Element *_currentSource;
//    int _currentIndex;
//    Element *_target;
//    int _targetIndex;
//    bool _isEof ;

//    bool advanceSource();
//    bool isEof();
//    bool sourceIsTarget();
//    void setFoundElement(XSingleElementContent* foundItem);


//    SchemaSearchContext();
//    ~SchemaSearchContext();

//    /**
//      set the target of the search
//      */
//    void setTarget( Element *source, const int sourceIndex );
//};


//void XSingleElementContent::schemaSearchInit()
//{
//    _isExploring = true ;
//    // reset the total counter of itself and all its children recursively
//    resetOccurrencesRecursive(true);
//}

//void XSingleElementContent::resetOccurrencesRecursive(const bool resetSelf)
//{
//    _exploring = true ;
//    if( resetSelf ) {
//        _occurrences = 0;
//    }
//    foreach(XSingleElementContent * child, _children) {
//        child->resetOccurrences();
//    }
//}

//SchemaSearchContext::ESearchState XSingleElementContent::scanForPosition(SchemaSearchContext * context)
//{
//    bool redo = false;
//    schemaSearchInitRecursive(true);
//    SchemaSearchContextPosition pos;
//    do {
//        redo = false;
//        schemaSearchInitRecursive(false);
//        bool found = false;

//        foreach(XSchemaObject * child, children) {
//            if( isChoice() ) {
//                context->savePosition(pos);
//            }
//            SchemaSearchContext::ESearchState result = child->scanForPosition(context);
//            switch(result) {
//            default:
//                Utils::TODO_NEXT_RELEASE("errore");
//                return SchemaSearchContext::ERROR;
//                //------------------------------
//            case SchemaSearchContext::FOUND:
//                return  SchemaSearchContext::FOUND;
//                //------------------------------
//            case SchemaSearchContext::NOTFOUND:
//                if( isChoice ) {
//                    if(!handleNOk(context)) {
//                        return SchemaSearchContext::ERROR;
//                    }
//                } else {
//                    return  SchemaSearchContext::NOTFOUND;
//                }
//                //------------------------------
//            case SchemaSearchContext::ERROR:
//                return SchemaSearchContext::ERROR;
//                //------------------------------
//            case SchemaSearchContext::OK:
//                handleOk(context);
//                found = true ;
//                break;
//            case SchemaSearchContext::NOK:
//                if(!handleNOk(context)) {
//                    return SchemaSearchContext::ERROR;
//                }
//                break;
//            }
//            if(isChoice()) {
//                if(found) {
//                    break;
//                }
//                context->restorePosition(pos);
//            }
//        }
//        if((_occurrences == 0) && !isOptional()) {
//            return SchemaSearchContext::ERROR ;
//        }
//        if(_occurrences < minOccurs()) {
//            redo = true ;
//        }
//        la logica del min, max, ecc e' la stessa per tutti i tipi?
//                vedi piu' sotto, fare una funzione.'
//    } while(redo);
//    // it is impossible to be here
//    return SchemaSearchContext::ERROR ;
//}

//    SchemaSearchContext::ESearchState XSingleElementContent::handleOk(SchemaSearchContext * context) {
//        confirm(context);
//        while(child.occurrences < child.maxOccurrences) {
//            advance();
//            retest child;
//            until OK;
//        }
//    }

//    SchemaSearchContext::ESearchState XSchemaObject::handleNOk(SchemaSearchContext * context) {
//        if(child.isOptional) {
//            return SchemaSearchContext::OK;
//        }
//        return SchemaSearchContext::ERROR;
//    }


//    SchemaSearchContext::ESearchState XSingleElementContent::confirm(SchemaSearchContext * context) {
//        if(isExploring) {
//            isExploring = false ;
//            occurrences ++ ;
//            if(NULL != parent) {
//                parent->confirm();
//            }
//        }
//    }

/*
        if( result == KO ) {
            if( !child->isOptional() ) {
                return KO ;
            }
        } else {
            if( conferma ) {
                vai a confermare il parent
            }
            if( NULL != parent ) {
            }
            }
            if( child->occurrences <= child->maxOccurrences() ) {
                redo = true ;
            }
        }
        se disgiunto (choice o sostituz, basta uno)
        dddddd
    }

    {

        ;
}
*/
/*
SchemaSearchContext::ESearchState XSingleElementContent::handleMatch(SchemaSearchContext *context)
{
    guarda il prossimo.
    bool redoTest = false ;
    do {
        if( tag() == context->source().tag() ) {
            if( context->sourceIsTarget(/ *TODO:passare i parametri* /) ) {
                context->setFoundElement(this);
                return SchemaSearchContext::FOUND ;
            }
            occurrences ++ ;
            if( occurrences < child.maxOccurrences ) {
                context->advanceSource();
                if( context->eof() ) {
                    return SchemaSearchContext::NOTFOUND;
                }
                redoTest = true ;
                until OK;
            } else {
                ;
            }
            return SchemaSearchContext::OK;
        } else {
            ;
        }
        return SchemaSearchContext::NOK;
    } while( !redoTest );
    return XXX;
}
*/

//// this is a element specialized test
//SchemaSearchContext::ESearchState XSingleElementContent::evaluateMatch(SchemaSearchContext * context)
//{
//    if( item.tag() == context->source().tag() ) {
//        if(context->sourceIsTarget(/*TODO:passare i parametri*/)) {
//            context->setFoundElement(this);
//            return SchemaSearchContext::FOUND ;
//        }
//        return SchemaSearchContext::OK ;
//    }
//    return SchemaSearchContext::NOK ;
//}

///**
//  this function calculates the match of a given element with the source
//    returns : ERROR as an error, NOK as "not found."
//    if OK, a match is given (_occurrence reports the multeplicity), and the parent can mark itself counted too.
//    Notes: if a choice branch is explored and fails, it should return NOK (NOT error, given that there is a global evaluation).
//    */
//SchemaSearchContext::ESearchState XSingleElementContent::scanForElement(SchemaSearchContext * context)
//{
//    _occurrences = 0 ;
//    bool redoTest = false ;
//    do {
//        SchemaSearchContext::ESearchState state = evaluateMatch(context);
//        if( SchemaSearchContext::FOUND == state ) {
//            return SchemaSearchContext::FOUND ;
//        }
//        if( state == SchemaSearchContext::OK ) {
//            _occurrences ++ ;
//            if(_occurrences < child.maxOccurrences) {
//                context->advanceSource();
//                if(context->isEof()) {
//                    return SchemaSearchContext::NOTFOUND;
//                }
//                redoTest = true ;
//                // until OK
//            } else {
//                // NB: can be only equal, not major
//                return SchemaSearchContext::OK;
//            }
//        } else { // no match
//            return evalTerminalCondition();
//            /*TODO: todelete if( 0 == _occurrences ) {
//                if( isOptional) {
//                    return SchemaSearchContext::NOTFOUND; // this is a skip flag
//                } else {
//                    return SchemaSearchContext::NOK ;
//                }
//            } else { // occurrences > 0 ) {
//                if( _occurrences < child.minOccurrences() ) {
//                    return SchemaSearchContext::NOK ;
//                }
//                return SchemaSearchContext::OK ;
//            }*/
//       } // end of evaluation

//    } while( !redoTest );
//    // it is impossible to be here
//    return SchemaSearchContext::ERROR ;
//}

///**
//  evaluate the return state from a match in a general sense
//  */
//SchemaSearchContext::ESearchState XSingleElementContent::evalTerminalCondition()
//{
//    if( 0 == _occurrences ) {
//        if( isOptional) {
//            return SchemaSearchContext::NOTFOUND; // this is a skip flag
//        } else {
//            return SchemaSearchContext::NOK ;
//        }
//    } else { // occurrences > 0
//        if( _occurrences < child.minOccurrences() ) {
//            return SchemaSearchContext::NOK ;
//        }
//        return SchemaSearchContext::OK ;
//    }
//}

