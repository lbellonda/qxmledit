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


#ifndef XSDENUMDIALOG_H
#define XSDENUMDIALOG_H

#include "xmlEdit.h"
#include "xsdeditor/xschema.h"
#include "modules/xsd/xsdhelper.h"

namespace Ui
{
class XSDEnumDialog;
}

class XSDEnumDialog : public QDialog, XSDAnnotationEditProvider
{
    Q_OBJECT

    QList<XSDFacet*> _facets;
    XSDAnnotationEditProvider *_XSDAnnotationEditProvider;

    void displayData();
    void insertRow(XSDFacet *facet, const bool selectItem = false);
    void updateRow(XSDFacet * facet, const int row, const bool selectItem);
    void setRow(XSDFacet * facet, const int row, const bool isModified, const bool selectItem);
    void selectionChanged();
    QTableWidgetItem *createItem(XSDFacet *facet, const QString &text);
    int currentSelectedIndex();
    XSDFacet *selectedFacet();
    XSDFacet *getUserData(QTableWidgetItem *item);
    void editAnnotation(XSDFacet *facet);

    XSDAnnotationEditProvider *XSDAnnotationEditProviderObject();
    void autoDelete();
    virtual XSDAnnotationEditor *newEditor(QWidget *window);

public:
    explicit XSDEnumDialog(QWidget *parent, QList<XSDFacet*> inputFacets);
    virtual ~XSDEnumDialog();

    QList<XSDFacet*> result();

    void addFacet(XSDFacet *newFacet);
    void updateFacet(XSDFacet *facet);
    void deleteFacetAt(const int index);
    XSDFacet *facetAt(const int index);
    void moveDown(const int index);
    void moveUp(const int index);
    void setXSDAnnotationEditProviderObject(XSDAnnotationEditProvider *newProvider);

private:
    Ui::XSDEnumDialog *ui;
private slots:
    void on_cmdAdd_clicked();
    void on_cmdMod_clicked();
    void on_cmdDel_clicked();
    void on_cmdMoveUp_clicked();
    void on_cmdMoveDown_clicked();
    void on_cmdAnnot_clicked();
    void on_facets_currentItemChanged(QTableWidgetItem * /*current*/, QTableWidgetItem * /*previous*/);
    void on_facets_itemDoubleClicked(QTableWidgetItem * item);
};

#endif // XSDENUMDIALOG_H
