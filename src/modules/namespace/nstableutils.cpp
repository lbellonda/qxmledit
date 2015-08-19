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

#include "nstableutils.h"
#include "modules/namespace/choosenamespacedialog.h"

NsTableUtils::NsTableUtils()
{
}

NsTableUtils::~NsTableUtils()
{
}

void NsTableUtils::setupTable(QTableWidget *widget)
{
    widget->setColumnCount(3);
    QStringList labels;
    labels << QObject::tr("Prefix") << QObject::tr("Uri") << QObject::tr("Description") ;
    widget->setHorizontalHeaderLabels(labels);
}

void NsTableUtils::insNsItem(QTableWidget *table, const int row, const int column, const QString &text, void *data)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setFlags(item->flags()& ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
    item->setData(Qt::UserRole, qVariantFromValue(data));
    item->setToolTip(text);
    table->setItem(row, column, item);
}

bool NsTableUtils::valueForRow(QTableWidget *table, const int row, NamespaceResult *setValues)
{
    if(row >= 0) {
        setValues->description = table->item(row, NsTableUtils::NsDescription)->text().trimmed();
        setValues->uri = table->item(row, NsTableUtils::NsUri)->text().trimmed();
        setValues->prefix = table->item(row, NsTableUtils::NsPrefix)->text().trimmed();
        return true;
    }
    return false;
}

int NsTableUtils::insNsInList(QTableWidget *table, const int desiredRow, const QString &prefix, const QString &uri, const QString &description, void *data)
{
    int row = desiredRow;
    if(row >= 0) {
        if(table->rowCount() > row) {
            table->removeRow(row);
        }
        table->insertRow(row);
    } else {
        row = table->rowCount();
        table->setRowCount(row + 1);
    }
    insNsItem(table, row, NsPrefix, prefix, data);
    insNsItem(table, row, NsUri, uri, data);
    insNsItem(table, row, NsDescription, description, data);

    return row ;
}

void *NsTableUtils::userData(QTableWidget *table, const int row, const int column)
{
    QTableWidgetItem *item = table->item(row, column);
    if(NULL != item) {
        QVariant data = item->data(Qt::UserRole);
        void * res = data.value<void*>();
        return res ;
    }
    return NULL ;
}

/* this to be set inline?
template<typename T>
T *NsTableUtils::userDataType(QTableWidget *table, const int row, const int column)
{
    QTableWidgetItem *item = table->item(row, column);
    if(NULL != item) {
        QVariant data = item->data(Qt::UserRole);
        T* res = (T*)data.value<void*>();
        return res ;
    }
    return NULL ;
}
*/
