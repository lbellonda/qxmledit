/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2011-2018 by Luca Bellonda and individual contributors  *
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
#ifndef WIDGETUTILITY_H
#define WIDGETUTILITY_H

#include "elementeditor.h"
#include "xsdeditor/xproperties.h"
#include "ui_elementeditor.h"

#include "utils.h"
#include <QList>
#include <QSet>

class WidgetUtility : public QObject
{
    Q_OBJECT

    static QStringList xsdTypesList;
    static QSet<QString> xsdTypesSet;
    static bool xsdDataLoaded ;

public:
    static void LoadComboBoxFinalType(QComboBox *combo);
    static void LoadComboBoxElementType(QComboBox *combo);
    static bool getComboBoxFinalTypeSelected(QComboBox *combo, XSchemaObject::EFinalType &result);
    static bool getComboBoxIntSelected(QComboBox *combo, int &result);
    static bool selectItemWithData(QComboBox *combo, const int dataToSelect);
    static int getComboItemByInt(QComboBox *combo, const int dataToSelect);
    static QStringList getStdTypes();
    static QStringList getStdTypes(const QString &prefix);
    static void loadComboTypes(QComboBox *combo, QStringList &customTypes);
    static XEnums::XBool getCheckState(QCheckBox *box);
    static void setCheckState(const XEnums::XBool value, QCheckBox *box);

};

#endif // WIDGETUTILITY_H
