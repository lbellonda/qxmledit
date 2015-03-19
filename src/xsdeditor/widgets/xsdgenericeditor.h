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

#ifndef XSDGENERICEDITOR_H
#define XSDGENERICEDITOR_H

#include <QWidget>
#include <QMap>

// generic editor/inspector

namespace Ui
{
class XSDGenericEditor;
}

class XMethInfo;
class XDescriptor1;

class XSDGenericEditor : public QWidget
{
    Q_OBJECT
    QObject *_target;
    QMap<QWidget*, XMethInfo*> _widgets;
    QMap<QString, QWidget*> _properties;

public:
    explicit XSDGenericEditor(QWidget *parent = 0);
    ~XSDGenericEditor();

    void setTarget(QObject *newTarget, XDescriptor1 &descriptor);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::XSDGenericEditor *ui;

private slots:
    void textPropertyChanged(const char*propertyName);
    void on_field_editTextChanged(const QString & text);
};

#endif // XSDGENERICEDITOR_H
