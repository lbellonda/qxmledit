/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#include "schemavalidator.h"
#include "xsdeditor/validator/xvalidationcontext.h"
#include "utils.h"

bool XSchemaAll::collect(XValidationContext * context, XSingleElementContent *parent)
{
    XSingleElementContent *allRoot = context->addAllowed(parent, this);
    if(NULL == allRoot) {
        return false ;
    }
    // complex type
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement *element = (XSchemaElement *) child;
            if(!element->collect(context, allRoot)) {
                return false ;
            }
        }
    }
    return true ;
}

bool XSchemaGroup::collect(XValidationContext * context, XSingleElementContent *parent)
{
    if(!_ref.isEmpty()) {
        XSchemaGroup *group = _root->schema()->topLevelGroup(_ref);
        if(NULL == group) {
            return false ;
        }
        return group->collect(context, parent);
    }
    XSingleElementContent *newGroup = context->addAllowed(parent, this);
    if(NULL == newGroup) {
        return false ;
    }

    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeAll) {
            XSchemaAll *element = (XSchemaAll*) child;
            if(!element->collect(context, newGroup)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeChoice) {
            XSchemaChoice * choice = (XSchemaChoice *) child;
            if(!choice->collect(context, newGroup)) {
                return false ;
            }

        } else if(child->getType() == SchemaTypeSequence) {
            XSchemaSequence * sequence = (XSchemaSequence *) child;
            if(!sequence->collect(context, newGroup)) {
                return false ;
            }
        }
    }
    return true ;
}

bool XSchemaSequence::collect(XValidationContext * context, XSingleElementContent *parent)
{
    // complex type
    foreach(XSchemaObject * child, _children) {
        if(child->getType() == SchemaTypeElement) {
            XSchemaElement *element = (XSchemaElement *) child;
            if(!element->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeGroup) {
            XSchemaGroup * group = (XSchemaGroup *) child;
            if(!group->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeChoice) {
            XSchemaChoice * choice = (XSchemaChoice *) child;
            if(!choice->collect(context, parent)) {
                return false ;
            }

        } else if(child->getType() == SchemaTypeSequence) {
            XSchemaSequence * sequence = (XSchemaSequence *) child;
            if(!sequence->collect(context, parent)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeAny) {
            //TODO group->collectAllowed(context, parent);
        }
    }
    return true ;
}

/**
  \brief: choice collection is the name of the choice, plus choice childrens
  */
bool XSchemaChoice::collect(XValidationContext * context, XSingleElementContent * parent)
{
    XSingleElementContent *choiceRoot = context->addAllowed(parent, this);
    if(NULL == choiceRoot) {
        return false ;
    }
    // complex type
    foreach(XSchemaObject * child, _children) {

        if(child->getType() == SchemaTypeElement) {
            XSchemaElement * element = (XSchemaElement *) child;
            if(!element->collect(context, choiceRoot)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeGroup) {
            XSchemaGroup * group = (XSchemaGroup*) child;
            if(!group->collect(context, choiceRoot)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeChoice) {
            XSchemaChoice * choice = (XSchemaChoice*) child;
            if(!choice->collect(context, choiceRoot)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeSequence) {
            //insert the sequence, then recurse into
            //add the sequence as a title, then sequence content
            XSchemaSequence * sequence = (XSchemaSequence*) child;
            XSingleElementContent *newSequence = context->addAllowed(choiceRoot, sequence);
            if(NULL == newSequence) {
                return false ;
            }
            if(!sequence->collect(context, newSequence)) {
                return false ;
            }
        } else if(child->getType() == SchemaTypeAny) {
            Utils::TODO_NEXT_RELEASE("Mettere a punto, mostrare, magari non selezionabile");
        }
    } // foreach children
    return true ;
}

int XSchemaObject::minOccurrences()
{
    return 1;
}

int XSchemaObject::maxOccurrences()
{
    return 1;
}


bool XSchemaObject::collect(XValidationContext * context, XSingleElementContent * parent)
{
    if(!context->addAllowed(parent, this)) {
        return false ;
    }
    return true ;
}

bool XSchemaElement::collect(XValidationContext * context, XSingleElementContent * parent)
{
    XSingleElementContent *newObj = NULL ;
    if(!this->ref().isEmpty()) {
        Utils::TODO_NEXT_RELEASE("GESTIRE I REFERENCES");
        newObj = context->addAllowed(parent, this);
    } else {
        newObj = context->addAllowed(parent, this) ;
    }
    if(NULL == newObj) {
        return false ;
    }
    return true ;
}
