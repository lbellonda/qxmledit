/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012 by Luca Bellonda and individual contributors       *
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


#ifndef XCOMPARESUMMARY_H
#define XCOMPARESUMMARY_H

#include <QWidget>
#include <QTreeWidgetItem>

class XSchemaObject;

namespace Ui
{
class XCompareSummary;
}

class XCompareSummary : public QWidget
{
    Q_OBJECT

public:
    explicit XCompareSummary(QWidget *parent = 0);
    ~XCompareSummary();

    void setReference(const QString &path);
    void setTarget(const QString &path);

    void setSummaryData(const QList<XSchemaObject*> &added, const QList<XSchemaObject*> &modified, const QList<XSchemaObject*> &deleted);
private:
    Ui::XCompareSummary *ui;

    void loadList(const QString & title, const QList<XSchemaObject*> &items, const QColor bkColor);
    void setupData();
private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem * item, int column);
signals:
    void objectDoubleClicked(XSchemaObject *target);
};

#endif // XCOMPARESUMMARY_H
