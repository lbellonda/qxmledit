/**************************************************************************
 *  This file is part of QXmlEdit                                         *
 *  Copyright (C) 2012-2018 by Luca Bellonda and individual contributors  *
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


#ifndef XSDCOMPARE_H
#define XSDCOMPARE_H

#include "regola.h"
#include "xsdeditor/xsdcontroller.h"
#include "xsdeditor/xschema.h"

class XSDCompareData;
class XSchemaObject;
class XSDSchema ;
class XSDCompareResult;
class QXmlEditData;

class XSDCompare : public QObject, public IXSDController
{
    Q_OBJECT

    QString _targetXSDFileName;
    Regola *_regola;
    XSDWindow *_window;

    QString getTargetFile(QWidget *parent, const QString &folderPath);

    friend class TestXSDDiff;

public:
    explicit XSDCompare();
    virtual ~XSDCompare();

    bool compare(QWidget *parent, QXmlEditData *appData, Regola *data, const QString &folderPath);

    void setXSDWindow(XSDWindow *newWindow);
    XSDWindow *getXSDWindow();
    void evaluate(XSDWindow *window, XSDCompareOptions &options, const bool isSwap);

private:
    XSDCompareResult *compareSchema(XSDCompareResult *result, XSDSchema *reference, XSDSchema *target, XSDCompareOptions &compareOptions);
    XSDSchema *loadXSDFromString(const QString &dataToLoad);
    XSDSchema *loadXSDFromFile(const QString &fileName);
    XSDCompareResult *innerCompare(const QString &referenceString, const QString &targetFileName, XSDCompareOptions &options, const bool isSwap);
    void setUIData(XSDWindow *window, XSDSchema *schema, const QString &referencePath, const QString &targetPath, const bool isSwap);
    void setSummary(XSDWindow *window, XSDSchema *schema);
};


#endif // XSDCOMPARE_H
