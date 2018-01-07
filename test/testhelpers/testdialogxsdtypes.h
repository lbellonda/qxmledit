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


#ifndef TESTDIALOGXSDTYPES_H
#define TESTDIALOGXSDTYPES_H

#include "modules/xsd/xsdtypedialog.h"

class TestDialogXSDTypesResult {
public:
   QString name;
   //------
   bool aref;
   QString aname;
   bool asimple;
   bool acomplex;
   //------
   bool bcontentenabled;
   bool bsimplec;
   bool bcomplexc;
   bool bnone;
   //------
   bool csimpleenabled;
   QString csimplename;
   bool crestr;
   bool clist;
   bool cunion;
   //------
   bool dcomplexenabled;
   QString dcomplexename;
   bool dnone;
   bool dext;
   bool drestr;
   bool dall;
   bool dgroup;
   bool dchoice;
   bool dseq;

};


class TestDialogXSDTypes : public XSDTypeDialog
{
    Q_OBJECT
    QString _cause;
    TestDialogXSDTypesResult *_exp;
public:
    explicit TestDialogXSDTypes(const bool isInsert, const XSDOperationParameters::EObjectType objectType, const QString &startName, Element *selection, QWidget *parent = 0);
    ~TestDialogXSDTypes();

    bool checkSettings(const TestDialogXSDTypesResult *expected);

    QString cause()
    {
        return _cause;
    }
};

#endif // TESTDIALOGXSDTYPES_H
