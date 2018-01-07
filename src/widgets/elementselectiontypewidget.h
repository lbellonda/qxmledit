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


#ifndef ELEMENTSELECTIONTYPEWIDGET_H
#define ELEMENTSELECTIONTYPEWIDGET_H

#include <QWidget>
#include "element.h"
#include "libQXmlEdit_global.h"

namespace Ui
{
class ElementSelectionTypeWidget;
}

class LIBQXMLEDITSHARED_EXPORT ElementSelectionTypeWidget : public QWidget
{
    Q_OBJECT

    TargetSelection::Type _selectionType;
    bool _fireEvents;

    void enableUI();
    void evaluateState();
    bool setSelectionTypeInner(const TargetSelection::Type selectionType);
public:
    explicit ElementSelectionTypeWidget(QWidget *parent = 0);
    virtual ~ElementSelectionTypeWidget();

    TargetSelection::Type selectionType() const;
    void setSelectionType(const TargetSelection::Type selectionType);

signals:
    void selectionTypeChanged(TargetSelection::Type selectionType);
private:
    Ui::ElementSelectionTypeWidget *ui;
private slots:
    void on_group_buttonClicked(QAbstractButton * button);
    void on_cbChildren_clicked();
};

#endif // ELEMENTSELECTIONTYPEWIDGET_H
