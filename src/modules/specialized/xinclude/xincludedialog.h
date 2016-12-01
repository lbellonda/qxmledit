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

#ifndef XINCLUDEDIALOG_H
#define XINCLUDEDIALOG_H

#include "xmlEdit.h"

class Element ;

namespace Ui
{
class XIncludeDialog;
}

class XIncludeAttr
{
public:
    QString href, xpointer, fragid, parse, setxmlid, accept, language, encoding ;
    bool useSetXMLid;
    bool declareLocalA;
    XIncludeAttr();
    ~XIncludeAttr();
};

class XIncludeDialog : public QDialog
{
    Q_OBJECT
    Element *_element ;
    XIncludeAttr _attr;

    bool checkValues();
    void setAttributes();
    void setValuesToElement();
    void setAttributesToUI();
    void readAttributesFromElement();
    void readAttributesFromUI();
    void postSetupUI();
    bool check20To7E(const QString &in);
    bool isXMLProcessing();
    void setAttr(const QString &attr, const QString &value);
    void setAttrByBool(const QString &attr, const QString &value, const bool useIt);

public:
    explicit XIncludeDialog(QWidget *parent, Element *element);
    ~XIncludeDialog();

    virtual void accept();

private:
    Ui::XIncludeDialog *ui;
};

#endif // XINCLUDEDIALOG_H
