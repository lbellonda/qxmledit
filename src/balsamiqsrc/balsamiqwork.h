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

#ifndef BALSAMIQWORK_H
#define BALSAMIQWORK_H

#include <QDomDocument>
#include <QWidget>
#include <QMap>
#include <QProgressDialog>
#include "balsamiqcontrol.h"
#include "balsamiqproxy.h"

class BalsamiqWork : public QObject
{
    Q_OBJECT

    QDomDocument _inputData;
    QMap<QString, BalsamiqControl*> _controls;
    BalsamiqControl* _defaultControl;
    bool _isError ;
    QString _errorReason ;
    BalsamiqProxy *_rootControl;
    QString _outputFilePath;

    void resetControls();
    bool registerControl(BalsamiqControl *ctrl);
    bool registerControls();
    BalsamiqControl *getControl(const QString &type);
    bool scanData(const QDomNode &node, BalsamiqProxy *currentControl);
    bool scanElement(const QDomElement &element, BalsamiqProxy *currentControl);
    bool read(const QString &filePath);
    bool translate(BalsamiqOpContext &context, const QString &file, const bool isOverwrite);
    bool doEmitData(BalsamiqOpContext &context, BalsamiqProxy *proxy, QDomNode &node);
    bool calculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node);
    bool emitGlobalsData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node);
    bool emitControlsData(BalsamiqOpContext &context, BalsamiqProxy *proxy, BalsamiqProxy *parent, QDomNode &node);
    bool test(QWidget *parent);

    bool doApplication(const QDomElement& element);
    bool doControls(const QDomElement& element, BalsamiqProxy *currentProxy);
    bool doControl(const QDomElement &element, BalsamiqProxy *currentProxy);
    void doControlProperties(const QDomElement &father, BalsamiqProxy *currentProxy);

    void setError(const QString &mesage);
    void clearError();
    void setErrorContext(BalsamiqOpContext &context, BalsamiqProxy *proxy, const QString &phase);
    void setErrorContextCalculateData(BalsamiqOpContext &context, BalsamiqProxy *proxy);
    void setErrorContextGenerateGlobals(BalsamiqOpContext &context, BalsamiqProxy *proxy);
    void setErrorContextGenerateControls(BalsamiqOpContext &context, BalsamiqProxy *proxy);

    bool setStep(QProgressDialog &progressDialog, BalsamiqOpContext &context, const int step, const QString &message);

    bool work(QWidget *parent, const QStringList &files, const QString &outputDir, const bool isOverwrite);
    bool doSingleWork(BalsamiqOpContext &context, const QString &fileIn, const QString &fileOut, const bool isOverwrite);

public:
    BalsamiqWork();
    ~BalsamiqWork();

    bool exec(QWidget *parent, const bool isOverwriteFiles, const QStringList files, const QString &outputDir);
    bool execTest(QWidget *parent);
};

#endif // BALSAMIQWORK_H
