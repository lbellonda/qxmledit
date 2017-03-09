/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2017 by Luca Bellonda and individual contributors       *
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

#ifndef COMBOUTILS_H
#define COMBOUTILS_H

#include "xmlEdit.h"

class XmlEditWidget;
class QXmlEditData;

class ComboUtils
{
public:
    ComboUtils();
    ~ComboUtils();

    enum eType {
        TypeBrowse,
        TypeEditor,
        TypeFile,
        TypeOutputEditor
    };

    class ComboItem
    {
    public:
        QString text;
        int code;
        void *data;
        QString dataString;

        ComboItem()
        {
            code = 0 ;
            data = NULL ;
        }
        ComboItem(const QString &name, const int newCode)
        {
            text = name ;
            code = newCode ;
            data = NULL ;
        }
        ~ComboItem() {}
    };
    void setupItemsForFile(QXmlEditData *data, QList<XmlEditWidget*> editors, const bool useEditors, const bool isSave);
    void loadButtonMenu(QToolButton *button, QObject *target, const char *method);
    static ComboUtils::ComboItem *actionData(QAction *action);
    static QString titleForEditor(XmlEditWidget* editor);
private:
    QList<ComboUtils::ComboItem*> _items;

    void reset();

};

#endif // COMBOUTILS_H
