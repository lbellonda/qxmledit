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
