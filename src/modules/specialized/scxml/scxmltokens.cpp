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
#include "modules/specialized/scxml/dialogs/scxmlstatedialog.h"
#include "modules/specialized/scxml/dialogs/scxmlparalleldialog.h"
#include "modules/specialized/scxml/dialogs/scxmltransitiondialog.h"
#include "modules/specialized/scxml/dialogs/scxmlfinaldialog.h"
#include "modules/specialized/scxml/dialogs/scxmlhistoryldialog.h"
#include "modules/specialized/scxml/dialogs/scxmlifdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlelseifdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlforeachdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlraisedialog.h"
#include "modules/specialized/scxml/dialogs/scxmllogdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlassigndialog.h"
#include "modules/specialized/scxml/dialogs/scxmlcontentdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlparamdialog.h"
#include "modules/specialized/scxml/dialogs/scxmldatadialog.h"
#include "modules/specialized/scxml/dialogs/scxmlscriptdialog.h"
#include "modules/specialized/scxml/dialogs/scxmlsenddialog.h"
#include "modules/specialized/scxml/dialogs/scxmlinvokedialog.h"
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

//------------------------------------------------------------------

SCXMLinitialToken::SCXMLinitialToken()
{
}

SCXMLinitialToken::~SCXMLinitialToken()
{
}

bool SCXMLinitialToken::editToken(QWidget * /*window*/, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                  Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_initial);
    }
    return true;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(id, SCXMLfinalToken);

SCXMLfinalToken::SCXMLfinalToken()
{
}

SCXMLfinalToken::~SCXMLfinalToken()
{
}

bool SCXMLfinalToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLFinalDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SCXMLonentryToken::SCXMLonentryToken()
{
}

SCXMLonentryToken::~SCXMLonentryToken()
{
}

bool SCXMLonentryToken::editToken(QWidget * /*window*/, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                  Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_onentry);
    }
    return true;
}

//------------------------------------------------------------------

SCXMLonexitToken::SCXMLonexitToken()
{
}

SCXMLonexitToken::~SCXMLonexitToken()
{
}

bool SCXMLonexitToken::editToken(QWidget * /*window*/, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_onexit);
    }
    return true;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(id, SCXMLhistoryToken);
SC_IMPL_ATTR_TK(type, SCXMLhistoryToken);

SCXMLhistoryToken::SCXMLhistoryToken()
{
}

SCXMLhistoryToken::~SCXMLhistoryToken()
{
}

bool SCXMLhistoryToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                  Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLHistoryDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(event, SCXMLraiseToken);

SCXMLraiseToken::SCXMLraiseToken()
{
}

SCXMLraiseToken::~SCXMLraiseToken()
{
}

bool SCXMLraiseToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLRaiseDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(cond, SCXMLifToken);

SCXMLifToken::SCXMLifToken()
{
}

SCXMLifToken::~SCXMLifToken()
{
}

bool SCXMLifToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                             Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLIfDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(cond, SCXMLelseifToken);

SCXMLelseifToken::SCXMLelseifToken()
{
}

SCXMLelseifToken::~SCXMLelseifToken()
{
}

bool SCXMLelseifToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    SCXMLElseIfDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SCXMLelseToken::SCXMLelseToken()
{
}

SCXMLelseToken::~SCXMLelseToken()
{
}

bool SCXMLelseToken::editToken(QWidget *, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                               Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_else);
    }
    return true;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(array, SCXMLforeachToken);
SC_IMPL_ATTR_TK(item, SCXMLforeachToken);
SC_IMPL_ATTR_TK(index, SCXMLforeachToken);

SCXMLforeachToken::SCXMLforeachToken()
{
}

SCXMLforeachToken::~SCXMLforeachToken()
{
}

bool SCXMLforeachToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                  Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLForeachDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(label, SCXMLlogToken);
SC_IMPL_ATTR_TK(expr, SCXMLlogToken);

SCXMLlogToken::SCXMLlogToken()
{
}

SCXMLlogToken::~SCXMLlogToken()
{
}

bool SCXMLlogToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                              Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    SCXMLLogDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SCXMLdatamodelToken::SCXMLdatamodelToken()
{
}

SCXMLdatamodelToken::~SCXMLdatamodelToken()
{
}

bool SCXMLdatamodelToken::editToken(QWidget *, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                    Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_datamodel);
    }
    return true;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(id, SCXMLdataToken);
SC_IMPL_ATTR_TK(src, SCXMLdataToken);
SC_IMPL_ATTR_TK(expr, SCXMLdataToken);

SCXMLdataToken::SCXMLdataToken()
{
}

SCXMLdataToken::~SCXMLdataToken()
{
}

bool SCXMLdataToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                               Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLDataDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(location, SCXMLassignToken);
SC_IMPL_ATTR_TK(expr, SCXMLassignToken);

SCXMLassignToken::SCXMLassignToken()
{
}

SCXMLassignToken::~SCXMLassignToken()
{
}

bool SCXMLassignToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLAssignDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------


SCXMLdonedataToken::SCXMLdonedataToken()
{
}

SCXMLdonedataToken::~SCXMLdonedataToken()
{
}

bool SCXMLdonedataToken::editToken(QWidget *, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                   Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_donedata);
    }
    return true;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(expr, SCXMLcontentToken);

SCXMLcontentToken::SCXMLcontentToken()
{
}

SCXMLcontentToken::~SCXMLcontentToken()
{
}

bool SCXMLcontentToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                  Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLContentDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(expr, SCXMLparamToken);
SC_IMPL_ATTR_TK(name, SCXMLparamToken);
SC_IMPL_ATTR_TK(location, SCXMLparamToken);

SCXMLparamToken::SCXMLparamToken()
{
}

SCXMLparamToken::~SCXMLparamToken()
{
}

bool SCXMLparamToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLParamDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(src, SCXMLscriptToken);

SCXMLscriptToken::SCXMLscriptToken()
{
}

SCXMLscriptToken::~SCXMLscriptToken()
{
}

bool SCXMLscriptToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLScriptDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------

SCXMLcancelToken::SCXMLcancelToken()
{
}

SCXMLcancelToken::~SCXMLcancelToken()
{
}

bool SCXMLcancelToken::editToken(QWidget *, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_cancel);
    }
    return true;
}

//------------------------------------------------------------------

SCXMLfinalizeToken::SCXMLfinalizeToken()
{
}

SCXMLfinalizeToken::~SCXMLfinalizeToken()
{
}

bool SCXMLfinalizeToken::editToken(QWidget *, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                   Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    if(isInsertOrEdit) {
        BaseSCXMLEditData editData(info, regola, isInsertOrEdit, isInsertOrAppend,
                                   toModifyElement, selectedElement, parentElement);
        editData.assignTag(SCXMLToken::Tag_finalize);
    }
    return true;
}

//------------------------------------------------------------------

SC_IMPL_ATTR_TK(type, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(typeexpr, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(src, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(srcexpr, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(id, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(idlocation, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(namelist, SCXMLinvokeToken);
SC_IMPL_ATTR_TK(autoforward, SCXMLinvokeToken);

SCXMLinvokeToken::SCXMLinvokeToken()
{
}

SCXMLinvokeToken::~SCXMLinvokeToken()
{
}

bool SCXMLinvokeToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                                 Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLInvokeDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------


SC_IMPL_ATTR_TK(event, SCXMLsendToken);
SC_IMPL_ATTR_TK(eventexpr, SCXMLsendToken);
SC_IMPL_ATTR_TK(target, SCXMLsendToken);
SC_IMPL_ATTR_TK(targetexpr, SCXMLsendToken);
SC_IMPL_ATTR_TK(type, SCXMLsendToken);
SC_IMPL_ATTR_TK(typeexpr, SCXMLsendToken);
SC_IMPL_ATTR_TK(id, SCXMLsendToken);
SC_IMPL_ATTR_TK(idlocation, SCXMLsendToken);
SC_IMPL_ATTR_TK(delay, SCXMLsendToken);
SC_IMPL_ATTR_TK(delayexpr, SCXMLsendToken);
SC_IMPL_ATTR_TK(namelist, SCXMLsendToken);

SCXMLsendToken::SCXMLsendToken()
{
}

SCXMLsendToken::~SCXMLsendToken()
{
}

bool SCXMLsendToken::editToken(QWidget *window, SCXMLInfo *info, Regola * regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                               Element *toModifyElement, Element *selectedElement, Element *parentElement)
{
    Utils::TODO_THIS_RELEASE("fare");
    SCXMLSendDialog dialog(window, info, regola, isInsertOrEdit, isInsertOrAppend, toModifyElement, selectedElement, parentElement);
    if(dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------
