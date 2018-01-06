/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2014-2018 by Luca Bellonda and individual contributors  *
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

bool XSDHelper::updateSimpleContentRestrictionElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeSimpleContentRestrictionElement();
    return execOper(element, params, oper);
}

bool XSDHelper::insertComplexTypeNoneElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeNoneElement();
    return execOper(element, params, oper);
}

bool XSDHelper::insertComplexTypeSequenceElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeSequenceElement();
    return execOper(element, params, oper);
}

bool XSDHelper::updateComplexTypeSequenceElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeSequenceElement();
    return execOper(element, params, oper);
}

bool XSDHelper::updateComplexTypeNoneElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeNoneElement();
    return execOper(element, params, oper);
}

bool XSDHelper::insertComplexTypeAllElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeAllElement();
    return execOper(element, params, oper);
}

bool XSDHelper::updateComplexTypeAllElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeAllElement();
    return execOper(element, params, oper);
}

bool XSDHelper::insertComplexTypeChoiceElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeChoiceElement();
    return execOper(element, params, oper);
}

bool XSDHelper::updateComplexTypeChoiceElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeChoiceElement();
    return execOper(element, params, oper);
}

bool XSDHelper::insertComplexTypeGroupElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeGroupElement();
    return execOper(element, params, oper);
}

bool XSDHelper::updateComplexTypeGroupElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeGroupElement();
    return execOper(element, params, oper);
}

bool XSDHelper::updateComplexContentRestrictionElement(Element *element, XSDOperationParameters *params)
{
    XSDOper *oper = _factory.getComplexTypeComplexContentRestrictionElement();
    return execOper(element, params, oper);
}
