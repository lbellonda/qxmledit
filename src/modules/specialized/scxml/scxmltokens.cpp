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

#include "scxmltoken.h"
#include "scxmlroot.h"
#include "dialogs/scxmlstatedialog.h"
#include "dialogs/scxmlparalleldialog.h"
#include "dialogs/scxmltransitiondialog.h"
#include "utils.h"

SCXMLGenericToken::SCXMLGenericToken()
{
}

SCXMLGenericToken::~SCXMLGenericToken()
{
}

bool SCXMLGenericToken::editToken(QWidget *, SCXMLInfo *, Regola *, const bool , const bool ,
                                  Element *, Element *, Element *)
{
    return false ;
}

//------------------------------------------------------------------

SCXMLRootToken::SCXMLRootToken()
{
}

SCXMLRootToken::~SCXMLRootToken()
{
}

bool SCXMLRootToken::editToken(QWidget *, SCXMLInfo *, Regola *, const bool , const bool ,
                               Element *, Element *, Element *)
{
    return false ;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(initial, SCXMLscxmlToken);
SC_IMPL_ATTR_TK(name, SCXMLscxmlToken);
SC_IMPL_ATTR_TK(xmlns, SCXMLscxmlToken);
SC_IMPL_ATTR_TK(version, SCXMLscxmlToken);
SC_IMPL_ATTR_TK(datamodel, SCXMLscxmlToken);
SC_IMPL_ATTR_TK(binding, SCXMLscxmlToken);

SCXMLscxmlToken::SCXMLscxmlToken()
{
}

SCXMLscxmlToken::~SCXMLscxmlToken()
{
}

bool SCXMLscxmlToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLRoot dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(initial, SCXMLstateToken);
SC_IMPL_ATTR_TK(id, SCXMLstateToken);

SCXMLstateToken::SCXMLstateToken()
{
}

SCXMLstateToken::~SCXMLstateToken()
{
}

bool SCXMLstateToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLStateDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(id, SCXMLparallelToken);

SCXMLparallelToken::SCXMLparallelToken()
{
}

SCXMLparallelToken::~SCXMLparallelToken()
{
}

bool SCXMLparallelToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                   Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLParallelDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(event, SCXMLtransitionToken);
SC_IMPL_ATTR_TK(cond, SCXMLtransitionToken);
SC_IMPL_ATTR_TK(target, SCXMLtransitionToken);
SC_IMPL_ATTR_TK(type, SCXMLtransitionToken);

SCXMLtransitionToken::SCXMLtransitionToken()
{
}

SCXMLtransitionToken::~SCXMLtransitionToken()
{
}

bool SCXMLtransitionToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                     Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLTransitionDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}
