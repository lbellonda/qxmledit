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

#ifndef SCXMLTOKEN_H
#define SCXMLTOKEN_H

#include "xmlEdit.h"
class SCXMLInfo;
class SCXMLTokenChild;
class Regola;
class Element;

#define DECL_SCXML_TAG(x) static const QString Tag_ ## x
#define IMPL_SCXML_TAG(x) const QString SCXMLToken::Tag_ ## x = #x
#define SC_DECL_ATTR_TK(a)  static const QString A_ ## a
#define SC_IMPL_ATTR_TK(a, cla)  const QString cla::A_ ## a = # a
//--

class SCXMLToken;

class SCXMLGroup
{
public:
    QString _name;
    QList<SCXMLTokenChild*>_children;
    SCXMLGroup(const QString &newName);
    ~SCXMLGroup();
    void addChild(SCXMLTokenChild* newToken);
};

class SCXMLToken
{
public:
    DECL_SCXML_TAG(scxml);
    DECL_SCXML_TAG(state);
    DECL_SCXML_TAG(parallel);
    DECL_SCXML_TAG(transition);
    DECL_SCXML_TAG(initial);
    DECL_SCXML_TAG(final);
    DECL_SCXML_TAG(onentry);
    DECL_SCXML_TAG(onexit);
    DECL_SCXML_TAG(history);
    DECL_SCXML_TAG(raise);
    DECL_SCXML_TAG(if);
    DECL_SCXML_TAG(else);
    DECL_SCXML_TAG(elseif);
    DECL_SCXML_TAG(foreach);
    DECL_SCXML_TAG(log);
    DECL_SCXML_TAG(datamodel);
    DECL_SCXML_TAG(data);
    DECL_SCXML_TAG(assign);
    DECL_SCXML_TAG(donedata);
    DECL_SCXML_TAG(content);
    DECL_SCXML_TAG(param);
    DECL_SCXML_TAG(script);
    DECL_SCXML_TAG(cancel);
    DECL_SCXML_TAG(finalize);
    DECL_SCXML_TAG(send);
    DECL_SCXML_TAG(invoke);
private:
    QString _name;
    QString _description;
    QList<SCXMLTokenChild*> _children;
public:
    SCXMLToken();
    virtual ~SCXMLToken();

    QString name() const;
    void setName(const QString &name);
    QString description() const;
    void setDescription(const QString &description);
    QList<SCXMLTokenChild *> children() const;
    void addChild(SCXMLTokenChild *child);
    /**
     * @brief editToken
     * @param window: window for dialog
     * @param regola:
     * @param isInsertOrEdit: true if inserting or appending a new element, else edit it.
     * @param isInsertOrAppend: true if insert and inserting a child, true if insert and inserting a sibling,
     * @param toModifyElement: the element to modify in the edit panel
     * @param selectedElement: the existing element that will be edited, if any
     * @param parentElement: the parent element of the existing element, if existing
     * @return
     */
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) = 0;
    void warningNoEdit(QWidget *window);
};

class SCXMLRootToken : public SCXMLToken
{
public:
    SCXMLRootToken();
    virtual ~SCXMLRootToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement);
};

class SCXMLGenericToken : public SCXMLToken
{
public:
    SCXMLGenericToken();
    virtual ~SCXMLGenericToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement);
};

class SCXMLscxmlToken : public SCXMLToken
{
public:
    SCXMLscxmlToken();
    virtual ~SCXMLscxmlToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(initial);
    SC_DECL_ATTR_TK(name);
    SC_DECL_ATTR_TK(xmlns);
    SC_DECL_ATTR_TK(version);
    SC_DECL_ATTR_TK(datamodel);
    SC_DECL_ATTR_TK(binding);
};

class SCXMLstateToken : public SCXMLToken
{
public:
    SCXMLstateToken();
    virtual ~SCXMLstateToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(initial);
    SC_DECL_ATTR_TK(id);
};

class SCXMLparallelToken : public SCXMLToken
{
public:
    SCXMLparallelToken();
    virtual ~SCXMLparallelToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(id);
};

class SCXMLtransitionToken : public SCXMLToken
{
public:
    SCXMLtransitionToken();
    virtual ~SCXMLtransitionToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(event);
    SC_DECL_ATTR_TK(cond);
    SC_DECL_ATTR_TK(target);
    SC_DECL_ATTR_TK(type);
};

class SCXMLinitialToken : public SCXMLToken
{
public:
    SCXMLinitialToken();
    virtual ~SCXMLinitialToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLfinalToken : public SCXMLToken
{
public:
    SCXMLfinalToken();
    virtual ~SCXMLfinalToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(id);
};

class SCXMLonentryToken : public SCXMLToken
{
public:
    SCXMLonentryToken();
    virtual ~SCXMLonentryToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLonexitToken : public SCXMLToken
{
public:
    SCXMLonexitToken();
    virtual ~SCXMLonexitToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLhistoryToken : public SCXMLToken
{
public:
    SCXMLhistoryToken();
    virtual ~SCXMLhistoryToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(id);
    SC_DECL_ATTR_TK(type);
};

class SCXMLraiseToken : public SCXMLToken
{
public:
    SCXMLraiseToken();
    virtual ~SCXMLraiseToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(event);
};

class SCXMLifToken : public SCXMLToken
{
public:
    SCXMLifToken();
    virtual ~SCXMLifToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(cond);
};

class SCXMLelseifToken : public SCXMLToken
{
public:
    SCXMLelseifToken();
    virtual ~SCXMLelseifToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(cond);
};

class SCXMLelseToken : public SCXMLToken
{
public:
    SCXMLelseToken();
    virtual ~SCXMLelseToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLforeachToken : public SCXMLToken
{
public:
    SCXMLforeachToken();
    virtual ~SCXMLforeachToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(array);
    SC_DECL_ATTR_TK(item);
    SC_DECL_ATTR_TK(index);
};

class SCXMLlogToken : public SCXMLToken
{
public:
    SCXMLlogToken();
    virtual ~SCXMLlogToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(label);
    SC_DECL_ATTR_TK(expr);
};

class SCXMLdatamodelToken : public SCXMLToken
{
public:
    SCXMLdatamodelToken();
    virtual ~SCXMLdatamodelToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLdataToken : public SCXMLToken
{
public:
    SCXMLdataToken();
    virtual ~SCXMLdataToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(id);
    SC_DECL_ATTR_TK(src);
    SC_DECL_ATTR_TK(expr);
};

class SCXMLassignToken : public SCXMLToken
{
public:
    SCXMLassignToken();
    virtual ~SCXMLassignToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(location);
    SC_DECL_ATTR_TK(expr);
};

class SCXMLdonedataToken : public SCXMLToken
{
public:
    SCXMLdonedataToken();
    virtual ~SCXMLdonedataToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLcontentToken : public SCXMLToken
{
public:
    SCXMLcontentToken();
    virtual ~SCXMLcontentToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(expr);
};

class SCXMLparamToken : public SCXMLToken
{
public:
    SCXMLparamToken();
    virtual ~SCXMLparamToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(expr);
    SC_DECL_ATTR_TK(name);
    SC_DECL_ATTR_TK(location);
};

class SCXMLscriptToken : public SCXMLToken
{
public:
    SCXMLscriptToken();
    virtual ~SCXMLscriptToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(src);
};

class SCXMLcancelToken : public SCXMLToken
{
public:
    SCXMLcancelToken();
    virtual ~SCXMLcancelToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLfinalizeToken : public SCXMLToken
{
public:
    SCXMLfinalizeToken();
    virtual ~SCXMLfinalizeToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
};

class SCXMLinvokeToken : public SCXMLToken
{
public:
    SCXMLinvokeToken();
    virtual ~SCXMLinvokeToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(type);
    SC_DECL_ATTR_TK(typeexpr);
    SC_DECL_ATTR_TK(src);
    SC_DECL_ATTR_TK(srcexpr);
    SC_DECL_ATTR_TK(id);
    SC_DECL_ATTR_TK(idlocation);
    SC_DECL_ATTR_TK(namelist);
    SC_DECL_ATTR_TK(autoforward);

};

class SCXMLsendToken : public SCXMLToken
{
public:
    SCXMLsendToken();
    virtual ~SCXMLsendToken();
    virtual bool editToken(QWidget *window, SCXMLInfo *info, Regola *regola, const bool isInsertOrEdit, const bool isInsertOrAppend,
                           Element *toModifyElement, Element *selectedElement, Element *parentElement) ;
    SC_DECL_ATTR_TK(event);
    SC_DECL_ATTR_TK(eventexpr);
    SC_DECL_ATTR_TK(target);
    SC_DECL_ATTR_TK(targetexpr);
    SC_DECL_ATTR_TK(type);
    SC_DECL_ATTR_TK(typeexpr);
    SC_DECL_ATTR_TK(id);
    SC_DECL_ATTR_TK(idlocation);
    SC_DECL_ATTR_TK(delay);
    SC_DECL_ATTR_TK(delayexpr);
    SC_DECL_ATTR_TK(namelist);

};

//------------------------------------------------------------------------------------------

class SCXMLTokenChild
{
    QString _name;
    QString _description;
    int _min;
    int _max;
    SCXMLToken *_parent;
public:
    SCXMLTokenChild();
    ~SCXMLTokenChild();

    QString name() const;
    void setName(const QString &name);
    QString description() const;
    void setDescription(const QString &description);
    int min() const;
    void setMin(int min);
    int max() const;
    void setMax(int max);
    SCXMLToken *parent() const;
    void setParent(SCXMLToken *parent);

    static const int Unbounded = -1 ;

};

#endif // SCXMLTOKEN_H
