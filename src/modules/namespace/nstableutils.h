/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2015 by Luca Bellonda and individual contributors       *
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


#ifndef NSTABLEUTILS_H
#define NSTABLEUTILS_H

#include "xmlEdit.h"

class NamespaceResult;

class NsTableUtils
{
public:

    enum ENsColumns {
        NsPrefix,
        NsUri,
        NsDescription
    };

    NsTableUtils();
    ~NsTableUtils();

    void setupTable(QTableWidget *widget);
    void insNsItem(QTableWidget *table, const int row, const int column, const QString &text, void *data);
    int insNsInList(QTableWidget *table, const int desiredRow, const QString &prefix, const QString &uri, const QString &description, void *data);
    void *userData(QTableWidget *table, const int row, const int column);
    template<typename T>
    inline T* userDataType(QTableWidget *table, const int row, const int column) {
        QTableWidgetItem *item = table->item(row, column);
        if(NULL != item) {
            QVariant data = item->data(Qt::UserRole);
            T* res = (T*)data.value<void*>();
            return res ;
        }
        return NULL ;
    }
    bool valueForRow(QTableWidget *table, const int row, NamespaceResult *setValues);
};

#endif // NSTABLEUTILS_H
